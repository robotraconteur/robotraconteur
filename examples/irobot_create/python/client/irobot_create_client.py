# Example iRobot Create client in Python

from RobotRaconteur.Client import *
import time
import numpy
import sys

# Function to call when "Bump" event occurs


def bumped():
    print("Bump!!")


def main():

    url = 'rr+tcp://localhost:22354?service=create'
    if (len(sys.argv) >= 2):
        url = sys.argv[1]

    # Connect to the service
    c = RRN.ConnectService(url)

    # Add a function handler for the "Bump" event
    c.bump += bumped

    # Demonstrate peeking a wire value
    state_val, _ = c.create_state.PeekInValue()
    print(state_val)

    # Connect a WireConnection to the "packets" wire
    wire = c.create_state.Connect()

    # Add a callback function for when the wire value changes
    wire.WireValueChanged += wire_changed

    # Set the play_callback function for this client
    c.claim_play_callback()
    c.play_callback.Function = play_callback

    # Drive a bit
    c.drive(0.1, 1)
    time.sleep(5)
    c.drive(0, 1)
    time.sleep(10)


# Function to call when the wire value changes
def wire_changed(w, value, time):

    val = w.InValue
    # Print the new value to the console.  Comment out this line
    # to see the other output more clearly
    print(val)

# Callback for when the play button is pressed on the Create


def play_callback(dist, angle):
    return numpy.array([69, 16, 60, 16, 69, 16], dtype='u1')


if __name__ == '__main__':
    main()
