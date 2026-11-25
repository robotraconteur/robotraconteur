// Regression test: an accepted TcpTransport socket that never sends its first bytes (or never finishes the
// WebSocket HTTP header) was held open forever with no timeout.

#include <gtest/gtest.h>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

#include <RobotRaconteur.h>

using namespace RobotRaconteur;
using boost::asio::ip::tcp;

namespace
{
struct closed_watch
{
    tcp::socket& s;
    uint8_t b;
    bool done;
    boost::system::error_code ec;
    closed_watch(tcp::socket& s) : s(s), b(0), done(false) {}
    void start()
    {
        s.async_read_some(
            boost::asio::buffer(&b, 1),
            boost::bind(&closed_watch::handler, this, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
    }
    void handler(const boost::system::error_code& e, size_t n)
    {
        if (!e && n > 0)
        {
            // Ignore any HTTP error response bytes, wait for the close
            start();
            return;
        }
        ec = e;
        done = true;
    }
};

// Run the ready handlers for about 100 ms (run_for needs Boost 1.66)
void run_for_100ms(RR_BOOST_ASIO_IO_CONTEXT& ioc)
{
    for (int i = 0; i < 10; i++)
    {
#if BOOST_ASIO_VERSION < 101200
        ioc.reset();
#else
        ioc.restart();
#endif
        ioc.poll();
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }
}
} // namespace

TEST(TcpTransport, IdleAcceptedSocketTimesOut)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    RR_SHARED_PTR<TcpTransport> t = RR_MAKE_SHARED<TcpTransport>(node);
    t->StartServer(0);
    // The accept stage is limited by the receive timeout
    t->SetDefaultReceiveTimeout(5000);
    node->RegisterTransport(t);
    uint16_t port = boost::numeric_cast<uint16_t>(t->GetListenPort());

    RR_BOOST_ASIO_IO_CONTEXT ioc;
    tcp::endpoint ep(boost::asio::ip::address_v4::loopback(), port);
    // Sends nothing
    tcp::socket s1(ioc);
    s1.connect(ep);
    // Starts a WebSocket request but never finishes the header
    tcp::socket s2(ioc);
    s2.connect(ep);
    std::string partial = "GET / HTTP/1.1\r\nHost: localhost\r\n";
    boost::asio::write(s2, boost::asio::buffer(partial));

    closed_watch w1(s1);
    closed_watch w2(s2);
    w1.start();
    w2.start();

    boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    while ((!w1.done || !w2.done) && (boost::posix_time::microsec_clock::universal_time() - start).total_seconds() < 20)
    {
        run_for_100ms(ioc);
    }

    EXPECT_TRUE(w1.done) << "silent accepted socket was not closed by the server";
    EXPECT_TRUE(w2.done) << "accepted socket with an unfinished WebSocket header was not closed by the server";

    node->Shutdown();
}

TEST(TcpTransport, AcceptTimeoutDoesNotCloseEstablishedConnection)
{
    // A raw connection that completes the RRAC magic within the accept timeout must not be closed by it
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    RR_SHARED_PTR<TcpTransport> t = RR_MAKE_SHARED<TcpTransport>(node);
    t->StartServer(0);
    t->SetDefaultReceiveTimeout(30000);
    node->RegisterTransport(t);
    uint16_t port = boost::numeric_cast<uint16_t>(t->GetListenPort());

    RR_BOOST_ASIO_IO_CONTEXT ioc;
    tcp::socket s1(ioc);
    s1.connect(tcp::endpoint(boost::asio::ip::address_v4::loopback(), port));
    boost::asio::write(s1, boost::asio::buffer(std::string("RRAC")));

    closed_watch w1(s1);
    w1.start();
    boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
    while (!w1.done && (boost::posix_time::microsec_clock::universal_time() - start).total_seconds() < 8)
    {
        run_for_100ms(ioc);
    }
    EXPECT_FALSE(w1.done) << "connection closed after the accept stage: " << w1.ec.message();

    node->Shutdown();
}
