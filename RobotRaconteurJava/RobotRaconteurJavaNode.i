%rr_intrusive_ptr(RobotRaconteur::MessageElement)

%shared_ptr(RobotRaconteur::RobotRaconteurNode);

%rename("%(firstlowercase)s", %$isfunction) "";

%typemap("javaimports") RobotRaconteur::RobotRaconteurNode
%{
import java.util.*;
%}

%typemap("javacode") RobotRaconteur::RobotRaconteurNode
%{












//Typemap start

	private static RobotRaconteurNode csharp_s=null;
	
	public static RobotRaconteurNode s()
	{
		if (csharp_s == null)
		{
			csharp_s = _get_s();
			 
		}

		return csharp_s;
	}

	public final NodeID getNodeID()
	{
		return _NodeID();
	}
	public final void setNodeID(NodeID value)
	{
		_SetNodeID(value);
	}

	public final String getNodeName()
	{
		return _NodeName();
	}
	public final void setNodeName(String value)
	{
		_SetNodeName(value);
	}

	private java.util.HashMap<String, ServiceFactory> servicetypes = new java.util.HashMap<String,ServiceFactory>();

	public final void registerServiceType(ServiceFactory servicetype)
	{
		synchronized (servicetypes)
		{
			servicetypes.put(servicetype.getServiceName(), servicetype);
			_RegisterServiceType(servicetype.serviceDef());
		}
	}

	public final ServiceFactory getServiceType(String servicename)
	{
		synchronized (servicetypes)
		{
			/*try
			{
				if (!servicetypes.containsKey(servicename))
				{
					String factoryname=servicename + "." + servicename.replace(".","__") + "Factory";
					Class factoryclass=Class.forName(factoryname);
					java.lang.reflect.Constructor constructor=factoryclass.getConstructor(new Class[] {});
					ServiceFactory factory=(ServiceFactory)constructor.newInstance();
					if (factory.getServiceName()==servicename)
					{
						servicetypes.put(servicename,factory);
					}
					
				}
				
			}
			catch (Exception e)
			{
				
			}	*/		
			
			try
			{
				ServiceFactory f= servicetypes.get(servicename);
				if (f==null) throw new ServiceException("Could not find service type " + servicename);
				return f;
			}
			catch (Exception e)
			{
				throw new ServiceException("Could not find service type " + servicename);
			}
		}
	}

	public final ServiceFactory getServiceFactory(String servicename)
	{
		return getServiceType(servicename);
	}




	


	public Object connectService(String url, String username, java.util.HashMap<String, Object> credentials)
	{
		return connectService(url, username, credentials, null, null);
	}

	public Object connectService(String url, String username)
	{
		return connectService(url, username, null, null, null);
	}

	public Object connectService(String url)
	{
		return connectService(url, null, null, null, null);
	}

	public final Object connectService(String url, String username, Map<String, Object> credentials, Action3<ServiceStub, ClientServiceListenerEventType, Object> listener)
	{
		return connectService(url, username, credentials, listener, null);
	}
	
	public final Object connectService(String url, String username, Map<String, Object> credentials, Action3<ServiceStub, ClientServiceListenerEventType, Object> listener, String objecttype)
	{
		MessageElementData credentials2 = null;
		try
		{
		if (username == null)
		{
			username = "";
		}
		if (credentials != null)
		{
			credentials2 = (MessageElementData)packVarType(credentials);
		}

		ClientServiceListenerDirectorJava listener2 = null;
		if (listener != null)
		{
			
			
			listener2 = new ClientServiceListenerDirectorJava(listener);
			listener2.setObjectheapid(RRObjectHeap.addObject(listener2));
		}

		if (objecttype==null) objecttype="";
		
		WrappedServiceStub s = _ConnectService(url, username, credentials2, listener2, objecttype);
		ServiceStub s2 = getServiceType(s.getRR_objecttype().getServiceDefinition().getName()).createStub(s);
		if (listener2 != null)
		{
			listener2.stub = s2;

			
			
		}
		return s2;
		}
		finally
		{
			if (credentials2!=null) credentials2.delete();
		}

	}


	public Object connectService(String[] url, String username, java.util.HashMap<String, Object> credentials)
	{
		return connectService(url, username, credentials, null, null);
	}

	public Object connectService(String[] url, String username)
	{
		return connectService(url, username, null, null, null);
	}

	public Object connectService(String[] url)
	{
		return connectService(url, null, null, null, null);
	}
	
	public final Object connectService(String[] url, String username, java.util.HashMap<String, Object> credentials, Action3<ServiceStub, ClientServiceListenerEventType, Object> listener)
	{
		return connectService(url, username, credentials, listener, null);
	}
	
	public final Object connectService(String[] url, String username, java.util.HashMap<String, Object> credentials, Action3<ServiceStub, ClientServiceListenerEventType, Object> listener, String objecttype)
	{
		MessageElementData credentials2 = null;
		try
		{
		if (username == null)
		{
			username = "";
		}
		if (credentials != null)
		{
			credentials2 = (MessageElementData)packVarType(credentials);
		}

		ClientServiceListenerDirectorJava listener2 = null;
		if (listener != null)
		{
			
			listener2 = new ClientServiceListenerDirectorJava(listener);
			listener2.setObjectheapid(RRObjectHeap.addObject(listener2));
		}

		vectorstring url2 = new vectorstring();
		for (String ss : url)
		{
			url2.add(ss);
		}

		if (objecttype==null) objecttype="";
		
		WrappedServiceStub s = _ConnectService(url2, username, credentials2, listener2, objecttype);
		ServiceStub s2 = getServiceType(s.getRR_objecttype().getServiceDefinition().getName()).createStub(s);
		if (listener2 != null)
		{
			listener2.stub = s2;
		
			
			
		}
		return s2;
		}
		finally
		{
			if (credentials2!=null) credentials2.delete();
		}

	}

	public final void disconnectService(Object obj)
	{
		ServiceStub stub = (ServiceStub)obj;
		_DisconnectService(stub.rr_innerstub);
	}

	public final java.util.HashMap<String, Object> getServiceAttributes(Object obj)
	{
		ServiceStub stub = (ServiceStub)obj;
		MessageElement me=null;
		try
		{
		me=_GetServiceAttributes(stub.rr_innerstub);
		return (java.util.HashMap<String,Object>)unpackVarType(me);
		}
		finally
		{
		if (me!=null) me.delete();
		}
	}

	public final MessageElementData packStructure(Object s)
	{
		if (s == null)
		{
			return null;
		}

		Class c;

		if (s instanceof PodMultiDimArray)
		{
			PodMultiDimArray s2=(PodMultiDimArray)s;
			c=s2.pod_array.getClass();
		}
		else if (s instanceof NamedMultiDimArray)
		{
			NamedMultiDimArray s2=(NamedMultiDimArray)s;
			c=s2.namedarray_array.getClass();
		}
		else
		{
			c=s.getClass();
		}
		if (c.isArray())
		{
			c=c.getComponentType();
		}
		
		String servicename = RobotRaconteurNode.splitQualifiedName(getTypeString(c))[0];

		ServiceFactory f = getServiceType(servicename);


		return f.packStructure(s);
	}

	public final <T> T unpackStructure(MessageElementData l)
	{
		if (l == null)
		{
			return null;
		}

		String servicename = RobotRaconteurNode.splitQualifiedName(l.getTypeString())[0];

		ServiceFactory f = getServiceType(servicename);

		return f.<T>unpackStructure(l);
	}

	public final <T> T unpackStructureDispose(MessageElementData l)
	{
		try
		{
			return this.<T>unpackStructure(l);
		}
		finally
		{
			if (l!=null) l.delete();
		}
		
	}

	private final <T> MessageElement packContainerValue(String name, Object data, Class<?> T_class)
	{
		if (data == null)
		{
			return null;
		}

		if (T_class == Object.class)
		{
			return MessageElementUtil.newMessageElementDispose(name, packVarType(data));
		}

		if (T_class == MultiDimArray.class)
		{
			return MessageElementUtil.newMessageElementDispose(name, packMultiDimArray((MultiDimArray)data));
		}

		if (T_class == PodMultiDimArray.class)
		{
			return MessageElementUtil.newMessageElementDispose(name, packStructure(data));
		}
		
		if (T_class == NamedMultiDimArray.class)
		{
			return MessageElementUtil.newMessageElementDispose(name, packStructure(data));
		}
		
		boolean is_array = T_class.isArray();
		if (is_array)
		{
			if (T_class.getComponentType().isPrimitive())
			{
				return MessageElementUtil.newMessageElementDispose(name, data);
			}

			if (RRPod.class.isAssignableFrom(T_class.getComponentType()))
			{
				return MessageElementUtil.newMessageElementDispose(name, packStructure(data));
			}

			return MessageElementUtil.newMessageElementDispose(name, packVarType(data));
		}

		if (RRStructure.class.isAssignableFrom(T_class) || RRPod.class.isAssignableFrom(T_class) || RRNamedArray.class.isAssignableFrom(T_class))
		{
			return MessageElementUtil.newMessageElementDispose(name, packStructure(data));
		}

		return MessageElementUtil.newMessageElementDispose(name, packVarType(data));
	}

	private final <T> MessageElement packContainerValue(int number, Object data, Class<?> T_class)
	{
		MessageElement m=null;
		try
		{
			m=packContainerValue("",data,T_class);
			m.setElementNumber(number);
			m.setElementFlags((short)( m.getElementFlags() & ~RobotRaconteurJava.getMessageElementFlags_ELEMENT_NAME_STR()));
			m.setElementFlags((short)( m.getElementFlags() | RobotRaconteurJava.getMessageElementFlags_ELEMENT_NUMBER()));
						
			return m;
			
		}
		catch(RuntimeException e)
		{
			if (m!=null) m.delete();
			throw e;		
		}
	}

	private final <T> T unpackContainerValue(MessageElement e)
	{
		switch (e.getElementType())
		{
			case DataTypes_void_t:
               return null;
            case DataTypes_double_t:
            case DataTypes_single_t:
            case DataTypes_int8_t:
            case DataTypes_uint8_t:
            case DataTypes_int16_t:
            case DataTypes_uint16_t:
            case DataTypes_int32_t:
            case DataTypes_uint32_t:
            case DataTypes_int64_t:
            case DataTypes_uint64_t:
            case DataTypes_cdouble_t:
            case DataTypes_csingle_t:
            case DataTypes_bool_t:            			
                return (T)e.getData();                
            case DataTypes_string_t:
                return (T)e.getData();
			case DataTypes_multidimarray_t:                
				return (T)unpackMultiDimArrayDispose((MessageElementMultiDimArray)e.getData());
			case DataTypes_pod_t:
			{
				vectorptr_messageelement m = new vectorptr_messageelement();
				try
				{			
					m.add(e);
					return (T)unpackStructureDispose(new MessageElementPodArray(e.getElementTypeName(),m));
				}
				finally
				{
					m.delete();
				}
			}
			case DataTypes_structure_t:
			case DataTypes_pod_array_t:
			case DataTypes_pod_multidimarray_t:
			case DataTypes_namedarray_array_t:
			case DataTypes_namedarray_multidimarray_t:
				return (T)unpackStructureDispose((MessageElementData)e.getData());
			default:
                    throw new DataTypeException("Invalid container data type");
		}
	
	}

	public final <Tkey, Tvalue> Object packMapType(Object data, Class<?> Tkey_class, Class<?> Tvalue_class)
	{
		if (data == null)
		{
			return null;
		}
		
		if (Tkey_class == Integer.class)
		{
			vectorptr_messageelement m = new vectorptr_messageelement();
			try
			{
			java.util.HashMap<Tkey, Tvalue> ddata = (java.util.HashMap<Tkey, Tvalue>)data;
						
			for (java.util.Map.Entry<Tkey, Tvalue> d : ddata.entrySet())
			{
				MessageElementUtil.addMessageElementDispose(m,packContainerValue((Integer)d.getKey(), d.getValue(), Tvalue_class));
			}	

			return new MessageElementMap_int32_t(m);
			}
			finally
			{
				m.delete();
			}

		}

		if (Tkey_class == String.class)
		{
			vectorptr_messageelement m = new vectorptr_messageelement();
			try
			{
			java.util.HashMap<Tkey, Tvalue> ddata = (java.util.HashMap<Tkey, Tvalue>)data;
						
			for (java.util.Map.Entry<Tkey, Tvalue> d : ddata.entrySet())
			{
				MessageElementUtil.addMessageElementDispose(m,packContainerValue((String)d.getKey(), d.getValue(), Tvalue_class));					
			}
			
			return new MessageElementMap_string(m);
			}
			finally
			{
				m.delete();
			}
		}

		throw new RuntimeException(new DataTypeException("Indexed types can only be indexed by int32 and string"));

	}


	public final <Tkey, Tvalue> Object unpackMapType(Object data)
	{
		if (data == null)
		{
			return null;
		}

		if (data instanceof MessageElementMap_int32_t)
		{
			java.util.HashMap<Integer, Tvalue> o = new java.util.HashMap<Integer, Tvalue>();

			MessageElementMap_int32_t cdata = (MessageElementMap_int32_t)data;
			vectorptr_messageelement m=cdata.getElements();
			try
			{
			for (int i=0; i<m.size(); i++)
			{
				MessageElement e=m.get(i);
				try
				{									
					o.put(MessageElementUtil.getMessageElementNumber(e), (Tvalue)unpackContainerValue(e));				
				}
				finally
				{
					if (e!=null) e.delete();
				}
			}
			return o;
			}
			finally
			{
				if (m!=null) m.delete();
			}
		}
		else if (data instanceof MessageElementMap_string)
		{
			java.util.HashMap<String, Tvalue> o = new java.util.HashMap<String, Tvalue>();

			MessageElementMap_string cdata = (MessageElementMap_string)data;
			vectorptr_messageelement m=cdata.getElements();
			try
			{
			for (int i=0; i<m.size(); i++)
			{
				MessageElement e=m.get(i);
				try
				{					
					o.put(e.getElementName(), (Tvalue)unpackContainerValue(e));
				}
				finally
				{
					if (e!=null) e.delete();
				}
			}
			return o;
			}
			finally
			{
				if (m!=null) m.delete();
			}
		}
		else
		{
			throw new RuntimeException(new DataTypeException("Indexed types can only be indexed by int32 and string"));
		}

	}
	
	//List types
	
	public final <Tkey, Tvalue> Object unpackMapTypeDispose(Object data)
	{
		try
		{
			return this.<Tkey,Tvalue>unpackMapType(data);
		}
		finally
		{
			if (data !=null)
			{
			if (data instanceof MessageElementData)
			{
				((MessageElementData)data).delete();
			}
			}
		}
	}
	
	public final <Tvalue> Object packListType(Object data,Class<?> Tvalue_class)
	{
		if (data == null)
		{
			return null;
		}
		DataTypes stype;
				
		vectorptr_messageelement m = new vectorptr_messageelement();
		try
		{
		java.util.List< Tvalue> ddata = (java.util.List<Tvalue>)data;

		int count=0;
		for (Tvalue d : ddata)
		{
			MessageElementUtil.addMessageElementDispose(m,packContainerValue(count, d, Tvalue_class));
					
			count++;
		}						

		return new MessageElementList(m);
		}
		finally
		{
			m.delete();
		}
		
	}


	public final <Tvalue> Object unpackListType(Object data)
	{
		if (data == null)
		{
			return null;
		}

		
			java.util.List<Tvalue> o = new java.util.ArrayList<Tvalue>();

			MessageElementList cdata = (MessageElementList)data;
			
			vectorptr_messageelement m=cdata.getElements();
			try
			{
			for (int i=0; i<m.size(); i++)
			{
				MessageElement e=m.get(i);
				if (MessageElementUtil.getMessageElementNumber(e) != i)
					throw new DataTypeException("Error in list format");
				try
				{
					o.add((Tvalue)unpackContainerValue(e));				
				}
				finally
				{
					e.delete();
				}
			}
			return o;
			}
			finally
			{
				if (m!=null) m.delete();
			}
	}	
	
	public final <Tvalue> Object unpackListTypeDispose(Object data)
	{
		try
		{
			return this.<Tvalue>unpackListType(data);
		}
		finally
		{
			if (data!=null)
			{
			if (data instanceof MessageElementData)
			{
				((MessageElementData)data).delete();
			}
			}
		}
	
	
	}
	

	/** 
	 Packs a varvalue data.  This can handle any type supported by the node
	 
	 @param data The data to be packed
	 @return The packed data for use with MessageElement.Data
	*/
	public final Object packVarType(Object data)
	{
		if (data == null)
		{
			return null;
		}
		
		if (data instanceof java.util.Map<?, ?>)
		{
			Map<?,?> datamap=(Map<?,?>)(data);
			if (datamap.size()==0)
			{
				return packMapType(data, Integer.class, Object.class);
			}
			else
			{
			Class<?> Tkey=datamap.keySet().iterator().next().getClass();
			if (Tkey==Integer.class)
			{
				return packMapType(data, Integer.class, Object.class);
			}
			else if (Tkey==String.class)
			{
				return packMapType(data, String.class, Object.class);
			}
			else
			{
				throw new RuntimeException(new DataTypeException("Could not pack varvalue"));
			}
			}
		}
		
		if (data instanceof java.util.List<?>)
		{
			return packListType(data, Object.class);
		}
				
		if (data instanceof MultiDimArray)
		{
			return packMultiDimArray((MultiDimArray)data);
		}

		if (data instanceof PodMultiDimArray)
		{
			return packStructure(data);
		}

		if (data instanceof NamedMultiDimArray)
		{
			return packStructure(data);
		}
		
		if (data instanceof RRStructure || data instanceof RRPod || data instanceof RRPod[])
		{
			return packStructure(data);
		}
		
		if (data instanceof RRNamedArray || data instanceof RRNamedArray[])
		{
			return packStructure(data);
		}
		
		if (data instanceof String)
		{
			return data;
		}

		if (data instanceof Number || data instanceof byte[] || data instanceof short[] || data instanceof int[] || data instanceof long[]
			|| data instanceof double[] || data instanceof float[]
		    || data instanceof UnsignedByte || data instanceof UnsignedBytes || data instanceof UnsignedInt || data instanceof UnsignedInts
			|| data instanceof UnsignedLong || data instanceof UnsignedLongs || data instanceof CDouble || data instanceof CDouble[]
			|| data instanceof CSingle || data instanceof CSingle[] || data instanceof Boolean || data instanceof boolean[]
			 )
		{
			return data;	
		}		
		
		throw new DataTypeException("Could not pack varvalue");
	}

	/** 
	 Unpacks a varvalue from a MessageElement.  This can unpack any type supported by the node 
	 
	 @param me The message element containing the data
	 @return The unpacked data
	*/
	public final Object unpackVarType(MessageElement me)
	{
		if (me == null)
		{
			return null;
		}

		switch (me.getElementType())
		{
			case DataTypes_void_t:
               return null;
            case DataTypes_double_t:
            case DataTypes_single_t:
            case DataTypes_int8_t:
            case DataTypes_uint8_t:
            case DataTypes_int16_t:
            case DataTypes_uint16_t:
            case DataTypes_int32_t:
            case DataTypes_uint32_t:
            case DataTypes_int64_t:
            case DataTypes_uint64_t:
            case DataTypes_cdouble_t:
            case DataTypes_csingle_t:
            case DataTypes_bool_t:                           
                return me.getData();                
            case DataTypes_string_t:
                return me.getData();
			case DataTypes_multidimarray_t:                
				return unpackMultiDimArrayDispose((MessageElementMultiDimArray)me.getData());            
			case DataTypes_pod_t:
			{
				vectorptr_messageelement m = new vectorptr_messageelement();
				try
				{			
					m.add(me);
					return (Object)unpackStructureDispose(new MessageElementPodArray(me.getElementTypeName(),m));
				}
				finally
				{
					m.delete();
				}
			}
			case DataTypes_structure_t:
			case DataTypes_pod_array_t:
			case DataTypes_pod_multidimarray_t:
			case DataTypes_namedarray_array_t:
			case DataTypes_namedarray_multidimarray_t:
				return unpackStructureDispose((MessageElementData)me.getData());
			case DataTypes_vector_t:
				return this.<Integer, Object>unpackMapType(me.getData());
			case DataTypes_dictionary_t:
				return this.<String, Object>unpackMapType(me.getData());
			case DataTypes_list_t:
				return this.<Object>unpackListType(me.getData());
			default:
                throw new DataTypeException("Invalid varvalue data type");
			
		}
	}
	
	public final Object unpackVarTypeDispose(MessageElement me)
	{
		try
		{
			return unpackVarType(me);
		}
		finally
		{
			if (me!=null)
			{
				me.delete();
			
			}
		}
	
	}
	
	
	
	/** 
	 Packs a MultiDimArray into a MessageElementMultiDimArray
	 
	 @param array The array to be packed
	 @return A packed array for use with MessageElement.Data
	*/
	public final MessageElementMultiDimArray packMultiDimArray(MultiDimArray array)
	{
		if (array == null)
		{
			return null;
		}
		vectorptr_messageelement l = new vectorptr_messageelement();
		try
		{
		MessageElementUtil.addMessageElementDispose(l,"dims", new UnsignedInts(array.dims));		
		MessageElementUtil.addMessageElementDispose(l,"array", array.array);
		
		return new MessageElementMultiDimArray(l);
		}
		finally
		{
		l.delete();
		}


	}

	/** 
	 Unpacks a MessageElementMultiDimArray and returns unpacked multidim array
	 
	 @param marray The MessageElementMultiDimArray to unpack
	 @return The unpacked multidim array
	*/
	public final MultiDimArray unpackMultiDimArray(MessageElementMultiDimArray marray)
	{
		if (marray == null)
		{
			return null;
		}
		
		vectorptr_messageelement marrayElements=marray.getElements();
		try
		{
		MultiDimArray m = new MultiDimArray();		
		m.dims = (MessageElementUtil.<UnsignedInts>findElementAndCast(marrayElements, "dims")).value;
		m.array = (MessageElementUtil.<Object>findElementAndCast(marrayElements, "array"));
		return m;
		}
		finally
		{
			if (marrayElements!=null) marrayElements.delete();
		}


	}

	public final MultiDimArray unpackMultiDimArrayDispose(MessageElementMultiDimArray marray)
	{
		try
		{
			return unpackMultiDimArray(marray);
		}
		finally
		{
			if (marray!=null) marray.delete();
		}
	
	}
	
	public final String requestObjectLock(Object obj, RobotRaconteurObjectLockFlags flags)
	{
		if (!(obj instanceof ServiceStub))
		{
			throw new UnsupportedOperationException("Can only lock object opened through Robot Raconteur");
		}
		ServiceStub s = (ServiceStub)obj;

		return _RequestObjectLock(s.rr_innerstub, flags);


	}



	public final String releaseObjectLock(Object obj)
	{
		if (!(obj instanceof ServiceStub))
		{
			throw new UnsupportedOperationException("Can only unlock object opened through Robot Raconteur");
		}
		ServiceStub s = (ServiceStub)obj;

		return _ReleaseObjectLock(s.rr_innerstub);


	}

	public final void asyncRequestObjectLock(Object obj, RobotRaconteurObjectLockFlags flags, Action2<String,RuntimeException> handler)
	{
		asyncRequestObjectLock(obj,flags,handler,-1);
	}
	
	public final void asyncRequestObjectLock(Object obj, RobotRaconteurObjectLockFlags flags, Action2<String,RuntimeException> handler, int timeout)
	{
		if (!(obj instanceof ServiceStub))
		{
			throw new UnsupportedOperationException("Can only lock object opened through Robot Raconteur");
		}
		ServiceStub s = (ServiceStub)obj;

		AsyncStringReturnDirectorImpl h=new AsyncStringReturnDirectorImpl(handler);
		int id=RRObjectHeap.addObject(h);
		_AsyncRequestObjectLock(s.rr_innerstub, flags,timeout,h,id);
	}

	public final void asyncReleaseObjectLock(Object obj, Action2<String,RuntimeException> handler)
	{
		asyncReleaseObjectLock(obj,handler,-1);
	}
	
	public final void asyncReleaseObjectLock(Object obj, Action2<String,RuntimeException> handler, int timeout)
	{
		if (!(obj instanceof ServiceStub))
		{
			throw new UnsupportedOperationException("Can only lock object opened through Robot Raconteur");
		}
		ServiceStub s = (ServiceStub)obj;

		AsyncStringReturnDirectorImpl h=new AsyncStringReturnDirectorImpl(handler);
		int id=RRObjectHeap.addObject(h);
		_AsyncReleaseObjectLock(s.rr_innerstub, timeout,h,id);
	}
	
	public void monitorEnter(Object obj)
	{
		monitorEnter(obj, -1);
	}

	public final void monitorEnter(Object obj, int timeout)
	{
		if (!(obj instanceof ServiceStub))
		{
			throw new UnsupportedOperationException("Only service stubs can be monitored by RobotRaconteurNode");
		}
		ServiceStub s = (ServiceStub)obj;

		_MonitorEnter(s.rr_innerstub, timeout);
	}

	public final void monitorExit(Object obj)
	{
		if (!(obj instanceof ServiceStub))
		{
			throw new UnsupportedOperationException("Only service stubs can be monitored by RobotRaconteurNode");
		}
		ServiceStub s = (ServiceStub)obj;

		_MonitorExit(s.rr_innerstub);
	}

	public final ServiceInfo2[] findServiceByType(String servicetype, String[] transportschemes)
	{
		vectorstring s = new vectorstring();
		for (String s2 : transportschemes)
		{
			s.add(s2);
		}
		vectorserviceinfo2wrapped i = RobotRaconteurJava.wrappedFindServiceByType(this, servicetype, s);

		java.util.ArrayList<ServiceInfo2> o = new java.util.ArrayList<ServiceInfo2>();
		for (int j=0; j<i.size(); j++)
		{
			ServiceInfo2Wrapped i2=i.get(j);
			ServiceInfo2 i3 = new ServiceInfo2(i2);			
			o.add(i3);
		}

		return o.toArray(new ServiceInfo2[0]);
	}

	public final NodeInfo2[] findNodeByID(NodeID id, String[] transportschemes)
	{
		vectorstring s = new vectorstring();
		for (String s2 : transportschemes)
		{
			s.add(s2);
		}
		vectornodeinfo2 i = RobotRaconteurJava.wrappedFindNodeByID(this, id, s);

		java.util.ArrayList<NodeInfo2> o = new java.util.ArrayList<NodeInfo2>();
		for (int j=0; j<i.size(); j++)
		{
			WrappedNodeInfo2 i2=i.get(j);
			NodeInfo2 i3 = new NodeInfo2(i2);			
			o.add(i3);
		}

		return o.toArray(new NodeInfo2[0]);
	}
	
	public final NodeInfo2[] findNodeByName(String name, String[] transportschemes)
	{
		vectorstring s = new vectorstring();
		for (String s2 : transportschemes)
		{
			s.add(s2);
		}
		vectornodeinfo2 i = RobotRaconteurJava.wrappedFindNodeByName(this, name, s);

		java.util.ArrayList<NodeInfo2> o = new java.util.ArrayList<NodeInfo2>();
		for (int j=0; j<i.size(); j++)
		{
			WrappedNodeInfo2 i2=i.get(j);
			NodeInfo2 i3 = new NodeInfo2(i2);			
			o.add(i3);
		}

		return o.toArray(new NodeInfo2[0]);
	}


	public ServerContext registerService(String name, String servicetype, Object obj)
	{
		return registerService(name, servicetype, obj, null);
	}

	public final ServerContext registerService(String name, String servicetype, Object obj, ServiceSecurityPolicy policy)
	{
		ServiceSkel skel = getServiceType(servicetype).createSkel(obj);
		int id = RRObjectHeap.addObject(skel);
		skel.innerskelid = id;
		WrappedRRObject o = new WrappedRRObject(skel.getRRType(), skel, id);
		return _RegisterService(name, servicetype, o,policy);
	}

    public GregorianCalendar nowUTC()
  {
	  return _NowUTC();
	  	  
  }
  
  public final Object findObjRefTyped(Object obj, String objref, String objecttype)
	{
		if (!(obj instanceof ServiceStub))
		{
			throw new UnsupportedOperationException("Only service stubs can have objref");
		}
		ServiceStub s = (ServiceStub)obj;

		return s.findObjRefTyped(objref,objecttype);
	}
	
	public final Object findObjRefTyped(Object obj, String objref, String index, String objecttype)
	{
		if (!(obj instanceof ServiceStub))
		{
			throw new UnsupportedOperationException("Only service stubs can have objref");
		}
		ServiceStub s = (ServiceStub)obj;

		return s.findObjRefTyped(objref,index,objecttype);
	}
	
	//Typemap end
	
	public final void shutdown()
	{
		this._Shutdown();
		RRNativeObjectHeapSupport.set_Support(null);
	}
	
		
	public final void asyncConnectService(String url, String username, Map<String, Object> credentials, Action3<ServiceStub, ClientServiceListenerEventType, Object> listener, String objecttype, Action2<Object,RuntimeException> handler)
  {
	  asyncConnectService(url,username,credentials,listener,objecttype,handler,-1);
  }
  
  	public final void asyncConnectService(String url, String username, Map<String, Object> credentials, Action3<ServiceStub, ClientServiceListenerEventType, Object> listener, String objecttype, Action2<Object,RuntimeException> handler, int timeout)
	{
		MessageElementData credentials2 = null;
		try
		{
			if (username == null)
			{
				username = "";
			}
			if (credentials != null)
			{
				credentials2 = (MessageElementData)packVarType(credentials);
			}
	
			ClientServiceListenerDirectorJava listener2 = null;
			if (listener != null)
			{
				
				
				listener2 = new ClientServiceListenerDirectorJava(listener);
				listener2.setObjectheapid(RRObjectHeap.addObject(listener2));
			}
	
			if (objecttype==null) objecttype="";
			
			AsyncStubReturnDirectorImpl<Object> h=new AsyncStubReturnDirectorImpl<Object>(handler);
			int id1=RRObjectHeap.addObject(h);
			
			_AsyncConnectService(url, username, credentials2, listener2, objecttype,timeout,h,id1);
		}
		finally
		{
			if (credentials2!=null) credentials2.delete();
		}

	}

  	public final void asyncConnectService(String[] url, String username, Map<String, Object> credentials, Action3<ServiceStub, ClientServiceListenerEventType, Object> listener, String objecttype, Action2<Object,RuntimeException> handler)
    {
  	  asyncConnectService(url,username,credentials,listener,objecttype,handler,-1);
    }
  	
  	public final void asyncConnectService(String[] url, String username, Map<String, Object> credentials, Action3<ServiceStub, ClientServiceListenerEventType, Object> listener, String objecttype, Action2<Object,RuntimeException> handler, int timeout)
	{
		MessageElementData credentials2 = null;
		try
		{
			if (username == null)
			{
				username = "";
			}
			if (credentials != null)
			{
				credentials2 = (MessageElementData)packVarType(credentials);
			}
	
			ClientServiceListenerDirectorJava listener2 = null;
			if (listener != null)
			{
				
				
				listener2 = new ClientServiceListenerDirectorJava(listener);
				listener2.setObjectheapid(RRObjectHeap.addObject(listener2));
			}
	
			if (objecttype==null) objecttype="";
			
			AsyncStubReturnDirectorImpl<Object> h=new AsyncStubReturnDirectorImpl<Object>(handler);
			int id1=RRObjectHeap.addObject(h);
			
			vectorstring url1=new vectorstring();
			for (int i=0; i<url.length; i++)
			{
				url1.add(url[i]);
			}
			_AsyncConnectService(url1, username, credentials2, listener2, objecttype,timeout,h,id1);
		}
		finally
		{
			if (credentials2!=null) credentials2.delete();
		}

	}

  	public final void asyncDisconnectService(Object obj, Action handler, int timeout)
  	{
  		ServiceStub stub = (ServiceStub)obj;
        AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl(handler);
        int id = RRObjectHeap.addObject(h);
        _AsyncDisconnectService(stub.rr_innerstub,h,id);
  	}
	
	private class AsyncServiceInfo2DirectorImpl extends AsyncServiceInfo2VectorReturnDirector
  {
	  protected Action1<ServiceInfo2[]> handler_func;
	  
	  public AsyncServiceInfo2DirectorImpl(Action1<ServiceInfo2[]> handler_func)
	  {
		  this.handler_func=handler_func;
	  }
	  
	  @Override
	  public void handler(vectorserviceinfo2wrapped i)
	  {
		  try
		  {
		 		  
		  java.util.ArrayList<ServiceInfo2> o=new java.util.ArrayList<ServiceInfo2>();
		  ServiceInfo2[] o2=new ServiceInfo2[0];
		  try
		  {			  			  
			  for (int ii=0; ii!=i.size(); ii++)
			  {
				  o.add(new ServiceInfo2(i.get(ii)));
			  }
			  
			  o2=o.toArray(new ServiceInfo2[0]);
		  }
		  catch (Exception e)
		  {
			  handler_func.action(new ServiceInfo2[0]);
		  }
		  
		  handler_func.action(o2);
		  }
		  catch (Exception e)
		{
			MessageEntry merr = new MessageEntry();
			RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
			RRDirectorExceptionHelper.setError(merr);
			merr.delete();
		}
	  }
	  
	  
  }

  private class AsyncNodeInfo2DirectorImpl extends AsyncNodeInfo2VectorReturnDirector
  {
	  protected Action1<NodeInfo2[]> handler_func;
	  
	  public AsyncNodeInfo2DirectorImpl(Action1<NodeInfo2[]> handler_func)
	  {
		  this.handler_func=handler_func;
	  }
	  
	  @Override
	  public void handler(vectornodeinfo2 i)
	  {
		  try
		  {
		 		  
		  java.util.ArrayList<NodeInfo2> o=new java.util.ArrayList<NodeInfo2>();
		  NodeInfo2[] o2=new NodeInfo2[0];
		  try
		  {			  
			  
			  for (int ii=0; ii!=i.size(); ii++)
			  {
				  o.add(new NodeInfo2(i.get(ii)));
			  }
			  
			  o2=o.toArray(new NodeInfo2[0]);
		  }
		  catch (Exception e)
		  {
			  handler_func.action(new NodeInfo2[0]);
		  }
		  
		  handler_func.action(o2);	
		  }
		  catch (Exception e)
			{
				MessageEntry merr = new MessageEntry();
				RobotRaconteurExceptionUtil.exceptionToMessageEntry(e, merr);
				RRDirectorExceptionHelper.setError(merr);
				merr.delete();
			}
	  }
	  
	  
    }

    public final void asyncFindServiceByType(String servicetype, String[] transportschemes, Action1<ServiceInfo2[]> handler)
	{
	  asyncFindServiceByType(servicetype,transportschemes,handler,-1);
	  
	}
  
    public final void asyncFindServiceByType(String servicetype, String[] transportschemes, Action1<ServiceInfo2[]> handler, int timeout)
	{
		vectorstring s = new vectorstring();
		for (String s2 : transportschemes)
		{
			s.add(s2);
		}
		
		AsyncServiceInfo2DirectorImpl h=new AsyncServiceInfo2DirectorImpl(handler);
		int id1=RRObjectHeap.addObject(h);
		
		RobotRaconteurJava.asyncWrappedFindServiceByType(this,servicetype, s, timeout, h, id1);

		
	}

    public final void asyncFindNodeByID(NodeID id, String[] transportschemes, Action1<NodeInfo2[]> handler)
	{
    	asyncFindNodeByID(id,transportschemes,handler,-1);
    	
	}
    
	public final void asyncFindNodeByID(NodeID id, String[] transportschemes, Action1<NodeInfo2[]> handler, int timeout)
	{
		vectorstring s = new vectorstring();
		for (String s2 : transportschemes)
		{
			s.add(s2);
		}
		AsyncNodeInfo2DirectorImpl h=new AsyncNodeInfo2DirectorImpl(handler);
		int id1=RRObjectHeap.addObject(h);
		
		RobotRaconteurJava.asyncWrappedFindNodeByID(this,id, s,timeout,h,id1);

		
	}
	
	public final void asyncFindNodeByName(String name, String[] transportschemes, Action1<NodeInfo2[]> handler)
	{
		asyncFindNodeByName(name,transportschemes,handler,-1);		
	}
	
	public final void asyncFindNodeByName(String name, String[] transportschemes, Action1<NodeInfo2[]> handler, int timeout)
	{
		vectorstring s = new vectorstring();
		for (String s2 : transportschemes)
		{
			s.add(s2);
		}
		AsyncNodeInfo2DirectorImpl h=new AsyncNodeInfo2DirectorImpl(handler);
		int id1=RRObjectHeap.addObject(h);
		
		RobotRaconteurJava.asyncWrappedFindNodeByName(this, name, s,timeout,h,id1);
		
	}
	
	public final void updateDetectedNodes(String[] schemes)
	{
		vectorstring schemes1=new vectorstring();
		for(String s : schemes) schemes1.add(s);
		RobotRaconteurJava.wrappedUpdateDetectedNodes(this, schemes1);
	}

	public final void asyncUpdateDetectedNodes(String[] schemes, Action handler)
  	{		
		asyncUpdateDetectedNodes(schemes, handler, 5000);
	}

	public final void asyncUpdateDetectedNodes(String[] schemes, Action handler, int timeout)
  	{  		
		vectorstring schemes1=new vectorstring();
		for(String s : schemes) schemes1.add(s);
        AsyncVoidNoErrReturnDirectorImpl h = new AsyncVoidNoErrReturnDirectorImpl(handler);
        int id = RRObjectHeap.addObject(h);
		RobotRaconteurJava.asyncWrappedUpdateDetectedNodes(this, schemes1, timeout,h,id);       
  	}

	public final NodeID[] getDetectedNodes()
	{
		vectorstring o1 = RobotRaconteurJava.wrappedGetDetectedNodes(this);
        NodeID[] o = new NodeID[(int)o1.size()];
        for (int i = 0; i < o.length; i++)
        {
            o[i] = new NodeID(o1.get(i));
        }
        return o;        
	}

	public void asyncFindObjRefTyped(Object obj, String objref, String objecttype, Action2<Object,RuntimeException> handler)
  {
	asyncFindObjRefTyped(obj,objref,objecttype,handler,-1);
  }
	
  public void asyncFindObjRefTyped(Object obj, String objref, String objecttype, Action2<Object,RuntimeException> handler, int timeout)
  {
      if (!(obj instanceof ServiceStub)) throw new RuntimeException("Only service stubs can have objref");
      ServiceStub s = (ServiceStub)obj;
      
      s.asyncFindObjRefTyped(objref, objecttype, handler, timeout);
  }

  public void asyncFindObjRefTyped(Object obj, String objref, String index, String objecttype, Action2<Object,RuntimeException> handler)
  {
	asyncFindObjRefTyped(obj,objref,index,objecttype,handler,-1);
  }
  
  public void asyncFindObjRefTyped(Object obj, String objref, String index, String objecttype, Action2<Object,RuntimeException> handler, int timeout)
  {
      if (!(obj instanceof ServiceStub)) throw new RuntimeException("Only service stubs can have objref");
      ServiceStub s = (ServiceStub)obj;

      s.asyncFindObjRefTyped(objref, index, objecttype,handler,timeout);
  }

  public String findObjectType(Object obj, String objref)
  {
      if (!(obj instanceof ServiceStub)) throw new RuntimeException("Only service stubs can have objref");
      ServiceStub s = (ServiceStub)obj;

      return _FindObjectType(s.rr_innerstub, objref);
  }

  public String findObjectType(Object obj, String objref, String index)
  {
      if (!(obj instanceof ServiceStub)) throw new RuntimeException("Only service stubs can have objref");
      ServiceStub s = (ServiceStub)obj;

      return _FindObjectType(s.rr_innerstub, objref, index);
  }

  public void asyncFindObjectType(Object obj, String objref, Action2<String,RuntimeException> handler)
  {
	asyncFindObjectType(obj,objref,handler,-1);
  }
  
  public void asyncFindObjectType(Object obj, String objref, Action2<String,RuntimeException> handler, int timeout)
  {
      if (!(obj instanceof ServiceStub)) throw new RuntimeException("Only service stubs can have objref");
      ServiceStub s = (ServiceStub)obj;
      
      AsyncStringReturnDirectorImpl h = new AsyncStringReturnDirectorImpl(handler);
      int id2 = RRObjectHeap.addObject(h);
      _AsyncFindObjectType(s.rr_innerstub, objref,timeout,h,id2);
  }

  public void asyncFindObjectType(Object obj, String objref, String index, Action2<String, RuntimeException> handler)
  {
	asyncFindObjectType(obj,objref,index,handler,-1);
  }
  
  public void asyncFindObjectType(Object obj, String objref, String index, Action2<String, RuntimeException> handler, int timeout)
  {
      if (!(obj instanceof ServiceStub)) throw new RuntimeException("Only service stubs can have objref");
      ServiceStub s = (ServiceStub)obj;

      AsyncStringReturnDirectorImpl h = new AsyncStringReturnDirectorImpl(handler);
      int id2 = RRObjectHeap.addObject(h);
      _AsyncFindObjectType(s.rr_innerstub, objref, index, timeout, h, id2);
  }
	
  public void setExceptionHandler(Action1<RuntimeException> handler)
  {
	if (handler == null)
	{
		_ClearExceptionHandler();
		return;
	}
      AsyncExceptionDirectorImpl h = new AsyncExceptionDirectorImpl(handler);
      int id1 = RRObjectHeap.addObject(h);
      _SetExceptionHandler(h,id1);
  }
  
  public static final int RR_TIMEOUT_INFINITE=-1;

  public final static String[] splitQualifiedName(String name)
  {
	int ind=name.lastIndexOf(".");
	if (ind==-1) throw new RuntimeException("Name is not qualified");
	return new String[] {name.substring(0,ind),name.substring(ind+1)};
  }
  
  public final static String getTypeString(Class type)
  {
	return type.getName().replaceAll("_\\.",".").replaceAll("_$","");
  }

  public final  Timer createTimer(int period, Action1<TimerEvent> handler)
  {
	return createTimer(period, handler,false);
  }
  
  public final Timer createTimer(int period, Action1<TimerEvent> handler, boolean oneshot)
	{
		AsyncTimerEventReturnDirectorImpl t = new AsyncTimerEventReturnDirectorImpl(handler);
		int id = RRObjectHeap.addObject(t);
		return _CreateTimer(period, oneshot, t, id);
	}
  
  public RobotRaconteurException downCastException(RobotRaconteurException exp)
  {
      if (exp==null) return exp;
      String type = exp.error;
      if (!type.contains(".")) return exp;
      String[] stype = RobotRaconteurNode.splitQualifiedName(type);
      if (!isServiceTypeRegistered(stype[0])) return exp;
      return getServiceType(stype[0]).downCastException(exp);

  }
  
  public void postToThreadPool(Action target)
  {
	AsyncVoidNoErrReturnDirectorImpl h=new AsyncVoidNoErrReturnDirectorImpl(target);
	int id = RRObjectHeap.addObject(h);
	_PostToThreadPool(h,id);
  
  }
  
  static class WrappedServiceSubscriptionFilterPredicateDirectorJava extends WrappedServiceSubscriptionFilterPredicateDirector
  {
	Func1<ServiceInfo2, Boolean> _f;

	public WrappedServiceSubscriptionFilterPredicateDirectorJava(Func1<ServiceInfo2, Boolean> f)
	{
		_f = f;
	}
	
	@Override
	public boolean predicate(ServiceInfo2Wrapped info)
	{
		ServiceInfo2 info2=new ServiceInfo2(info);
		return _f.func(info2);
	}

  }

  private WrappedServiceSubscriptionFilter subscribeService_LoadFilter(ServiceSubscriptionFilter filter)
  {
	WrappedServiceSubscriptionFilter filter2=null;

	if (filter != null)
	{
		filter2=new WrappedServiceSubscriptionFilter();

		if (filter.ServiceNames != null)
		{
			for (int i=0; i<filter.ServiceNames.length; i++)
			{
				filter2.getServiceNames().add(filter.ServiceNames[i]);
			}
		}
		
		if (filter.TransportSchemes != null)
		{
			for (int i=0; i<filter.TransportSchemes.length; i++)
			{
				filter2.getTransportSchemes().add(filter.TransportSchemes[i]);
			}
		}

		filter2.setMaxConnections(filter.MaxConnections);

		if (filter.Nodes != null)
		{
			vectorptr_wrappedservicesubscriptionnode nodes2 = new vectorptr_wrappedservicesubscriptionnode();
			for(int i=0; i<filter.Nodes.length; i++)
			{
				ServiceSubscriptionFilterNode n1=filter.Nodes[i];
				if (n1==null) continue;
				WrappedServiceSubscriptionFilterNode n2=new WrappedServiceSubscriptionFilterNode();

				if (n1.NodeID != null) n2.setNodeID(n1.NodeID);
				
				if (n1.NodeName != null) n2.setNodeName(n1.NodeName);
				
				if (n1.Username != null) n2.setUsername(n1.Username);

				if (n1.Credentials != null)
				{
					n2.setCredentials((MessageElementData)packMapType(n1.Credentials, String.class, Object.class));
				}
				nodes2.add(n2);			
			}

			filter2.setNodes(nodes2);
		}

		if (filter.Predicate != null)
		{
			WrappedServiceSubscriptionFilterPredicateDirectorJava director
				=new WrappedServiceSubscriptionFilterPredicateDirectorJava(filter.Predicate);
			int id= RRObjectHeap.addObject(director);
			filter2.setRRPredicateDirector(director,id);
		}

	}

	return filter2;
  }

  public ServiceSubscription subscribeService(String[] service_types)
  {
	return subscribeService(service_types, null);
  }
    
  public ServiceSubscription subscribeService(String[] service_types, ServiceSubscriptionFilter filter)
  {
	
	WrappedServiceSubscriptionFilter filter2=subscribeService_LoadFilter(filter);

	vectorstring service_types2 = new vectorstring();
	for(int i=0; i<service_types.length; i++)
	{
		service_types2.add(service_types[i]);
	}
		
	WrappedServiceSubscription sub1=RobotRaconteurJava.wrappedSubscribeService(this, service_types2, filter2);
	return new ServiceSubscription(sub1);

  }

  public ServiceInfo2Subscription subscribeServiceInfo2(String[] service_types)
  {
	return subscribeServiceInfo2(service_types, null);
  }

  public ServiceInfo2Subscription subscribeServiceInfo2(String[] service_types, ServiceSubscriptionFilter filter)
  {
	
	WrappedServiceSubscriptionFilter filter2=subscribeService_LoadFilter(filter);

	vectorstring service_types2 = new vectorstring();
	for(int i=0; i<service_types.length; i++)
	{
		service_types2.add(service_types[i]);
	}
		
	WrappedServiceInfo2Subscription sub1=RobotRaconteurJava.wrappedSubscribeServiceInfo2(this, service_types2, filter2);
	return new ServiceInfo2Subscription(sub1);

  }


  //End code typemap
  
  
%}

%include "RobotRaconteurNode.i"
