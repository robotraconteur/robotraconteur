using System;
using RobotRaconteur;
using experimental.reynard_the_robot;
using System.Collections.Generic;
using DrekarLaunchProcess;
using System.Linq;
using System.Net;
using System.Net.Sockets;

class Reynard_impl : Reynard_default_impl, IDisposable
{

    IPEndPoint _socket_ep;
    Timer _state_timer;

    public Reynard_impl(string host, int port)
    {
        var host_entry = Dns.GetHostEntry(host, AddressFamily.InterNetwork);
        _socket_ep = new IPEndPoint(host_entry.AddressList[0], port);
    }

    public string[] _communicate(string text_request, string expected_response_op)
    {
        using (var client = new System.Net.Sockets.TcpClient())
        {
            client.Connect(_socket_ep);
            using (var stream = client.GetStream())
            {
                var writer = new System.IO.StreamWriter(stream);
                writer.WriteLine(text_request);
                writer.Flush();

                var reader = new System.IO.StreamReader(stream);
                var response = reader.ReadLine();
                if (response == null)
                {
                    throw new System.IO.IOException("Connection closed");
                }

                if (response.StartsWith("ERROR"))
                {
                    throw new System.IO.IOException("Error from robot: " + response);
                }

                var response_parts = response.Split(' ');
                if (response_parts.Length < 1)
                {
                    throw new System.IO.IOException("Invalid response");
                }

                if (expected_response_op == "OK")
                {
                    if (response_parts[0] != "OK")
                    {
                        throw new System.IO.IOException("Unexpected response from robot: " + response);
                    }
                    return new string[] {};
                }

                if (response_parts[0] != expected_response_op)
                {
                    throw new System.IO.IOException("Invalid response from robot: " + response);
                }

                return response_parts.Skip(1).ToArray();
            }
        }
    }

    public override double[] robot_position
    {
        get {
            var socket_res = _communicate("STATE", "STATE");
            var ret = new double[2];
            ret[0] = double.Parse(socket_res[0]) * 1e-3;
            ret[1] = double.Parse(socket_res[1]) * 1e-3;
            return ret;
        }
        set {
            throw new NotImplementedException();
        }
    }
    public override double[] color
    {
        get {
            var socket_res = _communicate("COLORGET", "COLOR");
            return new double[] { double.Parse(socket_res[0]), double.Parse(socket_res[1]),
                                  double.Parse(socket_res[2]) };
        }
        set {
            string text_request =
                "COLORSET " + value[0].ToString() + " " + value[1].ToString() + " " + value[2].ToString();
            _communicate(text_request, "OK");
        }
    }
    public override void teleport(double x, double y)
    {
        string text_request = "TELEPORT " + (x * 1e3).ToString() + " " + (y * 1e3).ToString();
        _communicate(text_request, "OK");
    }
    public override void setf_arm_position(double q1, double q2, double q3)
    {
        string text_request = "SETARM " + (q1 * (180.0 / Math.PI)).ToString() + " " +
                              (q2 * (180.0 / Math.PI)).ToString() + " " + (q3 * (180.0 / Math.PI)).ToString();
        _communicate(text_request, "OK");
    }
    public override double[] getf_arm_position()
    {
        var socket_res = _communicate("STATE", "STATE");

        return new double[] { double.Parse(socket_res[2]) * (Math.PI / 180.0),
                              double.Parse(socket_res[3]) * (Math.PI / 180.0),
                              double.Parse(socket_res[4]) * (Math.PI / 180.0) };
    }
    public override void drive_robot(double vel_x, double vel_y, double timeout, bool wait)
    {
        string text_request = "DRIVE " + (vel_x * 1e3).ToString() + " " + (vel_y * 1e3).ToString() + " " +
                              timeout.ToString() + " " + (wait ? "1" : "0");
        _communicate(text_request, "OK");
    }
    public override void drive_arm(double q1, double q2, double q3, double timeout, bool wait)
    {
        string text_request = "DRIVEARM " + (q1 * (180.0 / Math.PI)).ToString() + " " +
                              (q2 * (180.0 / Math.PI)).ToString() + " " + (q3 * (180.0 / Math.PI)).ToString() + " " +
                              timeout.ToString() + " " + (wait ? "1" : "0");
        _communicate(text_request, "OK");
    }
    public override void say(string message)
    {
        string text_request = "SAY \"" + message + "\"";
        _communicate(text_request, "OK");
    }

    private void _timer_cb(TimerEvent evt)
    {
        try
        {
            var socket_res = _communicate("STATE", "STATE");

            var state =
                new ReynardState() { time = double.Parse(socket_res[0]),
                                     robot_position = new double[] { double.Parse(socket_res[1]) * 1e-3,
                                                                     double.Parse(socket_res[2]) * 1e-3 },
                                     arm_position = new double[] { double.Parse(socket_res[3]) * (Math.PI / 180.0),
                                                                   double.Parse(socket_res[4]) * (Math.PI / 180.0),
                                                                   double.Parse(socket_res[5]) * (Math.PI / 180.0) },
                                     robot_velocity = new double[] {}, arm_velocity = new double[] {} };

            if (rrvar_state != null)
            {
                rrvar_state.OutValue = state;
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
        var reynard = new Reynard_impl("localhost", 29202);
        using (reynard) using (var node_setup =
                                   new ServerNodeSetup("experimental.reynard_the_robot_csharp_socket", 59201, args))
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
