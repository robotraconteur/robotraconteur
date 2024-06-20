import com.robotraconteur.*;
import experimental.create3.*;

// This program provides a simple client to the iRobotCreate service
// that connects, drives a bit, and then disconnects

public class iRobotCreateClient
{

    /**
     * @param args
     */
    public static void main(String[] args)
    {

        String url = "rr+tcp://localhost:22354?service=create";

        if (args.length > 0)
        {
            url = args[0];
        }

        // Use ClientNodeSetup to initialize node
        ClientNodeSetup setup = new ClientNodeSetup();

        try
        {
            // Register the Create_interface service type
            RobotRaconteurNode.s().registerServiceType(new experimental__create3Factory());

            // Connect to the service
            Create c =
                (Create)RobotRaconteurNode.s().connectService(url, null, null, null, "experimental.create3.Create");

            // Set an event listener for the "Bump" event
            c.addbumpListener(new Bump());

            // Connect the "packets" wire and add a value changed event listener
            Wire<CreateState>.WireConnection wire = c.get_create_state().connect();
            wire.addWireValueListener(new wire_changed());

            // Set a function to be used by the callback.  This function will be called
            // when the service calls a callback with the endpoint corresponding to this
            // client
            c.get_play_callback().setFunction(new play_callback());
            c.claim_play_callback();

            // Drive a bit
            c.drive(0.150, 5.0);
            try
            {
                Thread.sleep(1000);
            }
            catch (Exception e)
            {}
            c.drive(0.0, 5.0);

            try
            {
                Thread.sleep(5000);
            }
            catch (Exception e)
            {}

            // Close the wire and stop streaming data
            try
            {
                wire.close();
            }
            catch (Exception e)
            {}
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

    // Function to handle the "Bump" event
    static class Bump implements Action
    {

        @Override public void action()
        {
            System.out.println("Bump!");
        }
    }

    // Function to handle when the wire value changes
    static class wire_changed implements Action3<Wire<CreateState>.WireConnection, CreateState, TimeSpec>
    {

        @Override public void action(Wire<CreateState>.WireConnection arg0, CreateState arg1, TimeSpec arg2)
        {
            CreateState value2 = arg0.getInValue();

            // Uncomment this line to print out the packets as they are received
            // System.out.println(value2.time);
        }
    }

    // Function that is called by the service as a callback.  This returns
    // a few notes to play.
    static class play_callback implements Func2<Double, Double, UnsignedBytes>
    {

        @Override public UnsignedBytes func(Double arg0, Double arg1)
        {
            byte[] notes = {69, 16, 60, 16, 69, 16};
            return new UnsignedBytes(notes);
        }
    }
}
