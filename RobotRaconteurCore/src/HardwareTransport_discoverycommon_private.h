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
#include <boost/range/algorithm.hpp>

#pragma once

namespace RobotRaconteur
{
namespace detail
{
template <typename usb_manager, typename bluetooth_connector>
class HardwareTransport_discovery
    : public RR_ENABLE_SHARED_FROM_THIS<HardwareTransport_discovery<usb_manager, bluetooth_connector> >
{
  public:
    class refresh_op
    {
      public:
        boost::mutex this_lock;
        bool handled;
        boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)> handler;
        int32_t count;
        RR_SHARED_PTR<boost::asio::deadline_timer> timer;
        RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > ret;

        refresh_op() : handled(false), count(0) {}
    };

    HardwareTransport_discovery(RR_SHARED_PTR<HardwareTransport> parent, const std::vector<std::string>& schemes,
                                RR_SHARED_PTR<usb_manager> usb, RR_SHARED_PTR<bluetooth_connector> bt)
    {
        this->parent = parent;
        this->usb = usb;
        this->bt = bt;
        this->schemes = schemes;
        this->node = parent->GetNode();
    }

    virtual ~HardwareTransport_discovery() {}

    RR_SHARED_PTR<HardwareTransport> GetParent()
    {
        RR_SHARED_PTR<HardwareTransport> p = parent.lock();
        if (!p)
            throw InvalidOperationException("HardwareTransport closed");
        return p;
    }

    virtual void Init() {}

    virtual void Close() {}

    virtual std::vector<NodeDiscoveryInfo> GetDriverDevices() = 0;

    void GetUsbDevices(boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)> handler)
    {
        try
        {
            RR_SHARED_PTR<usb_manager> m = usb.lock();
            if (!m)
                return;

            m->UpdateDevices(boost::bind(&HardwareTransport_discovery::GetUsbDevices1, this->shared_from_this(),
                                         boost::protect(handler)));
        }
        catch (std::exception&)
        {
            handler(RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >());
        }
    }

    void GetBluetoothDevices(boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)> handler)
    {
        if (!bt)
        {
            handler(RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >());
            return;
        }

        std::list<typename bluetooth_connector::btaddr_type> addr = bt->GetDeviceAddresses();

        if (addr.empty())
        {
            handler(RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >());
            return;
        }

        RR_SHARED_PTR<refresh_op> op = RR_MAKE_SHARED<refresh_op>();
        op->handler = handler;
        op->ret = RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();

        boost::mutex::scoped_lock lock(op->this_lock);

        BOOST_FOREACH (typename bluetooth_connector::btaddr_type a, addr)
        {
            if (RobotRaconteurNode::TryPostToThreadPool(
                    node,
                    boost::bind(&HardwareTransport_discovery::GetBluetoothDevices1, this->shared_from_this(), op, a)))
            {
                op->count++;
            }
        }

        if (op->count == 0)
        {
            lock.unlock();
            handler(RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >());
            return;
        }
    }

    void GetAll(boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)> handler, int32_t timeout)
    {
        // TODO: Test this function

        RR_SHARED_PTR<RobotRaconteurNode> n = GetParent()->GetNode();
        RR_SHARED_PTR<ThreadPool> p = n->GetThreadPool();

        RR_SHARED_PTR<refresh_op> op = RR_MAKE_SHARED<refresh_op>();
        op->handler = handler;
        op->ret = RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();

        std::vector<NodeDiscoveryInfo> v1 = GetDriverDevices();
        boost::range::copy(v1, std::back_inserter(*op->ret));

        boost::mutex::scoped_lock op_lock(op->this_lock);

        op->timer.reset(new boost::asio::deadline_timer(p->get_io_context()));
        if (timeout >= 0)
        {
            op->timer->expires_from_now(boost::posix_time::milliseconds(timeout));
            RobotRaconteurNode::asio_async_wait(node, op->timer,
                                                boost::bind(&HardwareTransport_discovery::GetAll2,
                                                            this->shared_from_this(), boost::asio::placeholders::error,
                                                            op));
        }

        RR_SHARED_PTR<usb_manager> u = usb.lock();

        if (boost::range::find(schemes, "rr+usb") != schemes.end())
        {
            if (u)
            {
                if (RobotRaconteurNode::TryPostToThreadPool(
                        n, boost::bind(
                               &HardwareTransport_discovery::GetUsbDevices, this->shared_from_this(),
                               boost::protect(boost::bind(&HardwareTransport_discovery::GetAll1,
                                                          this->shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), op)))))
                {
                    op->count++;
                }
            }
        }

        if (boost::range::find(schemes, "rr+usb") != schemes.end())
        {
            if (RobotRaconteurNode::TryPostToThreadPool(
                    n, boost::bind(&HardwareTransport_discovery::GetBluetoothDevices2, this->shared_from_this(), op)))
            {
                op->count++;
            }
        }

        if (op->count == 0)
        {
            RobotRaconteurNode::TryPostToThreadPool(n, boost::bind(op->handler, op->ret), true);
        }
    }

  protected:
    void GetUsbDevices1(boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)> handler)
    {
        RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > o = RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();
        try
        {
            RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();
            RR_SHARED_PTR<usb_manager> m = usb.lock();
            if (!m)
                return;

            std::list<boost::tuple<NodeID, std::string> > usb_dev = m->GetDetectedDevices();
            typedef boost::tuple<NodeID, std::string> e_type;

            BOOST_FOREACH (e_type& e, usb_dev)
            {
                NodeDiscoveryInfo n;
                n.NodeID = e.get<0>();
                n.NodeName = e.get<1>();
                NodeDiscoveryInfoURL n1;
                n1.URL = "rr+usb:///?nodeid=" + e.get<0>().ToString("D") + "&service=RobotRaconteurServiceIndex";
                n1.LastAnnounceTime = node->NowNodeTime();
                n.URLs.push_back(n1);
                o->push_back(n);
            }
        }
        catch (std::exception&)
        {}

        handler(o);
    }

    void GetBluetoothDevices1(RR_SHARED_PTR<refresh_op> op, typename bluetooth_connector::btaddr_type addr)
    {
        RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();

        std::list<typename bluetooth_connector::device_info> d = bt->GetDeviceNodes(addr);

        boost::mutex::scoped_lock lock(op->this_lock);

        BOOST_FOREACH (typename bluetooth_connector::device_info& e, d)
        {
            if (!e.nodeid_str || !e.nodename_str)
                continue;

            NodeDiscoveryInfo n;
            n.NodeID = NodeID(*e.nodeid_str);
            n.NodeName = *e.nodeid_str;
            NodeDiscoveryInfoURL n1;
            n1.URL = "rr+bluetooth:///?nodeid=" + n.NodeID.ToString("D") + "&service=RobotRaconteurServiceIndex";
            n1.LastAnnounceTime = node->NowNodeTime();
            n.URLs.push_back(n1);
            op->ret->push_back(n);
        }

        op->count--;
        if (op->count > 0)
            return;

        op->handled = true;
        boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)> f = op->handler;
        op->handler.clear();
        lock.unlock();
        f(op->ret);
    }

    void GetBluetoothDevices2(RR_SHARED_PTR<refresh_op> op)
    {
        RR_SHARED_PTR<RobotRaconteurNode> n = GetParent()->GetNode();
        RR_SHARED_PTR<ThreadPool> p = n->GetThreadPool();

        std::list<typename bluetooth_connector::btaddr_type> addr = bt->GetDeviceAddresses();

        if (addr.empty())
        {
            GetAll1(RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >(), op);
            return;
        }

        boost::mutex::scoped_lock lock(op->this_lock);

        op->count--;

        BOOST_FOREACH (typename bluetooth_connector::btaddr_type a, addr)
        {
            p->Post(boost::bind(&HardwareTransport_discovery::GetBluetoothDevices3, this->shared_from_this(), op, a));
            op->count++;
        }
    }

    void GetBluetoothDevices3(RR_SHARED_PTR<refresh_op> op, typename bluetooth_connector::btaddr_type addr)
    {
        RR_SHARED_PTR<RobotRaconteurNode> node = GetParent()->GetNode();

        std::list<typename bluetooth_connector::device_info> d = bt->GetDeviceNodes(addr);

        boost::mutex::scoped_lock lock(op->this_lock);

        if (op->handled)
            return;

        RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > o = RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();

        BOOST_FOREACH (typename bluetooth_connector::device_info& e, d)
        {
            if (!e.nodeid_str || !e.nodename_str)
                continue;

            NodeDiscoveryInfo n;
            n.NodeID = NodeID(*e.nodeid_str);
            n.NodeName = *e.nodename_str;
            NodeDiscoveryInfoURL n1;
            n1.URL = "rr+bluetooth:///?nodeid=" + n.NodeID.ToString("D") + "&service=RobotRaconteurServiceIndex";
            n1.LastAnnounceTime = node->NowNodeTime();
            n.URLs.push_back(n1);
            o->push_back(n);
        }

        lock.unlock();

        GetAll1(o, op);
    }

    void GetAll1(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > nodeinfo, RR_SHARED_PTR<refresh_op> op)
    {
        boost::mutex::scoped_lock lock(op->this_lock);

        if (op->handled)
            return;

        if (nodeinfo)
        {
            boost::range::copy(*nodeinfo, std::back_inserter(*op->ret));
        }

        op->count--;
        if (op->count > 0)
            return;
        op->handled = true;
        boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)> f = op->handler;
        op->handler.clear();
        if (op->timer)
            op->timer->cancel();
        op->timer.reset();
        lock.unlock();
        f(op->ret);
    }

    void GetAll2(const boost::system::error_code& ec, RR_SHARED_PTR<refresh_op> op)
    {
        if (ec)
            return;

        boost::mutex::scoped_lock lock(op->this_lock);
        if (op->handled)
            return;
        op->handled = true;
        boost::function<void(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >)> f = op->handler;
        op->handler.clear();
        op->timer.reset();
        lock.unlock();
        f(op->ret);
    }

    void OnDeviceChanged(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > d)
    {
        if (!d)
            return;
        try
        {
            RR_SHARED_PTR<RobotRaconteurNode> n = GetParent()->GetNode();
            RR_SHARED_PTR<ThreadPool> p = n->GetThreadPool();

            BOOST_FOREACH (NodeDiscoveryInfo& d2, *d)
            {
                n->NodeDetected(d2);
            }
        }
        catch (std::exception&)
        {}
    }
    void OnBluetoothChanged(typename bluetooth_connector::btaddr_type addr)
    {
        boost::mutex::scoped_lock lock(this_lock);

        if (!bt)
            return;

        RR_SHARED_PTR<RobotRaconteurNode> n = GetParent()->GetNode();
        RR_SHARED_PTR<ThreadPool> p = n->GetThreadPool();

        typedef typename std::list<
            boost::tuple<typename bluetooth_connector::btaddr_type, boost::posix_time::ptime> >::iterator e_type;
        for (e_type e = detected_bluetooth.begin(); e != detected_bluetooth.end();)
        {
            typename bluetooth_connector::btaddr_type addr1 = e->template get<0>();
            if (memcmp(&addr1, &addr, sizeof(addr)) == 0)
            {
                if (n->NowNodeTime() > (e->template get<1>() + boost::posix_time::seconds(30)))
                {
                    e = detected_bluetooth.erase(e);
                }
                else
                {
                    e++;
                    return;
                }
            }
            else
            {
                e++;
            }
        }

        RR_SHARED_PTR<refresh_op> op = RR_MAKE_SHARED<refresh_op>();
        op->handler = boost::bind(&HardwareTransport_discovery::OnBluetoothChanged1, this->shared_from_this(),
                                  RR_BOOST_PLACEHOLDERS(_1));
        op->ret = RR_MAKE_SHARED<std::vector<NodeDiscoveryInfo> >();

        detected_bluetooth.push_back(boost::make_tuple(addr, n->NowNodeTime()));

        if (RobotRaconteurNode::TryPostToThreadPool(
                n, boost::bind(&HardwareTransport_discovery::GetBluetoothDevices1, this->shared_from_this(), op, addr)))
        {
            op->count++;
        }
    }

    void OnBluetoothChanged1(RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> > d)
    {
        if (!d)
            return;

        try
        {
            RR_SHARED_PTR<RobotRaconteurNode> n = GetParent()->GetNode();
            RR_SHARED_PTR<ThreadPool> p = n->GetThreadPool();

            BOOST_FOREACH (NodeDiscoveryInfo& d1, *d)
            {
                n->NodeDetected(d1);
            }
        }
        catch (std::exception&)
        {}
    }

    boost::mutex this_lock;

    RR_WEAK_PTR<HardwareTransport> parent;

    RR_WEAK_PTR<usb_manager> usb;
    RR_SHARED_PTR<bluetooth_connector> bt;
    std::list<boost::tuple<typename bluetooth_connector::btaddr_type, boost::posix_time::ptime> > detected_bluetooth;
    std::vector<std::string> schemes;
    RR_WEAK_PTR<RobotRaconteurNode> node;
};

} // namespace detail
} // namespace RobotRaconteur
