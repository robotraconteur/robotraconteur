include(${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurSWIGErrorGen.cmake)

rr_swig_parse_errors(RR_ERRORS)

set(RR_CSHARP_ERR_OVERRIDE
    "MessageErrorType_NullValue,NullReferenceException;MessageErrorType_InvalidOperation,InvalidOperationException;MessageErrorType_OutOfRange,ArgumentOutOfRangeException;MessageErrorType_InvalidArgument,ArgumentException;MessageErrorType_NotImplementedError,NotImplementedException;MessageErrorType_KeyNotFound,KeyNotFoundException;MessageErrorType_IOError,System.IO.IOException"
)
set(RR_CSHARP_ERR_DECL_M
    "
    public class @RR_ERROR_TYPE@ : RobotRaconteurException
    {
        public @RR_ERROR_TYPE@(string message,string errorsubname=null,object param_=null)
            : base(@RR_ERROR_CODE@,@RR_ERROR_TYPE_STR@,message,errorsubname,param_)
        {
        }
    }")

set(RR_CSHARP_ERR_DECL_M2
    "
    public class @RR_ERROR_TYPE@ : RobotRaconteurException
    {
        public @RR_ERROR_TYPE@(string error, string message,string errorsubname=null,object param_=null)
            : base(@RR_ERROR_CODE@,error,message,errorsubname,param_)
        {
        }
    }")

rr_swig_replace_errors(
    "${RR_CSHARP_ERR_DECL_M}"
    "${RR_CSHARP_ERR_DECL_M2}"
    ""
    "${RR_ERRORS}"
    "${RR_CSHARP_ERR_OVERRIDE}"
    RR_ERRORS_DECL1
    REMOVE_OVERRIDE)
string(REPLACE "MessageErrorType_" "MessageErrorType." RR_ERRORS_DECL "${RR_ERRORS_DECL1}")

set(RR_CSHARP_ERR_CASE_M
    "
             case  @RR_ERROR_CODE@:
                  return new  @RR_ERROR_TYPE@(errorstring,errorsubname,param2);
")

set(RR_CSHARP_ERR_CASE_M2
    "
             case  @RR_ERROR_CODE@:
                  return new  @RR_ERROR_TYPE@(errorname, errorstring,errorsubname,param2);
")

set(RR_CSHARP_ERR_CASE_M3
    "
             case  @RR_ERROR_CODE@:
             {
                var error_ret = new @RR_ERROR_TYPE@(errorstring);
                if (errorsubname != null) error_ret.Data.Add(\"ErrorSubName\",errorsubname);
                if (param2 != null) error_ret.Data.Add(\"ErrorParam\",param2);
                return error_ret;
             }
")

rr_swig_replace_errors(
    "${RR_CSHARP_ERR_CASE_M}"
    "${RR_CSHARP_ERR_CASE_M2}"
    "${RR_CSHARP_ERR_CASE_M3}"
    "${RR_ERRORS}"
    "${RR_CSHARP_ERR_OVERRIDE};MessageErrorType_RemoteError,RobotRaconteurRemoteException"
    RR_ERRORS_CASE1
    IGNORE_CODE
    MessageErrorType_RemoteError)
string(REPLACE "MessageErrorType_" "MessageErrorType." RR_ERRORS_CASE "${RR_ERRORS_CASE1}")

set(RR_CSHARP_ERR_CATCH_M
    "
             if (exception is @RR_ERROR_TYPE@)
             {
                 entry.Error = @RR_ERROR_CODE@;
                 entry.AddElement(\"errorname\", @RR_ERROR_TYPE_STR@);
                 entry.AddElement(\"errorstring\", exception.Message);
                 if (errorsubname != null)
                 {
                     entry.AddElement(\"errorsubname\",errorsubname);
                 }
                 if (param2 != null)
                 {
                     entry.AddElement(\"errorparam\",param2);
                 }
                 return;
             }
")

rr_swig_replace_errors(
    "${RR_CSHARP_ERR_CATCH_M}"
    ""
    "${RR_CSHARP_ERR_CATCH_M}"
    "${RR_ERRORS}"
    "${RR_CSHARP_ERR_OVERRIDE}"
    RR_ERRORS_CATCH1
    OVERRIDE_ONLY)
string(REPLACE "MessageErrorType_" "MessageErrorType." RR_ERRORS_CATCH "${RR_ERRORS_CATCH1}")

set(RR_CSHARP_ERR_CATCH2_M
    "
             if (exception is @RR_ERROR_TYPE@)
             {
                 error_info.error_code = (uint)@RR_ERROR_CODE@;
                 error_info.errorname = @RR_ERROR_TYPE_STR@;
                 error_info.errormessage = exception.Message;
                 if (errorsubname != null)
                 {
                     error_info.errorsubname = errorsubname;
                 }
                 if (param2 != null)
                 {
                     error_info.param_ = param2;
                 }
                 return error_info;
             }
")

rr_swig_replace_errors(
    "${RR_CSHARP_ERR_CATCH2_M}"
    ""
    "${RR_CSHARP_ERR_CATCH2_M}"
    "${RR_ERRORS}"
    "${RR_CSHARP_ERR_OVERRIDE}"
    RR_ERRORS_CATCH2
    OVERRIDE_ONLY)
string(REPLACE "MessageErrorType_" "MessageErrorType." RR_ERRORS_CATCH2 "${RR_ERRORS_CATCH2}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/Error.cs.in" "${CMAKE_CURRENT_BINARY_DIR}/Error.cs" @ONLY)
