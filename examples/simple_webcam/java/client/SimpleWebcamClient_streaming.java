import com.robotraconteur.*;

import java.awt.image.*;
import java.io.*;
import javax.imageio.*;
import javax.swing.*;

import org.opencv.core.*;
import org.opencv.imgcodecs.*;

import experimental.simplewebcam3.*;

// Simple client to read streaming images from the Webcam pipe to show
// a live view from the cameras

public class SimpleWebcamClient_streaming
{

    /**
     * @param args
     */
    public static void main(String[] args)
    {

        String url = "rr+tcp://localhost:22355?service=webcam";
        if (args.length > 0)
        {
            url = args[0];
        }

        // Load the opencv library
        nu.pattern.OpenCV.loadShared();

        // Use ClientNodeSetup to initialize node
        ClientNodeSetup setup = new ClientNodeSetup();

        try
        {

            // Register the service type
            RobotRaconteurNode.s().registerServiceType(new experimental__simplewebcam3Factory());

            // Connect to the service
            Webcam c1 = (Webcam)RobotRaconteurNode.s().connectService(url, null, null, null,
                                                                      "experimental.simplewebcam3.Webcam");

            // Connect to the FrameStream pipe and receive a PipeEndpoint
            // PipeEndpoints a symmetric on client and service meaning that
            // you can send and receive on both ends
            Pipe<WebcamImage>.PipeEndpoint p = c1.get_frame_stream().connect(-1);
            // Add a callback for when a new pipe packet is received
            p.addPacketReceivedListener(new new_frame());

            // Start the packets streaming.  If there is an exception ignore it.
            // Exceptions are passed transparently to the client/service.
            try
            {
                c1.start_streaming();
            }
            catch (Exception e)
            {}

            JFrame frame = new JFrame();
            JLabel label = new JLabel();
            frame.setTitle("Live View");
            frame.getContentPane().add(label);
            frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
            frame.pack();
            frame.setVisible(true);

            while (true)
            {
                if (current_frame != null)
                {
                    try
                    {
                        MatOfByte matOfByte = new MatOfByte();

                        Imgcodecs.imencode(".bmp", current_frame, matOfByte);
                        byte[] byteArray = matOfByte.toArray();
                        BufferedImage bufImage = null;
                        InputStream in = new ByteArrayInputStream(byteArray);
                        bufImage = ImageIO.read(in);
                        label.setIcon(new ImageIcon(bufImage));
                        frame.pack();
                    }
                    catch (Exception e)
                    {
                        e.printStackTrace();
                        return;
                    }
                }
                if (!frame.isVisible())
                {
                    break;
                }
                try
                {
                    Thread.sleep(50);
                }
                catch (InterruptedException e)
                {}
            }

            // Close the PipeEndpoint
            p.close();

            // Stop streaming the frame
            c1.stop_streaming();
        }
        finally
        {
            setup.finalize();
        }
    }

    // Convert WebcamImage to OpenCV format
    public static Mat WebcamImageToMat(WebcamImage image)
    {
        Mat mat = new Mat(image.height, image.width, CvType.CV_8UC3);
        mat.put(0, 0, image.data.value);
        return mat;
    }

    public static Mat current_frame;

    // Callback for when a new frame packet is received
    public static class new_frame implements Action1<Pipe<WebcamImage>.PipeEndpoint>
    {
        public void action(Pipe<WebcamImage>.PipeEndpoint pipe_ep)
        {
            while (pipe_ep.available() > 0)
            {
                WebcamImage image = pipe_ep.receivePacket();
                current_frame = WebcamImageToMat(image);
            }
        }
    }
}
