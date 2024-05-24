import experimental.reynard_the_robot.*;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import com.robotraconteur.*;

public class reynard_robotraconteur_service_http_rest
{

    public static void main(String[] args)
    {
        ServerNodeSetup node_setup = new ServerNodeSetup("experimental.reynard_the_robot", 59201);
        try
        {
            // Register the service type
            RobotRaconteurNode.s().registerServiceType(new experimental__reynard_the_robotFactory());

            // Create the object
            Reynard_impl reynard = new Reynard_impl();

            // Register the service with Robot Raconteur
            RobotRaconteurNode.s().registerService("reynard", "experimental.reynard_the_robot_http_rest", reynard);

            reynard._start();

            // Wait for the user to shutdown the service
            System.out.println("Press enter to quit");
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
            reader.readLine();

            // Shutdown
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}
