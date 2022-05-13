#include <RobotRaconteur.h>

using namespace RobotRaconteur;
using namespace std;


int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cout << "Usage for findservicebytype:  findservicebytype servicetype schemes" << endl;
        return -1;
    }

    string servicetype = string(argv[1]);
    vector<string> schemes;
    boost::split(schemes, argv[2], boost::is_from_range(',', ','));

    RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

    RR_SHARED_PTR<TcpTransport> t = RR_MAKE_SHARED<TcpTransport>();
    t->EnableNodeDiscoveryListening();
    RobotRaconteurNode::s()->RegisterTransport(t);

    RR_SHARED_PTR<LocalTransport> c2 = RR_MAKE_SHARED<LocalTransport>();
    RobotRaconteurNode::s()->RegisterTransport(c2);

    RR_SHARED_PTR<HardwareTransport> c5 = RR_MAKE_SHARED<HardwareTransport>();
    RobotRaconteurNode::s()->RegisterTransport(c5);

    boost::this_thread::sleep(boost::posix_time::seconds(6));

    vector<ServiceInfo2> r = RobotRaconteurNode::s()->FindServiceByType(servicetype, schemes);

    for (vector<ServiceInfo2>::iterator e = r.begin(); e != r.end(); e++)
    {
        cout << "Name: " << e->Name << endl;
        cout << "RootObjectType: " << e->RootObjectType << endl;
        cout << "RootObjectImplements: " << boost::join(e->RootObjectImplements, ", ") << endl;
        cout << "ConnectionURL: " << boost::join(e->ConnectionURL, ", ") << endl;

        vector<string> a;
        for (std::map<std::string, RR_INTRUSIVE_PTR<RRValue> >::iterator ee = e->Attributes.begin();
                ee != e->Attributes.end(); ee++)
        {
            RR_INTRUSIVE_PTR<RRArray<char> > d = RR_DYNAMIC_POINTER_CAST<RRArray<char> >(ee->second);
            if (d)
            {
                a.push_back(ee->first + "=" + RRArrayToString(d));
            }

            RR_INTRUSIVE_PTR<RRArray<int32_t> > d2 = RR_DYNAMIC_POINTER_CAST<RRArray<int32_t> >(ee->second);
            if (d2)
            {
                if (d2->size() > 0)
                {
                    a.push_back(ee->first + "=" + boost::lexical_cast<string>((*d2)[0]));
                }
            }
        }

        cout << "Attributes: " + boost::join(a, ", ") << endl;
        cout << "NodeID: " << e->NodeID.ToString() << endl;
        cout << "NodeName: " << e->NodeName << endl;
        cout << endl;
    }

    RobotRaconteurNode::s()->Shutdown();

    return 0;
}