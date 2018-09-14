INCLUDE(${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurSWIGErrorGen.cmake)

RR_SWIG_PARSE_ERRORS(RR_ERRORS)

set(RR_CSHARP_ERR_OVERRIDE "MessageErrorType_NullValue,NullReferenceException;MessageErrorType_InvalidOperation,InvalidOperationException;MessageErrorType_InvalidArgument,ArgumentException")
set(RR_CSHARP_ERR_DECL_M "
    public class @RR_ERROR_TYPE@ : RobotRaconteurException
    {        
        public @RR_ERROR_TYPE@(string message) 
            : base(@RR_ERROR_CODE@,@RR_ERROR_TYPE_STR@,message)
        {
        }
    }")

set(RR_CSHARP_ERR_DECL_M2 "
    public class @RR_ERROR_TYPE@ : RobotRaconteurException
    {        
        public @RR_ERROR_TYPE@(string error, string message) 
            : base(@RR_ERROR_CODE@,error,message)
        {
        }
    }")

RR_SWIG_REPLACE_ERRORS("${RR_CSHARP_ERR_DECL_M}" "${RR_CSHARP_ERR_DECL_M2}" "${RR_ERRORS}" "${RR_CSHARP_ERR_OVERRIDE}" RR_ERRORS_DECL1 REMOVE_OVERRIDE)
string(REPLACE "MessageErrorType_" "MessageErrorType." RR_ERRORS_DECL "${RR_ERRORS_DECL1}" )

set(RR_CSHARP_ERR_CASE_M "
             case  @RR_ERROR_CODE@:
                  return new  @RR_ERROR_TYPE@(errorstring);
")

set(RR_CSHARP_ERR_CASE_M2 "
             case  @RR_ERROR_CODE@:
                  return new  @RR_ERROR_TYPE@(errorname, errorstring);
")

RR_SWIG_REPLACE_ERRORS("${RR_CSHARP_ERR_CASE_M}" "${RR_CSHARP_ERR_CASE_M2}" "${RR_ERRORS}" "${RR_CSHARP_ERR_OVERRIDE};MessageErrorType_RemoteError,RobotRaconteurRemoteException" RR_ERRORS_CASE1 IGNORE_CODE MessageErrorType_RemoteError)
string(REPLACE "MessageErrorType_" "MessageErrorType." RR_ERRORS_CASE "${RR_ERRORS_CASE1}" )

set(RR_CSHARP_ERR_CATCH_M "
             if (exception is @RR_ERROR_TYPE@)
             {
                 entry.Error = @RR_ERROR_CODE@;
                 entry.AddElement(\"errorname\", @RR_ERROR_TYPE_STR@);
                 entry.AddElement(\"errorstring\", exception.Message);
                 return;
             }             
")


RR_SWIG_REPLACE_ERRORS("${RR_CSHARP_ERR_CATCH_M}" "" "${RR_ERRORS}" "${RR_CSHARP_ERR_OVERRIDE}" RR_ERRORS_CATCH1 OVERRIDE_ONLY)
string(REPLACE "MessageErrorType_" "MessageErrorType." RR_ERRORS_CATCH "${RR_ERRORS_CATCH1}" )

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurNET/Error.cs.in" "${CMAKE_CURRENT_BINARY_DIR}/Error.cs" @ONLY)
