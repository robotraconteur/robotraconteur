from RobotRaconteur.Client import *
import sys
import time

# cSpell: ignore rrname


def main():
    rrname = sys.argv[1]
    transports = sys.argv[2].split(",")

    time.sleep(6)

    ret = RRN.FindNodeByName(rrname, transports)

    for r in ret:
        print_NodeInfo2(r)

    def printret(ret):
        for r in ret:
            print_NodeInfo2(r)
    RRN.AsyncFindNodeByName(rrname, transports, printret, 1)

    time.sleep(2)


def print_NodeInfo2(s):
    print("NodeID: " + str(s.NodeID))
    print("NodeName: " + s.NodeName)
    print("ConnectionURL: " + str(s.ConnectionURL))


if __name__ == "__main__":
    main()
