package com.robotraconteur;

public interface IUserAuthenticator {
	AuthenticatedUser authenticateUser(String username, java.util.Map<String,Object> credentials, ServerContext context); 
}
