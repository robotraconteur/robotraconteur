// Regression test: websocket_stream::sha1_hash() and random_nonce() leaked their OpenSSL BIO chain on every call
// (one leak per WebSocket handshake, on both server and client side).

#include <gtest/gtest.h>

#include <boost/atomic.hpp>
#include <cstdlib>

#include <openssl/crypto.h>

#include "RobotRaconteur/TcpTransport.h"
#include "websocket_stream.hpp"
#include "websocket_test_compat.h"

namespace
{
boost::atomic<long> live_allocs(0);

void* count_malloc(size_t num, const char* file, int line)
{
    (void)file;
    (void)line;
    void* p = std::malloc(num);
    if (p)
        live_allocs++;
    return p;
}

void* count_realloc(void* ptr, size_t num, const char* file, int line)
{
    (void)file;
    (void)line;
    if (num == 0)
    {
        if (ptr)
        {
            live_allocs--;
            std::free(ptr);
        }
        return NULL;
    }
    void* p = std::realloc(ptr, num);
    if (p && !ptr)
        live_allocs++;
    return p;
}

void count_free(void* ptr, const char* file, int line)
{
    (void)file;
    (void)line;
    if (ptr)
    {
        live_allocs--;
        std::free(ptr);
    }
}

// Must run before OpenSSL makes its first allocation
#if OPENSSL_VERSION_NUMBER < 0x10100000L
void count_free_nofile(void* ptr) { count_free(ptr, NULL, 0); }
const int mem_hooks_installed = CRYPTO_set_mem_ex_functions(&count_malloc, &count_realloc, &count_free_nofile);
#else
const int mem_hooks_installed = CRYPTO_set_mem_functions(&count_malloc, &count_realloc, &count_free);
#endif

class websocket_stream_test : public RobotRaconteur::detail::websocket_stream<boost::asio::ip::tcp::socket&>
{
  public:
    websocket_stream_test(boost::asio::ip::tcp::socket& s)
        : RobotRaconteur::detail::websocket_stream<boost::asio::ip::tcp::socket&>(s)
    {}
    using RobotRaconteur::detail::websocket_stream<boost::asio::ip::tcp::socket&>::random_nonce;
    using RobotRaconteur::detail::websocket_stream<boost::asio::ip::tcp::socket&>::sha1_hash;
};
} // namespace

TEST(WebSocketStream, Sha1HashValue)
{
    RR_BOOST_ASIO_IO_CONTEXT ioc;
    boost::asio::ip::tcp::socket sock(ioc);
    websocket_stream_test ws(sock);
    // RFC 6455 section 1.3 example
    EXPECT_EQ(ws.sha1_hash("dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11"),
              "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=");
    EXPECT_EQ(ws.random_nonce().size(), 24u);
}

TEST(WebSocketStream, Sha1HashNoLeak)
{
    if (!mem_hooks_installed)
    {
        GTEST_SKIP() << "Could not install OpenSSL memory hooks";
    }

    RR_BOOST_ASIO_IO_CONTEXT ioc;
    boost::asio::ip::tcp::socket sock(ioc);
    websocket_stream_test ws(sock);

    // Warm up any one-time OpenSSL initialization
    ws.sha1_hash("warmup");
    ws.random_nonce();

    long before = live_allocs.load();
    for (int i = 0; i < 100; i++)
    {
        ws.sha1_hash("dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
        ws.random_nonce();
    }
    long after = live_allocs.load();
    EXPECT_LT(after - before, 10) << "OpenSSL allocations leaked by sha1_hash/random_nonce";
}
