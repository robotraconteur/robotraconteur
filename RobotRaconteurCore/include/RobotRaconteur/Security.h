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

#include "RobotRaconteur/DataTypes.h"

#pragma once

namespace RobotRaconteur
{
	class ROBOTRACONTEUR_CORE_API  UserAuthenticator;
	class ROBOTRACONTEUR_CORE_API  ServerContext;

	class ROBOTRACONTEUR_CORE_API  ServiceSecurityPolicy
	{
	public:
		RR_SHARED_PTR<UserAuthenticator> Authenticator;
		std::map<std::string, std::string> Policies;

		ServiceSecurityPolicy();

		ServiceSecurityPolicy(RR_SHARED_PTR<UserAuthenticator> Authenticator, const std::map<std::string, std::string>& Policies);

	};
	
	class ROBOTRACONTEUR_CORE_API  AuthenticatedUser
	{
	private:
		std::string m_Username;
		std::vector<std::string> m_Privileges;
		std::vector<std::string> m_Properties;
		boost::posix_time::ptime m_LoginTime;
		boost::posix_time::ptime m_LastAccessTime;

		boost::mutex m_LastAccessTime_lock;

		RR_WEAK_PTR<ServerContext> context;

	public:
		virtual std::string GetUsername();

		virtual std::vector<std::string> GetPrivileges();

		virtual std::vector<std::string> GetProperties();

		virtual boost::posix_time::ptime GetLoginTime();

		virtual boost::posix_time::ptime GetLastAccessTime();

		AuthenticatedUser(const std::string &username, const std::vector<std::string>& privileges, const std::vector<std::string>& properties, RR_SHARED_PTR<ServerContext> context);

		virtual void UpdateLastAccess();

		virtual ~AuthenticatedUser() {}

	};


	class ROBOTRACONTEUR_CORE_API  UserAuthenticator
	{
	public:
		virtual RR_SHARED_PTR<AuthenticatedUser> AuthenticateUser(const std::string &username, const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > &credentials, RR_SHARED_PTR<ServerContext> context) = 0;

		virtual ~UserAuthenticator() {}

	};

	class ROBOTRACONTEUR_CORE_API  PasswordFileUserAuthenticator : public UserAuthenticator
	{

	private:
		class ROBOTRACONTEUR_CORE_API  User
		{
		public:
			std::string username;
			std::string passwordhash;
			std::vector<std::string> privileges;

		};

	private:
		std::map<std::string, RR_SHARED_PTR<User> > validusers;

	public:
		PasswordFileUserAuthenticator(std::istream &file);

		PasswordFileUserAuthenticator(const std::string &data);

		virtual ~PasswordFileUserAuthenticator() {}

	private:
		void load(const std::string &data);

	public:
		virtual RR_SHARED_PTR<AuthenticatedUser> AuthenticateUser(const std::string &username, const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > &credentials, RR_SHARED_PTR<ServerContext> context);


		static std::string MD5Hash(const std::string &text);


	private:
		void InitializeInstanceFields();
	};
}