# Reynard Matlab Robot Raconteur Client Example

This example demonstrates how to use Robot Raconteur to control Reynard the Robot using a Matlab client. The
example expects the Python Reynard the Robot server to be running. This example uses the plug-and-play capabilities
to connect to the Reynard service and automatically generate the required type information.

Instructions are provided for Windows and Ubuntu. These examples will work on other platforms with minor modifications.

## Setup

Reynard the Robot and the Matlab Robot Raconteur add-on are required to run the example.

Install the Reynard the Robot server by following the instructions in the [Python example Readme](../../python/client/README.md).
Alternatively the Windows installer can be used to install Reynard the Robot. See the
[Reynard the Robot Readme](https://github.com/robotraconteur/reynard-the-robot/blob/master/README.md)
for more information.

Install the Robot Raconteur Matlab add-on if not already installed. The add-on can be installed using
the [Add-On Explorer in Matlab](https://www.mathworks.com/products/matlab/add-on-explorer.html).
Search for "Robot Raconteur" and install the add-on. The homepage for the add-on
can be found [here](https://www.mathworks.com/matlabcentral/fileexchange/80509-robot-raconteur-matlab).

## Run Example

Open a command prompt and start the Reynard the Robot server:

```cmd
python -m reynard_the_robot
```

On Linux, it may be necessary to use `python3` instead of `python`.

```bash
python3 -m reynard_the_robot
```

If the Windows installer was used, there will be a shortcut in the start menu "Robot Raconteur" folder to start the server.

Open Matlab and navigate to the `examples/reynard_the_robot/matlab/client` directory. Run the following command:

```matlab
reynard_robotraconteur_client
```
