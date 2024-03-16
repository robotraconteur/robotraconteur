//Allow all STL errors to fall through to the global error catcher
%typemap(throws) std::bad_exception     "throw _e;"
%typemap(throws) std::domain_error      "throw _e;"
%typemap(throws) std::exception         "throw _e;"
%typemap(throws) std::invalid_argument  "throw _e;"
%typemap(throws) std::length_error      "throw _e;"
%typemap(throws) std::logic_error       "throw _e;"
%typemap(throws) std::out_of_range      "throw _e;"
%typemap(throws) std::overflow_error    "throw _e;"
%typemap(throws) std::range_error       "throw _e;"
%typemap(throws) std::runtime_error     "throw _e;"
%typemap(throws) std::underflow_error   "throw _e;"

%{

static void RRExceptionToPythonError(RobotRaconteurException& rrexp)
{
	PyObject* modules_dict= PyImport_GetModuleDict();
	if (modules_dict==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load RobotRaconeturPythonError module");
		return;
	}

	PyObject* err_module=PyDict_GetItemString(modules_dict, "RobotRaconteur.RobotRaconteurPythonError");
	if (err_module==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load RobotRaconeturPythonError module");
		return;
	}

	swig::SwigVar_PyObject exceptionUtil=PyObject_GetAttrString(err_module, "RobotRaconteurExceptionUtil");
	if (exceptionUtil==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load RobotRaconeturPythonError.RobotRaconteurExceptionUtil class");
		return;
	}

	swig::SwigVar_PyObject errorCodeToException=PyObject_GetAttrString(exceptionUtil, "ErrorInfoToException");
	if (errorCodeToException==NULL)
	{
		PyErr_SetString(PyExc_Exception, "Could not load RobotRaconeturExceptionUtil.ErrorInfoToException function");
		return;
	}

	RobotRaconteur::HandlerErrorInfo error(rrexp);

	swig::SwigVar_PyObject error_obj;
  	error_obj = SWIG_NewPointerObj(SWIG_as_voidptr(&error), SWIGTYPE_p_RobotRaconteur__HandlerErrorInfo,  0 );

	swig::SwigVar_PyObject pyErr=PyObject_CallFunction(errorCodeToException, "O", (PyObject*)error_obj);
	swig::SwigVar_PyObject pyErrType=PyObject_Type(pyErr);

	PyErr_SetObject(pyErrType,pyErr);
}

%}

%define RR_Py_Exception_GIL()
%exception %{

    try {
		RR_Release_GIL gil_save;
        $action
    }

    catch (RobotRaconteurException &e) {
		//PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
		RRExceptionToPythonError(e);
		goto fail;
	}
    catch (std::exception &e) {
        PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
        goto fail;
    }
    /*catch (...)
    {
		PyErr_SetString(PyExc_Exception,"Unknown Robot Raconteur error");
		goto fail;
    }*/

%}
%enddef

%define RR_Py_Exception()
%exception %{
    try {
        $action
    }

    catch (RobotRaconteurException &e) {
		//PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
		RRExceptionToPythonError(e);
		goto fail;
	}
    catch (std::exception &e) {
        PyErr_SetString(PyExc_Exception, const_cast<char*>(e.what()));
        goto fail;
    }
    /*catch (...)
    {
		PyErr_SetString(PyExc_Exception,"Unknown Robot Raconteur error");
		goto fail;
    }*/

%}
%enddef

RR_Py_Exception()

%{

static void ThrowPythonError()
{
	PyObject* exc1;
	PyObject* val1;
	PyObject* tb1;
	PyErr_Fetch(&exc1,&val1,&tb1);
	PyErr_NormalizeException(&exc1,&val1,&tb1);
	swig::SwigVar_PyObject exc = exc1;
	swig::SwigVar_PyObject val = val1;
	swig::SwigVar_PyObject tb = tb1;

	if (RobotRaconteur::PythonTracebackPrintExc)
	{
		std::cerr << "RobotRaconteurPython caught exception:" << std::endl;
		if (exc && val && tb)
		{
			PyErr_Display(exc,val,tb);
		}
		PyErr_Clear();
	}
	else
	{
		PyErr_Clear();
	}

	PyObject* modules_dict= PyImport_GetModuleDict();
	if (modules_dict==NULL)
	{
		throw InternalErrorException("Could not load RobotRaconeturPythonError module");
	}

	PyObject* err_module=PyDict_GetItemString(modules_dict, "RobotRaconteur.RobotRaconteurPythonError");
	if (err_module==NULL)
	{
		throw InternalErrorException("Could not load RobotRaconeturPythonError module");
	}

	swig::SwigVar_PyObject rr_py_RobotRaconteurException=PyObject_GetAttrString(err_module, "RobotRaconteurException");
	if (rr_py_RobotRaconteurException==NULL)
	{
		throw InternalErrorException("Could not load RobotRaconeturPythonError.RobotRaconteurException type");
	}

	if (PyErr_GivenExceptionMatches(exc,rr_py_RobotRaconteurException))
	{
		swig::SwigVar_PyObject message1 = PyObject_GetAttrString(val,"message");
		swig::SwigVar_PyObject errorname1 = PyObject_GetAttrString(val,"errorname");
		swig::SwigVar_PyObject errorcode1 = PyObject_GetAttrString(val,"errorcode");

		swig::SwigVar_PyObject message=PyObject_Str(message1);
		swig::SwigVar_PyObject errorname=PyObject_Str(errorname1);
		swig::SwigVar_PyObject errorcode=PyNumber_Long(errorcode1);

		if (!(PyObject*)message || !(PyObject*)errorcode || !(PyObject*)errorname)
		{
			throw InternalErrorException("Exception occurred in Python code");
		}

		std::string message2 = PyObjectToUTF8(message);
		std::string errorname2=PyObjectToUTF8(errorname);
		long errorcode2=PyLong_AsLong(errorcode);
		if (errorcode2 > 65535 || errorcode2 < 0) errorcode2=MessageErrorType_UnknownError;

		boost::intrusive_ptr<MessageEntry> m=CreateMessageEntry();
		m->Error=(MessageErrorType)(uint16_t)errorcode2;
		m->AddElement("errorname",stringToRRArray(errorname2));
		m->AddElement("errorstring", stringToRRArray(message2));

		swig::SwigVar_PyObject errorsubname = PyObject_GetAttrString(val,"errorsubname");
		if ((PyObject*)errorsubname && (PyObject*)errorsubname != Py_None)
		{
			swig::SwigVar_PyObject errorsubname2 = PyObject_Str(errorsubname);
			if ((PyObject*)errorsubname2)
			{
				m->AddElement("errorsubname",stringToRRArray(PyObjectToUTF8(errorsubname2)));
			}
		}

		swig::SwigVar_PyObject errorparam = PyObject_GetAttrString(val,"errorparam");
		if ((PyObject*)errorparam && (PyObject*)errorparam != Py_None)
		{
			PyObject* util_module=PyDict_GetItemString(modules_dict, "RobotRaconteur.RobotRaconteurPythonUtil");
			if (util_module)
			{
				swig::SwigVar_PyObject python_pack_element=PyObject_GetAttrString(util_module, "PackMessageElement");
				if ((PyObject*)python_pack_element)
				{
					swig::SwigVar_PyObject py_errorparam_elem = PyObject_CallFunction(python_pack_element,"OsOO",(PyObject*)errorparam,"varvalue errorparam",Py_None,Py_None);
					if ((PyObject*)py_errorparam_elem)
					{
						try
						{
							boost::intrusive_ptr< RobotRaconteur::MessageElement > arg1 ;
							void *argp1 ;
							int newmem1 = 0 ;
							// Extract swig pointer
							int res1 = SWIG_ConvertPtrAndOwn(py_errorparam_elem, &argp1, SWIGTYPE_p_boost__shared_ptrT_RobotRaconteur__MessageElement_t,  0 , &newmem1);
							if (SWIG_IsOK(res1)) {
								if (argp1) arg1 = boost::intrusive_ptr<  RobotRaconteur::MessageElement >(reinterpret_cast< boost::shared_ptr< RobotRaconteur::MessageElement >* >(argp1)->get(),true);
								if (newmem1 & SWIG_CAST_NEW_MEMORY) delete reinterpret_cast< boost::shared_ptr< RobotRaconteur::MessageElement >* >(argp1);
								if (arg1)
								{
									arg1->ElementName = "errorparam";
									m->AddElement(arg1);
								}
							}

						}
						catch (std::exception)
						{
							//TODO: log error
						}
					}
				}
			}
		}


		RobotRaconteurExceptionUtil::ThrowMessageEntryException(m);
	}
	else
	{
		swig::SwigVar_PyObject str=PyObject_Str(val);
		swig::SwigVar_PyObject strname1 = PyObject_GetAttrString(exc,"__name__");
		swig::SwigVar_PyObject strname=PyObject_Str(strname1);
		std::string str2=PyObjectToUTF8(str);
		std::string strname2=PyObjectToUTF8(strname);

		throw UnknownException(strname2, str2);
	}
}

%}

%feature("director:except") {
    if ($error != NULL) {
    	ThrowPythonError();
    }
}
