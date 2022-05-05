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

#include <boost/asio/ssl.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "RobotRaconteur/NodeID.h"

#pragma once

namespace RobotRaconteur
{
namespace detail
{
class OpenSSLAuthContext : boost::noncopyable
{
  protected:
    boost::shared_ptr<boost::asio::ssl::context> client_context;
    boost::shared_ptr<boost::asio::ssl::context> server_context;
    boost::mutex mylock;
    NodeID nodeid;

    boost::shared_ptr<void> p12_ca;
    boost::shared_ptr<X509> p12_cert;
    boost::shared_ptr<EVP_PKEY> p12_key;

    static void InitCA(boost::shared_ptr<boost::asio::ssl::context> context);

  public:
    OpenSSLAuthContext(const NodeID& nodeid);

    void LoadPKCS12FromBuffer(boost::asio::mutable_buffer& buf);

    void LoadPKCS12FromFile(boost::string_ref fname);

    boost::shared_ptr<boost::asio::ssl::context> GetServerCredentials();

    boost::shared_ptr<boost::asio::ssl::context> GetClientCredentials();

    bool IsCertificateLoaded();

    bool VerifyRemoteNodeCertificate(SSL* connection, const NodeID& remote_node);
};
} // namespace detail
} // namespace RobotRaconteur
