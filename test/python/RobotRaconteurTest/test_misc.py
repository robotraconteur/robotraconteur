from __future__ import print_function

from RobotRaconteur.Client import *


def test_nowutc():
    print(RRN.NowUTC())
    return


def test_logging():
    import datetime
    r = RR.RRLogRecord()
    r.Node = RRN
    r.Time = datetime.datetime.now()
    r.Level = RR.LogLevel_Warning
    r.Message = "This is a test warning"

    RRN.LogRecord(r)

    print(r)


def test_loghandler():
    log_rrn = RR.RobotRaconteurNode()
    log_rrn.Init()
    log_rrn.SetLogLevel(RR.LogLevel_Debug)
    user_log_handler = RR.UserLogRecordHandler(
        lambda x: print("python handler: " + str(x)))
    log_rrn.SetLogRecordHandler(user_log_handler)

    import datetime
    r = RR.RRLogRecord()
    r.Node = log_rrn
    r.Time = datetime.datetime.now()
    r.Level = RR.LogLevel_Warning
    r.Message = "This is a test warning"

    log_rrn.LogRecord(r)

    log_rrn.Shutdown()


def test_numpy_dtypes():

    np_node = RR.RobotRaconteurNode()
    np_node.Init()

    try:
        np_node.RegisterServiceTypeFromFile(
            "com.robotraconteur.testing.TestService2")
        np_node.RegisterServiceTypeFromFile(
            "com.robotraconteur.testing.TestService1")
        np_node.RegisterServiceTypeFromFile(
            "com.robotraconteur.testing.TestService3")

        np_node.GetNamedArrayDType(
            'com.robotraconteur.testing.TestService3.transform')
        np_node.GetConstants("com.robotraconteur.testing.TestService3")
        np_node.GetPodDType('com.robotraconteur.testing.TestService3.testpod2')
    finally:
        np_node.Shutdown()


def test_file_log_record_handler(tmpdir):
    import datetime
    fname = str(tmpdir.join("rr_log.txt"))
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        h = RR.FileLogRecordHandler()
        h.OpenFile(fname, False)
        node.SetLogRecordHandler(h)
        r = RR.RRLogRecord()
        r.Node = node
        r.Time = datetime.datetime.now()
        r.Level = RR.LogLevel_Warning
        r.Message = "file log record handler test"
        node.LogRecord(r)
        node.SetLogRecordHandler(None)
        del h
    finally:
        node.Shutdown()
    with open(fname) as f:
        assert "file log record handler test" in f.read()
