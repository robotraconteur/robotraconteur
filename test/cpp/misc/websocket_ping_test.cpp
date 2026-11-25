// Regression test: a ping control frame closed the connection (frame state was reset before the opcode
// switch) and a ping split across reads called the read handler twice (missing return after re-arming).

#include <gtest/gtest.h>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_array.hpp>
#include <boost/thread.hpp>

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

TEST(WebSocketStream, PingFrameAnsweredWithPong)
{
    socket_pair p;
    ws_t ws(p.ws);

    const uint8_t frames[] = {0x89, 0x02, 'h', 'i', 0x82, 0x05, 'h', 'e', 'l', 'l', 'o'};
    boost::asio::write(p.raw, boost::asio::buffer(frames));

    ws_reader<ws_t> r(ws, 5);
    r.start();
    run_until(p.ioc, r.done);
    ASSERT_TRUE(r.done);
    ASSERT_FALSE(r.ec) << r.ec.message();
    EXPECT_EQ(r.data, "hello");

    // The next write must send the pong first
    bool write_done = false;
    boost::system::error_code write_ec;
    const char out[] = "abc";
    ws.async_write_some(boost::asio::buffer(out, 3), websocket_test::make_ec_size_handler(write_done, write_ec));
    run_until(p.ioc, write_done);
    ASSERT_TRUE(write_done);
    ASSERT_FALSE(write_ec) << write_ec.message();

    const uint8_t expected[] = {0x8A, 0x02, 'h', 'i', 0x82, 0x03, 'a', 'b', 'c'};
    // The pong and the data frame are separate writes; wait until both have arrived (on FreeBSD the second
    // segment may not be readable yet when the write handler runs)
    for (int i = 0; i < 200 && p.raw.available() < sizeof(expected); i++)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }
    ASSERT_GE(p.raw.available(), sizeof(expected));
    uint8_t received[sizeof(expected)] = {};
    boost::asio::read(p.raw, boost::asio::buffer(received));
    EXPECT_EQ(std::string(reinterpret_cast<char*>(received), sizeof(received)),
              std::string(reinterpret_cast<const char*>(expected), sizeof(expected)));
}

TEST(WebSocketStream, PingFrameSplitAcrossReads)
{
    socket_pair p;
    ws_t ws(p.ws);

    const uint8_t part1[] = {0x89, 0x03, 'a'};
    boost::asio::write(p.raw, boost::asio::buffer(part1));

    ws_reader<ws_t> r(ws, 2);
    r.start();
    run_until(p.ioc, r.done, 300);
    ASSERT_FALSE(r.done) << "read completed early: " << r.ec.message();

    const uint8_t part2[] = {'b', 'c', 0x82, 0x02, 'o', 'k'};
    boost::asio::write(p.raw, boost::asio::buffer(part2));
    run_until(p.ioc, r.done);
    ASSERT_TRUE(r.done);
    ASSERT_FALSE(r.ec) << r.ec.message();
    EXPECT_EQ(r.data, "ok");
}
