// Copyright 2011-2018 Wason Technology, LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteur/Error.h"
#include "RobotRaconteur/ErrorUtil.h"
#include "RobotRaconteur/RobotRaconteurConstants.h"

namespace RobotRaconteur
{

	RobotRaconteurException::RobotRaconteurException() : std::runtime_error("")
	{
		InitializeInstanceFields();
		what_string = ToString();
	}

	RobotRaconteurException::RobotRaconteurException(MessageErrorType ErrorCode, const std::string &error, const std::string &message) : std::runtime_error((error + " " + message).c_str())
	{
		InitializeInstanceFields();
		Error = error;
		Message=message;
		this->ErrorCode = ErrorCode;
		what_string = ToString();
	}

	RobotRaconteurException::RobotRaconteurException(const std::string &message, std::exception &innerexception) : std::runtime_error(message.c_str())
	{

		InitializeInstanceFields();
	}

	std::string RobotRaconteurException::ToString()
	{
		return "RobotRaconteurException: " + Error + ": "+ Message;
	}

	const char* RobotRaconteurException::what() const throw ()
	{
		
		return what_string.c_str();
	}

	void RobotRaconteurException::InitializeInstanceFields()
	{
		ErrorCode = MessageErrorType_None;
		Error = "";
	}

#define RR_EXCEPTION_DEF_1(exp_cpp_type, exp_code, exp_type_str) \
	exp_cpp_type::exp_cpp_type(const std::string &message) : RobotRaconteurException(exp_code,exp_type_str,message) \
	{ \
	} \

#define RR_EXCEPTION_DEF_2(exp_cpp_type, exp_code) \
	exp_cpp_type::exp_cpp_type(const std::string &error, const std::string &message) : RobotRaconteurException(exp_code, error, message) \
	{ \
	} \
	exp_cpp_type::exp_cpp_type(std::exception &innerexception) : RobotRaconteurException(exp_code, typeid(innerexception).name(), innerexception.what()) \
	{ \
	} \

	RR_EXCEPTION_TYPES_INIT(RR_EXCEPTION_DEF_1, RR_EXCEPTION_DEF_2)

#undef RR_EXCEPTION_DEF_1
#undef RR_EXCEPTION_DEF_2
		
	void RobotRaconteurExceptionUtil::ExceptionToMessageEntry(std::exception &exception, RR_INTRUSIVE_PTR<MessageEntry> entry)
	{
		if (dynamic_cast<RobotRaconteurException*>(&exception) != 0)
		{
			RobotRaconteurException* r = static_cast<RobotRaconteurException*>(&exception);
			entry->Error = r->ErrorCode;
			entry->AddElement("errorname", stringToRRArray(r->Error));
			entry->AddElement("errorstring", stringToRRArray(r->Message));

		}
		else
		{
			entry->Error = MessageErrorType_RemoteError;
			entry->AddElement("errorname", stringToRRArray(std::string(typeid(exception).name())));
			entry->AddElement("errorstring", stringToRRArray(std::string(exception.what())));


		}

	}

#define RR_MessageEntryToException_M1(exp_cpp_type, exp_code, exp_type_str) \
	case exp_code : \
		return RR_MAKE_SHARED<exp_cpp_type>(entry->FindElement("errorstring")->CastDataToString()); \

#define RR_MessageEntryToException_M2(exp_cpp_type, exp_code) \
	case exp_code : \
		return RR_MAKE_SHARED<exp_cpp_type>(entry->FindElement("errorname")->CastDataToString(), entry->FindElement("errorstring")->CastDataToString()); \

	RR_SHARED_PTR<RobotRaconteurException> RobotRaconteurExceptionUtil::MessageEntryToException(RR_INTRUSIVE_PTR<MessageEntry> entry)
	{
		switch (entry->Error)
		{
			RR_EXCEPTION_TYPES_INIT(RR_MessageEntryToException_M1, RR_MessageEntryToException_M2)
            default:
                break;
        }

		return RR_MAKE_SHARED<RobotRaconteurException>(entry->Error,entry->FindElement("errorname")->CastDataToString(),entry->FindElement("errorstring")->CastDataToString());

	}

#define RR_ThrowMessageEntryException_M1(exp_cpp_type, exp_code, exp_type_str) \
	case exp_code : \
		throw exp_cpp_type(entry->FindElement("errorstring")->CastDataToString()); \

#define RR_ThrowMessageEntryException_M2(exp_cpp_type, exp_code) \
	case exp_code : \
		throw exp_cpp_type(entry->FindElement("errorname")->CastDataToString(), entry->FindElement("errorstring")->CastDataToString()); \


	void RobotRaconteurExceptionUtil::ThrowMessageEntryException(RR_INTRUSIVE_PTR<MessageEntry> entry)
	{
		switch (entry->Error)
		{
			RR_EXCEPTION_TYPES_INIT(RR_ThrowMessageEntryException_M1, RR_ThrowMessageEntryException_M2)
            default:
                break;

		}

		throw RobotRaconteurException(entry->Error,entry->FindElement("errorname")->CastDataToString(),entry->FindElement("errorstring")->CastDataToString());

	}

#define RR_DownCastException_M1(exp_cpp_type, exp_code, exp_type_str) \
	case exp_code : \
		return RR_MAKE_SHARED<exp_cpp_type>(err.Message); \

#define RR_DownCastException_M2(exp_cpp_type, exp_code) \
	case exp_code : \
		return RR_MAKE_SHARED<exp_cpp_type>(err.Error, err.Message); \


	RR_SHARED_PTR<RobotRaconteurException> RobotRaconteurExceptionUtil::DownCastException(RobotRaconteurException& err)
	{
		switch (err.ErrorCode)
		{
			RR_EXCEPTION_TYPES_INIT(RR_DownCastException_M1, RR_DownCastException_M2)
            default:
                break;

		}

		return RR_MAKE_SHARED<RobotRaconteurException>(err.ErrorCode,err.Error,err.Message);

	}

	RR_SHARED_PTR<RobotRaconteurException> RobotRaconteurExceptionUtil::ExceptionToSharedPtr(std::exception& err, MessageErrorType default_type)
	{
		RobotRaconteurException *err2 = dynamic_cast<RobotRaconteurException*>(&err);
		if (err2)
		{
			return DownCastException(*err2);
		}
		else
		{
			if (default_type == MessageErrorType_UnknownError)
			{
				return RR_MAKE_SHARED<UnknownException>(std::string(typeid(err).name()), std::string(err.what()));
			}
			else
			{
				RobotRaconteurException err3(default_type, std::string(typeid(err).name()), std::string(err.what()));
				return DownCastException(err3);
			}
		}
	}

#define RR_DownCastAndThrowException_M1(exp_cpp_type, exp_code, exp_type_str) \
	case exp_code : \
		throw exp_cpp_type(err->Message); \

#define RR_DownCastAndThrowException_M2(exp_cpp_type, exp_code) \
	case exp_code : \
		throw exp_cpp_type(err->Error, err->Message); \


	void RobotRaconteurExceptionUtil::DownCastAndThrowException(RR_SHARED_PTR<RobotRaconteurException> err)
	{
		switch (err->ErrorCode)
		{
			RR_EXCEPTION_TYPES_INIT(RR_DownCastAndThrowException_M1, RR_DownCastAndThrowException_M2)
		default:
			break;
		}

		throw RobotRaconteurException(err->ErrorCode,err->Error,err->Message);
	}
}
