using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using experimental.simplewebcam3;
using RobotRaconteur;
using System.Threading;
using System.Runtime.InteropServices;
using OpenCvSharp;
using DrekarLaunchProcess;

namespace SimpleWebcamService
{
// This program provides a simple Robot Raconteur server for viewing multiple webcams.
// It uses the Webcam_interface.robdef service definition
class Program
{
    static void Main(string[] args)
    {
        Webcam_impl cam = new Webcam_impl(0, "camera");

        // Use ServerNodeSetup to initialize server node
        using (new ServerNodeSetup("experimental.simplewebcam3", 22355))
        {
            // Register the webcam host object as a service so that it can be connected to
            var ctx = RobotRaconteurNode.s.RegisterService("webcam", "experimental.simplewebcam3", cam);

            cam.capture_frame();

            // Stay open until shut down
            Console.WriteLine("Simple Webcam Service Started");
            Console.WriteLine();
            Console.WriteLine("Candidate connection urls:");
            ctx.PrintCandidateConnectionURLs();
            Console.WriteLine();
            Console.WriteLine("Press Ctrl-C to quit");

            // Use DrekarLaunchProcessNET to wait for exit
            using (var wait_exit = new CWaitForExit())
            {
                wait_exit.WaitForExit();
            }

            // Shutdown
            cam.Shutdown();
        }
    }
}

// Class to implement the "Webcam" Robot Raconteur object
public class Webcam_impl : Webcam_default_impl, IRRServiceObject
{

    VideoCapture _capture;

    // Initialize the webcam
    public Webcam_impl(int cameraid, string cameraname)
    {
        _capture = new VideoCapture(cameraid);
        _capture.Set(VideoCaptureProperties.FrameWidth, 320);
        _capture.Set(VideoCaptureProperties.FrameHeight, 240);
        _Name = cameraname;
    }

    // Set frame_stream to set MaximumBacklog
    public void RRServiceObjectInit(ServerContext ctx, string service_path)
    {
        rrvar_frame_stream.MaxBacklog = 3;
    }

    // Shutdown the webcam
    public void Shutdown()
    {
        lock (this)
        {
            _capture.Dispose();
        }
    }

    string _Name = "";

    //"Name" property
    public override string name
    {
        get {
            return _Name;
        }
    }

    // Function to capture a frame and return the Robot Raconteur WebcamImage structure
    public override WebcamImage capture_frame()
    {
        lock (this)
        {
            var i = _capture.RetrieveMat();

            byte[] data = new byte[i.Total() * i.ElemSize()];
            Marshal.Copy(i.Data, data, 0, data.Length);

            WebcamImage o = new WebcamImage();
            o.height = i.Height;
            o.width = i.Width;
            o.step = (int)i.Step();
            o.data = data;
            return o;
        }
    }

    bool streaming = false;

    // Start streaming frames
    public override void start_streaming()
    {
        lock (this)
        {
            if (streaming)
                throw new InvalidOperationException("Already streaming");
            streaming = true;

            // Create a thread that retrieves and transmits frames
            Thread t = new Thread(frame_threadfunc);
            t.Start();
        }
    }

    // Stop the image streaming frame
    public override void stop_streaming()
    {
        lock (this)
        {
            if (!streaming)
                throw new InvalidOperationException("Not streaming");
            streaming = false;
        }
    }

    // Thread to stream frames by capturing data and sending it to
    // all connected PipeEndpoints
    public void frame_threadfunc()
    {
        while (streaming)
        {
            // Capture a frame
            WebcamImage frame = capture_frame();

            if (rrvar_frame_stream != null)
            {
                rrvar_frame_stream.SendPacket(frame);
            }

            Thread.Sleep(100);
        }
    }

    byte[] _buffer = new byte[0];
    MultiDimArray _multidimbuffer = new MultiDimArray(new uint[3] { 0, 0, 0 }, new byte[0]);

    // Capture a frame and save it to the memory buffers
    public override WebcamImage_size capture_frame_to_buffer()
    {
        WebcamImage image = capture_frame();
        _buffer = image.data;

        // Rearrange the data into the correct format for MATLAB arrays
        byte[] mdata = new byte[image.height * image.width * 3];
        // cSpell: ignore mdbuf
        MultiDimArray mdbuf = new MultiDimArray(new uint[] { (uint)image.height, (uint)image.width, 3 }, mdata);
        for (int channel = 0; channel < 3; channel++)
        {
            int channel0 = image.height * image.width * channel;
            for (int x = 0; x < image.width; x++)
            {
                for (int y = 0; y < image.height; y++)
                {
                    byte value = image.data[(y * image.step + x * 3) + (2 - channel)];
                    mdata[channel0 + x * image.height + y] = value;
                }
            }
        }
        _multidimbuffer = mdbuf;

        // Return a WebcamImage_size structure to the client
        WebcamImage_size size = new WebcamImage_size();
        size.width = image.width;
        size.height = image.height;
        size.step = image.step;
        return size;
    }

    // Return an ArrayMemory for the "buffer" data containing the image.
    public override ArrayMemory<byte> buffer
    {
        get {
            // In many cases this ArrayMemory would not be initialized every time,
            // but for this example return a new ArrayMemory
            return new ArrayMemory<byte>(_buffer);
        }
    }

    // Return a MultiDimArray for the "multidimbuffer" data containing the image
    public override MultiDimArrayMemory<byte> multidimbuffer
    {
        get {
            // In many cases this MultiDimArrayMemory would not be initialized every time,
            // but for this example return a new MultiDimArrayMemory
            return new MultiDimArrayMemory<byte>(_multidimbuffer);
        }
    }
}
}
