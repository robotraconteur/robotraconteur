# iRobot Create Matlab Robot Raconteur Client Examples

The iRobot Create Client example demonstrates how to use the iRobot Create service with Robot Raconteur.
The client drives the robot for a short distance, reads the states, and provides a callback for playing
a short song. (In most cases a callback would not be used for this function, but it is a good example of how to use callbacks.)

Press the "Play" button on the Create to play the song. The callback will be called when the song is played.

This example assumes that the iRobot Create Service is running. Any implementation of the service from the examples
can be used. Pass the correct URL as the first argument on the
command line of a different URL is required for the connection, for example if the service is running on a different
machine.

The iRobot Create example demonstrates using `property`, `function`, `event`, `callback`, and `wire` members.
It also demonstrates using constants and structures.

## Setup

The Matlab Robot Raconteur add-on are required to run the example.

Install the Robot Raconteur Matlab add-on if not already installed. The add-on can be installed using
the [Add-On Explorer in Matlab](https://www.mathworks.com/products/matlab/add-on-explorer.html).
Search for "Robot Raconteur" and install the add-on. The homepage for the add-on
can be found [here](https://www.mathworks.com/matlabcentral/fileexchange/80509-robot-raconteur-matlab).

One of the other iRobot Create service examples must be running. The instructions below use the Python
service examples. See the instructions for the other examples for additional setup steps.

## Run Example

See the [Python service example README](../../python/service/README.md) for instructions on running the service.

With the service running, open Matlab and navigate to the `examples/irobot_create/matlab/client/` directory. Run
the following command in the Matlab command window:

```matlab
irobot_create_client
```
