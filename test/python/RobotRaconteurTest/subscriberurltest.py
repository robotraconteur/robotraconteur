from RobotRaconteur.Client import *
import sys
import time

try:
    raw_input  # type: ignore
except NameError:
    raw_input = input


def main():

    if (len(sys.argv) < 2):
        print("Usage for subscriberurltest:  subscriberurltest.py url")
        return

    url = sys.argv[1]

    print(url)

    s = RRN.SubscribeService(url)

    def connected(s, client_id, client):
        print("Client connected: " + str(client_id.NodeID) +
              "," + client_id.ServiceName)

    def disconnected(s, client_id, client):
        print("Client disconnected: " + str(client_id.NodeID) +
              "," + client_id.ServiceName)

    def connect_failed(s, client_id, url, err):
        print("Client connect failed: " + str(client_id.NodeID) +
              " url: " + str(url) + " error: " + str(err))
    s.ClientConnected += connected
    s.ClientDisconnected += disconnected
    s.ClientConnectFailed += connect_failed

    def async_get_handler(obj, err):
        if err is not None:
            print("AsyncGetDefaultClient error: " + str(err))
        else:
            print("AsyncGetDefaultClient success: " + str(obj))

    s.AsyncGetDefaultClient(async_get_handler, 1)
    client1 = s.GetDefaultClientWait(6)
    print(s.TryGetDefaultClientWait(6))
    print(s.GetConnectedClients())
    try:
        print(s.GetDefaultClient().d1)
    except:
        print("Client not connected")
    print(s.TryGetDefaultClient())
    raw_input("Press enter")


if __name__ == "__main__":
    main()
