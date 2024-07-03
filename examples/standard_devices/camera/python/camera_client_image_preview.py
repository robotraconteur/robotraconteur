# Simple example Robot Raconteur standard camera client
# This program will show a live preview stream from
# the camera. The preview stream is a MJPEG reduced resolution stream
# that is suitable for real-time display.

from RobotRaconteur.Client import *
from RobotRaconteurCompanion.Util.ImageUtil import ImageUtil

import cv2
import sys
import traceback
import argparse
import numpy as np
import traceback

current_frame = None
image_util = None

# This function is called when a new pipe packet arrives


def new_frame(pipe_ep):
    global current_frame

    # Loop to get the newest frame
    while (pipe_ep.Available > 0):
        # Receive the packet
        image = pipe_ep.ReceivePacket()
        # Convert the packet to an image and set the global variable
        current_frame = image_util.compressed_image_to_array(image)
        return


def main():

    # URL for connecting to the camera. By default connects to Gazebo simulated camera
    url = 'rr+tcp://localhost:59823?service=camera'
    if (len(sys.argv) >= 2):
        url = sys.argv[1]

    # Connect to the camera
    cam = RRN.ConnectService(url)

    # Create an ImageUtil object to help with image conversion
    global image_util
    image_util = ImageUtil(RRN, cam)

    # Connect the pipe preview_stream to get the PipeEndpoint p

    p = cam.preview_stream.Connect(-1)

    # Set the callback for when a new pipe packet is received to the
    # new_frame function
    p.PacketReceivedEvent += new_frame

    # Start the camera streaming, ignore error if it is already streaming
    try:
        cam.start_streaming()
    except:
        traceback.print_exc()
        pass

    cv2.namedWindow("Image")

    while True:
        # Display the image
        if (not current_frame is None):

            cv2.imshow("Image", current_frame)
        if cv2.waitKey(50) != -1:
            break
    cv2.destroyAllWindows()

    p.Close()
    cam.stop_streaming()


if __name__ == '__main__':
    main()
