%exception {
try
{
  $action
  }
  catch (RobotRaconteur::RobotRaconteurException& e)
  {
    
	jclass clazz2;
	jmethodID mid2;
	jthrowable jexp;
    RobotRaconteur::HandlerErrorInfo error_info(e);
	clazz2=jenv->FindClass("com/robotraconteur/RobotRaconteurExceptionUtil");
	if (!clazz2) goto rrerrfail;
	mid2 = jenv->GetStaticMethodID(clazz2, "errorInfoToException" ,"(J)Ljava/lang/RuntimeException;");
	if (!mid2) goto rrerrfail;
	
	jexp=(jthrowable)jenv->CallStaticObjectMethod(clazz2,mid2,&error_info);
	jenv->Throw(jexp);
	return $null;
	
	rrerrfail:
		jclass clazz = jenv->FindClass( "java/lang/RuntimeException");
		jenv->ThrowNew(clazz, e.what());
		return $null;
	
	
  }
  catch (std::exception& e) {
    jclass clazz = jenv->FindClass( "java/lang/RuntimeException");
    jenv->ThrowNew(clazz, e.what());
    return $null;
  }
}