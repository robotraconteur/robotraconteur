# robotraconteur_crosscheck is a tool to check cross-platform compatibility over the network

import RobotRaconteur as RR
import argparse
import json
import socket
import sys
from contextlib import suppress

_crosscheck_robdef = """
service experimental.crosscheck

object Crosscheck
    property string version_robotraconteur [readonly]
    property string hostname [readonly]
    property string platform [readonly]
end
"""


class crosscheck_impl:
    def __init__(self, node):
        self.node = node

    @property
    def version_robotraconteur(self):
        return self.node.RobotRaconteurVersion

    @property
    def hostname(self):
        return socket.gethostname()

    @property
    def platform(self):
        return sys.platform


class crosscheck_server_nodes:
    def __init__(self, args):
        self.insecure_node = None
        self.secure_node = None
        self.insecure_node_setup = None
        self.secure_node_setup = None
        self.insecure_obj = None
        self.secure_obj = None

        # self.secure_obj = crosscheck_impl()

        self.insecure_node = RR.RobotRaconteurNode()
        self.insecure_node.Init()
        self.insecure_obj = crosscheck_impl(self.insecure_node)
        self.insecure_node.RegisterServiceType(_crosscheck_robdef)
        self.insecure_node.RegisterService(
            "crosscheck", "experimental.crosscheck.Crosscheck", self.insecure_obj)
        self.insecure_node_setup = RR.ServerNodeSetup(args.insecure_nodename, args.insecure_tcp_port,
                                                      node=self.insecure_node)

        if args.secure_enabled:
            self.secure_node = RR.RobotRaconteurNode()
            self.secure_node.Init()
            self.secure_obj = crosscheck_impl(self.secure_node)
            self.secure_node.RegisterServiceType(_crosscheck_robdef)
            self.secure_node.RegisterService(
                "crosscheck", "experimental.crosscheck.Crosscheck", self.secure_obj)
            self.secure_node_setup = RR.SecureServerNodeSetup(args.secure_nodename, args.secure_tcp_port,
                                                              node=self.secure_node)

    def close(self):
        if self.insecure_node_setup:
            self.insecure_node_setup.close()

        if self.secure_node_setup:
            self.secure_node_setup.close()


class crosscheck_client:
    def __init__(self, json=False):

        client_node_flags = RR.RobotRaconteurNodeSetupFlags_ENABLE_TCP_TRANSPORT \
            | RR.RobotRaconteurNodeSetupFlags_TCP_TRANSPORT_IPV6_DISCOVERY \
            | RR.RobotRaconteurNodeSetupFlags_ENABLE_NODE_DISCOVERY_LISTENING \
            | RR.RobotRaconteurNodeSetupFlags_DISABLE_STRINGTABLE

        self.json = json
        self.node = RR.RobotRaconteurNode()
        self.node.Init()
        self.node_setup = RR.RobotRaconteurNodeSetup(
            "", 0, client_node_flags, node=self.node)

        insecure_sub_filter = RR.ServiceSubscriptionFilter()
        insecure_sub_filter.TransportSchemes = ["rr+tcp"]
        self.insecure_sub = self.node.SubscribeServiceByType(
            "experimental.crosscheck.Crosscheck", insecure_sub_filter)

        secure_sub_filter = RR.ServiceSubscriptionFilter()
        secure_sub_filter.TransportSchemes = ["rrs+tcp"]
        self.secure_sub = self.node.SubscribeServiceByType(
            "experimental.crosscheck.Crosscheck", secure_sub_filter)

        self.timer = self.node.CreateTimer(1, self.timer_cb)
        self.timer.Start()

    def timer_cb(self, timer_event):
        if not self.json:
            insecure_clients = self.insecure_sub.GetConnectedClients()
            print(f"Insecure connected client count: {len(insecure_clients)}")
            for c in insecure_clients.values():
                print(f"    Node Name: {self.node.GetServiceNodeName(c)}")
                print(f"    Node ID: {self.node.GetServiceNodeID(c)}")
                print(f"    RR Version: {c.version_robotraconteur}")
                print(f"    Hostname: {c.hostname}")
                print(f"    Platform: {c.platform}")
                print()
            secure_clients = self.secure_sub.GetConnectedClients()
            print(f"Secure connected client count: {len(secure_clients)}")
            for c in secure_clients.values():
                print(f"    Node Name: {self.node.GetServiceNodeName(c)}")
                print(f"    Node ID: {self.node.GetServiceNodeID(c)}")
                print(f"    RR Version: {c.version_robotraconteur}")
                print(f"    Hostname: {c.hostname}")
                print(f"    Platform: {c.platform}")
                print()
            print()
        else:
            insecure_clients = self.insecure_sub.GetConnectedClients()
            insecure_clients_list = []
            for c in insecure_clients.values():
                insecure_clients_list.append({
                    "nodename": self.node.GetServiceNodeName(c),
                    "nodeid": str(self.node.GetServiceNodeID(c)),
                    "version_robotraconteur": c.version_robotraconteur,
                    "hostname": c.hostname,
                    "platform": c.platform
                })
            secure_clients = self.secure_sub.GetConnectedClients()
            secure_clients_list = []
            for c in secure_clients.values():
                secure_clients_list.append({
                    "nodename": self.node.GetServiceNodeName(c),
                    "nodeid": str(self.node.GetServiceNodeID(c)),
                    "version_robotraconteur": c.version_robotraconteur,
                    "hostname": c.hostname,
                    "platform": c.platform
                })
            print(json.dumps({
                "insecure": insecure_clients_list,
                "secure": secure_clients_list
            }))
            sys.stdout.flush()

    def close(self):
        if self.node_setup:
            self.node_setup.close()


def main():

    parser = argparse.ArgumentParser(
        description="tool to check Robot Raconteur cross-platform compatibility over the network")
    parser.add_argument("--servers-enabled", type=bool,
                        default=True, help="Enable server nodes")
    parser.add_argument("--insecure-nodename", type=str,
                        default="crosscheck", help="Node name for insecure server node")
    parser.add_argument("--insecure-tcp-port", type=int, default=0,
                        help="TCP listen port for insecure server node")
    parser.add_argument("--secure-enabled", type=bool,
                        default=True, help="Enable secure server node")
    parser.add_argument("--secure-nodename", type=str,
                        default="crosscheck_secure", help="Node name for secure server node")
    parser.add_argument("--secure-tcp-port", type=int, default=0,
                        help="TCP lister port for secure server node")
    parser.add_argument("--json", action="store_true",
                        help="Output in JSON format")

    args = parser.parse_args()

    servers = None
    client = None

    try:
        servers = crosscheck_server_nodes(args)
        client = crosscheck_client(json=args.json)
        print("Press enter to quit")
        input()

    finally:
        if servers:
            servers.close()
        if client:
            client.close()


if __name__ == "__main__":
    main()
