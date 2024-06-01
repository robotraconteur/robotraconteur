# Simple example Robot Raconteur webcam service

# Note: This example is intended to demonstrate Robot Raconteur
# and is designed to be simple rather than optimal.

import time
import RobotRaconteur as RR
# Convenience shorthand to the default node.
# RRN is equivalent to RR.RobotRaconteurNode.s
RRN = RR.RobotRaconteurNode.s
import threading
import numpy
import traceback
import cv2
import platform
import sys
import argparse
from pathlib import Path
import drekar_launch_process

# Class that implements a single webcam


class Webcam_impl(object):
    # Init the camera being passed the camera number and the camera name
    def __init__(self, cameraid, cameraname):
        self._lock = threading.RLock()
        self._streaming = False
        self._cameraname = cameraname

        self._webcamimage_type = RRN.GetStructureType("experimental.simplewebcam3.WebcamImage")
        self._webcamimage_size_type = RRN.GetStructureType("experimental.simplewebcam3.WebcamImage_size")

        # Create buffers for memory members
        self._buffer = numpy.array([], dtype="u1")
        self._multidimbuffer = numpy.array([], dtype="u1")

        # Initialize the camera
        with self._lock:
            if platform.system() == "Windows":
                self._capture = cv2.VideoCapture(cameraid + cv2.CAP_DSHOW)
            else:
                self._capture = cv2.VideoCapture(cameraid)
            self._capture.set(3, 320)
            self._capture.set(4, 240)

            for _ in range(10):
                try:
                    self._capture.read()
                except:
                    pass

    def RRServiceObjectInit(self, ctx, service_path):
        # frame_stream pipe member property getter and setter

        # Set the PipeBroadcaster backlog to 3 so packets
        # will be dropped if the transport is overloaded.
        # readonly pipes will be automatically initialized with a
        # PipeBroadcaster
        self.frame_stream.MaxBacklog = 3

    # Return the camera name
    @property
    def name(self):
        return self._cameraname

    # Capture a frame and return a WebcamImage structure to the client
    def capture_frame(self):
        with self._lock:
            image = self._webcamimage_type()
            ret, frame = self._capture.read()
            if not ret:
                raise Exception("Could not read from webcam")
            image.width = frame.shape[1]
            image.height = frame.shape[0]
            image.step = frame.shape[1] * 3
            image.data = frame.reshape(frame.size, order='C')

            return image

    # Start the thread that captures images and sends them through connected
    # frame_stream pipes
    def start_streaming(self):
        if (self._streaming):
            raise Exception("Already streaming")
        self._streaming = True
        t = threading.Thread(target=self.frame_threadfunc)
        t.start()

    # Stop the streaming thread
    def stop_streaming(self):
        if (not self._streaming):
            raise Exception("Not streaming")
        self._streaming = False

    # Function that will send a frame at ideally 4 fps, although in reality it
    # will be lower because Python is quite slow.  This is for
    # demonstration only...
    def frame_threadfunc(self):
        # Loop as long as we are streaming
        while (self._streaming):
            # Capture a frame
            try:
                frame = self.capture_frame()
            except:
                # TODO: notify the client that streaming has failed
                self._streaming = False
                return
            # Send the new frame to the broadcaster.  Use AsyncSendPacket
            # and a blank handler.  We really don't care when the send finishes
            # since we are using the "backlog" flow control in the broadcaster.
            self.frame_stream.AsyncSendPacket(frame, lambda: None)

            # Put in a 100 ms delay
            time.sleep(.1)

    # Captures a frame and places the data in the memory buffers
    def capture_frame_to_buffer(self):
        with self._lock:
            # Capture and image and place it into the buffer
            image = self.capture_frame()

            self._buffer = image.data
            self._multidimbuffer = numpy.concatenate((image.data[2::3].reshape((image.height, image.width, 1)), image.data[1::3].reshape(
                (image.height, image.width, 1)), image.data[0::3].reshape((image.height, image.width, 1))), axis=2)

            # Create and populate the size structure and return it
            size = self._webcamimage_size_type()
            size.height = image.height
            size.width = image.width
            size.step = image.step
            return size

    # Return the memories.  It would be better to reuse the memory objects,
    # but for simplicity return new instances when called
    @property
    def buffer(self):
        return RR.ArrayMemory(self._buffer)

    @property
    def multidimbuffer(self):
        return RR.MultiDimArrayMemory(self._multidimbuffer)

    # Shutdown the Webcam

    def _shutdown(self):
        self._streaming = False
        del (self._capture)


def main():

    # Use the robdef from a file. In practice, this is usually done using
    # a package resource. See RobotRaconteurCompanion.Util.RobDef.register_service_types_from_resources
    RRN.RegisterServiceTypesFromFiles(
        [str(Path(__file__).parent.parent.parent / "robdef" / "experimental.simplewebcam3.robdef")])

    obj = Webcam_impl(0, "camera")

    with RR.ServerNodeSetup("experimental.simplewebcam3", 22355, argv=sys.argv):

        ctx = RRN.RegisterService("webcam", "experimental.simplewebcam3.Webcam", obj)

        obj.capture_frame_to_buffer()

        print("Simple Webcam Service Started")
        print()
        ctx.PrintCandidateConnectionURLs()
        print()
        print("Press Ctrl-C to quit")

        drekar_launch_process.wait_exit()

        # Shutdown
        obj._shutdown()


if __name__ == '__main__':
    main()
