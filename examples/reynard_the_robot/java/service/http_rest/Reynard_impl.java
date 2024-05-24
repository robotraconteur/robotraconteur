
import experimental.reynard_the_robot.*;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.util.HashMap;
import java.util.Map;
import java.net.URL;
import java.nio.charset.StandardCharsets;

import com.robotraconteur.*;

public class Reynard_impl extends Reynard_default_impl implements Reynard
{
    private final Gson gson = new Gson();
    private final String baseUrl = "http://localhost:29201"; // replace with your base URL

    public String _postJson(String path, Map<String, Object> args) throws IOException
    {
        URL url = new URL(baseUrl + path);
        HttpURLConnection conn = (HttpURLConnection)url.openConnection();
        conn.setRequestMethod("POST");
        conn.setRequestProperty("Content-Type", "application/json; charset=utf-8");
        conn.setDoOutput(true);
        try (OutputStream os = conn.getOutputStream())
        {
            byte[] input = gson.toJson(args).getBytes(StandardCharsets.UTF_8);
            os.write(input, 0, input.length);
        }
        int responsecode = conn.getResponseCode();
        if (responsecode != 200)
        {
            throw new RuntimeException("HttpResponseCode: " + responsecode);
        }
        else
        {
            BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            String inputLine;
            StringBuilder content = new StringBuilder();
            while ((inputLine = in.readLine()) != null)
            {
                content.append(inputLine);
            }
            in.close();
            return content.toString();
        }
    }

    public String _getJson(String path) throws IOException
    {
        URL url = new URL(baseUrl + path);
        HttpURLConnection conn = (HttpURLConnection)url.openConnection();
        conn.setRequestMethod("GET");
        conn.connect();
        int responseCode = conn.getResponseCode();
        if (responseCode != HttpURLConnection.HTTP_OK)
        {
            throw new IOException("GET request not successful. Response code: " + responseCode);
        }
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(conn.getInputStream())))
        {
            StringBuilder content = new StringBuilder();
            String inputLine;
            while ((inputLine = reader.readLine()) != null)
            {
                content.append(inputLine);
            }
            return content.toString();
        }
    }

    public Map<String, Object> _getJsonMap(String path) throws IOException
    {
        String json = _getJson(path);
        return gson.fromJson(json, Map.class);
    }

    @Override public double[] get_robot_position()
    {
        try
        {
            Map<String, Object> json = _getJsonMap("/api/state");
            double[] ret = new double[2];
            ret[0] = ((Number)json.get("x")).doubleValue() * 1e-3;
            ret[1] = ((Number)json.get("y")).doubleValue() * 1e-3;
            return ret;
        }
        catch (IOException e)
        {
            throw new OperationFailedException("Could not get robot position: " + e.getMessage());
        }
    }

    @Override public void teleport(double x, double y)
    {
        Map<String, Object> args = new HashMap<>();
        args.put("x", x * 1e3);
        args.put("y", y * 1e3);
        try
        {
            _postJson("/api/teleport", args);
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
            Map<String, Object> json = _getJsonMap("/api/color");
            double[] ret = new double[3];
            ret[0] = ((Number)json.get("r")).doubleValue();
            ret[1] = ((Number)json.get("g")).doubleValue();
            ret[2] = ((Number)json.get("b")).doubleValue();
            return ret;
        }
        catch (IOException e)
        {
            throw new OperationFailedException("Could not get color: " + e.getMessage());
        }
    }

    @Override public void set_color(double[] color)
    {
        Map<String, Object> args = new HashMap<>();
        args.put("r", color[0]);
        args.put("g", color[1]);
        args.put("b", color[2]);
        try
        {
            _postJson("/api/color", args);
        }
        catch (Exception e)
        {
            throw new OperationFailedException("Could not set color: " + e.getMessage());
        }
    }

    @Override public void say(String message)
    {
        Map<String, Object> args = new HashMap<>();
        args.put("message", message);
        try
        {
            _postJson("/api/say", args);
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
            Map<String, Object> json = _getJsonMap("/api/arm");
            double[] ret = new double[3];
            ret[0] = ((Number)json.get("q1")).doubleValue() * (Math.PI / 180.0);
            ret[1] = ((Number)json.get("q2")).doubleValue() * (Math.PI / 180.0);
            ret[2] = ((Number)json.get("q3")).doubleValue() * (Math.PI / 180.0);
            return ret;
        }
        catch (IOException e)
        {
            throw new OperationFailedException("Could not get arm position: " + e.getMessage());
        }
    }

    @Override public void setf_arm_position(double q1, double q2, double q3)
    {
        Map<String, Object> args = new HashMap<>();
        args.put("q1", q1 * (180.0 / Math.PI));
        args.put("q2", q2 * (180.0 / Math.PI));
        args.put("q3", q3 * (180.0 / Math.PI));
        try
        {
            _postJson("/api/arm", args);
        }
        catch (Exception e)
        {
            throw new OperationFailedException("Could not set arm position: " + e.getMessage());
        }
    }

    @Override public void drive_robot(double vel_x, double vel_y, double timeout, boolean wait)
    {
        Map<String, Object> args = new HashMap<>();
        args.put("vel_x", vel_x * 1e3);
        args.put("vel_y", vel_y * 1e3);
        args.put("timeout", timeout);
        args.put("wait", wait);
        try
        {
            _postJson("/api/drive_robot", args);
        }
        catch (Exception e)
        {
            throw new OperationFailedException("Could not drive robot: " + e.getMessage());
        }
    }

    public void drive_arm(double q1, double q2, double q3, double timeout, boolean wait)
    {
        Map<String, Object> args = new HashMap<>();
        args.put("q1", q1 * (180.0 / Math.PI));
        args.put("q2", q2 * (180.0 / Math.PI));
        args.put("q3", q3 * (180.0 / Math.PI));
        args.put("timeout", timeout);
        args.put("wait", wait);
        try
        {
            _postJson("/api/drive_arm", args);
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
            Map<String, Object> json = _getJsonMap("/api/state");
            ReynardState state = new ReynardState();
            state.time = ((Number)json.get("time")).doubleValue();
            state.robot_position = new double[2];
            state.robot_position[0] = ((Number)json.get("x")).doubleValue() * 1e-3;
            state.robot_position[1] = ((Number)json.get("y")).doubleValue() * 1e-3;
            state.arm_position = new double[3];
            state.arm_position[0] = ((Number)json.get("q1")).doubleValue() * (Math.PI / 180.0);
            state.arm_position[1] = ((Number)json.get("q2")).doubleValue() * (Math.PI / 180.0);
            state.arm_position[2] = ((Number)json.get("q3")).doubleValue() * (Math.PI / 180.0);
            state.robot_velocity = new double[2];
            state.robot_velocity[0] = ((Number)json.get("vel_x")).doubleValue() * 1e-3;
            state.robot_velocity[1] = ((Number)json.get("vel_y")).doubleValue() * 1e-3;
            state.arm_velocity = new double[3];
            state.arm_velocity[0] = ((Number)json.get("vel_q1")).doubleValue() * (Math.PI / 180.0);
            state.arm_velocity[1] = ((Number)json.get("vel_q2")).doubleValue() * (Math.PI / 180.0);
            state.arm_velocity[2] = ((Number)json.get("vel_q3")).doubleValue() * (Math.PI / 180.0);

            if (rrvar_state != null)
            {
                rrvar_state.setOutValue(state);
            }

            String message_json_str = _getJson("/api/messages");
            String[] message_json = gson.fromJson(message_json_str, String[].class);
            for (int i = 0; i < message_json.length; i++)
            {
                String message_str = message_json[i];
                synchronized (rrvar_new_message)
                {
                    for (Action1<String> e : rrvar_new_message)
                    {
                        e.action(message_str);
                    }
                }
            }
        }
        catch (Exception e)
        {
            RobotRaconteurNode.s().logMessage(LogLevel.LogLevel_Warning, "Could not get state: " + e.getMessage());
        }
    }
}
