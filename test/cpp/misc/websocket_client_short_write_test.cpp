// Regression test: a partially written client handshake request was retried from the original string
// (data bound instead of data2), so the same chunk was resent forever and data2 was freed while in use.

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

namespace
{
const char* ws_protocol = "robotraconteur.robotraconteur.com";

std::string client_request(const std::string& key)
{
    return "GET / HTTP/1.1\r\nHost: localhost\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Key: " +
           key + "\r\nSec-WebSocket-Protocol: robotraconteur.robotraconteur.com\r\nSec-WebSocket-Version: 13\r\n\r\n";
}

std::string server_response(const std::string& request)
{
    // Parse the key from the request and compute the accept key like a real server
    std::string::size_type p1 = request.find("Sec-WebSocket-Key: ");
    if (p1 == std::string::npos)
        return "";
    p1 += 19;
    std::string::size_type p2 = request.find("\r\n", p1);
    std::string key = request.substr(p1, p2 - p1);
    RR_BOOST_ASIO_IO_CONTEXT ioc;
    tcp::socket s(ioc);
    struct hasher : public ws_t
    {
        hasher(tcp::socket& s) : ws_t(s) {}
        using ws_t::sha1_hash;
    } h(s);
    std::string accept = h.sha1_hash(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    return "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: " +
           accept + "\r\nSec-WebSocket-Protocol: robotraconteur.robotraconteur.com\r\n\r\n";
}

std::string read_http_header(tcp::socket& s)
{
    boost::asio::streambuf sb;
    size_t n = boost::asio::read_until(s, sb, "\r\n\r\n");
    std::string str(boost::asio::buffers_begin(sb.data()), boost::asio::buffers_begin(sb.data()) + n);
    return str;
}
} // namespace

namespace
{
// Keeps the copied buffer alive until the write completes
struct write_handler
{
    boost::shared_ptr<std::vector<uint8_t> > d;
    boost::function<void(const boost::system::error_code&, size_t)> h;
    void operator()(const boost::system::error_code& ec, size_t n) const { h(ec, n); }
};

// Stream wrapper that writes at most max_write bytes per async_write_some, to exercise partial-write paths
class short_write_socket
{
  public:
    typedef tcp::socket::lowest_layer_type lowest_layer_type;

    short_write_socket(tcp::socket& s, size_t max_write) : s_(s), max_write_(max_write) {}

    lowest_layer_type& lowest_layer() { return s_.lowest_layer(); }
#if BOOST_ASIO_VERSION >= 101200
    typedef tcp::socket::executor_type executor_type;
    executor_type get_executor() { return s_.get_executor(); }
#else
    RR_BOOST_ASIO_IO_CONTEXT& get_io_service() { return s_.get_io_service(); }
#endif

    template <typename MutableBufferSequence, typename Handler>
    void async_read_some(const MutableBufferSequence& buffers, Handler handler)
    {
        s_.async_read_some(buffers, handler);
    }

    template <typename ConstBufferSequence, typename Handler>
    void async_write_some(const ConstBufferSequence& buffers, Handler handler)
    {
        boost::shared_ptr<std::vector<uint8_t> > d =
            boost::make_shared<std::vector<uint8_t> >(std::min(max_write_, boost::asio::buffer_size(buffers)));
        boost::asio::buffer_copy(boost::asio::buffer(*d), buffers);
        write_handler h = {d, handler};
        s_.async_write_some(boost::asio::buffer(*d), h);
    }

  private:
    tcp::socket& s_;
    size_t max_write_;
};

typedef RobotRaconteur::detail::websocket_stream<short_write_socket&> short_ws_t;
} // namespace

TEST(WebSocketStream, ClientHandshakeShortWrite)
{
    socket_pair p;
    short_write_socket sw(p.ws, 16);
    short_ws_t ws(sw);

    bool hs_done = false;
    boost::system::error_code hs_ec;
    ws.async_client_handshake("ws://localhost:1234/", ws_protocol, websocket_test::make_ec_handler(hs_done, hs_ec));

    boost::asio::streambuf sb(8192);
    bool req_done = false;
    boost::system::error_code req_ec;
    size_t req_n = 0;
    boost::asio::async_read_until(p.raw, sb, "\r\n\r\n",
                                  websocket_test::make_ec_size_handler(req_done, req_ec, &req_n));
    run_until(p.ioc, req_done);
    ASSERT_TRUE(req_done);
    ASSERT_FALSE(req_ec) << "request was not received intact: " << req_ec.message();

    std::string req(boost::asio::buffers_begin(sb.data()), boost::asio::buffers_begin(sb.data()) + req_n);
    EXPECT_EQ(req.find("GET / HTTP/1.1\r\nHost: localhost:1234\r\nUpgrade: websocket\r\n"), 0u);

    std::string resp = server_response(req);
    ASSERT_FALSE(resp.empty());
    boost::asio::write(p.raw, boost::asio::buffer(resp));

    run_until(p.ioc, hs_done);
    ASSERT_TRUE(hs_done);
    EXPECT_FALSE(hs_ec) << hs_ec.message();
}
