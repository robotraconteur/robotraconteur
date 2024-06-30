# Example iRobot Create joystick drive client in Python

from RobotRaconteur.Client import *
import time
import numpy
import pygame
import sys

if (sys.version_info > (3, 0)):
    def cmp(x, y):
        return (x > y) - (x < y)


def bumped():
    print("Bump!!")


def main():

    url = 'rr+tcp://localhost:22354?service=create'
    if (len(sys.argv) >= 2):
        url = sys.argv[1]

    # Initialize Robot Raconteur and connect to the iRobot Create service
    c = RRN.ConnectService(url)

    c.bump += bumped

    c.play_callback.Function = play_callback
    c.claim_play_callback()

    # Init the joystick
    pygame.init()
    pygame.joystick.init()
    joy = pygame.joystick.Joystick(0)
    joy.init()
    clock = pygame.time.Clock()

    # Loop forever, Press Ctrl-C to exit
    try:
        while True:
            # Loop reading the joysticks and adjust to correct drive parameters
            for event in pygame.event.get():
                pass

            speed = 0
            radius = 32767

            x = joy.get_axis(0)
            if (abs(x) < .2):
                x = 0
            else:
                x = (abs(x) - .2) / .8 * cmp(x, 0)

            y = -joy.get_axis(1)
            if (abs(y) < .2):
                y = 0
            else:
                y = (abs(y) - .2) / .8 * cmp(y, 0)

            if (y == 0):
                if (x < 0 and x != 0):
                    radius = 1
                if (x > 0 and x != 0):
                    radius = -1
                if (x != 0):
                    speed = int(abs(x) * 200.0)
            else:
                speed = int(y * 200.0)
                if (x != 0):
                    radius = int(-(1 - abs(x)) * 5000 * cmp(x, 0))
                    if (radius == 0):
                        radius = -cmp(x, 0)

            # Write out the drive command to the robot
            c.drive(speed * 1e-3, radius * 1e-3)

            # Delay for 20 ms
            clock.tick(20)
    except KeyboardInterrupt:
        pass

# Callback for when the play button is pressed on the Create


def play_callback(dist, angle):
    # Return an array that plays three notes
    return numpy.array([69, 16, 60, 16, 69, 16], dtype='u1')


if __name__ == '__main__':
    main()
