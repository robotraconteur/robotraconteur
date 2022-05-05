/**
 * @file Security.h
 *
 * @author John Wason, PhD
 *
 * @copyright Copyright 2011-2020 Wason Technology, LLC
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * @par
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RobotRaconteur/DataTypes.h"

#pragma once

namespace RobotRaconteur
{
class ROBOTRACONTEUR_CORE_API UserAuthenticator;
class ROBOTRACONTEUR_CORE_API ServerContext;

/**
 * @brief Security policy for Robot Raconteur service
 *
 * The security policy sets an authenticator, and a set of policies. PasswordFileUserAuthenticator is
 * an example of an authenticator. The valid options for Policies are as follows:
 *
 * | Policy name | Possible Values | Default | Description |
 * | ---         | ---             | ---      | ---          |
 * | requirevaliduser | true,false | false | Set to "true" to require a user be authenticated before accessing service |
 * | allowobjectlock | true,false | false | If "true" allow users to request object locks. requirevaliduser most also be
 * "true" |
 *
 * The security policy is passed as a parameter to
 * RobotRaconteurNode::RegisterService(boost::string_ref, boost::string_ref,
 * boost::shared_ptr<RRObject>,  boost::shared_ptr< ServiceSecurityPolicy > securitypolicy),
 * or set using ServerContext::SetSecurityPolicy().
 *
 * See \ref security for more information.
 *
 */
class ROBOTRACONTEUR_CORE_API ServiceSecurityPolicy
{
  public:
    RR_SHARED_PTR<UserAuthenticator> Authenticator;
    std::map<std::string, std::string> Policies;

    /**
     * @brief Construct an empty ServiceSecurityPolicy
     *
     * Must use boost::make_shared<ServiceSecurityPolicy>()
     *
     */
    ServiceSecurityPolicy();

    /**
     * @brief Construct a ServiceSecurityPolicy
     *
     * @param Authenticator The user authenticator
     * @param Policies The security policies
     */
    ServiceSecurityPolicy(RR_SHARED_PTR<UserAuthenticator> Authenticator,
                          const std::map<std::string, std::string>& Policies);
};

/**
 * @brief Class representing an authenticated user
 *
 * Use ServerEndpoint::GetCurrentAuthenticatedUser() to retrieve the
 * authenticated user making a request
 *
 * See \ref security for more information.
 *
 */
class ROBOTRACONTEUR_CORE_API AuthenticatedUser
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
    /** @brief The authenticated username */
    virtual std::string GetUsername();

    /** @brief The user privileges */
    virtual std::vector<std::string> GetPrivileges();

    /** @brief The user properties */
    virtual std::vector<std::string> GetProperties();

    /** @brief The user login time */
    virtual boost::posix_time::ptime GetLoginTime();

    /** @brief The user last access time */
    virtual boost::posix_time::ptime GetLastAccessTime();

    /**
     * @brief Construct a new AuthenticatedUser
     *
     * Must use boost::make_shared<AuthenticatedUser>()
     *
     * Valid privileges are as follows:
     *
     * | Privilege Name | Description |
     * | --- | --- |
     * | objectlock | Allow user to lock objects |
     * | objectlockoverride | Allow user to unlock object locks made by other users |
     *
     * @param username The username
     * @param privileges The user privileges
     * @param properties The user properties
     * @param context The context of the service
     */
    AuthenticatedUser(boost::string_ref username, const std::vector<std::string>& privileges,
                      const std::vector<std::string>& properties, RR_SHARED_PTR<ServerContext> context);

    /** @brief Update the last access time to now */
    virtual void UpdateLastAccess();

    virtual ~AuthenticatedUser() {}
};

/**
 * @brief Base class for user authenticators
 *
 * Used with ServiceSecurityPolicy to secure services
 *
 * Override AuthenticateUser to implement different types
 * of user authenticators.
 *
 */
class ROBOTRACONTEUR_CORE_API UserAuthenticator
{
  public:
    /**
     * @brief Authenticate a user using username and credentials
     *
     * The service will call AuthenticateUser when clients attempt to authenticate. The
     * username and credentials are passed to RobotRaconteurNode::ConnectService() or
     * RobotRaconteurNode::AsyncConnectService(). The authenticator must analyze these
     * values to decide how to authenticate the user.
     *
     * On successful authentication, return a populated AuthenticatedUser object. On
     * failure, throw an exception.
     *
     * See \ref security for more information.
     *
     * @param username The username provided by the client
     * @param credentials The credentials provided by the client
     * @param context The context of the service requesting authentication
     * @return RR_SHARED_PTR<AuthenticatedUser> An authenticated user object
     */
    virtual RR_SHARED_PTR<AuthenticatedUser> AuthenticateUser(
        boost::string_ref username, const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >& credentials,
        RR_SHARED_PTR<ServerContext> context, RR_SHARED_PTR<ITransportConnection> transport) = 0;

    virtual ~UserAuthenticator() {}
};

/**
 * @brief Simple authenticator using a list of username, password hash, and privileges stored in a file or string
 *
 * The password user authenticator expects a `string` or `istream` containing a list of users,
 * one per line. Each line contains the username, password as md5 hash, and privileges, separated by white spaces.
 * An example of authentication file contents:
 *
 * ~~~
 *    user1 79e262a81dd19d40ae008f74eb59edce objectlock
 *    user2 309825a0951b3cf1f25e27b61cee8243 objectlock
 *    superuser1 11e5dfc68422e697563a4253ba360615 objectlock,objectlockoverride
 * ~~~
 *
 * The password is md5 hashed. This hash can be generated using the `--md5passwordhash` command in \ref
 * robotraconteurgen. The privileges are comma separated. Valid privileges are as follows:
 *
 * | Privilege Name | Description |
 * | --- | --- |
 * | objectlock | Allow user to lock objects |
 * | objectlockoverride | Allow user to unlock object locks made by other users |
 *
 */
class ROBOTRACONTEUR_CORE_API PasswordFileUserAuthenticator : public UserAuthenticator
{

  private:
    class ROBOTRACONTEUR_CORE_API User
    {
      public:
        std::string username;
        std::string passwordhash;
        std::vector<std::string> privileges;
        std::vector<NodeID> allowed_client_nodeid;
    };

  private:
    std::map<std::string, RR_SHARED_PTR<User> > validusers;
    bool require_verified_client;

  public:
    /**
     * @brief Construct a new PasswordFileUserAuthenticator using text supplied as a stream
     *
     * @param file The file text as a stream
     */
    PasswordFileUserAuthenticator(std::istream& file, bool require_verified_client = false);

    /**
     * @brief Construct a new PasswordFileUserAuthenticator using text supplied as a string
     *
     * @param data The file text
     */
    PasswordFileUserAuthenticator(boost::string_ref data, bool require_verified_client = false);

    virtual ~PasswordFileUserAuthenticator() {}

  private:
    void load(boost::string_ref data);

  public:
    virtual RR_SHARED_PTR<AuthenticatedUser> AuthenticateUser(
        boost::string_ref username, const std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >& credentials,
        RR_SHARED_PTR<ServerContext> context, RR_SHARED_PTR<ITransportConnection> transport);

    static std::string MD5Hash(boost::string_ref text);

  private:
    void InitializeInstanceFields();
};

#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES
/** @brief Convenience alias for ServiceSecurityPolicy shared_ptr */
using ServiceSecurityPolicyPtr = RR_SHARED_PTR<ServiceSecurityPolicy>;
/** @brief Convenience alias for AuthenticatedUser shared_ptr */
using AuthenticatedUserPtr = RR_SHARED_PTR<AuthenticatedUser>;
/** @brief Convenience alias for UserAuthenticator shared_ptr */
using UserAuthenticatorPtr = RR_SHARED_PTR<UserAuthenticator>;
/** @brief Convenience alias for PasswordFileUserAuthenticator shared_ptr */
using PasswordFileUserAuthenticatorPtr = RR_SHARED_PTR<PasswordFileUserAuthenticator>;
#endif

} // namespace RobotRaconteur