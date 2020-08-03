# Taps {#taps}

Taps provide a way to record message traffic and log messages (aka log records) that pass through a node. This is intended to provide the ability to inspect the operation of the system, and record raw data from experiments. Taps are node specific, meaning that each node needs to have its data recorded. This is in contrast to rosbag, which passively records an entire system. Robot Raconteur is an ad-hoc system, with no specific "master" to coordinate this behavior.

## Local Taps

Currently, Robot Raconteur provides "local" taps. These function by opening a Unix socket on the local filesystem, with permissions allowing the current user to open the socket. The node will passively transmit log records and optionally message traffic to any clients that connect to the socket. A separate program is used to record these messages to file, and optionally to forward to a central repository.

The data is sent in Message V2 format, with each message starting with the magic "RRAC" and the length of the message encoded as a little-endian `uint32`. See the "Robot Raconteur Message Version 2 Serialization Format" standard for more details. Log records are encoded as a `MessageEntry(EntryType=MessageEntryType.StreamOp,EntryName="logrecord")` with each field of the record stored as a message element. See [Logging](Logging) for more details on the format of the log record.

## Enabling Local Tap

Local taps can be enabled using command line options. Each tap needs a "name", which is used to identify the tap. This name is used with the recorders to identify nodes, and also to name the files. Names must be unique. Starting another tap with the same name may fail silently or have undefined behavior. To avoid this, it is suggested that the NodeName be used.

Example of starting a tap:

    ./my_robotraconteur_driver --robotraconteur-local-tap-enable=true --robotraconteur-local-tap-name=my_robot_1

## Tap for Python client

The convenience module `RobotRaconteur.Client` does not read command line options. `ClientNodeSetup` must be used instead. The following will provide equivalent behavior to `RobotRaconteur.Client`, but provide command line option parsing:

    import RobotRaconteur as RR
    RRN=RR.RobotRaconteurNode.s
    import sys

    main():
        with RR.ClientNodeSetup(argv=sys.argv):
            # Put your main code here!

    if __name__ == "__main__":
        main()

## Recording Local Taps

The [RobotRaconteurLocalTapRecorder](https://github.com/robotraconteur/RobotRaconteurLocalTapRecorder) utility can be used to save the output of local taps. To start it, use the following command:

    dotnet RobotRaconteurLocalTapRecorder.dll

By default, the utility will save the contents of all taps to the current directory. It has the following options:

| Option | Description |
|---     |--- |
| --tap-name= | The name of the tap to record, if not specified records all |
| --output-dir= | The directory to store output, if not specified current directory is used |
| --log-record-only | Only record log messages, do not record message traffic |

## Tap Playback

A module in Python has been provided for playing back message taps. This example will read and print the `MemberName`of each `MessageEntry` in a file:

    import RobotRaconteur as RR

    fname = r"my_robot_1-2020-05-21 22-46-00.robtap"

    with open(fname,"rb") as f:
        reader = RR.TapFileReader(f)
        while True:
            m = reader.ReadNextMessage()
            if m is None:
                break
            entry = m.entries[0]
            print (entry.MemberName)

The following is a more sophisticated example that prints the message field of log records:

    import RobotRaconteur as RR

    fname = r"my_robot_1-2020-05-21 22-46-00.robtap"

    with open(fname,"rb") as f:
        reader = RR.TapFileReader(f)
        while True:
            m = reader.ReadNextMessage()
            if m is None:
                break
            entry = m.entries[0]
            print (entry.MemberName)
            if (entry.MemberName == "logrecord"):
                for i in range(len(entry.elements)):
                    el = entry.elements[i]
                    if el.ElementName == "message":
                        val = reader.UnpackMessageElement(el)
                        print(val.data)

This program uses `UnpackMessageElement()` to read the contents of a message element. Message elements contain the data and parameters that are passed between members. `UnpackMessageElement()` can be used to recover the packed data in the form of `VarValue`. Use `val.datatype` to query the type returned, and `val.data` to retrieve the value. If `struct`, `pod`, or `namedarray` are used, the service type must first be registered using `RRN.RegisterServiceType()` or `RRN.RegisterServiceTypeFromFile()` so the system knows how to unpack the types.
