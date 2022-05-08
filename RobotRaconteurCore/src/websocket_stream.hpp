// Copyright 2011-2020 Wason Technology, LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <boost/thread.hpp>
#include <boost/bind/placeholders.hpp>
#include <boost/asio.hpp>
#include <boost/shared_array.hpp>
#include <boost/bind/protect.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random.hpp>
#include <boost/date_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/regex.hpp>
//#include <boost/asio/ip/tcp.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/type_traits/remove_reference.hpp>

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
#include <wincrypt.h>
#elif defined(ROBOTRACONTEUR_USE_OPENSSL)
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#else
#error Neither ROBOTRACONTEUR_USE_SCHANNEL or ROBOTRACONTEUR_OPENSSL specified
#endif

// TODO: It may be best to not depend on this file
#include "RobotRaconteur/DataTypes.h"

#pragma once

namespace RobotRaconteur
{
namespace detail
{

template <typename Stream, uint8_t DataFrameType = 0x2>
class websocket_stream : private boost::noncopyable
{
  protected:
    Stream next_layer_;
    boost::mutex next_layer_lock;

  public:
    enum WebSocketOpcode
    {
        WebSocketOpcode_continuation = 0x0,
        WebSocketOpcode_text = 0x1,
        WebSocketOpcode_binary = 0x2,
        WebSocketOpcode_close = 0x8,
        WebSocketOpcode_ping = 0x9,
        WebSocketOpcode_pong = 0xA
    };

#ifdef ROBOTRACONTEUR_USE_OPENSSL
    typedef typename boost::remove_reference<Stream>::type next_layer_type;
    typedef typename next_layer_type::lowest_layer_type lowest_layer_type;

    lowest_layer_type& lowest_layer() { return next_layer_.lowest_layer(); }

    const lowest_layer_type& lowest_layer() const { return next_layer_.lowest_layer(); }
#endif

  protected:
    boost::mutex random_lock;
    boost::random::mt19937 random;

  public:
#if BOOST_ASIO_VERSION >= 101200
    typedef RR_BOOST_ASIO_IO_CONTEXT executor_type;
    executor_type get_executor() BOOST_ASIO_NOEXCEPT { return next_layer_.get_executor(); }
#endif

    websocket_stream(Stream& next_layer)
        : next_layer_(next_layer) RR_MEMBER_ARRAY_INIT(recv_header1) RR_MEMBER_ARRAY_INIT(recv_header2)
              RR_MEMBER_ARRAY_INIT(recv_frame_mask)
    {
        extra_recv_data_len = 0;
        send_en_mask = false;

        recv_frame_length = 0;
        recv_frame_pos = 0;
        recv_frame_en_mask = false;
        recv_frame_opcode = 0;

        ping_requested = false;
        recv_handshake_first_line = false;
        ping_data_len = 0;

        boost::mutex::scoped_lock lock(random_lock);
        std::string str = boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::universal_time());
        boost::random::seed_seq seed1(str.begin(), str.end());
        random.seed(seed1);
    }

    void async_server_handshake(
        const std::string& protocol, const std::vector<std::string>& allowed_origins,
        boost::function<void(const std::string& protocol, const boost::system::error_code& ec)> handler)
    {
        boost::shared_array<uint8_t> buf(new uint8_t[4096]);
        recv_handshake_first_line = true;

        boost::mutex::scoped_lock lock(next_layer_lock);
        next_layer_.async_read_some(boost::asio::buffer(buf.get(), 4096),
                                    boost::bind(&websocket_stream::server_handshake2, this, buf, protocol,
                                                allowed_origins, boost::asio::placeholders::bytes_transferred,
                                                boost::asio::placeholders::error, boost::protect(RR_MOVE(handler))));
    }

  protected:
    boost::shared_array<uint8_t> extra_recv_data;
    size_t extra_recv_data_len;
    boost::mutex extra_recv_data_lock;

    boost::mutex handshake_lock;
    std::map<std::string, std::string> handshake_recv_args;
    bool recv_handshake_first_line;
    std::string server_handshake_op;
    std::string server_handshake_path;

    void server_handshake2(
        const boost::shared_array<uint8_t>& buf, const std::string& protocol,
        const std::vector<std::string>& allowed_origins, std::size_t n, const boost::system::error_code& ec,
        boost::function<void(const std::string& protocol, const boost::system::error_code& ec)> handler)
    {
        if (ec)
        {
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.close();
            }
            handler("", ec);
            return;
        }

        if (n == 0)
        {
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.close();
            }
            boost::system::error_code ec1(boost::system::errc::connection_aborted, boost::system::generic_category());
            handler("", ec1);
            return;
        }

        int64_t pos = 0;
        bool end_header = false;

        while (pos < boost::numeric_cast<int64_t>(n) && !end_header)
        {

            int64_t i_lf = -1;
            for (int64_t i = pos; i < boost::numeric_cast<int64_t>(n); i++)
            {
                if (buf[boost::numeric_cast<std::ptrdiff_t>(i)] == '\n')
                {
                    i_lf = i;
                    break;
                }
            }

            if (i_lf == -1)
            {
                if (extra_recv_data_len + n >= 4096)
                {
                    // We are just getting garbage...
                    {
                        boost::mutex::scoped_lock lock(next_layer_lock);
                        next_layer_.close();
                    }

                    boost::system::error_code ec1(boost::system::errc::connection_aborted,
                                                  boost::system::generic_category());
                    handler("", ec1);
                    return;
                }
                {
                    boost::mutex::scoped_lock lock(extra_recv_data_lock);
                    extra_recv_data = boost::shared_array<uint8_t>(new uint8_t[n - boost::numeric_cast<size_t>(pos)]);
                    memcpy(extra_recv_data.get(), buf.get() + boost::numeric_cast<size_t>(pos),
                           n - boost::numeric_cast<size_t>(pos));
                    extra_recv_data_len = n - boost::numeric_cast<size_t>(pos);
                }
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.async_read_some(boost::asio::buffer(buf.get(), 4096),
                                            boost::bind(&websocket_stream::server_handshake2, this, buf, protocol,
                                                        allowed_origins, boost::asio::placeholders::bytes_transferred,
                                                        boost::asio::placeholders::error, boost::protect(handler)));
                return;
            }

            std::string line(reinterpret_cast<char*>(buf.get()) + boost::numeric_cast<size_t>(pos),
                             boost::numeric_cast<size_t>(i_lf) - boost::numeric_cast<size_t>(pos) + 1);
            if (extra_recv_data_len != 0)
            {
                boost::mutex::scoped_lock lock(extra_recv_data_lock);
                line = std::string(reinterpret_cast<char*>(extra_recv_data.get()), extra_recv_data_len);
                extra_recv_data_len = 0;
                extra_recv_data.reset();
            }
            boost::trim(line);

            pos = i_lf + 1;

            if (line.empty())
            {
                end_header = true;
                continue;
            }

            if (recv_handshake_first_line)
            {
                std::vector<std::string> s1;
                boost::split(s1, line, boost::is_any_of(" \t"), boost::algorithm::token_compress_on);
                if (s1.size() != 3)
                {
                    send_server_error("400 request error", handler);
                    return;
                }

                if (s1.at(0) != "GET")
                {
                    send_server_error("404 File not found", handler);
                    return;
                }

                server_handshake_op = s1.at(0);

                if (s1.at(2) != "HTTP/1.1")
                {
                    send_server_error("400 request error", handler);
                    return;
                }

                std::string path_with_query = s1.at(1);
                std::vector<std::string> s2;
                std::string requested_path;
                boost::split(s2, path_with_query, boost::is_from_range('?', '?'));
                if (s2.size() > 2)
                {
                    send_server_error("404 File not found", handler);
                    return;
                }

                requested_path = s2.at(0);

                if (requested_path != "/" && requested_path != "*")
                {
                    send_server_error("404 File not found", handler);
                    return;
                }
                server_handshake_path = "/";
                recv_handshake_first_line = false;
            }
            else
            {
                size_t colon_pos = line.find(':');
                if (colon_pos == std::string::npos)
                {
                    send_server_error("400 request error", handler);
                    return;
                }

                std::string name = line.substr(0, colon_pos);
                std::string value;
                if (line.size() > colon_pos + 1)
                {
                    value = boost::trim_copy(line.substr(colon_pos + 1));
                }

                if (name == "Sec-WebSocket-Version" &&
                    handshake_recv_args.find("Sec-WebSocket-Version") != handshake_recv_args.end())
                {
                    std::string value2 = handshake_recv_args.at("Sec-WebSocket-Version") + ", " + value;
                    handshake_recv_args.at("Sec-WebSocket-Version") = value2;
                }
                if (name == "Sec-WebSocket-Protocol" &&
                    handshake_recv_args.find("Sec-WebSocket-Protocol") != handshake_recv_args.end())
                {
                    std::string value2 = handshake_recv_args.at("Sec-WebSocket-Protocol") + ", " + value;
                    handshake_recv_args.at("Sec-WebSocket-Protocol") = value2;
                }
                else
                {
                    handshake_recv_args.insert(std::make_pair(name, value));
                }
            }
        }

        std::string accept_key;

        if (handshake_recv_args.find("Upgrade") == handshake_recv_args.end() ||
            handshake_recv_args.find("Sec-WebSocket-Key") == handshake_recv_args.end() ||
            handshake_recv_args.find("Sec-WebSocket-Version") == handshake_recv_args.end())
        {
            send_server_error("426 Upgrade Required", handler);
            return;
        }

        if (boost::to_lower_copy(handshake_recv_args.at("Upgrade")) != "websocket")
        {
            send_server_error("426 Upgrade Required", handler);
            return;
        }

        std::vector<std::string> s2;
        boost::split(s2, handshake_recv_args.at("Sec-WebSocket-Version"), boost::is_from_range(',', ','));

        bool found_ver = false;
        BOOST_FOREACH (std::string& e, s2)
        {
            if (boost::trim_copy(e) == "13")
            {
                found_ver = true;
                break;
            }
        }

        if (!found_ver)
        {
            send_server_error("426 Upgrade Required", handler);
            return;
        }

        if (handshake_recv_args.find("Sec-WebSocket-Protocol") != handshake_recv_args.end())
        {
            bool found_protocol = false;

            std::vector<std::string> s3;
            boost::split(s3, handshake_recv_args.at("Sec-WebSocket-Protocol"), boost::is_from_range(',', ','));

            BOOST_FOREACH (std::string& e, s3)
            {
                if (boost::to_lower_copy(boost::trim_copy(e)) == boost::to_lower_copy(protocol))
                {
                    found_protocol = true;
                    break;
                }
            }

            if (!found_protocol)
            {
                send_server_error("426 Upgrade Required", handler);
                return;
            }
        }

        if (handshake_recv_args.find("Origin") != handshake_recv_args.end() ||
            handshake_recv_args.find("origin") != handshake_recv_args.end())
        {

            std::string origin1;
            if (handshake_recv_args.find("Origin") != handshake_recv_args.end())
            {
                origin1 = handshake_recv_args.at("Origin");
            }
            else
            {
                origin1 = handshake_recv_args.at("origin");
            }
            bool good_origin = false;

            if (boost::range::find(allowed_origins, origin1) != allowed_origins.end())
            {
                good_origin = true;
            }

            if (!good_origin)
            {

                boost::smatch origin_result;
                if (!boost::regex_search(origin1, origin_result, boost::regex("^([^:\\s]+://[^/]*).*$")))
                {
                    send_server_error("403 Forbidden Origin", handler);
                    return;
                }

                if (origin_result.empty())
                {
                    send_server_error("403 Forbidden Origin", handler);
                    return;
                }

                std::string origin = origin_result[1];

                BOOST_FOREACH (const std::string& e, allowed_origins)
                {
                    if (!boost::contains(e, "*"))
                    {
                        if (e == origin)
                        {
                            good_origin = true;
                            break;
                        }

                        if (boost::ends_with(e, "://"))
                        {
                            if (boost::starts_with(origin, e))
                            {
                                good_origin = true;
                                break;
                            }
                        }
                    }
                    else
                    {
                        std::string::size_type scheme_pos = e.find("://");
                        if (scheme_pos == std::string::npos)
                        {
                            continue;
                        }

                        std::string test_scheme = e.substr(0, scheme_pos) + "://";
                        if (!boost::starts_with(origin, test_scheme))
                        {
                            continue;
                        }

                        std::string origin2 = boost::replace_first_copy(origin, test_scheme, "");
                        std::string e2 = boost::replace_first_copy(e, test_scheme, "");
                        boost::replace_first(e2, "*", "");
                        if (boost::ends_with(origin2, e2))
                        {
                            good_origin = true;
                            break;
                        }
                    }
                }
            }

            if (!good_origin)
            {
                send_server_error("403 Forbidden Origin", handler);
                return;
            }
        }

        std::string key = handshake_recv_args.at("Sec-WebSocket-Key") + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        accept_key = sha1_hash(key);

        send_server_success_response(accept_key, protocol, handler);

        // send_server_error("404 File not found", handler);
    }

    void send_server_error(
        const std::string& error,
        boost::function<void(const std::string& protocol, const boost::system::error_code& ec)> handler)
    {
        std::string l = "HTTP/1.1 " + error +
                        "\r\nUpgrade: websocket\r\nSec-WebSocket-Protocol: "
                        "robotraconteur.robotraconteur.com\r\nSec-WebSocket-Version: 13\r\nConnection: close\r\n\r\n";
        boost::shared_ptr<std::string> data = boost::make_shared<std::string>(l);
        boost::mutex::scoped_lock lock(next_layer_lock);
        next_layer_.async_write_some(boost::asio::buffer(data->c_str(), data->size()),
                                     boost::bind(&websocket_stream::end_send_server_error, this, data,
                                                 boost::asio::placeholders::bytes_transferred,
                                                 boost::asio::placeholders::error, boost::protect(RR_MOVE(handler))));
    }

    void end_send_server_error(
        const boost::shared_ptr<std::string>& data, std::size_t n, const boost::system::error_code& ec,
        boost::function<void(const std::string& protocol, const boost::system::error_code& ec)> handler)
    {

        if (ec || n >= data->size() || n == 0)
        {
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.close();
            }
            boost::system::error_code ec1(boost::system::errc::connection_aborted, boost::system::generic_category());
            handler("", ec1);
            return;
        }

        boost::shared_ptr<std::string> data2 = boost::make_shared<std::string>(data->substr(n));
        boost::mutex::scoped_lock lock(next_layer_lock);
        next_layer_.async_write_some(boost::asio::buffer(data2->c_str(), data2->size()),
                                     boost::bind(&websocket_stream::end_send_server_error, this, data2,
                                                 boost::asio::placeholders::bytes_transferred,
                                                 boost::asio::placeholders::error, boost::protect(handler)));
    }

    void send_server_success_response(
        const std::string& accept_key, const std::string& protocol,
        boost::function<void(const std::string& protocol, const boost::system::error_code& ec)> handler)
    {
        std::string l =
            std::string("HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: "
                        "Upgrade\r\nSec-WebSocket-Accept: ") +
            accept_key +
            std::string(
                "\r\nSec-WebSocket-Protocol: robotraconteur.robotraconteur.com\r\nSec-WebSocket-Version: 13\r\n\r\n");
        boost::shared_ptr<std::string> data = boost::make_shared<std::string>(l);
        boost::mutex::scoped_lock lock(next_layer_lock);
        next_layer_.async_write_some(boost::asio::buffer(data->c_str(), data->size()),
                                     boost::bind(&websocket_stream::end_send_server_success_response, this, data,
                                                 protocol, boost::asio::placeholders::bytes_transferred,
                                                 boost::asio::placeholders::error, boost::protect(RR_MOVE(handler))));
    }

    void end_send_server_success_response(
        const boost::shared_ptr<std::string>& data, const std::string& protocol, std::size_t n,
        const boost::system::error_code& ec,
        boost::function<void(const std::string& protocol, const boost::system::error_code& ec)> handler)
    {

        if (ec || n == 0)
        {
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.close();
            }
            boost::system::error_code ec1(boost::system::errc::connection_aborted, boost::system::generic_category());
            handler("", ec1);
            return;
        }

        if (n >= data->size())
        {
            boost::system::error_code ec1;
            handler(protocol, ec1);
            return;
        }

        boost::shared_ptr<std::string> data2 = boost::make_shared<std::string>(data->substr(n));
        boost::mutex::scoped_lock lock(next_layer_lock);
        next_layer_.async_write_some(boost::asio::buffer(data2->c_str(), data2->size()),
                                     boost::bind(&websocket_stream::end_send_server_error, this, data2,
                                                 boost::asio::placeholders::bytes_transferred,
                                                 boost::asio::placeholders::error, boost::protect(handler)));
    }

  public:
    void async_client_handshake(const std::string& url, const std::string& protocol,
                                boost::function<void(const boost::system::error_code& ec)> handler)
    {
        this->send_en_mask = true;

        std::string key = random_nonce();

        boost::smatch url_result;
        boost::regex_search(
            url, url_result,
            boost::regex(
                "^([^:]+)://((?:\\[[A-Fa-f0-9:]+(?:\\%\\w*)?\\])|(?:[^\\[\\]\\:/\\?]+))(?::([^:/\\?]+))?/?(.*)$"));

        if (url_result.size() < 4)
            throw InvalidArgumentException("Malformed URL");

        if (url_result[1] != "ws" && url_result[1] != "wss")
            throw InvalidArgumentException("Invalid transport type for websocket");

        std::string host = url_result[2];
        std::string port_str = url_result[3];

        if (!port_str.empty())
        {
            host = host + ":" + port_str;
        }

        std::string path = "/" + url_result[4];

        std::string data1 = "GET " + path +
                            " HTTP/1.1\r\n"
                            "Host: " +
                            host +
                            "\r\n"
                            "Upgrade: websocket\r\n"
                            "Connection: Upgrade\r\n"
                            "Sec-WebSocket-Key: " +
                            key +
                            "\r\n"
                            "Sec-WebSocket-Protocol: " +
                            protocol +
                            "\r\n"
                            "Sec-WebSocket-Version: 13\r\n\r\n";

        boost::shared_ptr<std::string> data = boost::make_shared<std::string>(data1);

        boost::mutex::scoped_lock lock(next_layer_lock);
        next_layer_.async_write_some(boost::asio::buffer(data->c_str(), data->size()),
                                     boost::bind(&websocket_stream::async_client_handshake2, this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred, data, url, protocol, key,
                                                 boost::protect(RR_MOVE(handler))));
    }

  protected:
    void async_client_handshake2(const boost::system::error_code& ec, size_t n, boost::shared_ptr<std::string>(data),
                                 const std::string& url, const std::string& protocol, const std::string& key,
                                 boost::function<void(const boost::system::error_code& ec)> handler)
    {
        if (ec || n == 0)
        {
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.lowest_layer().close();
            }
            boost::system::error_code ec1(boost::system::errc::connection_aborted, boost::system::generic_category());
            handler(ec1);
            return;
        }

        if (n < data->size())
        {
            boost::shared_ptr<std::string> data2 = boost::make_shared<std::string>(data->substr(n));
            boost::mutex::scoped_lock lock(next_layer_lock);
            next_layer_.async_write_some(boost::asio::buffer(data2->c_str(), data2->size()),
                                         boost::bind(&websocket_stream::async_client_handshake2, this,
                                                     boost::asio::placeholders::error,
                                                     boost::asio::placeholders::bytes_transferred, data, url, protocol,
                                                     key, boost::protect(handler)));
            return;
        }

        boost::shared_array<uint8_t> buf(new uint8_t[4096]);
        recv_handshake_first_line = true;
        boost::mutex::scoped_lock lock(next_layer_lock);
        next_layer_.async_read_some(
            boost::asio::buffer(buf.get(), 4096),
            boost::bind(&websocket_stream::async_client_handshake3, this, buf, boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred, url, protocol, key, boost::protect(handler)));
    }

    void async_client_handshake3(const boost::shared_array<uint8_t>& buf, const boost::system::error_code& ec,
                                 std::size_t n, const std::string& url, const std::string& protocol,
                                 const std::string& key,
                                 boost::function<void(const boost::system::error_code& ec)> handler)
    {
        if (ec)
        {
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.lowest_layer().close();
            }
            handler(ec);
            return;
        }

        if (n == 0)
        {
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.lowest_layer().close();
            }
            boost::system::error_code ec1(boost::system::errc::connection_aborted, boost::system::generic_category());
            handler(ec1);
            return;
        }

        int64_t pos = 0;
        bool end_header = false;

        while (pos < boost::numeric_cast<int64_t>(n) && !end_header)
        {

            int64_t i_lf = -1;
            for (int64_t i = pos; i < boost::numeric_cast<int64_t>(n); i++)
            {
                if (buf[boost::numeric_cast<std::ptrdiff_t>(i)] == '\n')
                {
                    i_lf = i;
                    break;
                }
            }

            if (i_lf == -1)
            {
                if (extra_recv_data_len + n >= 4096)
                {
                    // We are just getting garbage...
                    {
                        boost::mutex::scoped_lock lock(next_layer_lock);
                        next_layer_.lowest_layer().close();
                    }
                    boost::system::error_code ec1(boost::system::errc::connection_aborted,
                                                  boost::system::generic_category());
                    handler(ec1);
                    return;
                }
                {
                    boost::mutex::scoped_lock lock(extra_recv_data_lock);
                    extra_recv_data = boost::shared_array<uint8_t>(new uint8_t[n - boost::numeric_cast<size_t>(pos)]);
                    memcpy(extra_recv_data.get(), buf.get() + boost::numeric_cast<size_t>(pos),
                           n - boost::numeric_cast<size_t>(pos));
                    extra_recv_data_len = n - boost::numeric_cast<size_t>(pos);
                }
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.async_read_some(boost::asio::buffer(buf.get(), 4096),
                                            boost::bind(&websocket_stream::async_client_handshake3, this, buf,
                                                        boost::asio::placeholders::error,
                                                        boost::asio::placeholders::bytes_transferred, url, protocol,
                                                        key, boost::protect(handler)));
                return;
            }

            std::string line(reinterpret_cast<char*>(buf.get()) + boost::numeric_cast<size_t>(pos),
                             boost::numeric_cast<size_t>(i_lf) - boost::numeric_cast<size_t>(pos) + 1);
            if (extra_recv_data_len != 0)
            {
                boost::mutex::scoped_lock lock(extra_recv_data_lock);
                line = std::string(reinterpret_cast<char*>(extra_recv_data.get()), extra_recv_data_len).append(line);
                extra_recv_data_len = 0;
                extra_recv_data.reset();
            }
            boost::trim(line);

            pos = i_lf + 1;

            if (line.empty())
            {
                end_header = true;
                continue;
            }

            if (recv_handshake_first_line)
            {
                std::vector<std::string> s1;
                boost::split(s1, line, boost::is_any_of(" \t"), boost::algorithm::token_compress_on);
                if (s1.size() < 2)
                {
                    {
                        boost::mutex::scoped_lock lock(next_layer_lock);
                        next_layer_.lowest_layer().close();
                    }
                    boost::system::error_code ec1(boost::system::errc::io_error, boost::system::generic_category());
                    handler(ec1);
                    return;
                }

                if (s1.at(0) != "HTTP/1.1")
                {
                    {
                        boost::mutex::scoped_lock lock(next_layer_lock);
                        next_layer_.lowest_layer().close();
                    }
                    boost::system::error_code ec1(boost::system::errc::io_error, boost::system::generic_category());
                    handler(ec1);
                    return;
                }

                if (s1.at(1) != "101")
                {
                    {
                        boost::mutex::scoped_lock lock(next_layer_lock);
                        next_layer_.lowest_layer().close();
                    }
                    boost::system::error_code ec1(boost::system::errc::io_error, boost::system::generic_category());
                    handler(ec1);
                    return;
                }

                recv_handshake_first_line = false;
            }
            else
            {
                size_t colon_pos = line.find(':');
                if (colon_pos == std::string::npos)
                {
                    {
                        boost::mutex::scoped_lock lock(next_layer_lock);
                        next_layer_.lowest_layer().close();
                    }
                    boost::system::error_code ec1(boost::system::errc::io_error, boost::system::generic_category());
                    handler(ec1);
                    return;
                }

                std::string name = line.substr(0, colon_pos);
                std::string value;
                if (line.size() > colon_pos + 1)
                {
                    value = boost::trim_copy(line.substr(colon_pos + 1));
                }

                if (handshake_recv_args.find(name) == handshake_recv_args.end())
                {
                    handshake_recv_args.insert(std::make_pair(name, value));
                }
                else
                {
                    std::string value1 = handshake_recv_args.at(name) + ", " + value;
                    handshake_recv_args.erase(name);
                    handshake_recv_args.insert(std::make_pair(name, value1));
                }
            }
        }

        if (handshake_recv_args.find("Upgrade") == handshake_recv_args.end() ||
            handshake_recv_args.find("Connection") == handshake_recv_args.end() ||
            handshake_recv_args.find("Sec-WebSocket-Accept") == handshake_recv_args.end())
        {
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.lowest_layer().close();
            }
            boost::system::error_code ec1(boost::system::errc::io_error, boost::system::generic_category());
            handler(ec1);
            return;
        }

        std::string accept = sha1_hash(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

        if (boost::to_lower_copy(handshake_recv_args.at("Upgrade")) != "websocket" ||
            boost::to_lower_copy(handshake_recv_args.at("Connection")) != "upgrade" ||
            handshake_recv_args.at("Sec-WebSocket-Accept") != accept)
        {
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.lowest_layer().close();
            }
            boost::system::error_code ec1(boost::system::errc::io_error, boost::system::generic_category());
            handler(ec1);
            return;
        }

        if (handshake_recv_args.find("Sec-WebSocket-Protocol") != handshake_recv_args.end())
        {
            if (boost::to_lower_copy(handshake_recv_args.at("Sec-WebSocket-Protocol")) !=
                boost::to_lower_copy(protocol))
            {
                {
                    boost::mutex::scoped_lock lock(next_layer_lock);
                    next_layer_.lowest_layer().close();
                }

                boost::system::error_code ec1(boost::system::errc::protocol_error, boost::system::generic_category());
                handler(ec1);
                return;
            }
        }

        boost::system::error_code ec2;
        handler(ec2);
    }

    std::string sha1_hash(const std::string& blob)
    {

#ifdef ROBOTRACONTEUR_USE_SCHANNEL

        HCRYPTPROV hProv = 0;
        HCRYPTPROV hHash = 0;

        if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
        {
            return "";
        }
        if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
        {
            CryptReleaseContext(hProv, 0);
            return "";
        }

        if (!CryptHashData(hHash, (BYTE*)blob.c_str(), (DWORD)blob.length(), 0))
        {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            return "";
        }

        BYTE rgbHash[20] = {0};
        DWORD cbHash = 20;

        if (!CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
        {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            return "";
        }

        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);

        DWORD hashlen = 0;
        CryptBinaryToString(rgbHash, cbHash, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &hashlen);

        boost::shared_array<char> buf2(new char[hashlen]);
        if (!CryptBinaryToString(rgbHash, cbHash, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, buf2.get(), &hashlen))
        {
            return "";
        }

        return std::string(buf2.get(), hashlen);
#endif
#ifdef ROBOTRACONTEUR_USE_OPENSSL
        boost::array<uint8_t, SHA_DIGEST_LENGTH> hash = {};
        if (!SHA1(reinterpret_cast<const uint8_t*>(blob.c_str()), blob.size(), hash.data()))
        {
            return "";
        }

        BIO* b64 = BIO_new(BIO_f_base64());
        if (!b64)
            return "";
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

        BIO* mem = BIO_new(BIO_s_mem());

        if (!mem)
        {
            BIO_free(b64);
            return "";
        }

        BIO_push(b64, mem);

        bool done = false;
        int res = 0;
        while (!done)
        {
            res = BIO_write(b64, hash.data(), (int)SHA_DIGEST_LENGTH);

            if (res <= 0)
            {
                if (BIO_should_retry(b64))
                {
                    continue;
                }
                else
                {
                    BIO_free(b64);
                    BIO_free(mem);
                }
            }
            else
            {
                done = true;
            }
        }

        BIO_flush(b64);
        char* output = NULL;
        int output_len = BIO_get_mem_data(mem, &output);
        return std::string(output, output_len);
#endif
    }

    std::string random_nonce()
    {

        boost::array<uint8_t, 16> random_data = {};

        boost::random::uniform_int_distribution<uint8_t> distribution(0, std::numeric_limits<uint8_t>::max());
        for (size_t i = 0; i < 16; i++)
        {
            random_data[i] = distribution(random);
        }
#ifdef ROBOTRACONTEUR_USE_SCHANNEL
        DWORD hashlen = 0;
        CryptBinaryToString(random_data.data(), 16, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &hashlen);

        boost::shared_array<char> buf2(new char[hashlen]);

        CryptBinaryToString(random_data.data(), 16, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, buf2.get(), &hashlen);

        return std::string(buf2.get(), hashlen);
#endif

#ifdef ROBOTRACONTEUR_USE_OPENSSL
        BIO* b64 = BIO_new(BIO_f_base64());
        if (!b64)
            return "";
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

        BIO* mem = BIO_new(BIO_s_mem());

        if (!mem)
        {
            BIO_free(b64);
            return "";
        }

        BIO_push(b64, mem);

        bool done = false;
        int res = 0;
        while (!done)
        {
            res = BIO_write(b64, random_data.data(), static_cast<int>(16));

            if (res <= 0)
            {
                if (BIO_should_retry(b64))
                {
                    continue;
                }
                else
                {
                    BIO_free(b64);
                    BIO_free(mem);
                }
            }
            else
            {
                done = true;
            }
        }

        BIO_flush(b64);
        char* output = NULL;
        int output_len = BIO_get_mem_data(mem, &output);
        return std::string(output, output_len);
#endif
    }

    bool send_en_mask;

    boost::shared_array<uint8_t> send_mask;

    const_buffers async_write_buffers;

    void async_write_message(uint8_t command, const const_buffers& buffer,
                             boost::function<void(const boost::system::error_code&, std::size_t)> handler)
    {
        size_t count = boost::asio::buffer_size(buffer);

        // Clip data at 16 KB
        if (count > std::numeric_limits<uint16_t>::max())
        {
            count = std::numeric_limits<uint16_t>::max();
        }

        // Send header
        size_t header_len = (count <= 125) ? 2 : 4;
        if (send_en_mask)
            header_len += 4;
        boost::shared_array<uint8_t> header(new uint8_t[header_len]);

        if (send_en_mask)
        {
            boost::mutex::scoped_lock lock(random_lock);
            send_mask = boost::shared_array<uint8_t>(new uint8_t[4]);
            boost::random::uniform_int_distribution<uint32_t> distribution(0, std::numeric_limits<uint32_t>::max());
            uint32_t imask = distribution(random);
            memcpy(send_mask.get(), &imask, 4);
        }

        header.get()[0] = static_cast<uint8_t>(0x80 | (command & 0xF));
        if (count <= 125)
        {
            header.get()[1] = static_cast<uint8_t>(count);
            if (send_en_mask)
            {
                memcpy(header.get() + 2, send_mask.get(), 4);
            }
        }
        else
        {
            header.get()[1] = 126;

            uint16_t c = static_cast<uint16_t>(count);
            uint8_t* c1 = reinterpret_cast<uint8_t*>(&c);
#if !BOOST_ENDIAN_BIG_BYTE
            header.get()[2] = c1[1];
            header.get()[3] = c1[0];
#else
            header.get()[2] = c1[0];
            header.get()[3] = c1[1];
#endif

            if (send_en_mask)
            {
                memcpy(header.get() + 4, send_mask.get(), 4);
            }
        }

        if (send_en_mask)
        {
            header.get()[1] |= 0x80;
        }

        const_buffers send_buffer;
        send_buffer.push_back(boost::asio::buffer(header.get(), header_len));

        // Send Data
        boost::shared_array<uint8_t> buffer2;
        if (send_en_mask)
        {
            // TODO: make this faster
            buffer2 = boost::shared_array<uint8_t>(new uint8_t[count]);
            boost::asio::buffers_iterator<const_buffers, uint8_t> buffer_iter =
                boost::asio::buffers_iterator<const_buffers, uint8_t>::begin(buffer);
            for (size_t i = 0; i < count; i++)
            {
                buffer2.get()[i] = (*buffer_iter) ^ send_mask.get()[i % 4];
                buffer_iter++;
            }

            send_buffer.push_back(boost::asio::buffer(buffer2.get(), count));
        }
        else
        {
            const_buffers send_buffer1 = buffers_truncate(buffer, count);
            send_buffer.insert(send_buffer.end(), send_buffer1.begin(), send_buffer1.end());
        }

        boost::mutex::scoped_lock lock(next_layer_lock);
        async_write_buffers = send_buffer;
        next_layer_.async_write_some(send_buffer, boost::bind(&websocket_stream::async_write_message3, this,
                                                              boost::asio::placeholders::bytes_transferred,
                                                              boost::asio::placeholders::error, count, header, buffer2,
                                                              boost::protect(RR_MOVE(handler))));
    }

    void async_write_message3(size_t n, const boost::system::error_code ec, size_t data_count,
                              const boost::shared_array<uint8_t>& header_buf,
                              const boost::shared_array<uint8_t>& buffer2,
                              boost::function<void(const boost::system::error_code&, std::size_t)> handler)
    {
        if (ec || n == 0)
        {
            async_write_buffers.clear();
            handler(ec, 0);
            return;
        }

        if (n < boost::asio::buffer_size(async_write_buffers))
        {
            buffers_consume(async_write_buffers, n);

            boost::mutex::scoped_lock lock(next_layer_lock);
            next_layer_.async_write_some(async_write_buffers,
                                         boost::bind(&websocket_stream::async_write_message3, this,
                                                     boost::asio::placeholders::bytes_transferred,
                                                     boost::asio::placeholders::error, data_count, header_buf, buffer2,
                                                     boost::protect(handler)));
            return;
        }

        async_write_buffers.clear();

        boost::system::error_code ec1;
        handler(ec1, data_count);
    }

    void async_write_message4(const boost::system::error_code& ec, size_t n,
                              const boost::shared_array<uint8_t>& ping_data2, size_t ping_data_len2,
                              size_t ping_data_pos2, boost::asio::const_buffer buffer,
                              boost::function<void(const boost::system::error_code&, std::size_t)> handler)
    {
        RR_UNUSED(ping_data2);
        RR_UNUSED(ping_data_pos2);
        if (ec || (n == 0 && ping_data_len2 != 0))
        {
            handler(ec, 0);
            return;
        }

        if (n != ping_data_len2)
        {
            boost::system::error_code ec1(boost::system::errc::broken_pipe, boost::system::generic_category());
            handler(ec1, 0);
            return;
        }

        const_buffers send_b;
        send_b.push_back(buffer);
        async_write_message(DataFrameType, send_b, handler);
    }

    boost::array<uint8_t, 2> recv_header1;
    boost::array<uint8_t, 12> recv_header2;

    uint64_t recv_frame_length;
    uint64_t recv_frame_pos;
    bool recv_frame_en_mask;
    boost::array<uint8_t, 4> recv_frame_mask;
    uint8_t recv_frame_opcode;

    void async_read_some2(boost::asio::mutable_buffer buf,
                          boost::function<void(const boost::system::error_code&, std::size_t)> handler)
    {

        if (recv_frame_pos > 0 && recv_frame_length > 0)
        {
            size_t c = boost::numeric_cast<size_t>(recv_frame_length - recv_frame_pos);
            if (boost::asio::buffer_size(buf) > c)
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.async_read_some(
                    boost::asio::buffer(buf, c),
                    boost::bind(&websocket_stream::async_read_some5, this, boost::asio::placeholders::bytes_transferred,
                                boost::asio::placeholders::error, buf, boost::protect(handler)));
            }
            else
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.async_read_some(
                    boost::asio::buffer(buf),
                    boost::bind(&websocket_stream::async_read_some5, this, boost::asio::placeholders::bytes_transferred,
                                boost::asio::placeholders::error, buf, boost::protect(handler)));
            }
            return;
        }

        boost::mutex::scoped_lock lock(next_layer_lock);
        next_layer_.async_read_some(boost::asio::buffer(recv_header1.data(), 2),
                                    boost::bind(&websocket_stream::async_read_some3, this,
                                                boost::asio::placeholders::bytes_transferred,
                                                boost::asio::placeholders::error, buf, 0, boost::protect(handler)));
    }

    void async_read_some3(size_t n, const boost::system::error_code& ec, boost::asio::mutable_buffer buf, size_t pos,
                          boost::function<void(const boost::system::error_code&, std::size_t)> handler)
    {
        if (ec || n == 0)
        {
            handler(ec, 0);
            return;
        }

        if (n < 2 - pos)
        {

            size_t pos2 = pos + n;
            boost::mutex::scoped_lock lock(next_layer_lock);
            next_layer_.async_read_some(
                boost::asio::buffer(recv_header1.data() + pos2, 2 - pos2),
                boost::bind(&websocket_stream::async_read_some3, this, boost::asio::placeholders::bytes_transferred,
                            boost::asio::placeholders::error, buf, pos2, boost::protect(handler)));
            return;
        }

        // recv_frame_pos = 0;

        uint8_t header2_len = 0;

        uint8_t opcode_recv1 = recv_header1[0] & 0x0F;
        bool recv_en_mask1 = (recv_header1[1] & 0x80) != 0;
        uint8_t count1 = recv_header1[1] & 0x7F;

        if ((opcode_recv1 == WebSocketOpcode_binary && DataFrameType != WebSocketOpcode_binary) ||
            (opcode_recv1 == WebSocketOpcode_text && DataFrameType != WebSocketOpcode_text))
        {
            // Not currently handling text
            boost::system::error_code ec2(boost::system::errc::protocol_error, boost::system::generic_category());
            handler(ec2, 0);
            return;
        }

        if (count1 == 126)
            header2_len += 2;
        if (count1 == 127)
            header2_len += 8;

        if (recv_en_mask1)
            header2_len += 4;

        if (count1 == 0)
        {
            boost::system::error_code ec1;
            handler(ec1, 0);
            return;
        }

        recv_frame_opcode = opcode_recv1;
        recv_frame_en_mask = recv_en_mask1;
        recv_frame_pos = 0;

        if (header2_len > 0)
        {
            boost::mutex::scoped_lock lock(next_layer_lock);
            next_layer_.async_read_some(boost::asio::buffer(recv_header2.data(), header2_len),
                                        boost::bind(&websocket_stream::async_read_some4, this,
                                                    boost::asio::placeholders::bytes_transferred,
                                                    boost::asio::placeholders::error, opcode_recv1, recv_en_mask1,
                                                    count1, buf, 0, boost::protect(handler)));
            return;
        }
        else
        {
            recv_frame_length = count1;

            if (opcode_recv1 != WebSocketOpcode_continuation && opcode_recv1 != DataFrameType)
            {
                if (recv_frame_length > 4096)
                {
                    boost::system::error_code ec1(boost::system::errc::broken_pipe, boost::system::generic_category());
                    handler(ec1, 0);
                    return;
                }

                boost::shared_array<uint8_t> op_buf(new uint8_t[boost::numeric_cast<size_t>(recv_frame_length)]);

                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.async_read_some(
                    boost::asio::buffer(op_buf.get(), boost::numeric_cast<size_t>(recv_frame_length)),
                    boost::bind(&websocket_stream::async_read_some6, this, boost::asio::placeholders::bytes_transferred,
                                boost::asio::placeholders::error, op_buf,
                                boost::numeric_cast<size_t>(recv_frame_length), 0, buf, boost::protect(handler)));
                return;
            }

            if (boost::asio::buffer_size(buf) > recv_frame_length)
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.async_read_some(
                    boost::asio::buffer(buf, boost::numeric_cast<size_t>(recv_frame_length)),
                    boost::bind(&websocket_stream::async_read_some5, this, boost::asio::placeholders::bytes_transferred,
                                boost::asio::placeholders::error, buf, boost::protect(handler)));
            }
            else
            {
                boost::mutex::scoped_lock lock(next_layer_lock);
                next_layer_.async_read_some(
                    boost::asio::buffer(buf),
                    boost::bind(&websocket_stream::async_read_some5, this, boost::asio::placeholders::bytes_transferred,
                                boost::asio::placeholders::error, buf, boost::protect(handler)));
            }
            return;
        }
    }

    void async_read_some4(size_t n, const boost::system::error_code& ec, uint8_t opcode, bool en_mask, uint8_t count1,
                          boost::asio::mutable_buffer buf, size_t pos,
                          boost::function<void(const boost::system::error_code&, std::size_t)> handler)
    {
        if (ec || n == 0)
        {
            handler(ec, 0);
            return;
        }

        uint8_t header2_len = 0;
        if (count1 == 126)
            header2_len += 2;
        if (count1 == 127)
            header2_len += 8;

        if (en_mask)
            header2_len += 4;

        if (n < header2_len - pos)
        {
            size_t pos2 = pos + n;
            boost::mutex::scoped_lock lock(next_layer_lock);
            next_layer_.async_read_some(boost::asio::buffer(recv_header2.data() + pos2, header2_len - pos2),
                                        boost::bind(&websocket_stream::async_read_some4, this,
                                                    boost::asio::placeholders::bytes_transferred,
                                                    boost::asio::placeholders::error, opcode, en_mask, count1, buf,
                                                    pos2, boost::protect(handler)));
            return;
        }

        if (count1 < 126)
        {
            recv_frame_length = count1;
            if (en_mask)
            {
                memcpy(recv_frame_mask.data(), recv_header2.data(), 4);
            }
        }
        else if (count1 == 126)
        {
#if !BOOST_ENDIAN_BIG_BYTE
            std::reverse(recv_header2.data(), recv_header2.data() + 2);
#endif
            uint16_t r_len = *reinterpret_cast<uint16_t*>(recv_header2.data());
            recv_frame_length = r_len;
            if (en_mask)
            {
                memcpy(recv_frame_mask.data(), recv_header2.data() + 2, 4);
            }
        }
        else
        {
#if !BOOST_ENDIAN_BIG_BYTE
            std::reverse(recv_header2.data(), recv_header2.data() + 8);
#endif
            uint64_t r_len = *reinterpret_cast<uint64_t*>(recv_header2.data());
            recv_frame_length = r_len;
            if (en_mask)
            {
                memcpy(recv_frame_mask.data(), recv_header2.data() + 8, 4);
            }
        }

        if (recv_frame_opcode != WebSocketOpcode_continuation && recv_frame_opcode != DataFrameType)
        {
            if (recv_frame_length > 4096)
            {
                boost::system::error_code ec1(boost::system::errc::broken_pipe, boost::system::generic_category());
                handler(ec1, 0);
                return;
            }

            boost::shared_array<uint8_t> op_buf(new uint8_t[boost::numeric_cast<size_t>(recv_frame_length)]);

            boost::mutex::scoped_lock lock(next_layer_lock);
            next_layer_.async_read_some(
                boost::asio::buffer(op_buf.get(), boost::numeric_cast<size_t>(recv_frame_length)),
                boost::bind(&websocket_stream::async_read_some6, this, boost::asio::placeholders::bytes_transferred,
                            boost::asio::placeholders::error, op_buf, boost::numeric_cast<size_t>(recv_frame_length), 0,
                            buf, boost::protect(handler)));
            return;
        }

        if (boost::asio::buffer_size(buf) > recv_frame_length)
        {
            boost::mutex::scoped_lock lock(next_layer_lock);
            next_layer_.async_read_some(boost::asio::buffer(buf, boost::numeric_cast<size_t>(recv_frame_length)),
                                        boost::bind(&websocket_stream::async_read_some5, this,
                                                    boost::asio::placeholders::bytes_transferred,
                                                    boost::asio::placeholders::error, buf, boost::protect(handler)));
        }
        else
        {
            boost::mutex::scoped_lock lock(next_layer_lock);
            next_layer_.async_read_some(boost::asio::buffer(buf),
                                        boost::bind(&websocket_stream::async_read_some5, this,
                                                    boost::asio::placeholders::bytes_transferred,
                                                    boost::asio::placeholders::error, buf, boost::protect(handler)));
        }
    }

    void async_read_some5(size_t n, const boost::system::error_code& ec, boost::asio::mutable_buffer buf,
                          boost::function<void(const boost::system::error_code&, std::size_t)> handler)
    {
        if (ec || (n == 0 && boost::asio::buffer_size(buf) != 0))
        {
            handler(ec, 0);
            return;
        }

        if (recv_frame_en_mask)
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
            uint8_t* dat = RR_BOOST_ASIO_BUFFER_CAST(uint8_t*, buf);
            for (size_t i = 0; i < n; i++)
            {
                dat[i] = dat[i] ^ recv_frame_mask[(i + recv_frame_pos) % 4];
            }
        }

        recv_frame_pos += n;
        if (recv_frame_pos >= recv_frame_length)
        {
            recv_frame_pos = 0;
            recv_frame_length = 0;
            recv_frame_opcode = 0;
            recv_frame_en_mask = false;
        }

        boost::system::error_code ec1;
        handler(ec1, n);
    }

    void async_read_some6(size_t n, const boost::system::error_code& ec, const boost::shared_array<uint8_t>& op_buf,
                          size_t op_len, size_t op_pos, boost::asio::mutable_buffer buf,
                          boost::function<void(const boost::system::error_code&, std::size_t)> handler)
    {
        if (ec || (n == 0 && op_len != 0))
        {
            if (handler)
            {
                handler(ec, 0);
            }

            return;
        }

        if (n < op_len - op_pos)
        {
            size_t op_pos2 = op_pos + n;
            boost::mutex::scoped_lock lock(next_layer_lock);
            next_layer_.async_read_some(
                boost::asio::buffer(op_buf.get() + op_pos2, op_len - op_pos2),
                boost::bind(&websocket_stream::async_read_some6, this, boost::asio::placeholders::bytes_transferred,
                            boost::asio::placeholders::error, op_buf, op_len, op_pos2, buf, boost::protect(handler)));
        }

        recv_frame_pos = 0;
        recv_frame_length = 0;
        recv_frame_opcode = 0;
        recv_frame_en_mask = false;

        // TODO: handle the opcode...
        try
        {
            switch (recv_frame_opcode)
            {
            case WebSocketOpcode_ping: {
                boost::mutex::scoped_lock lock(ping_lock);
                ping_requested = true;
                ping_data = op_buf;
                ping_data_len = op_len;
            }
            break;
            case WebSocketOpcode_pong:
                // Just ignore...
                break;
            case WebSocketOpcode_close:
            default:
                // Close and return error
                boost::system::error_code ec1(boost::system::errc::broken_pipe, boost::system::generic_category());
                handler(ec1, 0);
                return;
            }
        }
        catch (std::exception&)
        {}

        async_read_some2(buf, handler);
    }

    bool ping_requested;
    boost::shared_array<uint8_t> ping_data;
    size_t ping_data_len;
    boost::mutex ping_lock;

    template <typename Handler>
    class handler_wrapper
    {
      public:
        typedef typename boost::remove_reference<Handler>::type HandlerValueType;

        handler_wrapper(const Handler& handler) : handler_(static_cast<const Handler&>(handler)) {}

        void do_complete(const boost::system::error_code& ec, const std::size_t& bytes_transferred)
        {
            // boost::asio::detail::binder2<Handler, boost::system::error_code, std::size_t>
            // handler(handler_, ec, bytes_transferred);
            // boost_asio_handler_invoke_helpers::invoke(handler, handler.handler_);
            handler_(ec, bytes_transferred);
        }

      private:
        HandlerValueType handler_;
    };

  public:
    void close()
    {
        boost::mutex::scoped_lock lock(next_layer_lock);
        next_layer_.lowest_layer().close();
    }

    template <typename MutableBufferSequence, typename Handler>
    void async_read_some(const MutableBufferSequence& buffers, BOOST_ASIO_MOVE_ARG(Handler) handler)
    {
        if (boost::asio::buffer_size(
                boost::asio::detail::buffer_sequence_adapter<boost::asio::mutable_buffer, MutableBufferSequence>::first(
                    buffers)) == 0)
        {
            // Sometimes the ssl stream will request a zero buffer in order to trigger a callback.
            // This messes up the frame reading.  Pass zero reads on to the next layer.
            boost::mutex::scoped_lock lock(next_layer_lock);
            next_layer_.async_read_some(buffers, handler);
            return;
        }

        // TODO: use more than first buffer
        boost::shared_ptr<handler_wrapper<Handler> > handler2 =
            boost::make_shared<handler_wrapper<Handler> >(boost::ref(handler));
        async_read_some2(
            boost::asio::detail::buffer_sequence_adapter<boost::asio::mutable_buffer, MutableBufferSequence>::first(
                buffers),
            boost::bind(&handler_wrapper<Handler>::do_complete, handler2, RR_BOOST_PLACEHOLDERS(_1),
                        RR_BOOST_PLACEHOLDERS(_2)));
    }

    template <typename Handler>
    void async_write_some(const const_buffers& buffers, BOOST_ASIO_MOVE_ARG(Handler) handler)
    {
        boost::shared_ptr<handler_wrapper<Handler> > handler2 =
            boost::make_shared<handler_wrapper<Handler> >(boost::ref(handler));
        if (ping_requested)
        {
            boost::shared_array<uint8_t> ping_data2;
            size_t ping_data_len2 = 0;
            {
                boost::mutex::scoped_lock lock(ping_lock);
                ping_data2 = ping_data;
                ping_data_len2 = ping_data_len;
                ping_data.reset();
                ping_data_len = 0;
                ping_requested = false;
            }

            const_buffers send_b;
            send_b.push_back(boost::asio::buffer(ping_data2.get(), ping_data_len));

            async_write_message(
                WebSocketOpcode_pong, send_b,
                boost::bind(
                    &websocket_stream::async_write_message4, this, boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred, ping_data2, ping_data_len2, 0,
                    boost::asio::detail::buffer_sequence_adapter<boost::asio::const_buffer, const_buffers>::first(
                        buffers),
                    boost::protect(boost::bind(&handler_wrapper<Handler>::do_complete, handler2,
                                               RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)))));
        }
        else
        {
            // TODO: use more than first buffer
            async_write_message(DataFrameType, buffers,
                                boost::bind(&handler_wrapper<Handler>::do_complete, handler2, RR_BOOST_PLACEHOLDERS(_1),
                                            RR_BOOST_PLACEHOLDERS(_2)));
        }
    }

    template <typename ConstBufferSequence, typename Handler>
    void async_write_some(const ConstBufferSequence& buffers, BOOST_ASIO_MOVE_ARG(Handler) handler)
    {
        const_buffers b;
        boost::range::copy(buffers, std::back_inserter(b));
        async_write_some(b, handler);
    }
};

class websocket_tcp_connector : public boost::enable_shared_from_this<websocket_tcp_connector>
{
  protected:
    RR_BOOST_ASIO_IO_CONTEXT& _io_context_;
    std::string path;
    boost::shared_ptr<boost::asio::ip::tcp::resolver> resolver_;

    std::vector<boost::weak_ptr<boost::asio::ip::tcp::socket> > sockets_;

    boost::mutex handler_lock;
    bool complete;
    bool cancelled;

    boost::shared_ptr<boost::asio::deadline_timer> timeout_timer;

  public:
    websocket_tcp_connector(RR_BOOST_ASIO_IO_CONTEXT& _io_context) : _io_context_(_io_context)
    {
        complete = false;
        cancelled = false;
    }

    void connect(
        const std::string& url,
        boost::function<void(const boost::system::error_code&, const boost::shared_ptr<boost::asio::ip::tcp::socket>&)>
            handler,
        int32_t timeout = 10000)
    {
        boost::smatch url_result;
        boost::regex_search(
            url, url_result,
            boost::regex(
                "^([^:]+)://((?:\\[[A-Fa-f0-9:]+(?:\\%\\w*)?\\])|(?:[^\\[\\]\\:/]+))(?::([^:/\\?]+))?/?(.*)$"));

        if (url_result.size() < 4)
            throw InvalidArgumentException("Malformed URL");

        if (url_result[1] != "ws" && url_result[1] != "wss")
            throw InvalidArgumentException("Invalid transport type for websocket");

        std::string host = url_result[2];
        std::string port_str = url_result[3];

        boost::trim_left_if(host, boost::is_from_range('[', '['));
        boost::trim_right_if(host, boost::is_from_range(']', ']'));

        if (port_str.empty())
        {

            if (url_result[1] == "ws")
                port_str = "80";
            if (url_result[1] == "wss")
                port_str = "443";
        }
        path = url_result[4];

        boost::mutex::scoped_lock lock(handler_lock);

        resolver_ = RR_SHARED_PTR<boost::asio::ip::tcp::resolver>(new boost::asio::ip::tcp::resolver(_io_context_));
#if BOOST_ASIO_VERSION < 101200
        boost::asio::ip::basic_resolver_query<boost::asio::ip::tcp> q(host, port_str,
                                                                      boost::asio::ip::resolver_query_base::flags());
        resolver_->async_resolve(q, boost::bind(&websocket_tcp_connector::connect2, shared_from_this(),
                                                boost::asio::placeholders::error, boost::asio::placeholders::iterator,
                                                boost::protect(handler)));
#else
        resolver_->async_resolve(host, port_str,
                                 boost::bind(&websocket_tcp_connector::connect2, shared_from_this(),
                                             boost::asio::placeholders::error, boost::asio::placeholders::results,
                                             boost::protect(handler)));
#endif

        if (cancelled)
        {
            resolver_->cancel();
            return;
        }

        timeout_timer = RR_SHARED_PTR<boost::asio::deadline_timer>(new boost::asio::deadline_timer(_io_context_));
        timeout_timer->expires_from_now(boost::posix_time::milliseconds(timeout));
        timeout_timer->async_wait(boost::bind(&websocket_tcp_connector::connect4, shared_from_this(),
                                              boost::asio::placeholders::error, boost::protect(handler)));
    }

  protected:
#if BOOST_ASIO_VERSION < 101200
    void connect2(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
                  boost::function<void(const boost::system::error_code&,
                                       const boost::shared_ptr<boost::asio::ip::tcp::socket>& socket)>
                      handler){
#else
    void connect2(const boost::system::error_code& err, const boost::asio::ip::tcp::resolver::results_type& results,
                  boost::function<void(const boost::system::error_code&,
                                       const boost::shared_ptr<boost::asio::ip::tcp::socket>& socket)>
                      handler)
    {
        boost::asio::ip::tcp::resolver::results_type::iterator endpoint_iterator = results.begin();
#endif
        boost::mutex::scoped_lock lock(handler_lock);
    if (complete)
        return;
    if (err)
    {

        complete = true;
        if (timeout_timer)
            timeout_timer->cancel();
        lock.unlock();
        boost::shared_ptr<boost::asio::ip::tcp::socket> empty_s;
        handler(err, empty_s);
        return;
    }

    if (cancelled)
    {
        complete = true;
        if (timeout_timer)
            timeout_timer->cancel();
        lock.unlock();
        boost::shared_ptr<boost::asio::ip::tcp::socket> empty_s;
        handler(boost::asio::error::operation_aborted, empty_s);
        return;
    }

    boost::asio::ip::basic_resolver_iterator<boost::asio::ip::tcp> end;

    if (endpoint_iterator == end)
    {
        complete = true;
        if (timeout_timer)
            timeout_timer->cancel();
        lock.unlock();
        boost::system::error_code ec1(boost::system::errc::bad_address, boost::system::generic_category());
        boost::shared_ptr<boost::asio::ip::tcp::socket> empty_s;
        handler(ec1, empty_s);
        return;
    }

    std::vector<boost::asio::ip::tcp::endpoint> ipv4;
    std::vector<boost::asio::ip::tcp::endpoint> ipv6_1;

    for (; endpoint_iterator != end; endpoint_iterator++)
    {
        if (endpoint_iterator->endpoint().address().is_v4())
            ipv4.push_back(endpoint_iterator->endpoint());
        if (endpoint_iterator->endpoint().address().is_v6())
            ipv6_1.push_back(endpoint_iterator->endpoint());
    }

    std::vector<boost::asio::ip::address> local_ip;
#ifdef ROBOTRACONTEUR_VERSION
    TcpTransport::GetLocalAdapterIPAddresses(local_ip);
#endif

    std::vector<uint32_t> scopeids;
    BOOST_FOREACH (boost::asio::ip::address& ee, local_ip)
    {
        if (ee.is_v6())
        {
            boost::asio::ip::address_v6 a6 = ee.to_v6();
            if (a6.is_link_local())
            {
                if (std::find(scopeids.begin(), scopeids.end(), a6.scope_id()) == scopeids.end())
                {
                    scopeids.push_back(a6.scope_id());
                }
            }
        }
    }

    scopeids.push_back(0);

    std::vector<boost::asio::ip::tcp::endpoint> ipv6;
    BOOST_FOREACH (boost::asio::ip::tcp::endpoint& e, ipv6_1)
    {
        if (!e.address().is_v6())
            continue;

        boost::asio::ip::address_v6 addr = e.address().to_v6();
        uint16_t port = e.port();

        if (!addr.is_link_local() || (addr.is_link_local() && addr.scope_id() != 0))
        {
            ipv6.push_back(e);
        }
        else
        {
            // Link local address with no scope id, we need to try them all...

            BOOST_FOREACH (uint32_t e3, scopeids)
            {
                boost::asio::ip::address_v6 addr3 = addr;
                addr3.scope_id(e3);
                ipv6.push_back(boost::asio::ip::tcp::endpoint(addr3, port));
            }
        }
    }

    std::vector<boost::shared_ptr<boost::asio::ip::tcp::socket> > sockets1;
    size_t socket_count = ipv4.size() + ipv6.size();
    for (size_t i = 0; i < socket_count; i++)
    {
        boost::shared_ptr<boost::asio::ip::tcp::socket> sock(new boost::asio::ip::tcp::socket(_io_context_));
        sockets1.push_back(sock);
        sockets_.push_back(sock);
    }

    size_t k = 0;
    BOOST_FOREACH (boost::asio::ip::tcp::endpoint& e, ipv4)
    {
        boost::shared_ptr<boost::asio::ip::tcp::socket> sock = sockets1.at(k);
        k++;
        sock->async_connect(e, boost::bind(&websocket_tcp_connector::connect3, shared_from_this(), sock,
                                           boost::asio::placeholders::error, boost::protect(handler)));
    }

    BOOST_FOREACH (boost::asio::ip::tcp::endpoint& e, ipv6)
    {
        boost::shared_ptr<boost::asio::ip::tcp::socket> sock = sockets1.at(k);
        k++;
        sock->async_connect(e, boost::bind(&websocket_tcp_connector::connect3, shared_from_this(), sock,
                                           boost::asio::placeholders::error, boost::protect(handler)));
    }
}

	void connect3(const boost::shared_ptr<boost::asio::ip::tcp::socket>& sock, const boost::system::error_code& ec, boost::function<void(const boost::system::error_code&, const boost::shared_ptr<boost::asio::ip::tcp::socket>&)> handler)
{

    boost::mutex::scoped_lock lock(handler_lock);

    if (complete)
        return;

    for (std::vector<boost::weak_ptr<boost::asio::ip::tcp::socket> >::iterator e = sockets_.begin();
         e != sockets_.end();)
    {
        if (e->expired())
        {
            e++;
        }
        else
        {

            boost::shared_ptr<boost::asio::ip::tcp::socket> sock1 = e->lock();
            ;
            if (sock1 == sock)
            {
                e = sockets_.erase(e);
            }
            else
            {
                e++;
            }
        }
    }

    if (ec)
    {
        if (sockets_.empty() && !complete)
        {
            complete = true;
            if (timeout_timer)
                timeout_timer->cancel();
            lock.unlock();
            boost::shared_ptr<boost::asio::ip::tcp::socket> empty_s;
            handler(ec, empty_s);
            return;
        }
        return;
    }

    if (cancelled)
    {
        complete = true;
        if (timeout_timer)
            timeout_timer->cancel();
        lock.unlock();
        boost::shared_ptr<boost::asio::ip::tcp::socket> empty_s;
        handler(boost::asio::error::operation_aborted, empty_s);
        return;
    }

    complete = true;
    if (timeout_timer)
        timeout_timer->cancel();

    lock.unlock();
    boost::system::error_code ec1;
    handler(ec1, sock);
}

void connect4(
    const boost::system::error_code& ec,
    boost::function<void(const boost::system::error_code&, const boost::shared_ptr<boost::asio::ip::tcp::socket>&)>
        handler)
{
    boost::mutex::scoped_lock lock(handler_lock);
    if (complete)
        return;
    complete = true;
    timeout_timer.reset();

    if (ec)
        return;

    lock.unlock();
    boost::system::error_code ec1(boost::system::errc::timed_out, boost::system::generic_category());
    boost::shared_ptr<boost::asio::ip::tcp::socket> empty_s;
    handler(ec1, empty_s);
}

public:
void cancel()
{
    boost::mutex::scoped_lock lock(handler_lock);
    if (complete)
        return;
    cancelled = true;
    resolver_.reset();
    BOOST_FOREACH (RR_WEAK_PTR<boost::asio::ip::tcp::socket>& s, sockets_)
    {
        RR_SHARED_PTR<boost::asio::ip::tcp::socket> s1 = s.lock();
        if (s1)
        {
            s1->close();
        }
    }
    sockets_.clear();
}

}; // namespace detail

} // namespace RobotRaconteur
} // namespace RobotRaconteur
