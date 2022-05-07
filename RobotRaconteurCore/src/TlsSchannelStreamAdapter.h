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

#include <boost/config.hpp>

#if !defined(ROBOTRACONTEUR_USE_SCHANNEL) && !defined(ROBOTRACONTEUR_USE_OPENSSL)
#if defined(ROBOTRACONTEUR_WINDOWS)
#define ROBOTRACONTEUR_USE_SCHANNEL
#else
#define ROBOTRACONTEUR_USE_OPENSSL
#endif
#endif

//#undef ROBOTRACONTEUR_USE_SCHANNEL
//#define ROBOTRACONTEUR_USE_OPENSSL

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
#define SECURITY_WIN32
#endif

#include <string>
#include <vector>
#include <boost/bind/placeholders.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/tuple/tuple.hpp>
#include <queue>
#include <boost/uuid/uuid.hpp>
#include <boost/shared_array.hpp>

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
#define CERT_CHAIN_PARA_HAS_EXTRA_FIELDS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wincrypt.h>
//#include <wintrust.h>
#include <schannel.h>
#include <security.h>
#include <sspi.h>
#endif
#pragma once

#include "RobotRaconteur/NodeID.h"

namespace RobotRaconteur
{
namespace detail
{
class TlsSchannelAsyncStreamAdapter;

class TlsSchannelAsyncStreamAdapterContext : boost::noncopyable
{
  private:
#ifdef ROBOTRACONTEUR_USE_SCHANNEL

    HCERTSTORE activestore;
    PCCERT_CONTEXT activecertificate;
    CredHandle server_credentials;
    CredHandle client_credentials;
    std::vector<HCERTSTORE> stores;
    HCERTSTORE store;
    PCCERT_CONTEXT rootcertificate2015;
    PCCERT_CONTEXT rootcertificate2020;
    bool use_root_cert_2015;
    bool use_root_cert_2020;

#endif
    boost::mutex mylock;
    NodeID nodeid;

  public:
    friend TlsSchannelAsyncStreamAdapter;

    TlsSchannelAsyncStreamAdapterContext(const NodeID& nodeid);
    ~TlsSchannelAsyncStreamAdapterContext();

    CredHandle GetServerCredentials();

    CredHandle GetClientCredentials();

    bool IsCertificateLoaded();

    bool VerifyRemoteNodeCertificate(PCCERT_CONTEXT cert, const NodeID& remote_node);

    bool VerifyRemoteHostnameCertificate(PCCERT_CONTEXT cert, boost::string_ref hostname);

    void LoadCertificateFromMyStore();

  protected:
    bool VerifyCertificateOIDExtension(PCERT_INFO cert1, boost::string_ref searchoid);
};

class TlsSchannelAsyncStreamAdapter;

// This class is necessary due to the infurating behavior of ASIO templates
class TlsSchannelAsyncStreamAdapter_ASIO_adapter
{
    TlsSchannelAsyncStreamAdapter& next_layer_;

    template <typename Handler>
    class handler_wrapper
    {
      public:
        handler_wrapper(Handler& handler) : handler_(handler) {}

        void do_complete(const boost::system::error_code& ec, const std::size_t& bytes_transferred)
        {
            // boost::asio::detail::binder2<Handler, boost::system::error_code, std::size_t>
            // handler1(handler_, ec, bytes_transferred);
            // boost_asio_handler_invoke_helpers::invoke(handler1, handler1.handler_);

            handler_(ec, bytes_transferred);
        }

      private:
        Handler handler_;
    };

  public:
    TlsSchannelAsyncStreamAdapter_ASIO_adapter(TlsSchannelAsyncStreamAdapter& next_layer) : next_layer_(next_layer) {}

    template <typename MutableBufferSequence, typename Handler>
    void async_read_some(const MutableBufferSequence& buffers, BOOST_ASIO_MOVE_ARG(Handler) handler)
    {

        // TODO: Don't allocate here
        boost::shared_ptr<handler_wrapper<Handler> > handler2 =
            boost::make_shared<handler_wrapper<Handler> >(boost::ref(handler));

        // TODO: use more than just first buffer
        mutable_buffers b;
        b.push_back(
            boost::asio::detail::buffer_sequence_adapter<boost::asio::mutable_buffer, MutableBufferSequence>::first(
                buffers));
        next_layer_.async_read_some(b, boost::bind(&handler_wrapper<Handler>::do_complete, handler2,
                                                   RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
        return;
    }

    template <typename ConstBufferSequence, typename Handler>
    void async_write_some(const ConstBufferSequence& buffers, BOOST_ASIO_MOVE_ARG(Handler) handler)
    {
        // TODO: Don't allocate here
        boost::shared_ptr<handler_wrapper<Handler> > handler2 = boost::make_shared<handler_wrapper<Handler> >(handler);
        // TODO: use more than just first buffer
        const_buffers b;
        b.push_back(boost::asio::detail::buffer_sequence_adapter<boost::asio::const_buffer, ConstBufferSequence>::first(
            buffers));
        next_layer_.async_write_some(b, boost::bind(&handler_wrapper<Handler>::do_complete, handler2,
                                                    RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));
        return;
    }

    void close();

    TlsSchannelAsyncStreamAdapter& lowest_layer() { return next_layer_; }
};

class TlsSchannelAsyncStreamAdapter : public boost::enable_shared_from_this<TlsSchannelAsyncStreamAdapter>,
                                      boost::noncopyable
{

  public:
    enum direction_type
    {
        null,
        client,
        server
    };

    enum
    {
        max_tls_record_size = 24 * 1024
    };

  protected:
    boost::function<void(const_buffers&,
                         boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>)>
        _async_write_some;

    boost::function<void(mutable_buffers&,
                         boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>)>
        _async_read_some;

    boost::function<void()> _close;

    boost::mutex stream_lock;

    bool open;

    direction_type direction;

    RR_BOOST_ASIO_IO_CONTEXT& _io_context;

    std::string servername;

    boost::shared_array<uint8_t> recv_buffer;
    boost::shared_array<uint8_t> send_buffer;

    uint32_t send_buffer_end_pos;
    uint32_t recv_buffer_end_pos;
    uint32_t send_buffer_transfer_pos;
    // uint32_t recv_buffer_transfer_pos;

    bool reading;
    bool writing;

    bool request_shutdown;
    bool request_renegotiate;

    bool handshaking;
    bool shutingdown;

    bool mutual_auth;

    TlsSchannelAsyncStreamAdapter_ASIO_adapter asio_adapter;

#ifdef ROBOTRACONTEUR_USE_SCHANNEL

    boost::shared_ptr<CredHandle> hCreds;
    boost::shared_ptr<CtxtHandle> hContext;
    SecPkgContext_StreamSizes TlsStreamSizes;
    boost::shared_array<uint8_t> recv_buffer_un;
    uint32_t recv_buffer_un_end_pos;

    boost::shared_ptr<TlsSchannelAsyncStreamAdapterContext> context;

    static void release_context(PCtxtHandle phContext);
    static void release_credentials(PCredHandle phContext);

    void do_handshake1(const boost::system::error_code& error, size_t bytes_transferred,
                       boost::function<void(const boost::system::error_code&)> handler);
    void do_handshake2(const boost::system::error_code& error, size_t bytes_transferred,
                       boost::function<void(const boost::system::error_code&)> handler, bool doread);
    void do_handshake3(const boost::system::error_code& error, size_t bytes_transferred,
                       boost::function<void(const boost::system::error_code&)> handler, bool doread);
    void do_handshake4(const boost::system::error_code& error, size_t bytes_transferred,
                       boost::function<void(const boost::system::error_code&)> handler);
    void do_handshake5(boost::function<void(const boost::system::error_code&)> handler);
    void do_handshake6(const boost::system::error_code& error,
                       boost::function<void(const boost::system::error_code&)> handler);

    void async_write_some1(const boost::system::error_code& error, size_t bytes_transferred, size_t len,
                           const boost::function<void(const boost::system::error_code&, size_t)>& handler);
    void async_write_some2(const boost::system::error_code& error,
                           const boost::function<void(const boost::system::error_code&, size_t)>& handler);

    void async_read_some1(boost::asio::mutable_buffer& b, const boost::system::error_code& error,
                          size_t bytes_transferred,
                          const boost::function<void(const boost::system::error_code&, size_t)>& handler);

    void async_read_some2(const boost::system::error_code& error, boost::asio::mutable_buffer& b,
                          const boost::function<void(const boost::system::error_code&, size_t)>& handler);

    void do_shutdown1();
    void do_shutdown2(const boost::system::error_code& error, size_t bytes_transferred);
    void do_shutdown3(const boost::system::error_code& error);

    boost::function<void(const boost::system::error_code&)> async_shutdown_handler_op;
    boost::function<void()> async_shutdown_handler_rd;
    boost::function<void()> async_write_op;
    boost::function<void(const boost::system::error_code&)> async_handshake_handler_op;

#endif

  public:
    TlsSchannelAsyncStreamAdapter(
        RR_BOOST_ASIO_IO_CONTEXT& _io_context, const boost::shared_ptr<TlsSchannelAsyncStreamAdapterContext>& context,
        direction_type direction, boost::string_ref servername,
        boost::function<void(mutable_buffers&,
                             boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>)>
            async_read_some,
        boost::function<void(const_buffers&,
                             boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>)>
            async_write_some,
        boost::function<void()> close);
    ~TlsSchannelAsyncStreamAdapter();

    virtual void async_handshake(boost::function<void(const boost::system::error_code&)> handler);

    virtual void async_write_some(const_buffers& b,
                                  const boost::function<void(const boost::system::error_code&, size_t)>& handler);

    virtual void async_read_some(mutable_buffers& b,
                                 const boost::function<void(const boost::system::error_code&, size_t)>& handler);

    virtual void async_shutdown(boost::function<void(const boost::system::error_code&)> handler);

    virtual void close();

    virtual bool VerifyRemoteNodeCertificate(const NodeID& remote_node);

    virtual bool VerifyRemoteHostnameCertificate(boost::string_ref hostname);

    virtual boost::tuple<std::string, std::string> GetTlsPublicKeys();

    bool get_mutual_auth();
    void set_mutual_auth(bool mutual_auth);

    TlsSchannelAsyncStreamAdapter_ASIO_adapter& get_asio_adapter() { return asio_adapter; }
};
} // namespace detail
} // namespace RobotRaconteur
