# C++ Subscriptions {#cpp_subscriptions}

Subscriptions are a powerful feature of Robot Raconteur that can be used to automate the lifecycle of client connections. They come in three varieties:

| Subscription Type | Description | Creation Function |
| ---               | ---         | ---               |
| Service Subscription | Subscribe to a specific service using a URL | RobotRaconteur::RobotRaconteurNode::SubscribeService() |
| Service Type Subscription | Subscribe to all services of a specific type, optionally with a filter | RobotRaconteur::RobotRaconteurNode::SubscribeServiceByType() |
| ServiceInfo2 Subscription | Subscribe to information about services, receiving notifications as they are detected and lost | RobotRaconteur::RobotRaconteurNode::SubscribeServiceInfo2() |

## Service Subscriptions {#cpp_service_subscription}

Service subscriptions are used to create a robust connection to a service. The subscription is created using RobotRaconteur::RobotRaconteurNode::SubscribeService(). It takes the service URL, an optional username, and optional credentials. The ServiceSubscription will attempt to maintain a connection to the service, retrying the connection if it fails or is lost.

The connected client can be accessed using RobotRaconteur::ServiceSubscription::GetDefaultClient() or RobotRaconteur::ServiceSubscription::TryGetDefaultClient(). It is recommended these functions be called right before the client is used. This will ensure that the the client is the most recent connection, in case the client was reconnected after a connection was lost.

When using `pipe` or `wire` members, it is recommended that RobotRaconteur::PipeSubscription or RobotRaconteur::WireSubscription be used. These classes automatically create connections to the members, to send and receive streaming data.

An example of using a RobotRaconteur::ServiceSubscription to invoke a function:

    ServiceSubscriptionPtr sub = RobotRaconteurNode::s()->SubscribeService("rr+tcp://localhost:62222?service=my_service");
    
    experimental::service_example1::MyObjectPtr obj;
    while (!sub->TryGetDefaultClient(obj))
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    // obj is now a connected client that can be used normally
    obj->my_function();

Next, an example of using RobotRaconteur::WireSubscription for a client to the first order system service example from \ref cpp_service_wire:

    ServiceSubscriptionPtr sub = RobotRaconteurNode::s()->SubscribeService("rr+tcp://localhost:62222?service=filter_service");

    // Create wire subscriptions to input `u` and output `y`
    WireSubscriptionPtr<double> u_sub = sub->SubscribeWire<double>("u");
    u->SetIgnoreInValue(true); // Only use `u` for sending value
    WireSubscriptionPtr<double> y_sub = sub->SubscribeWire<double>("y");

    boost::posix_time::ptime start = RobotRaconteurNode::s()->NowUTC();

    // Use a Rate to stabilize the loop
    RatePtr rate = RobotRaconteurNode::s()->CreateRate(100);
    
    // Run loop
    while (true)
    {
        rate->Sleep();

        double y;
        if (!y_sub->TryGetInValue(y))
        {
            continue;
        }

        double t = (RobotRaconturNode::s()->NowUTC() - start).total_milliseconds()/1000.0;
        double u = sin(t);

        u_sub->SetOutValueAll(u);

        std::cout << "y=" << y << ", u=" << u << std::endl;    
    }

The above example uses wires to read and write the output `y` and input `u` of the first order system. Using wire subscriptions means that the user does not need to manually manage the lifecycle of the client connections.

## Subscription Filters {#cpp_subscription_filters}

RobotRaconteur::RobotRaconteurNode::SubscribeServiceByType() and RobotRaconteur::RobotRaconteurNode::SubscribeServiceInfo2() take a service type that should be connected or tracked, and a RobotRaconteur::ServiceSubscriptionFilterPtr to filter which services should be connected or tracked. See RobotRaconteur::ServiceSubscriptionFilter for more information on the criteria that can be used to filter nodes.

## Service Type Subscriptions {#cpp_subscription_service_type}

Service type subscriptions subscribe to all services that match a certain type and match an optional filter. They are created using RobotRaconteur::RobotRaconteurNode::SubscribeFilterByType(), and return a RobotRaconteur::ServiceSubscription. Service type subscriptions are similar to service subscriptions, except that multiple clients may be connected if they match the specified type. The connected clients may be accessed as a map using RobotRaconteur::ServiceSubscription::GetConnectedClients().

An example of connecting to a service with a filter:

    ServiceSubscriptionFilterPtr filter = boost::make_shared<ServiceSubscriptionFilter>();
    filter->max_connection_count = 8; // Limit to 8 concurrent connections.
    filter->ServiceNames = {"my_service"}; // Require a specific ServiceName to connect

    ServiceSubscriptionFilterNodePtr node_filter1 = boost::make_shared<ServiceSubscriptionFilterNode>();
    node_filter1->NodeName = "my_fancy_node"; // Require a specific node name
    ServiceSubscriptionFilterNodePtr node_filter2 = boost::make_shared<ServiceSubscriptionFilterNode>();
    node_filter2->NodeName = "another_fancy_node"; // Another allowed node name
    filter->Nodes = {node_filter1, node_filter2};

    ServiceSubscriptionPtr sub = RobotRaconteurNode::s()->SubscribeService({"experimental.service_example1"});

    // Give a few seconds for clients to be connected
    boost::this_thread::sleep(boost::posix_time::milliseconds(2500));

    // Now use the connected clients
    for (auto c : sub->GetConnectedClients())
    {
        experimental::service_example1::MyObjectPtr c2 
            = boost::dynamic_pointer_cast<experimental::service_example1::MyObject>(c.second);
        if (!c2)
            continue;
        c2->do_something();
    }

Note that the filter is optional, and may be omitted if no filtering is required. Using `max_connection_count` is highly recommended since the subscription may potentially connect more clients than the system can handle.

RobotRaconteur::ServiceSubscription may also use wire and pipe subscriptions, created using RobotRaconteur::ServiceSubscription::SubscribePipe() and RobotRaconteur::ServiceSubscription::SubscribeWire(). Note that more than one client may be connected and sending data to these members. If only one client is expected, use `max_connection_count = 1` in the filter, or use RobotRaconteur::RobotRaconteurNode::SubscribeService() to subscribe to a specific URL.

The clients retrieved using `GetConnectedClients()` have their lifecycle managed by the subscription. This means the subscription could disconnect and reconnect the client at any time. If the user needs a client to stay connected, the client can be "claimed". A claimed client will no longer have its lifecycle managed by the subscription. When the user is done with the client, it can be "released" so the subscription can continue to manage its lifecycle. The user claims a client using RobotRaconteur::ServiceSubscription::ClaimClient(). The claim is released using RobotRaconteur::ServiceSubscription::ReleaseClient().

## ServiceInfo2 Subscription {#cpp_subscription_serviceinfo2}

ServiceInfo2 subscriptions are used to track services as they are detected and lost. Unlike the service and service type subscriptions, they do not automatically create client connections. An example use case for ServiceInfo2 subscriptions is a dialog shown to users of detected services that is updated as services are detected or lost. Information about detected services is returned as RobotRaconteur::ServiceInfo2 structures.

ServiceInfo2 subscriptions are created using RobotRaconteur::RobotRaconteurNode::SubscribeServiceInfo2(). The function returns a RobotRaconteur::ServiceInfo2SubscriptionPtr. The `SubscribeServiceInfo2` takes a service name, and a filter. See \ref cpp_subscription_service_type for an example of using the filter.

The currently detected services can be retrieved using RobotRaconteur::ServiceInfo2Subscription::GetDetectedServiceInfo2(). Listeners for services being detected and lost can be added using RobotRaconteur::ServiceInfo2Subscription::AddServiceDetectedListener() and RobotRaconteur::ServiceInfo2Subscription::AddServiceLostListener().

An example using ServiceInfo2 subscription:

    ServiceSubscriptionPtr sub = RobotRaconteurNode::s()->SubscribeService({"experimental.service_example1"});

    // Give a few seconds for services to be detected
    boost::this_thread::sleep(boost::posix_time::milliseconds(2500));

    // Print out currently detected services
    for (auto e : sub->GetDetectedServiceInfo2())
    {
        for(auto e : detected_services)
        {
            std::cout << "Name: " << e.second.Name << std::endl;           
            std::cout << "ConnectionURL: " << boost::join(e.second.ConnectionURL,", ") << std::endl << std::endl;
        }
    }

    // Add listeners
    sub->AddServiceDetectedListener(
        [] (ServiceInfo2SubscriptionPtr subscription, const ServiceSubscriptionClientID& client_id, const ServiceInfo2& service_info)
        {
            std::cout << "Service Detected:"  << std::endl;
            std::cout << "Name: " << service_info.Name << std::endl;           
            std::cout << "ConnectionURL: " << boost::join(service_info.ConnectionURL,", ") << std::endl << std::endl;
        }
    );

    sub->AddServiceLostListener(
        [] (ServiceInfo2SubscriptionPtr subscription, const ServiceSubscriptionClientID& client_id, const ServiceInfo2& service_info)
        {
            std::cout << "Service Lost:"  << std::endl;
            std::cout << "Name: " << service_info.Name << std::endl;            
        }
    );

    // Continue with program, or spin to watch services come and go

