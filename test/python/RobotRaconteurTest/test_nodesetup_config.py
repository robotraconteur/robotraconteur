import RobotRaconteur as RR


def test_nodesetup_config_parser():
    # RobotRaconteurNodeSetup(config=...) must construct the wrapped setup with the node and the config
    node = RR.RobotRaconteurNode()
    node.Init()
    config = RR.CommandLineConfigParser(0)
    config.SetDefaults("nodesetup_config_test", 0, RR.RobotRaconteurNodeSetupFlags_ENABLE_INTRA_TRANSPORT)
    config.ParseCommandLine(RR.RobotRaconteurPython.vectorstring([]))
    with RR.RobotRaconteurNodeSetup(node=node, config=config) as setup:
        assert setup.intra_transport is not None
        assert setup.tcp_transport is None
        assert node.NodeName == "nodesetup_config_test"
