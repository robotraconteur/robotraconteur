import java.io.Console;
import java.util.*;
import com.robotraconteur.*;

public class reynard_robotraconteur_client
{
    public static void main(String[] args)
    {

        // Initialize the client node
        ClientNodeSetup node_setup = new ClientNodeSetup();
        try
        {
            // Register service type
            RobotRaconteurNode.s().registerServiceType(
                new experimental.reynard_the_robot.experimental__reynard_the_robotFactory());

            // Connect to the Reynard service using a URL
            experimental.reynard_the_robot.Reynard c =
                (experimental.reynard_the_robot.Reynard)RobotRaconteurNode.s().connectService(
                    "rr+tcp://localhost:29200?service=reynard");

            // Connect a callback function to listen for new messages
            // c.new_message += (msg) => { Console.WriteLine(msg); };

            // Read the current state using a wire "peek". Can also "connect" to receive streaming updates.
            experimental.reynard_the_robot.ReynardState state = c.get_state().peekInValue();

            // Teleport the robot
            c.teleport(0.1, -0.2);

            // Drive the robot with no timeout
            c.drive_robot(0.5, -0.2, -1, false);

            // Wait for one second
            RobotRaconteurNode.s().sleep(1000);

            // Stop the robot
            c.drive_robot(0, 0, -1, false);

            // Set the arm position
            c.setf_arm_position(100.0 * (Math.PI / 180), -30 * (Math.PI / 180), -70 * (Math.PI / 180));

            // Drive the arm using timeout and wait
            c.drive_arm(10.0 * (Math.PI / 180), -30 * (Math.PI / 180), -15 * (Math.PI / 180), 1.5, true);

            //  Set the color to red
            c.set_color(new double[] {1.0, 0.0, 0.0});

            // Read the color
            double[] color_in = c.get_color();

            RobotRaconteurNode.s().sleep(1000);

            // Reset the color
            c.set_color(new double[] {0.929, 0.49, 0.192});

            // Say hello
            c.say("Hello, World From Java!");
        }
        finally
        {
            // Shutdown the client node
            node_setup.finalize();
        }
    }
}
