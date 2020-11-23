/** 
 * @file WireMember.h
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

#pragma once


#include "RobotRaconteur/DataTypes.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/AsyncUtils.h"
#include <boost/call_traits.hpp>


#pragma warning (push)
#pragma warning(disable: 4250)
#pragma warning(disable: 4996)
#include <boost/signals2.hpp>

namespace RobotRaconteur
{
	class ROBOTRACONTEUR_CORE_API  WireBase;
	class ROBOTRACONTEUR_CORE_API  WireConnectionBase;
	class ROBOTRACONTEUR_CORE_API WireConnectionBaseListener;
	namespace detail 
	{ 
		class WireSubscription_connection;  
		bool WireConnectionBase_IsValueExpired(RR_WEAK_PTR<RobotRaconteurNode> node, const boost::posix_time::ptime& recv_time, int32_t lifespan);		
	}

	/**
	 * @brief Base class for WireConnection
	 * 
	 * Base class for templated WireConnection
	 * 
	 */
	class ROBOTRACONTEUR_CORE_API  WireConnectionBase : public RR_ENABLE_SHARED_FROM_THIS<WireConnectionBase>, private boost::noncopyable
	{

		friend class WireBase;
		friend class WireClientBase;
		friend class WireServerBase;
		friend class WireBroadcasterBase;
		friend class WireSubscriptionBase;
		friend class detail::WireSubscription_connection;

	public:

		/**
		 * @brief Returns the Robot Raconteur node Endpoint ID
		 * 
		 * Returns the endpoint associated with the ClientContext or ServerEndpoint
		 * associated with the wire connection.
		 * 
		 * @return uint32_t The Robot Raconteur node Endpoint ID
		 */
		virtual uint32_t GetEndpoint();
		
		/**
		 * @brief Get the timestamp of the last received value
		 * 
		 * Returns the timestamp of the value in the *senders* clock
		 * 
		 * @return TimeSpec The timestamp of the last received value
		 */
		virtual TimeSpec GetLastValueReceivedTime();

		/**
		 * @brief Get the timestamp of the last sent value
		 * 
		 * Returns the timestamp of the last sent value in the *local* clock
		 * 
		 * @return TimeSpec The timestamp of the last sent value
		 */
		virtual TimeSpec GetLastValueSentTime();
		
		/**
		 * @brief Close the wire connection
		 * 
		 * Close the wire connection. Blocks until close complete. The peer wire connection 
		 * is destroyed automatically.
		 * 
		 */
		virtual void Close();

		/**
		 * @brief Asynchronously close the wire connection
		 * 
		 * Same as Close() but returns asynchronously
		 * 
		 * @param handler A handler function to call on completion, possibly with an exception
		 * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
		 */
		virtual void AsyncClose(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout);

		WireConnectionBase(RR_SHARED_PTR<WireBase> parent, uint32_t endpoint = 0, MemberDefinition_Direction direction = MemberDefinition_Direction_both);

		virtual ~WireConnectionBase() {}

		virtual void WirePacketReceived(TimeSpec timespec, RR_INTRUSIVE_PTR<RRValue> packet);

		/**
		 * @brief Get if the InValue is valid
		 * 
		 * The InValue is valid if a value has been received and 
		 * the value has not expired
		 * 
		 * @return true The InValue is valid
		 * @return false The OutValue is invalid
		 */
		virtual bool GetInValueValid();

		/**
		 * @brief Get if the OutValue is valid
		 * 
		 * The OutValue is valid if a value has been
		 * set using SetOutValue()
		 * 
		 * @return true The OutValue is valid
		 * @return false The OutValue is invalid
		 */
		virtual bool GetOutValueValid();

		/**
		 * @brief Waits for InValue to be valid
		 * 
		 * Blocks the current thread until InValue is valid,
		 * with an optional timeout. Returns true if InValue is valid,
		 * or false if timeout occurred.
		 * 
		 * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
		 * @return true The InValue is  valid
		 * @return false The InValue is invalid
		 */
		bool WaitInValueValid(int32_t timeout = RR_TIMEOUT_INFINITE);

		/**
		 * @brief Waits for OutValue to be valid
		 * 
		 * Blocks the current thread until OutValue is valid,
		 * with an optional timeout. Returns true if OutValue is valid,
		 * or false if timeout occurred.
		 * 
		 * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
		 * @return true The InValue is  valid
		 * @return false The InValue is invalid
		 */
		bool WaitOutValueValid(int32_t timeout = RR_TIMEOUT_INFINITE);

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		/**
		 * @brief Get if wire connection is ignoring incoming values
		 * 
		 * If true, wire connection is ignoring incoming values and is not
		 * storing the value
		 * 
		 * @return true Wire connection is ignoring incoming values
		 * @return false Wire connection is not ignoring incoming values
		 */
		virtual bool GetIgnoreInValue();

		/**
		 * @brief Set whether wire connection should ignore incoming values
		 * 
		 * Wire connections may optionally desire to ignore incoming values. This is useful if the connection
		 * is only being used to send out values, and received values may create a potential memory . If ignore is true, 
		 * incoming values will be discarded.
		 * 
		 * @param ignore If true, incoming values are ignored. If false, the most recent value is stored.
		 */
		virtual void SetIgnoreInValue(bool ignore);
				
		virtual void AddListener(RR_SHARED_PTR<WireConnectionBaseListener> listener);

		/**
		 * @brief The direction of the wire
		 * 
		 * Wires may be declared *readonly* or *writeonly* in the service definition file. (If neither
		 * is specified, the wire is assumed to be full duplex.) *readonly* wire may only send out values from
		 * service to client. *writeonly* wires may only send out values from client to service.
		 * 
		 * @return MemberDefinition_Direction 
		 */
		MemberDefinition_Direction Direction();

		/**
		 * @brief Get the lifespan of InValue
		 * 
		 * InValue may optionally have a finite lifespan specified in milliseconds.
		 * Once the lifespan after reception has expired, the InValue is cleared, and becomes invalid.
		 * Attempts to access InValue will result in a ValueNotSetException.
		 * 
		 * @return int32_t The lifespan in milliseconds
		 */
		virtual int32_t GetInValueLifespan();
		/**
		 * @brief Set the lifespan of InValue
		 * 
		 * InValue may optionally have a finite lifespan specified in milliseconds. Once
		 * the lifespan after reception has expired, the InValue is cleared and becomes invalid.
		 * Attempts to access InValue will result in ValueNotSetException.
		 * 
		 * InValue lifespans may be used to avoid using a stale value received by the wire. If
		 * the lifespan is not set, the wire will continue to return the last received value, even
		 * if the value is old.
		 * 
		 * @param millis The lifespan in millisecond, or RR_VALUE_LIFESPAN_INFINITE for infinite lifespan
		 */
		virtual void SetInValueLifespan(int32_t millis);
		
		/**
		 * @brief Get the lifespan of OutValue
		 * 
		 * OutValue may optionally have a finite lifespan specified in milliseconds.
		 * Once the lifespan after sending has expired, the OutValue is cleared, and becomes invalid.
		 * Attempts to access OutValue will result in a ValueNotSetException.
		 * 
		 * @return int32_t The lifespan in milliseconds
		 */
		virtual int32_t GetOutValueLifespan();
		/**
		 * @brief Set the lifespan of OutValue
		 * 
		 * OutValue may optionally have a finite lifespan specified in milliseconds. Once
		 * the lifespan after sending has expired, the OutValue is cleared and becomes invalid.
		 * Attempts to access OutValue will result in ValueNotSetException.
		 * 
		 * OutValue lifespans may be used to avoid using a stale value sent by the wire. If
		 * the lifespan is not set, the wire will continue to return the last sent value, even
		 * if the value is old.
		 * 
		 * @param millis The lifespan in millisecond, or RR_VALUE_LIFESPAN_INFINITE for infinite lifespan
		 */
		virtual void SetOutValueLifespan(int32_t millis);
		

	protected:

		virtual void RemoteClose();

		RR_INTRUSIVE_PTR<RRValue> inval;
		RR_INTRUSIVE_PTR<RRValue> outval;

		bool inval_valid;
		TimeSpec lasttime_send;
		boost::posix_time::ptime lasttime_send_local;

		bool  outval_valid;
		TimeSpec lasttime_recv;
		boost::posix_time::ptime lasttime_recv_local;

		boost::condition_variable inval_wait;
		boost::condition_variable outval_wait;

		int32_t inval_lifespan;
		int32_t outval_lifespan;

		uint32_t endpoint;
		RR_WEAK_PTR<WireBase> parent;
		std::string service_path;
		std::string member_name;

		boost::mutex sendlock;
		boost::mutex recvlock;

		bool send_closed;
		bool recv_closed;

		RR_INTRUSIVE_PTR<RRValue> GetInValueBase();

		RR_INTRUSIVE_PTR<RRValue> GetOutValueBase();

		void SetOutValueBase(RR_INTRUSIVE_PTR<RRValue> value);
		
		bool TryGetInValueBase(RR_INTRUSIVE_PTR<RRValue>& value, TimeSpec& time);
		bool TryGetOutValueBase(RR_INTRUSIVE_PTR<RRValue>& value, TimeSpec& time);
		
		virtual void fire_WireValueChanged(RR_INTRUSIVE_PTR<RRValue> value, TimeSpec time)=0;
	
		virtual void fire_WireClosedCallback()=0;

		void Shutdown();
	

		RR_SHARED_PTR<WireBase> GetParent();

		boost::mutex inval_lock;
		boost::mutex outval_lock;

		bool ignore_inval;

		boost::mutex listeners_lock;
		std::list<RR_WEAK_PTR<WireConnectionBaseListener> > listeners;

		detail::async_signal_semaphore wire_value_changed_semaphore;

		RR_WEAK_PTR<RobotRaconteurNode> node;

		MemberDefinition_Direction direction;
		
	};

	
	/**
	 * @brief Wire connection used to transmit "most recent" values
	 * 
	 * Wire connections are used to transmit "most recent" values between connected
	 * wire members. See Wire for more information on wire members.
	 * 
	 * Wire connections are created by clients using the Wire::Connect() or Wire::AsyncConnect()
	 * functions. Services receive incoming wire connection requests through a 
	 * callback function specified using the Wire::SetWireConnectCallback() function. Services
	 * may also use the WireBroadcaster class to automate managing wire connection lifecycles and
	 * sending values to all connected clients, or use WireUnicastReceiver to receive an incoming
	 * value from the most recently connected client.
	 * 
	 * Wire connections are used to transmit "most recent" values between clients and services. Connection
	 * the wire creates a connection pair, one in the client, and one in the service. Each wire connection 
	 * object has an InValue and an OutValue. Setting the OutValue of one will cause the specified value to
	 * be transmitted to the InValue of the peer. See Wire for more information.
	 * 
	 * Values can optionally be specified to have a finite lifespan using SetInValueLifespan() and
	 * SetOutValueLifespan(). Lifespans can be used to prevent using old values that have
	 * not been recently updated.
	 *
	 * This class is instantiated by the Wire class. It should not be instantiated
	 * by the user.
	 * 
	 * @tparam T The value data type
	 */
	template <typename T>
	class WireConnection : public WireConnectionBase
	{
	private:
		boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> WireConnectionClosedCallback;
		boost::mutex WireConnectionClosedCallback_lock;

	public:
		/**
		 * @brief Signal invoked when the InValue is changed
		 * 
		 * Callback function must accept three arguments, receiving the WireConnectionPtr<T> that
		 * received a packet, the new value, and the value's TimeSpec timestamp
		 */
		boost::signals2::signal<void (RR_SHARED_PTR<WireConnection<T> > connection, T value, TimeSpec time)> WireValueChanged;

		/**
		 * @brief Get the currently configured connection closed callback function
		 * 
		 * @return boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> 
		 */

		boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectionClosedCallback()
		{
			boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
			return WireConnectionClosedCallback;
		}

		/**
		 * @brief Set the connection closed callback function
		 * 
		 * Sets a function to invoke when the wire connection has been closed.
		 * 
		 * Callback function must accept one argument, receiving the WireConnectionPtr<T> that
		 * was closed.
		 * 
		 * @param callback The callback function
		 */
		void SetWireConnectionClosedCallback(boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> callback)
		{
			boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
			WireConnectionClosedCallback=callback;
		}

		virtual ~WireConnection() {}

		/**
		 * @brief Get the current InValue
		 * 
		 * Gets the current InValue that was transmitted from the peer. Throws
		 * ValueNotSetException if no value has been received, or the most
		 * recent value lifespan has expired.
		 * 
		 * @return T The value
		 */
		virtual T GetInValue()
		{
			return RRPrimUtil<T>::PreUnpack(GetInValueBase());
		}

		/**
		 * @brief Get the current OutValue
		 * 
		 * Gets the current OutValue that was transmitted to the peer. Throws
		 * ValueNotSetException if no value has been received, or the most
		 * recent value lifespan has expired.
		 * 
		 * @return T The value
		 */
		virtual T GetOutValue()
		{
			return RRPrimUtil<T>::PreUnpack(GetOutValueBase());
		}

		/**
		 * @brief Set the OutValue and transmit to the peer connection
		 * 
		 * Sets the OutValue for the wire connection. The specified value will be
		 * transmitted to the peer, and will become the peers InValue. The transmission
		 * is unreliable, meaning that values may be dropped if newer values arrive.
		 * 
		 * @param value The new out value
		 */
		virtual void SetOutValue(typename boost::call_traits<T>::param_type value)
		{
			SetOutValueBase(RRPrimUtil<T>::PrePack(value));
		}

		/**
		 * @brief Try getting the InValue, returning true on success or false on failure
		 * 
		 * Get the current InValue and InValue timestamp. Return true or false on
		 * success or failure instead of throwing exception.
		 * 
		 * @param value [out] The current InValue
		 * @param time [out] The current InValue timestamp in the senders clock
		 * @return true The InValue was valid
		 * @return false The InValue was invalid. value and time are undefined
		 */
		bool TryGetInValue(T& value, TimeSpec& time)
		{
			RR_INTRUSIVE_PTR<RRValue> o;
			if (!TryGetInValueBase(o, time)) return false;
			value = RRPrimUtil<T>::PreUnpack(o);
			return true;
		}

		/**
		 * @brief Try getting the OutValue, returning true on success or false on failure
		 * 
		 * Get the current OutValue and OutValue timestamp. Return true or false on
		 * success and failure instead of throwing exception.
		 * 
		 * @param value [out] The current OutValue
		 * @param time [out] The current OutValue timestamp in the local clock
		 * @return true The OutValue was valid
		 * @return false The OutValue was invalid. value and time are undefined
		 */
		bool TryGetOutValue(T& value, TimeSpec& time)
		{
			RR_INTRUSIVE_PTR<RRValue> o;
			if (!TryGetOutValueBase(o, time)) return false;
			value = RRPrimUtil<T>::PreUnpack(o);
			return true;
		}

		WireConnection(RR_SHARED_PTR<WireBase> parent, uint32_t endpoint=0, MemberDefinition_Direction direction = MemberDefinition_Direction_both)
			: WireConnectionBase(parent,endpoint,direction) {}

	protected:
		virtual void fire_WireValueChanged(RR_INTRUSIVE_PTR<RRValue> value, TimeSpec time)
		{
			WireValueChanged(RR_STATIC_POINTER_CAST<WireConnection<T> >(shared_from_this()),RRPrimUtil<T>::PreUnpack(value),time);
		}
	
		virtual void fire_WireClosedCallback()
		{
			boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> c=GetWireConnectionClosedCallback();
			if (!c) return;
			c(RR_STATIC_POINTER_CAST<WireConnection<T> >(shared_from_this()));
		}

	public:
		virtual void Close()
		{
			WireConnectionBase::Close();
			{
				boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
				WireConnectionClosedCallback.clear();
			}
			WireValueChanged.disconnect_all_slots();
		}

	protected:

		virtual void AsyncClose1(RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>& handler)
		{
			try
			{
				{
					boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
					WireConnectionClosedCallback.clear();
				}
				WireValueChanged.disconnect_all_slots();
			}
			catch (std::exception&) {}

			
			handler(err);
		}

	public:
		virtual void AsyncClose(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout=2000)
		{
			WireConnectionBase::AsyncClose(boost::bind(&WireConnection<T>::AsyncClose1,RR_STATIC_POINTER_CAST<WireConnection<T> >(shared_from_this()),RR_BOOST_PLACEHOLDERS(_1),handler),timeout);
			
		}

	protected:
		virtual void RemoteClose()
		{
			WireConnectionBase::RemoteClose();
			{
				boost::mutex::scoped_lock lock(WireConnectionClosedCallback_lock);
				WireConnectionClosedCallback.clear();
			}
			WireValueChanged.disconnect_all_slots();
		}


	};


	/**
	 * @brief Base class for Wire
	 * 
	 */
	class ROBOTRACONTEUR_CORE_API  WireBase : public RR_ENABLE_SHARED_FROM_THIS<WireBase>, private boost::noncopyable
	{
		

	public:

		friend class WireConnectionBase;
		
		virtual ~WireBase() {}

		/**
		 * @brief Get the member name of the wire
		 * 
		 * @return std::string 
		 */
		virtual std::string GetMemberName()=0;

		virtual std::string GetServicePath()=0;

		virtual void WirePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e=0)=0;

		virtual void Shutdown()=0;


		

		virtual void AsyncClose(RR_SHARED_PTR<WireConnectionBase> endpoint, bool remote, uint32_t ee, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout)=0;

	protected:

		virtual void SendWirePacket(RR_INTRUSIVE_PTR<RRValue> data, TimeSpec time, uint32_t endpoint)=0;

		bool rawelements;		

		void DispatchPacket (RR_INTRUSIVE_PTR<MessageEntry> me, RR_SHARED_PTR<WireConnectionBase> e);

		RR_INTRUSIVE_PTR<RRValue> UnpackPacket(RR_INTRUSIVE_PTR<MessageEntry> me, TimeSpec& ts);

		RR_INTRUSIVE_PTR<MessageEntry> PackPacket(RR_INTRUSIVE_PTR<RRValue> data, TimeSpec time);

		virtual RR_INTRUSIVE_PTR<MessageElementData> PackData(RR_INTRUSIVE_PTR<RRValue> data)
		{
			return GetNode()->PackVarType(data);
		}

		virtual RR_INTRUSIVE_PTR<RRValue> UnpackData(RR_INTRUSIVE_PTR<MessageElement> mdata)
		{
			return GetNode()->UnpackVarType(mdata);
		}

		
		RR_WEAK_PTR<RobotRaconteurNode> node;

		MemberDefinition_Direction direction;

	public:

		RR_SHARED_PTR<RobotRaconteurNode> GetNode();

		/**
		 * @brief The direction of the wire
		 * 
		 * Wires may be declared *readonly* or *writeonly* in the service definition file. (If neither
		 * is specified, the wire is assumed to be full duplex.) *readonly* wire may only send out values from
		 * service to client. *writeonly* wires may only send out values from client to service.
		 * 
		 * @return MemberDefinition_Direction 
		 */
		MemberDefinition_Direction Direction();	

	};


	/**
	 * @brief `wire` member type interface
	 * 
	 * The Wire class implements the `wire` member type. Wires are declared in service definition files
	 * using the `wire` keyword within object declarations. Wires provide "most recent" value streaming
	 * between clients and services. They work by creating "connection" pairs between the client and service.
	 * The wire streams the current value between the wire connection pairs using packets. Wires 
	 * are unreliable; only the most recent value is of interest, and any older values 
	 * will be dropped. Wire connections have an InValue and an OutValue. Users set the OutValue on the
	 * connection. The new OutValue is transmitted to the peer wire connection, and becomes the peer's
	 * InValue. The peer can then read the InValue. The client and service have their own InValue
	 * and OutValue, meaning that each direction, client to service or service to client, has its own
	 * value.
	 * 
	 * Wire connections are created using the Connect() or AsyncConnect() functions. Services receive
	 * incoming connection requests through a callback function. Thes callback is configured using
	 * the SetWireConnectCallback() function. Services may also use the WireBroadcaster class
	 * or WireUnicastReceiver class to automate managing wire connection lifecycles. WireBroadcaster
	 * is used to send values to all connected clients. WireUnicastReceiver is used to receive the
	 * value from the most recent wire connection. See WireConnection for details on sending
	 * and receiving streaming values.
	 * 
	 * Wire clients may also optionally "peek" and "poke" the wire without forming a streaming
	 * connection. This is useful if the client needs to read the InValue or set the OutValue
	 * instantaniously, but does not need continuous updating. PeekInValue() or 
	 * AsyncPeekInValue() will retrieve the client's current InValue. PokeOutValue() or
	 * AsyncPokeOutValue() will send a new client OutValue to the service.
	 * PeekOutValue() or AsyncPeekOutValue() will retrieve the last client OutValue received by
	 * the service.
	 * 
	 * "Peek" and "poke" operations initiated by the client are received on the service using
	 * callbacks. Use SetPeekInValueCallback(), SetPeekOutValueCallback(),
	 * and SetPokeOutValueCallback() to configure the callbacks to handle these requests.
	 * WireBroadcaster and WireUnicastReceiver configure these callbacks automatically, so 
	 * the user does not need to configure the callbacks when these classes are used.
	 * 
	 * Wires can be declared *readonly* or *writeonly*. If neither is specified, the wire is assumed
	 * to be full duplex. *readonly* pipes may only send values from service to client, ie OutValue
	 * on service side and InValue on client side. *writeonly* pipes may only send values from
	 * client to service, ie OutValue on client side and InValue on service side. Use Direction()
	 * to determine the direction of the wire.
	 * 
	 * Unlike pipes, wire connections are not indexed, so only one connection pair can be
	 * created per client connection.
	 *  
	 * WireBroadcaster or WireUnicastReceiver are typically used to simplify using wires.
	 * See WireBroadcaster and WireUnicastReceiver for more information.
	 * 
	 * This class is instantiated by the node. It should not be instantiated by the user.
	 * 
	 * @tparam T The value data type
	 */
	template <typename T>
	class Wire : public virtual WireBase
	{

		friend class WireConnectionBase;

	public:

		Wire(boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify)
		{
			this->verify = verify;
		}

		virtual ~Wire() {}
		
		/**
		 * @brief Connect the wire
		 * 
		 * Creates a connection between the wire, returning the client connection. Used to create
		 * a "most recent" value streaming connection to the service.
		 * 
		 * Only valid on clients. Will throw InvalidOperationException on the service side.
		 * 
		 * Note: If a streaming connection is not required, use PeekInValue(), PeekOutValue(),
		 * or PokeOutValue() instead of creating a connection.
		 * 
		 * @return RR_SHARED_PTR<WireConnection<T> > The wire connection
		 */
		virtual RR_SHARED_PTR<WireConnection<T> > Connect() = 0;

		/**
		 * @brief Asynchronously connect the wire
		 * 
		 * Same as Connect(), but returns asynchronously
		 * 
		 * Only valid on clients. Will throw InvalidOperationException on the service side.
		 * 
		 * @param handler A handler function to receive the wire connection, or an exception
		 * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
		 */
		virtual void AsyncConnect(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<WireConnection<T> >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;

		/**
		 * @brief Peek the current InValue
		 * 
		 * Peeks the current InValue using a "request" instead of a streaming value. Use
		 * if only the instantanouse value is required. 
		 * 
		 * Peek and poke are similar to `property` members. Unlike streaming,
		 * peek and poke are reliable operations.
		 * 
		 * Throws ValueNotSetException if InValue is not valid.
		 * 
		 * Only valid on clients. Will throw InvalidOperationException on the service side.
		 * 
		 * @param ts [out] The timestamp of the current InValue
		 * @return T The current InValue
		 */
		virtual T PeekInValue(TimeSpec& ts) = 0;

		/**
		 * @brief Peek the current OutValue
		 * 
		 * Peeks the current OutValue using a "request" instead of a streaming value. Use
		 * if only the instantanouse value is required. 
		 * 
		 * Peek and poke are similar to `property` members. Unlike streaming,
		 * peek and poke are reliable operations. 
		 * 
		 * Throws ValueNotSetException if OutValue is not valid.
		 * 
		 * Only valid on clients. Will throw InvalidOperationException on the service side.
		 * 
		 * @param ts [out] The timestamp of the current OutValue
		 * @return T The current OutValue
		 */
		virtual T PeekOutValue(TimeSpec& ts) = 0;

		/**
		 * @brief Poke the OutValue
		 * 
		 * Pokes the OutValue using a "request" instead of a streaming value. Use 
		 * to update the OutValue if the value is updated infrequently.
		 * 
		 * Peek and poke are similar to `property` members. Unlike streaming,
		 * peek and poke are reliable operations.
		 * 
		 * Only valid on clients. Will throw InvalidOperationException on the service side.
		 * 
		 * @param value The new OutValue
		 */
		virtual void PokeOutValue(const T& value) = 0;

		/**
		 * @brief Asynchronously peek the current InValue
		 * 
		 * Same as PeekInValue(), but returns asynchronously.
		 * 
		 * Only valid on clients. Will throw InvalidOperationException on the service side.
		 * 
		 * @param handler A handler function to receive the InValue and timestamp, or an exception
		 * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
		 */
		virtual void AsyncPeekInValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;

		/**
		 * @brief Asynchronously peek the current OutValue
		 * 
		 * Same as PeekOutValue(), but returns asynchronously.
		 * 
		 * Only valid on clients. Will throw InvalidOperationException on the service side.
		 * 
		 * @param handler A handler function to receive the OutValue and timestamp, or an exception
		 * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
		 */
		virtual void AsyncPeekOutValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;

		/**
		 * @brief Asynchronously poke the OutValue
		 * 
		 * Same as PokeOutValue(), but returns asynchronously
		 * 
		 * Only valid on clients. Will throw InvalidOperationException on the service side.
		 * 
		 * @param handler A handler function to invoke on completion, with possible exception
		 * @param value The new OutValue
		 * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
		 */
		virtual void AsyncPokeOutValue(const T& value, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) = 0;
		
		/**
		 * @brief Get the currently configured wire connected callback function
		 * 
		 * Only valid for services. Will throw InvalidOperationException on client side.
		 * 
		 * @return boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> The currently configured callback function
		 */
		virtual boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectCallback()=0;

		/**
		 * @brief Set wire connected callback function
		 * 
		 * Callback function invoked when a client attempts to connect a the wire. The callback
		 * will receive the incoming wire connection as a parameter. The service must maintain a 
		 * reference to the wire connection, but the wire will retain ownership of the wire connection 
		 * until it is closed. Using  boost::weak_ptr to store the reference to the connection 
		 * is recommended.
		 * 
		 * The callback may throw an exception to reject incoming connect request.
		 *
		 * Note: Connect callback is configured automatically by WireBroadcaster or
		 * WireUnicastReceiver
		 * 
		 * Only valid for services. Will throw InvalidOperationException on the client side.
		 *  
		 * @param function Callback function to receive the incoming connection
		 */
		virtual void SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> function)=0;
		
		/**
		 * @brief Get the currently configure PeekInValue callback
		 * 
		 * Only valid for services. Will throw InvalidOperationException on the client side.
		 * 
		 * @return boost::function<T(const uint32_t&)> The currently configured callback function
		 */
		virtual boost::function<T(const uint32_t&)> GetPeekInValueCallback() = 0;

		/**
		 * @brief Set the PeekInValue callback function
		 * 
		 * Peek and poke operations are used when a streaming connection of the most recent value
		 * is not required. Clients initiate peek and poke operations using PeekInValue(), PeekOutValue(),
		 * PokeOutValue(), or their asynchronous equivalents. Services receive the peek and poke
		 * requests through callbacks.
		 * 
		 * SetPeekInValueCallback() configures the service callback for PeekInValue() requests.
		 * 
		 * The specified callback function should have the following signature:
		 * 
		 *     T peek_invalue_callback(uint32 client_endpoint);
		 * 
		 * The function receives the client endpoint ID, and returns the current InValue.
		 * 		 
		 * Note: Callback is configured automatically by WireBroadcaster or
		 * WireUnicastReceiver
		 * 
		 * Only valid for services. Will throw InvalidOperationException on the client side.
		 * 
		 * @param function The callback function
		 */
		virtual void SetPeekInValueCallback(boost::function<T(const uint32_t&)> function) = 0;

		/**
		 * @brief Get the currently configure PeekOutValue callback
		 * 
		 * Only valid for services. Will throw InvalidOperationException on the client side.
		 * 
		 * @return boost::function<T(const uint32_t&)> The currently configured callback function
		 */
		virtual boost::function<T(const uint32_t&)> GetPeekOutValueCallback() = 0;

		/**
		 * @brief Set the PeekOutValue callback function
		 * 
		 * Peek and poke operations are used when a streaming connection of the most recent value
		 * is not required. Clients initiate peek and poke operations using PeekInValue(), PeekOutValue(),
		 * PokeOutValue(), or their asynchronous equivalents. Services receive the peek and poke
		 * requests through callbacks.
		 * 
		 * SetPeekOutValueCallback() configures the service callback for PeekOutValue() requests.
		 * 
		 * The specified callback function should have the following signature:
		 * 
		 *     T peek_outvalue_callback(uint32 client_endpoint);
		 * 
		 * The function receives the client endpoint ID, and returns the current OutValue.
		 *
		 * Note: Callback is configured automatically by WireBroadcaster or
		 * WireUnicastReceiver
		 * 
		 * Only valid for services. Will throw InvalidOperationException on the client side.
		 * 
		 * @param function The callback function
		 */
		virtual void SetPeekOutValueCallback(boost::function<T(const uint32_t&)> function) = 0;

		/**
		 * @brief Get the currently configure PokeOutValue callback
		 * 
		 * Only valid for services. Will throw InvalidOperationException on the client side.
		 * 
		 * @return boost::function<void(const T&, const TimeSpec&, const uint32_t&)> The currently configured
		 *     callback function 
		 */
		virtual boost::function<void(const T&, const TimeSpec&, const uint32_t&)> GetPokeOutValueCallback() = 0;

		/**
		 * @brief Set the PokeOutValue callback function
		 * 
		 * Peek and poke operations are used when a streaming connection of the most recent value
		 * is not required. Clients initiate peek and poke operations using PeekInValue(), PeekOutValue(),
		 * PokeOutValue(), or their asynchronous equivalents. Services receive the peek and poke
		 * requests through callbacks.
		 * 
		 * SetPokeOutValueCallback() configures the service callback for PokeOutValue() requests.
		 * 
		 * The specified callback function should have the following signature:
		 * 
		 *     void poke_outvalue_callback(const T& value, const TimeSpec& timestamp, uint32 client_endpoint);
		 * 
		 * The function receives the new out value, the new out value timestamp in the client's clock, 
		 * and the client endpoint ID.
		 * 
		 * Note: Callback is configured automatically by WireBroadcaster or
		 * WireUnicastReceiver
		 * 
		 * Only valid for services. Will throw InvalidOperationException on the client side.
		 * 
		 * @param function The callback function
		 */ 
		virtual void SetPokeOutValueCallback(boost::function<void(const T&, const TimeSpec&, const uint32_t&)> function) = 0;
		
		
	protected:

		virtual RR_INTRUSIVE_PTR<MessageElementData> PackData(RR_INTRUSIVE_PTR<RRValue> data)
		{
			if (verify)
			{
				verify(data);
			}
			return GetNode()->template PackAnyType<typename RRPrimUtil<T>::BoxedType>(data);
		}

		virtual RR_INTRUSIVE_PTR<RRValue> UnpackData(RR_INTRUSIVE_PTR<MessageElement> mdata)
		{
			if (!verify)
			{
				return GetNode()->template UnpackAnyType<typename RRPrimUtil<T>::BoxedType>(mdata);
			}
			else
			{
				RR_INTRUSIVE_PTR<RRValue> ret = GetNode()->template UnpackAnyType<typename RRPrimUtil<T>::BoxedType>(mdata);
				verify(ret);
				return ret;
			}
		}

		boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify;
	};


	class ROBOTRACONTEUR_CORE_API  ServiceStub;
	
	class ROBOTRACONTEUR_CORE_API  WireClientBase : public virtual WireBase
	{
		friend class WireConnectionBase;
		friend class WireSubscriptionBase;
		friend class detail::WireSubscription_connection;

	public:

		virtual ~WireClientBase() {}

		virtual std::string GetMemberName();

		virtual std::string GetServicePath();

		virtual void WirePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e = 0);

		virtual void Shutdown();



		virtual void AsyncClose(RR_SHARED_PTR<WireConnectionBase> endpoint, bool remote, uint32_t ee, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout);

		RR_SHARED_PTR<ServiceStub> GetStub();

	protected:

		virtual void SendWirePacket(RR_INTRUSIVE_PTR<RRValue> packet, TimeSpec time, uint32_t endpoint);

		std::string m_MemberName;
		std::string service_path;
		uint32_t endpoint;

		RR_SHARED_PTR<WireConnectionBase> connection;
		boost::mutex connection_lock;

		RR_WEAK_PTR<ServiceStub> stub;

		void AsyncConnect_internal(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<WireConnectionBase>, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout);

		void AsyncConnect_internal1(RR_INTRUSIVE_PTR<MessageEntry> ret, RR_SHARED_PTR<RobotRaconteurException> err, boost::function<void(RR_SHARED_PTR<WireConnectionBase>, RR_SHARED_PTR<RobotRaconteurException>)>& handler);


		WireClientBase(boost::string_ref name, RR_SHARED_PTR<ServiceStub> stub, MemberDefinition_Direction direction);

		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(MemberDefinition_Direction direction) = 0;

		RR_INTRUSIVE_PTR<RRValue> PeekInValueBase(TimeSpec& ts);
		RR_INTRUSIVE_PTR<RRValue> PeekOutValueBase(TimeSpec& ts);
		void PokeOutValueBase(RR_INTRUSIVE_PTR<RRValue> value);

		void AsyncPeekInValueBase(RR_MOVE_ARG(boost::function<void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);
		void AsyncPeekOutValueBase(RR_MOVE_ARG(boost::function<void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);
		void AsyncPokeOutValueBase(const RR_INTRUSIVE_PTR<RRValue>& value, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE);

		void AsyncPeekValueBaseEnd1(RR_INTRUSIVE_PTR<MessageEntry> m, RR_SHARED_PTR<RobotRaconteurException> err,
			boost::function< void(const RR_INTRUSIVE_PTR<RRValue>&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>) >& handler);
				
	};

	template <typename T>
	class WireClient : public virtual Wire<T>, public virtual WireClientBase
	{
	public:

		WireClient(boost::string_ref name, RR_SHARED_PTR<ServiceStub> stub, MemberDefinition_Direction direction = MemberDefinition_Direction_both, boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify = NULL) : WireClientBase(name, stub, direction), Wire<T>(verify)
		{
			if (boost::is_same<T, RR_INTRUSIVE_PTR<MessageElement> >::value)
			{
				rawelements = true;
			}
			else
			{
				rawelements = false;
			}

		}

		virtual ~WireClient() {}
				
		virtual void AsyncConnect(RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<WireConnection<T> >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout=RR_TIMEOUT_INFINITE)
		{
			AsyncConnect_internal(boost::bind(handler,boost::bind(&WireClient<T>::AsyncConnect_cast,RR_BOOST_PLACEHOLDERS(_1)),RR_BOOST_PLACEHOLDERS(_2)),timeout);
		}

		virtual RR_SHARED_PTR<WireConnection<T> > Connect()
		{
			ROBOTRACONTEUR_ASSERT_MULTITHREADED(node);

			RR_SHARED_PTR<detail::sync_async_handler<WireConnection<T> > > t=RR_MAKE_SHARED<detail::sync_async_handler<WireConnection<T> > >();
			AsyncConnect(boost::bind(&detail::sync_async_handler<WireConnection<T> >::operator(),t,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2)),GetNode()->GetRequestTimeout());
			return t->end(); 
		}

	protected:

		static RR_SHARED_PTR<WireConnection<T> > AsyncConnect_cast(RR_SHARED_PTR<WireConnectionBase> b)
		{
			return rr_cast<WireConnection<T> >(b);
		}

		void AsyncPeekValueBaseEnd2(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, RR_SHARED_PTR<RobotRaconteurException> err,
			boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>& handler)
		{			

			if (err)
			{
				typename boost::initialized<T> err_value;
				handler(err_value, ts, err);
				return;
			}
			
			T value2;
			try
			{
				value2 = RRPrimUtil<T>::PreUnpack(value);				
			}
			catch (std::exception& exp)
			{
				typename boost::initialized<T> err_value;
				RR_SHARED_PTR<RobotRaconteurException> err = RobotRaconteurExceptionUtil::ExceptionToSharedPtr(exp);
				handler(err_value, ts, err);
				return;
			}

			handler(value2, ts, err);
		}

	public:

		virtual T PeekInValue(TimeSpec& ts) { return RRPrimUtil<T>::PreUnpack(PeekInValueBase(ts)); }
		virtual T PeekOutValue(TimeSpec& ts) { return RRPrimUtil<T>::PreUnpack(PeekOutValueBase(ts)); }
		virtual void PokeOutValue(const T& value) { return PokeOutValueBase(RRPrimUtil<T>::PrePack(value)); }
		virtual void AsyncPeekInValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			AsyncPeekInValueBase(boost::bind(&WireClient::AsyncPeekValueBaseEnd2, RR_DYNAMIC_POINTER_CAST<WireClient>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3), RR_MOVE(handler)), timeout);
		}
		virtual void AsyncPeekOutValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			AsyncPeekOutValueBase(boost::bind(&WireClient::AsyncPeekValueBaseEnd2, RR_DYNAMIC_POINTER_CAST<WireClient>(shared_from_this()), RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3), RR_MOVE(handler)), timeout);
		}
		virtual void AsyncPokeOutValue(const T& value, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			AsyncPokeOutValueBase(RRPrimUtil<T>::PrePack(value), RR_MOVE(handler), timeout);
		}
		
		//Unused service-side functions
		virtual boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectCallback() 
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName, "GetWireConnectCallback is not valid for WireClient");
			throw InvalidOperationException("Not valid for client"); 
		}
		virtual void SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> function) 
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName, "SetWireConnectCallback is not valid for WireClient"); 
			throw InvalidOperationException("Not valid for client"); 
		}
		virtual boost::function<T(const uint32_t&)> GetPeekInValueCallback()
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName, "GetPeekInValueCallback is not valid for WireClient"); 
			throw InvalidOperationException("Not valid for client");
		}
		virtual void SetPeekInValueCallback(boost::function<T(const uint32_t&)> function) 
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName, "SetPeekInValueCallback is not valid for WireClient");  
			throw InvalidOperationException("Not valid for client"); 
		}
		virtual boost::function<T(const uint32_t&)> GetPeekOutValueCallback()
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName, "GetPeekOutValueCallback is not valid for WireClient"); 
			throw InvalidOperationException("Not valid for client"); 
		}
		virtual void SetPeekOutValueCallback(boost::function<T(const uint32_t&)> function) 
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName, "SetPeekOutValueCallback is not valid for WireClient");  
			throw InvalidOperationException("Not valid for client"); 
		}
		virtual boost::function<void(const T&, const TimeSpec&, const uint32_t&)> GetPokeOutValueCallback() 
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName, "GetPokeOutValueCallback is not valid for WireClient");  
			throw InvalidOperationException("Not valid for client"); 
		}
		virtual void SetPokeOutValueCallback(boost::function<void(const T&, const TimeSpec&, const uint32_t&)> function) 
		{ 
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, endpoint, service_path, m_MemberName, "SetPokeOutValueCallback is not valid for WireClient"); 
			throw InvalidOperationException("Not valid for client"); 
		}
		

	protected:
		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(MemberDefinition_Direction direction)
		{
			return RR_MAKE_SHARED<WireConnection<T> >(RR_STATIC_POINTER_CAST<WireBase>(shared_from_this()), 0, direction);
		}

	};


	class ROBOTRACONTEUR_CORE_API  ServiceSkel;
	class ROBOTRACONTEUR_CORE_API  WireServerBase : public virtual WireBase
	{
		friend class WireConnectionBase;

	public:

		virtual ~WireServerBase() {}

		virtual std::string GetMemberName();

		virtual std::string GetServicePath();

		virtual void WirePacketReceived(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e=0);

		virtual void Shutdown();
		
		virtual void AsyncClose(RR_SHARED_PTR<WireConnectionBase> endpoint, bool remote, uint32_t ee, RR_MOVE_ARG(boost::function<void (RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout);

		virtual RR_INTRUSIVE_PTR<MessageEntry> WireCommand(RR_INTRUSIVE_PTR<MessageEntry> m, uint32_t e);

		RR_SHARED_PTR<ServiceSkel> GetSkel();

	protected:

		virtual void SendWirePacket(RR_INTRUSIVE_PTR<RRValue> data, TimeSpec time, uint32_t endpoint);

		std::string m_MemberName;
		std::string service_path;

		RR_UNORDERED_MAP<uint32_t,RR_SHARED_PTR<WireConnectionBase> > connections;
		boost::mutex connections_lock;

		RR_WEAK_PTR<ServiceSkel> skel;

		
		WireServerBase(boost::string_ref name, RR_SHARED_PTR<ServiceSkel> skel, MemberDefinition_Direction direction);

		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(uint32_t e, MemberDefinition_Direction direction)=0;
				
		virtual void fire_WireConnectCallback(RR_SHARED_PTR<WireConnectionBase> e)=0;

		

		bool init;
		boost::signals2::connection listener_connection;
	public:
		void ClientDisconnected(RR_SHARED_PTR<ServerContext> context, ServerServiceListenerEventType ev, RR_SHARED_PTR<void> param);

	protected:
		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekInValue(const uint32_t&) = 0;
		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekOutValue(const uint32_t&) = 0;
		virtual void do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec&, const uint32_t& ep) = 0;
		
	};

	template <typename T>
	class WireServer : public virtual WireServerBase, public virtual Wire<T>
	{

	public:

		WireServer(boost::string_ref name, RR_SHARED_PTR<ServiceSkel> skel, MemberDefinition_Direction direction = MemberDefinition_Direction_both, boost::function<void(RR_INTRUSIVE_PTR<RRValue>&)> verify = NULL) : WireServerBase(name, skel, direction), Wire<T>(verify)
		{
			if (boost::is_same<T, RR_INTRUSIVE_PTR<MessageElement> >::value)
			{
				rawelements = true;
			}
			else
			{
				rawelements = false;
			}
		}

		virtual ~WireServer() {}

		virtual void AsyncConnect(RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<WireConnection<T> >, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, -1, service_path, m_MemberName, "AsyncConnect is not valid for WireServer"); 
			throw InvalidOperationException("Not valid for server");
		}
		virtual RR_SHARED_PTR<WireConnection<T> > Connect()	
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, -1, service_path, m_MemberName, "Connect is not valid for WireServer");  
			throw InvalidOperationException("Not valid for server"); 
		}
		virtual T PeekInValue(TimeSpec& ts) 
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, -1, service_path, m_MemberName, "PeekInValue is not valid for WireServer");  
			throw InvalidOperationException("Not valid for server"); 
		}
		virtual T PeekOutValue(TimeSpec& ts) 
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, -1, service_path, m_MemberName, "PeekOutValue is not valid for WireServer");  
			throw InvalidOperationException("Not valid for server"); 
		}
		virtual void PokeOutValue(const T& value) 
		{ 
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, -1, service_path, m_MemberName, "PokeOutValue is not valid for WireServer"); 
			throw InvalidOperationException("Not valid for server"); 
		}
		virtual void AsyncPeekInValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) 
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, -1, service_path, m_MemberName, "AsyncPeekInValue is not valid for WireServer");  
			throw InvalidOperationException("Not valid for server"); 
		}
		virtual void AsyncPeekOutValue(RR_MOVE_ARG(boost::function<void(const T&, const TimeSpec&, RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE) 
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, -1, service_path, m_MemberName, "AsyncPeekOutValue is not valid for WireServer");  
			throw InvalidOperationException("Not valid for server"); 
		}
		virtual void AsyncPokeOutValue(const T& value, RR_MOVE_ARG(boost::function<void(RR_SHARED_PTR<RobotRaconteurException>)>) handler, int32_t timeout = RR_TIMEOUT_INFINITE)
		{
			ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, -1, service_path, m_MemberName, "AsyncPokeOutValue is not valid for WireServer");  
			throw InvalidOperationException("Not valid for server"); 
		}


		virtual boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> GetWireConnectCallback() {	return callback; }
		virtual void SetWireConnectCallback(boost::function<void(RR_SHARED_PTR<WireConnection<T> >)> function) { callback=function; }
		virtual boost::function<T(const uint32_t&)> GetPeekInValueCallback() { return peek_in_callback; }
		virtual void SetPeekInValueCallback(boost::function<T(const uint32_t&)> function) { peek_in_callback = function; }
		virtual boost::function<T(const uint32_t&)> GetPeekOutValueCallback() { return peek_out_callback; }
		virtual void SetPeekOutValueCallback(boost::function<T(const uint32_t&)> function) { peek_out_callback = function; }
		virtual boost::function<void(const T&, const TimeSpec&, const uint32_t&)> GetPokeOutValueCallback() { return poke_out_callback; }
		virtual void SetPokeOutValueCallback(boost::function<void(const T&, const TimeSpec&, const uint32_t&)> function) { poke_out_callback = function; }
		
	protected:
		virtual RR_SHARED_PTR<WireConnectionBase> CreateNewWireConnection(uint32_t e, MemberDefinition_Direction direction)
		{
			return RR_MAKE_SHARED<WireConnection<T> >(RR_STATIC_POINTER_CAST<WireBase>(shared_from_this()), e, direction);
		}

		boost::function<void (RR_SHARED_PTR<WireConnection<T> >)> callback;
		boost::function<T(const uint32_t&)> peek_in_callback;
		boost::function<T(const uint32_t&)> peek_out_callback;
		boost::function<void(const T&, const TimeSpec&, const uint32_t&)> poke_out_callback;

		virtual void fire_WireConnectCallback(RR_SHARED_PTR<WireConnectionBase> e)
		{
			if (!callback) return;
			callback(RR_STATIC_POINTER_CAST<WireConnection<T> >(e));
		}

		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekInValue(const uint32_t& ep)
		{
			if (!peek_in_callback) 
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, ep, service_path, m_MemberName, "Attempt to call PeekInValue when callback not set"); 
				throw InvalidOperationException("Invalid operation");
			}
			return RRPrimUtil<T>::PrePack(peek_in_callback(ep));
		}
		
		virtual RR_INTRUSIVE_PTR<RRValue> do_PeekOutValue(const uint32_t& ep)
		{
			if (!peek_out_callback)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, ep, service_path, m_MemberName, "Attempt to call PeekOutValue when callback not set"); 
				throw InvalidOperationException("Invalid operation");
			}
			return RRPrimUtil<T>::PrePack(peek_out_callback(ep));
		}

		virtual void do_PokeOutValue(const RR_INTRUSIVE_PTR<RRValue>& value, const TimeSpec& ts, const uint32_t& ep)
		{
			if (!poke_out_callback)
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, ep, service_path, m_MemberName, "Attempt to call PokeOutValue when callback not set"); 
				throw InvalidOperationException("Invalid operation");
			}
			return poke_out_callback(RRPrimUtil<T>::PreUnpack(value), ts, ep);
		}

	public:
		virtual void Shutdown()
		{
			WireServerBase::Shutdown();
			callback.clear();
			peek_in_callback.clear();
			peek_out_callback.clear();
			poke_out_callback.clear();
		}

	};

	namespace detail
	{
		template<typename T>
		class Wire_traits;

		template<typename T>
		class Wire_traits<Wire<T> >
		{
		public:
			typedef WireConnection<T> wireconnection_type;
			typedef WireClient<T> wireclient_type;
			typedef WireServer<T> wireserver_type;			
		};

	}

	namespace detail
	{
		class WireBroadcaster_connected_connection;
	}

	/**
	 * @brief Base class for WireBroadcaster
	 * 
	 * Base class for templated WireBroadcaster class
	 * 
	 */
	class ROBOTRACONTEUR_CORE_API WireBroadcasterBase : public RR_ENABLE_SHARED_FROM_THIS<WireBroadcasterBase>
	{
	public:

		size_t GetActiveWireConnectionCount();

		virtual ~WireBroadcasterBase();

		/**
		 * @brief Get the current predicate callback function
		 * 
		 * @return boost::function<bool(RR_SHARED_PTR<PipeBroadcasterBase>&, uint32_t, int32_t) > The predicate callback function
		 */
		boost::function<bool(RR_SHARED_PTR<WireBroadcasterBase>&, uint32_t)> GetPredicate();

		/**
		 * @brief Set the predicate callback function
		 * 
		 * A predicate is optionally used to regulate when values are sent to clients. This is used by the
		 * BroadcastDownsampler to regulate update rates of values sent to clients. 
		 * 
		 * The predicate callback is invoked before the broadcaster sets the OutValue of a connection. If the predicate returns 
		 * true, the OutValue packet will be sent. If it is false, the OutValue packet will not be sent to that endpoint. 
		 * The predicate callback must have the following signature:
		 * 
		 *     bool broadcaster_predicate(WireBroadcasterBasePtr& broadcaster, uint32_t client_endpoint);
		 * 
		 * It receives the broadcaster and the client endpoint ID. It returns true to send the OutValue packet,
		 * or false to not send the OutValue packet.
		 * 
		 * @param f The predicate callback function
		 */
		void SetPredicate(boost::function<bool(RR_SHARED_PTR<WireBroadcasterBase>&, uint32_t)> f);

		/** @copydoc WireConnectionBase::GetOutValueLifespan() */
		int32_t GetOutValueLifespan();

		/** @copydoc WireConnectionBase::SetOutValueLifespan() */
		void SetOutValueLifespan(int32_t millis);

	protected:

		WireBroadcasterBase();		

		void InitBase(RR_SHARED_PTR<WireBase> wire);

		void ConnectionClosedBase(RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> ep);

		void ConnectionConnectedBase(RR_SHARED_PTR<WireConnectionBase> ep);

		void SetOutValueBase(RR_INTRUSIVE_PTR<RRValue> value);

		virtual void AttachWireServerEvents(RR_SHARED_PTR<WireServerBase> w);

		virtual void AttachWireConnectionEvents(RR_SHARED_PTR<WireConnectionBase> w, RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> cep);

		RR_INTRUSIVE_PTR<RRValue> ClientPeekInValueBase();
		
		std::list<RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> > connected_wires;
		boost::mutex connected_wires_lock;
		RR_WEAK_PTR<WireServerBase> wire;
		RR_WEAK_PTR<RobotRaconteurNode> node;
		std::string service_path;
		std::string member_name;

		bool copy_element;

		boost::function<bool (RR_SHARED_PTR<WireBroadcasterBase>&, uint32_t)> predicate;

		RR_INTRUSIVE_PTR<RRValue> out_value;
		boost::initialized<bool> out_value_valid;

		int32_t out_value_lifespan;
		boost::posix_time::ptime out_value_lasttime_local;


		void ServiceEvent(ServerServiceListenerEventType evt);

		RR_SHARED_PTR<WireBase> GetWireBase();
	};



	/**
	 * @brief Broadcaster to send values to all connected clients
	 * 
	 * WireBroadcaster is used by services to send values to all
	 * connected client endpoints. It attaches to the wire on the service
	 * side, and manages the lifecycle of connections. WireBroadcaster
	 * should only we used with wires that are declared *readonly*, since
	 * it has no provisions for receiving incoming values from clients.
	 * 
	 * WireBroadcaster is initialized by the user, or by default implementation
	 * classes generated by RobotRaconteurGen (*_default_impl). Default
	 * implementation classes will automatically instantiate broadcasters for
	 * wires marked *readonly*. If default implementation classes are
	 * not used, the broadcaster must be instantiated manually. It is recommended this
	 * be done using the IRRServiceObject interface in the overridden 
	 * IRRServiceObject::RRServiceObjectInit() function. This function is called after
	 * the wires have been instantiated by the service.
	 * 
	 * Use SetOutValue() to broadcast values to all connected clients.
	 * 
	 * The rate that packets are sent can be regulated using a callback function configured
	 * with the SetPredicate() function, or using the BroadcastDownsampler class.
	 * 
	 * @tparam T The value data type
	 */
	template <typename T>
	class WireBroadcaster : public WireBroadcasterBase
	{		
	public:

		/**
		 * @brief Construct a new WireBroadcaster
		 * 
		 * Must use boost::make_shared<WireBroadcaster<T> > to construct.
		 * Must call Init() after construction.
		 * 
		 */
		WireBroadcaster() {}

		/**
		 * @brief Initialize the WireBroadcaster
		 * 
		 * Initialize the WireBroadcaster for use. Must be called after construction.
		 * 
		 * @param wire The wire to use for broadcasting. Must be a wire from a service object.
		 * Specifying a client wire will result in an exception.
		 */
		void Init(RR_SHARED_PTR<Wire<T> > wire)
		{			
			InitBase(wire);
		}

		/**
		 * @brief Set the OutValue for all connections
		 * 
		 * Sets the OutValue for all connections. This will transmit the value 
		 * to all connected clients using packets. The value will become the clients'
		 * InValue.
		 * 
		 * The value will be returned when clients call Wire::PeekInValue() or 
		 * Wire::AsyncPeekInValue()
		 *  
		 * @param value The new OutValue
		 */
		void SetOutValue(T value)
		{
			SetOutValueBase(RRPrimUtil<T>::PrePack(value));
		}

		RR_SHARED_PTR<Wire<T> > GetWire()
		{
			return rr_cast<Wire<T> >(GetWireBase());
		}

	protected:

		T ClientPeekInValue()
		{
			return RRPrimUtil<T>::PreUnpack(ClientPeekInValueBase());
		}

		static T ClientPeekOutValue() { throw ReadOnlyMemberException("Read only wire"); }
		static T ClientPokeOutValue() { throw ReadOnlyMemberException("Read only wire"); }

		virtual void AttachWireServerEvents(RR_SHARED_PTR<WireServerBase> w)
		{
			RR_SHARED_PTR < WireServer<T> > w_T= rr_cast<WireServer<T> >(w);
			w_T->SetWireConnectCallback(boost::bind(&WireBroadcaster::ConnectionConnectedBase, this->shared_from_this(), RR_BOOST_PLACEHOLDERS(_1)));
			w_T->SetPeekInValueCallback(boost::bind(&WireBroadcaster<T>::ClientPeekInValue, RR_STATIC_POINTER_CAST<WireBroadcaster<T> >(this->shared_from_this())));
			w_T->SetPeekOutValueCallback(boost::bind(&WireBroadcaster<T>::ClientPeekOutValue));
			w_T->SetPokeOutValueCallback(boost::bind(&WireBroadcaster<T>::ClientPokeOutValue));
		}

		virtual void AttachWireConnectionEvents(RR_SHARED_PTR<WireConnectionBase> w, RR_SHARED_PTR<detail::WireBroadcaster_connected_connection> c)
		{
			RR_SHARED_PTR<WireConnection<T> > w_T = rr_cast<WireConnection<T> >(w);
			w_T->SetWireConnectionClosedCallback(boost::bind(&WireBroadcaster::ConnectionClosedBase, this->shared_from_this(), c));
		}


	};

	namespace detail
	{
		static void WireUnicastReceiverBase_empty_close_handler(RR_SHARED_PTR<RobotRaconteurException> err)
		{

		}
	}
	
	template <typename T, typename U>
	class WireUnicastReceiverBase : public RR_ENABLE_SHARED_FROM_THIS<WireUnicastReceiverBase<T,U> >
	{
	public:

		typedef typename detail::Wire_traits<T>::wireserver_type wireserver_type;
		typedef typename detail::Wire_traits<T>::wireconnection_type wireconnection_type;

		/**
		 * @brief Construct a new WireUnicastReceiverBase
		 * 
		 * Must use boost::make_shared<WireUnicastReceiver<T> > to construct.
		 * Must call Init() after construction.
		 * 
		 */
		WireUnicastReceiverBase() {}
		virtual ~WireUnicastReceiverBase() {}

		/**
		 * @brief Initialize the WireUnicastReceiver
		 * 
		 * Initialize the WireUnicastReceiver. Must be called after construction.
		 * 
		 * @param wire The wire to use for broadcasting. Must be a wire from a service object.
		 * Specifying a client wire will result in an exception.
		 */
		void Init(RR_SHARED_PTR<T> wire)
		{
			node = wire->GetNode();
			in_value_lifespan = -1;
			RR_SHARED_PTR<wireserver_type> wire_server = RR_DYNAMIC_POINTER_CAST<wireserver_type>(wire);			
			if (!wire_server) 
			{
				ROBOTRACONTEUR_LOG_DEBUG_COMPONENT_PATH(node, Member, -1, service_path, member_name, "WireUnicastReceiver init must be passed a WireServer");
				throw InvalidOperationException("WireServer required for WireUnicastReceiver");
			}
			this->wire = wire_server;
			wire_server->SetWireConnectCallback(boost::bind(&WireUnicastReceiverBase<T,U>::ConnectionConnected, this->shared_from_this(), RR_BOOST_PLACEHOLDERS(_1)));
			wire_server->SetPeekInValueCallback(boost::bind(&WireUnicastReceiverBase<T,U>::ClientPeekInValue));
			wire_server->SetPeekOutValueCallback(boost::bind(&WireUnicastReceiverBase<T,U>::ClientPeekOutValue, this->shared_from_this()));
			wire_server->SetPokeOutValueCallback(boost::bind(&WireUnicastReceiverBase<T,U>::ClientPokeOutValue, this->shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3)));

			wire_server->GetSkel()->GetContext()->ServerServiceListener.connect(
				boost::signals2::signal<void(RR_SHARED_PTR<ServerContext>, ServerServiceListenerEventType, RR_SHARED_PTR<void>)>::slot_type(
					boost::bind(&WireUnicastReceiverBase::ServiceEvent, this, RR_BOOST_PLACEHOLDERS(_2))
				).track(this->shared_from_this())
			);

			this->service_path = wire_server->GetServicePath();
			this->member_name = wire_server->GetMemberName();

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, -1, service_path, member_name, "WireUnicastReceiver initialized");
		}

		/**
		 * @brief Get the current InValue
		 * 
		 * Gets the current InValue that was received from the active connection.
		 * Throws ValueNotSetException if no value has been received, or
		 * the most recent value lifespan has expired.
		 * 
		 * @param ts [out] The current InValue timestamp
		 * @param ep [out] The client endpoint ID of the InValue
		 * @return U The current InValue
		 */
		U GetInValue(TimeSpec& ts, uint32_t& ep)
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (!in_value_valid.data()) throw ValueNotSetException("Value not set");
			if(detail::WireConnectionBase_IsValueExpired(node, in_value_lasttime_local,in_value_lifespan))
			{
				throw ValueNotSetException("Value expired");
			}
			ts = in_value_ts;
			ep = in_value_ep;
			return in_value;
		}

		/**
		 * @brief Try getting the current InValue, returning true on success or false on failure
		 * 
		 * Gets the current InValue, its timestamp, and the client endpoint ID. Returns true if
		 * value is valid, or false if value is invalid. Value will be invalid if no value has
		 * been received, or the value lifespan has expired.
		 * 
		 * @param value [out] The current InValue
		 * @param ts [out] The current InValue timestamp
		 * @param ep [out] The client endpoint ID of the InValue
		 * @return true value is valid
		 * @return false value is invalid
		 */
		bool TryGetInValue(U& value, TimeSpec& ts, uint32_t& ep)
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (!in_value_valid) return false;
			if(detail::WireConnectionBase_IsValueExpired(node, in_value_lasttime_local,in_value_lifespan))
			{
				return false;
			}
			value = in_value;
			ts = in_value_ts;
			ep = in_value_ep;
			return true;
		}

		/**
		 * @brief Signal invoked when the InValue has changed
		 * 
		 * Callback function must accept three arguments, receiving the new value, 
		 * value's TimeSpec timestamp, and the client endpoint ID.
		 */
		boost::signals2::signal<void(const U&, const TimeSpec&, const uint32_t&)> InValueChanged;
		
		/** @copydoc WireConnectionBase::GetInValueLifespan() */
		int32_t GetInValueLifespan()
		{
			boost::mutex::scoped_lock lock(this_lock);
			return in_value_lifespan;
		}

		/** @copydoc WireConnectionBase::SetInValueLifespan() */
		void SetInValueLifespan(int32_t millis)
		{
			boost::mutex::scoped_lock lock(this_lock);
			in_value_lifespan = millis;
		}

		RR_SHARED_PTR<T> GetWire()
		{
			return wire;
		}

	protected:

		void ConnectionConnected(RR_SHARED_PTR<wireconnection_type > connection)
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (active_connection)
			{
				uint32_t active_ep = active_connection->GetEndpoint();
				try
				{
					active_connection->AsyncClose(&detail::WireUnicastReceiverBase_empty_close_handler);
				}
				catch (std::exception&) {}
				active_connection.reset();

				ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, active_ep, service_path, member_name, "WireUnicastReceiver active wire closed for new connection");
			}
			active_connection = connection;
			connection->SetWireConnectionClosedCallback(boost::bind(&WireUnicastReceiverBase<T,U>::ConnectionClosed, this->shared_from_this(), RR_BOOST_PLACEHOLDERS(_1)));			
			connection->WireValueChanged.connect(boost::bind(&WireUnicastReceiverBase<T,U>::ConnectionInValueChanged, this->shared_from_this(), RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3)));

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, -1, service_path, member_name, "WireUnicastReceiver wire connected, made active wire");
		}

		void ConnectionClosed(RR_SHARED_PTR<wireconnection_type> connection)
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (active_connection == connection)
			{
				active_connection.reset();				
			}
		}

		void ConnectionInValueChanged(RR_SHARED_PTR<wireconnection_type> connection, const U& value, const TimeSpec& time)
		{
			ClientPokeOutValue(value, time, connection->GetEndpoint());
		}

		static U ClientPeekInValue()
		{
			throw WriteOnlyMemberException("Write only wire");
		}

		U ClientPeekOutValue()
		{
			boost::mutex::scoped_lock lock(this_lock);
			if (!in_value_valid) throw ValueNotSetException("Value not set");
			return in_value;
		}

		void ClientPokeOutValue(const U& value, const TimeSpec& ts, const uint32_t& ep)
		{
			RR_SHARED_PTR<RobotRaconteurNode> n = node.lock();
			boost::mutex::scoped_lock lock(this_lock);
			in_value = value;
			in_value_ts = ts;
			in_value_valid.data() = true;
			in_value_ep.data() = ep;
			if(n)
			{
				in_value_lasttime_local = n->NowNodeTime();
			}

			lock.unlock();

			InValueChanged(value, ts, ep);

			ROBOTRACONTEUR_LOG_TRACE_COMPONENT_PATH(node, Member, ep, service_path, member_name, "WireUnicastReceiver value changed");
		}

		void ServiceEvent(ServerServiceListenerEventType evt)
		{
			if (evt != ServerServiceListenerEventType_ServiceClosed) return;
			boost::mutex::scoped_lock lock(this_lock);
			InValueChanged.disconnect_all_slots();			
		}

		RR_SHARED_PTR<wireserver_type> wire;
		RR_SHARED_PTR<wireconnection_type> active_connection;
		boost::mutex this_lock;
		U in_value;
		TimeSpec in_value_ts;
		boost::initialized<bool> in_value_valid;
		boost::initialized<uint32_t> in_value_ep;
		boost::posix_time::ptime in_value_lasttime_local;
		int32_t in_value_lifespan;

		std::string member_name;
		std::string service_path;
		RR_WEAK_PTR<RobotRaconteurNode> node;

	};

	/**
	 * @brief Receive the InValue from the most recent connection
	 * 
	 * WireUnicastReceiver is used by services to receive a value from a single client.
	 * When a client sets its OutValue, this value is transmitted to the service using
	 * packets, and becomes the service's InValue for that connection. Service wires
	 * can have multiple active clients, so the service needs to choose which connection
	 * is "active". The WireUnicastReceiver selects the "most recent" connection, and
	 * returns that connection's InValue. Any existing connections are closed.
	 * WireUnicastReceiver should only be used with wires that are declared *writeonly*.
	 * It is recommended that object locks be used to protect from concurrent
	 * access when unicast receivers are used.
	 *
	 * WireUnicastReceiver is initialized by the user, or by default implementation
	 * classes generated by RobotRaconteurGen (*_default_impl). Default
	 * implementation classes will automatically instantiate unicast receivers for
	 * wires marked *writeonly*. If default implementation classes are
	 * not used, the unicast receiver must be instantiated manually. It is recommended this
	 * be done using the IRRServiceObject interface in the overridden 
	 * IRRServiceObject::RRServiceObjectInit() function. This function is called after
	 * the wires have been instantiated by the service.
	 * 
	 * The current InValue is received using GetInValue() or TryGetInValue(). The
	 * InValueChanged signal can be used to monitor for changes to the InValue.
	 * 
	 * Clients may also use PokeOutValue() or AsyncPokeOutValue() to update the
	 * unicast receiver's value.
	 *  
	 * @tparam T The value type
	 */
	template <typename T>
	class WireUnicastReceiver : public WireUnicastReceiverBase<Wire<T>, T>
	{

	};
#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
	/** @brief Convenience alias for WireConnectionBase shared_ptr */
	using WireConnectionBasePtr = RR_SHARED_PTR<WireConnectionBase>;
	/** @brief Convenience alias for WireConnection shared_ptr */
	template <typename T> using WireConnectionPtr = RR_SHARED_PTR<WireConnection<T> >;
	/** @brief Convenience alias for WireBase shared_ptr */
	using WireBasePtr = RR_SHARED_PTR<WireBase>;
	/** @brief Convenience alias for Wire shared_ptr */
	template <typename T> using WirePtr = RR_SHARED_PTR<Wire<T> >;
	/** @brief Convenience alias for WireBroadcaster shared_ptr */
	template <typename T> using WireBroadcasterPtr = RR_SHARED_PTR<WireBroadcaster<T> >;
	/** @brief Convenience alias for WireUnicastReceiver shared_ptr */
	template <typename T> using WireUnicastReceiverPtr = RR_SHARED_PTR<WireUnicastReceiver<T> >;
#endif

}

#pragma warning(pop)
