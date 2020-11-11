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

%shared_ptr(RobotRaconteur::ServiceSecurityPolicy)
%shared_ptr(RobotRaconteur::UserAuthenticator)
%shared_ptr(RobotRaconteur::PasswordFileUserAuthenticator);
%shared_ptr(RobotRaconteur::WrappedUserAuthenticator);

%feature("director") RobotRaconteur::WrappedUserAuthenticatorDirector;

namespace RobotRaconteur
{
%rename(NativeUserAuthenticator) UserAuthenticator;
%nodefaultctor UserAuthenticator;
class UserAuthenticator
{
	public:
	RR_MAKE_METHOD_PRIVATE(AuthenticateUser)
	
	%extend
	{
	
	virtual boost::shared_ptr<RobotRaconteur::AuthenticatedUser> AuthenticateUser(const std::string &username, boost::intrusive_ptr<RobotRaconteur::MessageElement> credentials, boost::shared_ptr<RobotRaconteur::ServerContext> context)
	{
		if (!context) throw InvalidArgumentException("Context cannot be null");
		if (!credentials) throw InvalidArgumentException("Credentials cannot be null");
		boost::intrusive_ptr<RRMap<std::string,RRValue> > r=rr_cast<RRMap<std::string,RRValue> >(context->GetNode()->UnpackMapType<std::string,RRValue>(credentials->CastData<MessageElementNestedElementList >()));
		return $self->AuthenticateUser(username,r->GetStorageContainer(),context,RR_SHARED_PTR<RobotRaconteur::ITransportConnection>());
	}	
	}
		
};

class ServiceSecurityPolicy
{
public:
	ServiceSecurityPolicy(boost::shared_ptr<RobotRaconteur::UserAuthenticator> Authenticator, const std::map<std::string, std::string>& Policies);	
};

class PasswordFileUserAuthenticator : public UserAuthenticator
{
public:
	PasswordFileUserAuthenticator(const std::string& data, bool require_verified_client = false);
};

class WrappedUserAuthenticatorDirector
{
public:
	virtual ~WrappedUserAuthenticatorDirector() {}
	virtual boost::shared_ptr<AuthenticatedUser> AuthenticateUser(const std::string &username, boost::intrusive_ptr<RobotRaconteur::MessageElement> credentials, boost::shared_ptr<RobotRaconteur::ServerContext> context);
	
};

class WrappedUserAuthenticator : public UserAuthenticator
{
public:
	void SetRRDirector(WrappedUserAuthenticatorDirector* director, int id);
	
};
}
