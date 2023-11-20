// Simple test to read the RobotRaconteurNode version

#include <RobotRaconteur.h>
#include <iostream>

using namespace RobotRaconteur;

int main(int argc, char* argv[])
{
    std::cout << "Robot Raconteur Version: " << RobotRaconteurNode::s()->GetRobotRaconteurVersion() << std::endl;
    return 0;
}