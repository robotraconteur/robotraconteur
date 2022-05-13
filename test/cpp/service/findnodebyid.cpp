#include <RobotRaconteur.h>

using namespace RobotRaconteur;
using namespace std;


int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cout << "Usage for findnodebyid:  findnodebyid id schemes" << endl;
        return -1;
    }

    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    NodeID id = NodeID(string(argv[1]));
    vector<string> schemes;
    boost::split(schemes, argv[2], boost::is_from_range(',', ','));

    RR_SHARED_PTR<TcpTransport> t = RR_MAKE_SHARED<TcpTransport>();
    t->EnableNodeDiscoveryListening();
    RobotRaconteurNode::s()->RegisterTransport(t);

    RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
    RobotRaconteurNode::s()->RegisterTransport(c2);

    RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();
    RobotRaconteurNode::s()->RegisterTransport(c5);

    boost::this_thread::sleep(boost::posix_time::seconds(6));

    vector<NodeInfo2> r = RobotRaconteurNode::s()->FindNodeByID(id, schemes);

    for (vector<NodeInfo2>::iterator e = r.begin(); e != r.end(); e++)
    {
        cout << "NodeID: " << e->NodeID.ToString() << endl;
        cout << "NodeName: " << e->NodeName << endl;
        cout << "ConnectionURL: " << boost::join(e->ConnectionURL, ", ") << endl;
        cout << endl;
    }

    RobotRaconteurNode::s()->Shutdown();

    return 0;
}