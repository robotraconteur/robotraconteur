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

#include "HardwareTransport_private.h"

#include <boost/foreach.hpp>

#pragma once

#ifndef WINSOCK_VERSION
#ifndef SOCKET
#define SOCKET int
#endif
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

namespace RobotRaconteur
{
namespace detail
{

template <typename T>
static T BluetoothConnector_record_read_number(boost::asio::mutable_buffer& buf)
{
    T num;
    if (boost::asio::buffer_copy(boost::asio::buffer(&num, sizeof(T)), buf) != sizeof(T))
    {
        throw InvalidArgumentException("Invalid bluetooth header");
    }

    buf = buf + sizeof(T);
#if BOOST_ENDIAN_LITTLE_BYTE
    std::reverse(reinterpret_cast<uint8_t*>(&num), reinterpret_cast<uint8_t*>(&num) + sizeof(T));
#endif

    return num;
}

static boost::tuple<uint8_t, size_t> BluetoothConnector_record_read_header(boost::asio::mutable_buffer& buf)
{
    uint8_t hdr = BluetoothConnector_record_read_number<uint8_t>(buf);

    uint8_t type = hdr >> 3;
    uint8_t idx = hdr & 0x07;

    switch (idx)
    {
    case 0:
        return boost::make_tuple(type, static_cast<size_t>(1));
    case 1:
        return boost::make_tuple(type, static_cast<size_t>(2));
    case 2:
        return boost::make_tuple(type, static_cast<size_t>(4));
    case 3:
        return boost::make_tuple(type, static_cast<size_t>(8));
    case 4:
        return boost::make_tuple(type, static_cast<size_t>(16));
    case 5: {
        uint8_t len = BluetoothConnector_record_read_number<uint8_t>(buf);
        return boost::make_tuple(type, boost::numeric_cast<size_t>(len));
    }
    case 6: {
        uint16_t len = BluetoothConnector_record_read_number<uint16_t>(buf);
        return boost::make_tuple(type, boost::numeric_cast<size_t>(len));
    }
    case 7: {
        uint32_t len = BluetoothConnector_record_read_number<uint32_t>(buf);
        return boost::make_tuple(type, boost::numeric_cast<size_t>(len));
    }
    default:
        throw InvalidArgumentException("Invalid bluetooth header");
    }
}

template <typename btaddr, int family, int proto>
class BluetoothConnector : public RR_ENABLE_SHARED_FROM_THIS<BluetoothConnector<btaddr, family, proto> >
{
  public:
    typedef btaddr btaddr_type;

    struct device_info
    {
        boost::optional<std::string> nodeid_str;
        boost::optional<std::string> nodename_str;
        btaddr addr;
    };

    BluetoothConnector(RR_SHARED_PTR<HardwareTransport> parent)
    {
        this->parent = parent;
        this->connecting = false;
        this->endpoint = 0;
        node = parent->GetNode();
    }

    virtual ~BluetoothConnector() {}

    void Connect(
        const ParseConnectionURLResult& url, boost::string_ref noden, uint32_t endpoint,
        boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler)
    {
        target_nodeid = url.nodeid;
        target_nodename = url.nodename;

        this->endpoint = endpoint;
        this->noden = RR_MOVE(noden.to_string());

        this->connecting = true;

        RobotRaconteurNode::TryPostToThreadPool(
            node, boost::bind(&BluetoothConnector::DoConnect, this->shared_from_this(), boost::protect(handler)), true);
    }

    void UpdateDevices()
    {
        devices = GetDeviceAddresses();
        return;
    }

    std::list<btaddr> FindMatchingServices(btaddr addr, const NodeID& nodeid, boost::string_ref nodename)
    {
        std::list<device_info> infos = GetDeviceNodes(addr);
        std::list<btaddr> o;
        BOOST_FOREACH (device_info& info, infos)
        {
            try
            {
                bool match = false;
                if (!nodeid.IsAnyNode() && !nodename.empty())
                {
                    if (info.nodeid_str && info.nodename_str)
                    {
                        NodeID device_nodeid(*info.nodeid_str);
                        if (nodeid == device_nodeid && nodename == *info.nodename_str)
                        {
                            match = true;
                        }
                    }
                }
                else if (!nodename.empty())
                {
                    if (info.nodename_str)
                    {
                        if (nodename == *info.nodename_str)
                        {
                            match = true;
                        }
                    }
                }
                else if (!nodeid.IsAnyNode())
                {
                    if (info.nodeid_str)
                    {
                        NodeID device_nodeid(*info.nodeid_str);
                        if (nodeid == device_nodeid)
                        {
                            match = true;
                        }
                    }
                }

                if (match)
                {
                    o.push_back(info.addr);
                }
            }
            catch (std::exception&)
            {}
        }

        return o;
    }

    void DoConnect(
        boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler)
    {
        boost::mutex::scoped_lock lock(this_lock);

        try
        {
            UpdateDevices();
        }
        catch (std::exception& err)
        {
            handler(RR_SHARED_PTR<ITransportConnection>(), RR_MAKE_SHARED<ConnectionException>(err.what()));
            return;
        }

        if (devices.empty())
        {
            handler(RR_SHARED_PTR<ITransportConnection>(),
                    RR_MAKE_SHARED<ConnectionException>("Bluetooth node not found"));
            return;
        }

        int32_t key = 0;

        BOOST_FOREACH (btaddr& a1, devices)
        {
            boost::thread(boost::bind(&BluetoothConnector::DoConnect1, this->shared_from_this(), a1,
                                      boost::protect(handler), key));
            active_keys.push_back(key);
            key++;
        }
    }

    void DoConnect1(
        btaddr addr,
        boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler,
        int32_t key)
    {
        {
            boost::mutex::scoped_lock lock(this_lock);
            if (!connecting)
                return;
        }

        std::list<btaddr> a2 = FindMatchingServices(addr, target_nodeid, target_nodename);

        if (a2.empty())
        {
            DoConnect_err(RR_MAKE_SHARED<ConnectionException>("Bluetooth node not found"), handler, key);
            return;
        }

        RR_SHARED_PTR<boost::asio::generic::stream_protocol::socket> sock;
        RR_SHARED_PTR<HardwareTransportConnection_bluetooth> c;

        try
        {
            BOOST_FOREACH (btaddr& a1, a2)
            {
                {
                    boost::mutex::scoped_lock lock(this_lock);
                    if (!connecting)
                        return;
                }

                SOCKET s1 = socket(family, SOCK_STREAM, proto);
                if (s1 == INVALID_SOCKET)
                {
                    continue;
                }

                int res = connect(s1, reinterpret_cast<const sockaddr*>(&a1), sizeof(a1));
                if (res == SOCKET_ERROR)
                {
                    continue;
                }

                boost::asio::generic::stream_protocol protocol(family, proto);
#if BOOST_ASIO_VERSION < 101400
                RR_BOOST_ASIO_IO_CONTEXT& ex = parent->GetNode()->GetThreadPool()->get_io_context();
#else
                const boost::asio::generic::stream_protocol::socket::executor_type& ex =
                    parent->GetNode()->GetThreadPool()->get_io_context().get_executor();
#endif
                sock.reset(new boost::asio::generic::stream_protocol::socket(ex, protocol, s1));
                break;
            }
        }
        catch (std::exception& exp)
        {
            DoConnect_err(RR_MAKE_SHARED<ConnectionException>(exp.what()), handler, key);
            return;
        }

        if (!sock)
        {
            DoConnect_err(RR_MAKE_SHARED<ConnectionException>("Could not connect to Bluetooth socket"), handler, key);
            return;
        }

        try
        {
            boost::mutex::scoped_lock lock(this_lock);
            if (!connecting)
                return;
            connecting = false;

            c = RR_MAKE_SHARED<HardwareTransportConnection_bluetooth>(parent, false, endpoint);
            boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)> h =
                boost::bind(handler, c, RR_BOOST_PLACEHOLDERS(_1));
            c->AsyncAttachSocket(sock, noden, h);
            parent->AddCloseListener(c, &HardwareTransportConnection_bluetooth::Close);
        }
        catch (std::exception&)
        {
            handler(RR_SHARED_PTR<ITransportConnection>(),
                    RR_MAKE_SHARED<ConnectionException>("Could not initialize Bluetooth connection"));
        }
    }

    void DoConnect_err(
        RR_SHARED_PTR<RobotRaconteurException> exp,
        boost::function<void(RR_SHARED_PTR<ITransportConnection>, RR_SHARED_PTR<RobotRaconteurException>)> handler,
        int32_t key)
    {
        boost::mutex::scoped_lock lock(this_lock);
        if (!connecting)
            return;

        active_keys.remove(key);

        if (active_keys.empty())
        {
            connecting = false;
            lock.unlock();
            handler(RR_SHARED_PTR<ITransportConnection>(), exp);
        }
    }

    virtual std::list<btaddr> GetDeviceAddresses() = 0;

    virtual std::list<device_info> GetDeviceNodes(btaddr addr) = 0;

    bool ReadSdpRecord(boost::asio::mutable_buffer& raw_record, device_info& info)
    {
        try
        {
            boost::tuple<uint8_t, size_t> record_h = BluetoothConnector_record_read_header(raw_record);

            if (record_h.get<1>() != boost::asio::buffer_size(raw_record))
                return false;

            while (boost::asio::buffer_size(raw_record) > 0)
            {
                boost::tuple<uint8_t, size_t> entry_h = BluetoothConnector_record_read_header(raw_record);
                if (entry_h.get<0>() != 1 || entry_h.get<1>() != 2)
                    break;

                uint16_t data_id = BluetoothConnector_record_read_number<uint16_t>(raw_record);

                boost::tuple<uint8_t, size_t> data_h = BluetoothConnector_record_read_header(raw_record);

                if (data_h.get<1>() == 0)
                    break;

                if ((data_id == 0xF001 || data_id == 0xF002) && data_h.get<0>() == 4)
                {
                    std::string s;
                    if (data_h.get<1>() > 512)
                        continue;
                    s.resize(data_h.get<1>());
                    if (boost::asio::buffer_copy(boost::asio::buffer(&s[0], data_h.get<1>()), raw_record) !=
                        data_h.get<1>())
                    {
                        break;
                    }

                    raw_record = raw_record + data_h.get<1>();

                    if (data_id == 0xF001)
                    {
                        info.nodeid_str = s;
                    }
                    if (data_id == 0xF002)
                    {
                        info.nodename_str = s;
                    }
                }
                else
                {
                    raw_record = raw_record + data_h.get<1>();
                    if (boost::asio::buffer_size(raw_record) == 0)
                    {
                        break;
                    }
                }
            }
        }
        catch (std::exception&)
        {}

        return true;
    }

  protected:
    NodeID target_nodeid;
    std::string target_nodename;
    std::string noden;

    uint32_t endpoint;

    RR_SHARED_PTR<HardwareTransport> parent;

    std::list<btaddr> devices;

    boost::mutex this_lock;
    std::list<int32_t> active_keys;

    bool connecting;

    RR_SHARED_PTR<RobotRaconteurNode> node;
};

} // namespace detail
} // namespace RobotRaconteur
