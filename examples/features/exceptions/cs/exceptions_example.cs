// exception_example.cs - Example of handling exceptions in C#

using RobotRaconteur;
using System;
using System.Diagnostics;
using System.Linq;
using System.Collections.Generic;

using (var node_setup = new ClientNodeSetup(args))
{
    var c = (experimental.exception_example.ExceptionExample)RobotRaconteurNode.s.ConnectService(
        "rr+tcp://localhost:53224/?service=exception_example");

    // Catch an OperationFailedException built-in exception
    try
    {
        c.my_exception_function1();
    }
    catch (OperationFailedException e)
    {
        Console.WriteLine("Caught exception: " + e.Message);
    }

    // Catch a custom exception
    try
    {
        c.my_exception_function2();
    }
    catch (experimental.exception_example.MyExampleCustomException e)
    {
        Console.WriteLine("Caught exception: " + e.Message);
    }
}
