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

%shared_ptr(RobotRaconteur::WrappedGeneratorClient);

%feature("director") RobotRaconteur::AsyncGeneratorReturnRequestDirector;
%feature("director") RobotRaconteur::WrappedGeneratorServerDirector;

namespace RobotRaconteur
{
	%nodefaultctor WrappedGeneratorClient;
	class WrappedGeneratorClient
	{
	public:		

	RR_RELEASE_GIL()
		virtual boost::intrusive_ptr<MessageElement> Next(boost::intrusive_ptr<MessageElement> v);
		virtual void AsyncNext(boost::intrusive_ptr<MessageElement> v, int32_t timeout, AsyncRequestDirector* handler, int32_t id);

		virtual void Abort();
		virtual void AsyncAbort(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);

		virtual void Close();
		virtual void AsyncClose(int32_t timeout, AsyncVoidReturnDirector* handler, int32_t id);

		
		virtual std::vector<boost::intrusive_ptr<MessageElement> > NextAll();	
	RR_KEEP_GIL()
	};

	class AsyncGeneratorClientReturnDirector
	{
	public:
		virtual ~AsyncGeneratorClientReturnDirector() {}
		virtual void handler(boost::shared_ptr<WrappedGeneratorClient> ret, uint32_t error_code, const std::string& errorname, const std::string& errormessage);
	};

	class WrappedGeneratorServerDirector
	{
	public:
		WrappedGeneratorServerDirector();
		virtual ~WrappedGeneratorServerDirector() {}

		virtual boost::intrusive_ptr<MessageElement> Next(boost::intrusive_ptr<MessageElement> m);

		virtual void Abort();
		virtual void Close();

		int32_t objectheapid;
	};
}