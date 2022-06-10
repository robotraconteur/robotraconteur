from RobotRaconteur.Client import *
import sys
import time

try:
    raw_input  # type: ignore
except NameError:
    raw_input = input


def main():

    if (len(sys.argv) < 2):
        print(
            "Usage for serviceinfo2subscribertest:  serviceinfo2subscribertest servicetype")
        return

    service_type = sys.argv[1]

    s = RRN.SubscribeServiceInfo2(service_type)

    def detected(s, client_id, info):
        print("Service detected: " + str(client_id.NodeID) +
              "," + client_id.ServiceName + "\n")

    def lost(s, client_id, info):
        print("Service lost: " + str(client_id.NodeID) +
              "," + client_id.ServiceName + "\n")
    s.ServiceDetected += detected
    s.ServiceLost += lost

    time.sleep(3)
    print(s.GetDetectedServiceInfo2())

    raw_input("Press enter")

    return


if __name__ == "__main__":
    main()
