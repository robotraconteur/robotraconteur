# Simple example Robot Raconteur webcam client
# This program will capture a frame from both webcams and show it
# on the screen
from RobotRaconteur.Client import *
import time
import numpy
import cv2
import sys

# Function to take the data structure returned from the Webcam service
# and convert it to an OpenCV array


def webcam_image_to_mat(image):
    frame2 = image.data.reshape([image.height, image.width, 3], order='C')
    return frame2

# Main program


def main():

    url = 'rr+tcp://localhost:22355/?service=multiwebcam'
    if (len(sys.argv) >= 2):
        url = sys.argv[1]

    # Start up Robot Raconteur and connect, standard by this point
    c_host = RRN.ConnectService(url)

    # Use objref's to get the cameras. c_host is a "WebcamHost" type
    # and is used to find the webcams
    c1 = c_host.get_webcams(0)
    c2 = c_host.get_webcams(1)

    # Pull a frame from each camera, c1 and c2
    frame1 = webcam_image_to_mat(c1.capture_frame())
    try:
        frame2 = webcam_image_to_mat(c2.capture_frame())
    except:
        print("Warning: could not read some second camera")
        frame2 = None

    # Show the images
    cv2.imshow(c1.name, frame1)
    if frame2 is not None:
        cv2.imshow(c2.name, frame2)

    # CV wait for key press on the image window and then destroy
    cv2.waitKey()
    cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
