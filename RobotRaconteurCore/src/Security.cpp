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

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteur/Service.h"
#include "RobotRaconteur/Security.h"
#include "RobotRaconteur/LocalTransport.h"
#include "RobotRaconteur/IntraTransport.h"
#include "RobotRaconteur/TcpTransport.h"

#include <boost/algorithm/string.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>
#include <map>
#include <string>

#ifdef ROBOTRACONTEUR_WINDOWS
#include <Windows.h>
#include <WinCrypt.h>
#else
#include <openssl/md5.h>
#endif

#include <iostream>

namespace RobotRaconteur
{
	ServiceSecurityPolicy::ServiceSecurityPolicy()
	{
		Authenticator.reset();
		Policies.clear();
	}

	ServiceSecurityPolicy::ServiceSecurityPolicy(RR_SHARED_PTR<UserAuthenticator> Authenticator, const std::map<std::string, std::string>& Policies)
	{
		this->Authenticator = Authenticator;
		this->Policies = Policies;
	}

	std::string AuthenticatedUser::GetUsername()
	{
		return m_Username;
	}

	std::vector<std::string> AuthenticatedUser::GetPrivileges()
	{
		return m_Privileges;
	}

	std::vector<std::string> AuthenticatedUser::GetProperties()
	{
		return m_Properties;
	}

	boost::posix_time::ptime AuthenticatedUser::GetLoginTime()
	{
		return m_LoginTime;
	}

	boost::posix_time::ptime AuthenticatedUser::GetLastAccessTime()
	{
		boost::mutex::scoped_lock lock(m_LastAccessTime_lock);
		return m_LastAccessTime;
	}

	AuthenticatedUser::AuthenticatedUser(boost::string_ref username, const std::vector<std::string>& privileges, const std::vector<std::string>& properties, RR_SHARED_PTR<ServerContext> context)
	{
		this->m_Username = RR_MOVE(username.to_string());
		this->m_Privileges = privileges;
		this->m_Properties = properties;
		this->context = context;
		m_LoginTime = context->GetNode()->NowNodeTime();
		m_LastAccessTime = context->GetNode()->NowNodeTime();
	}

	void AuthenticatedUser::UpdateLastAccess()
	{
		boost::mutex::scoped_lock lock(m_LastAccessTime_lock);
		RR_SHARED_PTR<ServerContext> c = context.lock();
		if (!c) throw InvalidOperationException("Context has been released");
		m_LastAccessTime = c->GetNode()->NowNodeTime();
	}

	PasswordFileUserAuthenticator::PasswordFileUserAuthenticator(std::istream &file, bool require_verified_client)
	{
		InitializeInstanceFields();
		std::stringstream buffer;
		buffer << file.rdbuf();
		load(buffer.str());
		this->require_verified_client = require_verified_client;
	}

	PasswordFileUserAuthenticator::PasswordFileUserAuthenticator(boost::string_ref data, bool require_verified_client)
	{
		InitializeInstanceFields();
		load(data);
		this->require_verified_client = require_verified_client;
	}

	void PasswordFileUserAuthenticator::load(boost::string_ref data)
	{

		std::vector<std::string> lines;
		boost::split(lines, data, boost::is_from_range('\n', '\n'));


		BOOST_FOREACH(std::string& l, lines)
		{
			if (l.empty()) continue;
			std::vector<std::string> g;
			std::string g1 = boost::trim_copy(l);
			boost::split(g, g1, boost::is_space(), boost::algorithm::token_compress_on);
			RR_SHARED_PTR<User> u = RR_MAKE_SHARED<User>();
			u->username = g.at(0);
			u->passwordhash = g.at(1);

			if (g.at(2) != "_")
			{
				boost::split(u->privileges, g.at(2), boost::is_from_range(',', ','));
			}

			if (g.size() > 3)
			{
				std::vector<std::string> n_ids;
				boost::split(n_ids, g.at(3), boost::is_from_range(',', ','));
				BOOST_FOREACH(std::string n_id, n_ids)
				{
					u->allowed_client_nodeid.push_back(NodeID(n_id));
				}
			}

			validusers.insert(make_pair(u->username, u));
		}
	}

	RR_SHARED_PTR<AuthenticatedUser> PasswordFileUserAuthenticator::AuthenticateUser(boost::string_ref username, const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > &credentials, RR_SHARED_PTR<ServerContext> context, RR_SHARED_PTR<ITransportConnection> transport)
	{

		std::map<std::string, RR_SHARED_PTR<PasswordFileUserAuthenticator::User> >::iterator u = validusers.find(username.to_string());

		if (u == validusers.end())
			throw AuthenticationException("Invalid username or password");
		std::string password;
		try
		{
			password = RRArrayToString(rr_cast<RRArray<char> >(credentials.at("password")));
		}
		catch (std::exception&)
		{
			throw AuthenticationException("Password not supplied in credentials");
		}

		std::string passwordhash = MD5Hash(password);

		if (u->second->passwordhash != passwordhash)
			throw AuthenticationException("Invalid username or password");
		
		bool client_verified = false;

		if (require_verified_client && transport)
		{
			RR_SHARED_PTR<Transport> t = transport->GetTransport();
			NodeID remote_nodeid;
			RR_SHARED_PTR<LocalTransport> local_t = RR_DYNAMIC_POINTER_CAST<LocalTransport>(t);
			RR_SHARED_PTR<IntraTransport> intra_t = RR_DYNAMIC_POINTER_CAST<IntraTransport>(t);
			if (local_t || intra_t)
			{
				remote_nodeid = transport->GetRemoteNodeID();
				if (!remote_nodeid.IsAnyNode())
				{
					client_verified = true;
				}
			}

			RR_SHARED_PTR<TcpTransport> tcp_t = RR_DYNAMIC_POINTER_CAST<TcpTransport>(t);
			if (tcp_t)
			{
				if (tcp_t->IsSecurePeerIdentityVerified(transport))
				{
					client_verified = true;
					remote_nodeid = NodeID(tcp_t->GetSecurePeerIdentity(transport));
				}
			}

			if (!local_t && !intra_t && !tcp_t)
			{
				throw AuthenticationException("Selected transport does not suppert client verification");
			}

			if (!client_verified)
			{
				throw AuthenticationException("Client connection could not be verified. Check certificate or transport");
			}
			if (!u->second->allowed_client_nodeid.empty())
			{
				if (std::find(u->second->allowed_client_nodeid.begin(), u->second->allowed_client_nodeid.end(), remote_nodeid) == u->second->allowed_client_nodeid.end())
				{
					throw AuthenticationException("Incorrect client NodeID or certificate error");
				}
			}

		}		

		std::vector<std::string> properties;
		return RR_MAKE_SHARED<AuthenticatedUser>(username, validusers.at(username.to_string())->privileges, properties, context);
	}

	std::string PasswordFileUserAuthenticator::MD5Hash(boost::string_ref text)
	{
#ifdef ROBOTRACONTEUR_WINDOWS
		HCRYPTPROV hProv = 0;
		HCRYPTPROV hHash = 0;

		CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
		CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);

		CryptHashData(hHash, (BYTE*)text.data(), (DWORD)text.length(), 0);

		BYTE rgbHash[16] = { 0 };
		DWORD cbHash = 16;
		CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0);

		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);

		std::string s2;
		for (size_t i = 0; i<16; i++)
		{
			std::stringstream s;
			s << std::hex << std::setw(2) << std::setfill('0');

			s << (int)rgbHash[i];
			s2 += s.str();
		}


		return s2;
#else
		uint8_t md[MD5_DIGEST_LENGTH];
		MD5(reinterpret_cast<const uint8_t*>(text.data()), text.size(), reinterpret_cast<uint8_t*>(md));

		std::string s2;
		for (size_t i = 0; i<16; i++)
		{
			std::stringstream s;

			uint32_t a = md[i];
			s << std::hex << std::setw(2) << std::setfill('0') << a;

			s2 += s.str().substr(0, 2);
		}


		return s2;
#endif

	}

	void PasswordFileUserAuthenticator::InitializeInstanceFields()
	{

	}
}