#include <RobotRaconteur.h>

#include "robotraconteur_generated.h"

#include "ServiceTestClient.h"
#include "ServiceTestClient2.h"

using namespace RobotRaconteur;
using namespace std;


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage for certauthclient:  certauthclient url" << endl;
        return -1;
    }

    string url1(argv[1]);
    {

        RR_SHARED_PTR<CommandLineConfigParser> config =
            RR_MAKE_SHARED<CommandLineConfigParser>(RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT_ALLOWED_OVERRIDE);
        config->SetDefaults("authclient", 0,
                            RobotRaconteurNodeSetupFlags_CLIENT_DEFAULT |
                                RobotRaconteurNodeSetupFlags_LOAD_TLS_CERT |
                                RobotRaconteurNodeSetupFlags_REQUIRE_TLS);
        config->ParseCommandLine(argc, argv);

        RobotRaconteurNodeSetup node_setup(RobotRaconteurNode::sp(), ROBOTRACONTEUR_SERVICE_TYPES, config);

        cout << RobotRaconteurNode::s()->NodeID().ToString() << endl;

        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        RR_INTRUSIVE_PTR<RRMap<string, RRValue> > cred1 = AllocateEmptyRRMap<string, RRValue>();
        cred1->insert(make_pair("password", stringToRRArray("testpass1")));

        RR_SHARED_PTR<com::robotraconteur::testing::TestService1::testroot> c1 =
            rr_cast<com::robotraconteur::testing::TestService1::testroot>(
                RobotRaconteurNode::s()->ConnectService(url1, "testuser1", cred1));

        c1->get_d1();
    }

    boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    cout << "Test completed, no errors detected!" << endl;
    return 0;
}