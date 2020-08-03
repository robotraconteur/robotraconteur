# `RobotRaconteurGen` Utility {#robotraconteurgen}

Robot Raconteur uses Service Definitions to define object types, value types, exceptions, constants, and enums for use with services. See \ref service_definitions for more information. For dynamic languages like Python and MATLAB, these types are handled automatically. For statically compiled languages like C++, Java, and C\#, code generation is used to generate "thunk" source that implements the defined types. This "thunk" source must be generated before the program is compiled, and the generated code must be included in the program when it is compiled. The `RobotRaconteurGen` utility is used to generate this "thunk" source.

`RobotRaconteurGen` generates "thunk" source, and has other capabilities. The mode is selected by the "command" parameter. The valid commands are `--thunksource`, `--verify-robdef`, `--string-table`, `--newnodeid`, `--md5passwordhash`, `--pullservicedef`, and `--help`.

## `RobotRaconteurGen --thunksource`

    RobotRaconteurGen --thunksource --lang=<target_language> [options] service1.robdef [service2.robdef ...]

| Option | Required | Description |
| ---    | ---      |---         |
| `--lang=` | Required | Specify language: `cpp`, `csharp`, `java` |
| `--import=` | Optional, one or more | Import other `*.robdef` without generating |
| `--include-path=` or `-I` | Optional, one or more | Add search path for `*.robdef` files |
| `--master-header` | Optional | Generate a master header file. C++ only |
| `--outfile=` | Optional | Output all generated code to single file. C\# only |

The `RobotRaconteurGen --thunksource` command is used to generate the thunk source for use with C++, Java, and C\#. The desired language is specified using the `--lang=` option. Valid options for `lang=` are `cpp`, `csharp`, and `java`. One or more service definition `*.robdef` files are specified as inputs. The service definitions are verified to make sure that all declared and imported types are valid. If types are used but do not need to be generated, they can be specified with one or more `--import=importedservice.robdef` options. 

The `--include-path=` or `-I` option can be used to add search directories for `*.robdef` files. More than one `--include-path` may be specified. The environmental variable `ROBOTRACONTEUR_ROBDEF_PATH` may also be used to specify a search path for `*.robdef` files, with each path separated by a semicolon (;) on Windows, and a colon (:) on other operating systems.

The output directory for generated files will be the current working directory. This can be changed using the `--outdir=` option. The directory must exist.

For C++, a "master header" can be generated. This master header automatically includes all generated header files, and generateds the `ROBOTRACONTEUR_SERVICE_TYPES` macro for use with RobotRaconteurNodeSetup. Use `--master-header` flag to enable generating the master header.

For C\#, all thunk source can be generated as a single file. Use the `--outfile=<filename>` option to specify a filename.

## `RobotRaconteurGen --verify-robdef`

    RobotRaconteurGen --verify-robdef [options] service1.robdef [service2.robdef ...]

| Option | Required | Description |
| ---    | ---      |---         |
| `--include-path=` or `-I` | Optional, one or more | Add search path for `*.robdef` files |

The `RobotRaconteurGen --verify-robdef` command is used to verify one or a group of service definition files without generating thunk source. 

The `--include-path=` or `-I` option can be used to add search directories for `*.robdef` files. More than one `--include-path` may be specified. The environmental variable `ROBOTRACONTEUR_ROBDEF_PATH` may also be used to specify a search path for `*.robdef` files, with each path separated by a semicolon (;) on Windows, and a colon (:) on other operating systems.

The `--import=` is also allowed for compatibility, but since no thunk source is being generated, its use is unnecessary.

## `RobotRaconteurGen --string-table`

    RobotRaconteurGen --string-table [options] service1.robdef [service2.robdef ...]

The `--string-table` command is used to list all the qualified names defined in the service definitions. This is used to help generate string tables for transports. This is an advanced topic not encountered by most users.

## `RobotRaconteurGen --newnodeid`

    RobotRaconteurGen --newnodeid

The `--newnodeid` command generates and prints a new random gen 4 UUUID to stdout.

## `RobotRaconteurGen --md5passwordhash`

    RobotRaconteurGen --md5passwordhash <password>

Returns the hash of the specified password for use with PasswordFileUserAuthenticator.

## `RobotRaconteurGen --pullservicedef`

    RobotRaconteurGen --pullservicedef <url>

The `RobotRaconteurGen --pullservicedef` will connect to the specified Robot Raconteur URL and pull the service definition and all imported service definitions of the service specified. The service definitions will be saved to files in the current directory with the `.robdef` extension.

## `RobotRaconteurGen --help`

    RobotRaconteurGen --help

`RobotRaconteurGen --help` Prints out the following help message:

    RobotRaconteurGen version 0.10.0

    Allowed options:
    -h [ --help ]             print usage message
    --version                 print program version
    --output-dir arg (=.)     directory for output
    --thunksource             generate RR thunk source files
    --verify-robdef           verify robdef files
    --string-table            generate string table entries for robdef
    --newnodeid               generate a new NodeID
    --md5passwordhash         hash a password using MD5 algorithm
    --pullservicedef          pull a service definition from a service URL
    --lang arg                language to generate thunk code for
    -I [ --include-path ] arg include path
    --import arg              input file for use in imports
    --master-header arg       master header file for generated cpp files
    --outfile arg             unified output file (csharp only)