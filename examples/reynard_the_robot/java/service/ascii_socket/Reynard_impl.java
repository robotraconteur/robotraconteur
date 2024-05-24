
import experimental.reynard_the_robot.*;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.net.URL;
import java.nio.charset.StandardCharsets;

import com.robotraconteur.*;

public class Reynard_impl extends Reynard_default_impl implements Reynard
{
    private final String host = "localhost";
    private final int port = 29202;

    private InetSocketAddress socketEp;
    private Timer stateTimer;

    public Reynard_impl()
    {}

    public String[] _communicate(String textRequest, String expectedResponseOp) throws IOException
    {

        if (socketEp == null)
        {
            socketEp = new InetSocketAddress(InetAddress.getByName(host), port);
        }

        try (Socket client = new Socket())
        {
            client.connect(socketEp);
            try (BufferedReader reader = new BufferedReader(new InputStreamReader(client.getInputStream()));
                 PrintWriter writer = new PrintWriter(client.getOutputStream(), true))
            {

                writer.println(textRequest);

                String response = reader.readLine();
                if (response == null)
                {
                    throw new IOException("Connection closed");
                }

                if (response.startsWith("ERROR"))
                {
                    throw new IOException("Error from robot: " + response);
                }

                String[] responseParts = response.split(" ");
                if (responseParts.length < 1)
                {
                    throw new IOException("Invalid response");
                }

                if (expectedResponseOp.equals("OK"))
                {
                    if (!responseParts[0].equals("OK"))
                    {
                        throw new IOException("Unexpected response from robot: " + response);
                    }
                    return new String[] {};
                }

                if (!responseParts[0].equals(expectedResponseOp))
                {
                    throw new IOException("Invalid response from robot: " + response);
                }

                return Arrays.copyOfRange(responseParts, 1, responseParts.length);
            }
        }
    }

    @Override public double[] get_robot_position()
    {
        try
        {
            String[] socket_res = _communicate("STATE", "STATE");
            double[] ret = new double[2];
            ret[0] = Double.parseDouble(socket_res[0]) * 1e-3;
            ret[1] = Double.parseDouble(socket_res[1]) * 1e-3;
            return ret;
        }
        catch (IOException e)
        {
            throw new OperationFailedException("Could not get robot position: " + e.getMessage());
        }
    }

    @Override public void teleport(double x, double y)
    {
        try
        {
            String req = String.format("TELEPORT %f %f", x * 1e3, y * 1e3);
            _communicate(req, "OK");
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    @Override public double[] get_color()
    {
        try
        {
            String[] socket_res = _communicate("COLORGET", "COLOR");
            double[] ret = new double[3];
            ret[0] = Double.parseDouble(socket_res[0]);
            ret[1] = Double.parseDouble(socket_res[1]);
            ret[2] = Double.parseDouble(socket_res[2]);
            return ret;
        }
        catch (IOException e)
        {
            throw new OperationFailedException("Could not get color: " + e.getMessage());
        }
    }

    @Override public void set_color(double[] color)
    {

        try
        {
            String req = String.format("COLORSET %f %f %f", color[0], color[1], color[2]);
            _communicate(req, "OK");
        }
        catch (Exception e)
        {
            throw new OperationFailedException("Could not set color: " + e.getMessage());
        }
    }

    @Override public void say(String message)
    {
        try
        {
            String req = String.format("SAY \"%s\"", message);
            _communicate(req, "OK");
        }
        catch (Exception e)
        {
            throw new OperationFailedException("Could not say message: " + e.getMessage());
        }
    }

    @Override public double[] getf_arm_position()
    {
        try
        {
            String[] socket_res = _communicate("STATE", "STATE");
            double[] ret = new double[3];
            ret[0] = Double.parseDouble(socket_res[2]) * (Math.PI / 180.0);
            ret[1] = Double.parseDouble(socket_res[3]) * (Math.PI / 180.0);
            ret[2] = Double.parseDouble(socket_res[4]) * (Math.PI / 180.0);
            return ret;
        }
        catch (IOException e)
        {
            throw new OperationFailedException("Could not get arm position: " + e.getMessage());
        }
    }

    @Override public void setf_arm_position(double q1, double q2, double q3)
    {
        try
        {
            String req = String.format("SETARM %f %f %f", q1 * (180.0 / Math.PI), q2 * (180.0 / Math.PI),
                                       q3 * (180.0 / Math.PI));
            _communicate(req, "OK");
        }
        catch (Exception e)
        {
            throw new OperationFailedException("Could not set arm position: " + e.getMessage());
        }
    }

    @Override public void drive_robot(double vel_x, double vel_y, double timeout, boolean wait)
    {
        try
        {
            String req = String.format("DRIVE %f %f %f %d", vel_x * 1e3, vel_y * 1e3, timeout, wait ? 1 : 0);
            _communicate(req, "OK");
        }
        catch (Exception e)
        {
            throw new OperationFailedException("Could not drive robot: " + e.getMessage());
        }
    }

    public void drive_arm(double q1, double q2, double q3, double timeout, boolean wait)
    {
        try
        {
            String req = String.format("DRIVEARM %f %f %f %f %d", q1 * (180.0 / Math.PI), q2 * (180.0 / Math.PI),
                                       q3 * (180.0 / Math.PI), timeout, wait ? 1 : 0);
            _communicate(req, "OK");
        }
        catch (Exception e)
        {
            throw new OperationFailedException("Could not drive arm: " + e.getMessage());
        }
    }

    class _timer_cb implements Action1<TimerEvent>
    {
        public void action(TimerEvent ev)
        {
            _timer_cb_fn(ev);
        }
    }

    Timer _state_timer;

    public void _start()
    {
        _timer_cb cb = new _timer_cb();
        _state_timer = RobotRaconteurNode.s().createTimer(250, cb);
        _state_timer.start();
    }

    public void _stop()
    {
        if (_state_timer != null)
        {
            _state_timer.tryStop();
        }
        _state_timer = null;
    }

    void _timer_cb_fn(TimerEvent ev)
    {
        try
        {
            String[] socket_res = _communicate("STATE", "STATE");
            ReynardState state = new ReynardState();
            state.time = Double.parseDouble(socket_res[0]);
            state.robot_position = new double[2];
            state.robot_position[0] = Double.parseDouble(socket_res[1]) * 1e-3;
            state.robot_position[1] = Double.parseDouble(socket_res[2]) * 1e-3;
            state.arm_position = new double[3];
            state.arm_position[0] = Double.parseDouble(socket_res[3]) * (Math.PI / 180.0);
            state.arm_position[1] = Double.parseDouble(socket_res[4]) * (Math.PI / 180.0);
            state.arm_position[2] = Double.parseDouble(socket_res[5]) * (Math.PI / 180.0);
            state.robot_velocity = new double[2];
            state.arm_velocity = new double[3];

            if (rrvar_state != null)
            {
                rrvar_state.setOutValue(state);
            }
        }
        catch (Exception e)
        {
            RobotRaconteurNode.s().logMessage(LogLevel.LogLevel_Warning, "Could not get state: " + e.getMessage());
        }
    }
}
