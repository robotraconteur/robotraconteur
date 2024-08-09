// container_value_types.cpp - Example of using container value types (maps and lists)

#include <stdio.h>
#include <iostream>
#include <boost/range/algorithm.hpp>
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

// Only use the RR alias in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;

int main(int argc, char* argv[])
{
    RR::ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, argc, argv);

    experimental::exception_example::ExceptionExamplePtr c =
        RR::rr_cast<experimental::exception_example::ExceptionExample>(
            RR::RobotRaconteurNode::s()->ConnectService("rr+tcp://localhost:53224/?service=exception_example"));

    // Catch an OperationFailedException built-in exception
    try
    {
        c->my_exception_function1();
    }
    catch (const RR::OperationFailedException& e)
    {
        std::cout << "Caught OperationFailedException: " << e.what() << std::endl;
    }

    // Catch a custom exception
    try
    {
        c->my_exception_function2();
    }
    catch (const experimental::exception_example::MyExampleCustomException& e)
    {
        std::cout << "Caught MyExampleCustomException: " << e.what() << std::endl;
    }

    std::cout << "exception_example.cpp example complete" << std::endl;
    return 0;
}
