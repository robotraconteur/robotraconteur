INCLUDE(${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurSWIGErrorGen.cmake)

RR_SWIG_PARSE_ERRORS(RR_ERRORS)

#set(RR_PYTHON_ERR_OVERRIDE "MessageErrorType_NullValue,NullReferenceException;MessageErrorType_InvalidOperation,InvalidOperationException;MessageErrorType_InvalidArgument,ArgumentException")
set(RR_PYTHON_ERR_DECL_M "
class @RR_ERROR_TYPE@(RobotRaconteurException):
    def __init__(self,message=\"\"):
        super(@RR_ERROR_TYPE@,self).__init__(RobotRaconteurPython.@RR_ERROR_CODE@, @RR_ERROR_TYPE_STR@,message)
")

set(RR_PYTHON_ERR_DECL_M2 "
class @RR_ERROR_TYPE@(RobotRaconteurException):
    def __init__(self,name,message):
        super(@RR_ERROR_TYPE@,self).__init__(RobotRaconteurPython.@RR_ERROR_CODE@, name,message)
")

RR_SWIG_REPLACE_ERRORS("${RR_PYTHON_ERR_DECL_M}" "${RR_PYTHON_ERR_DECL_M2}" "${RR_ERRORS}" "${RR_PYTHON_ERR_OVERRIDE}" RR_ERRORS_DECL REMOVE_OVERRIDE)

set(RR_PYTHON_ERR_CASE_M "
        if (code==RobotRaconteurPython.@RR_ERROR_CODE@):
            return @RR_ERROR_TYPE@(errstr)
")

set(RR_PYTHON_ERR_CASE_M2 "
        if (code==RobotRaconteurPython.@RR_ERROR_CODE@):
            return @RR_ERROR_TYPE@(name,errstr)
")

RR_SWIG_REPLACE_ERRORS("${RR_PYTHON_ERR_CASE_M}" "${RR_PYTHON_ERR_CASE_M2}" "${RR_ERRORS}" "${RR_PYTHON_ERR_OVERRIDE}" RR_ERRORS_CASE)

set(RR_PYTHON_ERR_CATCH_M "
        if (isisntance(exception,@RR_ERROR_TYPE@)):
            return RobotRaconteurPython.@RR_ERROR_CODE@, @RR_ERROR_TYPE_STR@, exception.message		 
")


RR_SWIG_REPLACE_ERRORS("${RR_PYTHON_ERR_CATCH_M}" "" "${RR_ERRORS}" "${RR_PYTHON_ERR_OVERRIDE}" RR_ERRORS_CATCH OVERRIDE_ONLY)

configure_file("${PYTHON2_SOURCE_DIR}/RobotRaconteurPythonError.py.in" "${CMAKE_CURRENT_BINARY_DIR}/RobotRaconteurPythonError.py" @ONLY)
