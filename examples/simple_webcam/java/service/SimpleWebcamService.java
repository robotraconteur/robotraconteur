import experimental.simplewebcam3.*;

import com.robotraconteur.*;

// This program provides a simple Robot Raconteur server for viewing multiple webcams.
// It uses the Webcam_interface.robdef service definition

public class SimpleWebcamService
{

    /**
     * @param args
     */
    public static void main(String[] args)
    {
        try
        {

            // Load the opencv library
            nu.pattern.OpenCV.loadShared();

            // Create the webcam service object
            Webcam_impl webcam = new Webcam_impl(0, "Camera");

            ServerNodeSetup setup = new ServerNodeSetup("experimental.simplewebcam3", 22355);
            try
            {
                // Register the Webcam_interface type so that the node can understand the service definition
                RobotRaconteurNode.s().registerServiceType(new experimental__simplewebcam3Factory());

                // Register the webcam host object as a service so that it can be connected to
                RobotRaconteurNode.s().registerService("webcam", "experimental.simplewebcam3", webcam);

                // Stay open until shut down
                System.out.println("Webcam server started press enter to quit");
                System.in.read();

                // Shutdown
                webcam.shutdown();
            }
            finally
            {
                setup.finalize();
            }
        }
        catch (Exception e)
        {
            e.printStackTrace(System.out);
        }
    }
}
