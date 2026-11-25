import RobotRaconteur as RR


def test_node_timeout_properties_float():
    # The timeout properties are documented as float seconds
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        node.RequestTimeout = 1.5
        assert node.RequestTimeout == 1.5
        node.TransportInactivityTimeout = 2.25
        assert node.TransportInactivityTimeout == 2.25
        node.EndpointInactivityTimeout = 3.5
        assert node.EndpointInactivityTimeout == 3.5
        node.RequestTimeout = 20
        assert node.RequestTimeout == 20.0
    finally:
        node.Shutdown()


def test_tcp_transport_timeout_properties_float():
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        t = RR.TcpTransport(node)
        t.DefaultReceiveTimeout = 2.5
        assert t.DefaultReceiveTimeout == 2.5
        t.DefaultConnectTimeout = 1.25
        assert t.DefaultConnectTimeout == 1.25
        t.DefaultHeartbeatPeriod = 0.5
        assert t.DefaultHeartbeatPeriod == 0.5
        t.NodeAnnouncePeriod = 10.5
        assert t.NodeAnnouncePeriod == 10.5
        t.Close()
    finally:
        node.Shutdown()
