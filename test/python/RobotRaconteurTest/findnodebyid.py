from RobotRaconteur.Client import *
import sys
import time


def main():
    rrid = RR.NodeID(sys.argv[1])
    transports = sys.argv[2].split(",")

    time.sleep(6)

    ret = RRN.FindNodeByID(rrid, transports)

    for r in ret:
        print_NodeInfo2(r)

    def printret(ret):
        for r in ret:
            print_NodeInfo2(r)
    RRN.AsyncFindNodeByID(rrid, transports, printret, 1)

    time.sleep(2)


def print_NodeInfo2(s):
    print("NodeID: " + str(s.NodeID))
    print("NodeName: " + s.NodeName)
    print("ConnectionURL: " + str(s.ConnectionURL))


if __name__ == "__main__":
    main()
