import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s
import numpy as np


class AuthenticationExample_impl:
    def say(self, message):
        print(f"Message: {message}")


def main():
    with RR.ServerNodeSetup("experimental.authentication_example", 53226) as node_setup:

        # Use reynard the robot robdef but only implement "say"
        RRN.RegisterServiceTypeFromFile("../../../reynard_the_robot/robdef/experimental.reynard_the_robot.robdef")

        # Create the object
        obj = AuthenticationExample_impl()

        # Set the authentication configuration
        authdata = "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock\ntestuser2 841c4221c2e7e0cefbc0392a35222512 objectlock\ntestsuperuser 503ed776c50169f681ad7bbc14198b68 objectlock,objectlockoverride"
        p = RR.PasswordFileUserAuthenticator(authdata)
        policies = {"requirevaliduser": "true", "allowobjectlock": "true"}
        s = RR.ServiceSecurityPolicy(p, policies)

        # Register the service
        RRN.RegisterService("authentication_example", "experimental.reynard_the_robot.Reynard", obj, s)

        # Wait for the user to quit
        input("Server started, press enter to quit...")


if __name__ == '__main__':
    main()
