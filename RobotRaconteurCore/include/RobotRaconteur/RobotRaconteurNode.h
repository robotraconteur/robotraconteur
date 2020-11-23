/** 
 * @file RobotRaconteurNode.h
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
#include "RobotRaconteur/NodeID.h"
#include "RobotRaconteur/Message.h"
#include "RobotRaconteur/ServiceFactory.h"
#include "RobotRaconteur/Transport.h"
#include "RobotRaconteur/AutoResetEvent.h"
#include "RobotRaconteur/Error.h"
#include "RobotRaconteur/AsyncUtils.h"
#include "RobotRaconteur/ThreadPool.h"
#include "RobotRaconteur/Timer.h"
#include "RobotRaconteur/Discovery.h"
#include "RobotRaconteur/DataTypesPacking.h"
#include "RobotRaconteur/Logging.h"
#include "RobotRaconteur/Tap.h"

#include <queue>
#include <boost/bind/placeholders.hpp>
#include <boost/asio.hpp>
#include <boost/unordered_map.hpp>
#include <boost/function.hpp>
#include <boost/bind/protect.hpp>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/chrono.hpp>

#ifdef ROBOTRACONTEUR_WINDOWS

#undef SendMessage

#endif

namespace boost
{
namespace random
{
	class random_device;
}
}

namespace RobotRaconteur
{

	/**
	 * @brief The type of object lock
	 */
	enum RobotRaconteurObjectLockFlags
	{
		/**
		 * @brief User level lock
		 * 
		 * The object will be accesible for all client connections
		 * authenticated by the current user
		 */
		RobotRaconteurObjectLockFlags_USER_LOCK = 0,
		/**
		 * @brief Client level lock
		 * 
		 * Only the current client connection will have access 
		 * to the locked object
		 */
		RobotRaconteurObjectLockFlags_CLIENT_LOCK
	};
	
	class ROBOTRACONTEUR_CORE_API ServiceSecurityPolicy;
	class ROBOTRACONTEUR_CORE_API ServiceInfo2Subscription;
	class ROBOTRACONTEUR_CORE_API ServiceSubscription;
	class ROBOTRACONTEUR_CORE_API ServiceSubscriptionFilter;

	namespace detail
	{
		class Discovery;
	}

	/**
	 * @brief The central node implementation
	 * 
	 * RobotRaconteurNode implements the current Robot Raconteur instance
	 * and acts as the central switchpoint for the instance. The user 
	 * registers types, connects clients, registers services, and 
	 * registers transports through this class.
	 * 
	 * If the current program only needs one instance of RobotRaconteurNode,
	 * the singleton can be used. The singleton is accessed using:
	 *  
	 * `RobotRaconteurNode::s()`
	 * 
	 * or
	 * 
	 * `RobotRaconteurNode::sp()`
	 * 
	 * The node must be shut down before existing the program,
	 * or a memory leak/hard crash will occur. This can either be
	 * accomplished manually using the `Shutdown()` function,
	 * or automatically by using the ClientNodeSetup or
	 * ServerNodeSetup classes.
	 *  
	 */
	class ROBOTRACONTEUR_CORE_API RobotRaconteurNode : boost::noncopyable, public RR_ENABLE_SHARED_FROM_THIS<RobotRaconteurNode>
	{
	private:
		
		bool is_shutdown;
		boost::mutex shutdown_lock;
		boost::signals2::signal<void ()> shutdown_listeners;
				
		static bool is_init;

		bool instance_is_init;
		static boost::mutex init_lock;

	public:

		friend class detail::Discovery;
		friend class Transport;
		
		/**
		 * @brief Initialize the node. Called automatically for `s()` and `sp()`
		 * 
		 * This function must be called to initialize background tasks before
		 * using the node. It is called automatically by the `s()` and `sp()`
		 * singleton accesors, so the user only needs to call this function
		 * when not using the singleton. If a custom thread pool is being
		 * used, the thread pool factory must be specified before 
		 * calling init.
		 */
		void Init();

		/**
		 * @brief Construct a new Robot Raconteur Node object
		 * 
		 * Must be called using RR_MAKE_SHARED<RobotRaconteurNode>.
		 * Call `Init()` after construction.
		 */
		RobotRaconteurNode();

		virtual ~RobotRaconteurNode();
		
		/**
		 * @brief Singleton accessor
		 * 
		 * The RobotRaconteurNode singleton can be used when only
		 * one instance of Robot Raconteur is required in a program.
		 * The singleton must be shut down when the program exits.
		 * 
		 * This function returns a pointer to the singleton node.
		 * Using `sp()` if a shared_ptr is required.
		 * 
		 * @return RobotRaconteurNode* 
		 */
		static RobotRaconteurNode* s();

		/**
		 * @brief Singleton shared_ptr accessor
		 * 
		 * Same as `s()`, but returns a shared_ptr instead of
		 * a plain pointer.
		 * 
		 * @return RR_SHARED_PTR<RobotRaconteurNode>
		 */
		static RR_SHARED_PTR<RobotRaconteurNode> sp();

		/**
		 * @brief Singleton weak_ptr accessor
		 * 
		 * Same as `sp()`, but returns a weak_ptr instead of
		 * a shared_ptr.
		 * 
		 * @return RR_SHARED_PTR<RobotRaconteurNode>
		 */
		static RR_WEAK_PTR<RobotRaconteurNode> weak_sp();

		/**
		 * @brief Get the current NodeID
		 * 
		 * Gets the current NodeID. If one has not been set using `SetNodeID()`,
		 * one will be automatically generated.
		 * 
		 * @return NodeID 
		 */
		RobotRaconteur::NodeID NodeID();

		/**
		 * @brief Get the current NodeName
		 * 
		 * Gets the current NodeName. If one has not been set using `SetNodeName()`,
		 * it will be an empty string.
		 * 
		 * @return std::string 
		 */
		std::string NodeName();

		/**
		 * @brief Set the NodeID
		 * 
		 * The NodeID must be set before `NodeID()` is called. If an attempt to set 
		 * the NodeID after `NodeID()` has been called, an InvalidOperationException
		 * will be thrown.
		 * 
		 * The NodeID must not be all zeros.
		 * 
		 * @param id 
		 */
		void SetNodeID(const RobotRaconteur::NodeID& id);

		/**
		 * @brief Set the NodeName
		 * 
		 * The NodeName must be set before calling `NodeName()` If an attempt to set 
		 * the NodeName after `NodeName()` has been called, an InvalidOperationException
		 * will be thrown.
		 * 
		 * The NodeName must not be empty, and must conform to the following regex:
		 * 
		 * `^[a-zA-Z][a-zA-Z0-9_\\.\\-]*$`
		 * 
		 * @param name 
		 */
		void SetNodeName(boost::string_ref name);

		/**
		 * @brief Tries getting the current NodeID
		 * 
		 * Tries to get the NodeID. Returns false if no
		 * NodeID has been configured
		 * 
		 * @param id the NodeID if set
		 * 
		 * @return bool
		 */
		bool TryGetNodeID(RobotRaconteur::NodeID& id);

		/**
		 * @brief Tries getting the current NodeName
		 * 
		 * Gets the current NodeName. Returns false if NodeName is
		 * empty
		 * 
		 * @param node_name the NodeName if not empty
		 * 
		 * @return bool
		 */
		bool TryGetNodeName(std::string& node_name);

		/**
		 * @brief Returns a previously registered service type
		 * 
		 * @param type The name of the service type to retrieve
		 * @return RR_SHARED_PTR<ServiceFactory>
		 */
		RR_SHARED_PTR<ServiceFactory> GetServiceType(boost::string_ref type);
		
		/**
		 * @brief Check if a service type has been registered
		 * 
		 * @param type The name of the service to check
		 * @return true 
		 * @return false 
		 */
		bool IsServiceTypeRegistered(boost::string_ref type);

		/**
		 * @brief Register a service type
		 * 
		 * @param factory The service factory implementing the type to register
		 */
		void RegisterServiceType(RR_SHARED_PTR<ServiceFactory> factory);


		/**
		 * @brief Unregister a previously registered service type
		 * 
		 * This function is not recommended as the results can be 
		 * unpredicatle
		 * 
		 * @param type 
		 */
		void UnregisterServiceType(boost::string_ref type);

		/**
		 * @brief Return names of registered service types
		 * 
		 * @return std::vector<std::string> 
		 */
		std::vector<std::string> GetRegisteredServiceTypes();

		/**
		 * @internal
		 * 
		 * @brief Packs a structure for use in a MessageElement
		 * 
		 * Finds the correct ServiceFactory to pack the structure, then
		 * uses the ServiceFactory to pack the structure to a
		 * MessageElementNestedElementList for use in a MessageElement.
		 * 
		 * Throws ServiceException if the correct ServiceFactory cannot
		 * be found.
		 * 
		 * @param structure 
		 * @return RR_INTRUSIVE_PTR<MessageElementNestedElementList> 
		 */
		RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackStructure(RR_INTRUSIVE_PTR<RRStructure> structure);

		/**
		 * @internal
		 * 
		 * @brief Unpacks a structure from a MessageElementNestedElementList
		 *		 
		 * structure must have type code DataTypes_structure_t. Finds the correct
		 * ServiceFactory to unpack the structure, then uses the ServiceFactory
		 * to unpack the structure.
		 * 
		 * Throws ServiceException if the correct ServiceFactory cannot
		 * be found.
		 * 
		 * @param structure 
		 * @return RR_INTRUSIVE_PTR<RRStructure> 
		 */
		RR_INTRUSIVE_PTR<RRStructure> UnpackStructure(RR_INTRUSIVE_PTR<MessageElementNestedElementList> structure);

		/**
		 * @internal
		 * 
		 * @brief Packs a pod array for use in a MessageElement
		 * 
		 * Pods must be stored
		 * in an RRPodArray before serialization. (Their "boxed" form.)
		 * Finds the correct ServiceFactory to pack the pod_array, then
		 * uses the ServiceFactory to pack the pod_array to a
		 * MessageElementNestedElementList for use in a MessageElement.
		 * 
		 * Throws ServiceException if the correct ServiceFactory cannot
		 * be found.
		 * 
		 * @param pod_array
		 * @return RR_INTRUSIVE_PTR<MessageElementNestedElementList> 
		 */
		RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackPodArray(RR_INTRUSIVE_PTR<RRPodBaseArray> pod_array);

		/**
		 * @internal
		 * 
		 * @brief Unpacks a pod array type from a MessageElementNestedElementList
		 * 
		 * pod_array must have type code DataTypes_pod_array_t. Finds the correct
		 * ServiceFactory to unpack the pod array, then uses the ServiceFactory
		 * to unpack the pod array.
		 * 
		 * Throws ServiceException if the correct ServiceFactory cannot
		 * be found.
		 * 
		 * @param pod_array 
		 * @return RR_INTRUSIVE_PTR<RRPodBaseArray> 
		 */
		RR_INTRUSIVE_PTR<RRPodBaseArray> UnpackPodArray(RR_INTRUSIVE_PTR<MessageElementNestedElementList> pod_array);
		
		/**
		 * @internal
		 * 
		 * @brief Packs a pod multidimarray for use in a MessageElement
		 * 
		 * Finds the correct ServiceFactory to pack the pod_multiarray, then
		 * uses the ServiceFactory to pack the pod_multiarray to a
		 * MessageElementNestedElementList for use in a MessageElement.
		 * 
		 * Throws ServiceException if the correct ServiceFactory cannot
		 * be found.
		 * 
		 * @param pod_multiarray
		 * @return RR_INTRUSIVE_PTR<MessageElementNestedElementList> 
		 */
		RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackPodMultiDimArray(RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> pod_multiarray);

		/**
		 * @internal
		 * 
		 * @brief Unpacks a pod multidimarray type from a MessageElementNestedElementList
		 * 
		 * pod_multiarray must have type code DataTypes_pod_multidimarray_t. Finds the correct
		 * ServiceFactory to unpack the pod multidimarray, then uses the ServiceFactory
		 * to unpack the pod multidimarray.
		 * 
		 * Throws ServiceException if the correct ServiceFactory cannot
		 * be found.
		 * 
		 * @param pod_multiarray 
		 * @return RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> 
		 */
		RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> UnpackPodMultiDimArray(RR_INTRUSIVE_PTR<MessageElementNestedElementList> pod_multiarray);

		/**
		 * @internal
		 * 
		 * @brief Packs a named array for use in a MessageElement
		 * 
		 * Named arrays must be stored
		 * in an RRNamedArray before serialization. (Their "boxed" form.)
		 * Finds the correct ServiceFactory to pack the namedarray, then
		 * uses the ServiceFactory to pack the namedarray to a
		 * MessageElementNestedElementList for use in a MessageElement.
		 * 
		 * Throws ServiceException if the correct ServiceFactory cannot
		 * be found.
		 * 
		 * @param namedarray
		 * @return RR_INTRUSIVE_PTR<MessageElementNestedElementList> 
		 */
		RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackNamedArray(RR_INTRUSIVE_PTR<RRNamedBaseArray> namedarray);

		/**
		 * @internal
		 * 
		 * @brief Unpacks a named array type from a MessageElementNestedElementList
		 * 
		 * namedarray must have type code DataTypes_namedarray_array_t. Finds the correct
		 * ServiceFactory to unpack the named array, then uses the ServiceFactory
		 * to unpack the named array.
		 * 
		 * Throws ServiceException if the correct ServiceFactory cannot
		 * be found.
		 * 
		 * @param namedarray 
		 * @return RR_INTRUSIVE_PTR<RRNamedBaseArray> 
		 */
		RR_INTRUSIVE_PTR<RRNamedBaseArray> UnpackNamedArray(RR_INTRUSIVE_PTR<MessageElementNestedElementList> namedarray);

		/**
		 * @internal
		 * 
		 * @brief Packs a named multidimarray for use in a MessageElement
		 * 
		 * Finds the correct ServiceFactory to pack the named_multiarray, then
		 * uses the ServiceFactory to pack the named_multiarray to a
		 * MessageElementNestedElementList for use in a MessageElement.
		 * 
		 * Throws ServiceException if the correct ServiceFactory cannot
		 * be found.
		 * 
		 * @param named_multiarray
		 * @return RR_INTRUSIVE_PTR<MessageElementNestedElementList> 
		 */
		RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackNamedMultiDimArray(RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> named_multiarray);

		/**
		 * @internal
		 * 
		 * @brief Unpacks a named multidimarray type from a MessageElementNestedElementList
		 * 
		 * named_multiarray must have type code DataTypes_namedarray_multidimarray_t. Finds the correct
		 * ServiceFactory to unpack the named multidimarray, then uses the ServiceFactory
		 * to unpack the named multidimarray.
		 * 
		 * Throws ServiceException if the correct ServiceFactory cannot
		 * be found.
		 * 
		 * @param named_multiarray 
		 * @return RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> 
		 */
		RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> UnpackNamedMultiDimArray(RR_INTRUSIVE_PTR<MessageElementNestedElementList> named_multiarray);


		/**
		 * @internal
		 * 
		 * @brief Packs a numeric primitive MultiDimArray for use in a MessageElement
		 * 
		 * @tparam T The numeric type
		 * @param arr The RRMultiDimArray to pack
		 * @return RR_INTRUSIVE_PTR<MessageElementNestedElementList> 
		 */
		template <typename T>
		RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackMultiDimArray(RR_INTRUSIVE_PTR<RRMultiDimArray<T> > arr)
		{
			return detail::packing::PackMultiDimArray<T>(arr);
		}

		/**
		 * @internal
		 * 
		 * @brief Unpacks a numeric primitive multidimarray from a MessageElementNestedElementList
		 * 
		 * The numeric primitive
		 * type is determined by the data type of the "array" nested MessageElement.
		 * 
		 * @tparam T The expected numeric type
		 * @param ar 
		 * @return RR_INTRUSIVE_PTR<RRMultiDimArray<T> > 
		 */
		template <typename T>
		RR_INTRUSIVE_PTR<RRMultiDimArray<T> > UnpackMultiDimArray(RR_INTRUSIVE_PTR<MessageElementNestedElementList> ar)
		{
			return detail::packing::UnpackMultiDimArray<T>(ar);
		}

	
		/**
		 * @internal
		 * 
		 * @brief Packs a varvalue type to a MessageElementNestedElementList
		 * 
		 * If vardata contains user defined types, the required ServiceFactory is found.
		 * If the required ServiceFactory cannot be found, a ServiceException is thrown.
		 * 
		 * @param vardata 
		 * @return RR_INTRUSIVE_PTR<MessageElementData> 
		 */
		RR_INTRUSIVE_PTR<MessageElementData> PackVarType(RR_INTRUSIVE_PTR<RRValue> vardata);

		/**
		 * @internal
		 * 
		 * @brief Unpacks a varvalue type from a MessageElement
		 * 
		 * The type of the data stored in the MessageElement is determined using the
		 * information contained in the MessageElement header. If the MessageElement 
		 * contains user defined types, the required ServiceFactory is found. If
		 * the required ServiceFactory cannot be found, a ServiceException is thrown.
		 * 
		 * @param mvardata 
		 * @return RR_INTRUSIVE_PTR<RRValue> 
		 */
		RR_INTRUSIVE_PTR<RRValue> UnpackVarType(RR_INTRUSIVE_PTR<MessageElement> mvardata);
	
	public:

		/**
		 * @internal
		 * 
		 * @brief Packs a map container type to a MessageElementNestedElementList
		 * 
		 * Will throw ServiceException if a required ServiceFactory is not found.
		 * 
		 * @tparam K The key type. Must be int32_t or std::string
		 * @tparam T The type of the data stored in the map
		 * @tparam U The type of the map variable, normally determined implicitly
		 * @param map
		 * @return RR_INTRUSIVE_PTR<MessageElementNestedElementList> 
		 */
		template<typename K, typename T, typename U>
		RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackMapType(const U& map)
		{
			return detail::packing::PackMapType<K,T,U>(map,this);
		}

		/**
		 * @internal
		 * 
		 * @brief Unpacks a map container type from a MessageElementNestedElementList
		 * 
		 * Will throw ServiceException if a required ServiceFactory is not found.
		 * 
		 * @tparam K The expected key type. Must be int32_t or std::string
		 * @tparam T The expected type of the data stored in the map
		 * @param map 
		 * @return RR_INTRUSIVE_PTR<RRMap<K,T> > 
		 */
		template<typename K, typename T>
		RR_INTRUSIVE_PTR<RRMap<K,T> > UnpackMapType(const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& map)
		{
			return detail::packing::UnpackMapType<K,T>(map,this);
		}	


		/**
		 * @internal
		 * 
		 * @brief Packs a list container type to a MessageElementNestedElementList
		 * 
		 * Will throw ServiceException if a required ServiceFactory is not found.
		 * 
		 * @tparam T The type of the data stored in the list
		 * @tparam U The type of the list variable, normally determined implicitly
		 * @param list
		 * @return RR_INTRUSIVE_PTR<MessageElementNestedElementList > 
		 */
		template<typename T, typename U>
		RR_INTRUSIVE_PTR<MessageElementNestedElementList > PackListType(U& list)
		{
			return detail::packing::PackListType<T, U>(list,this);
		}

		/**
		 * @internal
		 * 
		 * @brief Unpacks a list container type from a MessageElementNestedElementList
		 * 
		 * Will throw ServiceException if a required ServiceFactory is not found.
		 * 
		 * @tparam T The expected type of the data contained in the list
		 * @param list
		 * @return RR_INTRUSIVE_PTR<RRList<T> > 
		 */
		template<typename T>
		RR_INTRUSIVE_PTR<RRList<T> > UnpackListType(const RR_INTRUSIVE_PTR<MessageElementNestedElementList >& list)
		{
			return detail::packing::UnpackListType<T>(list, this);
		}

	public:
		
		/**
		 * @internal
		 * 
		 * @brief Packs a value to a MessageElementData using template logic
		 * 
		 * The correct packing function 
		 * is determined at compile time based on T. Will throw ServiceException if a required
		 * ServiceFactory cannot be found.
		 * 
		 * @tparam T The type of the data being packed
		 * @tparam U The type of the variable data
		 * @param data 
		 * @return RR_INTRUSIVE_PTR<MessageElementData> 
		 */
		template<typename T, typename U> RR_INTRUSIVE_PTR<MessageElementData> PackAnyType(const RR_INTRUSIVE_PTR<U>& data)
		{
			return detail::packing::PackAnyType<T,U>(data,this);
		}

		/**
		 * @internal
		 * 
		 * @brief Unpacks a value from a MessageElement based on an expected type using template logic
		 * 
		 * 
		 * The correct unpacking function is determined at compile time
		 * based on T. Will throw ServiceException if a required ServiceFactory
		 * cannot be found. 
		 * 
		 * @tparam T The type of the data being unpacked
		 * @param mdata 
		 * @return T 
		 */
		template<typename T> T UnpackAnyType(const RR_INTRUSIVE_PTR<MessageElement>& mdata)
		{
			return detail::packing::UnpackAnyType<T>(mdata,this);
		}
		
		/**
		 * @brief Shuts down the node. Called automatically by ClientNodeSetup and ServerNodeSetup
		 * 
		 * Shutdown must be called before program exit to avoid segfaults and other undefined
		 * behavior. The use of ClientNodeSetup and ServerNodeSetup is recommended to automate
		 * the node lifecycle. Calling this function does the following:
		 * 1. Closes all services and releases all service objects
		 * 2. Closes all client connections
		 * 3. Shuts down discovery
		 * 4. Shuts down all transports
		 * 5. Notifies all shutdown listeners
		 * 6. Releases all periodic cleanup task listeners
		 * 7. Shuts down and releases the thread pool
		 *  
		 * Requires Multithreading
		 * 
		 */
		virtual void Shutdown();

		/**
		 * @brief signals2 connection type for AddShutdownListener()
		 * 
		 */
		typedef boost::signals2::connection shutdown_listener_connection;

		/**
		 * @brief Adds a shutdown listener
		 * 
		 * Adds a listener handler function that will be called when the 
		 * node is shutting down.
		 * 
		 * @tparam Handler 
		 * @return shutdown_listener_connection 
		 */
		template<typename Handler>
		shutdown_listener_connection AddShutdownListener(BOOST_ASIO_MOVE_ARG(Handler) h)
		{
			return shutdown_listeners.connect(h);
		}
				
		/**
		 * @brief Register a transport for use by the node
		 * 
		 * @param transport 
		 * @return uint32_t The transport internal id
		 */
		uint32_t RegisterTransport(RR_SHARED_PTR<Transport> transport);

		/**
		 * @internal
		 * 
		 * @brief Asynchronously sends a message to another node
		 * 
		 * The destination is determined by 
		 * the LocalEndpoint in the MessageHeader. handler is called after the
		 * message has been sent by the transport (but before it has arrived).
		 * If an error occurs, the exception will be passed to the handler. Otherwise
		 * the exception will be null.
		 * 
		 * @param m The message to send
		 * @param handler The handler called after the message has been sent
		 */
		void AsyncSendMessage(RR_INTRUSIVE_PTR<Message> m, boost::function<void (RR_SHARED_PTR<RobotRaconteurException> )>& handler);

		/**
		 * @internal
		 * 
		 * @brief Sends a message synchronously to another node
		 * 
		 * The destination is determined by 
		 * the LocalEndpoint in the MessageHeader. This function will return
		 * once the transport has sent the message, or an error occurs. An exception
		 * is thrown if the message cannot be sent.
		 * 
		 * Requires Multithreading
		 * 
		 * @param m The message to send
		 * 
		 */
		void SendMessage(RR_INTRUSIVE_PTR<Message> m);

		/**
		 * @internal
		 * 
		 * @brief Called by transports when a message has been received
		 * 
		 * MessageReceived is called by transports to deliver messages to the node.
		 * The received messages are then dispatched to the correct client or service
		 * endpoints based on the LocalEndpoint stored in the MessageHeader.
		 * 
		 * @param m The received message
		 */
		void MessageReceived(RR_INTRUSIVE_PTR<Message> m);
	protected:

		/**
		 * @internal
		 * 
		 * @brief Called by transports to inform the node that a TransportConnection
		 * has been closed
		 * 
		 * @param endpoint The LocalEndpoint number that was closed
		 */
		void TransportConnectionClosed(uint32_t endpoint);

	private:
		uint32_t RequestTimeout;
		boost::mutex RequestTimeout_lock;

		uint32_t TransportInactivityTimeout;
		boost::mutex TransportInactivityTimeout_lock;

		uint32_t EndpointInactivityTimeout;
		boost::mutex EndpointInactivityTimeout_lock;
		
		uint32_t MemoryMaxTransferSize;
		boost::mutex MemoryMaxTransferSize_lock;
				
	public:

		/**
		 * @brief Get the timeout for requests in milliseconds
		 * 
		 * Requests are calls to a remote node that expect a response. `function`,
		 * `property`, `callback`, `memory`, and setup calls in `pipe` and `wire`
		 * are all requests. All other Robot Raconteur functions that call the remote
		 * node and expect a response are requests. Default timeout is 15 seconds.
		 * 
		 * @return uint32_t Timeout in milliseconds
		 */
		uint32_t GetRequestTimeout();

		/**
		 * @brief Set the timeout for requests in milliseconds
		 * 
		 * See GetRequestTimeout() for explanation of request timeouts. Default timeout
		 * is 15 seconds.
		 * 
		 * @param timeout Timeout in milliseconds
		 */
		void SetRequestTimeout(uint32_t timeout);

		/**
		 * @brief Get the timeout for transport activity in milliseconds
		 * 
		 * Sets a timeout for transport inactivity. If no message
		 * is sent or received on the transport for the specified time,
		 * the transport is closed. Default timeout is 10 minutes.
		 * 
		 * 
		 * @return uint32_t Timeout in milliseconds
		 */
		uint32_t GetTransportInactivityTimeout();

		/**
		 * @brief Set the timeout for transport activity in milliseconds
		 * 
		 * See GetTransportInactivityTimeout() for explanation of tranport
		 * inactivity timeouts. Default value is 10 minutes.
		 * 
		 * @param timeout The timeout in milliseconds
		 */
		void SetTransportInactivityTimeout(uint32_t timeout);

		/**
		 * @brief Get the timeout for endpoint activity in milliseconds
		 * 
		 * Sets a timeout for endpoint inactivity. If no message
		 * is sent or received by the endpoint for the specified time,
		 * the endpoint is closed. Default timeout is 10 minutes.
		 * 
		 * 
		 * @return uint32_t Timeout in milliseconds
		 */
		uint32_t GetEndpointInactivityTimeout();

		/**
		 * @brief Set the timeout for endpoint activity in milliseconds
		 * 
		 * See GetEndpointInactivityTimeout() for explanation of endpoint
		 * inactivity timeouts. Default value is 10 minutes.
		 * 
		 * @param timeout The timeout in milliseconds
		 */
		void SetEndpointInactivityTimeout(uint32_t timeout);

		/**
		 * @brief Get the maximum chunk size for memory transfers in bytes
		 * 
		 * `memory` members break up large transfers into chunks to avoid
		 * sending messages larger than the transport maximum, which is normally
		 * approximately 10 MB. The memory max transfer size is the largest
		 * data chunk the memory will send, in bytes. Default is 100 kB.
		 * 
		 * @return uint32_t The max memory transfer size in bytes
		 */
		uint32_t GetMemoryMaxTransferSize();

		/**
		 * @brief Set the maximum chunk size for memory transfers in bytes
		 * 
		 * See GetMemoryMaxTransferSize() for an explanation of memory
		 * transfer sizes. Be careful not to exceed the message size
		 * limitation of the transports in use.
		 * 
		 * @param size The max memory transfer size in bytes.
		 */
		void SetMemoryMaxTransferSize(uint32_t size);

	protected:
		/** @internal @brief endpoints storage*/
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Endpoint> > endpoints;
		/** @internal @brief recently used endpoint numbers, avoid collisions */
		std::map<uint32_t,boost::posix_time::ptime> recent_endpoints;
		
		/** @internal @brief transports storage*/
		RR_UNORDERED_MAP<uint32_t, RR_SHARED_PTR<Transport> > transports;
		/** @internal @brief transports storage mutex */
		boost::mutex transports_lock;
	
		/** @internal @brief dynamic_factory for wrappers*/
		RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> dynamic_factory;
		/** @internal @brief dynamic_factory mutex */
		boost::mutex dynamic_factory_lock;
	public:

		/**
		 * @brief Get the current DynamicServiceFactory. May be null
		 * 
		 * See SetDynamicServiceFactory()
		 * 
		 * @return const RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> 
		 */
		const RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> GetDynamicServiceFactory() ;

		/**
		 * @brief Set the DynamicServicefactory
		 * 
		 * DynamicServiceFactory is used by language wrappers to dynamically generate
		 * ServiceFactories based on ServiceDefinitions. DynamicServiceFactories allow
		 * Robot Raconteur to provide dynamic type handling at runtime.
		 * 
		 * @param f The DynamicServiceFactory instance
		 */
		void SetDynamicServiceFactory(RR_SHARED_PTR<RobotRaconteur::DynamicServiceFactory> f);

		/**
		 * @internal
		 * 
		 * @brief Generates an error return message for a message
		 * 
		 * This message will fill a message header with return information,
		 * and generates a response MessageEntry for each MessageEntry in the 
		 * original message filled with the specified error.
		 * 
		 * @param m The original message
		 * @param err The error code
		 * @param errname The name of the error 
		 * @param errdesc A human readible description
		 * @return RR_INTRUSIVE_PTR<Message> 
		 */
		RR_INTRUSIVE_PTR<Message> GenerateErrorReturnMessage(RR_INTRUSIVE_PTR<Message> m, MessageErrorType err, boost::string_ref errname, boost::string_ref errdesc);


		/**
		 * @brief Registers a service for clients to connect
		 * 
		 * The supplied object becomes the root object in the service. Other objects may
		 * be accessed by clients using `objref` members. The name of the service must conform
		 * to the naming rules of Robot Raconteur member names. A service is closed using
		 * either CloseService() or when Shutdown() is called.
		 * 
		 * Multiple services can be registered within the same node. Service names
		 * within a single node must be unique.
		 * 
		 * @param name The name of the service, must follow member naming rules
		 * @param servicetype The name of the service definition containing the object type. 
		 *     Do not include the object type.
		 * @param obj The root object of the service
		 * @param securitypolicy An optional security policy for the service to control authentication
		 *     and other security functions
		 * @return RR_SHARED_PTR<ServerContext> The instantiated ServerContext. This object is owned 
		 *    by the node and the return can be safely ignored.
		 */
		RR_SHARED_PTR<ServerContext> RegisterService(boost::string_ref name, boost::string_ref servicetype, RR_SHARED_PTR<RRObject> obj, RR_SHARED_PTR<ServiceSecurityPolicy> securitypolicy = RR_SHARED_PTR<ServiceSecurityPolicy>());


		/**
		 * @brief Registers a service using a previously instantiated ServerContext
		 * 
		 * The node will store a reference to the provided ServerContext and 
		 * manage its lifecycle
		 * 
		 * @param c The ServerContext to register
		 * @return RR_SHARED_PTR<ServerContext> 
		 */
		RR_SHARED_PTR<ServerContext> RegisterService(RR_SHARED_PTR<ServerContext> c);

		/**
		 * @brief Closes a previously registered service
		 * 
		 * Services are automatically closed by Shutdown, so this function
		 * is rarely used.
		 * 
		 * @param sname The name of the service to close
		 */
		void CloseService(boost::string_ref sname);

		/**
		 * @brief Get the names of registered services
		 * 
		 * @return std::vector<std::string> The registered service names
		 */
		std::vector<std::string> GetRegisteredServiceNames();

		/**
		 * @brief Gets a ServerContext for a service by name
		 * 
		 * @param name The name of the service
		 * @return RR_SHARED_PTR<ServerContext> The ServerContext for a registered
		 * service
		 */
		RR_SHARED_PTR<ServerContext> GetService(boost::string_ref name);

		public:
			
		/**
		 * @internal
		 * 
		 * @brief Called by a transport to execute a special request
		 * 
		 * Special requests are transport request MessageElements that occur before
		 * an endpoint is assigned. This function is only called by transports.
		 * 
		 * @param m The incoming message
		 * @param transportid The internal ID of the calling transport
		 * @param tc The transport connection that received the message
		 * @return RR_INTRUSIVE_PTR<Message> message to return to the sender
		 */
		RR_INTRUSIVE_PTR<Message> SpecialRequest(RR_INTRUSIVE_PTR<Message> m, uint32_t transportid, RR_SHARED_PTR<ITransportConnection> tc);

		/**
		 * @brief Create a client connection to a remote service using a URL
		 * 
		 * Synchronously creates a connection to a remote service using a URL. URLs are either provided by
		 * the service, or are determined using discovery functions such as FindServiceByType().
		 * This function is the primary way to create client connections.
		 * 
		 * username and credentials can be used to specify authentication information. Credentials will
		 * often contain a "password" or token entry.
		 * 
		 * The listener is a function that is called during various events. See ClientServiceListenerEventType
		 * for a description of the possible events.
		 * 
		 * ConnectService will attempt to instantiate a client object reference (proxy) based on the type
		 * information provided by the service. The type information will contain the type of the object,
		 * and all the implemented types. The client will normally want a specific one of the implement types.
		 * Specify this desired type in objecttype to avoid future compatibility issues.
		 * 
		 * Requires multithreading
		 * 
		 * @param url The URL of the service to connect
		 * @param username An optional username for authentication
		 * @param credentials Optional credentials for authentication
		 * @param listener An optional listener callback function
		 * @param objecttype The desired root object proxy type. Optional but highly recommended.
		 * @return RR_SHARED_PTR<RRObject> The client object reference (proxy). Must be cast to expected type. 
		 */
		RR_SHARED_PTR<RRObject> ConnectService(boost::string_ref url, boost::string_ref username = "", RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials=(RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >()), boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener = 0, boost::string_ref objecttype="");

		/**
		 * @brief Asynchronously create a client connection to a remote service using a URL
		 * 
		 * Same as ConnectService but returns asynchronously. See ConnectService() for more details
		 * on client connections.
		 * 
		 * handler is called after the client connection succeeds are fails. On success, the RRObject
		 * is returned and the exception is null. On failure, the RRObject is null and the exception
		 * contains a subclass of RobotRaconteurException.
		 * 
		 * @param url The URL of the service to connect
		 * @param username An optional username for authentication
		 * @param credentials Optional credentials for authentication
		 * @param listener An optional listener callback function
		 * @param objecttype The desired root object proxy type. Optional but highly recommended. 
		 * @param handler A handler function to receive the object reference or an exception
		 * @param timeout Timeout is milliseconds, or RR_TIMEOUT_INFINITE for no timeout.
		 */
		void AsyncConnectService(boost::string_ref url, boost::string_ref username, RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, boost::string_ref objecttype, boost::function<void(RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		/**
		 * @brief Same as ConnectService(), but accepts a vector of candidate URLs
		 * 
		 * Frequently during discovery, multiple candidate URLs will be discovered. This 
		 * function will attempt each of the candidate URLs kind-of-the-hill style to 
		 * find the best one.
		 * 
		 * Requires multithreading
		 * 
		 * @param urls Vector of candidate URLs to attempt
		 * @param username An optional username for authentication
		 * @param credentials Optional credentials for authentication
		 * @param listener An optional listener callback function
		 * @param objecttype The desired root object proxy type. Optional but highly recommended.
		 * @return RR_SHARED_PTR<RRObject> The client object reference (proxy). Must be cast to expected type.
		 */
		RR_SHARED_PTR<RRObject> ConnectService(const std::vector<std::string>& urls, boost::string_ref username = "", RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials=(RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >()), boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener = 0, boost::string_ref objecttype="");

		/**
		 * @brief Same as AsyncConnectService(), but accepts a vector of candidate URLs
		 * 
		 * Frequently during discovery, multiple candidate URLs will be discovered. This 
		 * function will attempt each of the candidate URLs kind-of-the-hill style to 
		 * find the best one.
		 * 
		 * @param url Vector of candidate URLs to attempt
		 * @param username An optional username for authentication
		 * @param credentials Optional credentials for authentication
		 * @param listener An optional listener callback function
		 * @param objecttype The desired root object proxy type. Optional but highly recommended. 
		 * @param handler A handler function to receive the object reference or an exception
		 * @param timeout Timeout is milliseconds, or RR_TIMEOUT_INFINITE for no timeout.
		 */
		void AsyncConnectService(const std::vector<std::string> &url, boost::string_ref username, RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials, boost::function<void (RR_SHARED_PTR<ClientContext>,ClientServiceListenerEventType,RR_SHARED_PTR<void>)> listener, boost::string_ref objecttype, boost::function<void(RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		/**
		 * @brief Disconnects a client connection to a service
		 * 
		 * Synchronously disconnects a client connection. Client connections
		 * are automatically closed by Shutdown(), so this function
		 * is optional. 
		 * 
		 * Requires multithreading
		 * 
		 * @param obj The root object of the service to disconnect
		 */
		void DisconnectService(RR_SHARED_PTR<RRObject> obj);

		/**
		 * @brief Asynchronously disconnects a client connection to a service
		 * 
		 * Same as DisconnectService() but returns asynchronously.
		 * 
		 * @param obj The root object of the client to disconnect
		 * @param handler The handler to call when complete
		 */
		void AsyncDisconnectService(RR_SHARED_PTR<RRObject> obj, boost::function<void()> handler);

		/**
		 * @brief Get the service attributes of a client connection
		 * 
		 * Returns the service attributes of a client connected using 
		 * ConnectService()
		 * 
		 * @param obj The root object of the client to use to retrieve service attributes
		 * @return std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > The service attributes
		 */
		std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > GetServiceAttributes(RR_SHARED_PTR<RRObject> obj);

		/**
		 * @brief Get the service NodeID of the remote node from a client connection
		 * 
		 * Returns the NodeID of the remote node that a client is connected
		 * 
		 * @param obj The root object of the client to use to retrieve service attributes
		 * @return NodeID The NodeID
		 */
		RobotRaconteur::NodeID GetServiceNodeID(RR_SHARED_PTR<RRObject> obj);

		/**
		 * @brief Get the service NodeName of the remote node from a client connection
		 * 
		 * Returns the NodeName of the remote node that a client is connected
		 * 
		 * @param obj The root object of the client to use to retrieve service attributes
		 * @return std::string The NodeName
		 */
		std::string GetServiceNodeName(RR_SHARED_PTR<RRObject> obj);

		
		/**
		 * @brief Get the name of a service from a client connection
		 * 
		 * Returns the service name of the remote service that a client is connected
		 * 
		 * @param obj The root object of the client to use to retrieve service attributes
		 * @return std::string The service name
		 */
		std::string GetServiceName(RR_SHARED_PTR<RRObject> obj);

		/**
		 * @brief Get the service path of a client object
		 * 
		 * @param obj The object to get the service path for
		 * @return std::string The object's service path
		 */
		std::string GetObjectServicePath(RR_SHARED_PTR<RRObject> obj);

		/**
		 * @internal
		 * 
		 * @brief Registers an endpoint for use with the node
		 * 
		 * The endpoint will be assigned a LocalEndpoint uint32_t identifier
		 * 
		 * @param e The endpoint to register
		 * @return uint32_t The LocalEndpoint identifier
		 */
		uint32_t RegisterEndpoint(RR_SHARED_PTR<Endpoint> e);

		/**
		 * @internal
		 * 
		 * @brief Deletes an endpoint
		 * 
		 * Deletes an endpoint. Any associated transport connections will be closed.
		 * 
		 * @param e The Endpoint to delete
		 */
		void DeleteEndpoint(RR_SHARED_PTR<Endpoint> e);

		/**
		 * 
		 * @brief Check that the TransportConnection associated with an endpoint
		 * is connected
		 * 
		 * @param endpoint The LocalEndpoint identifier to check
		 */
		void CheckConnection(uint32_t endpoint);



	private:

		
		uint32_t transport_count;
		

		boost::shared_mutex transport_lock;
		boost::mutex endpoint_lock;

		static RobotRaconteurNode m_s;
		static RR_SHARED_PTR<RobotRaconteurNode> m_sp;
		static RR_WEAK_PTR<RobotRaconteurNode> m_weak_sp;

		RobotRaconteur::NodeID m_NodeID;
		std::string m_NodeName;

		bool NodeID_set;
		bool NodeName_set;

		RR_UNORDERED_MAP<std::string,RR_SHARED_PTR<ServiceFactory> > service_factories;
		boost::shared_mutex service_factories_lock;
		boost::mutex id_lock;

		RR_UNORDERED_MAP<std::string,RR_SHARED_PTR<ServerContext> > services;
		boost::shared_mutex services_lock;

		RR_SHARED_PTR<detail::Discovery> m_Discovery;
		boost::signals2::signal<void(const NodeDiscoveryInfo&, const std::vector<ServiceInfo2>& )> discovery_updated_listeners;

		boost::signals2::signal<void(const NodeDiscoveryInfo&)> discovery_lost_listeners;

		RR_WEAK_PTR<RobotRaconteurNode> weak_this;
				
	public:

		/**
		 * @brief Get the nodes currently detected by Transports
		 * 
		 * Transports configured to listen for node discovery send detected node
		 * information to the parent node, where it is stored. Normally this information
		 * will expire after one minute, and needs to be constantly refreshed.
		 * 
		 * This node information is not verified. It is the raw discovery 
		 * information received by the transports. Verification is done
		 * when the node is interrogated for service information.
		 * 
		 * @return std::vector<NodeDiscoveryInfo> 
		 */
		std::vector<NodeDiscoveryInfo> GetDetectedNodes();

		/**
		 * @brief The boost::signals2 connection type for AddNodeServicesDetectedListener
		 * 
		 */
		typedef boost::signals2::connection node_updated_listener_connection;

		/**
		 * @brief Add a handler to be called when detected services are updated
		 * 
		 * The specified handler is called when the internal cache of available services
		 * is updated. The cache is only active when a subscription is in use,
		 * or is updated when FindServiceByType is called.
		 * 
		 * Subscriptions should be used instead of this function to detect and
		 * connect to services.
		 * 
		 * @param h The handler to call. Must have signature `void my_handler(const NodeDiscoveryInfo& node_info
		 * , const std::vector<ServiceInfo2>& service_info)`
		 * @return node_updated_listener_connection boost::signals2 connection object
		 */
		template<typename Handler>
		node_updated_listener_connection AddNodeServicesDetectedListener(BOOST_ASIO_MOVE_ARG(Handler) h)
		{
			boost::shared_lock<boost::shared_mutex> l(thread_pool_lock);
			if (is_shutdown)
			{
				throw InvalidOperationException("Node has been shut down");
			}

			return discovery_updated_listeners.connect(h);
		}

		/**
		 * @brief The boost::signals2 connection type for AddNodeDetectionLostListener
		 * 
		 */
		typedef boost::signals2::connection node_lost_listener_connection;

		/**
		 * @brief Add a handler to be called when a node is no longer detected
		 * 
		 * Nodes are considered no longer detected when the cached detected
		 * node information expires, usually after one minute. The cache must
		 * constantly be refreshed with new detection information to 
		 * prevent expiration.
		 * 
		 * @param h The handler to call. Must have signature 
		 *     `void my_handler(const NodeDiscoveryInfo&)`
		 * @return node_lost_listener_connection 
		 */
		template<typename Handler>
		node_lost_listener_connection AddNodeDetectionLostListener(BOOST_ASIO_MOVE_ARG(Handler) h)
		{
			boost::shared_lock<boost::shared_mutex> l(thread_pool_lock);
			if (is_shutdown)
			{
				throw InvalidOperationException("Node has been shut down");
			}
			return discovery_lost_listeners.connect(h);
		}

		/**
		 * @internal
		 * 
		 * @brief Provide a raw node announce discovery packet to the node
		 * 
		 * Called by Transports to inform the node that a node announce
		 * packet has been received. The node will parse and cache 
		 * the received node information.
		 * 
		 * @param packet The raw announce packet received by the Transport
		 */
		void NodeAnnouncePacketReceived(boost::string_ref packet);

		/**
		 * @internal
		 * 
		 * @brief Inform the node that a node has been detected
		 * 
		 * Used by Transports to inform the node of a detected node.
		 * info is a parsed node announce packet.
		 * 
		 * @param info Information about the detected node
		 */
		void NodeDetected(const NodeDiscoveryInfo& info);

		/**
		 * @brief Update the detected nodes cache
		 * 
		 * The node keeps a cache of detected nodes, but this may become stale
		 * if nodes are rapidly added and removed from the network. Call this
		 * function to update the detected nodes.
		 * 
		 * Requires multithreading
		 * 
		 * @param schemes A vector of transport schemes, ie "rr+tcp", "rr+local", etc. to update.
		 */
		void UpdateDetectedNodes(const std::vector<std::string>& schemes);

		/**
		 * @brief Asynchronously update the detected nodes cache
		 * 
		 * Same as UpdateDetectedNodes() but returns asynchronously
		 * 
		 * @param schemes A vector of transport schemes, ie "rr+tcp", "rr+local", etc. to update.
		 * @param handler The handler to call on completion
		 * @param timeout The timeout for the operation in milliseconds. This function will often run
		 * for the full timeout, so values less than 5 seconds are recommended.
		 */
		void AsyncUpdateDetectedNodes(const std::vector<std::string>& schemes, boost::function<void()> handler, int32_t timeout = 5000);

		/**
		 * @brief Get the maximum number of detected nodes that will be cached
		 * 
		 * The node keeps a cache of detected nodes. The NodeDiscoveryMaxCacheCount
		 * sets an upper limit to how many detected nodes are cached. By default set to 4096
		 * 
		 * @return uint32_t 
		 */
		uint32_t GetNodeDiscoveryMaxCacheCount();

		/**
		 * @brief Set maximum number of detected nodes that will be cached
		 * 
		 * See GetNodeDiscoveryMaxCacheCount() for more information
		 * 
		 * @param count The maximum number of detected nodes to cache
		 */
		void SetNodeDiscoveryMaxCacheCount(uint32_t count);

		/**
		 * @brief Subscribe to listen for available services information
		 * 
		 * A ServiceInfo2Subscription will track the availability of service types and
		 * inform when services become available or are lost. If connections to
		 * available services are also required, ServiceSubscription should be used.
		 * 
		 * @param service_types A std::vector of service types to listen for, ie `com.robotraconteur.robotics.robot.Robot`
		 * @param filter A filter to select individual services based on specified criteria
		 * @return RR_SHARED_PTR<ServiceInfo2Subscription> The active subscription
		 */
		RR_SHARED_PTR<ServiceInfo2Subscription> SubscribeServiceInfo2(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter = RR_SHARED_PTR<ServiceSubscriptionFilter>());

		/**
		 * @brief Subscribe to listen for available services and automatically connect
		 * 
		 * A ServiceSubscription will track the availability of service types and
		 * create connections when available.
		 * 
		 * @param service_types A std::vector of service types to listen for, ie `com.robotraconteur.robotics.robot.Robot`
		 * @param filter A filter to select individual services based on specified criteria
		 * @return RR_SHARED_PTR<ServiceSubscription> The active subscription
		 */
		RR_SHARED_PTR<ServiceSubscription> SubscribeServiceByType(const std::vector<std::string>& service_types, RR_SHARED_PTR<ServiceSubscriptionFilter> filter = RR_SHARED_PTR<ServiceSubscriptionFilter>());

		/**
		 * @brief Subscribe to a service using one or more URL. Used to create robust connections to services
		 * 
		 * Creates a ServiceSubscription assigned to a service with one or more candidate connection URLs. The
		 * subscription will attempt to maintain a peristent connection, reconnecting if the connection is lost.
		 * 
		 * @param url One or more candidate connection urls
		 * @param username An optional username for authentication
		 * @param credentials Optional credentials for authentication
		 * @param objecttype The desired root object proxy type. Optional but highly recommended.
		 * @return RR_SHARED_PTR<ServiceSubscription> The subscription object
		 */
		RR_SHARED_PTR<ServiceSubscription> SubscribeService(const std::vector<std::string>& url, boost::string_ref username = "", RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials=(RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >()),  boost::string_ref objecttype = "");

		/**
		 * @brief Subscribe to a service using a URL. Used to create robust connections to services
		 * 
		 * Creates a ServiceSubscription assigned to a service with a URL. The
		 * subscription will attempt to maintain a peristent connection, reconnecting if the connection is lost.
		 * 
		 * @param url The connection URL
		 * @param username An optional username for authentication
		 * @param credentials Optional credentials for authentication
		 * @param objecttype The desired root object proxy type. Optional but highly recommended.
		 * @return RR_SHARED_PTR<ServiceSubscription> The subscription object
		 */
		RR_SHARED_PTR<ServiceSubscription> SubscribeService(const std::string& url, boost::string_ref username = "", RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > credentials=(RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> >()),  boost::string_ref objecttype = "");


	protected:

		/**
		 * @internal
		 * 
		 * @brief Check the expiration time of cached detected node information
		 * and remove expired nodes
		 * 
		 */
		void CleanDiscoveredNodes();

		/**
		 * @internal
		 * 
		 * @brief Fire the NodeDetected event
		 * 
		 * @param node 
		 * @param services 
		 */
		void FireNodeDetected(RR_SHARED_PTR<NodeDiscoveryInfo> node, RR_SHARED_PTR<std::vector<ServiceInfo2> > services);

		/**
		 * @internal
		 * 
		 * @brief Fire the NodeLost event
		 * 
		 * @param node 
		 */
		void FireNodeLost(RR_SHARED_PTR<NodeDiscoveryInfo> node);

	public:

		/**
		 * @brief Select the "best" URL from a std::vector of candidates
		 * 
		 * Service discovery will often return a list of candidate URLs to 
		 * use to connect to a node. This function uses hueristics to select
		 * the "best" URL to use. The selection criteria ranks URLs in roughly
		 * the following order, lower number being better:
		 * 
		 * 1. "rr+intra" for IntraTransport
		 * 2. "rr+local" for LocalTransport
		 * 3. "rr+pci" or "rr+usb" for HardwareTransport
		 * 4. "rrs+tcp://127.0.0.1" for secure TcpTransport loopback
		 * 5. "rrs+tcp://[::1]" for secure TcpTransport IPv6 loopback
		 * 6. "rrs+tcp://localhost" for secure TcpTransport loopback
		 * 7. "rrs+tcp://[fe80" for secure TcpTransport link-local IPv6
		 * 8. "rrs+tcp://" for any secure TcpTransport
		 * 9. "rr+tcp://127.0.0.1" for TcpTransport loopback
		 * 10. "rr+tcp://[::1]" for TcpTransport IPv6 loopback
		 * 11. "rr+tcp://localhost" for TcpTransport loopback
		 * 12. "rr+tcp://[fe80" for TcpTransport link-local IPv6
		 * 13. "rr+tcp://" for any TcpTransport
		 *  
		 * TODO: "rr+quic" QuicTransport
		 *  
		 * @param urls The candidate URLs
		 * @return std::string The "best" URL out of the candidates
		 */
		static std::string SelectRemoteNodeURL(const std::vector<std::string>& urls);
		
		/**
		 * @brief Use discovery to find available services by service type
		 * 
		 * Uses discovery to find available services based on a service type. This
		 * service type is the type of the root object, ie 
		 * `com.robotraconteur.robotics.robot.Robot`. This process will update the detected 
		 * node cache.
		 * 
		 * Requires multithreading
		 * 
		 * @param servicetype The service type to find, ie `com.robotraconteur.robotics.robot.Robot`
		 * @param transportschemes A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`, etc
		 * @return std::vector<ServiceInfo2> The detected services
		 */
		std::vector<ServiceInfo2> FindServiceByType(boost::string_ref servicetype, const std::vector<std::string>& transportschemes);

		/**
		 * @brief Asynchronously use discovery to find availabe services by service type
		 * 
		 * Same as FindServiceByType() but returns asynchronously
		 * 
		 * @param servicetype The service type to find, ie `com.robotraconteur.robotics.robot.Robot`
		 * @param transportschemes A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`, etc
		 * @param handler Handler to call on completion
		 * @param timeout Timeout in milliseconds. Using a timeout greater than 5 seconds is not recommended.
		 */
		void AsyncFindServiceByType(boost::string_ref servicetype, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<ServiceInfo2> >) > handler, int32_t timeout=5000);
		
		/**
		 * @brief Finds nodes on the network with a specified NodeID
		 * 
		 * Updates the discovery cache and find nodes with the specified NodeID.
		 * This function returns unverified cache information.
		 * 
		 * Requires multithreading
		 * 
		 * @param id The NodeID to find
		 * @param transportschemes A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`, etc
		 * @return std::vector<NodeInfo2> 
		 */
		std::vector<NodeInfo2> FindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes);

		/**
		 * @brief Asynchronously finds nodes on the network with the specified NodeID
		 * 
		 * Same as FindNodeByID() but returns asynchronously
		 * 
		 * @param id The NodeID to find
		 * @param transportschemes A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`, etc
		 * @param handler Handler to call on completion
		 * @param timeout Timeout in milliseconds. Using a timeout greater than 5 seconds is not recommended.
		 */
		void AsyncFindNodeByID(const RobotRaconteur::NodeID& id, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > handler, int32_t timeout=5000);

		/**
		 * @brief Finds nodes on the network with a specified NodeName
		 * 
		 * Updates the discovery cache and find nodes with the specified NodeName.
		 * This function returns unverified cache information.
		 * 
		 * Requires multithreading
		 * 
		 * @param name The NodeName to find
		 * @param transportschemes A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`, etc
		 * @return std::vector<NodeInfo2> 
		 */
		std::vector<NodeInfo2> FindNodeByName(boost::string_ref name, const std::vector<std::string>& transportschemes);

		/**
		 * @brief Asynchronously finds nodes on the network with the specified NodeName
		 * 
		 * Same as FindNodeByName() but returns asynchronously
		 * 
		 * @param name The NodeName to find
		 * @param transportschemes A list of transport types to search, ie `rr+tcp`, `rr+local`, `rrs+tcp`, etc
		 * @param handler Handler to call on completion
		 * @param timeout Timeout in milliseconds. Using a timeout greater than 5 seconds is not recommended.
		 */
		void AsyncFindNodeByName(boost::string_ref name, const std::vector<std::string>& transportschemes, boost::function< void(RR_SHARED_PTR<std::vector<NodeInfo2> >) > handler, int32_t timeout=5000);

	public:

		/**
		 * @brief Request an exclusive access lock to a service object
		 * 
		 * Called by clients to request an exclusive lock on a service object and
		 * all subobjects (`objrefs`) in the service. The exclusive access lock will
		 * prevent other users ("User" lock) or client connections  ("Session" lock) 
		 * from interacting with the objects.
		 *  
		 * Requires multithreading
		 * 
		 * @param obj The object to lock. Must be returned by ConnectService or returned by an `objref`
		 * @param flags Select either a "User" or "Session" lock
		 * @return std::string "OK" on success
		 */
		std::string RequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags);

		/**
		 * @brief Asynchronously request an exclusive access lock to a service object
		 * 
		 * Same as RequestObjectLock() but returns asynchronously
		 * 
		 * @param obj The object to lock. Must be returned by ConnectService or returned by an `objref`
		 * @param flags Select either a "User" or "Session" lock
		 * @param handler Handler to call on completion
		 * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE
		 */
		void AsyncRequestObjectLock(RR_SHARED_PTR<RRObject> obj, RobotRaconteurObjectLockFlags flags, boost::function<void(RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		/**
		 * @brief Release an excluse access lock previously locked with RequestObjectLock()
		 * or AsyncRequestObjectLock()
		 * 
		 * Object must have previously been locked using RequestObjectLock() or
		 * AsyncRequestObjectLock()
		 * 
		 * Requires multithreading
		 * 
		 * @param obj The object previously locked
		 * @return std::string "OK" on success
		 */
		std::string ReleaseObjectLock(RR_SHARED_PTR<RRObject> obj);

		/**
		 * @brief Asynchronously release an excluse access lock previously locked 
		 * with RequestObjectLock() or AsyncRequestObjectLock()
		 * 
		 * Same as ReleaseObjectLock() but returns asynchronously
		 * 
		 * @param obj The object previously locked
		 * @param handler Handler to call on completion
		 * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE
		 */
		void AsyncReleaseObjectLock(RR_SHARED_PTR<RRObject> obj, boost::function<void(RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		/**
		 * @brief Creates a monitor lock on a specified object
		 * 
		 * Monitor locks are intendended for short operations that require
		 * guarding to prevent races, corruption, or other concurrency problems.
		 * Monitors emulate a single thread locking the service object.
		 * 
		 * Use of ScopedMonitorLock instead of this function is highly recommended
		 * to take advantage of RAII scoping
		 * 
		 * Monitor locks do not lock any sub-objects (objref)
		 * 
		 * Requires multithreading
		 * 
		 * @param obj The object to lock
		 * @param timeout The timeout in milliseconds to acquire the monitor lock, 
		 * or RR_TIMEOUT_INFINITE
		 */
		void MonitorEnter(RR_SHARED_PTR<RRObject> obj, int32_t timeout = RR_TIMEOUT_INFINITE);

		/**
		 * @brief Releases a monitor lock
		 * 
		 * Use of ScopedMonitorLock instead of this function is highly recommended
		 * to take advantage of RAII scoping
		 * 
		 * Requires multithreading
		 * 
		 * @param obj The object previously locked by MonitorEnter()
		 */
		void MonitorExit(RR_SHARED_PTR<RRObject> obj);

		/**
		 * @brief Wrapper for RobotRaconteurNode::MonitorEnter() and 
		 * RobotRaconteurNode::MonitorExit() to take advantage of RAII
		 * scoping 
		 */
		class ROBOTRACONTEUR_CORE_API ScopedMonitorLock
		{
			bool locked;
			RR_SHARED_PTR<RRObject> obj;
			RR_WEAK_PTR<RobotRaconteurNode> node;
			
		public:

			/**
			 * @brief Get the Node assosciated with the lock
			 * 
			 * @return RR_SHARED_PTR<RobotRaconteurNode> 
			 */
			RR_SHARED_PTR<RobotRaconteurNode> GetNode();
			
			/**
			 * @brief Create a monitor lock for the specified object
			 * 
			 * Creates a monitor lock by calling RobotRaconteur::MonitorEnter().
			 * Object will be locked once the object is created.
			 * 
			 * @param obj The object to monitor lock
			 * @param timeout The timeout in milliseconds to acquire the monitor lock, 
			 * or RR_TIMEOUT_INFINITE
			 */
			ScopedMonitorLock(RR_SHARED_PTR<RRObject> obj, int32_t timeout=-1);

			/**
			 * @brief Relock the object after calling unlock()
			 * 
			 * @param timeout The timeout in milliseconds to acquire the monitor lock, 
			 * or RR_TIMEOUT_INFINITE
			 */
			void lock(int32_t timeout=-1);

			/**
			 * @brief Releases the monitor lock
			 * 
			 * The ScopedMonitorLock destructor will release
			 * the lock automatically, so in most cases it is
			 * not necessary to call this function
			 */
			void unlock();

			/**
			 * @brief Release the monitor lock from the class
			 * 
			 * The monitor lock is released from the ScopedMonitorLock
			 * instance. The monitor lock will not be released
			 * by the ScopedMonitorLock destructor. 
			 */
			void release();

			~ScopedMonitorLock();
			

		};

	private:
		
		

		
		RR_SHARED_PTR<Timer> PeriodicCleanupTask_timer;
		boost::shared_mutex PeriodicCleanupTask_timer_lock;

		static void StartPeriodicCleanupTask(RR_SHARED_PTR<RobotRaconteurNode> node);

		void PeriodicCleanupTask(const TimerEvent& err);
		bool PeriodicCleanupTask_timerstarted;

		std::list<RR_SHARED_PTR<IPeriodicCleanupTask> > cleanupobjs;

		boost::mutex cleanupobjs_lock;

		boost::mutex cleanup_thread_lock;

	public:

		/**
		 * @brief Add a periodic cleanup task
		 * 
		 * The node will call periodic cleanup tasks every 5-15 seconds.
		 * Use these tasks instead of timers for cleanup operations.
		 * 
		 * @param task The task to call periodically
		 */
		void AddPeriodicCleanupTask(RR_SHARED_PTR<IPeriodicCleanupTask> task);

		/**
		 * @brief Remove a task previously registered with AddPeriodicCleanupTask()
		 * 
		 * @param task The task to remove
		 */
		void RemovePeriodicCleanupTask(RR_SHARED_PTR<IPeriodicCleanupTask> task);

		

	public:
		
		/**
		 * @brief Returns an objref as a specific type
		 * 
		 * Robot Raconteur service object types are polymorphic using inheritence, 
		 * meaning that an object may be represented using multiple object types.
		 * `objref` will attempt to return the relevant type, but it is sometimes
		 * necessary to request a specific type for an objref.
		 * 
		 * This function will return the object from an `objref` as the specified type,
		 * or throw an error if the type is invalid.
		 * 
		 * Requires multithreading
		 * 
		 * @param obj The object with the desired `objref`
		 * @param objref The name of the `objref` member
		 * @param objecttype The desired service object type
		 * @return RR_SHARED_PTR<RRObject> The object with the specified interface type. Must be cast to the desired type
		 */

		RR_SHARED_PTR<RRObject> FindObjRefTyped(RR_SHARED_PTR<RRObject> obj, boost::string_ref objref, boost::string_ref objecttype);

		/**
		 * @brief Returns an indexed objref as a specified type
		 * 
		 * Same as FindObjectTyped() but includes an `objref` index
		 * 
		 * Requires multithreading
		 * 
		 * @param obj The object with the desired `objref`
		 * @param objref The name of the `objref` member
		 * @param index The index for the `objref`, convert int to string for int32 index type
		 * @param objecttype The desired service object type
		 * @return RR_SHARED_PTR<RRObject> The object with the specified interface type. Must be cast to the desired type
		 */
		RR_SHARED_PTR<RRObject> FindObjRefTyped(RR_SHARED_PTR<RRObject> obj, boost::string_ref objref, boost::string_ref index, boost::string_ref objecttype);

		/**
		 * @brief Asynchronously returns an objref as a specific type
		 * 
		 * Same as FindObjectType() but returns asynchronously
		 * 
		 * @param obj The object with the desired `objref`
		 * @param objref The name of the `objref` member
		 * @param objecttype The desired service object type
		 * @param handler A handler function to receive the object reference or an exception
		 * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
		 */
		void AsyncFindObjRefTyped(RR_SHARED_PTR<RRObject> obj, boost::string_ref objref, boost::string_ref objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		/**
		 * @brief Asynchronously returns an objref as a specific type
		 * 
		 * Same as FindObjectType() but returns asynchronously
		 * 
		 * @param obj The object with the desired `objref`
		 * @param objref The name of the `objref` member
		 * @param index The index for the `objref`, convert int to string for int32 index type
		 * @param objecttype The desired service object type
		 * @param handler A handler function to receive the object reference or an exception
		 * @param timeout Timeout in milliseconds, or RR_TIMEOUT_INFINITE for no timeout
		 */
		void AsyncFindObjRefTyped(RR_SHARED_PTR<RRObject> obj, boost::string_ref objref, boost::string_ref index, boost::string_ref objecttype, boost::function<void (RR_SHARED_PTR<RRObject>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		/**
		 * @brief Returns the type of a service object
		 * 
		 * Returns the fully qualified object type that would be returned by an `objref` member
		 * 
		 * Requires multithreading
		 * 
		 * @param obj The object with the desired `objref`
		 * @param objref The name of the `objref` member
		 * @return std::string The fully qaulified object type
		 */
		std::string FindObjectType(RR_SHARED_PTR<RRObject> obj, boost::string_ref objref);

		/**
		 * @brief Returns the type of a service object
		 * 
		 * Returns the fully qualified object type that would be returned by an indexed `objref` member
		 * 
		 * Requires multithreading
		 * 
		 * @param obj The object with the desired `objref`
		 * @param objref The name of the `objref` member
		 * @param index The index for the `objref`, convert int to string for int32 index type
		 * @return std::string The fully qaulified object type
		 */
		std::string FindObjectType(RR_SHARED_PTR<RRObject> obj, boost::string_ref objref, boost::string_ref index);
		
		/**
		 * @brief Asynchronously returns the type of a service object
		 * 
		 * Same as FindObjectType() but returns asynchronously
		 * 
		 * @param obj The object with the desired `objref`
		 * @param objref The name of the `objref` member
		 * @param handler A handler function to receive the object type or an exception
		 * @param timeout Timeout is milliseconds, or RR_TIMEOUT_INFINITE for no timeout
		 */
		void AsyncFindObjectType(RR_SHARED_PTR<RRObject> obj, boost::string_ref objref, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);

		/**
		 * @brief Asynchronously returns the type of a service object
		 * 
		 * Same as FindObjectType() but returns asynchronously
		 * 
		 * @param obj The object with the desired `objref`
		 * @param objref The name of the `objref` member
		 * @param index The index for the `objref`, convert int to string for int32 index type
		 * @param handler A handler function to receive the object type or an exception
		 * @param timeout Timeout is milliseconds, or RR_TIMEOUT_INFINITE for no timeout
		 */
		void AsyncFindObjectType(RR_SHARED_PTR<RRObject> obj, boost::string_ref objref, boost::string_ref index, boost::function<void (RR_SHARED_PTR<std::string>,RR_SHARED_PTR<RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE);


	private:
		RR_SHARED_PTR<ThreadPool> thread_pool;
		boost::shared_mutex thread_pool_lock;
		RR_SHARED_PTR<ThreadPoolFactory> thread_pool_factory;
		boost::mutex thread_pool_factory_lock;

	public:

		/**
		 * @brief Get the current ThreadPool for the node
		 * 
		 * Returns the current ThreadPool in use by the node. If no ThreadPool has been
		 * configured, a ThreadPool is created using the ThreadPoolFactory. By default
		 * a ThreadPool is created with 20 threads
		 * 
		 * @return RR_SHARED_PTR<ThreadPool> The current thread pool
		 */
		RR_SHARED_PTR<ThreadPool> GetThreadPool();

		/**
		 * @brief Tries to get the ThreadPool, returns false if the ThreadPool
		 * is not available
		 * 
		 * This function will normally only return false if the node has been
		 * shut down
		 * 
		 * 
		 * @param pool Out parameter to receive ThreadPool shared_ptr
		 * @return true pool contains shared_ptr to the ThreadPool
		 * @return false Threadpool is not available
		 */
		bool TryGetThreadPool(RR_SHARED_PTR<ThreadPool>& pool);

		/**
		 * @brief Sets the ThreadPool for the node
		 * 
		 * This function must be called before a ThreadPool has been 
		 * set, either through this fuction, or a ThreadPool is
		 * created by GetThreadPool()
		 * 
		 * @param pool The ThreadPool for the node
		 */
		void SetThreadPool(RR_SHARED_PTR<ThreadPool> pool);

		/**
		 * @brief Get the current ThreadPoolFactory for the node
		 * 
		 * ThreadPoolFactory is used to create a ThreadPool for
		 * the node if one is not already set. By default uses
		 * ThreadPoolFactory
		 * 
		 * 
		 * @return RR_SHARED_PTR<ThreadPoolFactory> 
		 */
		RR_SHARED_PTR<ThreadPoolFactory> GetThreadPoolFactory();

		/**
		 * @brief Set the ThreadPoolFactory for the node
		 * 
		 * ThreadPoolFactory is used to create a ThreadPool for
		 * the node if one is not already set
		 * 
		 * @param factory The ThreadPoolFactory for the node
		 */
		void SetThreadPoolFactory(RR_SHARED_PTR<ThreadPoolFactory> factory);

		/**
		 * @brief Get the number of threads for the node ThreadPool 
		 * 
		 * @return int32_t 
		 */
		int32_t GetThreadPoolCount();

		/**
		 * @brief Set the number of threads for the node ThreadPool
		 * 
		 * The ThreadPool will use a maximum of count threads. If
		 * this number is lower than the current ThreadPool count,
		 * the ThreadPool will attempt to release threads beyond count
		 * as they return to idle
		 * 
		 * @param count The number of threads for the ThreadPool
		 */
		void SetThreadPoolCount(int32_t count);

		/**
		 * @brief Tries to post a handler function to be called by a thread in the 
		 * thread pool. Returns immediately
		 * 
		 * This function will not wait for the handler function to complete.
		 * 
		 * If node is null or ThreadPool is not available, the handler is not
		 * called and false is returned 
		 * 
		 * @tparam HandlerType 
		 * @param node The node
		 * @param h The handler to post
		 * @param shutdown_op True if handler should be called during node shutdown
		 * @return true The function was post to the ThreadPool
		 * @return false The handler could not be post to the ThreadPool
		 */
		template<typename HandlerType>
		static bool TryPostToThreadPool(RR_WEAK_PTR<RobotRaconteurNode> node, BOOST_ASIO_MOVE_ARG(HandlerType) h, bool shutdown_op=false)
		{			
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			{
				boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
				if (!shutdown_op && node1->is_shutdown) return false;
			}
			RR_SHARED_PTR<ThreadPool> t;
			if (!node1->TryGetThreadPool(t)) return false;
			return t->TryPost(RR_MOVE(h));
		}

		/**
		 * @brief Shuts down and releases the node thred pool
		 * Do not call ReleaseThreadPool unless you really know what 
		 * you are doing. In most cases it will be destroyed automatically.
		 */
		void ReleaseThreadPool();

	protected:

		/**
		 * @internal
		 * 
		 * @brief Initializes a new thread pool using ThreadPoolFactory
		 * and sets the thread_count
		 * 
		 * @param thread_count The thread_count for the new ThreadPool
		 * @return true ThreadPool was initialized
		 * @return false ThreadPool was not initialized, most likely
		 * the ThreadPool has already been set
		 */
		bool InitThreadPool(int32_t thread_count);

	public:

		/**
		 * @brief Get the names of service types pulled by a client
		 * 
		 * Clients pull service definitions from services and create
		 * instances of ServiceFactory if a DynamicServiceFactory has 
		 * been configured. GetPulledServiceTypes returns a list of  the
		 * names of these pulled service types. Use GetPulledServiceType()
		 * to retrieve the ServiceFactory for a specific type.
		 * 
		 * @param obj Client object referenece returned by ConnectService() or 
		 * AsyncConnectService()
		 * @return std::vector<std::string> The names of the pulled service types
		 */
		std::vector<std::string> GetPulledServiceTypes(RR_SHARED_PTR<RRObject> obj);

		/**
		 * @brief Get a ServiceFactory created from a service type pulled by a client
		 * 
		 * Clients pull service definitions from services and create
		 * instances of ServiceFactory if a DynamicServiceFactory has 
		 * been configured. GetPulledServiceType() returns a
		 * generated ServiceFactory. Use GetPulledServiceTypes() to 
		 * return a list of available service types.
		 * 		 
		 * @param obj Client object referenece returned by ConnectService() or 
		 * AsyncConnectService()
		 * @param type The name of the service type
		 * @return RR_SHARED_PTR<ServiceFactory> The pulled service factory
		 */
		RR_SHARED_PTR<ServiceFactory> GetPulledServiceType(RR_SHARED_PTR<RRObject> obj, boost::string_ref type);

		/**
		 * @brief Set an exception handler function
		 * 
		 * The ThreadPool will catch exceptions that are uncaught
		 * by other logic and pass the exception to the specified
		 * exception handler. User handler functions that throw exceptions
		 * will also be caught and passed to the specified handler
		 * function
		 * 
		 * @param handler The handler function for uncaught exceptions
		 */
		void SetExceptionHandler(boost::function<void (const std::exception*)> handler);

		/**
		 * @brief Get the node's current exception handler, or null
		 * 
		 * Returns the exception handler previously set using
		 * SetExceptionHandler()
		 * 
		 * @return boost::function<void (const std::exception*)> The current exception handler, or null
		 */
		boost::function<void (const std::exception*)> GetExceptionHandler();

		/**
		 * @brief Handle exceptino by passing the current exception
		 * to the handler function
		 * 
		 * @param exp The exception to pass to the handler function
		 */
		void HandleException(const std::exception* exp);

		/**
		 * @brief Try to pass an exception to the exception handler
		 * 
		 * Attempts to pass an exception to the handler function. Will fail
		 * and return false if node is null or the exception handler
		 * has not been set
		 * 
		 * @param node The node to handle the exception
		 * @param exp The exception to pass to the handler function
		 * @return true The handler function was called
		 * @return false The handler function was not called
		 */
		static bool TryHandleException(RR_WEAK_PTR<RobotRaconteurNode> node, const std::exception* exp);

	protected:

		/** @internal @brief exception_handler storage */
		boost::function<void (const std::exception*) > exception_handler;
		/** @internal @brief exception_handler mutex */
		boost::mutex exception_handler_lock;

		/** @internal @brief random_generator mutex*/
		boost::mutex random_generator_lock;
		/** @internal @brief random_generator using hardware random number generator (if available)*/
		RR_SHARED_PTR<boost::random::random_device> random_generator;

	public:

		/**
		 * @brief The current time in UTC time zone
		 * 
		 * Uses the internal node clock to get the current time in UTC.
		 * While this will normally use the system clock, this may
		 * use simulation time in certain circumstances
		 * 
		 * @return boost::posix_time::ptime The current time in UTC
		 */
		virtual boost::posix_time::ptime NowUTC();

		/**
		 * @brief The current node time as a TimeSpec
		 * 
		 * The current node time as a TimeSpec. See NowNodeTime()
		 * 
		 * @return TimeSpec The current node time as a TimeSpec
		 */
		virtual TimeSpec NowTimeSpec();


		/**
		 * @brief The current node time
		 * 
		 * UTC time is not monotonic, due to the introduction of leap-seconds, and the possibility
		 * of the system clock being updated by the user. For a real-time systems, 
		 * this is unaccetpable and can lead to system instability. The "node time" used by Robot Raconteur
		 * is synchronized to UTC at startup, and is then steadily increasing from that initial time.
		 * It will ignore changes to the system clock, and will also ignore corrections like leap
		 * seconds.
		 * 
		 * @return boost::posix_time::ptime The current node time
		 */
		virtual boost::posix_time::ptime NowNodeTime();

		/**
		 * @brief The sync time of the node
		 * 
		 * The node synchronizes it's clock with the system time in UTC
		 * when the node is initialized. After this time, a steady
		 * clock is used. This prevents the clock from jumping
		 * forward and back in time. It will no longer be updated
		 * by changes in the system time.
		 * 
		 * If an external high precision clock source like PTP is available, 
		 * that clock will be used in place of the system and steady clock.
		 * 
		 * @return boost::posix_time::ptime The node sync time in UTC
		 */
		virtual boost::posix_time::ptime NodeSyncTimeUTC();

		/**
		 * @brief The sync time of the node as a TimeSpec
		 * 
		 * See NodeSyncTimeUTC()
		 * 
		 * @return TimeSpec The node sync time as a TimeSpec
		 */
		virtual TimeSpec NodeSyncTimeSpec();

	protected:

		/** @internal @brief The tranport providing simulation time, or null if using system time */
		RR_WEAK_PTR<ITransportTimeProvider> time_provider;
		/** @internal  @brief time_provider mutex */
		boost::shared_mutex time_provider_lock;

		/** @internal @brief The sync time for the node clock **/
		boost::posix_time::ptime node_sync_time;
		TimeSpec node_sync_timespec;
		boost::chrono::steady_clock::time_point node_internal_start_time;
		
	public:

		/**
		 * @brief Create a Timer object
		 * 
		 * This function will normally return a WallTimer instance
		 * 
		 * Start() must be called after timer creation
		 * 
		 * @param period The period of the timer
		 * @param handler The handler function to call when timer times out
		 * @param oneshot True if timer is a one-shot timer, false for repeated timer
		 * @return RR_SHARED_PTR<Timer> The new Timer object. Must call Start()
		 */
		virtual RR_SHARED_PTR<Timer> CreateTimer(const boost::posix_time::time_duration& period, boost::function<void (const TimerEvent&)> handler, bool oneshot=false);
		
		/**
		 * @brief Create a Rate object
		 * 
		 * Rate is used to stabilize periodic loops to a specified frequency
		 * 
		 * This function will normally return a WallRate instance
		 * 
		 * @param frequency Frequency of loop in Hz
		 * @return RR_SHARED_PTR<Rate> The new Rate object
		 */
		virtual RR_SHARED_PTR<Rate> CreateRate(double frequency);

		/**
		 * @brief Sleeps for a specified duration
		 * 
		 * Normally will sleep based on the system clock, but in certain
		 * circumstances will use simulation time
		 * 
		 * Requires multithreading
		 * 
		 * @param duration The duration to sleep
		 */
		virtual void Sleep(const boost::posix_time::time_duration& duration);

		/**
		 * @brief Create an AutoResetEvent object
		 * 
		 * Normally the AutoResetEvent will use the system clock for timeouts,
		 * but in certain circumstances will use simulation time
		 * 
		 * @return RR_SHARED_PTR<AutoResetEvent> The new AutoResetEvent object
		 */
		virtual RR_SHARED_PTR<AutoResetEvent> CreateAutoResetEvent();

		/**
		 * @brief Downcasts a RobotRaconteurException and throws it
		 * 
		 * Serialized RobotRaconteurException may not be correctly downcast when
		 * deserialized. DownCastAndThrowException will find the correct type,
		 * downcast the exception, and throw the correctly typed exception
		 * 
		 * @param exp The RobotRaconteurException to downcast and throw
		 */
		void DownCastAndThrowException(RobotRaconteurException& exp);

		/**
		 * @brief Downcasts a RobotRaconteurException
		 * 
		 * Serialized RobotRaconteurException may not be correctly downcast when
		 * deserialized. DownCastException will find the correct type,
		 * downcast the exception, and return the correctly typed exception
		 * 
		 * @param exp The RobotRaconteurException to downcast
		 */
		RR_SHARED_PTR<RobotRaconteurException> DownCastException(RR_SHARED_PTR<RobotRaconteurException> exp);

		/**
		 * @brief Get the ServicePath of a client object reference
		 * obj must be returned by ConnectService(), AsyncConnectService(),
		 * or an `objref`
		 * @param obj The object to query
		 * @return std::string The servicepath of the object
		 */
		virtual std::string GetServicePath(RR_SHARED_PTR<RRObject> obj);

		/**
		 * @brief Check if the endpoint is authorized for large message transfer
		 * 
		 * Saturating a transport with large messages to cause memory exhaustion
		 * is a potential attack vector for unauthorized clients. Limit transfers
		 * by unauthorized clients to small messages to prevent this
		 * 
		 * @param endpoint The LocalEndpoint id
		 * @return true Large transfer authorized
		 * @return false Large transfer is not authorized
		 */
		bool IsEndpointLargeTransferAuthorized(uint32_t endpoint);

		/**
		 * @brief Get the current RobotRaconteurVersion as a string
		 * 
		 * Version is three numbers separated by dots, ie "0.9.2"
		 * 
		 * @return std::string The current version
		 */
		std::string GetRobotRaconteurVersion();

		/**
		 * @brief Returns a random integer using the node's random number
		 * generator
		 * 
		 * @tparam T The integer type, ie int32_t
		 * @param min The minimum value for the returned integer
		 * @param max The maximum value for the returned integer
		 * @return T The generated random integer
		 */
		template<typename T>
		T GetRandomInt(T min, T max)
		{
			boost::mutex::scoped_lock lock(random_generator_lock);
			boost::random::uniform_int_distribution<T> d(min, max);
			return d(*random_generator);
		}

		/**
		 * @brief Returns a vector of random integer using the node's 
		 * random number generator
		 * 
		 * @tparam T The integer type, ie int32_t
		 * @param count The number of random integers to generate
		 * @param min The minimum value for the returned integers
		 * @param max The maximum value for the returned integers
		 * @return std::vector<T> The generated random integer vector
		 */
		template<typename T>
		std::vector<T> GetRandomInts(size_t count, T min, T max)
		{
			std::vector<T> o;
			o.resize(count);
			boost::mutex::scoped_lock lock(random_generator_lock);
			for (size_t i = 0; i < count; i++)
			{
				boost::random::uniform_int_distribution<T> d(min, max);
				o[i]=d(*random_generator);
			}
			return o;
		}

		/**
		 * @brief Generates a random string with the specified character count
		 * using the node's random number generator
		 * 
		 * The returned string will consist of letters and numbers
		 * 
		 * @param count The number of characters to return
		 * @return std::string The random string
		 */
		std::string GetRandomString(size_t count);

	protected:

		/** @internal @brief current service_state_nonce storage*/
		std::string service_state_nonce;
		/** @internal @brief service_state_nonce mutex*/
		boost::mutex service_state_nonce_lock;

	public:

		/**
		 * @internal
		 * 
		 * @brief Get the current ServiceStateNonce
		 * 
		 * The ServiceStateNonce is a short 16 character randomly
		 * generated string that identifies the current service configuration
		 * of the node. This nonce is sent with node discovery packets. A change
		 * in the ServiceStateNonce informs listeners that the node state
		 * has changed, and the listener's cache should be refreshed by interrogating the
		 * node
		 * 
		 * @return std::string The 16 character nonce
		 */
		std::string GetServiceStateNonce();
		
		/**
		 * @internal
		 * 
		 * @brief Update the ServiceStateNonce due to change in the node's state
		 * 
		 * Call to update the ServiceStateNonce if services have changed
		 */
		void UpdateServiceStateNonce();

	protected:

		RR_SHARED_PTR<LogRecordHandler> log_handler;
		boost::shared_mutex log_handler_mutex;
		RobotRaconteur_LogLevel log_level;
		boost::shared_mutex log_level_mutex;

	public:

		/**
		 * @brief Test if the specified log level would be accepted
		 * 
		 * @param log_level Log level to test
		 * @return true The log would be accepted
		 * @return false The log would be ignored
		 */
		bool CompareLogLevel(RobotRaconteur_LogLevel log_level);

		/**
		 * @brief Log a simple message using the current node
		 * 
		 * The record will be sent to the configured log handler,
		 * or sent to std::cerr if none is configured
		 * 
		 * If the level of the message is below the current log level
		 * for the node, the record will be ignored
		 * 
		 * @param level The level for the log message
		 * @param message The log message
		 */
		void LogMessage(RobotRaconteur_LogLevel level, const std::string& message);

		/**
		 * @brief Log a record to the node. Use the macros specified in Logging.h
		 * instead of this function directly.
		 * 
		 * The record will be sent to the configured log handler,
		 * or sent to std::cerr if none is configured
		 * 
		 * If the level of the message is below the current log level
		 * for the node, it will be ignored
		 * 
		 * @param record The record to log
		 */
		void LogRecord(const RRLogRecord& record);

		/**
		 * @brief Get the current log level for the node
		 * 
		 * Default level is "info"
		 * 
		 * @return RobotRaconteur_LogLevel 
		 */
		RobotRaconteur_LogLevel GetLogLevel();
		
		/**
		 * @brief Set the log level for the node
		 * 
		 * Set RobotRaconteur_LogLevel_Disable to disable logging
		 * 
		 * @param level The desired log level
		 */
		void SetLogLevel(RobotRaconteur_LogLevel level);

		/**
		 * @brief Set the log level for the node from a string
		 * 
		 * Must be one of the following values: DISABLE, FATAL, ERROR, WARNING, INFO, DEBUG, TRACE
		 * 
		 * Defaults to WARNING
		 * 
		 * @param level The desired log level
		 * @return RobotRaconteur_LogLevel The log level
		 */
		RobotRaconteur_LogLevel SetLogLevelFromString(boost::string_ref level);

		/**
		 * @brief Set the log level for the node from specified environmental variable
		 * 
		 * Retrieves the specified environmental variable and sets the log level based
		 * on one of the following values: DISABLE, FATAL, ERROR, WARNING, INFO, DEBUG, TRACE
		 * 
		 * If an invalid value or the variable does not exist, the log level is left unchanged. 
		 * 
		 * @param env_variable_name The environmental variable to use. Defaults to 
		 * `ROBOTRACONTEUR_LOG_LEVEL`
		 * @return RobotRaconteur_LogLevel The log level
		 */
		RobotRaconteur_LogLevel SetLogLevelFromEnvVariable(const std::string& env_variable_name = "ROBOTRACONTEUR_LOG_LEVEL");

		/**
		 * @brief Get the currently configured log record handler
		 * 
		 * If NULL, records are sent to std::cerr
		 * 
		 * @return RR_SHARED_PTR<LogRecordHandler>
		 */
		RR_SHARED_PTR<LogRecordHandler> GetLogRecordHandler();

		/**
		 * @brief Set the handler for log records
		 * 
		 * If handler is NULL, records are sent to std::cerr
		 * 
		 * @param handler The log record handler function
		 */
		void SetLogRecordHandler(RR_SHARED_PTR<LogRecordHandler> handler);

	protected:
		boost::shared_mutex tap_lock;
		RR_SHARED_PTR<MessageTap> tap;

	public:

		/**
		 * @brief Get the active message tap
		 * 
		 * @return RR_SHARED_PTR<MessageTap> 
		 */
		RR_SHARED_PTR<MessageTap> GetMessageTap();

		/**
		 * @brief Set the a message tap to record log records and messages
		 * 
		 * @param message_tap The message tap to use
		 */
		void SetMessageTap(RR_SHARED_PTR<MessageTap> message_tap);
		
	protected:
		/**
		 * @internal		 
		 */
		template <typename F>
		class asio_async_wait1
		{
		public:
			asio_async_wait1(F f) : f_(BOOST_ASIO_MOVE_CAST(F)(f))
			{
			}

			void do_complete(const boost::system::error_code& err)
			{
				f_(err);
			}

		protected:

			F f_;			
		};


	public:
		/**
		 * @internal
		 * 
		 * @brief Race free async_wait
		 * 
		 * Wrapper for async_wait on boost::asio objects that prevents
		 * races with node shutdown
		 * 
		 * @tparam T 
		 * @tparam F 
		 * @param node 
		 * @param t 
		 * @param f 
		 * @return true 
		 * @return false 
		 */
		template<typename T, typename F>
		static bool asio_async_wait(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted));
			}

			RR_SHARED_PTR<asio_async_wait1<F> > f1(new asio_async_wait1<F>(f));
			t->async_wait(boost::bind(&asio_async_wait1<F>::do_complete, f1, boost::asio::placeholders::error));
			
			node1->shutdown_listeners.connect(
				boost::signals2::signal<void()>::slot_type(
					boost::bind(&T::cancel, t.get())
				).track(t).track(f1));
			return true;
		}

	public:

		/**
		 * @internal
		 * 
		 * @brief Race free wrapper for async_read_some
		 * Wrapper for async_read_some on boost::asio objects that prevents
		 * races with node shutdown
		 * 
		 * @tparam T 
		 * @tparam B 
		 * @tparam F 
		 * @param node 
		 * @param t 
		 * @param b
		 * @param f
		 * @return true 
		 * @return false 
		 */
		template<typename T, typename B, typename F>
		static bool asio_async_read_some(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, B& b, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted, 0));
			}

			t->async_read_some(b,f);			
			return true;
		}

		/**
		 * @internal
		 * 
		 * @brief Race free wrapper for async_write_some
		 * 
		 * Wrapper for async_write_some on boost::asio objects that prevents
		 * races with node shutdown
		 * 
		 * @tparam T 
		 * @tparam B 
		 * @tparam F 
		 * @param node 
		 * @param t 
		 * @param b
		 * @param f
		 * @return true 
		 * @return false 
		 */
		template<typename T, typename B, typename F>
		static bool asio_async_write_some(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, B& b, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (!node1->thread_pool) return false;
			t->async_write_some(b, f);
			return true;
		}

		/**
		 * @internal
		 * 
		 * @brief Race free wrapper for async_connect
		 * 
		 * Wrapper for async_connect on boost::asio objects that prevents
		 * races with node shutdown		 
		 * 
		 * @tparam T 
		 * @tparam B 
		 * @tparam F 
		 * @param node 
		 * @param t 
		 * @param b
		 * @param f
		 * @return true 
		 * @return false 
		 */
		template<typename T, typename B, typename F>
		static bool asio_async_connect(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, const B& b, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (!node1->thread_pool) return false;
			t->async_connect(b, f);
			return true;
		}

		/**
		 * @internal
		 * 
		 * @brief Race free wrapper for async_connect
		 * 
		 * Wrapper for async_connect on boost::asio objects that prevents
		 * races with node shutdown	
		 * 
		 * @tparam T 
		 * @tparam B 
		 * @tparam C 
		 * @tparam F 
		 * @param node 
		 * @param t 
		 * @param b 
		 * @param c
		 * @param f
		 * @return true 
		 * @return false 
		 */
		template<typename T, typename B, typename C, typename F>
		static bool asio_async_connect(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, const B& b, const C& c, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (!node1->thread_pool) return false;
			t->async_connect(b, c, f);
			return true;
		}

#if BOOST_ASIO_VERSION < 101200
		/**
		 * @internal
		 * 
		 * @brief Race free wrapper for async_resolve
		 * 
		 * Wrapper for async_resolve on boost::asio objects that prevents
		 * races with node shutdown	
		 * 
		 * @tparam T 
		 * @tparam B 
		 * @tparam F 
		 * @param node 
		 * @param t 
		 * @param b 
		 * @param f
		 * @return true 
		 * @return false 
		 */
		template<typename T, typename B, typename F>
		static bool asio_async_resolve(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, const B& b, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;
				typename T::iterator iter;
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted, iter));
			}

			t->async_resolve(b, f);
			return true;
		}
#else
		/**
		 * @internal
		 * 
		 * @brief Race free wrapper for async_resolve
		 * 
		 * Wrapper for async_resolve on boost::asio objects that prevents
		 * races with node shutdown	
		 * 
		 * @tparam T 
		 * @tparam B 
		 * @tparam C 
		 * @tparam F 
		 * @param node 
		 * @param t 
		 * @param b 
		 * @param c 
		 * @param f
		 * @return true 
		 * @return false 
		 */
		template<typename T, typename B, typename C, typename F>
		static bool asio_async_resolve(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, const B& b, const C& c, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;
				typename T::results_type results;
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted, results));
			}

			t->async_resolve(b, c, f);
			return true;
		}
#endif

		/**
		 * @internal
		 * 
		 * @brief Race free wrapper for async_handshake
		 * 
		 * Wrapper for async_hanshake on boost::asio objects that prevents
		 * races with node shutdown	
		 * 
		 * @tparam T 
		 * @tparam F 
		 * @param node 
		 * @param t
		 * @param f
		 * @return true 
		 * @return false 
		 */
		template<typename T, typename F>
		static bool asio_async_handshake(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;				
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted));
			}

			t->async_handshake(f);
			return true;
		}

		/**
		 * @internal
		 * 
		 *  @brief Race free wrapper for async_handshake
		 * 
		 * Wrapper for async_handshake on boost::asio objects that prevents
		 * races with node shutdown	
		 * 
		 * @tparam T 
		 * @tparam U 
		 * @tparam F 
		 * @param node 
		 * @param t 
		 * @param u
		 * @param f
		 * @return true 
		 * @return false 
		 */
		template<typename T, typename U, typename F>
		static bool asio_async_handshake(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, const U& u, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;				
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted));
			}

			t->async_handshake(u, f);
			return true;
		}

		/**
		 * @internal
		 * 
		 * @brief Race free wrapper for async_shutdown
		 * 
		 * Wrapper for async_shutdown on boost::asio objects that prevents
		 * races with node shutdown	 
		 * 
		 * @tparam T 
		 * @tparam F 
		 * @param node 
		 * @param t
		 * @param f
		 * @return true 
		 * @return false 
		 */
		template<typename T, typename F>
		static bool asio_async_shutdown(RR_WEAK_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<T>& t, BOOST_ASIO_MOVE_ARG(F) f)
		{
			RR_SHARED_PTR<RobotRaconteurNode> node1 = node.lock();
			if (!node1) return false;
			boost::shared_lock<boost::shared_mutex> l(node1->thread_pool_lock);
			if (node1->is_shutdown)
			{
				l.unlock();
				RR_SHARED_PTR<ThreadPool> t;
				if (!node1->TryGetThreadPool(t)) return false;				
				return t->TryPost(boost::bind(f, boost::asio::error::operation_aborted));
			}

			t->async_shutdown(f);
			return true;
		}

	};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
	/** @brief Convenience alias for RobotRaconteurNode shared_ptr */
	using RobotRaconteurNodePtr = RR_SHARED_PTR<RobotRaconteurNode>;
#endif

}
