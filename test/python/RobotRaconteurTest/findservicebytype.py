from RobotRaconteur.Client import *
import sys
import time


def main():
    rrtype = sys.argv[1]
    transports = sys.argv[2].split(",")

    time.sleep(6)

    ret = RRN.FindServiceByType(rrtype, transports)

    for r in ret:
        print_ServiceInfo2(r)

    def printret(ret):
        for r in ret:
            print_ServiceInfo2(r)
    RRN.AsyncFindServiceByType(rrtype, transports, printret, 1)

    time.sleep(2)


def print_ServiceInfo2(s):
    print("Name: " + s.Name)
    print("RootObjectType: " + s.RootObjectType)
    print("RootObjectImplements: " + str(s.RootObjectImplements))
    print("ConnectionURL: " + str(s.ConnectionURL))
    print("Attributes: " + str(s.Attributes))
    print("NodeID: " + str(s.NodeID))
    print("NodeName: " + s.NodeName)
    print("")


if __name__ == "__main__":
    main()
