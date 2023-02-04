# -------------------------------------------------------------------------------
# Name:        module1
# Purpose:
#
# Author:      wasonj
#
# Created:     17/10/2014
# Copyright:   (c) wasonj 2014
# Licence:     <your licence>
# -------------------------------------------------------------------------------

import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s
import time


def ev1():
    print "Event 1"


def ev2(a, b):
    print "Event2 : " + str(a) + " " + str(b)


def main():
    RRN.RegisterTransport(RR.LocalTransport())
    RRN.RegisterTransport(RR.TcpTransport())

    cred1 = {"password": RR.RobotRaconteurVarValue("testpass1", "string")}
    c = RRN.ConnectService(
        'tcp://localhost:4343/{0}/MatlabTestService', "testuser1", cred1)

    c.testevent1 += ev1
    c.testevent2 += ev2

    c.testfunc1(10, 20)
    print c.testprop1
    c.testprop1 = 30
    print c.testfunc2(20)

    print c.testprop2
    s = RRN.NewStructure('example.MatlabTestService.MatlabTestStruct', c)
    s.val1 = 38274
    s.val2 = [1.234, 3.14]

    c.testprop2 = s
    c2 = c.testprop2
    print c2.val1
    print c2.val2

    time.sleep(2)

    RRN.DisconnectService(c)


if __name__ == '__main__':
    main()
