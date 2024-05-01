# Robot Raconteur Quick Start Examples

This directory contains the Quick Start example files. The Quick Start uses the Reynard the Robot
example to demonstrate the basic features of the Robot Raconteur framework. Before executing the Python
examples, make sure to install the required packages:

```bash
python -m pip install robotraconteur reynard-the-robot
```

On Linux, use `python3` instead of `python` to run the Python 3 interpreter. Use `python3` in the rest
of the examples as well.

To run the MATLAB example, make sure to install the MATLAB Robot Raconteur Add-On. It can be installed using
the Add-On Explorer in MATLAB and searching for "Robot Raconteur".

- [reynard_quickstart_service.py](reynard_quickstart_service.py): Example of a minimal Robot Raconteur service for
  Reynard written in Python.
- [reynard_quickstart_client.py](reynard_quickstart_client.py): Example of a minimal Robot Raconteur client for
  Reynard written in Python.
- [reynard_quickstart_client.m](reynard_quickstart_client.m): Example of a minimal Robot Raconteur client for
  Reynard written in MATLAB.
- [reynard_quickstart_client_sub.py](reynard_quickstart_client_sub.py): Example of a minimal Robot Raconteur client for
  Reynard written in Python using Subscriptions to connect to the robot.

To run the service, execute the following command:

```bash
python reynard_quickstart_service.py
```

And open a browser to [http://localhost:29201](http://localhost:29201) to view the Reynard user interface.

To run the client, execute the following command:

```bash
python reynard_quickstart_client.py
```
