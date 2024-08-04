# pyri_device_manager_client.py - Example of using the PyriDeviceManagerClient class to connect to a device

# The package pyri-common must be installed to use this example. See https://github.com/pyri-project/pyri-core
# for more information.

from pyri.device_manager_client import DeviceManagerClient
from RobotRaconteur.Client import *
from RobotRaconteurCompanion.Util.ImageUtil import ImageUtil
import cv2

# Create the DeviceManagerClient object
d = DeviceManagerClient()

# Refresh the available devices from the service
d.refresh_devices(1)

# Get the default client and capture and image
c = d.get_device_client("camera", timeout=10)
raw_img = c.capture_frame()
image_util = ImageUtil(client_obj=c)

# Display the image
img = image_util.image_to_array(raw_img)
cv2.namedWindow("Image")
cv2.imshow("Image", img)
cv2.waitKey()
cv2.destroyAllWindows()
