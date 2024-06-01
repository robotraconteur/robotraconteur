using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotRaconteur;
using experimental.simplewebcam3;
using OpenCvSharp;
using System.Runtime.InteropServices;

namespace SimpleWebcamClient
{
// Simple client to read images from a Webcam server
// and display the images

class Program
{
    static void Main(string[] args)
    {
        // Use ClientNodeSetup to initialize node
        using (new ClientNodeSetup())
        {

            // Connect to the service
            WebcamHost c_host = (WebcamHost)RobotRaconteurNode.s.ConnectService(
                "rr+tcp://localhost:22355?service=multiwebcam", objecttype: "experimental.simplewebcam3.WebcamHost");

            // Get the Webcam objects from the "Webcams" objref
            Webcam c1 = c_host.get_webcams(0);
            Webcam c2 = c_host.get_webcams(1);

            // Capture an image and convert to OpenCV image type
            Mat frame1 = WebcamImageToCVImage(c1.capture_frame());
            Mat frame2 = WebcamImageToCVImage(c1.capture_frame());

            // Show image
            Cv2.ImShow(c1.name, frame1);
            Cv2.ImShow(c2.name, frame2);

            // Wait for enter to be pressed
            Cv2.WaitKey(0);
        }
    }

    // Convert WebcamImage to OpenCV format
    static Mat WebcamImageToCVImage(WebcamImage i)
    {

        Mat o = new Mat(i.height, i.width, MatType.CV_8UC3);
        if (i.data.Length != o.Total() * o.ElemSize())
        {
            throw new ArgumentException("Invalid image data size returned by service");
        }
        Marshal.Copy(i.data, 0, o.Data, i.data.Length);
        return o;
    }
}
}
