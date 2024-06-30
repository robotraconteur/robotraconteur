using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotRaconteur;
using experimental.simplewebcam3;
using OpenCvSharp;
using System.Runtime.InteropServices;

namespace simple_webcam_client_streaming
{
// Simple client to read streaming images from the Webcam pipe to show
// a live view from the cameras

class Program
{
    static void Main(string[] args)
    {
        // Use ClientNodeSetup to initialize node
        using (new ClientNodeSetup())
        {
            // Connect to the service
            Webcam c = (Webcam)RobotRaconteurNode.s.ConnectService("rr+tcp://localhost:22355?service=webcam",
                                                                   objecttype: "experimental.simplewebcam3.Webcam");

            // Connect to the FrameStream pipe and receive a PipeEndpoint
            // PipeEndpoints a symmetric on client and service meaning that
            // you can send and receive on both ends
            Pipe<WebcamImage>.PipeEndpoint p = c.frame_stream.Connect(-1);
            // Add a callback for when a new pipe packet is received
            p.PacketReceivedEvent += new_frame;

            // Start the packets streaming.  If there is an exception ignore it.
            // Exceptions are passed transparently to the client/service.
            try
            {
                c.start_streaming();
            }
            catch (Exception e)
            {
                Console.WriteLine("Was already streaming...");
            }

            // Show a named window
            Cv2.NamedWindow("Image");

            // Loop through and show the new image if available
            while (true)
            {
                if (current_frame != null)
                {
                    Cv2.ImShow("Image", current_frame);
                }
                // Break the loop if "enter" is pressed on a window
                if (Cv2.WaitKey(50) != -1)
                    break;
            }

            // Close the window
            Cv2.DestroyWindow("Image");

            // Close the PipeEndpoint
            p.Close();

            // Stop streaming frame
            c.stop_streaming();
        }
    }

    static Mat current_frame = null;

    // Convert a frame to OpenCV format
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

    // Function to handle when a new frame is received
    // This function will be called by a separate thread by
    // Robot Raconteur.
    // Note: callbacks don't always need to be static
    static void new_frame(Pipe<WebcamImage>.PipeEndpoint pipe_ep)
    {
        // Get the newest frame and save it to the variable to be shown
        // by the display thread
        while (pipe_ep.Available > 0)
        {
            WebcamImage image = pipe_ep.ReceivePacket();
            current_frame = WebcamImageToCVImage(image);
        }
    }
}
}
