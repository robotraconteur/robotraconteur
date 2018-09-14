package com.robotraconteur;

import java.util.Enumeration;
import java.util.Vector;
import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Map;

public class ServiceInfo2Subscription
{
	class WrappedServiceInfo2SubscriptionDirectorJava extends WrappedServiceInfo2SubscriptionDirector
	{
		WeakReference<ServiceInfo2Subscription> subscription1;

		WrappedServiceInfo2SubscriptionDirectorJava(ServiceInfo2Subscription subscription)
		{
			subscription1 = new WeakReference<ServiceInfo2Subscription>(subscription);
		}

		@Override
		public void serviceDetected(WrappedServiceInfo2Subscription subscription, WrappedServiceSubscriptionClientID id, ServiceInfo2Wrapped info)
		{
			ServiceInfo2Subscription s=subscription1.get();
			if (s==null) return;
						
			ServiceSubscriptionClientID id1=new ServiceSubscriptionClientID(id);
			ServiceInfo2 info1=new ServiceInfo2(info);

			synchronized (s.detected_listeners)
			{
				for(Enumeration<Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2>> e= s.detected_listeners.elements(); e.hasMoreElements(); )
				{
					Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2> ee=e.nextElement();
					ee.action(s, id1, info1);
						
				}
			}

		}

		@Override
		public void serviceLost(WrappedServiceInfo2Subscription subscription, WrappedServiceSubscriptionClientID id, ServiceInfo2Wrapped info)
		{
			ServiceInfo2Subscription s=subscription1.get();
			if (s==null) return;
						
			ServiceSubscriptionClientID id1=new ServiceSubscriptionClientID(id);
			ServiceInfo2 info1=new ServiceInfo2(info);

			synchronized (s.lost_listeners)
			{
				for(Enumeration<Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2>> e= s.lost_listeners.elements(); e.hasMoreElements(); )
				{
					Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2> ee=e.nextElement();
					ee.action(s, id1, info1);
						
				}
			}

		}
				
	}

	Vector<Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2>> detected_listeners = new Vector<Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2>>();
	Vector<Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2>> lost_listeners = new Vector<Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2>>();
	HashMap<Integer,Object> client_stubs = new HashMap<Integer,Object>();
	    
	WrappedServiceInfo2Subscription _subscription;

	ServiceInfo2Subscription(WrappedServiceInfo2Subscription subscription)
	{
		_subscription = subscription;
		WrappedServiceInfo2SubscriptionDirectorJava director = new WrappedServiceInfo2SubscriptionDirectorJava(this);
		int id=RRObjectHeap.addObject(director);
		subscription.setRRDirector(director,id);
	}

	public Map<ServiceSubscriptionClientID, ServiceInfo2> getDetectedServiceInfo2()
	{
		HashMap<ServiceSubscriptionClientID, ServiceInfo2> o = new HashMap<ServiceSubscriptionClientID, ServiceInfo2>();
		map_subscriptionserviceinfo2 c1 = _subscription.getDetectedServiceInfo2();

		vectorptr_subscriptionclientid c2=RobotRaconteurJava.wrappedServiceInfo2SubscriptionServicesToVector(c1);

		for (int i=0; i< c2.size(); i++)
		{
			WrappedServiceSubscriptionClientID id2=c2.get(i);
			ServiceInfo2 info1=new ServiceInfo2(c1.get(id2));
			o.put(new ServiceSubscriptionClientID(id2), info1);
		}

		return o;
	}

	public void close()
	{
		_subscription.close();
	}

	public void addServiceDetectedListener(Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2> listener)
	{
		synchronized (detected_listeners)
		{
			detected_listeners.add(listener);
		}
	}

	public void removeServiceDetectedListener(Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2> listener)
	{
		synchronized (detected_listeners)
		{
			detected_listeners.remove(listener);
		}
	}

	public void addServiceLostListener(Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2> listener)
	{
		synchronized (lost_listeners)
		{
			lost_listeners.add(listener);
		}
	}

	public void removeLostListenerListener(Action3<ServiceInfo2Subscription, ServiceSubscriptionClientID, ServiceInfo2> listener)
	{
		synchronized (lost_listeners)
		{
			lost_listeners.remove(listener);
		}
	}
		

}