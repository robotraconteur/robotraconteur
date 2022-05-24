from RobotRaconteur.Client import *
import sys
import time

try:
    raw_input  # type: ignore
except NameError:
    raw_input = input


def main():

    if (len(sys.argv) < 2):
        print("Usage for subscriberfiltertest: subscriberfiltertest servicetype")
        return

    service_type = sys.argv[1]

    f = RR.ServiceSubscriptionFilter()
    if (len(sys.argv) >= 4):
        subcommand = sys.argv[2]

        if (subcommand == "nodeid"):
            if (len(sys.argv) < 4):
                raise Exception(
                    "Usage for subscriberfiltertest:  subscriberfiltertest nodeid <nodeid>")

            n = RR.ServiceSubscriptionFilterNode()
            n.NodeID = RR.NodeID(sys.argv[3])
            f.Nodes.append(n)
        elif (subcommand == "nodename"):
            if (len(sys.argv) < 4):
                raise Exception(
                    "Usage for subscriberfiltertest:  subscriberfiltertest nodename <nodename>")

            n = RR.ServiceSubscriptionFilterNode()
            n.NodeName = sys.argv[3]
            f.Nodes.append(n)
        elif (subcommand == "nodeidscheme"):
            if (len(sys.argv) < 5):
                raise Exception(
                    "Usage for subscriberfiltertest:  subscriberfiltertest nodeidscheme <nodeid> <schemes>")

            n = RR.ServiceSubscriptionFilterNode()
            n.NodeID = RR.NodeID(sys.argv[3])
            f.Nodes.append(n)
            f.TransportSchemes = sys.argv[4].split(',')
        elif (subcommand == "nodeidauth"):
            if (len(sys.argv) < 6):
                raise Exception(
                    "Usage for subscriberfiltertest:  subscriberfiltertest nodeidauth <nodeid> <username> <password>")

            n = RR.ServiceSubscriptionFilterNode()
            n.NodeID = RR.NodeID(sys.argv[3])
            n.Username = sys.argv[4]
            n.Credentials = {'password': RR.VarValue(sys.argv[6], 'string')}
            f.Nodes.append(n)
        elif (subcommand == "servicename"):
            if (len(sys.argv) < 4):
                raise Exception(
                    "Usage for subscriberfiltertest:  subscriberfiltertest servicename <servicename>")
            f.ServiceNames.append(sys.argv[3])

        elif (subcommand == "predicate"):

            def pred(serviceinfo):
                print(serviceinfo.NodeName)
                return serviceinfo.NodeName == 'testprog'

            f.Predicate = pred

        else:
            raise Exception("Unknown subscriberfiltertest subcommand")

    s = RRN.SubscribeServiceByType(service_type, f)

    def connected(s, client_id, client):
        print("Client connected: " + str(client_id.NodeID) +
              "," + client_id.ServiceName)

    def disconnected(s, client_id, client):
        print("Client disconnected: " + str(client_id.NodeID) +
              "," + client_id.ServiceName)
    s.ClientConnected += connected
    s.ClientDisconnected += disconnected

    raw_input("Press enter")

    return


if __name__ == "__main__":
    main()
