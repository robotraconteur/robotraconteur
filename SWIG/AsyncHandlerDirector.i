// Copyright 2011-2020 Wason Technology, LLC
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

%include "DataTypes.i"

//%feature("director") RobotRaconteur::AsyncHandlerDirector;

%feature("director") RobotRaconteur::AsyncRequestDirector;
%feature("director") RobotRaconteur::AsyncVoidReturnDirector;
%feature("director") RobotRaconteur::AsyncVoidNoErrReturnDirector;
%feature("director") RobotRaconteur::AsyncStringReturnDirector;
%feature("director") RobotRaconteur::AsyncUInt32ReturnDirector;

namespace RobotRaconteur
{
/*class AsyncHandlerDirector
{
public:
	virtual ~AsyncHandlerDirector() {}
	virtual void handler(void* m, uint32_t error_code,const std::string& errorname,const std::string& errormessage);

};*/

class HandlerErrorInfo
{
public:
	uint32_t error_code;
	std::string errorname;
	std::string errormessage;
	std::string errorsubname;
	boost::intrusive_ptr<RobotRaconteur::MessageElement> param_;
};

class AsyncRequestDirector
{
public:
	virtual ~AsyncRequestDirector() {}
	virtual void handler(boost::intrusive_ptr<RobotRaconteur::MessageElement> ret, HandlerErrorInfo& error) = 0;

};

class AsyncVoidReturnDirector
{
public:
	virtual ~AsyncVoidReturnDirector() {}
	virtual void handler(HandlerErrorInfo& error) = 0;
};

class AsyncVoidNoErrReturnDirector
{
public:
	virtual ~AsyncVoidNoErrReturnDirector() {}
	virtual void handler() = 0;
};

class AsyncStringReturnDirector
{
public:
	virtual ~AsyncStringReturnDirector() {}
	virtual void handler(const std::string& ret, HandlerErrorInfo& error) = 0;
};

class AsyncUInt32ReturnDirector
{
public:
	virtual ~AsyncUInt32ReturnDirector() {}
	virtual void handler(uint32_t ret, HandlerErrorInfo& error) = 0;
};

}