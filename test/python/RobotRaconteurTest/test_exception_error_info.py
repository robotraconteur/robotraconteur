import RobotRaconteur as RR
from RobotRaconteur.RobotRaconteurPythonError import RobotRaconteurExceptionUtil


def test_exception_to_error_info_param():
    # The error parameter must be stored in HandlerErrorInfo.param_ so it reaches the client
    exp = RR.InvalidOperationException("x", "sub1", RR.VarValue(5, "int32"))
    error_info = RobotRaconteurExceptionUtil.ExceptionToErrorInfo(exp)
    assert error_info.param_ is not None
    exp2 = RobotRaconteurExceptionUtil.ErrorInfoToException(error_info)
    assert isinstance(exp2, RR.InvalidOperationException)
    assert exp2.message == "x"
    assert exp2.errorsubname == "sub1"
    assert exp2.errorparam.data == 5
