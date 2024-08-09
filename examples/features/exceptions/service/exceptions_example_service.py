import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s
import numpy as np


class ExceptionExample_impl:
    def my_exception_function1(self):
        raise RR.OperationFailedException("This is an example of an OperationFailedException")

    def my_exception_function2(self):
        exp_type = RRN.GetExceptionType("experimental.exception_example.MyExampleCustomException")
        raise exp_type("This is an example of a custom exception")


def main():
    with RR.ServerNodeSetup("experimental.exception_example", 53224) as node_setup:

        # Register the service type
        RRN.RegisterServiceTypeFromFile("../robdef/experimental.exception_example.robdef")

        # Create the object
        obj = ExceptionExample_impl()

        # Register the service
        RRN.RegisterService("exception_example", "experimental.exception_example.ExceptionExample", obj)

        # Wait for the user to quit
        input("Server started, press enter to quit...")


if __name__ == '__main__':
    main()
