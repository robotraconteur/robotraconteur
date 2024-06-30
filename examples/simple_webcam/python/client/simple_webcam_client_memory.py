# Simple example Robot Raconteur webcam client
# This program will capture a frame from both webcams and show it
# on the screen

from RobotRaconteur.Client import *
import time
import numpy
import cv2
import sys

# Main program


def main():

    url = 'rr+tcp://localhost:22355?service=webcam'
    if (len(sys.argv) >= 2):
        url = sys.argv[1]

    # Start up Robot Raconteur and connect, standard by this point
    c1 = RRN.ConnectService(url)

    # Save image to buffer
    size = c1.capture_frame_to_buffer()

    # Read the data from the "buffer "memory. For this example just read the
    # entire buffer
    l = c1.buffer.Length
    data = numpy.zeros(l, dtype="u1")

    c1.buffer.Read(0, data, 0, l)

    frame1 = data.reshape([size.height, size.width, 3], order='C')
    cv2.imshow("buffer", frame1)

    # Read segment from the "multidimbuffer" and display the "red" channel
    bufsize = c1.multidimbuffer.Dimensions
    print(bufsize)

    # create a smaller buffer with 1 channel
    segdata = numpy.zeros([100, 100, 1], dtype="u1")
    c1.multidimbuffer.Read([10, 10, 0], segdata, [0, 0, 0], [100, 100, 1])

    cv2.imshow("multidimbuffer", segdata)

    # CV wait for key press on the image window and then destroy
    cv2.waitKey(0)
    cv2.destroyAllWindows()


if __name__ == '__main__':
    main()
