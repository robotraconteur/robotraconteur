import com.robotraconteur.*;
import experimental.create3.*;

// This program provides a simple Robot Raconteur server for controlling the iRobot Create.

public class iRobotCreateService
{

    /**
     * @param args
     */
    public static void main(String[] args)
    {

        ServerNodeSetup setup = new ServerNodeSetup("experimental.create3", 22354);

        try
        {
            String port = "/dev/ttyUSB0"; // Default port
            if (args.length > 0)
            {
                port = args[0];
            }

            // Initialize the create robot object
            Create_impl c = new Create_impl();
            c.start(port);

            // Register the Create_interface type so that the node can understand the service definition
            RobotRaconteurNode.s().registerServiceType(new experimental.create3.experimental__create3Factory());

            // Register the create object as a service so that it can be connected to
            RobotRaconteurNode.s().registerService("create", "experimental.create3", c);

            // Stay open until shut down
            System.out.println("Create server started press enter to quit");
            System.in.read();

            c.close();
        }
        catch (Exception e)
        {
            e.printStackTrace(System.out);
        }
        finally
        {
            setup.finalize();
        }
    }
}
