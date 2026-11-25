import RobotRaconteur as RR
from RobotRaconteur import RobotRaconteurPythonError


def test_get_exception_type_non_string():
    # A non-string name returns Exception (it raised NameError for "unicode" on Python 3)
    assert RobotRaconteurPythonError.GetExceptionType(None) is Exception
    assert RobotRaconteurPythonError.GetExceptionType(5) is Exception


def test_generated_exception_subclass():
    # Subclassing a generated remote exception type must not recurse forever in __init__
    t = RobotRaconteurPythonError.GetExceptionType("experimental.exception_type_factory.MyError")

    class MySubError(t):
        pass

    e = MySubError("my message")
    assert isinstance(e, RR.RobotRaconteurRemoteException)
    assert e.errorname == "experimental.exception_type_factory.MyError"
    assert e.message == "my message"

    e2 = t("msg2", "sub2")
    assert e2.errorname == "experimental.exception_type_factory.MyError"
    assert e2.errorsubname == "sub2"
