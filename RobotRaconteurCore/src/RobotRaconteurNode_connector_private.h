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

#include "RobotRaconteur/RobotRaconteurNode.h"

#pragma once

namespace RobotRaconteur
{
namespace detail
{
class RobotRaconteurNode_connector : public RR_ENABLE_SHARED_FROM_THIS<RobotRaconteurNode_connector>
{
  protected:
    boost::mutex active_lock;
    std::list<int32_t> active;
    int32_t active_count;
    std::map<std::string, RR_WEAK_PTR<Transport> > connectors;
    std::string username;
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials;
    boost::function<void(RR_SHARED_PTR<ClientContext>, ClientServiceListenerEventType, RR_SHARED_PTR<void>)> listener;
    std::string objecttype;
    boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> handler;
    int32_t timeout;
    boost::mutex connecting_lock;
    bool connecting;
    bool transport_connected;
    std::list<RR_SHARED_PTR<RobotRaconteurException> > errors;

    RR_SHARED_PTR<boost::asio::deadline_timer> connect_timer;
    boost::mutex connect_timer_lock;

    RR_SHARED_PTR<RobotRaconteurNode> node;

    boost::mutex handler_lock;
    RR_SHARED_PTR<boost::asio::deadline_timer> connect_backoff_timer;

    class endpoint_cleanup
    {
      public:
        boost::mutex eplock;
        RR_SHARED_PTR<ClientContext> ep;
        RR_SHARED_PTR<RobotRaconteurNode> node;

        endpoint_cleanup(RR_SHARED_PTR<ClientContext> ep, RR_SHARED_PTR<RobotRaconteurNode> node);

        virtual ~endpoint_cleanup();

        void release();
    };

  public:
    RobotRaconteurNode_connector(RR_SHARED_PTR<RobotRaconteurNode> node);

  protected:
    void handle_error(const int32_t& key, RR_SHARED_PTR<RobotRaconteurException> err);

    void connected_client(RR_SHARED_PTR<RRObject> client, RR_SHARED_PTR<RobotRaconteurException> err, std::string url,
                          RR_SHARED_PTR<endpoint_cleanup> ep, int32_t key);

    void connected_transport(RR_SHARED_PTR<Transport> transport, RR_SHARED_PTR<ITransportConnection> connection,
                             RR_SHARED_PTR<RobotRaconteurException> err, std::string url,
                             RR_SHARED_PTR<endpoint_cleanup> ep, int32_t key);

    void connect_timer_callback(const boost::system::error_code& ec);

    void connect2(RR_SHARED_PTR<std::vector<std::string> > urls, int32_t main_key, const boost::system::error_code& ec);

    void start_connect_timer();

  public:
    void connect(
        const std::map<std::string, RR_WEAK_PTR<Transport> >& connectors, boost::string_ref username,
        RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > credentials,
        boost::function<void(RR_SHARED_PTR<ClientContext>, ClientServiceListenerEventType, RR_SHARED_PTR<void>)>
            listener,
        boost::string_ref objecttype,
        boost::function<void(RR_SHARED_PTR<RRObject>, RR_SHARED_PTR<RobotRaconteurException>)> handler,
        int32_t timeout);
};

} // namespace detail

} // namespace RobotRaconteur