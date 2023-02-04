#pragma once

// Workaround for older Boost.Asio versions that don't support Windows UNIX sockets.
// Include before anything else in cpp files

#define BOOST_ASIO_HAS_LOCAL_SOCKETS 1

#ifdef _WIN32
#include <WinSock2.h>
#include <versionhelpers.h>

#ifdef IO_REPARSE_TAG_AF_UNIX
#include <afunix.h>
#else
// Need to copy over sockaddr_un due to missing afunix.h
#define UNIX_PATH_MAX 108

typedef struct sockaddr_un
{
    ADDRESS_FAMILY sun_family;    /* AF_UNIX */
    char sun_path[UNIX_PATH_MAX]; /* pathname */
} SOCKADDR_UN, *PSOCKADDR_UN;
#endif

#include <boost/asio/version.hpp>

#if BOOST_ASIO_VERSION < 101801
namespace boost
{
namespace asio
{
namespace detail
{
typedef sockaddr_un sockaddr_un_type;
}
} // namespace asio
} // namespace boost

#include <boost/asio/local/stream_protocol.hpp>
#endif

#endif