# device_connector_yaml.py - Example of using the DeviceConnector class to connect to a device

# To run this example:
#    python device_connector_yml.py --devices devices.yaml

from RobotRaconteur.Client import *
from RobotRaconteurCompanion.Util.DeviceConnector import DeviceConnector, DeviceConnectorDetails
from RobotRaconteurCompanion.Util.ImageUtil import ImageUtil
import cv2
import argparse

# Parse command line arguments to get the devices yaml file object
parser = argparse.ArgumentParser(description="Device connector example")
parser.add_argument("--devices", type=argparse.FileType("r"), required=True, help="The devices yaml file")
args, _ = parser.parse_known_args()

# Create a DeviceConnector object and load the devices yaml file
con = DeviceConnector(devices_yaml_f=args.devices)

# Get the ServiceSubscription to the camera
sub = con.GetDevice("camera")

# Get the default client and capture and image
c = sub.GetDefaultClientWait(10)
raw_img = c.capture_frame()
image_util = ImageUtil(client_obj=c)

# Display the image
img = image_util.image_to_array(raw_img)
cv2.namedWindow("Image")
cv2.imshow("Image", img)
cv2.waitKey()
cv2.destroyAllWindows()
