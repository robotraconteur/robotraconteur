using System;
using RobotRaconteur;
using experimental.reynard_the_robot;
using RestSharp;
using Newtonsoft.Json;
using System.Collections.Generic;
using DrekarLaunchProcess;

class Reynard_impl : Reynard_default_impl, IDisposable
{

    string _base_url;
    Timer _state_timer;

    public Reynard_impl(string base_url)
    {
        _base_url = base_url;
    }

    public dynamic _get_json(string path)
    {
        using (var client = new RestClient(_base_url))
        {
            var request = new RestRequest(path, Method.Get);
            var response = client.Execute(request);
            if (!response.IsSuccessful)
            {
                throw new OperationFailedException("HTTP request failed: " + response.Content);
            }
            var content = response.Content;
            return JsonConvert.DeserializeObject(content);
        }
    }

    public dynamic _post_json(string path, object args)
    {
        using (var client = new RestClient(_base_url))
        {
            var request = new RestRequest(path, Method.Post);
            request.AddHeader("Content-Type", "application/json");
            request.AddJsonBody(args);
            var response = client.Execute(request);
            if (!response.IsSuccessful)
            {
                throw new OperationFailedException("HTTP request failed: " + response.Content);
            }
            var content = response.Content;
            if (content != null)
            {
                return JsonConvert.DeserializeObject(content);
            }
            else
            {
                return null;
            }
        }
    }

    public override double[] robot_position
    {
        get {
            var json = _get_json("/api/state");
            var ret = new double[2];
            ret[0] = json.x * 1e-3;
            ret[1] = json.y * 1e-3;
            return ret;
        }
        set {
            throw new NotImplementedException();
        }
    }
    public override double[] color
    {
        get {
            var json = _get_json("/api/color");
            return new double[] { json.r, json.g, json.b };
        }
        set {
            var args = new Dictionary<string, double>() { { "r", value[0] }, { "g", value[1] }, { "b", value[2] } };
            _post_json("/api/color", args);
        }
    }
    public override void teleport(double x, double y)
    {
        var args = new Dictionary<string, double>() { { "x", x * 1e3 }, { "y", y * 1e3 } };
        _post_json("/api/teleport", args);
    }
    public override void setf_arm_position(double q1, double q2, double q3)
    {
        var args = new Dictionary<string, double>() { { "q1", q1 * (180.0 / Math.PI) },
                                                      { "q2", q2 * (180.0 / Math.PI) },
                                                      { "q3", q3 * (180.0 / Math.PI) } };
        _post_json("/api/arm", args);
    }
    public override double[] getf_arm_position()
    {
        var json = _get_json("/api/arm");
        return new double[] { json.q1 * (Math.PI / 180.0), json.q2 * (Math.PI / 180.0), json.q3 * (Math.PI / 180.0) };
    }
    public override void drive_robot(double vel_x, double vel_y, double timeout, bool wait)
    {
        var args = new Dictionary<string, double>() {
            { "vel_x", vel_x * 1e3 }, { "vel_y", vel_y * 1e3 }, { "timeout", timeout }, { "wait", wait ? 1 : 0 }
        };
        _post_json("/api/drive_robot", args);
    }
    public override void drive_arm(double q1, double q2, double q3, double timeout, bool wait)
    {
        var args = new Dictionary<string, double>() { { "q1", q1 * (180.0 / Math.PI) },
                                                      { "q2", q2 * (180.0 / Math.PI) },
                                                      { "q3", q3 * (180.0 / Math.PI) },
                                                      { "timeout", timeout },
                                                      { "wait", wait ? 1 : 0 } };
        _post_json("/api/drive_arm", args);
    }
    public override void say(string message)
    {
        var args = new Dictionary<string, string>() { { "message", message } };
        _post_json("api/say", args);
    }

    private void _timer_cb(TimerEvent evt)
    {
        try
        {
            var json = _get_json("/api/state");

            var state = new ReynardState() {
                time = json.time, robot_position = new double[] { json.x * 1e-3, json.y * 1e-3 },
                arm_position = new double[] { json.q1 * (Math.PI / 180.0), json.q2 * (Math.PI / 180.0),
                                              json.q3 * (Math.PI / 180.0) },
                robot_velocity = new double[] { json.vel_x * 1e-3, json.vel_y * 1e-3 },
                arm_velocity = new double[] { json.vel_q1 * (Math.PI / 180.0), json.vel_q2 * (Math.PI / 180.0),
                                              json.vel_q3 * (Math.PI / 180.0) }
            };

            if (rrvar_state != null)
            {
                rrvar_state.OutValue = state;
            }

            var message_json = _get_json("/api/messages");
            foreach (string message in message_json)
            {
                rrfire_new_message(message);
            }
        }
        catch (Exception e)
        {
            RobotRaconteurNode.s.LogMessage(LogLevel.LogLevel_Warning, "Error updating state: " + e.Message);
        }
    }

    public void _start()
    {
        _state_timer = RobotRaconteurNode.s.CreateTimer(250, _timer_cb);
        _state_timer.Start();
    }

    public void Dispose()
    {
        _state_timer?.Stop();
        _state_timer = null;
    }
}

class Program
{
    static int Main(string[] args)
    {
        var reynard = new Reynard_impl("http://localhost:29201");
        using (reynard) using (var node_setup =
                                   new ServerNodeSetup("experimental.reynard_the_robot_csharp_rest", 59201, args))
        {
            var ctx = RobotRaconteurNode.s.RegisterService("reynard", "experimental.reynard_the_robot", reynard);

            reynard._start();

            Console.WriteLine("Reynard the Robot C# Service Started");
            Console.WriteLine();
            Console.WriteLine("Candidate connection urls:");
            ctx.PrintCandidateConnectionURLs();
            Console.WriteLine();
            Console.WriteLine("Press Ctrl-C to quit");

            using (var wait_exit = new CWaitForExit())
            {
                wait_exit.WaitForExit();
            }
        }

        return 0;
    }
}
