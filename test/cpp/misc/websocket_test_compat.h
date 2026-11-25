// Helpers shared by the websocket_stream regression tests. They avoid C++11 and Boost.Asio APIs newer than
// Boost 1.58, so the tests also build on the oldest supported platform (Ubuntu 16.04: GCC 5 in C++98 mode,
// Boost 1.58, gtest 1.8).

#pragma once

#include <string>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>

#include "RobotRaconteur/RobotRaconteurConfig.h"

namespace websocket_test
{
// Run the handlers that are ready until done is set or the timeout expires
inline void run_until(RR_BOOST_ASIO_IO_CONTEXT& ioc, const bool& done, int timeout_ms = 2000)
{
    for (int i = 0; i < timeout_ms / 10 && !done; i++)
    {
#if BOOST_ASIO_VERSION < 101200
        ioc.reset();
#else
        ioc.restart();
#endif
        ioc.poll();
        if (!done)
        {
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        }
    }
}

// Completion handlers that record the result and set a done flag
struct ec_handler
{
    bool* done;
    boost::system::error_code* ec;
    void operator()(const boost::system::error_code& e) const
    {
        *ec = e;
        *done = true;
    }
};

struct ec_size_handler
{
    bool* done;
    boost::system::error_code* ec;
    size_t* n;
    void operator()(const boost::system::error_code& e, size_t n1) const
    {
        *ec = e;
        if (n)
            *n = n1;
        *done = true;
    }
};

struct protocol_ec_handler
{
    bool* done;
    boost::system::error_code* ec;
    std::string* protocol;
    void operator()(const std::string& p, const boost::system::error_code& e) const
    {
        if (protocol)
            *protocol = p;
        *ec = e;
        *done = true;
    }
};

inline ec_handler make_ec_handler(bool& done, boost::system::error_code& ec)
{
    ec_handler h = {&done, &ec};
    return h;
}

inline ec_size_handler make_ec_size_handler(bool& done, boost::system::error_code& ec, size_t* n = NULL)
{
    ec_size_handler h = {&done, &ec, n};
    return h;
}

inline protocol_ec_handler make_protocol_ec_handler(bool& done, boost::system::error_code& ec,
                                                    std::string* protocol = NULL)
{
    protocol_ec_handler h = {&done, &ec, protocol};
    return h;
}
} // namespace websocket_test

// GTEST_SKIP was added in googletest 1.10. The fallback returns from the test; a streamed message is not printed
#ifndef GTEST_SKIP
#include <iostream>
#define GTEST_SKIP()                                                                                                   \
    if (true)                                                                                                          \
    {                                                                                                                  \
        std::cerr << "[  SKIPPED ] (googletest < 1.10)" << std::endl;                                                  \
        return;                                                                                                        \
    }                                                                                                                  \
    else                                                                                                               \
        std::cerr
#endif
