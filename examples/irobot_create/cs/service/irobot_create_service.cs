using System;
using RobotRaconteur;
using RestSharp;
using Newtonsoft.Json;
using System.Collections.Generic;
using System.IO;
using System.IO.Ports;
using DrekarLaunchProcess;
using experimental.create3;
using System.ComponentModel;
using System.Reflection.Metadata.Ecma335;

class Create_impl : Create_default_impl, IDisposable
{

    const byte CREATE_OI_OP_START = 128;
    const byte CREATE_OI_OP_FULL = 132;
    const byte CREATE_OI_OP_DRIVE = 137;
    const byte CREATE_OI_OP_LEDS = 139;
    const byte CREATE_OI_OP_SONG = 140;
    const byte CREATE_OI_OP_PLAY_SONG = 141;
    const byte CREATE_OI_OP_DRIVE_DIRECT = 145;
    const byte CREATE_OI_OP_STREAM = 148;
    const byte CREATE_OI_OP_STREAM_RESUME = 150;

    const byte CREATE_OI_FLAGS_BUMP_RIGHT = 0x1;
    const byte CREATE_OI_FLAGS_BUMP_LEFT = 0x2;
    const byte CREATE_OI_FLAGS_WHEEL_DROP_RIGHT = 0x4;
    const byte CREATE_OI_FLAGS_WHEEL_DROP_LEFT = 0x8;
    const byte CREATE_OI_FLAGS_WHEEL_DROP_CASTER = 0x10;

    const byte CREATE_OI_FLAGS_PLAY_BUTTON = 0x1;
    const byte CREATE_OI_FLAGS_ADVANCE_BUTTON = 0x4;

    const byte CREATE_OI_SET_LED_PLAY = 2;
    const byte CREATE_OI_SET_LED_ADVANCE = 8;

    SerialPort port;

    public Create_impl()
    {}

    public void Start(string portname)
    {
        lock (this)
        {
            port = new SerialPort(portname, 57600, Parity.None, 8, StopBits.One);
            port.Open();

            port.DataReceived += SerialDataReceived;

            byte[] command = {
                CREATE_OI_OP_START, CREATE_OI_OP_FULL, CREATE_OI_OP_STREAM_RESUME, 0, CREATE_OI_OP_LEDS, 0, 80, 255
            };

            port.Write(command, 0, command.Length);

            try
            {
                System.Threading.Thread.Sleep(500);
            }
            catch
            {}
        }

        StartStreaming();
    }

    // Serial event callback

    bool recv_magic_found = false;
    byte recv_len = 0;
    int checksum_error_count = 0;

    private void ReadAll(SerialPort stream, byte[] buffer, int offset, int count)
    {
        while (count > 0)
        {
            int read = stream.Read(buffer, offset, count);
            if (read == 0)
            {
                throw new IOException("Read error");
            }
            offset += read;
            count -= read;
        }
    }

    private void SerialDataReceived(Object sender, SerialDataReceivedEventArgs args)
    {
        if (args.EventType == SerialData.Chars)
        {
            while (port.BytesToRead > 0)
            {
                if (!recv_magic_found)
                {
                    if (port.BytesToRead < 2)
                    {
                        return;
                    }

                    var recv_buf = new byte[2];
                    ReadAll(port, recv_buf, 0, recv_buf.Length);
                    if (recv_buf[0] != 19)
                    {
                        return;
                    }

                    recv_len = recv_buf[1];
                    recv_magic_found = true;
                }

                if (port.BytesToRead < recv_len + 1)
                {
                    return;
                }

                var packet_buf = new byte[recv_len];
                ReadAll(port, packet_buf, 0, packet_buf.Length);
                var checksum = new byte[1];
                ReadAll(port, checksum, 0, checksum.Length);

                recv_magic_found = false;

                uint checksum_calc = 19u + recv_len;

                foreach (var b in packet_buf)
                {
                    checksum_calc += b;
                }

                checksum_calc += checksum[0];

                checksum_calc &= 0xff;

                if (checksum_calc != 0)
                {
                    checksum_error_count++;
                    if (checksum_error_count > 20)
                    {
                        Console.WriteLine("Checksum error");
                        checksum_error_count = 0;
                    }
                    continue;
                }

                var state = ParseSensorPacket(packet_buf);

                lock (this)
                {
                    if (rrvar_create_state != null)
                    {
                        rrvar_create_state.OutValue = state;
                    }

                    if ((state.create_state_flags & (uint)(CreateStateFlags.bump_right | CreateStateFlags.bump_left)) !=
                        0)
                    {
                        if (!bump_fired)
                        {
                            rrfire_bump();
                            bump_fired = true;
                        }
                    }
                    else
                    {
                        bump_fired = false;
                    }

                    distance_traveled = state.distance_traveled;
                    angle_traveled = state.angle_traveled;
                    bumpers = (byte)(state.create_state_flags &
                                     (uint)(CreateStateFlags.bump_right | CreateStateFlags.bump_left));

                    if ((state.create_state_flags & (uint)(CreateStateFlags.play_button)) != 0)
                    {
                        if (!play_pressed)
                        {
                            var t = new System.Threading.Thread(() => Play());
                            t.Start();
                            play_pressed = true;
                        }
                    }
                    else
                    {
                        play_pressed = false;
                    }
                }
            }
        }
    }

    bool bump_fired = false;
    bool play_pressed = false;

    void Play()
    {
        if (play_client == 0)
        {
            return;
        }

        if (rrvar_play_callback == null)
        {
            return;
        }

        byte[] notes = play_callback.GetClientFunction(play_client)(distance_traveled, angle_traveled);

        byte[] command = new byte[notes.Length + 5];
        command[0] = CREATE_OI_OP_SONG;
        command[1] = 0;
        command[2] = (byte)(notes.Length / 2);
        Array.Copy(notes, 0, command, 3, notes.Length);
        command[3 + notes.Length] = CREATE_OI_OP_PLAY_SONG;
        command[4 + notes.Length] = 0;

        lock (this)
        {
            port.Write(command, 0, command.Length);
        }
    }

    short ReadShortBE(Stream stream)
    {
        var b = new byte[2];
        stream.Read(b, 1, 1);
        stream.Read(b, 0, 1);
        return BitConverter.ToInt16(b);
    }

    ushort ReadUShortBE(Stream stream)
    {
        var b = new byte[2];
        stream.Read(b, 1, 1);
        stream.Read(b, 0, 1);
        return BitConverter.ToUInt16(b);
    }

    byte ReadByte(Stream stream)
    {
        var b = new byte[1];
        stream.Read(b, 0, 1);
        return b[0];
    }

    CreateState ParseSensorPacket(byte[] data)
    {
        var stream = new MemoryStream(data);
        var ret = new CreateState();

        ReadByte(stream);

        byte bump_flags_b = ReadByte(stream);
        byte wall_b = ReadByte(stream);
        byte cliff_left_b = ReadByte(stream);
        byte cliff_front_left_b = ReadByte(stream);
        byte cliff_front_right_b = ReadByte(stream);
        byte cliff_right_b = ReadByte(stream);
        byte virtual_wall_b = ReadByte(stream);
        for (int i = 0; i < 4; i++)
        {
            ReadByte(stream);
        }
        byte buttons_b = ReadByte(stream);
        short distance_h = ReadShortBE(stream);
        short angle_h = ReadShortBE(stream);

        for (int i = 0; i < 6; i++)
        {
            ReadByte(stream);
        }

        ushort charge_H = ReadUShortBE(stream);
        ushort capacity_H = ReadUShortBE(stream);

        for (int i = 0; i < 18; i++)
        {
            ReadByte(stream);
        }

        short velocity_h = ReadShortBE(stream);
        short radius_h = ReadShortBE(stream);
        short velocity_right_h = ReadShortBE(stream);
        short velocity_left_h = ReadShortBE(stream);

        uint state_flags = 0;
        if ((bump_flags_b & CREATE_OI_FLAGS_BUMP_RIGHT) != 0)
        {
            state_flags |= (uint)CreateStateFlags.bump_right;
        }
        if ((bump_flags_b & CREATE_OI_FLAGS_BUMP_LEFT) != 0)
        {
            state_flags |= (uint)CreateStateFlags.bump_left;
        }
        if ((bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_RIGHT) != 0)
        {
            state_flags |= (uint)CreateStateFlags.wheel_drop_right;
        }
        if ((bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_LEFT) != 0)
        {
            state_flags |= (uint)CreateStateFlags.wheel_drop_left;
        }
        if ((bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_CASTER) != 0)
        {
            state_flags |= (uint)CreateStateFlags.wheel_drop_caster;
        }

        if (wall_b != 0)
        {
            state_flags |= (uint)CreateStateFlags.wall_sensor;
        }

        if (cliff_left_b != 0)
        {
            state_flags |= (uint)CreateStateFlags.cliff_left;
        }
        if (cliff_front_left_b != 0)
        {
            state_flags |= (uint)CreateStateFlags.cliff_front_left;
        }
        if (cliff_front_right_b != 0)
        {
            state_flags |= (uint)CreateStateFlags.cliff_front_right;
        }
        if (cliff_right_b != 0)
        {
            state_flags |= (uint)CreateStateFlags.cliff_right;
        }

        if (virtual_wall_b != 0)
        {
            state_flags |= (uint)CreateStateFlags.virtual_wall;
        }

        if ((buttons_b & CREATE_OI_FLAGS_PLAY_BUTTON) != 0)
        {
            state_flags |= (uint)CreateStateFlags.play_button;
        }

        if ((buttons_b & CREATE_OI_FLAGS_ADVANCE_BUTTON) != 0)
        {
            state_flags |= (uint)CreateStateFlags.advance_button;
        }

        ret.create_state_flags = state_flags;

        TimeSpec time = RobotRaconteurNode.s.NowTimeSpec;

        ret.time = Convert.ToDouble(time.seconds) + (Convert.ToDouble(time.nanoseconds) / 1e9);
        ret.velocity = Convert.ToDouble(velocity_h) * 1e-3;
        ret.radius = Convert.ToDouble(radius_h) * 1e-3;
        ret.right_wheel_velocity = Convert.ToDouble(velocity_right_h) * 1e-3;
        ret.left_wheel_velocity = Convert.ToDouble(velocity_left_h) * 1e-3;
        ret.distance_traveled = Convert.ToDouble(distance_h) * 1e-3;
        ret.angle_traveled = Convert.ToDouble(angle_h) * (Math.PI / 180.0);
        ret.battery_charge = Convert.ToDouble(charge_H);
        ret.battery_capacity = Convert.ToDouble(capacity_H);

        return ret;
    }

    private bool streaming = false;
    private void StartStreaming()
    {
        lock (this)
        {

            byte[] command = { CREATE_OI_OP_STREAM, 1, 6 };
            port.Write(command, 0, command.Length);
            streaming = true;
        }
    }

    private void StopStreaming()
    {
        lock (this)
        {
            byte[] command = { CREATE_OI_OP_STREAM_RESUME, 0 };
            port.Write(command, 0, command.Length);
            streaming = false;
        }
    }

    public void Dispose()
    {
        StopStreaming();

        byte[] command = { CREATE_OI_OP_START };
        lock (this)
        {
            port.Write(command, 0, command.Length);
            port.Close();
            port = null;
        }
    }

    public override void drive(double velocity, double radius)
    {
        lock (this)
        {
            byte[] vel = BitConverter.GetBytes((short)(velocity * 1e3));
            byte[] rad = BitConverter.GetBytes((short)(radius * 1e3));

            byte[] command = { CREATE_OI_OP_DRIVE, vel[1], vel[0], rad[1], rad[0] };

            port.Write(command, 0, command.Length);
        }
    }

    public override void drive_direct(double right_wheel_velocity, double left_wheel_velocity)
    {
        lock (this)
        {
            byte[] r = BitConverter.GetBytes((short)(right_wheel_velocity * 1e3));
            byte[] l = BitConverter.GetBytes((short)(right_wheel_velocity * 1e3));

            byte[] command = { CREATE_OI_OP_DRIVE_DIRECT, r[1], r[0], l[1], l[0] };

            port.Write(command, 0, command.Length);
        }
    }

    public override void stop()
    {
        drive(0, 0);
    }

    public override void setf_leds(bool play, bool advance)
    {
        lock (this)
        {
            byte bits = 0;
            if (play)
            {
                bits |= CREATE_OI_SET_LED_PLAY;
            }
            if (advance)
            {
                bits |= CREATE_OI_SET_LED_ADVANCE;
            }

            byte[] command = { CREATE_OI_OP_LEDS, bits, 80, 255 };
        }
    }

    uint play_client;

    public override void claim_play_callback()
    {
        lock (this)
        {
            play_client = ServerEndpoint.GetCurrentEndpoint();
        }
    }
}

class Program
{
    static int Main(string[] args)
    {
        string port = "/dev/ttyUSB0";

        if (args.Length > 0)
        {
            port = args[0];
        }

        var create = new Create_impl();
        create.Start(port);
        using (create) using (var node_setup = new ServerNodeSetup("experimental.create3", 22354, args))
        {
            var ctx = RobotRaconteurNode.s.RegisterService("create", "experimental.create3", create);

            Console.WriteLine("iRobot Create C# Service Started");
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
