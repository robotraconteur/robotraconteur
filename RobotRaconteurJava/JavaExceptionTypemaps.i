%exception {
try
{
  $action
  }
  catch (RobotRaconteur::RobotRaconteurException& e)
  {
    jclass clazz1;
	jmethodID mid1;
	jobject errc;
	jclass clazz2;
	jmethodID mid2;
	jobject errname;
	jobject errmsg;
	jthrowable jexp;
    clazz1=jenv->FindClass("com/robotraconteur/MessageErrorType");
	if (!clazz1) goto rrerrfail;
	mid1=jenv->GetStaticMethodID(clazz1, "swigToEnum", "(I)Lcom/robotraconteur/MessageErrorType;");
	if (!mid1) goto rrerrfail;
	errc=jenv->CallStaticObjectMethod(clazz1,mid1,(jint)e.ErrorCode);
	if (!errc) goto rrerrfail;
	clazz2=jenv->FindClass("com/robotraconteur/RobotRaconteurExceptionUtil");
	if (!clazz2) goto rrerrfail;
	mid2 = jenv->GetStaticMethodID(clazz2, "errorCodeToException" ,"(Lcom/robotraconteur/MessageErrorType;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/RuntimeException;");
	if (!mid2) goto rrerrfail;
	errname=jenv->NewStringUTF(e.Error.c_str());
	errmsg=jenv->NewStringUTF(e.Message.c_str());
	jexp=(jthrowable)jenv->CallStaticObjectMethod(clazz2,mid2,errc,errname,errmsg);
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