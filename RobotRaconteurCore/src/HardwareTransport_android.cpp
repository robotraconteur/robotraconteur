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

#include "HardwareTransport_android_private.h"

#include "RobotRaconteur/IOUtils.h"

namespace RobotRaconteur
{
namespace detail
{

void AndroidHardwareDirector::SetDirector(AndroidHardwareDirector* d)
{
    boost::mutex::scoped_lock lock(director_lock);
    if (director == NULL)
    {
        director = d;
    }
}

void AndroidHardwareDirector::ClearDirector(AndroidHardwareDirector* d)
{
    boost::mutex::scoped_lock lock(director_lock);
    if (d == director)
    {
        director = NULL;
    }
}

AndroidHardwareDirector::AndroidHardwareDirector() {}
AndroidHardwareDirector::~AndroidHardwareDirector() {}

void AndroidHardwareDirector::ConnectBluetooth(
    const RR_SHARED_PTR<HardwareTransport>& parent, const ParseConnectionURLResult& url, boost::string_ref noden,
    uint32_t endpoint,
    boost::function<void(const RR_SHARED_PTR<ITransportConnection>&, const RR_SHARED_PTR<RobotRaconteurException>&)>
        handler)
{

    boost::mutex::scoped_lock lock(director_lock);
    if (!director)
    {
        RobotRaconteurNode::TryPostToThreadPool(
            parent->GetNode(),
            boost::bind(handler, RR_SHARED_PTR<ITransportConnection>(),
                        RR_MAKE_SHARED<ConnectionException>("Internal bluetooth error")),
            true);
        return;
    }

    RR_SHARED_PTR<AndroidBluetoothConnector_params> p = RR_MAKE_SHARED<AndroidBluetoothConnector_params>();
    p->parent = parent;
    p->target_nodeid = url.nodeid;
    p->target_nodename = url.nodename;
    p->noden = noden;
    p->endpoint = endpoint;
    p->handler = handler;

    director->ConnectBluetooth2(&p);
}

AndroidHardwareDirector* AndroidHardwareDirector::director = NULL;
boost::mutex AndroidHardwareDirector::director_lock;

int32_t AndroidHardwareHelper::ConnectBluetooth_success(const RR_SHARED_PTR<AndroidBluetoothConnector_params>& p)
{
    if (!p)
        throw InvalidOperationException("");

    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0)
    {
        return 0;
    }

    boost::asio::generic::stream_protocol protocol(AF_UNIX, SOCK_STREAM);

    RR_SHARED_PTR<boost::asio::generic::stream_protocol::socket> sock =
        RR_MAKE_SHARED<boost::asio::generic::stream_protocol::socket>(
            boost::ref(p->parent->GetNode()->GetThreadPool()->get_io_context()), protocol, fds[0]);

    RR_SHARED_PTR<HardwareTransportConnection_bluetooth> c =
        RR_MAKE_SHARED<HardwareTransportConnection_bluetooth>(p->parent, false, p->endpoint);
    c->AsyncAttachSocket(sock, p->noden, boost::bind(p->handler, c, RR_BOOST_PLACEHOLDERS(_1)));

    std::cout << "Got a connection success" << std::endl;
    return fds[1];
}

void AndroidHardwareHelper::ConnectBluetooth_error(const RR_SHARED_PTR<AndroidBluetoothConnector_params>& p,
                                                   boost::string_ref message)
{
    RobotRaconteurNode::TryPostToThreadPool(
        p->parent->GetNode(),
        boost::bind(p->handler, RR_SHARED_PTR<ITransportConnection>(), RR_MAKE_SHARED<ConnectionException>(message)),
        true);
}

RR_SHARED_PTR<AndroidBluetoothConnector_params> AndroidHardwareHelper::VoidToAndroidBluetoothConnector_params(void* p)
{
    return RR_SHARED_PTR<AndroidBluetoothConnector_params>(
        *static_cast<RR_SHARED_PTR<AndroidBluetoothConnector_params>*>(p));
}

std::vector<int8_t> AndroidHardwareHelper::MessageToVector(const RR_INTRUSIVE_PTR<Message>& m)
{
    if (!m)
        throw InvalidArgumentException("");

    size_t s = m->ComputeSize();
    std::vector<int8_t> o;
    o.resize(s);
    ArrayBinaryWriter w((uint8_t*)&o[0], 0, s);
    m->Write(w);
    return o;
}
RR_INTRUSIVE_PTR<Message> AndroidHardwareHelper::VectorToMessage(std::vector<int8_t> v)
{
    RR_INTRUSIVE_PTR<Message> o = CreateMessage();
    ArrayBinaryReader r((uint8_t*)&v[0], 0, v.size());
    o->Read(r);
    return o;
}

} // namespace detail
} // namespace RobotRaconteur
