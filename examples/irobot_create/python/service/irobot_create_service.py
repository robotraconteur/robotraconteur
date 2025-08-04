# Example Robot Raconteur service in Python

from contextlib import suppress
from pathlib import Path
import serial
import struct
import time
import RobotRaconteur as RR
# Convenience shorthand to the default node.
# RRN is equivalent to RR.RobotRaconteurNode.s
RRN = RR.RobotRaconteurNode.s
import threading
import numpy
import traceback
import sys
import argparse
import drekar_launch_process
import numpy as np

# Port names and NodeID of this service
serial_port_name = "/dev/ttyUSB0"

CREATE_OI_OP_START = 128
CREATE_OI_OP_FULL = 132
CREATE_OI_OP_DRIVE = 137
CREATE_OI_OP_LEDS = 139
CREATE_OI_OP_SONG = 140
CREATE_OI_OP_DRIVE_DIRECT = 145
CREATE_OI_OP_STREAM = 148
CREATE_OI_OP_STREAM_RESUME = 150

CREATE_OI_FLAGS_BUMP_RIGHT = 0x1
CREATE_OI_FLAGS_BUMP_LEFT = 0x2
CREATE_OI_FLAGS_WHEEL_DROP_RIGHT = 0x4
CREATE_OI_FLAGS_WHEEL_DROP_LEFT = 0x8
CREATE_OI_FLAGS_WHEEL_DROP_CASTER = 0x10

CREATE_OI_FLAGS_PLAY_BUTTON = 0x1
CREATE_OI_FLAGS_ADVANCE_BUTTON = 0x4

CREATE_OI_SET_LED_PLAY = 2
CREATE_OI_SET_LED_ADVANCE = 8


class Create_impl(object):
    def __init__(self):
        self.bump = RR.EventHook()
        self._lock = threading.RLock()
        self._recv_lock = threading.RLock()
        self._play_callback = None
        self._streaming = False

        self._lastbump = False
        self._bumpers = 0
        self._play_pressed = False
        self._distance_traveled = 0
        self._angle_traveled = 0
        self._downsample = 0
        self._ep = 0

        self._create_state_type = RRN.GetStructureType("experimental.create3.CreateState")
        self._create_constants = RRN.GetConstants("experimental.create3")
        self._create_state_flags = self._create_constants["CreateStateFlags"]
        # cSpell: disable-next-line
        self._packet_unpack = struct.Struct(">xBBBBBBB4xBhh6xHH14x4xhhhh")

        self._wires_ready = False

        self._bump_fired = False

    def RRServiceObjectInit(self, ctx, service_path):
        self._wires_ready = True

    def drive(self, velocity, radius):
        with self._lock:
            dat = struct.pack(">B2h", CREATE_OI_OP_DRIVE, int(velocity * 1e3), int(radius * 1e3))
            self._serial.write(dat)

    def drive_direct(self, right_velocity, left_velocity):
        with self._lock:
            dat = struct.pack(">B2h", CREATE_OI_OP_DRIVE_DIRECT, int(right_velocity * 1e3), int(left_velocity * 1e3))
            self._serial.write(dat)

    def stop(self):
        self.drive(0, 0)

    def setf_leds(self, play, advance):
        with self._lock:
            bits = 0
            if play:
                bits |= CREATE_OI_SET_LED_PLAY
            if advance:
                bits |= CREATE_OI_SET_LED_ADVANCE
            dat = struct.pack(">4B", CREATE_OI_OP_LEDS, bits, 80, 255)
            self._serial.write(dat)

    def _start_streaming(self):
        with self._lock:
            if (self._streaming):
                raise Exception("Already streaming")

            # Send stop streaming command
            command = struct.pack(">2B", CREATE_OI_OP_STREAM_RESUME, 0)
            # Flush the serial read buffer
            self._serial.read_all()
            time.sleep(0.5)
            self._serial.read_all()

            # Start the thread that receives serial data
            self._streaming = True
            t = threading.Thread(target=self._recv_thread)
            t.start()
            # Send command to start streaming packets after a short delay
            time.sleep(.1)
            command = struct.pack(">3B", CREATE_OI_OP_STREAM, 1, 6)
            self._serial.write(command)

    def _stop_streaming(self):
        with self._lock:
            command = struct.pack(">2B", CREATE_OI_OP_STREAM_RESUME, 0)
            self._serial.write(command)
            self._streaming = False

    @property
    def distance_traveled(self):
        return self._distance_traveled

    @property
    def angle_traveled(self):
        return self._angle_traveled

    @property
    def bumpers(self):
        return self._bumpers

    @property
    def play_callback(self):
        return self._play_callback

    @play_callback.setter
    def play_callback(self, value):
        self._play_callback = value

    def _init(self, port):
        with self._lock:
            self._serial = serial.Serial(port=port, baudrate=57600)
            dat = struct.pack(">8B", CREATE_OI_OP_START, CREATE_OI_OP_FULL,
                              CREATE_OI_OP_STREAM_RESUME, 0, CREATE_OI_OP_LEDS, 0, 80, 255)
            self._serial.write(dat)
            time.sleep(.1)
            self._serial.flushInput()
        self._start_streaming()

    def _close(self):
        with suppress(Exception):
            self._stop_streaming()
        dat = struct.pack(">B", CREATE_OI_OP_START)
        self._serial.write(dat)
        with self._lock:
            self._serial.close()

    # Thread function that runs serial receive loop
    def _recv_thread(self):
        try:
            while self._streaming:
                if (not self._streaming):
                    return
                self._receive_sensor_packets()
        except:
            # Exception will be thrown when the port is closed
            # just ignore it
            if (self._streaming):

                traceback.print_exc()
            pass

    # Receive the packets and execute the right commands
    def _receive_sensor_packets(self):
        while self._streaming:
            magic = struct.unpack('>B', self._serial.read(1))[0]

            if (magic != 19):
                continue

            nbytes = struct.unpack('>B', self._serial.read(1))[0]

            if nbytes == 0:
                continue

            packets = self._serial.read(nbytes)

            checksum = struct.unpack('>B', self._serial.read(1))[0]

            if ((magic + nbytes + sum(packets) + checksum) & 0xFF) != 0:
                # Bad checksum
                continue

            state = self._parse_sensor_packets(magic, packets)

            # Send packet to the client through wire.  If there is a large backlog
            # of packets don't send
            if self._wires_ready and (self._serial.inWaiting() < 85):

                self.create_state.OutValue = state

            # Check for bump event
            if (state.create_state_flags & self._create_state_flags["bump_right"] or
                    state.create_state_flags & self._create_state_flags["bump_left"]):
                if not self._bump_fired:
                    self._fire_bump()
                    self._bump_fired = True
            else:
                self._bump_fired = False

            # Set properties
            self._distance_traveled = state.distance_traveled
            self._angle_traveled = state.angle_traveled
            self._bumpers = state.create_state_flags & (
                self._create_state_flags["bump_right"] | self._create_state_flags["bump_left"])

            # Check for play button press
            if (state.create_state_flags & self._create_state_flags["play_button"]):
                if not self._play_pressed:
                    self._play_pressed = True
                    self._play()
            else:
                self._play_pressed = False

    def _parse_sensor_packets(self, magic, packets):

        ret = self._create_state_type()

        bump_flags_b, wall_b, cliff_left_b, cliff_front_left_b, cliff_front_right_b, cliff_right_b, virtual_wall_b, \
            buttons_b, distance_h, angle_h, charge_H, capacity_H, velocity_h, radius_h, right_vel_h, left_vel_h, = \
            self._packet_unpack.unpack(packets)

        state_flags = 0
        if bump_flags_b & CREATE_OI_FLAGS_BUMP_RIGHT:
            state_flags |= self._create_state_flags["bump_right"]
        if bump_flags_b & CREATE_OI_FLAGS_BUMP_LEFT:
            state_flags |= self._create_state_flags["bump_left"]
        if bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_RIGHT:
            state_flags |= self._create_state_flags["wheel_drop_right"]
        if bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_LEFT:
            state_flags |= self._create_state_flags["wheel_drop_left"]
        if bump_flags_b & CREATE_OI_FLAGS_WHEEL_DROP_CASTER:
            state_flags |= self._create_state_flags["wheel_drop_caster"]

        if wall_b != 0:
            state_flags |= self._create_state_flags["wall_sensor"]

        if cliff_left_b != 0:
            state_flags |= self._create_state_flags["cliff_left"]
        if cliff_front_left_b != 0:
            state_flags |= self._create_state_flags["cliff_front_left"]
        if cliff_front_right_b != 0:
            state_flags |= self._create_state_flags["cliff_front_right"]
        if cliff_right_b != 0:
            state_flags |= self._create_state_flags["cliff_right"]

        if virtual_wall_b != 0:
            state_flags |= self._create_state_flags["virtual_wall"]

        if buttons_b & CREATE_OI_FLAGS_PLAY_BUTTON:
            state_flags |= self._create_state_flags["play_button"]
        if buttons_b & CREATE_OI_FLAGS_ADVANCE_BUTTON:
            state_flags |= self._create_state_flags["advance_button"]

        ret.create_state_flags = state_flags

        ret.time = time.perf_counter()

        ret.velocity = float(velocity_h) * 1e-3
        ret.radius = float(radius_h) * 1e-3
        ret.right_wheel_velocity = float(right_vel_h) * 1e-3
        ret.left_wheel_velocity = float(left_vel_h) * 1e-3
        ret.distance_traveled = float(distance_h) * 1e-3
        ret.angle_traveled = np.deg2rad(float(angle_h))
        ret.battery_charge = float(charge_H)
        ret.battery_capacity = float(capacity_H)

        return ret

    # Fire the bump event, all connected clients will receive
    def _fire_bump(self):
        self.bump.fire()

    def claim_play_callback(self):
        with self._lock:
            self._ep = RR.ServerEndpoint.GetCurrentEndpoint()

    def _play(self):
        if (self._ep == 0):
            return

        try:
            # Callback used for example only, most practical applications would not use this
            cb_func = self.play_callback.GetClientFunction(self._ep)
            notes = cb_func(self._distance_traveled, self._angle_traveled)
            notes2 = list(notes) + [141, 0]

            command = struct.pack("%sB" % (5 + len(notes)), CREATE_OI_OP_SONG, 0, int(len(notes) / 2), *list(notes2))
            with self._lock:
                self._serial.write(command)

        except:
            traceback.print_exc()


def main():

    # Accept the names of the nodename and port from command line
    parser = argparse.ArgumentParser(description="Example Robot Raconteur iRobot Create service")
    parser.add_argument("--serialport", type=str, default=serial_port_name, help="The serial port to use")
    args, _ = parser.parse_known_args()

    # Use the robdef from a file. In practice, this is usually done using
    # a package resource. See RobotRaconteurCompanion.Util.RobDef.register_service_types_from_resources
    RRN.RegisterServiceTypesFromFiles(
        [str(Path(__file__).parent.parent.parent / "robdef" / "experimental.create3.robdef")])

    # Initialize the object in the service
    obj = Create_impl()

    obj._init(args.serialport)

    with RR.ServerNodeSetup("experimental.create3", 22354, argv=sys.argv):

        # Register the service
        ctx = RRN.RegisterService("create", "experimental.create3.Create", obj)

        print("iRobot Create Service Started")
        print()
        print("Candidate connection URLs:")
        ctx.PrintCandidateConnectionURLs()
        print()
        print("Press Ctrl-C to quit")

        # Use drekar_launch_process to wait for exit
        drekar_launch_process.wait_exit()

        # Shutdown
        obj._close()


if __name__ == '__main__':
    main()
