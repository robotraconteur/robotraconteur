INCLUDE(${CMAKE_SOURCE_DIR}/SWIG/RobotRaconteurSWIGErrorGen.cmake)

RR_SWIG_PARSE_ERRORS(RR_ERRORS)

set(RR_JAVA_ERR_OVERRIDE "MessageErrorType_NullValue,NullPointerException;MessageErrorType_InvalidOperation,IllegalStateException;MessageErrorType_InvalidArgument,IllegalArgumentException")
set(RR_JAVA_ERR_DECL_M "
    public class @RR_ERROR_TYPE@ extends RobotRaconteurException
    {        
        public @RR_ERROR_TYPE@(String message) 
		{
            super(MessageErrorType.@RR_ERROR_CODE@,@RR_ERROR_TYPE_STR@,message);
        }

        public @RR_ERROR_TYPE@(String message,String subname, Object param_) 
		{
            super(MessageErrorType.@RR_ERROR_CODE@,@RR_ERROR_TYPE_STR@,message,subname,param_);
        }

    }")

set(RR_JAVA_ERR_DECL_M2 "
    public class @RR_ERROR_TYPE@ extends RobotRaconteurException
    {        
        public @RR_ERROR_TYPE@(String error, String message)
		{
            super(MessageErrorType.@RR_ERROR_CODE@,error,message);
        }

        public @RR_ERROR_TYPE@(String error, String message, String subname, Object param_)
		{
            super(MessageErrorType.@RR_ERROR_CODE@,error,message,subname,param_);
        }
    }")

foreach (RR_ERROR ${RR_ERRORS})
RR_SWIG_REPLACE_ERRORS("${RR_JAVA_ERR_DECL_M}" "${RR_JAVA_ERR_DECL_M2}" "" "${RR_ERROR}" "${RR_JAVA_ERR_OVERRIDE}" RR_ERROR_DECL REMOVE_OVERRIDE)
if (NOT "${RR_ERROR_DECL}" STREQUAL "")
string(REGEX REPLACE "[ \t\n\r]*public class ([A-Za-z0-1_]+).*" "\\1" RR_ERROR_DECL_TYPE "${RR_ERROR_DECL}")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ExceptionTemplate.java.in" "${CMAKE_CURRENT_BINARY_DIR}/exceptionsrc/com/robotraconteur/${RR_ERROR_DECL_TYPE}.java" @ONLY)
endif()
endforeach()
	
set(RR_JAVA_ERR_CASE_M "
             case  @RR_ERROR_CODE@:
                  return new  @RR_ERROR_TYPE@(errorstring,errorsubname,param2);
")

set(RR_JAVA_ERR_CASE_M2 "
             case  @RR_ERROR_CODE@:
                  return new  @RR_ERROR_TYPE@(errorname,errorstring,errorsubname,param2);
")

set(RR_JAVA_ERR_CASE_M3 "
             case  @RR_ERROR_CODE@:
                  return new  @RR_ERROR_TYPE@(errorname,errorstring);
")

RR_SWIG_REPLACE_ERRORS("${RR_JAVA_ERR_CASE_M}" "${RR_JAVA_ERR_CASE_M2}" "${RR_CSHARP_ERR_CASE_M3}" "${RR_ERRORS}" "${RR_JAVA_ERR_OVERRIDE};MessageErrorType_RemoteError,RobotRaconteurRemoteException" RR_ERRORS_CASE IGNORE_CODE MessageErrorType_RemoteError)

set(RR_JAVA_ERR_CATCH_M "
             if (exception instanceof @RR_ERROR_TYPE@)
             {
                 entry.setError(MessageErrorType.@RR_ERROR_CODE@);
                 entry.addElement(\"errorname\", @RR_ERROR_TYPE_STR@);
                 entry.addElement(\"errorstring\", message);
                 return;
             }             
")


RR_SWIG_REPLACE_ERRORS("${RR_JAVA_ERR_CATCH_M}" "" "${RR_JAVA_ERR_CATCH_M}" "${RR_ERRORS}" "${RR_JAVA_ERR_OVERRIDE}" RR_ERRORS_CATCH OVERRIDE_ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/RobotRaconteurExceptionUtil.java.in" "${CMAKE_CURRENT_BINARY_DIR}/exceptionsrc/com/robotraconteur/RobotRaconteurExceptionUtil.java" @ONLY)
