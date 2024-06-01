import com.robotraconteur.*;

import java.awt.image.*;
import java.io.*;
import javax.imageio.*;
import javax.swing.*;

import org.opencv.core.*;
import org.opencv.imgcodecs.*;

import experimental.simplewebcam3.*;

// Simple client to read images from a Webcam server
// and display the image.  This example uses the "memory"
// member type

public class SimpleWebcamClient_memory
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

            // Capture an image to the "buffer" and "multidimbuffer"
            WebcamImage_size size = c1.capture_frame_to_buffer();

            // Read the full image from the "buffer" memory
            long l = c1.get_buffer().length();
            UnsignedBytes data = new UnsignedBytes((int)l);
            c1.get_buffer().read(0, data, 0, l);

            // Convert and show the image retrieved from the buffer memory
            Mat frame1 = new Mat(size.height, size.width, CvType.CV_8UC3);
            frame1.put(0, 0, data.value);

            // Show the image
            showImage("buffer", frame1);

            // Read the dimensions of the "multidimbuffer" member
            long[] bufsize = c1.get_multidimbuffer().dimensions();

            // Retrieve the data from the "multidimbuffer"
            MultiDimArray segdata = new MultiDimArray(new int[] {100, 100, 1}, new UnsignedBytes(10000));
            c1.get_multidimbuffer().read(new long[] {10, 10, 0}, segdata, new long[] {0, 0, 0},
                                         new long[] {100, 100, 1});

            // Create a new image to hold the image
            Mat frame2 = new Mat(100, 100, CvType.CV_8U);

            // This will actually give you the transpose of the image because MultiDimArray is stored in column-major
            // order, transpose the image to be the correct orientation
            frame2.put(0, 0, ((UnsignedBytes)segdata.array).value);
            Mat frame3 = frame2.t();

            // Show the image
            showImage("multidimbuffer", frame3);
        }
        finally
        {
            setup.finalize();
        }
    }

    public static void showImage(String title, Mat img)
    {
        MatOfByte matOfByte = new MatOfByte();

        Imgcodecs.imencode(".bmp", img, matOfByte);
        byte[] byteArray = matOfByte.toArray();
        BufferedImage bufImage = null;
        try
        {
            InputStream in = new ByteArrayInputStream(byteArray);
            bufImage = ImageIO.read(in);
            JFrame frame = new JFrame();
            frame.setTitle(title);
            frame.getContentPane().add(new JLabel(new ImageIcon(bufImage)));
            frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
            frame.pack();
            frame.setVisible(true);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}
