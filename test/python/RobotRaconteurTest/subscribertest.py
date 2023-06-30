from RobotRaconteur.Client import *
import sys
import time

try:
    raw_input  # type: ignore
except NameError:
    raw_input = input


def main():

    if (len(sys.argv) < 2):
        print("Usage for subscribertest:  subscribertest servicetype")
        return

    service_type = sys.argv[1]

    s = RRN.SubscribeServiceByType(service_type)

    def connected(s, client_id, client):
        print("Client connected: " + str(client_id.NodeID) +
              "," + client_id.ServiceName)

    def disconnected(s, client_id, client):
        print("Client disconnected: " + str(client_id.NodeID) +
              "," + client_id.ServiceName)
    s.ClientConnected += connected
    s.ClientDisconnected += disconnected

    def wire_value_changed(s, value, time):
        # print "Wire value changed: " + str(value) + ", " + str(time)
        pass

    w = s.SubscribeWire('broadcastwire')
    w.WireValueChanged += wire_value_changed

    def pipe_value_changed(s):
        while True:
            (res, p) = s.TryReceivePacket()
            if (not res):
                break
            print(p)

    p = s.SubscribePipe('broadcastpipe')
    p.PipePacketReceived += pipe_value_changed
    time.sleep(3)
    print(s.GetConnectedClients())
    print(w.InValue)

    try_val_res, try_val, try_val_ts = w.TryGetInValue()
    assert try_val_res
    print(try_val)

    raw_input("Press enter")

    return


if __name__ == "__main__":
    main()
