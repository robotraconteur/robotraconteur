# Logging {#logging}

Robot Raconteur version 0.9.3 introduced logging capabilities. 1200+ log points have been inserted into the core library to assist developers using the library. These log points provide detailed insight into the behavior of the library and assist with tracing the location and cause of errors.

Log data is contained in "records" which have the following form:

```
class ROBOTRACONTEUR_CORE_API RRLogRecord
{
public:
    RR_WEAK_PTR<RobotRaconteurNode> Node;
    RobotRaconteur_LogLevel Level;
    RobotRaconteur_LogComponent Component;
    std::string ComponentName;
    std::string ComponentObjectID;
    int64_t Endpoint;
    std::string ServicePath;
    std::string Member;
    std::string Message;
    boost::posix_time::ptime Time;
    std::string SourceFile;
    uint32_t SourceLine;
    std::string ThreadID;
    std::string FiberID;
};
```

The record contains the originating node, the level of the log message, the component the message originated from, the Robot Raconteur endpoint id, the service path, the object member, the timestamp of the message, the source file an line of the log point, and the thread/coroutine fiber.

By default log messages are sent to `stderr`. They can optionally be sent to a file using `FileLogRecordHandler`. The log records have the following default text format, where italic are left out if not specified:

    [Time (as ISO)] [LogLevel] [ThreadID,FiberID] [NodeID] [Component,*ComponentName*,*ComponentObjectID*] [Endpoint,*ServicePath*,*Member*] *[SourceFile,SourceLine]* Message

Log messages can have the following levels:

```
enum RobotRaconteur_LogLevel
{
	RobotRaconteur_LogLevel_Trace, // Used to trace the activity of the node. Disabled in Release builds
	RobotRaconteur_LogLevel_Debug, // Notification of errors or important activity, use to debug unexpected behavior
	RobotRaconteur_LogLevel_Info, // Information about activity that can be logged during normal operation
	RobotRaconteur_LogLevel_Warning, // Warnings of potential errors
	RobotRaconteur_LogLevel_Error, // Notification of non-fatal errors
	RobotRaconteur_LogLevel_Fatal, // Notification of fatal errors that may affect program stability
	RobotRaconteur_LogLevel_Disable=1000 // Disable all logging
};
```

Log levels are set using the `RobotRaconteurNode::SetLogLevel(level)` function. Logging may also be configured using the `RobotRaconteurNode::SetLogLevelFromEnvVariable()` function. This function is called automatically by `NodeSetup`, and by Python and Matlab clients. The environmental variable `ROBOTRACONTEUR_LOG_LEVEL` is used to set the logging level. It can have the following values: `TRACE`, `DEBUG`, 'INFO`, `WARNING`, `ERROR`, `FATAL`. Log levels can also be configured using \ref command_line_options.

For example, on Linux and Mac OSX, enable debug log level using:

    export ROBOTRACONTEUR_LOG_LEVEL=DEBUG

for Windows, use:

    set ROBOTRACONTEUR_LOG_LEVEL=DEBUG

Records are typically created using the `ROBOTRACONTEUR_LOG` macros. These can be found in Logging.h. These macros are named for the log level, and can optionally include component, path, and component name information.

Records are passed to the node using the `RobotRaconteurNode::LogRecord()` function. By default, the result is sent to `stderr`. The function `RobotRaconteurNode::SetLogRecordHandler(handler)` can be used to send the log record to handler. See below for examples.

## Enable Debug Logging Examples

Examples of enabling Debug level logging:

### Env Variable

Linux:

    export ROBOTRACONTEUR_LOG_LEVEL=DEBUG

Windows:

    set ROBOTRACONTEUR_LOG_LEVEL=DEBUG

### C++

    RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Debug);

### C#

    RobotRaconteurNode.s.SetLogLevel(LogLevel.LogLevel_Debug);

### Java

    RobotRaconteurNode.s().setLogLevel(LogLevel.LogLevel_Debug);

### Python

    RRN.SetLogLevel(RR.LogLevel_Debug)

### MATLAB

    RobotRaconteur.SetLogLevel('DEBUG')

## Save log to file

### C++

    FileLogRecordHandlerPtr log_handler = RR_MAKE_SHARED<FileLogRecordHandler>();
    log_handler.OpenFile("my_log.txt");
    RobotRaconteurNode::s()->SetLogRecordHandler(log_handler);

### C#

    var log_handler = new FileLogRecordHandler();
    log_handler.OpenFile("my_log.txt");
    RobotRaconteurNode.s.SetLogRecordHandler(log_handler);

### Java

    FileLogRecordHandler log_handler = new FileLogRecordHandler();
    log_handler.openFile("my_log.txt");
    RobotRaconteurNode.s().setLogRecordHandler(log_handler);

### Python

    log_handler = RR.FileLogRecordHandler()
    log_handler.OpenFile("my_log.txt")
    RRN.SetLogRecordHandler(log_handler)

### MATLAB

    RobotRaconteur.OpenLogFile('my_log.txt')
