from RobotRaconteur.Client import *
import sys


def main():

    if (len(sys.argv) < 2):
        print("Usage for peeridentity:  peeridentity url")
        return

    url1 = sys.argv[1]

    o = RRN.ConnectService(url1)

    o.func3(1.0, 2.4)

    if (tcp_transport.IsTransportConnectionSecure(o)):
        print("Connection is secure")

        if (tcp_transport.IsSecurePeerIdentityVerified(o)):
            print("Peer identity is verified: " +
                  tcp_transport.GetSecurePeerIdentity(o))
        else:
            print("Peer identity is not verified")
    return
