import RobotRaconteur as RR


def test_ws_https():
    node1 = RR.RobotRaconteurNode()
    node1.Init()

    with RR.ClientNodeSetup(node=node1):
        c = node1.ConnectService(
            'rr+wss://wstest2.wasontech.com/robotraconteur?service=testobj')

        assert c.add_two_numbers(2, 4) == 6

    print("Test ws https passed")
