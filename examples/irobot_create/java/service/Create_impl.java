import java.util.*;
import java.nio.*;
import java.time.Instant;
import java.io.*;
import com.robotraconteur.*;

import experimental.create3.*;

import com.fazecast.jSerialComm.*;

public class Create_impl extends Create_default_impl implements Create
{
    final int CREATE_OI_OP_START = 128;
    final int CREATE_OI_OP_FULL = 132;
    final int CREATE_OI_OP_DRIVE = 137;
    final int CREATE_OI_OP_LEDS = 139;
    final int CREATE_OI_OP_SONG = 140;
    final int CREATE_OI_OP_PLAY_SONG = 141;
    final int CREATE_OI_OP_DRIVE_DIRECT = 145;
    final int CREATE_OI_OP_STREAM = 148;
    final int CREATE_OI_OP_STREAM_RESUME = 150;

    final int CREATE_OI_FLAGS_BUMP_RIGHT = 0x1;
    final int CREATE_OI_FLAGS_BUMP_LEFT = 0x2;
    final int CREATE_OI_FLAGS_WHEEL_DROP_RIGHT = 0x4;
    final int CREATE_OI_FLAGS_WHEEL_DROP_LEFT = 0x8;
    final int CREATE_OI_FLAGS_WHEEL_DROP_CASTER = 0x10;

    final int CREATE_OI_FLAGS_PLAY_BUTTON = 0x1;
    final int CREATE_OI_FLAGS_ADVANCE_BUTTON = 0x4;

    final int CREATE_OI_SET_LED_PLAY = 2;
    final int CREATE_OI_SET_LED_ADVANCE = 8;

    private SerialPort port;
    private InputStream port_in;
    private boolean recv_magic_found = false;
    private int recv_len = 0;
    private int checksum_error_count = 0;

    public void start(String portName)
    {
        synchronized (this)
        {
            port = SerialPort.getCommPort(portName);
            port.setComPortParameters(57600, 8, SerialPort.ONE_STOP_BIT, SerialPort.NO_PARITY);
            port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);
            port.openPort();

            port.addDataListener(new SerialPortDataListener() {
                @Override public int getListeningEvents()
                {
                    return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
                }
                @Override public void serialEvent(SerialPortEvent event)
                {
                    if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE)
                        return;

                    serialDataReceived();
                }
            });

            port_in = port.getInputStream();

            byte[] command = {(byte)CREATE_OI_OP_START,
                              (byte)CREATE_OI_OP_FULL,
                              (byte)CREATE_OI_OP_STREAM_RESUME,
                              0,
                              (byte)CREATE_OI_OP_LEDS,
                              0,
                              80,
                              (byte)255};

            port.writeBytes(command, command.length);

            try
            {
                Thread.sleep(500);
            }
            catch (InterruptedException ignored)
            {}

            startStreaming();
        }
    }

    private void readAll(InputStream stream, byte[] buffer, int offset, int count) throws java.io.IOException
    {
        while (count > 0)
        {
            int read = stream.read(buffer, offset, count);
            if (read == -1)
            {
                throw new java.io.IOException("Read error");
            }
            offset += read;
            count -= read;
        }
    }

    private boolean bump_fired = false;
    private boolean play_pressed = false;

    private void serialDataReceived()
    {
        try
        {
            while (port.bytesAvailable() > 0)
            {
                if (!recv_magic_found)
                {
                    if (port.bytesAvailable() < 2)
                    {
                        return;
                    }

                    byte[] recv_buf = new byte[2];
                    readAll(port_in, recv_buf, 0, recv_buf.length);
                    if (recv_buf[0] != 19)
                    {
                        return;
                    }

                    recv_len = recv_buf[1];
                    recv_magic_found = true;
                }

                if (port.bytesAvailable() < recv_len + 1)
                {
                    return;
                }

                byte[] packet_buf = new byte[recv_len];
                readAll(port_in, packet_buf, 0, packet_buf.length);
                byte[] checksum = new byte[1];
                readAll(port_in, checksum, 0, checksum.length);

                recv_magic_found = false;

                int checksum_calc = 19 + recv_len;

                for (byte b : packet_buf)
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
                        System.out.println("Checksum error");
                        checksum_error_count = 0;
                    }
                    continue;
                }

                CreateState state = parseSensorPacket(packet_buf);

                synchronized (this)
                {
                    if (rrvar_create_state != null)
                    {
                        rrvar_create_state.setOutValue(state);
                    }

                    if ((state.create_state_flags.value &
                         (CreateStateFlags.bump_right.getValue() | CreateStateFlags.bump_left.getValue())) != 0)
                    {
                        if (!bump_fired)
                        {
                            for (Action a : rrvar_bump)
                            {
                                a.action();
                            }
                            bump_fired = true;
                        }
                    }
                    else
                    {
                        bump_fired = false;
                    }

                    rrvar_distance_traveled = state.distance_traveled;
                    rrvar_angle_traveled = state.angle_traveled;
                    rrvar_bumpers = new UnsignedByte(
                        (byte)(state.create_state_flags.value &
                               (CreateStateFlags.bump_right.getValue() | CreateStateFlags.bump_left.getValue())));

                    if ((state.create_state_flags.value & CreateStateFlags.play_button.getValue()) != 0)
                    {
                        if (!play_pressed)
                        {
                            Thread t = new Thread(new Runnable() {
                                @Override public void run()
                                {
                                    play();
                                }
                            });
                            t.start();
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
        catch (java.io.IOException e)
        {
            e.printStackTrace();
        }
    }

    short readShortBE(InputStream stream) throws java.io.IOException
    {
        byte[] b = new byte[2];
        stream.read(b, 0, 1);
        stream.read(b, 1, 1);
        return ByteBuffer.wrap(b).order(ByteOrder.BIG_ENDIAN).getShort();
    }

    int readUShortBE(InputStream stream) throws java.io.IOException
    {
        byte[] b = new byte[2];
        stream.read(b, 0, 1);
        stream.read(b, 1, 1);
        return ByteBuffer.wrap(b).order(ByteOrder.BIG_ENDIAN).getShort() & 0xffff;
    }

    byte readByte(InputStream stream) throws java.io.IOException
    {
        byte[] b = new byte[1];
        stream.read(b, 0, 1);
        return b[0];
    }

    CreateState parseSensorPacket(byte[] data) throws java.io.IOException
    {
        InputStream stream = new ByteArrayInputStream(data);
        CreateState ret = new CreateState();

        readByte(stream);

        byte bump_flags_b = readByte(stream);
        byte wall_b = readByte(stream);
        byte cliff_left_b = readByte(stream);
        byte cliff_front_left_b = readByte(stream);
        byte cliff_front_right_b = readByte(stream);
        byte cliff_right_b = readByte(stream);
        byte virtual_wall_b = readByte(stream);
        for (int i = 0; i < 4; i++)
        {
            readByte(stream);
        }
        byte buttons_b = readByte(stream);
        short distance_h = readShortBE(stream);
        short angle_h = readShortBE(stream);

        for (int i = 0; i < 6; i++)
        {
            readByte(stream);
        }

        int charge_H = readUShortBE(stream);
        int capacity_H = readUShortBE(stream);

        for (int i = 0; i < 18; i++)
        {
            readByte(stream);
        }

        short velocity_h = readShortBE(stream);
        short radius_h = readShortBE(stream);
        short velocity_right_h = readShortBE(stream);
        short velocity_left_h = readShortBE(stream);

        int state_flags = 0;
        if ((bump_flags_b & CREATE_OI_FLAGS_BUMP_RIGHT) != 0)
        {
            state_flags |= CreateStateFlags.bump_right.getValue();
        }
        if ((bump_flags_b & CREATE_OI_FLAGS_BUMP_LEFT) != 0)
        {
            state_flags |= CreateStateFlags.bump_left.getValue();
        }
        if ((bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_RIGHT) != 0)
        {
            state_flags |= CreateStateFlags.wheel_drop_right.getValue();
        }
        if ((bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_LEFT) != 0)
        {
            state_flags |= CreateStateFlags.wheel_drop_left.getValue();
        }
        if ((bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_CASTER) != 0)
        {
            state_flags |= CreateStateFlags.wheel_drop_caster.getValue();
        }

        if (wall_b != 0)
        {
            state_flags |= CreateStateFlags.wall_sensor.getValue();
        }

        if (cliff_left_b != 0)
        {
            state_flags |= CreateStateFlags.cliff_left.getValue();
        }
        if (cliff_front_left_b != 0)
        {
            state_flags |= CreateStateFlags.cliff_front_left.getValue();
        }
        if (cliff_front_right_b != 0)
        {
            state_flags |= CreateStateFlags.cliff_front_right.getValue();
        }
        if (cliff_right_b != 0)
        {
            state_flags |= CreateStateFlags.cliff_right.getValue();
        }

        if (virtual_wall_b != 0)
        {
            state_flags |= CreateStateFlags.virtual_wall.getValue();
        }

        if ((buttons_b & CREATE_OI_FLAGS_PLAY_BUTTON) != 0)
        {
            state_flags |= CreateStateFlags.play_button.getValue();
        }

        if ((buttons_b & CREATE_OI_FLAGS_ADVANCE_BUTTON) != 0)
        {
            state_flags |= CreateStateFlags.advance_button.getValue();
        }

        ret.create_state_flags = new UnsignedInt(state_flags);

        Instant time = Instant.now();

        ret.time = time.getEpochSecond() + (time.getNano() / 1e9);
        ret.velocity = velocity_h * 1e-3;
        ret.radius = radius_h * 1e-3;
        ret.right_wheel_velocity = velocity_right_h * 1e-3;
        ret.left_wheel_velocity = velocity_left_h * 1e-3;
        ret.distance_traveled = distance_h * 1e-3;
        ret.angle_traveled = angle_h * (Math.PI / 180.0);
        ret.battery_charge = charge_H;
        ret.battery_capacity = capacity_H;

        return ret;
    }

    private boolean streaming = false;

    private void startStreaming()
    {
        synchronized (this)
        {
            byte[] command = {(byte)CREATE_OI_OP_STREAM, 1, 6};
            port.writeBytes(command, command.length);
            streaming = true;
        }
    }

    private void stopStreaming()
    {
        synchronized (this)
        {
            byte[] command = {(byte)CREATE_OI_OP_STREAM_RESUME, 0};
            port.writeBytes(command, command.length);
            streaming = false;
        }
    }

    public void close()
    {
        stopStreaming();

        byte[] command = {(byte)CREATE_OI_OP_START};
        synchronized (this)
        {
            port.writeBytes(command, command.length);
            port.closePort();
            port = null;
        }
    }

    public void drive(double velocity, double radius)
    {
        synchronized (this)
        {
            byte[] vel = ByteBuffer.allocate(2).putShort((short)(velocity * 1e3)).array();
            byte[] rad = ByteBuffer.allocate(2).putShort((short)(radius * 1e3)).array();

            byte[] command = {(byte)CREATE_OI_OP_DRIVE, vel[0], vel[1], rad[0], rad[1]};

            port.writeBytes(command, command.length);
        }
    }

    public void drive_direct(double rightWheelVelocity, double leftWheelVelocity)
    {
        synchronized (this)
        {
            byte[] r = ByteBuffer.allocate(2).putShort((short)(rightWheelVelocity * 1e3)).array();
            byte[] l = ByteBuffer.allocate(2).putShort((short)(leftWheelVelocity * 1e3)).array();

            byte[] command = {(byte)CREATE_OI_OP_DRIVE_DIRECT, r[0], r[1], l[0], l[1]};

            port.writeBytes(command, command.length);
        }
    }

    public void stop()
    {
        drive(0, 0);
    }

    public synchronized void setf_leds(boolean play, boolean advance)
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

        byte[] command = {(byte)CREATE_OI_OP_LEDS, bits, 80, (byte)255};
    }

    long play_client;

    public synchronized void claim_play_callback()
    {
        play_client = ServerEndpoint.getCurrentEndpoint();
    }

    // Play a song.  This calls the client through the play_callback
    // to retrieve the notes
    private void play()
    {
        // If there is no current client, return
        if (play_client == 0)
            return;

        // Call the play_callback on the client and receive notes
        UnsignedBytes notes =
            get_play_callback().getClientFunction(play_client).func(rrvar_distance_traveled, rrvar_angle_traveled);

        // Generate the command and send to the robot to play the song
        byte[] command = new byte[notes.value.length + 5];
        command[0] = (byte)CREATE_OI_OP_SONG;
        command[1] = 0;
        command[2] = (byte)(notes.value.length / 2);
        System.arraycopy(notes.value, 0, command, 3, notes.value.length);
        command[3 + notes.value.length] = (byte)CREATE_OI_OP_PLAY_SONG;
        command[4 + notes.value.length] = 0;
        synchronized (this)
        {
            try
            {
                port.writeBytes(command, command.length);
            }
            catch (Exception e)
            {
                e.printStackTrace(System.out);
            }
        }
    }
}
