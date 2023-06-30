# Command Line Options {#command_line_options}

Robot Raconteur version 0.9.3 introduced command line option parsing for Node Setup classes. The following is an example of a program being started with the hardware transport disabled, the node name changed to testprog10, the TCP listen port changed to 62222, and the log level set to `DEBUG`. (There are many other options available beyond the ones shown in this example!)

    ./my_device_driver --robotraconteur-hardware-enable=false --robotraconteur-nodename=testprog10 --robotraconteur-tcp-port=62222 --robotraconteur-log-level=DEBUG

*Node Setup classes must be configured to use command line options. See below.*

## Available Options

Options can be `boolean`, `int`, or `string`. Boolean must be `true` or `false`.


The following options are available:


| Option | Type | Client Default | Server Default | Description |
| ---  | ---  | ---            | ---            | ---         |
| `--robotraconteur-discovery-listening-enable=` | boolean | true | true | Enable node discovery listening |
| `--robotraconteur-discovery-announce-enable=` | boolean | false| true | Enable node discovery listening |
| `--robotraconteur-local-enable=` | boolean | true | true | Enable Local transport |
| `--robotraconteur-tcp-enable=` | boolean | true | true | Enable TCP transport |
| `--robotraconteur-hardware-enable=` | boolean | false | false | Enable Hardware transport (USB,PCIe,Bluetooth) |
| `--robotraconteur-intra-enable=` | boolean | true | true | Enable Intra transport |
| `--robotraconteur-local-start-server=` | boolean | false | true | Start Local transport server |
| `--robotraconteur-local-start-client=` | boolean | false | false| Start Local transport named client. Ignored if `start-server` is `true` |
| `--robotraconteur-local-server-public=` | boolean | false | true | Start Local transport server as public on local machine |
| `--robotraconteur-tcp-start-server=` | boolean | false | true | Start TCP transport server |
| `--robotraconteur-tcp-listen-localhost=` | boolean | false | false | TCP transport server listen on localhost only |
| `--robotraconteur-tcp-start-server-sharer=` | boolean | false | true | Start TCP transport server using port sharer |
| `--robotraconteur-tcp-ws-add-origin=` | string |  |  | Comma separated list of WebSocket origins to add to TCP transport access control |
| `--robotraconteur-tcp-ws-remove-origin=` | string |  |  | Comma separated list of WebSocket origins to remove from TCP transport access control |
| `--robotraconteur-tcp-ipv4-discovery=` | boolean | false | false | Enable IPv4 discovery if discovery announce and/or listening is enabled |
| `--robotraconteur-tcp-ipv6-discovery=` | boolean | true | true | Enable IPv6 discovery if discovery announce and/or listening is enabled |
| `--robotraconteur-intra-start-server=` | boolean | false | true | Start Intra transport server |
| `--robotraconteur-disable-timeouts=` | boolean | false | false | Disable timeouts by setting to a large value (for debugging purposes) |
| `--robotraconteur-disable-message4=` | boolean | false | false | |
| `--robotraconteur-disable-stringtable=` | boolean | true | true | |
| `--robotraconteur-load-tls=` | boolean | false | false | Load TLS certificate for TCP transport. Default `true` for `SecureServerNodeSetup` |
| `--robotraconteur-require-tls=` | boolean | false | false | Require TLS certificate for TCP transport. Default `true` for `SecureServerNodeSetup` |
| `--robotraconteur-nodeid=` | string |  | | Override the NodeID for the node. NodeID is typically determined by the Local transport on startup using system cache |
| `--robotraconteur-nodename=` | string |  | *program specific* | Override the NodeName for the node. The default NodeName is typically set by the program |
| `--robotraconteur-tcp-port=` | string |  | *program specific* | Override the TCP listen port. The default port is typically set by the program |
| `--robotraconteur-log-level=` | string | WARNING | WARNING | Sets the log level of the node. This overrides the environmental variable. Valid settings are `FATAL`, `ERROR`, `WARNING`, `INFO`, `DEBUG`, `TRACE`.
| `--robotraconteur-local-tap-enable=` | boolean | false | false | Enable local tap feature (must also specify tap name) |
| `--robotraconteur-local-tap-name=` | string | | | Name of local tap |
| `--robotraconteur-jumbo-message=` | boolean | false | false | Enable jumbo messages (up to 100 MB) |

## Enable Command Line Options

Programs using Robot Raconteur must be configured to use command line options. This is accomplished using the NodeSetup classes. The following sections demonstrate using the command line options in each language.

** Note that [tcp_port] and [node_name] are placeholders and the real values are specific to each program! **

### C++

C++ uses `argc` and `argv` from the main function to pass to the node setup:

Client:

    ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, argc, argv);

Server:

    ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, [node_name], [tcp_port], argc, argv);

### C#

C# uses `args` (type `string[]`) from the program main function to pass to the node setup:

Client:

    using(var node_setup = ClientNodeSetup(args)) { ... }

Server:

    using(var node_setup = ServerNodeSetup([node_name], [tcp_port], args)) { ... }

### Java

Java uses `args` (type `String[]`) from the program main function to pass to the node setup:

Client:

    ClientNodeSetup node_setup = ClientNodeSetup(args);

Server:

    ServerNodeSetup node_setup = ServerNodeSetup([node_name], [tcp_port], args);

### Python

Python can either be initialized using `from RobotRaconteur.Client import *` for a default client configurations, or using the node setup classes. The `RobotRaconteur.Client` module supports command line options starting in version 0.10.
    

Client:

    from RobotRaconteur.Client import *
    
    ...

Server:

    import RobotRaconteur as RR
    RRN = RR.RobotRaconteurNode.s
    import sys

    with RR.ServerNodeSetup([node_name], [tcp_port], argv=sys.argv) as node_setup:
        ...

### MATLAB

Command line options are not supported for MATLAB
