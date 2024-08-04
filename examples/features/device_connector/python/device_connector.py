# device_connector.py - Example of using the DeviceConnector class to connect to a device

from RobotRaconteur.Client import *
from RobotRaconteurCompanion.Util.DeviceConnector import DeviceConnector, DeviceConnectorDetails
from RobotRaconteurCompanion.Util.ImageUtil import ImageUtil
import cv2

# Create a DeviceConnector object
con = DeviceConnector()

# Add a device to the connector
d1 = DeviceConnectorDetails(device_nickname="camera", device="gazebo_camera")
con.AddDevice(d1)

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
