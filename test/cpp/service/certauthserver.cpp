#include <RobotRaconteur.h>

#include "ServiceTestClient.h"
#include "ServiceTestClient2.h"
#include "ServiceTest.h"
#include "ServiceTest2.h"

using namespace RobotRaconteur;
using namespace RobotRaconteurTest;
using namespace std;


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage for certauthserver:  certauthserver client_nodeids" << endl;
        return -1;
    }

    SecureServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, "authserver", 22224, argc, argv);

    RR_SHARED_PTR<TcpTransport> c = node_setup.GetTcpTransport();

    RobotRaconteurTestServiceSupport s;

    std::vector<NodeID> n;
    std::vector<std::string> n1;
    boost::split(n1, argv[1], boost::is_any_of(","));
    BOOST_FOREACH (const std::string& n2, n1)
    {
        n.push_back(NodeID(n2));
    }

    s.RegisterSecureServices(c, n);

    cout << "Server started, press enter to quit" << endl;
    getchar();
    RobotRaconteurNode::s()->Shutdown();
    cout << "Test completed, no errors detected!" << endl;
    return 0;
}