/**
 * @file HardwareTransport.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2020 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RobotRaconteur/RobotRaconteurNode.h"
#include <boost/shared_array.hpp>
#include <boost/asio/windows/stream_handle.hpp>

#pragma once

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API LocalTransportConnection;

/**
 * @brief Transport for USB, Bluetooth, and PCIe hardware devices
 *
 * **WARNING: THE HARDWARE TRANSPORT IS EXPERIMENTAL!**
 *
 * The HardwareTransport is disabled by default by the node setup classes.
 * Use `--robotraconteur-hardware-enable=true` option to enable.
 *
 * It is recommended that ClientNodeSetup, ServerNodeSetup, or SecureServerNodeSetup
 * be used to construct this class.
 *
 * See \ref robotraconteur_url for more information on URLs.
 *
 * Contact Wason Technology, LLC for more information on the hardware
 * transport.
 *
 */
class ROBOTRACONTEUR_CORE_API HardwareTransport : public Transport, public RR_ENABLE_SHARED_FROM_THIS<HardwareTransport>
{
    friend class HardwareTransportConnection;

  private:
    bool transportopen;

  public:
    RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<ITransportConnection> > TransportConnections;
    boost::mutex TransportConnections_lock;

    /**
     * @brief Construct a new HardwareTransport
     *
     * Must use boost::make_shared<HardwareTransport>()
     *
     * The use of RobotRaconteurNodeSetup and subclasses is recommended to construct
     * transports.
     *
     * The transport must be registered with the node using
     * RobotRaconteurNode::RegisterTransport() after construction.
     *
     * @param node The node that will use the transport. Default is the singleton node
     */
    HardwareTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node = RobotRaconteurNode::sp());

    RR_OVIRTUAL ~HardwareTransport() RR_OVERRIDE;

    RR_OVIRTUAL bool IsServer() const RR_OVERRIDE;

    RR_OVIRTUAL bool IsClient() const RR_OVERRIDE;

    RR_OVIRTUAL std::string GetUrlSchemeString() const RR_OVERRIDE;

    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE;

    RR_OVIRTUAL void AsyncCreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& callback) RR_OVERRIDE;

    RR_OVIRTUAL RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE;

    RR_OVIRTUAL void CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE;

  protected:
    virtual void AsyncCreateTransportConnection2(
        const std::string& noden, const RR_SHARED_PTR<ITransportConnection>& transport,
        const RR_SHARED_PTR<RobotRaconteurException>& err,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& callback);

    virtual void CloseTransportConnection_timed(const boost::system::error_code& err, const RR_SHARED_PTR<Endpoint>& e,
                                                const RR_SHARED_PTR<void>& timer);

  public:
    RR_OVIRTUAL bool CanConnectService(boost::string_ref url) RR_OVERRIDE;

    RR_OVIRTUAL void Close() RR_OVERRIDE;

    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE;

    RR_OVIRTUAL void PeriodicCleanupTask() RR_OVERRIDE;

    RR_OVIRTUAL uint32_t TransportCapability(boost::string_ref name) RR_OVERRIDE;

    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE;

    RR_OVIRTUAL void AsyncGetDetectedNodes(
        const std::vector<std::string>& schemes,
        const boost::function<void(const RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >&)>& handler,
        int32_t timeout = RR_TIMEOUT_INFINITE) RR_OVERRIDE;

    /** @copydoc TcpTransport::GetMaxMessageSize() */
    virtual int32_t GetMaxMessageSize();
    /** @copydoc TcpTransport::SetMaxMessageSize() */
    virtual void SetMaxMessageSize(int32_t size);

    /** @copydoc TcpTransport::GetDisableMessage4() */
    virtual bool GetDisableMessage4();
    /** @copydoc TcpTransport::SetDisableMessage4() */
    virtual void SetDisableMessage4(bool d);

    /** @copydoc TcpTransport::GetDisableStringTable() */
    virtual bool GetDisableStringTable();
    /** @copydoc TcpTransport::SetDisableStringTable() */
    virtual void SetDisableStringTable(bool d);

    /** @copydoc TcpTransport::GetDisableAsyncMessageIO() */
    virtual bool GetDisableAsyncMessageIO();
    /** @copydoc TcpTransport::GetDisableAsyncMessageIO() */
    virtual void SetDisableAsyncMessageIO(bool d);

    /**
     * @brief Add a USB VID/PID pair to the search list
     *
     * @param vid The USB VID
     * @param pid The USB PID
     * @param interface_ The Robot Raconteur USB interface number
     */
    virtual void AddUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_);

    /**
     * @brief Remove a registered USB VID/PID pair
     *
     * @param vid The USB VID
     * @param pid The USB PID
     * @param interface_ The Robot Raconteur USB interface number
     */
    virtual void RemoveUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_);

    /**
     * @brief Check if VID/PID pair has been registered
     *
     * @param vid The USB VID
     * @param pid The USB PID
     * @param interface_ The Robot Raconteur USB interface number
     * @return true
     * @return false
     */
    virtual bool IsValidUsbDevice(uint16_t vid, uint16_t pid, uint8_t interface_);

    virtual void register_transport(const RR_SHARED_PTR<ITransportConnection>& connection);
    virtual void erase_transport(const RR_SHARED_PTR<ITransportConnection>& connection);

    template <typename T, typename F>
    boost::signals2::connection AddCloseListener(const RR_SHARED_PTR<T>& t, const F& f)
    {
        boost::mutex::scoped_lock lock(closed_lock);
        if (closed)
        {
            lock.unlock();
            boost::bind(f, t)();
            return boost::signals2::connection();
        }

        return close_signal.connect(boost::signals2::signal<void()>::slot_type(boost::bind(f, t.get())).track(t));
    }

  protected:
    boost::mutex parameter_lock;
    int32_t max_message_size;
    bool disable_message4;
    bool disable_string_table;
    bool disable_async_message_io;

    boost::mutex discovery_lock;

    RR_SHARED_PTR<void> internal1;
    RR_SHARED_PTR<void> internal2;
    RR_SHARED_PTR<void> internal3;
    RR_SHARED_PTR<void> internal4;

    std::list<boost::tuple<uint16_t, uint16_t, uint8_t> > usb_devices;

    bool closed;
    boost::mutex closed_lock;
    boost::signals2::signal<void()> close_signal;
};
#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
using HardwareTransportPtr = RR_SHARED_PTR<HardwareTransport>;
#endif
} // namespace RobotRaconteur
