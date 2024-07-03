# Simple example Robot Raconteur standard camera client
# This program will capture a single compressed frame and
# display it.

from RobotRaconteur.Client import *
from RobotRaconteurCompanion.Util.ImageUtil import ImageUtil

import cv2
import sys
import traceback
import argparse
import numpy as np
import traceback


def main():

    # URL for connecting to the camera. By default connects to Gazebo simulated camera
    url = 'rr+tcp://localhost:59823?service=camera'
    if (len(sys.argv) >= 2):
        url = sys.argv[1]

    # Connect to the camera
    cam = RRN.ConnectService(url)

    # Create an ImageUtil object to help with image conversion
    image_util = ImageUtil(RRN, cam)

    # Capture the compressed frame from the camera, returns in PNG format
    raw_frame = cam.capture_frame_compressed()

    # Convert raw_frame to opencv format using companion library ImageUtil
    current_frame = image_util.compressed_image_to_array(raw_frame)

    cv2.namedWindow("Image")

    cv2.imshow("Image", current_frame)
    cv2.waitKey()

    cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
