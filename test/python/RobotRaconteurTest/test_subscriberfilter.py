import RobotRaconteur as RR
import random
import time

_robdef = """
service com.robotraconteur.testing.subtestfilter

object sub_testroot
    property double d1
end

object sub_testroot2
    property double d1
end

"""


class _sub_testroot_impl(object):
    def __init__(self):
        self.d1 = random.random()


def _init_node(nodename, servicename, attributes, servicetype_suffix=""):
    node1 = RR.RobotRaconteurNode()
    node1.Init()
    node1.SetNodeName(nodename)
    t1 = RR.IntraTransport(node1)
    node1.RegisterTransport(t1)
    t1.StartServer()

    node1.RegisterServiceType(_robdef)

    c1 = _sub_testroot_impl()
    c1_context = node1.RegisterService(
        servicename, "com.robotraconteur.testing.subtestfilter.sub_testroot" + servicetype_suffix, c1)
    c1_context.SetServiceAttributes(attributes)
    return node1


def _register_service_auth(node, servicename, servicetype_suffix=""):
    authdata = "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock\ntestuser2 841c4221c2e7e0cefbc0392a35222512 objectlock\ntestsuperuser 503ed776c50169f681ad7bbc14198b68 objectlock,objectlockoverride"
    p = RR.PasswordFileUserAuthenticator(authdata)
    policies = {"requirevaliduser": "true", "allowobjectlock": "true"}
    s = RR.ServiceSecurityPolicy(p, policies)
    c1 = _sub_testroot_impl()
    c1_context = node.RegisterService(
        servicename, "com.robotraconteur.testing.subtestfilter.sub_testroot" + servicetype_suffix, c1, s)


def _init_client_node():
    node1 = RR.RobotRaconteurNode()
    node1.Init()
    t1 = RR.IntraTransport(node1)
    node1.RegisterTransport(t1)
    t1.StartClient()
    node1.SetLogLevelFromString("WARNING")

    return node1


def _assert_connected_clients(c, count):
    try_count = 0
    if count == 0:
        time.sleep(0.5)
    while True:
        time.sleep(0.1)

        try:
            assert len(c.GetConnectedClients()) == count
            break
        except AssertionError:
            if try_count > 50:
                s = c.GetConnectedClients()
                for s1 in s:
                    print(s1.NodeID.ToString())
                    print(s1.ServiceName)
                print(len(c.GetConnectedClients()))
                raise
            try_count += 1
            time.sleep(0.1)


def _run_attributes_filter_test(client_node, attributes_groups, expected_count):
    filter1 = RR.ServiceSubscriptionFilter()

    for k, v in attributes_groups.items():
        filter1.Attributes[k] = v

    sub2 = client_node.SubscribeServiceByType(
        ["com.robotraconteur.testing.subtestfilter.sub_testroot"], filter1)
    _assert_connected_clients(sub2, expected_count)
    sub2.Close()


def _run_filter_test(client_node, filter_, expected_count, servicetype_suffix=""):

    sub2 = client_node.SubscribeServiceByType(
        ["com.robotraconteur.testing.subtestfilter.sub_testroot" + servicetype_suffix], filter_)
    _assert_connected_clients(sub2, expected_count)
    sub2.Close()


def test_subscriber_attribute_filter():

    # Use IntraTransport for tests so everything is local to the test

    # Create server nodes

    node1_attrs = {
        "a1": RR.VarValue("test_attr_val1", "string"),
        "a2": RR.VarValue("test_attr_val2", "string"),
        "a4": RR.VarValue("test_attr_val4,test_attr_val4_1", "string")
    }

    node2_attrs = {
        "a2": RR.VarValue("test_attr_val2", "string"),
        "a3": RR.VarValue("test_attr_val3,test_attr_val3_1", "string")
    }

    node3_attrs = {
        "a1": RR.VarValue("test_attr_val3", "string"),
        "a2": RR.VarValue("test_attr_val5", "string"),
        "a4": RR.VarValue("test_attr_val4,test_attr_val4_2", "string")
    }

    node1 = _init_node("test_node1", "service1", node1_attrs)
    node2 = _init_node("test_node2", "service2", node2_attrs)
    node3 = _init_node("test_node7", "service3", node3_attrs)

    time.sleep(0.5)

    # Create client node
    client_node = _init_client_node()

    # Connect and disconnect to make sure everything is working
    c1 = client_node.ConnectService(
        "rr+intra:///?nodename=test_node1&service=service1")
    c2 = client_node.ConnectService(
        "rr+intra:///?nodename=test_node2&service=service2")
    c3 = client_node.ConnectService(
        "rr+intra:///?nodename=test_node7&service=service3")

    client_node.DisconnectService(c1)
    client_node.DisconnectService(c2)
    client_node.DisconnectService(c3)

    sub1 = client_node.SubscribeServiceByType(
        ["com.robotraconteur.testing.subtestfilter.sub_testroot"])
    _assert_connected_clients(sub1, 3)
    sub1.Close()

    attr_grp1 = RR.ServiceSubscriptionFilterAttributeGroup()
    attr_grp1.Attributes.append(
        RR.ServiceSubscriptionFilterAttribute("test_attr_val1"))
    attr_grps1 = {"a1": attr_grp1}
    _run_attributes_filter_test(client_node, attr_grps1, 1)

    attr_grp2 = RR.ServiceSubscriptionFilterAttributeGroup()
    attr_grp2.Attributes.append(
        RR.ServiceSubscriptionFilterAttribute("test_attr_val4"))
    attr_grp2.Attributes.append(
        RR.ServiceSubscriptionFilterAttribute("test_attr_val4_2"))
    attr_grps2 = {"a4": attr_grp2}
    _run_attributes_filter_test(client_node, attr_grps2, 2)

    attr_grp4 = RR.ServiceSubscriptionFilterAttributeGroup()
    attr_grp4.Attributes.append(
        RR.ServiceSubscriptionFilterAttribute("test_attr_val4"))
    attr_grp4.Attributes.append(
        RR.ServiceSubscriptionFilterAttribute("test_attr_val4_2"))
    attr_grps4 = {"a4": attr_grp4}
    attr_grp4.Operation = RR.ServiceSubscriptionFilterAttributeGroupOperation_AND
    _run_attributes_filter_test(client_node, attr_grps4, 1)

    attr_grp3 = RR.ServiceSubscriptionFilterAttributeGroup()
    attr_grp3.Attributes.append(
        RR.CreateServiceSubscriptionFilterAttributeRegex(".*_attr_val1"))
    attr_grps3 = {"a1": attr_grp3}
    _run_attributes_filter_test(client_node, attr_grps3, 1)

    attr_grp5 = RR.ServiceSubscriptionFilterAttributeGroup()
    attr_grp5.Attributes.append(
        RR.CreateServiceSubscriptionFilterAttributeRegex(".*_attr_val1"))
    attr_grps5 = {"a1": attr_grp5}
    attr_grp6 = RR.ServiceSubscriptionFilterAttributeGroup()
    attr_grp6.Attributes.append(
        RR.ServiceSubscriptionFilterAttribute("test_attr_val_not_there"))
    attr_grps5["a4"] = attr_grp6

    filter2 = RR.ServiceSubscriptionFilter()
    filter2.Attributes = attr_grps5
    filter2.AttributesMatchOperation = RR.ServiceSubscriptionFilterAttributeGroupOperation_OR
    _run_filter_test(client_node, filter2, 1)

    node1.Shutdown()
    node2.Shutdown()
    node3.Shutdown()


def test_subscriber_filter():

    node1 = _init_node("test_node3", "service1", {}, "2")
    node2 = _init_node("test_node4", "service1", {}, "2")
    node3 = _init_node("test_node5", "service3", {}, "2")
    node4 = _init_node("test_node6", "service2", {}, "2")

    _register_service_auth(node3, "service1", "2")

    time.sleep(0.5)

    # Create client node
    client_node = _init_client_node()

    # Connect and disconnect to make sure everything is working
    c1 = client_node.ConnectService(
        "rr+intra:///?nodename=test_node3&service=service1")
    c2 = client_node.ConnectService(
        "rr+intra:///?nodename=test_node4&service=service1")
    cred1 = {"password": RR.VarValue("testpass1", "string")}
    c3 = client_node.ConnectService(
        "rr+intra:///?nodename=test_node5&service=service1", "testuser1", cred1)
    c4 = client_node.ConnectService(
        "rr+intra:///?nodename=test_node6&service=service2")

    client_node.DisconnectService(c1)
    client_node.DisconnectService(c2)
    client_node.DisconnectService(c3)
    client_node.DisconnectService(c4)

    sub1 = client_node.SubscribeServiceByType(
        ["com.robotraconteur.testing.subtestfilter.sub_testroot2"])
    _assert_connected_clients(sub1, 5)
    sub1.Close()

    filter1 = RR.ServiceSubscriptionFilter()
    filter1.ServiceNames.append("service1")
    _run_filter_test(client_node, filter1, 3, "2")

    filter1_node = RR.ServiceSubscriptionFilterNode()
    filter1_node.NodeName = "test_node3"
    filter1.Nodes.append(filter1_node)
    _run_filter_test(client_node, filter1, 1, "2")

    filter2 = RR.ServiceSubscriptionFilter()
    filter2_node = RR.ServiceSubscriptionFilterNode()
    filter2_node.NodeName = "test_node5"
    filter2_node.Username = "testuser1"
    filter2_node.Credentials = {"password": RR.VarValue("testpass1", "string")}
    filter2.ServiceNames.append("service1")
    filter2.Nodes.append(filter2_node)
    _run_filter_test(client_node, filter2, 1, "2")

    node1.Shutdown()
    node2.Shutdown()
    node3.Shutdown()
    node4.Shutdown()
