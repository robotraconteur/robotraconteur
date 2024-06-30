using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotRaconteur;
using experimental.simplewebcam3;
using OpenCvSharp;
using System.Runtime.InteropServices;

namespace simple_webcam_client_memory
{
// Simple client to read images from a Webcam server
// and display the image.  This example uses the "memory"
// member type
class Program
{
    static void Main(string[] args)
    {
        // Use ClientNodeSetup to initialize node
        using (new ClientNodeSetup())
        {

            // Connect to the service
            Webcam c1 = (Webcam)RobotRaconteurNode.s.ConnectService("rr+tcp://localhost:22355?service=webcam",
                                                                    objecttype: "experimental.simplewebcam3.Webcam");

            // Capture an image to the "buffer" and "multidimbuffer"
            WebcamImage_size size = c1.capture_frame_to_buffer();

            // Read the full image from the "buffer" memory
            ulong l = c1.buffer.Length;
            byte[] data = new byte[l];
            c1.buffer.Read(0, data, 0, l);

            // Convert and show the image retrieved from the buffer memory
            Mat frame1 = new Mat(size.height, size.width, MatType.CV_8UC3);
            if (data.Length != frame1.Total() * frame1.ElemSize())
            {
                throw new ArgumentException("Invalid image data size returned by service");
            }
            Marshal.Copy(data, 0, frame1.Data, (int)l);

            Cv2.ImShow("buffer", frame1);

            // Read the dimensions of the "multidimbuffer" member
            ulong[] bufsize = c1.multidimbuffer.Dimensions;

            // Retrieve the data from the "multidimbuffer"
            byte[] segdata_bytes = new byte[100000];
            MultiDimArray segdata = new MultiDimArray(new uint[] { 100, 100, 1 }, segdata_bytes);
            c1.multidimbuffer.Read(new ulong[] { 10, 10, 0 }, segdata, new ulong[] { 0, 0, 0 },
                                   new ulong[] { 100, 100, 1 });

            // Create a new image to hold the image
            //  Image<Gray, byte> frame2 = new Image<Gray, byte>(100, 100);
            Mat frame2 = new Mat(100, 100, MatType.CV_8UC1);

            // This will actually give you the transpose of the image because MultiDimArray is stored in column-major
            // order, as an exercise transpose the image to be the correct orientation
            //  frame2.Bytes = segdata_bytes;
            Marshal.Copy(segdata_bytes, 0, frame2.Data, 10000);

            // Take transpose because of array ordering difference
            Mat frame3 = frame2.T();

            // Show the image
            Cv2.ImShow("multidimbuffer", frame3);
            Cv2.WaitKey(0);
        }
    }
}
}
