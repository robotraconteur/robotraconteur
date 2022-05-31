package com.robotraconteur;

//  Robot Raconteur - A communication library for robotics and automation systems
//  Copyright (C) 2011  John Wason <wason@wasontech.com>
//                      Wason Technology, LLC
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Lesser GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  Lesser GNU General Public License for more details.
//
//  You should have received a copy of the Lesser GNU General Public License
//  along with Robot Raconteur.  If not, see <http://www.gnu.org/licenses/>.

/**
 The base class for Robot Raconteur exceptions.  These exception contain a Robot Raconteur error code
*/
public class RobotRaconteurException extends RuntimeException
{
    /**
     Initializes an empty exception
    */
    public RobotRaconteurException()
    {
        super();
    }

    /**
     Initializes a new exception

     @param ErrorCode The error code
     @param error The Robot Raconteur error name
     @param message The Robot Raconteur error message
    */
    public RobotRaconteurException(MessageErrorType ErrorCode, String error, String message)
    {
        super(message);
        this.error = error;
        this.errorCode = ErrorCode;
    }

    public RobotRaconteurException(MessageErrorType ErrorCode, String error, String message, String subname,
                                   Object param_)
    {
        super(message);
        this.error = error;
        this.errorCode = ErrorCode;
        this.errorSubName = subname;
        this.errorParam = param_;
    }

    /**
     Initializes a Robot Raconteur exception that contains a C# exception

     @param message The message
     @param innerexception The C# contained by this exception
    */
    public RobotRaconteurException(String message, RuntimeException innerexception)
    {
        super(message, innerexception);
    }

    /**
     The error code
    */
    public MessageErrorType errorCode = MessageErrorType.MessageErrorType_None;

    /**
     The error name
    */
    public String error = "";

    public String errorSubName = null;

    public Object errorParam = null;

    /**
     Returns as tring representation of this exception

     @return The string representation
    */
    @Override public String toString()
    {
        return "RobotRaconteurException: " + error + ": " + this.getMessage();
    }
}