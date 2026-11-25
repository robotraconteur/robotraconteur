// Regression test: a zero-length frame with the mask bit set returned before reading its 4 mask bytes,
// so they were parsed as the next frame header.

#include <gtest/gtest.h>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_array.hpp>

#include "RobotRaconteur/TcpTransport.h"
#include "websocket_stream.hpp"
#include "websocket_test_compat.h"

using boost::asio::ip::tcp;

namespace
{
typedef RobotRaconteur::detail::websocket_stream<tcp::socket&> ws_t;

// Connected loopback socket pair: raw is the test's raw peer, ws is wrapped by websocket_stream
struct socket_pair
{
    RR_BOOST_ASIO_IO_CONTEXT ioc;
    tcp::socket raw;
    tcp::socket ws;
    socket_pair() : raw(ioc), ws(ioc)
    {
        tcp::acceptor acc(ioc, tcp::endpoint(boost::asio::ip::address_v4::loopback(), 0));
        raw.connect(acc.local_endpoint());
        acc.accept(ws);
        raw.set_option(tcp::no_delay(true));
        ws.set_option(tcp::no_delay(true));
    }
};

using websocket_test::run_until;

template <typename Stream>
struct ws_reader
{
    Stream& ws;
    size_t want;
    boost::shared_array<uint8_t> buf;
    std::string data;
    boost::system::error_code ec;
    bool done;
    int zero_reads;
    ws_reader(Stream& ws, size_t want) : ws(ws), want(want), buf(new uint8_t[1024]), done(false), zero_reads(0) {}
    void start()
    {
        ws.async_read_some(
            boost::asio::buffer(buf.get(), 1024),
            boost::bind(&ws_reader::handler, this, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
    }
    void handler(const boost::system::error_code& e, size_t n)
    {
        if (done)
            return;
        if (e)
        {
            ec = e;
            done = true;
            return;
        }
        data.append(reinterpret_cast<char*>(buf.get()), n);
        if (n == 0 && ++zero_reads > 10)
        {
            done = true;
            return;
        }
        if (data.size() >= want)
        {
            done = true;
            return;
        }
        start();
    }
};
} // namespace

TEST(WebSocketStream, MaskedEmptyFrame)
{
    socket_pair p;
    ws_t ws(p.ws);

    const uint8_t frames[] = {// empty masked binary frame, mask bytes look like a text frame header
                              0x82, 0x80, 0x81, 0x01, 0x00, 0x00,
                              // masked binary frame "hello"
                              0x82, 0x85, 0x01, 0x02, 0x03, 0x04, 'h' ^ 0x01, 'e' ^ 0x02, 'l' ^ 0x03, 'l' ^ 0x04,
                              'o' ^ 0x01};
    boost::asio::write(p.raw, boost::asio::buffer(frames));

    ws_reader<ws_t> r(ws, 5);
    r.start();
    run_until(p.ioc, r.done);
    ASSERT_TRUE(r.done);
    ASSERT_FALSE(r.ec) << r.ec.message();
    EXPECT_EQ(r.data, "hello");
}

TEST(WebSocketStream, MaskedEmptyPing)
{
    socket_pair p;
    ws_t ws(p.ws);

    const uint8_t frames[] = {// empty masked ping
                              0x89, 0x80, 0x81, 0x01, 0x00, 0x00,
                              // unmasked binary frame "ok"
                              0x82, 0x02, 'o', 'k'};
    boost::asio::write(p.raw, boost::asio::buffer(frames));

    ws_reader<ws_t> r(ws, 2);
    r.start();
    run_until(p.ioc, r.done);
    ASSERT_TRUE(r.done);
    // Before the fix, the mask bytes 0x81 0x01 were parsed as a text frame header (protocol_error)
    EXPECT_NE(r.ec, boost::system::errc::protocol_error) << r.ec.message();
}
