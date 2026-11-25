// Regression test: find_http_header_end picked the header terminator by pattern priority ("\r\n\r\n" anywhere
// first) instead of the earliest one, so with LF-only headers it could point past the first empty line, which is
// where the line parser actually stops. Bytes in between were silently skipped.

#include <gtest/gtest.h>

#include <boost/asio.hpp>
#include <boost/utility/string_ref.hpp>

#include "RobotRaconteur/TcpTransport.h"
#include "websocket_stream.hpp"
#include "websocket_test_compat.h"

namespace
{
class websocket_stream_test : public RobotRaconteur::detail::websocket_stream<boost::asio::ip::tcp::socket&>
{
  public:
    websocket_stream_test(boost::asio::ip::tcp::socket& s)
        : RobotRaconteur::detail::websocket_stream<boost::asio::ip::tcp::socket&>(s)
    {}
    int64_t header_end(const std::string& s)
    {
        boost::string_ref r(s);
        return find_http_header_end(r);
    }
};
} // namespace

TEST(WebSocketStream, HttpHeaderEndIsEarliestTerminator)
{
    RR_BOOST_ASIO_IO_CONTEXT ioc;
    boost::asio::ip::tcp::socket sock(ioc);
    websocket_stream_test ws(sock);

    EXPECT_EQ(ws.header_end("GET / HTTP/1.1\r\nA: b\r\n\r\n"), 24);
    EXPECT_EQ(ws.header_end("GET / HTTP/1.1\nA: b\n\n"), 21);
    EXPECT_EQ(ws.header_end("GET / HTTP/1.1\nA: b"), -1);
    // LF-only header followed by data that contains CRLFCRLF
    EXPECT_EQ(ws.header_end("GET / HTTP/1.1\nA: b\n\nxx\r\n\r\nyy"), 21);
    // CRLF header followed by data that contains LFLF
    EXPECT_EQ(ws.header_end("GET / HTTP/1.1\r\nA: b\r\n\r\nxx\n\nyy"), 24);
}
