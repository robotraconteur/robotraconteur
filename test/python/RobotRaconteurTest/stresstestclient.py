from RobotRaconteur.Client import *
import sys
import threading
import functools
import traceback

try:
    xrange  # type: ignore
except NameError:
    xrange = range

try:
    raw_input  # type: ignore
except NameError:
    raw_input = input


def main():
    url = sys.argv[1]

    servicetest_count = [0]
    servicetest_connectcount = [0]
    servicetest_keepgoing = True

    servicetest_lock = threading.Lock()

    def servicetest1(o, exp):
        if (exp is not None):
            if (servicetest_keepgoing):
                sys.exit("Got exception: " + str(exp))
        try:
            o.async_func3(1, 2, functools.partial(servicetest2, o))
        except Exception as ee:
            if (servicetest_keepgoing):
                sys.exit("Got exception: " + str(ee))

    def servicetest2(o, d, exp):
        if (exp is not None):
            if (servicetest_keepgoing):
                sys.exit("Got exception: " + str(exp))
        try:
            o.async_func3(1, 2, functools.partial(servicetest2, o))
        except Exception as ee:
            if (servicetest_keepgoing):
                sys.exit("Got exception: " + str(ee))

    def servicetest3(url1, obj, exp):
        if (exp is not None):
            if (servicetest_keepgoing):
                sys.exit("Got exception: " + str(exp))
        try:
            RRN.AsyncDisconnectService(
                obj, functools.partial(servicetest4, url))
        except Exception as ee:
            if (servicetest_keepgoing):
                sys.exit("Got exception: " + str(ee))

    def servicetest4(url):

        RRN.AsyncConnectService(url, None, None, None,
                                functools.partial(servicetest3, url))

    def servicetest5(p, w, ev):
        try:
            if (ev.stopped):
                return
            p.AsyncSendPacket(servicetest_count[0], lambda pnum, err: None)

            for i in xrange(100):
                d = servicetest_count[0] * 100 + i
                w.OutValue = d

            servicetest_count[0] += 1
        except:
            traceback.print_exc()

    def servicetest7(e):
        with servicetest_lock:
            while p.Available > 0:
                p.ReceivePacket()

    o = RRN.ConnectService(url)
    o.async_func3(1, 2, functools.partial(servicetest2, o))

    p = o.broadcastpipe.Connect(-1)
    p.PacketReceivedEvent += servicetest7
    w = o.broadcastwire.Connect()

    # tt=RobotRaconteurNode.s.CreateTimer(.04,functools.partial(servicetest5,p,w))
    tt = RR.WallTimer(.04, functools.partial(servicetest5, p, w))
    tt.Start()

    print("Press enter to stop")
    a = raw_input()

    servicetest_keepgoing = False

    tt.Stop()
    RR.DisconnectService(o)


if __name__ == "__main__":
    main()
