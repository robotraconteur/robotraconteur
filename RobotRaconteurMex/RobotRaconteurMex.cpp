// Copyright 2011-2020 Wason Technology, LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "RobotRaconteurMex.h"
#include <boost/algorithm/string.hpp>
#include <RobotRaconteur/RobotRaconteurServiceIndex_stubskel.h>

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>

#include <boost/locale.hpp>
#include <boost/range/numeric.hpp>

// NOLINTBEGIN(cppcoreguidelines-pro-type-cstyle-cast,cppcoreguidelines-avoid-c-arrays)

bool isinit = false;

void rrAtExit()
{

    if (isinit)
    {
        {
            boost::recursive_mutex::scoped_lock lock(stubs_lock);
            stubs.clear();
        }
        try
        {
            RobotRaconteurNode::s()->Shutdown();
            RobotRaconteurNode::s()->ReleaseThreadPool();
        }
        catch (std::exception&)
        {}
        catch (...)
        {}
        // mexPrintf("Unload!\n");
        // boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
    }
}

static std::vector<mxArray*> rr_mxDestroyArray_vec;
static boost::mutex rr_mxDestroyArray_lock;

void rr_mxDestroyArray(mxArray* mx)
{
    boost::mutex::scoped_lock lock(rr_mxDestroyArray_lock);
    rr_mxDestroyArray_vec.push_back(mx);
}

void rr_free_mxDestroyArray()
{
    boost::mutex::scoped_lock lock(rr_mxDestroyArray_lock);
    BOOST_FOREACH (mxArray* mx, rr_mxDestroyArray_vec)
    {
        mxDestroyArray(mx);
    }
    rr_mxDestroyArray_vec.clear();
}

std::string mxToString(const mxArray* str)
{
    if (!mxIsChar(str))
    {
        if (mxIsClass(str, "string"))
        {
            throw InvalidArgumentException("New style MATLAB strings are not supported by Robot Raconteur");
        }
        throw InvalidArgumentException("Character array (old-style MATLAB string) expected");
    }
    char* str1 = mxArrayToString(str);
    std::string str2(str1);
    mxFree(str1);
    return str2;
}

void mxToVectorString(const mxArray* in, std::vector<std::string>& vec, const std::string& error_message)
{
    if (!in)
        return;

    if (mxIsChar(in))
    {
        vec.push_back(mxToString(in));
        return;
    }

    if (mxIsCell(in))
    {
        size_t c = mxGetNumberOfElements(in);
        for (size_t i = 0; i < c; i++)
        {
            vec.push_back(mxToString(mxGetCell(in, i)));
        }
        return;
    }

    throw DataTypeException(error_message);
}

int32_t GetInt32Scalar(const mxArray* arr)
{
    if (!mxIsInt32(arr))
        throw DataTypeException("Scalar must be int32");

    if (mxGetNumberOfElements(arr) != 1)
        throw DataTypeException("Expected scalar");
    return ((int32_t*)mxGetData(arr))[0];
}

mxArray* CreateInt32Scalar(const int32_t value)
{
    mxArray* ret = ::mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    ((int32_t*)mxGetData(ret))[0] = value;
    return ret;
}

double GetDoubleScalar(const mxArray* arr)
{
    if (!mxIsDouble(arr))
        throw DataTypeException("Scalar must be double");

    if (mxGetNumberOfElements(arr) != 1)
        throw DataTypeException("Expected scalar");
    return ((double*)mxGetData(arr))[0];
}

mxLogical GetLogicalScalar(const mxArray* arr)
{
    if (!mxIsLogical(arr))
        throw DataTypeException("Scalar must be logical");

    if (mxGetNumberOfElements(arr) != 1)
        throw DataTypeException("Expected scalar");
    return ((mxLogical*)mxGetData(arr))[0];
}

mxArray* CreateDoubleScalar(const double value)
{
    mxArray* ret = ::mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
    ((double*)mxGetData(ret))[0] = value;
    return ret;
}

int32_t mxToTimeout(const mxArray* val)
{
    if (mxIsDouble(val))
    {
        if (::mxGetNumberOfElements(val) != 1)
            throw InvalidArgumentException("Timeout must be a scalar");
        double p1 = ((double*)mxGetData(val))[0] * 1000;
        return boost::lexical_cast<int32_t>(p1);
    }
    else if (mxIsInt32(val))
    {
        if (::mxGetNumberOfElements(val) != 1)
            throw InvalidArgumentException("Timeout must be a scalar");
        int32_t p1 = ((int32_t*)mxGetData(val))[0] * 1000;
        return p1;
    }
    else
    {
        throw InvalidArgumentException("Timeout must be double or int32 scalar");
    }
}

int32_t mxToTimeoutAdjusted(const mxArray* val)
{
    if (mxIsDouble(val))
    {
        if (::mxGetNumberOfElements(val) != 1)
            throw InvalidArgumentException("Timeout must be a scalar");
        double p1 = ((double*)mxGetData(val))[0];
        if (p1 < 0)
            return -1;
        return boost::lexical_cast<int32_t>(p1) * 1000;
    }
    else if (mxIsInt32(val))
    {
        if (::mxGetNumberOfElements(val) != 1)
            throw InvalidArgumentException("Timeout must be a scalar");
        int32_t p1 = ((int32_t*)mxGetData(val))[0];
        if (p1 < 0)
            return -1;
        return p1 * 1000;
    }
    else
    {
        throw InvalidArgumentException("Timeout must be double or int32 scalar");
    }
}

static std::string ptimeToString(const boost::posix_time::ptime& now)
{
    std::stringstream o;
    o << boost::gregorian::to_iso_string(now.date()) << "T";
    boost::posix_time::time_duration t = now.time_of_day();
    o << std::setw(2) << std::setfill('0') << t.hours() << std::setw(2) << t.minutes() << std::setw(2) << t.seconds()
      << ".";
    std::stringstream o2;
    o2 << std::setw(boost::posix_time::time_duration::num_fractional_digits()) << std::setfill('0')
       << t.fractional_seconds();
    std::string o3 = o2.str();
    while (o3.size() < 3)
        o3 += "0";
    o << o3.substr(0, 3);
    return o.str();
}

RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > mxArrayToRRMap(const mxArray* in)
{
    boost::shared_ptr<TypeDefinition> tc = boost::make_shared<TypeDefinition>();
    tc->Type = DataTypes_varvalue_t;
    tc->ContainerType = DataTypes_ContainerTypes_map_string;

    RR_INTRUSIVE_PTR<MessageElementNestedElementList> o1 =
        PackMxArrayToMessageElement(in, tc, RR_SHARED_PTR<ServiceStub>())->CastDataToNestedList(DataTypes_dictionary_t);

    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > o = AllocateEmptyRRMap<std::string, RRValue>();

    std::vector<RR_INTRUSIVE_PTR<MessageElement> >& elems = o1->Elements;

    BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, elems)
    {
        o->insert(std::make_pair((e)->ElementName.str().to_string(), RobotRaconteurNode::s()->UnpackVarType(e)));
    }

    return o;
}

// NOLINTNEXTLINE(readability-function-size)
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    try
    {
        if (nrhs < 1)
        {
            throw InvalidArgumentException("RobotRaconteurMex requires at least one argument");
            return;
        }

        if (!isinit)
        {
            mexAtExit(rrAtExit);

            RobotRaconteurNode::s()->SetLogLevelFromEnvVariable();

            RobotRaconteurNode::s()->SetDynamicServiceFactory(boost::make_shared<MexDynamicServiceFactory>());

            boost::shared_ptr<TcpTransport> t = boost::make_shared<TcpTransport>();
            t->EnableNodeDiscoveryListening(IPNodeDiscoveryFlags_NODE_LOCAL | IPNodeDiscoveryFlags_LINK_LOCAL |
                                            IPNodeDiscoveryFlags_SITE_LOCAL);

            RobotRaconteurNode::s()->RegisterTransport(t);

            RR_SHARED_PTR<LocalTransport> t2 = RR_MAKE_SHARED<LocalTransport>();
            RobotRaconteurNode::s()->RegisterTransport(t2);

            RR_SHARED_PTR<HardwareTransport> t4 = RR_MAKE_SHARED<HardwareTransport>();
            RobotRaconteurNode::s()->RegisterTransport(t4);

            tcp_transport = t;
            local_transport = t2;
            usb_transport = t4;

            isinit = true;
        }

        std::string command = mxToString(prhs[0]);

        rr_free_mxDestroyArray();

        if (command == "Shutdown")
        {
            RobotRaconteurNode::s()->Shutdown();
        }

        else if (command == "Connect")
        {
            if (nlhs != 1 || (nrhs != 2 && nrhs != 4))
                throw InvalidArgumentException(
                    "RobotRaconteurMex Connect requires 2 or 4 input and 1 output arguments");
            if (nrhs == 2)
            {
                plhs[0] = ConnectClient(prhs[1], NULL, NULL);
            }
            else
            {
                plhs[0] = ConnectClient(prhs[1], prhs[2], prhs[3]);
            }
            return;
        }
        else if (command == "Disconnect")
        {
            if (nlhs != 0 || nrhs != 3)
                throw InvalidArgumentException("RobotRaconteurMex Disconnect requires 3 input and 0 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                RR_SHARED_PTR<MexServiceStub> s;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    s = e1->second;
                }
                RobotRaconteurNode::s()->DisconnectService(s);
            }
            else
                throw InvalidArgumentException("RobotRaconteur object not a stub");

            rr_free_mxDestroyArray();

            return;
        }
        else if (command == "subsref")
        {
            if (nlhs != 1 || nrhs != 4)
                throw InvalidArgumentException("RobotRaconteurMex subsasgn requires 4 input and 1 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {

                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }
                plhs[0] = o->subsref(prhs[3]);
            }
            else if (stubtype == RR_MEX_PIPE_ENDPOINT)
            {

                boost::shared_ptr<MexPipeEndpoint> o;
                {
                    boost::recursive_mutex::scoped_lock lock(pipeendpoints_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexPipeEndpoint> >::iterator e1 = pipeendpoints.find(stubid);
                    if (e1 == pipeendpoints.end())
                        throw InvalidArgumentException("Cannot find endpoint");
                    o = e1->second;
                }
                plhs[0] = o->subsref(prhs[3]);
            }
            else if (stubtype == RR_MEX_WIRE_CONNECTION)
            {

                boost::shared_ptr<MexWireConnection> o;
                {
                    boost::recursive_mutex::scoped_lock lock(wireconnections_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexWireConnection> >::iterator e1 = wireconnections.find(stubid);
                    if (e1 == wireconnections.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }
                plhs[0] = o->subsref(prhs[3]);
            }
            else if (stubtype == RR_MEX_SERVICEINFO2_SUBSCRIPTION)
            {
                boost::shared_ptr<MexServiceInfo2Subscription> o;
                {
                    boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceInfo2Subscription> >::iterator e1 =
                        serviceinfo2subscriptions.find(stubid);
                    if (e1 == serviceinfo2subscriptions.end())
                        throw InvalidArgumentException("Cannot find ServiceInfo2Subscription");
                    o = e1->second;
                }
                plhs[0] = o->subsref(prhs[3]);
            }
            else if (stubtype == RR_MEX_SERVICE_SUBSCRIPTION)
            {
                boost::shared_ptr<MexServiceSubscription> o;
                {
                    boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceSubscription> >::iterator e1 =
                        servicesubscriptions.find(stubid);
                    if (e1 == servicesubscriptions.end())
                        throw InvalidArgumentException("Cannot find ServiceSubscription");
                    o = e1->second;
                }
                plhs[0] = o->subsref(prhs[3]);
            }
            else if (stubtype == RR_MEX_WIRE_SUBSCRIPTION)
            {
                boost::shared_ptr<MexWireSubscription> o;
                {
                    boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexWireSubscription> >::iterator e1 =
                        wiresubscriptions.find(stubid);
                    if (e1 == wiresubscriptions.end())
                        throw InvalidArgumentException("Cannot find WireSubscription");
                    o = e1->second;
                }
                plhs[0] = o->subsref(prhs[3]);
            }
            else if (stubtype == RR_MEX_PIPE_SUBSCRIPTION)
            {
                boost::shared_ptr<MexPipeSubscription> o;
                {
                    boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexPipeSubscription> >::iterator e1 =
                        pipesubscriptions.find(stubid);
                    if (e1 == pipesubscriptions.end())
                        throw InvalidArgumentException("Cannot find PipeSubscription");
                    o = e1->second;
                }
                plhs[0] = o->subsref(prhs[3]);
            }
            else if (stubtype == RR_MEX_GENERATOR_CLIENT)
            {
                boost::shared_ptr<MexGeneratorClient> o;
                {
                    boost::mutex::scoped_lock lock(generators_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexGeneratorClient> >::iterator e1 = generators.find(stubid);
                    if (e1 == generators.end())
                    {
                        // Return "false" if TryNext was called
                        if (MexGeneratorClient::GetFunctionName(prhs[3]) == "TryNext")
                        {
                            mxArray* try_next_ret = mxCreateCellMatrix(1, 2);
                            mxSetCell(try_next_ret, 0, mxCreateLogicalScalar(false));
                            mxSetCell(try_next_ret, 1, mxCreateDoubleScalar(0));
                            plhs[0] = try_next_ret;
                            return;
                        }
                        throw InvalidArgumentException("Cannot find generator");
                    }
                    o = e1->second;
                }
                plhs[0] = o->subsref(prhs[3]);
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "subsasgn")
        {
            if (nlhs != 0 || nrhs != 5)
                throw InvalidArgumentException("RobotRaconteurMex PropertySet requires 5 input and 0 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }
                o->subsasgn(prhs[3], prhs[4]);
            }
            else if (stubtype == RR_MEX_PIPE_ENDPOINT)
            {

                boost::shared_ptr<MexPipeEndpoint> o;
                {
                    boost::recursive_mutex::scoped_lock lock(pipeendpoints_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexPipeEndpoint> >::iterator e1 = pipeendpoints.find(stubid);
                    if (e1 == pipeendpoints.end())
                        throw InvalidArgumentException("Cannot find pipe endpoint");
                    o = e1->second;
                }
                o->subsasgn(prhs[3], prhs[4]);
            }
            else if (stubtype == RR_MEX_WIRE_CONNECTION)
            {

                boost::shared_ptr<MexWireConnection> o;
                {
                    boost::recursive_mutex::scoped_lock lock(wireconnections_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexWireConnection> >::iterator e1 = wireconnections.find(stubid);
                    if (e1 == wireconnections.end())
                        throw InvalidArgumentException("Cannot find wire connection");
                    o = e1->second;
                }
                o->subsasgn(prhs[3], prhs[4]);
            }
            else if (stubtype == RR_MEX_SERVICEINFO2_SUBSCRIPTION)
            {
                boost::shared_ptr<MexServiceInfo2Subscription> o;
                {
                    boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceInfo2Subscription> >::iterator e1 =
                        serviceinfo2subscriptions.find(stubid);
                    if (e1 == serviceinfo2subscriptions.end())
                        throw InvalidArgumentException("Cannot find ServiceInfo2Subscription");
                    o = e1->second;
                }
                o->subsasgn(prhs[3], prhs[4]);
            }
            else if (stubtype == RR_MEX_SERVICE_SUBSCRIPTION)
            {
                boost::shared_ptr<MexServiceSubscription> o;
                {
                    boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceSubscription> >::iterator e1 =
                        servicesubscriptions.find(stubid);
                    if (e1 == servicesubscriptions.end())
                        throw InvalidArgumentException("Cannot find ServiceSubscription");
                    o = e1->second;
                }
                o->subsasgn(prhs[3], prhs[4]);
            }
            else if (stubtype == RR_MEX_WIRE_SUBSCRIPTION)
            {
                boost::shared_ptr<MexWireSubscription> o;
                {
                    boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexWireSubscription> >::iterator e1 =
                        wiresubscriptions.find(stubid);
                    if (e1 == wiresubscriptions.end())
                        throw InvalidArgumentException("Cannot find WireSubscription");
                    o = e1->second;
                }
                o->subsasgn(prhs[3], prhs[4]);
            }
            else if (stubtype == RR_MEX_PIPE_SUBSCRIPTION)
            {
                boost::shared_ptr<MexPipeSubscription> o;
                {
                    boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexPipeSubscription> >::iterator e1 =
                        pipesubscriptions.find(stubid);
                    if (e1 == pipesubscriptions.end())
                        throw InvalidArgumentException("Cannot find PipeSubscription");
                    o = e1->second;
                }
                o->subsasgn(prhs[3], prhs[4]);
            }
            else if (stubtype == RR_MEX_GENERATOR_CLIENT)
            {
                boost::shared_ptr<MexGeneratorClient> o;
                {
                    boost::mutex::scoped_lock lock(generators_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexGeneratorClient> >::iterator e1 = generators.find(stubid);
                    if (e1 == generators.end())
                        throw InvalidArgumentException("Cannot find GeneratorClient");
                    o = e1->second;
                }
                o->subsasgn(prhs[3], prhs[4]);
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "addlistener")
        {
            if (nlhs != 1 || nrhs != 5)
                throw InvalidArgumentException("RobotRaconteurMex addlistener requires 5 input and 1 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }
                plhs[0] = o->addlistener(prhs[3], prhs[4]);
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "deletelistener")
        {
            if (nlhs != 0 || nrhs != 4)
                throw InvalidArgumentException(
                    "RobotRaconteurMex deletelistener requires 4 input and 0 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }
                o->deletelistener(prhs[3]);
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "enableevents")
        {
            if (nlhs != 0 || nrhs != 3)
                throw InvalidArgumentException(
                    "RobotRaconteurMex enableevents requires 3 input and 0 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }
                o->EnableEvents = true;
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "disableevents")
        {
            if (nlhs != 0 || nrhs != 3)
                throw InvalidArgumentException(
                    "RobotRaconteurMex enableevents requires 3 input and 0 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }
                o->EnableEvents = false;
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "members")
        {
            if (nlhs != 1 || nrhs != 3)
                throw InvalidArgumentException("RobotRaconteurMex members requires 3 input and 1 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }
                std::string mem((o->RR_objecttype->ToString() + "\n"));
                plhs[0] = mxCreateString(mem.c_str());
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "type")
        {
            if (nlhs != 1 || nrhs != 3)
                throw InvalidArgumentException("RobotRaconteurMex type requires 3 input and 1 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }

                std::string type = o->RR_objecttype->ServiceDefinition_.lock()->Name + "." + o->RR_objecttype->Name;
                plhs[0] = mxCreateString(type.c_str());
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "servicedef")
        {
            if (nlhs != 1 || nrhs != 3)
                throw InvalidArgumentException("RobotRaconteurMex servicedef requires 3 input and 1 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }
                RR_SHARED_PTR<ServiceFactory> factory =
                    RobotRaconteurNode::s()->GetPulledServiceType(o, o->RR_objecttype->ServiceDefinition_.lock()->Name);
                std::string mem(factory->DefString());
                plhs[0] = mxCreateString(mem.c_str());
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "GetPulledServiceTypes")
        {
            if (nlhs != 1 || nrhs != 3)
                throw InvalidArgumentException(
                    "RobotRaconteurMex GetPulledServiceTypes requires 3 input and 1 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }

                std::vector<std::string> types = RobotRaconteurNode::s()->GetPulledServiceTypes(o);
                mxArray* o2 = mxCreateCellMatrix(types.size(), 1);
                for (size_t i = 0; i < types.size(); i++)
                {
                    mxSetCell(o2, i, mxCreateString(types.at(i).c_str()));
                }
                plhs[0] = o2;
                // std::string type=o->RR_objecttype->ServiceDefinition_.lock()->Name + "." + o->RR_objecttype->Name;
                // plhs[0]= mxCreateString(type.c_str());
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "GetPulledServiceType")
        {
            if (nlhs != 1 || nrhs != 4)
                throw InvalidArgumentException(
                    "RobotRaconteurMex GetPulledServiceType requires 3 input and 1 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }

                std::string type = mxToString(prhs[3]);

                std::string def = RobotRaconteurNode::s()->GetPulledServiceType(o, type)->DefString();
                mxArray* o2 = mxCreateString(def.c_str());
                plhs[0] = o2;
                // std::string type=o->RR_objecttype->ServiceDefinition_.lock()->Name + "." + o->RR_objecttype->Name;
                // plhs[0]= mxCreateString(type.c_str());
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "ProcessRequests")
        {

            std::map<int32_t, boost::shared_ptr<MexServiceStub> > stubs2;

            {
                boost::recursive_mutex::scoped_lock lock(stubs_lock);
                stubs2 = stubs;
            }

            BOOST_FOREACH (boost::shared_ptr<MexServiceStub>& e, stubs2 | boost::adaptors::map_values)
            {
                e->MexProcessRequests();
            }

            rr_free_mxDestroyArray();

            return;
        }
        else if (command == "MemoryOp")
        {
            if (nlhs != 1 || !(nrhs >= 5 && nrhs <= 7))
                throw InvalidArgumentException(
                    "RobotRaconteurMex MemoryOp requires 5 to 7 input and 1 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }

                const mxArray* data = NULL;
                const mxArray* S = NULL;
                if (nrhs >= 6)
                {
                    S = prhs[5];
                }
                if (nrhs >= 7)
                {
                    data = prhs[6];
                }

                plhs[0] = o->MemoryOp(prhs[3], prhs[4], S, data);
            }

            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }

        if (command == "LockOp")
        {
            if (nlhs != 0 || nrhs != 4)
                throw InvalidArgumentException("RobotRaconteurMex LockOp requires 4 input and 0 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }

                o->LockOp(prhs[3]);
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "FindService")
        {
            if (nlhs != 1 || nrhs != 2)
                throw InvalidArgumentException("RobotRaconteurMex FindService requires 2 input and 1 output arguments");
            plhs[0] = FindService(prhs[1]);
        }
        else if (command == "FindNodeByID")
        {
            if (nlhs != 1 || nrhs != 2)
                throw InvalidArgumentException(
                    "RobotRaconteurMex FindNodeByID requires 2 input and 1 output arguments");
            plhs[0] = FindNodeByID(prhs[1]);
        }
        else if (command == "FindNodeByName")
        {
            if (nlhs != 1 || nrhs != 2)
                throw InvalidArgumentException(
                    "RobotRaconteurMex FindNadeByName requires 2 input and 1 output arguments");
            plhs[0] = FindNodeByName(prhs[1]);
        }
        else if (command == "UpdateDetectedNodes")
        {
            if (nlhs != 0 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex UpdateDetectedNodes requires 1 input and 0 output arguments");
            UpdateDetectedNodes();
        }
        else if (command == "GetDetectedNodes")
        {
            if (nlhs != 1 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex GetDetectedNodes requires 1 input and 1 output arguments");
            plhs[0] = GetDetectedNodes();
        }
        else if (command == "constants")
        {
            if (nlhs != 1 || nrhs != 3)
                throw InvalidArgumentException("RobotRaconteurMex constants requires 3 input and 1 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }

                plhs[0] = ServiceDefinitionConstants(o->RR_objecttype->ServiceDefinition_.lock());
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur object type");
            return;
        }
        else if (command == "NowUTC")
        {
            if (nlhs != 1 || nrhs != 1)
                throw InvalidArgumentException("RobotRaconteurMex NowUTC requires 1 input and 1 output arguments");

            boost::posix_time::ptime now = RobotRaconteur::RobotRaconteurNode::s()->NowUTC();
            std::string now_str = ptimeToString(now);
            plhs[0] = mxCreateString(now_str.c_str());
            return;
        }
        else if (command == "NowNodeTime")
        {
            if (nlhs != 1 || nrhs != 1)
                throw InvalidArgumentException("RobotRaconteurMex NowNodeTime requires 1 input and 1 output arguments");

            boost::posix_time::ptime now = RobotRaconteur::RobotRaconteurNode::s()->NowNodeTime();
            std::string now_str = ptimeToString(now);
            plhs[0] = mxCreateString(now_str.c_str());
            return;
        }
        else if (command == "NodeSyncTimeUTC")
        {
            if (nlhs != 1 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex NodeSyncTimeUTC requires 1 input and 1 output arguments");

            boost::posix_time::ptime now = RobotRaconteur::RobotRaconteurNode::s()->NodeSyncTimeUTC();
            std::string now_str = ptimeToString(now);
            plhs[0] = mxCreateString(now_str.c_str());
            return;
        }
        else if (command == "Sleep")
        {
            if (nlhs != 0 || nrhs != 2)
                throw InvalidArgumentException("RobotRaconteurMex Sleep requires 2 input and 0 output arguments");
            if (!mxIsDouble(prhs[1]))
                throw InvalidArgumentException("Sleep duration must be scalar double");
            if (mxGetNumberOfElements(prhs[1]) != 1)
                throw InvalidArgumentException("Sleep duration must be scalar double");
            double duration = ((double*)(mxGetData(prhs[1])))[0];
            double duration2 = duration * 1000000;
            RobotRaconteurNode::s()->Sleep(boost::posix_time::microseconds((long)duration2));
            return;
        }
        else if (command == "CreateRate")
        {
            if (nlhs != 1 || nrhs != 2)
                throw InvalidArgumentException("RobotRaconteurMex CreateRate requires 2 input and 1 output arguments");
            if (!mxIsDouble(prhs[1]))
                throw InvalidArgumentException("Frequency must be scalar double");
            if (mxGetNumberOfElements(prhs[1]) != 1)
                throw InvalidArgumentException("Frequency must be scalar double");
            double rate = ((double*)(mxGetData(prhs[1])))[0];

            boost::recursive_mutex::scoped_lock lock(rate_lock);
            do
            {
                rate_count++;
                if (rate_count >= std::numeric_limits<int32_t>::max())
                    rate_count = 0;
            } while (rates.find(rate_count) != rates.end());

            RR_SHARED_PTR<RobotRaconteur::Rate> r = RobotRaconteur::RobotRaconteurNode::s()->CreateRate(rate);
            rates.insert(std::make_pair(rate_count, r));
            mxArray* ret = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
            ((int32_t*)(mxGetData(prhs[1])))[0] = rate_count;
            plhs[0] = ret;
            return;
        }
        else if (command == "SleepRate")
        {
            if (nlhs != 0 || nrhs != 2)
                throw InvalidArgumentException("RobotRaconteurMex SleepRate requires 2 input and 0 output arguments");
            if (!mxIsInt32(prhs[1]))
                throw InvalidArgumentException("id must be scalar int32");
            if (mxGetNumberOfElements(prhs[1]) != 1)
                throw InvalidArgumentException("id must be scalar int32");
            int32_t id = ((int32_t*)(mxGetData(prhs[1])))[0];

            RR_SHARED_PTR<RobotRaconteur::Rate> r;
            {
                boost::recursive_mutex::scoped_lock lock(rate_lock);
                r = rates.at(id);
            }

            if (!r)
                throw InvalidArgumentException("Rate not found");
            r->Sleep();
            return;
        }
        else if (command == "DeleteRate")
        {
            if (nlhs != 0 || nrhs != 2)
                throw InvalidArgumentException("RobotRaconteurMex SleepRate requires 2 input and 0 output arguments");
            if (!mxIsInt32(prhs[1]))
                throw InvalidArgumentException("id must be scalar int32");
            if (mxGetNumberOfElements(prhs[1]) != 1)
                throw InvalidArgumentException("id must be scalar int32");
            int32_t id = ((int32_t*)(mxGetData(prhs[1])))[0];

            {
                boost::recursive_mutex::scoped_lock lock(rate_lock);
                rates.erase(id);
            }
            return;
        }
        // Commands to enable node server.  Experimental...

        else if (command == "GetServiceType")
        {
            if (nlhs != 1 || nrhs != 2)
                throw InvalidArgumentException(
                    "RobotRaconteurMex GetServiceType requires 2 input and 1 output arguments");
            std::string typen = mxToString(prhs[1]);
            plhs[0] = ::mxCreateString(RobotRaconteurNode::s()->GetServiceType(typen)->DefString().c_str());
            return;
        }
        else if (command == "GetRegisteredServiceTypes")
        {
            if (nlhs != 1 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex GetRegisteredServiceTypes requires 1 input and 1 output arguments");
            std::vector<std::string> o = RobotRaconteurNode::s()->GetRegisteredServiceTypes();
            mxArray* mo = mxCreateCellMatrix(o.size(), 1);
            for (size_t i = 0; i < o.size(); i++)
            {
                ::mxSetCell(mo, i, mxCreateString(o[i].c_str()));
            }
            prhs[0] = mo;
            return;
        }
        else if (command == "RegisterServiceType")
        {
            if (nlhs != 0 || nrhs != 2)
                throw InvalidArgumentException(
                    "RobotRaconteurMex RegisterServiceType requires 2 input and 1 output arguments");
            std::string defstr = mxToString(prhs[1]);
            boost::shared_ptr<MexServiceFactory> m = boost::make_shared<MexServiceFactory>(defstr);

            std::vector<boost::shared_ptr<ServiceDefinition> > defs;
            std::vector<std::string> names = RobotRaconteurNode::s()->GetRegisteredServiceTypes();
            BOOST_FOREACH (std::string& e, names)
            {
                if (e != "RobotRaconteurServiceIndex")
                    defs.push_back(RobotRaconteurNode::s()->GetServiceType(e)->ServiceDef());
            }
            defs.push_back(m->ServiceDef());

            VerifyServiceDefinitions(defs);

            RobotRaconteurNode::s()->RegisterServiceType(m);
            return;
        }
        else if (command == "StartLocalServer")
        {
            if (nlhs != 0 || (nrhs != 2))
                throw InvalidArgumentException(
                    "RobotRaconteurMex StartLocalServer requires 2  input and 0 output arguments");
            std::string id = mxToString(prhs[1]);

            boost::shared_ptr<LocalTransport> t2 = local_transport.lock();

            if (!t2)
                throw InvalidArgumentException("Internal error, could not start server");

            if (id.find('{') != std::string::npos)
            {
                t2->StartServerAsNodeID(NodeID(id));
            }
            else
            {
                t2->StartServerAsNodeName(id);
            }

            return;
        }
        else if (command == "StartLocalClient")
        {
            if (nlhs != 0 || (nrhs != 2))
                throw InvalidArgumentException(
                    "RobotRaconteurMex StartLocalClient requires 2  input and 0 output arguments");
            std::string id = mxToString(prhs[1]);

            boost::shared_ptr<LocalTransport> t2 = local_transport.lock();

            if (!t2)
                throw InvalidArgumentException("Internal error, could not start server");

            if (id.find('{') != std::string::npos)
            {
                RobotRaconteurNode::s()->SetNodeID(NodeID(id));
            }
            else
            {
                t2->StartClientAsNodeName(id);
            }

            return;
        }
        else if (command == "StartTcpServer")
        {
            if (nlhs != 0 || (nrhs != 2))
                throw InvalidArgumentException(
                    "RobotRaconteurMex StartTcpServer requires 2 or 3 input and 0 output arguments");

            boost::shared_ptr<TcpTransport> t = tcp_transport.lock();

            if (!t)
                throw InvalidArgumentException("Internal error, could not start server");

            int32_t port = 0;
            if (mxIsDouble(prhs[1]))
            {
                if (::mxGetNumberOfElements(prhs[1]) != 1)
                    throw InvalidArgumentException("Port must be a scalar");
                double p1 = ((double*)mxGetData(prhs[1]))[0];
                port = boost::lexical_cast<int32_t>(p1);
            }
            else if (mxIsInt32(prhs[1]))
            {
                if (::mxGetNumberOfElements(prhs[1]) != 1)
                    throw InvalidArgumentException("Port must be a scalar");
                int32_t p1 = ((int32_t*)mxGetData(prhs[1]))[0];
                port = p1;
            }
            else if (mxIsChar(prhs[1]))
            {
                std::string p1 = mxToString(prhs[1]);
                try
                {
                    port = boost::lexical_cast<int32_t>(p1);
                }
                catch (std::exception&)
                {
                    throw InvalidArgumentException("Could not parse port number");
                }
            }
            else
            {
                throw InvalidArgumentException("Port must be int32, double, or string");
            }

            t->StartServer(port);

            if (port == 0)
            {
                mexPrintf("Server started on port: %i", t->GetListenPort());
            }

            return;
        }
        else if (command == "GetTcpListenPort")
        {
            if (nlhs != 1 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex GetTcpListenPort requires 1 input and 1 output arguments");
            boost::shared_ptr<TcpTransport> t = tcp_transport.lock();
            if (!t)
                throw InvalidOperationException("TcpTransport not running");
            mxArray* o = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
            ((int32_t*)mxGetData(o))[0] = t->GetListenPort();
            plhs[0] = o;
            return;
        }
        else if (command == "StartTcpServerUsingPortSharer")
        {
            if (nlhs != 0 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex StartTcpServerUsingPortSharer requires 1 input and 0 output arguments");
            boost::shared_ptr<TcpTransport> t = tcp_transport.lock();
            if (!t)
                throw InvalidOperationException("TcpTransport not running");
            t->StartServerUsingPortSharer();
            return;
        }
        else if (command == "IsTcpPortSharerRunning")
        {
            if (nlhs != 1 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex IsTcpPortSharerRunning requires 1 input and 1 output arguments");
            boost::shared_ptr<TcpTransport> t = tcp_transport.lock();
            if (!t)
                throw InvalidOperationException("TcpTransport not running");
            mxArray* o = mxCreateNumericMatrix(1, 1, mxLOGICAL_CLASS, mxREAL);
            ((mxLogical*)mxGetData(o))[0] = t->IsPortSharerRunning();
            plhs[0] = o;
            return;
        }
        else if (command == "LoadTlsNodeCertificate")
        {
            if (nlhs != 0 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex LoadTlsNodeCertificate requires 1 input and 0 output arguments");
            boost::shared_ptr<TcpTransport> t = tcp_transport.lock();
            if (!t)
                throw InvalidOperationException("TcpTransport not running");
            t->LoadTlsNodeCertificate();
            return;
        }
        else if (command == "IsTlsNodeCertificateLoaded")
        {
            if (nlhs != 1 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex IsTlsNodeCertificateLoaded requires 1 input and 1 output arguments");
            boost::shared_ptr<TcpTransport> t = tcp_transport.lock();
            if (!t)
                throw InvalidOperationException("TcpTransport not running");
            mxArray* o = mxCreateNumericMatrix(1, 1, mxLOGICAL_CLASS, mxREAL);
            ((mxLogical*)mxGetData(o))[0] = t->IsTlsNodeCertificateLoaded();
            plhs[0] = o;
            return;
        }
        else if (command == "RegisterService")
        {
            if (nlhs != 0 || (nrhs != 4 && nrhs != 5))
                throw InvalidArgumentException(
                    "RobotRaconteurMex RegisterService requires 4 or 5 input and 0 output arguments");
            std::string name = mxToString(prhs[1]);
            std::string type = mxToString(prhs[2]);
            boost::shared_ptr<mxArray> mxobj = boost::shared_ptr<mxArray>(mxDuplicateArray(prhs[3]), rr_mxDestroyArray);
            mexMakeArrayPersistent(mxobj.get());

            boost::tuple<boost::string_ref, boost::string_ref> s1 = SplitQualifiedName(type);
            boost::shared_ptr<ServiceDefinition> def =
                RobotRaconteurNode::s()->GetServiceType(s1.get<0>())->ServiceDef();
            boost::shared_ptr<ServiceEntryDefinition> type1;
            BOOST_FOREACH (boost::shared_ptr<ServiceEntryDefinition>& e, def->Objects)
            {
                if (e->Name == s1.get<1>())
                {
                    type1 = e;
                    break;
                }
            }

            if (!type1)
                throw ServiceException("Unknown service type: " + type);

            BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, type1->Members)
            {
                if (boost::dynamic_pointer_cast<PropertyDefinition>(e) == 0 &&
                    boost::dynamic_pointer_cast<FunctionDefinition>(e) == 0 &&
                    boost::dynamic_pointer_cast<EventDefinition>(e) == 0)
                    throw ServiceException(
                        "MATLAB service objects can only have members of type property, function, and event");
            }

            boost::shared_ptr<MexRRObject> o = boost::make_shared<MexRRObject>();
            o->mxobj = mxobj;
            o->type = type1;
            o->strtype = type;

            if (nrhs == 4)
            {
                RobotRaconteurNode::s()->RegisterService(name, s1.get<0>(), o);
            }
            else
            {
                std::string authdata = mxToString(prhs[4]);

                RR_SHARED_PTR<PasswordFileUserAuthenticator> p =
                    RR_MAKE_SHARED<PasswordFileUserAuthenticator>(authdata);
                std::map<std::string, std::string> policies;
                policies.insert(std::make_pair("requirevaliduser", "true"));
                policies.insert(std::make_pair("allowobjectlock", "true"));

                RR_SHARED_PTR<ServiceSecurityPolicy> s = RR_MAKE_SHARED<ServiceSecurityPolicy>(p, policies);

                RobotRaconteurNode::s()->RegisterService(name, s1.get<0>(), o, s);
            }

            return;
        }
        else if (command == "CloseService")
        {
            if (nlhs != 0 || nrhs != 2)
                throw InvalidArgumentException(
                    "RobotRaconteurMex CloseService requires 2 input and 0 output arguments");
            std::string name = mxToString(prhs[1]);
            RobotRaconteurNode::s()->CloseService(name);
            return;
        }
        else if (command == "DispatchSkelEvent")
        {
            if (nlhs != 0 || nrhs != 4)
                throw InvalidArgumentException(
                    "RobotRaconteurMex DispatchSkelEvent requires 4 input and 0 output arguments");

            int32_t skelid = GetInt32Scalar(prhs[1]);
            std::string name = mxToString(prhs[2]);

            boost::shared_ptr<MexServiceSkel> skel;

            {
                boost::mutex::scoped_lock lock(skels_lock);
                skel = skels.at(skelid).lock();
            }

            if (!skel)
                return;

            skel->DispatchMexEvent(name, prhs[3]);

            return;
        }

        else if (command == "ProcessServerRequests")
        {
            if (nlhs != 0 || nrhs != 2)
                throw InvalidArgumentException(
                    "RobotRaconteurMex ProcessServerRequests requires 2 input and 0 output arguments");
            int32_t timeout = mxToTimeout(prhs[1]);

            skels_waiting.WaitOne(timeout);

            std::list<boost::weak_ptr<MexServiceSkel> > skels2;

            {
                boost::mutex::scoped_lock lock(skels_lock);
                for (std::map<int, boost::weak_ptr<MexServiceSkel> >::iterator e = skels.begin(); e != skels.end();)
                {
                    boost::shared_ptr<MexServiceSkel> temp = e->second.lock();
                    if (temp)
                    {
                        skels2.push_back(e->second);
                        e++;
                    }
                    else
                    {
                        skels.erase(e++);
                    }
                }
            }

            for (std::list<boost::weak_ptr<MexServiceSkel> >::const_iterator e = skels2.begin(); e != skels2.end();)
            {
                boost::shared_ptr<MexServiceSkel> skel3 = e->lock();
                /*if (!skel3)
                {
                    e=skels2.erase(e);
                }
                else*/
                if (skel3)
                {
                    skel3->ProcessRequests();
                    e++;
                }
                else
                {
                    e++;
                }
            }
        }
        else if (command == "SubscribeServiceInfo2")
        {
            if (nlhs != 1 || (nrhs != 2 && nrhs != 3))
                throw InvalidArgumentException(
                    "RobotRaconteurMex SubscribeServiceInfo2 requires 2 or 3 input and 1 output arguments");
            switch (nrhs)
            {
            case 2:
                plhs[0] = SubscribeServiceInfo2(prhs[1], NULL);
                return;
            case 3:
                plhs[0] = SubscribeServiceInfo2(prhs[1], prhs[2]);
                return;
            default:
                return;
            }
        }
        else if (command == "SubscribeServiceByType")
        {
            if (nlhs != 1 || (nrhs != 2 && nrhs != 3))
                throw InvalidArgumentException(
                    "RobotRaconteurMex SubscribeServiceByType requires 2 or 3 input and 1 output arguments");
            switch (nrhs)
            {
            case 2:
                plhs[0] = SubscribeServiceByType(prhs[1], NULL);
                return;
            case 3:
                plhs[0] = SubscribeServiceByType(prhs[1], prhs[2]);
                return;
            default:
                return;
            }
        }
        else if (command == "SubscribeService")
        {
            if (nlhs != 1 || (nrhs != 2 && nrhs != 4))
                throw InvalidArgumentException(
                    "RobotRaconteurMex SubscribeService requires 2 or 4 input and 1 output arguments");
            if (nrhs == 2)
            {
                plhs[0] = SubscribeService(prhs[1], NULL, NULL);
            }
            else
            {
                plhs[0] = SubscribeService(prhs[1], prhs[2], prhs[3]);
            }
            return;
        }

        // Timeout adjustment

        else if (command == "GetTransportTimeout")
        {
            if (nlhs != 1 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex GetTransportTimeout requires 2 input and 0 output arguments");
            boost::shared_ptr<TcpTransport> t = tcp_transport.lock();

            if (!t)
                throw InternalErrorException("Internal error, could not contact transport");

            double timeout = boost::lexical_cast<double>(t->GetDefaultReceiveTimeout()) / 1000;
            mxArray* ret = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
            ((double*)mxGetData(ret))[0] = timeout;
            plhs[0] = ret;
        }

        else if (command == "SetTransportTimeout")
        {
            if (nlhs != 0 || nrhs != 2)
                throw InvalidArgumentException("RobotRaconteurMex SetTimeout requires 2 input and 0 output arguments");
            int32_t timeout = mxToTimeout(prhs[1]);

            boost::shared_ptr<TcpTransport> t = tcp_transport.lock();

            if (!t)
                throw InternalErrorException("Internal error, could not contact transport");
            t->SetDefaultReceiveTimeout(timeout);
            return;
        }

        else if (command == "GetRequestTimeout")
        {
            if (nlhs != 1 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex GetTransportTimeout requires 2 input and 0 output arguments");
            boost::shared_ptr<TcpTransport> t = tcp_transport.lock();

            if (!t)
                throw InternalErrorException("Internal error, could not contact transport");

            double timeout = boost::lexical_cast<double>(RobotRaconteurNode::s()->GetRequestTimeout()) / 1000;
            mxArray* ret = mxCreateNumericMatrix(1, 1, mxDOUBLE_CLASS, mxREAL);
            ((double*)mxGetData(ret))[0] = timeout;
            plhs[0] = ret;
        }
        else if (command == "SetRequestTimeout")
        {
            if (nlhs != 0 || nrhs != 2)
                throw InvalidArgumentException("RobotRaconteurMex SetTimeout requires 2 input and 0 output arguments");
            int32_t timeout = mxToTimeout(prhs[1]);

            RobotRaconteurNode::s()->SetRequestTimeout(timeout);
            return;
        }

        // Logging
        else if (command == "GetLogLevel")
        {
            if (nlhs != 1 || nrhs != 1)
                throw InvalidArgumentException("RobotRaconteurMex GetLogLevel requires 1 input and 1 output arguments");
            std::string level;
            RobotRaconteur_LogLevel l = RobotRaconteurNode::s()->GetLogLevel();
            switch (l)
            {
            case RobotRaconteur_LogLevel_Disable:
                level = "DISABLE";
                break;
            case RobotRaconteur_LogLevel_Fatal:
                level = "FATAL";
                break;
            case RobotRaconteur_LogLevel_Error:
                level = "ERROR";
                break;
            case RobotRaconteur_LogLevel_Warning:
                level = "WARNING";
                break;
            case RobotRaconteur_LogLevel_Info:
                level = "INFO";
                break;
            case RobotRaconteur_LogLevel_Debug:
                level = "DEBUG";
                break;
            case RobotRaconteur_LogLevel_Trace:
                level = "TRACE";
                break;
            default:
                level = "DISABLE";
                break;
            }

            plhs[0] = mxCreateString(level.c_str());
            return;
        }

        else if (command == "SetLogLevel")
        {
            if (nlhs != 0 || nrhs != 2)
                throw InvalidArgumentException("RobotRaconteurMex SetLogLevel requires 2 input and 0 output arguments");
            std::string level = mxToString(prhs[1]);
            boost::to_lower(level);
            if (level == "disable")
            {
                RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Disable);
            }
            else if (level == "fatal")
            {
                RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Fatal);
            }
            else if (level == "error")
            {
                RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Error);
            }
            else if (level == "warning")
            {
                RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Warning);
            }
            else if (level == "info")
            {
                RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Info);
            }
            else if (level == "debug")
            {
                RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Debug);
            }
            else if (level == "trace")
            {
                RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Trace);
            }
            else
            {
                throw InvalidArgumentException("Invalid log level: " + level);
            }
            return;
        }
        else if (command == "OpenLogFile")
        {
            if (nlhs != 0 || nrhs != 2)
                throw InvalidArgumentException("RobotRaconteurMex OpenLogFile requires 2 input and 0 output arguments");
            std::string fname = mxToString(prhs[1]);
            RR_SHARED_PTR<FileLogRecordHandler> fhandler = RR_MAKE_SHARED<FileLogRecordHandler>();
            fhandler->OpenFile(fname);
            RobotRaconteurNode::s()->SetLogRecordHandler(fhandler);
            return;
        }
        else if (command == "CloseLogFile")
        {
            if (nlhs != 0 || nrhs != 1)
                throw InvalidArgumentException(
                    "RobotRaconteurMex CloseLogFile requires 1 input and 0 output arguments");
            RobotRaconteurNode::s()->SetLogRecordHandler(RR_SHARED_PTR<FileLogRecordHandler>());
            return;
        }
        else if (command == "CreateStructure")
        {
            if (nlhs != 1 || nrhs != 4)
                throw InvalidArgumentException(
                    "RobotRaconteurMex CreateStructure requires 3 input and 1 output arguments");
            int32_t stubtype = GetInt32Scalar(prhs[1]);
            int32_t stubid = GetInt32Scalar(prhs[2]);
            std::string type_str = mxToString(prhs[3]);
            if (stubtype == RR_MEX_STUB)
            {
                boost::shared_ptr<MexServiceStub> o;
                {
                    boost::recursive_mutex::scoped_lock lock(stubs_lock);
                    std::map<int32_t, RR_SHARED_PTR<MexServiceStub> >::iterator e1 = stubs.find(stubid);
                    if (e1 == stubs.end())
                        throw InvalidArgumentException("Cannot find stub");
                    o = e1->second;
                }

                plhs[0] = CreateEmptyStructure(type_str, o);
                return;
            }
            else
                throw InvalidArgumentException("Unknown RobotRaconteur structure type");
            return;
        }
        else
        {
            throw InvalidArgumentException("Unknown command for RobotRaconteurMex");
        }
    }
    catch (RobotRaconteurException& e)
    {
        std::string err = "RobotRaconteurMex Error: " + e.Error + " " + e.Message;
        mexErrMsgTxt(err.c_str());
        return;
    }
    catch (std::exception& e)
    {
        std::string err = "RobotRaconteurMex Error: " + std::string(typeid(e).name()) + " " + std::string(e.what());
        mexErrMsgTxt(err.c_str());
        return;
    }
}

// Data conversion functions

DataTypes mxClassIDToRRDataType(mxClassID type)
{
    switch (type)
    {
    case mxDOUBLE_CLASS:
        return DataTypes_double_t;
    case mxSINGLE_CLASS:
        return DataTypes_single_t;
    case mxINT8_CLASS:
        return DataTypes_int8_t;
    case mxUINT8_CLASS:
        return DataTypes_uint8_t;
    case mxINT16_CLASS:
        return DataTypes_int16_t;
    case mxUINT16_CLASS:
        return DataTypes_uint16_t;
    case mxINT32_CLASS:
        return DataTypes_int32_t;
    case mxUINT32_CLASS:
        return DataTypes_uint32_t;
    case mxINT64_CLASS:
        return DataTypes_int64_t;
    case mxUINT64_CLASS:
        return DataTypes_uint64_t;
    default:
        throw DataTypeException("Not a numeric data type");
    }
}

mxClassID rrDataTypeToMxClassID(DataTypes type)
{
    switch (type)
    {
    case DataTypes_double_t:
        return mxDOUBLE_CLASS;
    case DataTypes_single_t:
        return mxSINGLE_CLASS;
    case DataTypes_int8_t:
        return mxINT8_CLASS;
    case DataTypes_uint8_t:
        return mxUINT8_CLASS;
    case DataTypes_int16_t:
        return mxINT16_CLASS;
    case DataTypes_uint16_t:
        return mxUINT16_CLASS;
    case DataTypes_int32_t:
        return mxINT32_CLASS;
    case DataTypes_uint32_t:
        return mxUINT32_CLASS;
    case DataTypes_int64_t:
        return mxINT64_CLASS;
    case DataTypes_uint64_t:
        return mxUINT64_CLASS;
    case DataTypes_bool_t:
        return mxLOGICAL_CLASS;
    default:
        throw DataTypeException("Not a numeric data type");
    }
}

RR_INTRUSIVE_PTR<RRBaseArray> GetRRArrayFromMxArray(const mxArray* pa)
{
    mxClassID mxtypeid = mxGetClassID(pa);
    size_t count = mxGetNumberOfElements(pa);
    if (!mxIsComplex(pa))
    {
        RR_INTRUSIVE_PTR<RRBaseArray> rrarray;
        switch (mxtypeid)
        {
        case mxDOUBLE_CLASS:
            rrarray = AllocateRRArray<double>(count);
            break;
        case mxSINGLE_CLASS:
            rrarray = AllocateRRArray<float>(count);
            break;
        case mxINT8_CLASS:
            rrarray = AllocateRRArray<int8_t>(count);
            break;
        case mxUINT8_CLASS:
            rrarray = AllocateRRArray<uint8_t>(count);
            break;
        case mxINT16_CLASS:
            rrarray = AllocateRRArray<int16_t>(count);
            break;
        case mxUINT16_CLASS:
            rrarray = AllocateRRArray<uint16_t>(count);
            break;
        case mxINT32_CLASS:
            rrarray = AllocateRRArray<int32_t>(count);
            break;
        case mxUINT32_CLASS:
            rrarray = AllocateRRArray<uint32_t>(count);
            break;
        case mxINT64_CLASS:
            rrarray = AllocateRRArray<int64_t>(count);
            break;
        case mxUINT64_CLASS:
            rrarray = AllocateRRArray<uint64_t>(count);
            break;
        case mxLOGICAL_CLASS: {
            RR_INTRUSIVE_PTR<RRArray<rr_bool> > bool_array = AllocateRRArray<rr_bool>(count);
            mxLogical* l = (mxLogical*)mxGetData(pa);
            for (size_t i = 0; i < count; i++)
            {
                (*bool_array)[i] = l[i] ? 1 : 0;
            }
            return bool_array;
        }
        default:
            throw DataTypeException("Invalid numeric type");
        }

        void* mxdat = mxGetData(pa);
        memcpy(rrarray->void_ptr(), mxdat, count * rrarray->ElementSize());

        return rrarray;
    }
    else
    {
        switch (mxtypeid)
        {
        case mxDOUBLE_CLASS: {
            RR_INTRUSIVE_PTR<RRArray<cdouble> > complex_array = AllocateRRArray<cdouble>(count);
            mxComplexDouble* mx_complex = mxGetComplexDoubles(pa);
            for (size_t i = 0; i < count; i++)
            {
                (*complex_array)[i] = cdouble(mx_complex[i].real, mx_complex[i].imag);
            }
            return complex_array;
        }
        case mxSINGLE_CLASS: {
            RR_INTRUSIVE_PTR<RRArray<cfloat> > complex_array = AllocateRRArray<cfloat>(count);
            mxComplexSingle* mx_complex = mxGetComplexSingles(pa);
            for (size_t i = 0; i < count; i++)
            {
                (*complex_array)[i] = cfloat(mx_complex[i].real, mx_complex[i].imag);
            }
            return complex_array;
        }
        default:
            throw DataTypeException("Invalid complex type, must be double or single");
        }
    }
}

mxArray* GetMxArrayFromRRArray(const RR_INTRUSIVE_PTR<RRBaseArray>& array_, std::vector<mwSize> dims)
{
    mwSize ndims = 1;

    if (dims.empty())
    {
        dims.push_back(array_->size());
    }
    else
    {
        ndims = dims.size();
    }

    size_t count = 1;
    for (mwSize i = 0; i < dims.size(); i++)
    {
        count *= dims[i];
    }

    if (count != array_->size())
        throw InvalidArgumentException("Dimension element count mismatch");

    mxClassID mxtypeid = mxVOID_CLASS;
    switch (array_->GetTypeID())
    {
    case DataTypes_double_t:
        mxtypeid = mxDOUBLE_CLASS;
        break;
    case DataTypes_single_t:
        mxtypeid = mxSINGLE_CLASS;
        break;
    case DataTypes_int8_t:
        mxtypeid = mxINT8_CLASS;
        break;
    case DataTypes_uint8_t:
        mxtypeid = mxUINT8_CLASS;
        break;
    case DataTypes_int16_t:
        mxtypeid = mxINT16_CLASS;
        break;
    case DataTypes_uint16_t:
        mxtypeid = mxUINT16_CLASS;
        break;
    case DataTypes_int32_t:
        mxtypeid = mxINT32_CLASS;
        break;
    case DataTypes_uint32_t:
        mxtypeid = mxUINT32_CLASS;
        break;
    case DataTypes_int64_t:
        mxtypeid = mxINT64_CLASS;
        break;
    case DataTypes_uint64_t:
        mxtypeid = mxUINT64_CLASS;
        break;
    case DataTypes_bool_t: {
        RR_INTRUSIVE_PTR<RRArray<rr_bool> > bool_array = rr_cast<RRArray<rr_bool> >(array_);
        mxArray* pa2 = mxCreateLogicalArray(ndims, &dims[0]);
        mxLogical* l = (mxLogical*)mxGetData(pa2);
        for (size_t i = 0; i < count; i++)
        {
            l[i] = (*bool_array)[i] != 0;
        }
        return pa2;
    }
    case DataTypes_cdouble_t: {
        RR_INTRUSIVE_PTR<RRArray<cdouble> > complex_array = rr_cast<RRArray<cdouble> >(array_);
        mxArray* pa2 = mxCreateNumericArray(ndims, &dims[0], mxDOUBLE_CLASS, mxCOMPLEX);
        mxComplexDouble* mx_complex = mxGetComplexDoubles(pa2);
        for (size_t i = 0; i < count; i++)
        {
            mx_complex[i].real = (*complex_array)[i].real;
            mx_complex[i].imag = (*complex_array)[i].imag;
        }
        return pa2;
    }
    case DataTypes_csingle_t: {
        RR_INTRUSIVE_PTR<RRArray<cfloat> > complex_array = rr_cast<RRArray<cfloat> >(array_);
        mxArray* pa2 = mxCreateNumericArray(ndims, &dims[0], mxSINGLE_CLASS, mxCOMPLEX);
        mxComplexSingle* mx_complex = mxGetComplexSingles(pa2);
        for (size_t i = 0; i < count; i++)
        {
            mx_complex[i].real = (*complex_array)[i].real;
            mx_complex[i].imag = (*complex_array)[i].imag;
        }
        return pa2;
    }
    default:
        throw DataTypeException("Invalid Robot Raconteur data type");
    }

    mxArray* pa = mxCreateNumericArray(ndims, &dims[0], mxtypeid, mxREAL);
    void* mxreal = mxGetData(pa);
    memcpy(mxreal, array_->void_ptr(), array_->size() * array_->ElementSize());
    return pa;
}

class PackMxArrayToMessageElementImpl
{
  public:
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackMxArrayToMessageElement_pod(
        const mxArray* pm, const boost::shared_ptr<TypeDefinition>& type1, const boost::shared_ptr<ServiceStub>& obj)
    {

        if (!mxIsStruct(pm))
            throw DataTypeException("Matlab structure expected");

        if (type1->Type != DataTypes_varvalue_t && type1->ArrayType == DataTypes_ArrayTypes_none &&
            mxGetNumberOfElements(pm) != 1)
        {
            throw DataTypeException("Scalar expected");
        }
        else
        {
            int32_t c = boost::accumulate(type1->ArrayLength, 1, std::multiplies<int32_t>());
            if (!type1->ArrayLength.empty() && c != 0)
            {
                if (type1->ArrayVarLength)
                {
                    if (mxGetNumberOfElements(pm) > c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
                else
                {
                    if (mxGetNumberOfElements(pm) != c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
            }
        }

        std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
        boost::shared_ptr<ServiceEntryDefinition> struct_def = RR_DYNAMIC_POINTER_CAST<ServiceEntryDefinition>(
            type1->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), obj));
        if (!struct_def)
            throw DataTypeException("Invalid pod data type");
        if (struct_def->EntryType != DataTypes_pod_t)
            throw DataTypeException("Invalid pod data type");
        std::string typestr2 = struct_def->ResolveQualifiedName();

        std::vector<RR_INTRUSIVE_PTR<MessageElement> > ret;

        int mxcount = mxGetNumberOfFields(pm);

        RR_SHARED_PTR<TypeDefinition> type2 = type1->Clone();
        type2->RemoveArray();

        for (size_t i = 0; i < mxGetNumberOfElements(pm); i++)
        {
            push_field_level("[" + boost::lexical_cast<std::string>(i) + "]", type2);
            std::vector<RR_INTRUSIVE_PTR<MessageElement> > m_struct;
            BOOST_FOREACH (const boost::shared_ptr<MemberDefinition>& m_def, struct_def->Members)
            {
                boost::shared_ptr<PropertyDefinition> p_def = boost::dynamic_pointer_cast<PropertyDefinition>(m_def);
                if (!p_def)
                    throw ServiceException("Invalid structure definition: " + typestr2);

                push_field_level(p_def->Name, p_def->Type);

                mxArray* field_data = NULL;
                for (int j = 0; j < mxcount; j++)
                {
                    std::string fname(mxGetFieldNameByNumber(pm, j));
                    if (fname == p_def->Name)
                        field_data = mxGetFieldByNumber(pm, i, j);
                }

                if (!field_data)
                    throw DataTypeException("Structure field not found");

                RR_SHARED_PTR<TypeDefinition> p_def2_type = p_def->Type;
                RR_INTRUSIVE_PTR<MessageElement> el;
                if (p_def2_type->ArrayType == DataTypes_ArrayTypes_multidimarray)
                {
                    size_t elem_size = 1;

                    std::vector<int> dims2 = p_def2_type->ArrayLength;
                    if (p_def->Type->Type == DataTypes_namedtype_t)
                    {
                        RR_SHARED_PTR<NamedTypeDefinition> nt =
                            p_def->Type->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), obj);
                        if (nt->RRDataType() == DataTypes_namedarray_t)
                        {
                            RR_SHARED_PTR<ServiceEntryDefinition> namedarray_def = rr_cast<ServiceEntryDefinition>(nt);
                            boost::tuple<DataTypes, size_t> namedarray_info = GetNamedArrayElementTypeAndCount(
                                namedarray_def, empty_defs, RobotRaconteurNode::sp(), obj);

                            dims2.insert(dims2.begin(), (int)namedarray_info.get<1>());
                            elem_size = namedarray_info.get<1>();
                        }
                    }

                    mwSize dim_count = mxGetNumberOfDimensions(field_data);
                    if (dim_count != dims2.size())
                        throw DataTypeException("Array dimension mismatch");
                    const mwSize* dims = mxGetDimensions(field_data);
                    for (size_t j = 0; j < dim_count; j++)
                    {
                        if (dims[j] != dims2[j])
                            throw DataTypeException("Array dimension mismatch");
                    }

                    p_def2_type = RR_MAKE_SHARED<TypeDefinition>();
                    p_def->Type->CopyTo(*p_def2_type);
                    int32_t c = boost::accumulate(dims2, 1, std::multiplies<int32_t>());
                    p_def2_type->ArrayLength.clear();
                    p_def2_type->ArrayLength.push_back(boost::numeric_cast<int32_t>(c / elem_size));
                    p_def2_type->ArrayType = DataTypes_ArrayTypes_array;
                    p_def2_type->ArrayVarLength = false;

                    mxArray* field_data2 = mxDuplicateArray(field_data);
                    if (elem_size == 1)
                    {
                        mwSize flat_dims = c;
                        mxSetDimensions(field_data2, &flat_dims, 1);
                    }
                    else
                    {
                        mwSize flat_dims[2];
                        flat_dims[0] = elem_size;
                        flat_dims[1] = c / elem_size;
                        mxSetDimensions(field_data2, flat_dims, 2);
                    }
                    el = PackMxArrayToMessageElement(field_data2, p_def2_type, obj, false);
                }
                else
                {
                    el = PackMxArrayToMessageElement(field_data, p_def2_type, obj, false);
                }

                el->ElementName = p_def->Name;
                m_struct.push_back(el);

                pop_field_level();
            }

            RR_INTRUSIVE_PTR<MessageElement> el2 =
                CreateMessageElement("", CreateMessageElementNestedElementList(DataTypes_pod_t, "", m_struct));
            el2->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
            el2->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
            el2->ElementNumber = boost::numeric_cast<int32_t>(i);

            ret.push_back(el2);
            pop_field_level();
        }

        return CreateMessageElementNestedElementList(DataTypes_pod_array_t, typestr2, ret);
    }

    RR_INTRUSIVE_PTR<MessageElement> PackMxArrayToMessageElement(const mxArray* pm,
                                                                 const boost::shared_ptr<TypeDefinition>& tdef,
                                                                 const RR_SHARED_PTR<ServiceStub>& stub,
                                                                 bool allow_null = true)
    {
        if (std::string(mxGetClassName(pm)) == "missing" && allow_null)
        {
            if (tdef && (tdef->ContainerType == DataTypes_ContainerTypes_none))
            {
                std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
                if (IsTypeNumeric(tdef->Type))
                {
                    throw DataTypeException("Scalars and arrays must not be null");
                }
                if (tdef->Type == DataTypes_string_t)
                {
                    throw DataTypeException("Strings must not be null");
                }

                if (tdef->Type == DataTypes_namedtype_t)
                {
                    DataTypes tdef_namedtype =
                        tdef->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), stub)->RRDataType();
                    if (tdef_namedtype == DataTypes_pod_t)
                    {
                        throw DataTypeException("Pods must not be null");
                    }
                    if (tdef_namedtype == DataTypes_namedarray_t)
                    {
                        throw DataTypeException("NamedArrays must not be null");
                    }
                    if (tdef_namedtype == DataTypes_enum_t)
                    {
                        throw DataTypeException("Enums must not be null");
                    }
                }
            }

            return CreateMessageElement(tdef->Name, RR_INTRUSIVE_PTR<MessageElementData>());
        }

        if (tdef->Type == DataTypes_varvalue_t && tdef->ContainerType == DataTypes_ContainerTypes_none)
        {
            if (std::string(mxGetClassName(pm)) != "RobotRaconteurVarValue")
            {
                throw DataTypeException("varvalue types must use RobotRaconteurVarValue");
            }

            mxArray* mxdata = mxGetProperty(pm, 0, "data");
            if (!mxdata)
            {
                throw DataTypeException("Invalid RobotRaconteurVarValue");
            }
            std::string datatype = mxToString(mxGetProperty(pm, 0, "datatype"));

            boost::shared_ptr<TypeDefinition> tdef2 = boost::make_shared<TypeDefinition>();
            tdef2->FromString(datatype);
            tdef2->Name = tdef->Name;
            tdef2->member = tdef->member;

            return PackMxArrayToMessageElement(mxdata, tdef2, stub);
        }

        if (tdef->ContainerType == DataTypes_ContainerTypes_map_int32 ||
            tdef->ContainerType == DataTypes_ContainerTypes_map_string)
        {
            boost::shared_ptr<TypeDefinition> tdef2 = tdef->Clone();
            tdef2->RemoveContainers();

            if (std::string(mxGetClassName(pm)) != "containers.Map")
                throw DataTypeException("Expected object of containers.Map");

            mxArray* keys = NULL;
            mxArray* values = NULL;
            mxArray* pm2 = (mxArray*)pm;
            mexCallMATLAB(1, &keys, 1, &pm2, "keys");
            if (!keys)
                throw DataTypeException("Could not read keys in map");
            mxArray* pm3[2];
            pm3[0] = pm2;
            pm3[1] = keys;
            mexCallMATLAB(1, &values, 1, pm3, "values");
            if (!values)
                throw DataTypeException("Could not read values in map");

            mwSize elementcount = mxGetNumberOfElements(keys);
            if (elementcount != mxGetNumberOfElements(values))
                throw InternalErrorException("Internal map error");

            std::vector<RR_INTRUSIVE_PTR<MessageElement> > mapelems;

            for (mwSize i = 0; i < elementcount; i++)
            {
                mxArray* key = mxGetCell(keys, i);
                mxArray* data = mxGetCell(values, i);

                if (key == NULL || data == NULL)
                    throw DataTypeException("Cell contents must be key value pair");

                if (mxIsChar(key))
                {
                    push_field_level("[\"" + mxToString(key) + "\"]", tdef2);
                }
                else if (mxIsInt32(key) && mxGetNumberOfElements(key) == 1)
                {
                    int32_t elemkey = (((int32_t*)mxGetData(key))[0]);
                    push_field_level("[" + boost::lexical_cast<std::string>(elemkey) + "]", tdef2);
                }
                else
                {
                    throw DataTypeException("Key must be scaler int32 or string");
                }
                RR_INTRUSIVE_PTR<MessageElement> me = PackMxArrayToMessageElement(data, tdef2, stub);

                if (tdef->ContainerType == DataTypes_ContainerTypes_map_int32)
                {
                    int32_t elemkey = 0;
                    if (!mxIsInt32(key) || mxGetNumberOfElements(key) != 1)
                        throw DataTypeException("Key must be scaler int32");
                    elemkey = (((int32_t*)mxGetData(key))[0]);
                    me->ElementName = "";
                    me->ElementNumber = elemkey;
                    me->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
                    me->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;
                }
                else
                {
                    std::string elemkey;
                    if (!mxIsChar(key))
                        throw DataTypeException("Key must be string");
                    elemkey = mxToString(key);
                    me->ElementName = elemkey;
                }

                mapelems.push_back(me);

                pop_field_level();
            }

            if (tdef->ContainerType == DataTypes_ContainerTypes_map_int32)
            {
                return CreateMessageElement(tdef->Name,
                                            CreateMessageElementNestedElementList(DataTypes_vector_t, "", mapelems));
            }
            else
            {
                return CreateMessageElement(
                    tdef->Name, CreateMessageElementNestedElementList(DataTypes_dictionary_t, "", mapelems));
            }
        }

        if (tdef->ContainerType == DataTypes_ContainerTypes_list)
        {
            boost::shared_ptr<TypeDefinition> tdef2 = tdef->Clone();
            tdef2->RemoveContainers();

            if (!mxIsCell(pm))
                throw DataTypeException("Argument is not a list type (must be cell vector)");
            size_t ndims = mxGetNumberOfDimensions(pm);
            std::vector<mwSize> dims1(ndims);
            memcpy(&dims1[0], mxGetDimensions(pm), sizeof(mwSize) * ndims);
            if (mxGetNumberOfElements(pm) > 0)
            {
                for (size_t i = 1; i < ndims; i++)
                {
                    if (dims1[i] != 1)
                        throw DataTypeException("List types must be single dimensional column vector");
                }
            }

            mwSize elementcount = dims1[0];

            std::vector<RR_INTRUSIVE_PTR<MessageElement> > mapelems;

            for (mwSize i = 0; i < elementcount; i++)
            {
                push_field_level("[" + boost::lexical_cast<std::string>(i) + "]", tdef2);

                mxArray* v = mxGetCell(pm, i);

                RR_INTRUSIVE_PTR<MessageElement> me = PackMxArrayToMessageElement(v, tdef2, stub);
                me->ElementName = "";
                me->ElementNumber = boost::numeric_cast<int32_t>(i);
                me->ElementFlags &= ~MessageElementFlags_ELEMENT_NAME_STR;
                me->ElementFlags |= MessageElementFlags_ELEMENT_NUMBER;

                mapelems.push_back(me);

                pop_field_level();
            }

            return CreateMessageElement(tdef->Name,
                                        CreateMessageElementNestedElementList(DataTypes_list_t, "", mapelems));
        }

        if (IsTypeNumeric(tdef->Type) && tdef->ArrayType != DataTypes_ArrayTypes_multidimarray)
        {
            if (!(mxIsNumeric(pm) || mxIsLogical(pm)))
                throw DataTypeException("Column vector expected");

            size_t ndims = mxGetNumberOfDimensions(pm);
            std::vector<mwSize> dims(ndims);
            memcpy(&dims[0], mxGetDimensions(pm), ndims * sizeof(mwSize));
            if (mxGetNumberOfElements(pm) > 0)
            {
                for (size_t i = 1; i < ndims; i++)
                {
                    if (dims[i] != 1)
                        throw DataTypeException("Column vector expected");
                }
            }

            if (tdef->ArrayType == DataTypes_ArrayTypes_none && dims[0] != 1)
                throw DataTypeException("Scalar number expected");

            if (tdef->ArrayType == DataTypes_ArrayTypes_array && !tdef->ArrayVarLength)
            {
                if (tdef->ArrayLength.at(0) != dims[0])
                    throw DataTypeException("Column vector expected with length " +
                                            boost::lexical_cast<std::string>(tdef->ArrayLength.at(0)));
            }

            RR_INTRUSIVE_PTR<RRBaseArray> o1 = GetRRArrayFromMxArray(pm);
            if (o1->GetTypeID() != tdef->Type)
                throw DataTypeException("Numeric array type mismatch");
            if ((tdef->ArrayType != DataTypes_ArrayTypes_none) && tdef->ArrayLength.at(0) != 0)
            {
                if (tdef->ArrayVarLength)
                {
                    if (o1->size() > tdef->ArrayLength.at(0))
                        throw DataTypeException("Array exceeds maximum length");
                }
                else
                {
                    if (o1->size() != tdef->ArrayLength.at(0))
                        throw DataTypeException("Array length mismatch");
                }
            }
            return CreateMessageElement(tdef->Name, o1);
        }

        if (IsTypeNumeric(tdef->Type) && tdef->ArrayType == DataTypes_ArrayTypes_multidimarray)
        {
            if (!(mxIsNumeric(pm) || mxIsLogical(pm)))
                throw DataTypeException("Column vector expected");

            size_t ndims = mxGetNumberOfDimensions(pm);
            std::vector<mwSize> dims(ndims);
            memcpy(&dims[0], mxGetDimensions(pm), ndims * sizeof(mwSize));

            if (!tdef->ArrayVarLength)
            {
                if (mxIsComplex(pm))
                    throw DataTypeException("Array must not be complex");
                if (dims.size() != tdef->ArrayLength.size())
                    throw DataTypeException("Array dimension mismatch");
                for (size_t i = 0; i < dims.size(); i++)
                {
                    if (dims.at(i) != tdef->ArrayLength.at(i))
                        throw DataTypeException("Array dimension mismatch");
                }
            }

            std::vector<RR_INTRUSIVE_PTR<MessageElement> > multidimvec;

            multidimvec.push_back(CreateMessageElement("dims", VectorToRRArray<uint32_t>(dims)));

            RR_INTRUSIVE_PTR<RRBaseArray> array1 = GetRRArrayFromMxArray(pm);

            multidimvec.push_back(CreateMessageElement("array", array1));

            if (array1->GetTypeID() != tdef->Type)
                throw DataTypeException("Array type mismatch");

            return CreateMessageElement(
                tdef->Name, CreateMessageElementNestedElementList(DataTypes_multidimarray_t, "", multidimvec));
        }

        if (tdef->Type == DataTypes_string_t)
        {
            if (!mxIsChar(pm))
                throw DataTypeException("String expected");
            if (mxGetNumberOfDimensions(pm) != 2)
                throw DataTypeException("1xN string expected ");
            const mwSize* pm_dims = mxGetDimensions(pm);
            if (mxGetNumberOfElements(pm) > 0)
            {
                if (pm_dims[0] != 1)
                    throw DataTypeException("1xN string expected");
            }

            const uint16_t* str_utf16 = (const uint16_t*)mxGetData(pm);
            std::string str_utf8 = boost::locale::conv::utf_to_utf<char>(str_utf16, str_utf16 + pm_dims[1]);
            RR_INTRUSIVE_PTR<RRArray<char> > str = stringToRRArray(str_utf8);
            return CreateMessageElement(tdef->Name, str);
        }

        if (tdef->Type == DataTypes_namedtype_t)
        {
            std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
            RR_SHARED_PTR<NamedTypeDefinition> nt = tdef->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), stub);
            switch (nt->RRDataType())
            {
            case DataTypes_structure_t: {
                if (!mxIsStruct(pm))
                    throw DataTypeException("Structure expected");

                std::vector<RR_INTRUSIVE_PTR<MessageElement> > structvec;
                int mxcount = mxGetNumberOfFields(pm);
                RR_SHARED_PTR<ServiceEntryDefinition> d = rr_cast<ServiceEntryDefinition>(nt);
                RR_SHARED_PTR<ServiceDefinition> def = d->ServiceDefinition_.lock();
                if (!def)
                    throw InternalErrorException("Internal type error");

                BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, d->Members)
                {
                    boost::shared_ptr<PropertyDefinition> p = rr_cast<PropertyDefinition>(e);

                    mxArray* data = NULL;
                    for (int i = 0; i < mxcount; i++)
                    {
                        std::string fname(mxGetFieldNameByNumber(pm, i));
                        if (fname == p->Name)
                            data = mxGetFieldByNumber(pm, 0, i);
                    }

                    if (!data)
                        throw DataTypeException("Structure field \"" + p->Name + "\" not found");

                    push_field_level(p->Name, p->Type);

                    RR_INTRUSIVE_PTR<MessageElement> mdata = PackMxArrayToMessageElement(data, p->Type, stub);
                    mdata->ElementName = p->Name;
                    structvec.push_back(mdata);

                    pop_field_level();
                }
                return CreateMessageElement(
                    tdef->Name, CreateMessageElementNestedElementList(DataTypes_structure_t, def->Name + "." + nt->Name,
                                                                      structvec));
            }
            case DataTypes_enum_t: {
                RR_SHARED_PTR<TypeDefinition> enum_type = RR_MAKE_SHARED<TypeDefinition>();
                enum_type->Type = DataTypes_int32_t;
                enum_type->Name = "value";
                return CreateMessageElement(tdef->Name, ScalarToRRArray<int32_t>(GetInt32Scalar(pm)));
            }
            case DataTypes_pod_t: {
                if (tdef->ArrayType == DataTypes_ArrayTypes_multidimarray)
                {
                    std::vector<RR_INTRUSIVE_PTR<MessageElement> > map_vec;

                    if (!mxIsStruct(pm))
                        throw DataTypeException("Invalid PodMultidimArray");

                    size_t ndims = mxGetNumberOfDimensions(pm);
                    std::vector<mwSize> dims(ndims);
                    memcpy(&dims[0], mxGetDimensions(pm), ndims * sizeof(mwSize));

                    if (!tdef->ArrayVarLength)
                    {
                        if (dims.size() != tdef->ArrayLength.size())
                            throw DataTypeException("Array dimension mismatch");
                        for (size_t i = 0; i < dims.size(); i++)
                        {
                            if (dims.at(i) != tdef->ArrayLength.at(i))
                                throw DataTypeException("Array dimension mismatch");
                        }
                    }

                    map_vec.push_back(CreateMessageElement("dims", VectorToRRArray<uint32_t>(dims)));

                    boost::shared_ptr<TypeDefinition> array_type = boost::make_shared<TypeDefinition>();
                    tdef->CopyTo(*array_type);
                    array_type->ArrayType = DataTypes_ArrayTypes_array;
                    array_type->ArrayLength.clear();
                    array_type->ArrayVarLength = true;
                    array_type->Name = "array";
                    // type1->Name = "real";

                    map_vec.push_back(
                        CreateMessageElement("array", PackMxArrayToMessageElement_pod(pm, array_type, stub)));

                    RR_INTRUSIVE_PTR<MessageElementNestedElementList> mm = CreateMessageElementNestedElementList(
                        DataTypes_pod_multidimarray_t, tdef->ResolveNamedType()->ResolveQualifiedName(), map_vec);

                    return CreateMessageElement(tdef->Name, mm);
                }
                else
                {
                    return CreateMessageElement(tdef->Name, PackMxArrayToMessageElement_pod(pm, tdef, stub));
                }
            }
            case DataTypes_namedarray_t: {
                RR_SHARED_PTR<ServiceEntryDefinition> namedarray_def = rr_cast<ServiceEntryDefinition>(nt);
                boost::tuple<DataTypes, size_t> namedarray_info =
                    GetNamedArrayElementTypeAndCount(namedarray_def, empty_defs, RobotRaconteurNode::sp(), stub);

                if (namedarray_info.get<0>() != mxClassIDToRRDataType(mxGetClassID(pm)))
                    throw DataTypeException("Invalid namedarray data type");

                size_t ndims = mxGetNumberOfDimensions(pm);
                if (ndims < 1)
                    throw DataTypeException("Invalid namedarray data type");
                std::vector<mwSize> dims(ndims);
                memcpy(&dims[0], mxGetDimensions(pm), ndims * sizeof(mwSize));
                if (mxGetNumberOfElements(pm) > 0)
                {
                    if (dims.at(0) != namedarray_info.get<1>())
                        throw DataTypeException("Invalid namedarray data size");
                }

                switch (tdef->ArrayType)
                {
                case DataTypes_ArrayTypes_none: {
                    for (size_t i = 1; i < dims.size(); i++)
                    {
                        if (dims.at(i) != 1)
                            throw DataTypeException("Invalid namedarray data size");
                    }

                    std::vector<RR_INTRUSIVE_PTR<MessageElement> > o1;
                    o1.push_back(CreateMessageElement("array", GetRRArrayFromMxArray(pm)));
                    RR_INTRUSIVE_PTR<MessageElementNestedElementList> o2 = CreateMessageElementNestedElementList(
                        DataTypes_namedarray_array_t, tdef->ResolveNamedType()->ResolveQualifiedName(), o1);
                    return CreateMessageElement(tdef->Name, o2);
                }
                case DataTypes_ArrayTypes_array: {
                    if (ndims > 1)
                    {
                        if (tdef->ArrayType == DataTypes_ArrayTypes_array)
                        {
                            if (!tdef->ArrayVarLength && tdef->ArrayLength.at(0) != dims.at(1))
                                throw DataTypeException("Invalid namedarray length");
                            if (tdef->ArrayVarLength && tdef->ArrayLength.at(0) != 0 &&
                                tdef->ArrayLength.at(0) < dims.at(1))
                                throw DataTypeException("Invalid namedarray length");
                        }
                    }

                    if (mxGetNumberOfElements(pm) > 0)
                    {
                        for (size_t i = 2; i < dims.size(); i++)
                        {
                            if (dims.at(i) != 1)
                                throw DataTypeException("Invalid namedarray data size");
                        }
                    }

                    std::vector<RR_INTRUSIVE_PTR<MessageElement> > o1;
                    o1.push_back(CreateMessageElement("array", GetRRArrayFromMxArray(pm)));
                    RR_INTRUSIVE_PTR<MessageElementNestedElementList> o2 = CreateMessageElementNestedElementList(
                        DataTypes_namedarray_array_t, tdef->ResolveNamedType()->ResolveQualifiedName(), o1);
                    return CreateMessageElement(tdef->Name, o2);
                }
                case DataTypes_ArrayTypes_multidimarray: {
                    if (!tdef->ArrayVarLength)
                    {
                        if (dims.size() != (tdef->ArrayLength.size() + 1))
                            throw DataTypeException("Invalid namedarray data size");
                        for (size_t i = 1; i < dims.size(); i++)
                        {
                            if (dims.at(i) != tdef->ArrayLength.at(i - 1))
                                throw DataTypeException("Invalid namedarray data size");
                        }
                    }

                    std::vector<RR_INTRUSIVE_PTR<MessageElement> > o1;
                    o1.push_back(CreateMessageElement("array", GetRRArrayFromMxArray(pm)));
                    RR_INTRUSIVE_PTR<MessageElementNestedElementList> o2 = CreateMessageElementNestedElementList(
                        DataTypes_namedarray_array_t, tdef->ResolveNamedType()->ResolveQualifiedName(), o1);

                    std::vector<RR_INTRUSIVE_PTR<MessageElement> > o3;
                    RR_INTRUSIVE_PTR<RRArray<uint32_t> > rr_dims = AllocateRRArray<uint32_t>(dims.size() - 1);
                    for (size_t i = 0; i < rr_dims->size(); i++)
                    {
                        (*rr_dims)[i] = dims.at(i + 1);
                    }
                    o3.push_back(CreateMessageElement("dims", rr_dims));
                    o3.push_back(CreateMessageElement("array", o2));
                    RR_INTRUSIVE_PTR<MessageElementNestedElementList> o4 = CreateMessageElementNestedElementList(
                        DataTypes_namedarray_multidimarray_t, tdef->ResolveNamedType()->ResolveQualifiedName(), o3);
                    return CreateMessageElement(tdef->Name, o4);
                }
                default:
                    throw DataTypeException("Invalid namedarray type");
                }
            }
            default:
                throw DataTypeException("Unknown named type id");
            }
        }

        throw DataTypeException("Unknown data type");
    }

    std::vector<std::string> field_name;
    std::vector<RR_SHARED_PTR<TypeDefinition> > field_type;

    void push_field_level(const std::string& new_var, const RR_SHARED_PTR<TypeDefinition>& new_type)
    {
        field_name.push_back(new_var);
        field_type.push_back(new_type);
    }

    void pop_field_level()
    {
        field_name.pop_back();
        field_type.pop_back();
    }

    boost::tuple<std::string, RR_SHARED_PTR<TypeDefinition> > get_current_field()
    {
        if (field_name.empty())
        {
            return boost::make_tuple("**internal error**", RR_SHARED_PTR<TypeDefinition>());
        }

        std::string name = boost::join(field_name, ".");
        boost::replace_all(name, ".[", "[");
        RR_SHARED_PTR<TypeDefinition> type = field_type.back();

        return boost::make_tuple(name, type);
    }

    std::string get_exception_message(const std::string& msg)
    {
        boost::tuple<std::string, RR_SHARED_PTR<TypeDefinition> > f = get_current_field();

        std::string msg2 = msg + " for field \"" + f.get<0>() + "\"";
        if (f.get<1>())
        {
            RR_SHARED_PTR<TypeDefinition> f2 = f.get<1>()->Clone();
            if (f2->Type == DataTypes_namedtype_t)
            {
                try
                {
                    f2->TypeString = f.get<1>()->ResolveNamedType()->ResolveQualifiedName();
                }
                catch (std::exception&)
                {}
            }
            std::vector<std::string> f_split;
            std::string f2_str = f2->ToString();
            boost::split(f_split, f2_str, boost::is_any_of(" \t"), boost::token_compress_on);
            msg2 += " expected Robot Raconteur type \"" + f_split.at(0) + "\"";
        }

        return msg2;
    }
};

RR_INTRUSIVE_PTR<MessageElement> PackMxArrayToMessageElement(const mxArray* pm,
                                                             const boost::shared_ptr<TypeDefinition>& tdef,
                                                             const RR_SHARED_PTR<ServiceStub>& stub, bool allow_null)
{
    PackMxArrayToMessageElementImpl p;
    if (tdef)
    {
        p.push_field_level(tdef->Name, tdef);
    }
    else
    {
        p.push_field_level("value", RR_SHARED_PTR<TypeDefinition>());
    }
    try
    {
        return p.PackMxArrayToMessageElement(pm, tdef, stub, allow_null);
    }
    catch (DataTypeException& e)
    {
        e.Message = p.get_exception_message(e.Message);
        throw;
    }
    catch (InvalidArgumentException& e)
    {
        e.Message = p.get_exception_message(e.Message);
        throw;
    }
}

RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackMxArrayToMessageElement_pod(
    const mxArray* pm, const boost::shared_ptr<TypeDefinition>& type1, const boost::shared_ptr<ServiceStub>& obj)
{
    PackMxArrayToMessageElementImpl p;
    if (type1)
    {
        p.push_field_level(type1->Name, type1);
    }
    else
    {
        p.push_field_level("value", RR_SHARED_PTR<TypeDefinition>());
    }
    try
    {
        return p.PackMxArrayToMessageElement_pod(pm, type1, obj);
    }
    catch (DataTypeException& e)
    {
        e.Message = p.get_exception_message(e.Message);
        throw;
    }
    catch (InvalidArgumentException& e)
    {
        e.Message = p.get_exception_message(e.Message);
        throw;
    }
}

class UnpackMessageElementToMxArrayImpl
{
  public:
    mxArray* UnpackMessageElementToMxArray_pod(const RR_INTRUSIVE_PTR<MessageElement>& element,
                                               const boost::shared_ptr<TypeDefinition>& type1,
                                               const boost::shared_ptr<ServiceStub>& stub)
    {
        std::vector<boost::shared_ptr<ServiceDefinition> > other_defs;
        boost::shared_ptr<ServiceEntryDefinition> struct_def =
            rr_cast<ServiceEntryDefinition>(type1->ResolveNamedType(other_defs, RobotRaconteurNode::sp(), stub));
        if (!struct_def)
            throw DataTypeException("Invalid pod type");

        RR_INTRUSIVE_PTR<MessageElementNestedElementList> l = element->CastDataToNestedList(DataTypes_pod_array_t);

        RR_SHARED_PTR<TypeDefinition> type2;

        if (type1)
        {
            int32_t c = boost::accumulate(type1->ArrayLength, 1, std::multiplies<int32_t>());
            if (!type1->ArrayLength.empty() && c != 0)
            {

                if (type1->ArrayVarLength)
                {
                    if (l->Elements.size() > c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
                else
                {
                    if (l->Elements.size() != c)
                    {
                        throw DataTypeException("Array dimension mismatch");
                    }
                }
            }

            if (type1->ArrayType == DataTypes_ArrayTypes_none)
            {
                if (l->Elements.size() != 1)
                    throw DataTypeException("Expected scalar");
            }

            type2 = type1->Clone();
            type2->RemoveArray();
        }

        boost::shared_array<const char*> fieldnames(new const char*[struct_def->Members.size()]);

        for (size_t i = 0; i < struct_def->Members.size(); i++)
        {
            fieldnames[i] = struct_def->Members[i]->Name.c_str(); // NOLINT
        }

        mwSize dims = l->Elements.size();
        mxArray* a = mxCreateStructArray(1, &dims, (int)struct_def->Members.size(), fieldnames.get());

        for (uint32_t i = 0; i < boost::numeric_cast<uint32_t>(l->Elements.size()); i++)
        {
            push_field_level("[" + boost::lexical_cast<std::string>(i) + "]", type2);

            RR_INTRUSIVE_PTR<MessageElement>& el1 = l->Elements[i];

            if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
            {
                if (i != el1->ElementNumber)
                    throw DataTypeException("Invalid pod array");
            }
            else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
            {
                if (i != boost::lexical_cast<int32_t>(el1->ElementName.str()))
                    throw DataTypeException("Invalid pod array");
            }
            else
            {
                throw DataTypeException("Invalid pod array");
            }

            RR_INTRUSIVE_PTR<MessageElementNestedElementList> s = el1->CastDataToNestedList(DataTypes_pod_t);

            std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;

            int fieldnumber = 0;
            BOOST_FOREACH (const boost::shared_ptr<MemberDefinition>& m, struct_def->Members)
            {
                boost::shared_ptr<PropertyDefinition> p = rr_cast<PropertyDefinition>(m);

                push_field_level(p->Name, p->Type);

                RR_INTRUSIVE_PTR<MessageElement> el = MessageElement::FindElement(s->Elements, p->Name);

                RR_SHARED_PTR<TypeDefinition> p1 = p->Type;
                if (p->Type->ArrayType == DataTypes_ArrayTypes_multidimarray)
                {
                    p1 = RR_MAKE_SHARED<TypeDefinition>();
                    p->Type->CopyTo(*p1);
                    int32_t c = boost::accumulate(p1->ArrayLength, 1, std::multiplies<int32_t>());
                    p1->ArrayLength.clear();
                    p1->ArrayLength.push_back(c);
                    p1->ArrayType = DataTypes_ArrayTypes_array;
                    p1->ArrayVarLength = false;
                }

                if (p1->Type == DataTypes_namedtype_t)
                {
                    std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
                    if (p1->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), stub)->RRDataType() ==
                        DataTypes_pod_t)
                    {
                        mxArray* el1 = UnpackMessageElementToMxArray_pod(el, p1, stub);
                        if (p->Type->ArrayType == DataTypes_ArrayTypes_multidimarray && !p->Type->ArrayLength.empty())
                        {
                            std::vector<mwSize> mw_dims;
                            BOOST_FOREACH (int32_t d, p->Type->ArrayLength)
                                mw_dims.push_back((mwSize)d);
                            if (mxSetDimensions(el1, &mw_dims[0], mw_dims.size()))
                            {
                                throw DataTypeException("Pod multidimarray dimension mismatch");
                            }
                        }
                        mxSetFieldByNumber(a, i, fieldnumber, el1);
                        fieldnumber++;
                        continue;
                    }
                }

                {
                    mxArray* el1 = UnpackMessageElementToMxArray(el, p1, stub);
                    if (p->Type->ArrayType == DataTypes_ArrayTypes_multidimarray && !p->Type->ArrayLength.empty())
                    {
                        std::vector<mwSize> mw_dims;
                        if (p->Type->Type == DataTypes_namedtype_t)
                        {
                            RR_SHARED_PTR<NamedTypeDefinition> nt =
                                p->Type->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), stub);
                            if (nt->RRDataType() == DataTypes_namedarray_t)
                            {
                                RR_SHARED_PTR<ServiceEntryDefinition> namedarray_def =
                                    rr_cast<ServiceEntryDefinition>(nt);
                                boost::tuple<DataTypes, size_t> namedarray_info = GetNamedArrayElementTypeAndCount(
                                    namedarray_def, empty_defs, RobotRaconteurNode::sp(), stub);
                                mw_dims.push_back(namedarray_info.get<1>());
                            }
                        }

                        BOOST_FOREACH (int32_t d, p->Type->ArrayLength)
                            mw_dims.push_back((mwSize)d);
                        if (mxSetDimensions(el1, &mw_dims[0], mw_dims.size()))
                        {
                            throw DataTypeException("Pod multidimarray dimension mismatch");
                        }
                    }
                    mxSetFieldByNumber(a, i, fieldnumber, el1);
                    fieldnumber++;
                }

                pop_field_level();
            }

            pop_field_level();
        }

        return a;
    }

    mxArray* UnpackMessageElementToMxArray(const RR_INTRUSIVE_PTR<MessageElement>& m,
                                           const boost::shared_ptr<TypeDefinition>& tdef,
                                           const RR_SHARED_PTR<ServiceStub>& stub)
    {
        if (!m || m->ElementType == DataTypes_void_t)
        {
            if (tdef && (tdef->ContainerType == DataTypes_ContainerTypes_none))
            {
                std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
                if (IsTypeNumeric(tdef->Type))
                {
                    throw DataTypeException("Scalars and arrays must not be null");
                }
                if (tdef->Type == DataTypes_string_t)
                {
                    throw DataTypeException("Strings must not be null");
                }

                if (tdef->Type == DataTypes_namedtype_t)
                {
                    DataTypes tdef_namedtype =
                        tdef->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), stub)->RRDataType();
                    if (tdef_namedtype == DataTypes_pod_t)
                    {
                        throw DataTypeException("Pods must not be null");
                    }
                    if (tdef_namedtype == DataTypes_namedarray_t)
                    {
                        throw DataTypeException("NamedArrays must not be null");
                    }
                    if (tdef_namedtype == DataTypes_enum_t)
                    {
                        throw DataTypeException("Enums must not be null");
                    }
                }
            }

            mxArray* o = NULL;
            mexCallMATLAB(1, &o, 0, NULL, "missing");
            return o;
        }

        if (tdef && tdef->Type == DataTypes_varvalue_t)
        {
            if (m->ElementType == DataTypes_void_t)
            {
                mxArray* o = NULL;
                mexCallMATLAB(1, &o, 0, NULL, "missing");
                return o;
            }

            if (tdef->ContainerType == DataTypes_ContainerTypes_none)
            {

                boost::shared_ptr<TypeDefinition> tdef_3t;
                switch (m->ElementType)
                {
                case DataTypes_vector_t: {
                    tdef_3t = boost::make_shared<TypeDefinition>();
                    tdef_3t->Type = DataTypes_varvalue_t;
                    tdef_3t->ContainerType = DataTypes_ContainerTypes_map_int32;
                    tdef_3t->Name = "value";
                    break;
                }
                case DataTypes_dictionary_t: {
                    tdef_3t = boost::make_shared<TypeDefinition>();
                    tdef_3t->Type = DataTypes_varvalue_t;
                    tdef_3t->ContainerType = DataTypes_ContainerTypes_map_string;
                    tdef_3t->Name = "value";
                    break;
                }
                case DataTypes_list_t: {
                    tdef_3t = boost::make_shared<TypeDefinition>();
                    tdef_3t->Type = DataTypes_varvalue_t;
                    tdef_3t->ContainerType = DataTypes_ContainerTypes_list;
                    tdef_3t->Name = "value";
                    break;
                }
                default:
                    break;
                }

                if (tdef_3t)
                {

                    {
                        mxArray* vardata = UnpackMessageElementToMxArray(m, tdef_3t, stub);

                        std::string tdef_3t_str = tdef_3t->ToString();
                        mxArray* tdef_3t_mx = mxCreateString(tdef_3t_str.c_str());
                        mxArray* o = NULL;
                        mxArray* prhs[2];
                        prhs[0] = vardata;
                        prhs[1] = tdef_3t_mx;
                        mexCallMATLAB(1, &o, 2, prhs, "RobotRaconteurVarValue");
                        return o;
                    }
                }

                boost::shared_ptr<TypeDefinition> tdef_2t = boost::make_shared<TypeDefinition>();
                if (IsTypeNumeric(m->ElementType))
                {
                    tdef_2t->ArrayType = DataTypes_ArrayTypes_array;
                    tdef_2t->ArrayVarLength = true;
                    tdef_2t->Type = m->ElementType;
                    tdef_2t->ArrayLength.push_back(0);
                }
                else if (m->ElementType == DataTypes_string_t)
                {
                    tdef_2t->Type = DataTypes_string_t;
                }
                else if (m->ElementType == DataTypes_structure_t)
                {
                    tdef_2t->Type = DataTypes_namedtype_t;
                    tdef_2t->TypeString = m->ElementTypeName.str().to_string();
                }
                else if (m->ElementType == DataTypes_pod_t)
                {
                    tdef_2t->Type = DataTypes_namedtype_t;
                    tdef_2t->TypeString = m->ElementTypeName.str().to_string();
                }
                else if (m->ElementType == DataTypes_pod_array_t)
                {
                    tdef_2t->Type = DataTypes_namedtype_t;
                    tdef_2t->TypeString = m->ElementTypeName.str().to_string();
                    tdef_2t->ArrayVarLength = true;
                    tdef_2t->ArrayLength.push_back(0);
                    tdef_2t->ArrayType = DataTypes_ArrayTypes_array;
                }
                else if (m->ElementType == DataTypes_pod_multidimarray_t)
                {
                    tdef_2t->Type = DataTypes_namedtype_t;
                    tdef_2t->TypeString = m->ElementTypeName.str().to_string();
                    tdef_2t->ArrayVarLength = true;
                    tdef_2t->ArrayType = DataTypes_ArrayTypes_multidimarray;
                }
                else if (m->ElementType == DataTypes_namedarray_array_t)
                {
                    tdef_2t->Type = DataTypes_namedtype_t;
                    tdef_2t->TypeString = m->ElementTypeName.str().to_string();
                    tdef_2t->ArrayVarLength = true;
                    tdef_2t->ArrayLength.push_back(0);
                    tdef_2t->ArrayType = DataTypes_ArrayTypes_array;
                }
                else if (m->ElementType == DataTypes_namedarray_multidimarray_t)
                {
                    tdef_2t->Type = DataTypes_namedtype_t;
                    tdef_2t->TypeString = m->ElementTypeName.str().to_string();
                    tdef_2t->ArrayVarLength = true;
                    tdef_2t->ArrayType = DataTypes_ArrayTypes_multidimarray;
                }
                else if (m->ElementType == DataTypes_list_t)
                {
                    tdef_2t->Type = DataTypes_varvalue_t;
                    tdef_2t->ContainerType = DataTypes_ContainerTypes_list;
                }
                else if (m->ElementType == DataTypes_vector_t)
                {
                    tdef_2t->Type = DataTypes_varvalue_t;
                    tdef_2t->ContainerType = DataTypes_ContainerTypes_map_int32;
                }
                else if (m->ElementType == DataTypes_dictionary_t)
                {
                    tdef_2t->Type = DataTypes_varvalue_t;
                    tdef_2t->ContainerType = DataTypes_ContainerTypes_map_string;
                }
                else if (m->ElementType == DataTypes_multidimarray_t)
                {
                    tdef_2t->ArrayType = DataTypes_ArrayTypes_multidimarray;
                    tdef_2t->ArrayVarLength = true;
                    tdef_2t->ArrayLength.push_back(0);
                    tdef_2t->Type = MessageElement::FindElement(
                                        m->CastDataToNestedList(DataTypes_multidimarray_t)->Elements, "array")
                                        ->ElementType;
                    if (!IsTypeNumeric(tdef_2t->Type))
                        throw DataTypeException("Invalid MultiDimArray");
                }
                else
                {
                    throw DataTypeException("Invalid data type for field " + tdef->Name);
                }

                {
                    mxArray* vardata = UnpackMessageElementToMxArray(m, tdef_2t, stub);

                    std::string tdef_2t_str = tdef_2t->ToString();
                    mxArray* tdef_2t_mx = mxCreateString(tdef_2t_str.c_str());
                    mxArray* o = NULL;
                    mxArray* prhs[2];
                    prhs[0] = vardata;
                    prhs[1] = tdef_2t_mx;
                    mexCallMATLAB(1, &o, 2, prhs, "RobotRaconteurVarValue");
                    return o;
                }
            }
            else
            {
                if (m->ElementType == DataTypes_list_t)
                {
                    boost::shared_ptr<TypeDefinition> type2 = boost::make_shared<TypeDefinition>();
                    type2->Type = DataTypes_varvalue_t;
                    type2->Name = "value";

                    boost::intrusive_ptr<MessageElementNestedElementList> l = m->CastDataToNestedList(DataTypes_list_t);
                    mwSize dims = (mwSize)l->Elements.size();

                    mxArray* v = mxCreateCellArray(1, &dims);

                    mwIndex count = 0;

                    for (uint32_t i = 0; i < boost::numeric_cast<uint32_t>(l->Elements.size()); i++)
                    {
                        boost::intrusive_ptr<MessageElement>& el1 = l->Elements[i];

                        if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
                        {
                            if (i != el1->ElementNumber)
                                throw DataTypeException("Invalid list");
                        }
                        else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
                        {
                            if (i != boost::lexical_cast<int32_t>(el1->ElementName.str()))
                                throw DataTypeException("Invalid list");
                        }
                        else
                        {
                            throw DataTypeException("Invalid list");
                        }

                        push_field_level("[" + boost::lexical_cast<std::string>(i) + "]", type2);
                        mxArray* mxdata = UnpackMessageElementToMxArray(el1, type2, stub);
                        mxSetCell(v, count, mxdata);

                        count++;
                        pop_field_level();
                    }

                    return v;
                }

                if (m->ElementType == DataTypes_vector_t)
                {
                    boost::shared_ptr<TypeDefinition> type2 = boost::make_shared<TypeDefinition>();
                    type2->Type = DataTypes_varvalue_t;
                    type2->Name = "value";

                    boost::intrusive_ptr<MessageElementNestedElementList> l =
                        m->CastDataToNestedList(DataTypes_vector_t);
                    mwIndex count = 0;

                    mxArray* o = NULL;
                    mxArray* prhs[4];
                    prhs[0] = mxCreateString("KeyType");
                    prhs[1] = mxCreateString("int32");
                    prhs[2] = mxCreateString("ValueType");
                    prhs[3] = mxCreateString("any");

                    mexCallMATLAB(1, &o, 4, prhs, "containers.Map");

                    for (int32_t i = 0; i < boost::numeric_cast<int32_t>(l->Elements.size()); i++)
                    {
                        boost::intrusive_ptr<MessageElement>& el1 = l->Elements[i];
                        const char* fnames[] = {"type", "subs"};
                        mxArray* subs = mxCreateStructMatrix(1, 1, 2, fnames);
                        mxSetFieldByNumber(subs, 0, 0, mxCreateString("()"));

                        mxArray* mxkey = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
                        int32_t* key = (int32_t*)mxGetData(mxkey);
                        if (el1->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
                        {
                            key[0] = el1->ElementNumber;
                        }
                        else if (el1->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
                        {
                            key[0] = boost::lexical_cast<int32_t>(el1->ElementName.str());
                        }
                        else
                        {
                            throw DataTypeException("Invalid key type");
                        }

                        push_field_level("[" + boost::lexical_cast<std::string>(key[0]) + "]", type2);
                        mxArray* k_cell = mxCreateCellMatrix(1, 1);

                        mxSetCell(k_cell, 0, mxkey);

                        mxSetFieldByNumber(subs, 0, 1, k_cell);

                        mxArray* mxdata = UnpackMessageElementToMxArray(el1, type2, stub);

                        mxArray* prhs2[3];
                        prhs2[0] = o;
                        prhs2[1] = subs;
                        prhs2[2] = mxdata;

                        mexCallMATLAB(0, NULL, 3, prhs2, "subsasgn");

                        count++;
                        pop_field_level();
                    }

                    return o;
                }

                if (m->ElementType == DataTypes_dictionary_t)
                {
                    boost::shared_ptr<TypeDefinition> type2 = boost::make_shared<TypeDefinition>();
                    type2->Type = DataTypes_varvalue_t;
                    type2->Name = "value";

                    boost::intrusive_ptr<MessageElementNestedElementList> l =
                        m->CastDataToNestedList(DataTypes_dictionary_t);
                    mwIndex count = 0;

                    mxArray* o = NULL;
                    mxArray* prhs[4];
                    prhs[0] = mxCreateString("KeyType");
                    prhs[1] = mxCreateString("char");
                    prhs[2] = mxCreateString("ValueType");
                    prhs[3] = mxCreateString("any");

                    mexCallMATLAB(1, &o, 4, prhs, "containers.Map");

                    for (int32_t i = 0; i < boost::numeric_cast<int32_t>(l->Elements.size()); i++)
                    {
                        boost::intrusive_ptr<MessageElement>& el1 = l->Elements[i];

                        push_field_level("[\"" + el1->ElementName.str() + "\"]", type2);
                        const char* fnames[] = {"type", "subs"};
                        mxArray* subs = mxCreateStructMatrix(1, 1, 2, fnames);
                        mxSetFieldByNumber(subs, 0, 0, mxCreateString("()"));

                        std::string key1 = el1->ElementName.str().to_string();
                        mxArray* mxkey = mxCreateString(key1.c_str());
                        mxSetFieldByNumber(subs, 0, 1, mxkey);

                        // mxSetCell(k,count,mxkey);

                        mxArray* mxdata = UnpackMessageElementToMxArray(el1, type2, stub);
                        // mxSetCell(v,count,mxdata);

                        mxArray* prhs2[3];
                        prhs2[0] = o;
                        prhs2[1] = subs;
                        prhs2[2] = mxdata;

                        mexCallMATLAB(0, NULL, 3, prhs2, "subsasgn");

                        count++;
                        pop_field_level();
                    }

                    return o;
                }
            }

            throw DataTypeException("Invalid message collection type");
        }

        if (tdef->ContainerType == DataTypes_ContainerTypes_map_int32 ||
            tdef->ContainerType == DataTypes_ContainerTypes_map_string)
        {

            boost::shared_ptr<TypeDefinition> tdef2 = tdef->Clone();
            tdef2->RemoveContainers();

            if (tdef->ContainerType == DataTypes_ContainerTypes_map_int32)
            {
                std::vector<RR_INTRUSIVE_PTR<MessageElement> > elems =
                    m->CastDataToNestedList(DataTypes_vector_t)->Elements;

                mwIndex count = 0;

                mxArray* o = NULL;
                mxArray* prhs[4];
                prhs[0] = mxCreateString("KeyType");
                prhs[1] = mxCreateString("int32");
                prhs[2] = mxCreateString("ValueType");
                prhs[3] = mxCreateString("any");

                mexCallMATLAB(1, &o, 4, prhs, "containers.Map");

                BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, elems)
                {
                    const char* fnames[] = {"type", "subs"};
                    mxArray* subs = mxCreateStructMatrix(1, 1, 2, fnames);
                    mxSetFieldByNumber(subs, 0, 0, mxCreateString("()"));

                    mxArray* mxkey = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
                    int32_t* key = (int32_t*)mxGetData(mxkey);
                    if (e->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
                    {
                        key[0] = e->ElementNumber;
                    }
                    else if (e->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
                    {
                        key[0] = boost::lexical_cast<int32_t>(e->ElementName.str());
                    }
                    else
                    {
                        throw DataTypeException("Invalid key type");
                    }

                    push_field_level("[" + boost::lexical_cast<std::string>(key[0]) + "]", tdef2);

                    mxArray* k_cell = mxCreateCellMatrix(1, 1);

                    mxSetCell(k_cell, 0, mxkey);

                    mxSetFieldByNumber(subs, 0, 1, k_cell);

                    mxArray* mxdata = UnpackMessageElementToMxArray(e, tdef2, stub);

                    mxArray* prhs2[3];
                    prhs2[0] = o;
                    prhs2[1] = subs;
                    prhs2[2] = mxdata;

                    mexCallMATLAB(0, NULL, 3, prhs2, "subsasgn");

                    count++;

                    pop_field_level();
                }

                return o;
            }

            if (tdef->ContainerType == DataTypes_ContainerTypes_map_string)
            {
                std::vector<RR_INTRUSIVE_PTR<MessageElement> > elems =
                    m->CastDataToNestedList(DataTypes_dictionary_t)->Elements;

                mwIndex count = 0;

                mxArray* o = NULL;
                mxArray* prhs[4];
                prhs[0] = mxCreateString("KeyType");
                prhs[1] = mxCreateString("char");
                prhs[2] = mxCreateString("ValueType");
                prhs[3] = mxCreateString("any");

                mexCallMATLAB(1, &o, 4, prhs, "containers.Map");

                BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, elems)
                {
                    push_field_level("[\"" + e->ElementName.str() + "\"]", tdef2);

                    const char* fnames[] = {"type", "subs"};
                    mxArray* subs = mxCreateStructMatrix(1, 1, 2, fnames);
                    mxSetFieldByNumber(subs, 0, 0, mxCreateString("()"));

                    std::string key1 = e->ElementName.str().to_string();
                    mxArray* mxkey = mxCreateString(key1.c_str());
                    mxSetFieldByNumber(subs, 0, 1, mxkey);

                    // mxSetCell(k,count,mxkey);

                    mxArray* mxdata = UnpackMessageElementToMxArray(e, tdef2, stub);
                    // mxSetCell(v,count,mxdata);

                    mxArray* prhs2[3];
                    prhs2[0] = o;
                    prhs2[1] = subs;
                    prhs2[2] = mxdata;

                    mexCallMATLAB(0, NULL, 3, prhs2, "subsasgn");

                    count++;

                    pop_field_level();
                }
                return o;
            }

            throw DataTypeException("Unknown map key type");
        }

        if (tdef->ContainerType == DataTypes_ContainerTypes_list)
        {
            boost::shared_ptr<TypeDefinition> tdef2 = tdef->Clone();
            tdef2->RemoveContainers();

            std::vector<RR_INTRUSIVE_PTR<MessageElement> > elems = m->CastDataToNestedList(DataTypes_list_t)->Elements;

            mwSize dims = (mwSize)elems.size();

            mxArray* v = mxCreateCellArray(1, &dims);

            mwIndex count = 0;

            BOOST_FOREACH (RR_INTRUSIVE_PTR<MessageElement>& e, elems)
            {

                int32_t c = 0;
                if (e->ElementFlags & MessageElementFlags_ELEMENT_NUMBER)
                {
                    c = e->ElementNumber;
                }
                else if (e->ElementFlags & MessageElementFlags_ELEMENT_NAME_STR)
                {
                    c = boost::lexical_cast<int32_t>(e->ElementName.str());
                }
                else
                {
                    throw DataTypeException("Invalid list type");
                }

                if (count != c)
                    throw DataTypeException("Error in list format");

                push_field_level("[" + boost::lexical_cast<std::string>(c) + "]", tdef2);

                mxArray* mxdata = UnpackMessageElementToMxArray(e, tdef2, stub);
                mxSetCell(v, count, mxdata);

                count++;

                pop_field_level();
            }

            return v;
        }

        if (IsTypeNumeric(tdef->Type))
        {
            if (tdef->ArrayType != DataTypes_ArrayTypes_multidimarray)
            {
                RR_INTRUSIVE_PTR<RRBaseArray> data = m->CastData<RRBaseArray>();
                if ((tdef->ArrayType != DataTypes_ArrayTypes_none) && tdef->ArrayLength.at(0) != 0)
                {
                    if (tdef->ArrayVarLength)
                    {
                        // if (data)
                        if (data->size() > tdef->ArrayLength.at(0))
                            throw DataTypeException("Array exceeds maximum length");
                    }
                    else
                    {
                        // if (!data) throw DataTypeException("Array " + tdef->Name + " must not be null");
                        if (data->size() != tdef->ArrayLength.at(0))
                            throw DataTypeException("Array length mismatch");
                    }
                }
                if (tdef->ArrayType == DataTypes_ArrayTypes_none)
                {
                    if (data->size() != 1)
                        throw DataTypeException("Expected scalar");
                }
                return GetMxArrayFromRRArray(data);
            }
            else
            {
                RR_INTRUSIVE_PTR<MessageElementNestedElementList> a =
                    m->CastDataToNestedList(DataTypes_multidimarray_t);
                std::vector<mwSize> dims = RRArrayToVector<mwSize>(
                    MessageElement::FindElement(a->Elements, "dims")->CastData<RRArray<uint32_t> >());
                RR_INTRUSIVE_PTR<RRBaseArray> array =
                    MessageElement::FindElement(a->Elements, "array")->CastData<RRBaseArray>();

                if (!tdef->ArrayVarLength)
                {
                    if (dims.size() != tdef->ArrayLength.size())
                        throw DataTypeException("Array dimension mismatch");
                    for (size_t i = 0; i < dims.size(); i++)
                    {
                        if (dims.at(i) != tdef->ArrayLength.at(i))
                            throw DataTypeException("Array dimension mismatch");
                    }
                }

                return GetMxArrayFromRRArray(array, dims);
            }
        }

        if (tdef->Type == DataTypes_string_t)
        {
            RR_INTRUSIVE_PTR<RRArray<char> > m_str = m->CastData<RRArray<char> >();
            std::basic_string<uint16_t> data_utf16 =
                boost::locale::conv::utf_to_utf<uint16_t>(m_str->data(), m_str->data() + m_str->size());

            mwSize data_size[2];
            data_size[0] = 1;
            data_size[1] = data_utf16.size();

            mxArray* mx_str = mxCreateCharArray(2, data_size);
            uint16_t* mx_str_ptr = (uint16_t*)mxGetData(mx_str);
            memcpy(mx_str_ptr, data_utf16.c_str(), data_size[1] * sizeof(uint16_t));

            return mx_str;
        }

        if (tdef->Type == DataTypes_namedtype_t)
        {
            std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
            RR_SHARED_PTR<NamedTypeDefinition> nt = tdef->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), stub);
            switch (nt->RRDataType())
            {
            case DataTypes_structure_t: {
                RR_INTRUSIVE_PTR<MessageElementNestedElementList> mstruct =
                    m->CastDataToNestedList(DataTypes_structure_t);
                std::vector<RR_INTRUSIVE_PTR<MessageElement> >& melements = mstruct->Elements;

                boost::shared_ptr<ServiceEntryDefinition> d = rr_cast<ServiceEntryDefinition>(nt);

                boost::shared_array<const char*> fieldnames(new const char*[d->Members.size()]);

                for (size_t i = 0; i < d->Members.size(); i++)
                {
                    fieldnames[i] = d->Members[i]->Name.c_str(); // NOLINT
                }

                mwSize dims = 1;
                mxArray* o = mxCreateStructArray(1, &dims, (int)d->Members.size(), fieldnames.get());

                RR_INTRUSIVE_PTR<MessageElement> elem;

                int fieldnumber = 0;
                BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, d->Members)
                {
                    boost::shared_ptr<PropertyDefinition> p = rr_cast<PropertyDefinition>(e);

                    push_field_level(p->Name, p->Type);
                    elem = MessageElement::FindElement(melements, e->Name);

                    mxArray* data = UnpackMessageElementToMxArray(elem, p->Type, stub);

                    mxSetFieldByNumber(o, 0, fieldnumber, data);

                    fieldnumber++;
                    pop_field_level();
                }

                return o;
            }
            case DataTypes_enum_t: {
                RR_SHARED_PTR<TypeDefinition> enum_type = RR_MAKE_SHARED<TypeDefinition>();
                enum_type->Type = DataTypes_int32_t;
                enum_type->Name = "value";
                return UnpackMessageElementToMxArray(m, enum_type, stub);
            }
            case DataTypes_pod_t: {
                switch (tdef->ArrayType)
                {
                case DataTypes_ArrayTypes_none:
                case DataTypes_ArrayTypes_array: {
                    return UnpackMessageElementToMxArray_pod(m, tdef, stub);
                }
                case DataTypes_ArrayTypes_multidimarray: {
                    RR_INTRUSIVE_PTR<MessageElementNestedElementList> mm =
                        m->CastDataToNestedList(DataTypes_pod_multidimarray_t);
                    if (!mm)
                        throw DataTypeException("Invalid pod array");

                    std::vector<mwSize> dims = RRArrayToVector<mwSize>(
                        MessageElement::FindElement(mm->Elements, "dims")->CastData<RRArray<uint32_t> >());

                    RR_INTRUSIVE_PTR<MessageElement> array = MessageElement::FindElement(mm->Elements, "array");
                    if (!array)
                        throw DataTypeException("Invalid PodMultiDimArray");
                    boost::shared_ptr<TypeDefinition> type2;
                    if (tdef)
                    {
                        type2 = boost::make_shared<TypeDefinition>();
                        tdef->CopyTo(*type2);
                        type2->ArrayType = DataTypes_ArrayTypes_array;
                        type2->ArrayVarLength = true;
                        type2->ArrayLength.clear();
                    }
                    mxArray* pod_array = UnpackMessageElementToMxArray_pod(array, type2, stub);
                    if (mxSetDimensions(pod_array, &dims[0], dims.size()))
                    {
                        throw DataTypeException("Dimensions mismatch for pod multidimarray");
                    }
                    return pod_array;
                }
                default:
                    throw DataTypeException("Invalid pod type");
                }
            }
            case DataTypes_namedarray_t: {
                RR_INTRUSIVE_PTR<RRBaseArray> namedarray_array;
                RR_SHARED_PTR<ServiceEntryDefinition> namedarray_def = rr_cast<ServiceEntryDefinition>(nt);
                boost::tuple<DataTypes, size_t> namedarray_info =
                    GetNamedArrayElementTypeAndCount(namedarray_def, empty_defs, RobotRaconteurNode::sp(), stub);

                std::vector<size_t> dims;
                dims.push_back(namedarray_info.get<1>());

                switch (tdef->ArrayType)
                {
                case DataTypes_ArrayTypes_none:
                case DataTypes_ArrayTypes_array: {
                    RR_INTRUSIVE_PTR<MessageElementNestedElementList> mm =
                        m->CastDataToNestedList(DataTypes_namedarray_array_t);
                    if (!mm)
                        throw DataTypeException("Invalid namedarray");

                    namedarray_array = MessageElement::FindElement(mm->Elements, "array")->CastData<RRBaseArray>();
                    if (!namedarray_array)
                        throw DataTypeException("Invalid namedarray");
                    if (namedarray_array->GetTypeID() != namedarray_info.get<0>())
                        throw DataTypeException("Invalid namedarray type");
                    if (namedarray_array->size() % namedarray_info.get<1>() != 0)
                        throw DataTypeException("Invalid namedarray length");
                    size_t array_len = namedarray_array->size() / namedarray_info.get<1>();
                    if ((tdef->ArrayType != DataTypes_ArrayTypes_none) && tdef->ArrayLength.at(0) != 0)
                    {
                        if (tdef->ArrayVarLength)
                        {
                            if (array_len > tdef->ArrayLength.at(0))
                                throw DataTypeException("namedarray exceeds maximum length");
                        }
                        else
                        {
                            if (array_len != tdef->ArrayLength.at(0))
                                throw DataTypeException("namedarray length mismatch");
                        }
                    }
                    if (tdef->ArrayType == DataTypes_ArrayTypes_none)
                    {
                        if (array_len != 1)
                            throw DataTypeException("Expected scalar");
                    }
                    dims.push_back(array_len);
                    return GetMxArrayFromRRArray(namedarray_array, dims);
                }
                case DataTypes_ArrayTypes_multidimarray: {
                    RR_INTRUSIVE_PTR<MessageElementNestedElementList> mm1 =
                        m->CastDataToNestedList(DataTypes_namedarray_multidimarray_t);
                    if (!mm1)
                        throw DataTypeException("Invalid namedarray");

                    std::vector<size_t> dims1 = RRArrayToVector<mwSize>(
                        MessageElement::FindElement(mm1->Elements, "dims")->CastData<RRArray<uint32_t> >());
                    if (!tdef->ArrayVarLength)
                    {
                        if (dims1.size() != tdef->ArrayLength.size())
                            throw DataTypeException("namedarray dimension mismatch");
                        for (size_t i = 0; i < dims1.size(); i++)
                        {
                            if (dims1.at(i) != tdef->ArrayLength.at(i))
                                throw DataTypeException("namedarray dimension mismatch");
                        }
                    }

                    size_t expected_n_elems = boost::numeric_cast<size_t>(
                        boost::accumulate(dims1, 1, std::multiplies<int32_t>()) * namedarray_info.get<1>());

                    RR_INTRUSIVE_PTR<MessageElementNestedElementList> mm =
                        MessageElement::FindElement(mm1->Elements, "array")
                            ->CastDataToNestedList(DataTypes_namedarray_array_t);
                    if (!mm)
                        throw DataTypeException("Invalid namedarray");

                    namedarray_array = MessageElement::FindElement(mm->Elements, "array")->CastData<RRBaseArray>();
                    if (!namedarray_array)
                        throw DataTypeException("Invalid namedarray");
                    if (namedarray_array->GetTypeID() != namedarray_info.get<0>())
                        throw DataTypeException("Invalid namedarray type");
                    if (namedarray_array->size() != expected_n_elems)
                        throw DataTypeException("namedarray dimension mismatch");

                    boost::range::copy(dims1, std::back_inserter(dims));
                    return GetMxArrayFromRRArray(namedarray_array, dims);
                }
                default:
                    throw DataTypeException("Invalid namedarray type");
                }
            }
            default:
                throw DataTypeException("Unknown named type id");
            }
        }

        throw DataTypeException("Unknown data type");
    }

    std::vector<std::string> field_name;
    std::vector<RR_SHARED_PTR<TypeDefinition> > field_type;

    void push_field_level(const std::string& new_var, const RR_SHARED_PTR<TypeDefinition>& new_type)
    {
        field_name.push_back(new_var);
        field_type.push_back(new_type);
    }

    void pop_field_level()
    {
        field_name.pop_back();
        field_type.pop_back();
    }

    boost::tuple<std::string, RR_SHARED_PTR<TypeDefinition> > get_current_field()
    {
        if (field_name.empty())
        {
            return boost::make_tuple("**internal error**", RR_SHARED_PTR<TypeDefinition>());
        }

        std::string name = boost::join(field_name, ".");
        boost::replace_all(name, ".[", "[");
        RR_SHARED_PTR<TypeDefinition> type = field_type.back();

        return boost::make_tuple(name, type);
    }

    std::string get_exception_message(const std::string& msg)
    {
        boost::tuple<std::string, RR_SHARED_PTR<TypeDefinition> > f = get_current_field();

        std::string msg2 = msg + " for field \"" + f.get<0>() + "\"";
        if (f.get<1>())
        {
            RR_SHARED_PTR<TypeDefinition> f2 = f.get<1>()->Clone();
            if (f2->Type == DataTypes_namedtype_t)
            {
                try
                {
                    f2->TypeString = f.get<1>()->ResolveNamedType()->ResolveQualifiedName();
                }
                catch (std::exception&)
                {}
            }
            std::vector<std::string> f_split;
            std::string f2_str = f2->ToString();
            boost::split(f_split, f2_str, boost::is_any_of(" \t"), boost::token_compress_on);
            msg2 += " expected Robot Raconteur type \"" + f_split.at(0) + "\"";
        }

        return msg2;
    }
};

mxArray* UnpackMessageElementToMxArray_pod(const RR_INTRUSIVE_PTR<MessageElement>& element,
                                           const boost::shared_ptr<TypeDefinition>& type1,
                                           const boost::shared_ptr<ServiceStub>& stub)
{
    UnpackMessageElementToMxArrayImpl u;
    if (type1)
    {
        u.push_field_level(type1->Name, type1);
    }
    else
    {
        u.push_field_level("value", RR_SHARED_PTR<TypeDefinition>());
    }
    try
    {
        return u.UnpackMessageElementToMxArray_pod(element, type1, stub);
    }
    catch (DataTypeException& e)
    {
        e.Message = u.get_exception_message(e.Message);
        throw;
    }
    catch (InvalidArgumentException& e)
    {
        e.Message = u.get_exception_message(e.Message);
        throw;
    }
}

mxArray* UnpackMessageElementToMxArray(const RR_INTRUSIVE_PTR<MessageElement>& m,
                                       const boost::shared_ptr<TypeDefinition>& tdef,
                                       const RR_SHARED_PTR<ServiceStub>& stub)
{
    UnpackMessageElementToMxArrayImpl u;
    if (tdef)
    {
        u.push_field_level(tdef->Name, tdef);
    }
    else
    {
        u.push_field_level("value", RR_SHARED_PTR<TypeDefinition>());
    }
    try
    {
        return u.UnpackMessageElementToMxArray(m, tdef, stub);
    }
    catch (DataTypeException& e)
    {
        e.Message = u.get_exception_message(e.Message);
        throw;
    }
    catch (InvalidArgumentException& e)
    {
        e.Message = u.get_exception_message(e.Message);
        throw;
    }
}

// MexServiceFactory

MexServiceFactory::MexServiceFactory(const std::string& defstring)
{
    this->defstring = defstring;
    servicedef = RR_MAKE_SHARED<ServiceDefinition>();
    servicedef->FromString(defstring);
}

MexServiceFactory::MexServiceFactory(const boost::shared_ptr<RobotRaconteur::ServiceDefinition>& def)
{
    this->defstring = def->ToString();
    servicedef = def;
}

std::string MexServiceFactory::GetServiceName() { return servicedef->Name; }

std::string MexServiceFactory::DefString() { return defstring; }

RR_SHARED_PTR<ServiceDefinition> MexServiceFactory::ServiceDef() { return servicedef; }

RR_SHARED_PTR<RobotRaconteur::StructureStub> MexServiceFactory::FindStructureStub(boost::string_ref s)
{
    RR_UNUSED(s);
    throw ServiceException("Invalid for wrapped service type");
}

RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> MexServiceFactory::PackStructure(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure>& structin)
{
    RR_UNUSED(structin);
    throw ServiceException("Invalid for wrapped service type");
}

RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> MexServiceFactory::UnpackStructure(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& mstructin)
{
    RR_UNUSED(mstructin);
    throw ServiceException("Invalid for wrapped service type");
}
RR_INTRUSIVE_PTR<MessageElementNestedElementList> MexServiceFactory::PackPodArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray>& structure)
{
    RR_UNUSED(structure);
    throw ServiceException("Invalid for wrapped service type");
}
RR_INTRUSIVE_PTR<RRPodBaseArray> MexServiceFactory::UnpackPodArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure)
{
    RR_UNUSED(structure);
    throw ServiceException("Invalid for wrapped service type");
}
RR_INTRUSIVE_PTR<MessageElementNestedElementList> MexServiceFactory::PackPodMultiDimArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray>& structure)
{
    RR_UNUSED(structure);
    throw ServiceException("Invalid for wrapped service type");
}
RR_INTRUSIVE_PTR<RRPodBaseMultiDimArray> MexServiceFactory::UnpackPodMultiDimArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure)
{
    RR_UNUSED(structure);
    throw ServiceException("Invalid for wrapped service type");
}
RR_INTRUSIVE_PTR<MessageElementNestedElementList> MexServiceFactory::PackNamedArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray>& structure)
{
    RR_UNUSED(structure);
    throw ServiceException("Invalid for wrapped service type");
}
RR_INTRUSIVE_PTR<RRNamedBaseArray> MexServiceFactory::UnpackNamedArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure)
{
    RR_UNUSED(structure);
    throw ServiceException("Invalid for wrapped service type");
}
RR_INTRUSIVE_PTR<MessageElementNestedElementList> MexServiceFactory::PackNamedMultiDimArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray>& structure)
{
    RR_UNUSED(structure);
    throw ServiceException("Invalid for wrapped service type");
}
RR_INTRUSIVE_PTR<RRNamedBaseMultiDimArray> MexServiceFactory::UnpackNamedMultiDimArray(
    const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure)
{
    RR_UNUSED(structure);
    throw ServiceException("Invalid for wrapped service type");
}
RR_SHARED_PTR<RobotRaconteur::ServiceStub> MexServiceFactory::CreateStub(
    boost::string_ref type, boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::ClientContext>& context)
{
    boost::tuple<boost::string_ref, boost::string_ref> res = SplitQualifiedName(type);

    std::string servicetype = res.get<0>().to_string();
    std::string objecttype = res.get<1>().to_string();
    if (servicetype != GetServiceName())
        return RobotRaconteur::RobotRaconteurNode::s()->GetServiceType(servicetype)->CreateStub(type, path, context);
    BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& ee, servicedef->Objects)
    {
        if (ee->Name == objecttype)
        {
            RR_SHARED_PTR<MexServiceStub> out = RR_MAKE_SHARED<MexServiceStub>(path, ee, context);
            out->RRInitStub();
            return out;
        }
    }
    throw RobotRaconteur::ServiceException("Invalid service stub type.");
}

RR_SHARED_PTR<RobotRaconteur::ServiceSkel> MexServiceFactory::CreateSkel(
    boost::string_ref type, boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::RRObject>& obj,
    const RR_SHARED_PTR<RobotRaconteur::ServerContext>& context)
{

    boost::tuple<boost::string_ref, boost::string_ref> res = SplitQualifiedName(type);

    std::string servicetype = res.get<0>().to_string();
    std::string objecttype = res.get<1>().to_string();
    if (servicetype != GetServiceName())
        throw ServiceException("MATLAB cannot create objref skels");

    RR_SHARED_PTR<MexServiceSkel> out = RR_MAKE_SHARED<MexServiceSkel>();
    out->Init(path.to_string(), obj, context);
    return out;

    throw RobotRaconteur::ServiceException("Invalid service skel type.");
}

// Mex Dynamic Service Factory
RR_SHARED_PTR<ServiceFactory> MexDynamicServiceFactory::CreateServiceFactory(boost::string_ref def)
{
    return RR_MAKE_SHARED<MexServiceFactory>(def.to_string());
}

std::vector<RR_SHARED_PTR<ServiceFactory> > MexDynamicServiceFactory::CreateServiceFactories(
    const std::vector<std::string>& def)
{
    std::vector<RR_SHARED_PTR<ServiceFactory> > out;
    BOOST_FOREACH (const std::string& ee, def)
    {
        out.push_back(RR_MAKE_SHARED<MexServiceFactory>(ee));
    }
    return out;
}

// MexServiceStub

MexServiceStub::MexServiceStub(boost::string_ref path, const RR_SHARED_PTR<ServiceEntryDefinition>& type,
                               const RR_SHARED_PTR<RobotRaconteur::ClientContext>& c)
    : RobotRaconteur::ServiceStub(path, c)
{
    RR_objecttype = type;
    stubid = 0;
    // stubptr=NULL;
    EnableEvents = false;
    eventconnections_count = 10;
}

void MexServiceStub::RRInitStub()
{
    BOOST_FOREACH (RR_SHARED_PTR<MemberDefinition>& e, RR_objecttype->Members)
    {

        RR_SHARED_PTR<PipeDefinition> p = boost::dynamic_pointer_cast<PipeDefinition>(e);
        if (p)
        {
            RR_SHARED_PTR<MexPipeClient> c =
                RR_MAKE_SHARED<MexPipeClient>(p->Name, shared_from_this(), p->Type, p->IsUnreliable(), p->Direction());
            pipes.insert(std::make_pair(p->Name, c));
        }

        RR_SHARED_PTR<WireDefinition> w = boost::dynamic_pointer_cast<WireDefinition>(e);
        if (w)
        {
            RR_SHARED_PTR<MexWireClient> c =
                RR_MAKE_SHARED<MexWireClient>(w->Name, shared_from_this(), w->Type, w->Direction());
            wires.insert(std::make_pair(w->Name, c));
        }

        RR_SHARED_PTR<MemoryDefinition> m = boost::dynamic_pointer_cast<MemoryDefinition>(e);
        if (m)
        {

            if (IsTypeNumeric(m->Type->Type))
            {
                if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
                {
                    RR_SHARED_PTR<ArrayMemoryBase> o;
                    switch (m->Type->Type)
                    {
                    case DataTypes_double_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<double> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_single_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<float> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_int8_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<int8_t> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_uint8_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<uint8_t> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_int16_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<int16_t> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_uint16_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<uint16_t> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_int32_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<int32_t> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_uint32_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<uint32_t> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_int64_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<int64_t> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_uint64_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<uint64_t> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_bool_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<rr_bool> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_cdouble_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<cdouble> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    case DataTypes_csingle_t:
                        o = RR_MAKE_SHARED<ArrayMemoryClient<cfloat> >(m->Name, shared_from_this(), m->Direction());
                        break;
                    default:
                        throw DataTypeException("Invalid memory data type");
                    }
                    arraymemories.insert(std::make_pair(m->Name, o));
                }
                else
                {
                    RR_SHARED_PTR<MultiDimArrayMemoryBase> o;
                    switch (m->Type->Type)
                    {
                    case DataTypes_double_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<double> >(m->Name, shared_from_this(),
                                                                               m->Direction());
                        break;
                    case DataTypes_single_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<float> >(m->Name, shared_from_this(),
                                                                              m->Direction());
                        break;
                    case DataTypes_int8_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<int8_t> >(m->Name, shared_from_this(),
                                                                               m->Direction());
                        break;
                    case DataTypes_uint8_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<uint8_t> >(m->Name, shared_from_this(),
                                                                                m->Direction());
                        break;
                    case DataTypes_int16_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<int16_t> >(m->Name, shared_from_this(),
                                                                                m->Direction());
                        break;
                    case DataTypes_uint16_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<uint16_t> >(m->Name, shared_from_this(),
                                                                                 m->Direction());
                        break;
                    case DataTypes_int32_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<int32_t> >(m->Name, shared_from_this(),
                                                                                m->Direction());
                        break;
                    case DataTypes_uint32_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<uint32_t> >(m->Name, shared_from_this(),
                                                                                 m->Direction());
                        break;
                    case DataTypes_int64_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<int64_t> >(m->Name, shared_from_this(),
                                                                                m->Direction());
                        break;
                    case DataTypes_uint64_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<uint64_t> >(m->Name, shared_from_this(),
                                                                                 m->Direction());
                        break;
                    case DataTypes_bool_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<rr_bool> >(m->Name, shared_from_this(),
                                                                                m->Direction());
                        break;
                    case DataTypes_cdouble_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<cdouble> >(m->Name, shared_from_this(),
                                                                                m->Direction());
                        break;
                    case DataTypes_csingle_t:
                        o = RR_MAKE_SHARED<MultiDimArrayMemoryClient<cfloat> >(m->Name, shared_from_this(),
                                                                               m->Direction());
                        break;
                    default:
                        throw InvalidArgumentException("Invalid memory data type");
                    }
                    multidimarraymemories.insert(std::make_pair(m->Name, o));
                }
            }
            else
            {
                RR_SHARED_PTR<ServiceEntryDefinition> entry =
                    rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType());

                switch (entry->EntryType)
                {
                case DataTypes_pod_t: {

                    if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
                    {
                        std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
                        size_t element_size = EstimatePodPackedElementSize(entry, other_defs, RRGetNode());

                        RR_SHARED_PTR<ArrayMemoryBase> o = RR_MAKE_SHARED<MexPodArrayMemoryClient>(
                            m->Name, shared_from_this(), element_size, m->Direction(), m->Type);

                        arraymemories.insert(std::make_pair(m->Name, o));
                    }
                    else
                    {
                        std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
                        size_t element_size = EstimatePodPackedElementSize(entry, other_defs, RRGetNode());

                        RR_SHARED_PTR<MultiDimArrayMemoryBase> o = RR_MAKE_SHARED<MexPodMultiDimArrayMemoryClient>(
                            m->Name, shared_from_this(), element_size, m->Direction(), m->Type);

                        multidimarraymemories.insert(std::make_pair(m->Name, o));
                    }
                    break;
                case DataTypes_namedarray_t:
                    if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
                    {
                        std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
                        boost::tuple<DataTypes, size_t> array_info =
                            GetNamedArrayElementTypeAndCount(entry, other_defs, RRGetNode());

                        RR_SHARED_PTR<ArrayMemoryBase> o = RR_MAKE_SHARED<MexNamedArrayMemoryClient>(
                            m->Name, shared_from_this(), array_info, m->Direction(), m->Type);

                        arraymemories.insert(std::make_pair(m->Name, o));
                    }
                    else
                    {
                        std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs;
                        boost::tuple<DataTypes, size_t> array_info =
                            GetNamedArrayElementTypeAndCount(entry, other_defs, RRGetNode());

                        RR_SHARED_PTR<MultiDimArrayMemoryBase> o = RR_MAKE_SHARED<MexNamedMultiDimArrayMemoryClient>(
                            m->Name, shared_from_this(), array_info, m->Direction(), m->Type);

                        multidimarraymemories.insert(std::make_pair(m->Name, o));
                    }
                    break;
                }
                default:
                    throw InvalidArgumentException("Invalid memory data type");
                }
            }
        }
    }
}

mxArray* MexServiceStub::PropertyGet(const std::string& PropertyName)
{
    boost::shared_ptr<PropertyDefinition> pdef;
    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
    {
        if (PropertyName == e->Name)
        {
            pdef = rr_cast<PropertyDefinition>(e);
            break;
        }
    }
    if (!pdef)
        throw MemberNotFoundException("Property " + PropertyName + " not found");

    RR_INTRUSIVE_PTR<MessageEntry> req = CreateMessageEntry(MessageEntryType_PropertyGetReq, PropertyName);
    RR_INTRUSIVE_PTR<MessageEntry> res = ProcessRequest(req);
    return UnpackMessageElementToMxArray(res->FindElement("value"), pdef->Type, shared_from_this());
}

void MexServiceStub::PropertySet(const std::string& PropertyName, const mxArray* value)
{
    boost::shared_ptr<PropertyDefinition> pdef;
    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
    {
        if (PropertyName == (e)->Name)
        {
            pdef = rr_cast<PropertyDefinition>(e);
            break;
        }
    }
    if (!pdef)
        throw MemberNotFoundException("Property " + PropertyName + " not found");

    RR_INTRUSIVE_PTR<MessageElement> value2 = PackMxArrayToMessageElement(value, pdef->Type, shared_from_this());
    RR_INTRUSIVE_PTR<MessageEntry> req = CreateMessageEntry(MessageEntryType_PropertySetReq, PropertyName);
    value2->ElementName = "value";
    req->AddElement(value2);
    RR_INTRUSIVE_PTR<MessageEntry> res = ProcessRequest(req);
}

mxArray* CreateGeneratorClientMxArray(int32_t id)
{
    // boost::recursive_mutex::scoped_lock lock(pipeendpoints_lock);
    // pipeendpoints_count++;
    mxArray* lhs[1];
    mxArray* rhs[2];
    rhs[0] = CreateInt32Scalar(RR_MEX_GENERATOR_CLIENT);
    rhs[1] = CreateInt32Scalar(id);
    int merror = mexCallMATLAB(1, lhs, 2, rhs, "RobotRaconteurGenerator");
    if (merror)
    {
        throw InternalErrorException("Internal error");
    }
    return lhs[0];
}

mxArray* MexServiceStub::FunctionCall(const std::string& FunctionName, std::vector<const mxArray*> args)
{
    boost::shared_ptr<FunctionDefinition> fdef;
    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
    {
        if (FunctionName == (e)->Name)
        {
            fdef = rr_cast<FunctionDefinition>(e);
            break;
        }
    }
    if (!fdef)
        throw MemberNotFoundException("Function " + FunctionName + " not found");

    if (!fdef->IsGenerator())
    {

        if (args.size() != fdef->Parameters.size())
            throw InvalidArgumentException(FunctionName + " expects " +
                                           boost::lexical_cast<std::string>(fdef->Parameters.size()) + " arguments");

        RR_INTRUSIVE_PTR<MessageEntry> req = CreateMessageEntry(MessageEntryType_FunctionCallReq, FunctionName);

        for (size_t i = 0; i < fdef->Parameters.size(); i++)
        {
            req->elements.push_back(PackMxArrayToMessageElement(args[i], fdef->Parameters[i], shared_from_this()));
        }

        RR_INTRUSIVE_PTR<MessageEntry> res = ProcessRequest(req);
        if (fdef->ReturnType->Type == DataTypes_void_t)
        {

            return mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
        }
        else
        {
            return UnpackMessageElementToMxArray(res->FindElement("return"), fdef->ReturnType, shared_from_this());
        }
    }
    else
    {
        RR_SHARED_PTR<TypeDefinition> return_type;
        RR_SHARED_PTR<TypeDefinition> param_type;
        std::vector<RR_SHARED_PTR<TypeDefinition> > params;
        if (!fdef->Parameters.empty() && fdef->Parameters.back()->ContainerType == DataTypes_ContainerTypes_generator)
        {
            std::copy(fdef->Parameters.begin(), --fdef->Parameters.end(), std::back_inserter(params));
            param_type = RR_MAKE_SHARED<TypeDefinition>();
            fdef->Parameters.back()->CopyTo(*param_type);
            param_type->RemoveContainers();
        }
        else
        {
            boost::range::copy(fdef->Parameters, std::back_inserter(params));
        }

        if (fdef->ReturnType->Type != DataTypes_void_t)
        {
            return_type = RR_MAKE_SHARED<TypeDefinition>();
            fdef->ReturnType->CopyTo(*return_type);
            return_type->RemoveContainers();
        }

        if (args.size() != params.size())
            throw InvalidArgumentException(FunctionName + " expects " +
                                           boost::lexical_cast<std::string>(params.size()) + " arguments");

        RR_INTRUSIVE_PTR<MessageEntry> req = CreateMessageEntry(MessageEntryType_FunctionCallReq, FunctionName);

        for (size_t i = 0; i < params.size(); i++)
        {
            req->elements.push_back(PackMxArrayToMessageElement(args[i], params[i], shared_from_this()));
        }

        RR_INTRUSIVE_PTR<MessageEntry> res = ProcessRequest(req);

        int32_t generator_index = RRArrayToScalar(res->FindElement("index")->CastData<RRArray<int32_t> >());

        boost::mutex::scoped_lock lock(generators_lock);
        RR_SHARED_PTR<MexGeneratorClient> gen = RR_MAKE_SHARED<MexGeneratorClient>(
            fdef->Name, generator_index, shared_from_this(), return_type, param_type);
        gen->generatorid = ++generators_count;
        generators.insert(std::make_pair(gen->generatorid, gen));
        return CreateGeneratorClientMxArray(gen->generatorid);
    }
}

void MexServiceStub::AsyncPropertyGet(const std::string& PropertyName, const RR_SHARED_PTR<mxArray>& handler,
                                      const RR_SHARED_PTR<mxArray>& param, uint32_t timeout)
{
    boost::shared_ptr<PropertyDefinition> pdef;
    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
    {
        if (PropertyName == (e)->Name)
        {
            pdef = rr_cast<PropertyDefinition>(e);
            break;
        }
    }
    if (!pdef)
        throw MemberNotFoundException("Property " + PropertyName + " not found");

    RR_INTRUSIVE_PTR<MessageEntry> req = CreateMessageEntry(MessageEntryType_PropertyGetReq, PropertyName);
    AsyncProcessRequest(req,
                        boost::bind(&MexServiceStub::EndAsyncPropertyGet, rr_cast<MexServiceStub>(shared_from_this()),
                                    RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), handler, param, pdef),
                        boost::numeric_cast<int32_t>(timeout));
}

void MexServiceStub::EndAsyncPropertyGet(const RR_INTRUSIVE_PTR<MessageEntry>& res,
                                         const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err,
                                         const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param,
                                         const RR_SHARED_PTR<PropertyDefinition>& pdef)
{
    RR_SHARED_PTR<MexAsyncResult> ares;

    if (err)
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(handler, param, err);
    }
    else if (res->Error != RobotRaconteur::MessageErrorType_None)
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(
            handler, param, RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(res));
    }
    else
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(handler, param, RR_SHARED_PTR<RobotRaconteurException>(), pdef->Type,
                                              res->FindElement("value"));
    }

    boost::mutex::scoped_lock lock(async_results_lock);
    async_results.push(ares);
}

void MexServiceStub::AsyncPropertySet(const std::string& PropertyName, const mxArray* value,
                                      const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param,
                                      uint32_t timeout)
{
    boost::shared_ptr<PropertyDefinition> pdef;
    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
    {
        if (PropertyName == (e)->Name)
        {
            pdef = rr_cast<PropertyDefinition>(e);
            break;
        }
    }
    if (!pdef)
        throw MemberNotFoundException("Property " + PropertyName + " not found");

    RR_INTRUSIVE_PTR<MessageElement> value2 = PackMxArrayToMessageElement(value, pdef->Type, shared_from_this());
    RR_INTRUSIVE_PTR<MessageEntry> req = CreateMessageEntry(MessageEntryType_PropertySetReq, PropertyName);
    value2->ElementName = "value";
    req->AddElement(value2);
    AsyncProcessRequest(req,
                        boost::bind(&MexServiceStub::EndAsyncPropertySet, rr_cast<MexServiceStub>(shared_from_this()),
                                    RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), handler, param, pdef),
                        boost::numeric_cast<int32_t>(timeout));
}

void MexServiceStub::EndAsyncPropertySet(const RR_INTRUSIVE_PTR<MessageEntry>& res,
                                         const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err,
                                         const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param,
                                         const RR_SHARED_PTR<PropertyDefinition>& pdef)
{
    RR_UNUSED(pdef);
    RR_SHARED_PTR<MexAsyncResult> ares;

    if (err)
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(handler, param, err);
    }
    else if (res->Error != RobotRaconteur::MessageErrorType_None)
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(
            handler, param, RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(res));
    }
    else
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(handler, param, RR_SHARED_PTR<RobotRaconteurException>());
    }

    boost::mutex::scoped_lock lock(async_results_lock);
    async_results.push(ares);
}

void MexServiceStub::AsyncFunctionCall(const std::string& FunctionName, std::vector<const mxArray*> args,
                                       const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param,
                                       uint32_t timeout)
{
    boost::shared_ptr<FunctionDefinition> fdef;
    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
    {
        if (FunctionName == (e)->Name)
        {
            fdef = rr_cast<FunctionDefinition>(e);
            break;
        }
    }
    if (!fdef)
        throw MemberNotFoundException("Function " + FunctionName + " not found");

    if (fdef->IsGenerator())
        throw InvalidOperationException("Not supported for generator functions");

    if (args.size() != fdef->Parameters.size())
        throw InvalidArgumentException(FunctionName + " expects " +
                                       boost::lexical_cast<std::string>(fdef->Parameters.size()) + " arguments");

    RR_INTRUSIVE_PTR<MessageEntry> req = CreateMessageEntry(MessageEntryType_FunctionCallReq, FunctionName);

    for (size_t i = 0; i < fdef->Parameters.size(); i++)
    {
        req->elements.push_back(PackMxArrayToMessageElement(args[i], fdef->Parameters[i], shared_from_this()));
    }

    AsyncProcessRequest(req,
                        boost::bind(&MexServiceStub::EndAsyncFunctionCall, rr_cast<MexServiceStub>(shared_from_this()),
                                    RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), handler, param, fdef),
                        boost::numeric_cast<int32_t>(timeout));
}

void MexServiceStub::EndAsyncFunctionCall(const RR_INTRUSIVE_PTR<MessageEntry>& res,
                                          const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err,
                                          const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param,
                                          const RR_SHARED_PTR<FunctionDefinition>& fdef)
{
    RR_SHARED_PTR<MexAsyncResult> ares;
    if (err)
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(handler, param, err);
    }
    else if (res->Error != RobotRaconteur::MessageErrorType_None)
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(
            handler, param, RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(res));
    }
    else
    {

        ares = RR_MAKE_SHARED<MexAsyncResult>(handler, param, RR_SHARED_PTR<RobotRaconteurException>());
    }

    if (fdef->ReturnType->Type != DataTypes_void_t)
    {
        ares->return_type = fdef->ReturnType;
        if (!ares->error)
        {

            ares->return_value = res->FindElement("return");
        }
    }
    boost::mutex::scoped_lock lock(async_results_lock);
    async_results.push(ares);
}

void MexServiceStub::DispatchEvent(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{
    if (!EnableEvents)
        return;
    boost::recursive_mutex::scoped_lock lock2(events_lock);
    events.push_back(m);

    // throw MemberNotFoundException("Event member not found");
}

void MexServiceStub::DispatchPipeMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{
    std::map<std::string, RR_SHARED_PTR<MexPipeClient> >::iterator e = pipes.find(m->MemberName.str().to_string());
    if (e == pipes.end())
        throw MemberNotFoundException("Pipe Member Not Found");
    e->second->PipePacketReceived(m);
}

void MexServiceStub::DispatchWireMessage(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{
    std::map<std::string, RR_SHARED_PTR<MexWireClient> >::iterator e = wires.find(m->MemberName.str().to_string());
    if (e == wires.end())
        throw MemberNotFoundException("Pipe Member Not Found");
    e->second->WirePacketReceived(m);
}

void MexServiceStub::RRClose()
{

    if (stubid != 0)
    {

        try
        {
            boost::recursive_mutex::scoped_lock lock(stubs_lock);
            stubs.erase(stubid);
        }
        catch (...)
        {}

        try
        {
            boost::recursive_mutex::scoped_lock lock(eventconnections_lock);

            eventconnections.clear();
        }
        catch (...)
        {}

        try
        {
            boost::recursive_mutex::scoped_lock lock(callback_lock);

            callbacks.clear();
        }
        catch (...)
        {}
    }

    ServiceStub::RRClose();
}

std::string MexServiceStub::RRType() { return RR_objecttype->Name; }

RR_SHARED_PTR<PipeClientBase> MexServiceStub::RRGetPipeClient(boost::string_ref membername)
{
    std::map<std::string, RR_SHARED_PTR<MexPipeClient> >::iterator e = pipes.find(membername.to_string());
    if (e == pipes.end())
        throw MemberNotFoundException("Pipe Member Not Found");
    return e->second;
}

RR_SHARED_PTR<WireClientBase> MexServiceStub::RRGetWireClient(boost::string_ref membername)
{
    std::map<std::string, RR_SHARED_PTR<MexWireClient> >::iterator e = wires.find(membername.to_string());
    if (e == wires.end())
        throw MemberNotFoundException("Wire Member Not Found");
    return e->second;
}

mxArray* CreatePipeEndpointMxArray(int32_t id)
{
    // boost::recursive_mutex::scoped_lock lock(pipeendpoints_lock);
    // pipeendpoints_count++;
    mxArray* lhs[1];
    mxArray* rhs[2];
    rhs[0] = CreateInt32Scalar(RR_MEX_PIPE_ENDPOINT);
    rhs[1] = CreateInt32Scalar(id);
    int merror = mexCallMATLAB(1, lhs, 2, rhs, "RobotRaconteurPipeEndpoint");
    if (merror)
    {
        throw InternalErrorException("Internal error");
    }
    return lhs[0];
}

mxArray* CreateWireConnectionMxArray(int32_t id)
{
    // boost::recursive_mutex::scoped_lock lock(pipeendpoints_lock);
    // pipeendpoints_count++;
    mxArray* lhs[1];
    mxArray* rhs[2];
    rhs[0] = CreateInt32Scalar(RR_MEX_WIRE_CONNECTION);
    rhs[1] = CreateInt32Scalar(id);
    int merror = mexCallMATLAB(1, lhs, 2, rhs, "RobotRaconteurWireConnection");
    if (merror)
    {
        throw InternalErrorException("Internal error");
    }
    return lhs[0];
}

mxArray* MexServiceStub::subsref(const mxArray* S)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);
    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteurMex error");

    std::string type = mxToString(mxGetField(S, 0, "type"));

    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    if (!mxIsChar(mxGetField(S, 0, "subs")))
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    boost::shared_ptr<MemberDefinition> memdef;

    if (boost::starts_with(membername, "async_"))
    {

        if (c1 != 2)
            throw InvalidArgumentException("async operation expects () notation");
        std::string type2 = mxToString(mxGetField(S, 1, "type"));
        if (type2 != "()")
            throw InvalidArgumentException("Function call must use '()' notation");
        mxArray* cell_args = mxGetField(S, 1, "subs");

        int c2 = (int)mxGetNumberOfElements(cell_args);

        if (boost::starts_with(membername, "async_get_"))
        {
            membername = boost::replace_first_copy(membername, "async_get_", "");

            BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
            {
                if ((e)->Name == membername)
                {
                    memdef = e;
                    break;
                }
            }

            if (c2 != 2 && c2 != 3)
                throw InvalidArgumentException("async_get expects 2 or 3 arguments");

            int32_t timeout = RR_TIMEOUT_INFINITE;
            if (c2 == 3)
            {
                timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 2));
            }

            // mexPrintf("async_get: %s\n",membername.c_str());
            RR_SHARED_PTR<mxArray> handler(mxDuplicateArray(mxGetCell(cell_args, 0)), rr_mxDestroyArray);
            RR_SHARED_PTR<mxArray> param(mxDuplicateArray(mxGetCell(cell_args, 1)), rr_mxDestroyArray);

            mexMakeArrayPersistent(handler.get());
            mexMakeArrayPersistent(param.get());

            if (!::mxIsFunctionHandle(handler.get()))
                throw InvalidArgumentException("handler must be a function handle");

            AsyncPropertyGet(membername, handler, param, timeout);

            return mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
        }
        else if (boost::starts_with(membername, "async_set_"))
        {
            membername = boost::replace_first_copy(membername, "async_set_", "");

            BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
            {
                if ((e)->Name == membername)
                {
                    memdef = e;
                    break;
                }
            }

            if (c2 != 3 && c2 != 4)
                throw InvalidArgumentException("async_get expects 2 or 3 arguments");

            int32_t timeout = RR_TIMEOUT_INFINITE;
            if (c2 == 4)
            {
                timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 3));
            }

            // mexPrintf("async_get: %s\n",membername.c_str());
            RR_SHARED_PTR<mxArray> handler(mxDuplicateArray(mxGetCell(cell_args, 1)), rr_mxDestroyArray);
            RR_SHARED_PTR<mxArray> param(mxDuplicateArray(mxGetCell(cell_args, 2)), rr_mxDestroyArray);

            mexMakeArrayPersistent(handler.get());
            mexMakeArrayPersistent(param.get());

            if (!::mxIsFunctionHandle(handler.get()))
                throw InvalidArgumentException("handler must be a function handle");

            AsyncPropertySet(membername, mxGetCell(cell_args, 0), handler, param, timeout);

            return mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
        }
        else
        {
            membername = boost::replace_first_copy(membername, "async_", "");

            BOOST_FOREACH (const boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
            {
                if ((e)->Name == membername)
                {
                    memdef = e;
                    break;
                }
            }

            RR_SHARED_PTR<FunctionDefinition> fdef = rr_cast<FunctionDefinition>(memdef);

            size_t pcount = fdef->Parameters.size();

            if (c2 != pcount + 2 && c2 != pcount + 3)
                throw InvalidArgumentException("async_func expects parameters plus 2 or 3 arguments");

            int32_t timeout = RR_TIMEOUT_INFINITE;
            if (c2 == pcount + 3)
            {
                timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, pcount + 2));
            }

            // mexPrintf("async_get: %s\n",membername.c_str());
            std::vector<const mxArray*> params;
            for (size_t i = 0; i < pcount; i++)
            {
                params.push_back(mxGetCell(cell_args, i));
            }

            RR_SHARED_PTR<mxArray> handler(mxDuplicateArray(mxGetCell(cell_args, pcount)), rr_mxDestroyArray);
            RR_SHARED_PTR<mxArray> param(mxDuplicateArray(mxGetCell(cell_args, pcount + 1)), rr_mxDestroyArray);

            mexMakeArrayPersistent(handler.get());
            mexMakeArrayPersistent(param.get());

            if (!::mxIsFunctionHandle(handler.get()))
                throw InvalidArgumentException("handler must be a function handle");

            AsyncFunctionCall(membername, params, handler, param, timeout);

            return mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
        }
    }

    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
    {
        if (e->Name == membername)
        {
            memdef = e;
            break;
        }
    }

    if (c1 == 1)
    {

        if (!memdef)
            throw MemberNotFoundException("Member " + membername + " not found in object type " + RR_objecttype->Name);

        boost::shared_ptr<PropertyDefinition> pdef = boost::dynamic_pointer_cast<PropertyDefinition>(memdef);
        boost::shared_ptr<MemoryDefinition> mdef = boost::dynamic_pointer_cast<MemoryDefinition>(memdef);
        if (!pdef && !mdef)
            throw MemberNotFoundException("Member " + membername + " is not a property or memory");

        // Property Get
        if (pdef)
        {
            return PropertyGet(membername);
        }

        // Memory client
        if (mdef)
        {
            mxArray* lhs[1];
            mxArray* rhs[3];
            rhs[0] = CreateInt32Scalar(RR_MEX_STUB);
            rhs[1] = CreateInt32Scalar(stubid);
            rhs[2] = mxCreateString(mdef->Name.c_str());
            if (mexCallMATLAB(1, lhs, 3, rhs, "RobotRaconteurMemoryClient"))
            {
                throw InternalErrorException("Robot Raconteur internal error");
            }

            return lhs[0];
        }
    }

    if (c1 == 2)
    {
        // if (!memdef) throw MemberNotFoundException("Property " + membername + " not found in object type " +
        // RR_objecttype->Name); Function call
        boost::shared_ptr<FunctionDefinition> fdef = boost::dynamic_pointer_cast<FunctionDefinition>(memdef);
        if (fdef)
        {
            std::string type2 = mxToString(mxGetField(S, 1, "type"));
            if (type2 != "()")
                throw InvalidArgumentException("Function call must use '()' notation");
            mxArray* cell_args = mxGetField(S, 1, "subs");
            size_t argc = mxGetNumberOfElements(cell_args);
            std::vector<const mxArray*> args;
            for (size_t i = 0; i < argc; i++)
            {
                mxArray* arg = mxGetCell(cell_args, i);
                if (arg == NULL)
                    throw InvalidArgumentException("RobotRaconteurMex error");
                args.push_back(arg);
            }

            return FunctionCall(membername, args);
        }

        boost::shared_ptr<MemberDefinition> memdef2;

        BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
        {
            if ("get_" + (e)->Name == membername)
            {
                memdef2 = e;
                break;
            }
        }

        boost::shared_ptr<ObjRefDefinition> odef = boost::dynamic_pointer_cast<ObjRefDefinition>(memdef2);
        if (odef)
        {
            std::string type2 = mxToString(mxGetField(S, 1, "type"));
            if (type2 != "()")
                throw InvalidArgumentException("Function call must use '()' notation");
            mxArray* cell_args = mxGetField(S, 1, "subs");
            size_t argc = mxGetNumberOfElements(cell_args);

            boost::shared_ptr<MexServiceStub> ostub;

            if (odef->ArrayType == DataTypes_ArrayTypes_none && odef->ContainerType == DataTypes_ContainerTypes_none)
            {
                if (argc != 0)
                    throw InvalidArgumentException("ObjRef " + odef->Name + " is not indexed");
                ostub = rr_cast<MexServiceStub>(FindObjRef(odef->Name));
            }
            else if (odef->ArrayType == DataTypes_ArrayTypes_array ||
                     odef->ContainerType == DataTypes_ContainerTypes_map_int32)
            {
                if (argc != 1)
                    throw InvalidArgumentException("ObjRef " + odef->Name + " is indexed by int32");
                mxArray* index = mxGetCell(cell_args, 0);
                if (!mxIsDouble(index) && !mxIsInt32(index))
                    throw InvalidArgumentException("ObjRef " + odef->Name + " is indexed by int32");
                if (mxGetNumberOfElements(index) != 1)
                    throw InvalidArgumentException("ObjRef " + odef->Name + " is indexed by scalar int32");
                int32_t iindex = 0;
                if (mxIsDouble(index))
                {
                    iindex = boost::numeric_cast<int32_t>(((double*)mxGetData(index))[0]);
                }
                else
                {
                    iindex = ((int32_t*)mxGetData(index))[0];
                }

                ostub = rr_cast<MexServiceStub>(FindObjRef(odef->Name, boost::lexical_cast<std::string>(iindex)));
            }
            else if (odef->ContainerType == DataTypes_ContainerTypes_map_string)
            {
                if (argc != 1)
                    throw InvalidArgumentException("ObjRef " + odef->Name + " is indexed by string");
                mxArray* index = mxGetCell(cell_args, 0);
                if (!mxIsChar(index))
                    throw InvalidArgumentException("ObjRef " + odef->Name + " is indexed by int32");

                std::string iindex = mxToString(index);

                ostub = rr_cast<MexServiceStub>(FindObjRef(odef->Name, iindex));
            }
            else
                throw InvalidArgumentException("Invalid objref indexer");

            if (!ostub)
                throw InvalidArgumentException("Could not get objref");

            boost::recursive_mutex::scoped_lock lock(stubs_lock);
            do
            {
                stubcount++;
                if (stubcount >= std::numeric_limits<int32_t>::max())
                    stubcount = 0;
            } while (stubs.find(stubcount) != stubs.end());

            int stubid = stubcount;
            ostub->stubid = stubid;
            mxArray* mxstub = MatlabObjectFromMexStub(ostub);
            stubs.insert(std::make_pair(stubid, ostub));

            return mxDuplicateArray(mxstub);
        }

        boost::shared_ptr<MemberDefinition> memdef3;

        BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
        {
            if ("connect_" + (e)->Name == membername)
            {
                memdef3 = e;
                break;
            }
        }

        boost::shared_ptr<PipeDefinition> ppdef = boost::dynamic_pointer_cast<PipeDefinition>(memdef3);
        if (ppdef)
        {
            std::string type2 = mxToString(mxGetField(S, 1, "type"));
            if (type2 != "()")
                throw InvalidArgumentException("Function call must use '()' notation");
            mxArray* cell_args = mxGetField(S, 1, "subs");
            size_t argc = mxGetNumberOfElements(cell_args);
            if (argc != 1)
                throw InvalidArgumentException("Pipe connect expects 1 argument");
            int index = 0;
            mxArray* mxindex = mxGetCell(cell_args, 0);
            if (mxGetNumberOfElements(mxindex) != 1)
                throw InvalidArgumentException("Pipe connect expects 1 argument");
            if (mxIsInt32(mxindex))
            {
                index = GetInt32Scalar(mxindex);
            }
            else if (mxIsDouble(mxindex))
            {
                index = (int)(((double*)mxGetData(mxindex)))[0];
            }
            else
                throw InvalidArgumentException("Pipe connect expects 1 int32 or double argument");

            boost::shared_ptr<MexPipeEndpoint> ep = pipes.at(ppdef->Name)->Connect(index);
            boost::recursive_mutex::scoped_lock lock(pipeendpoints_lock);
            do
            {
                pipeendpoints_count++;
                if (pipeendpoints_count >= std::numeric_limits<int32_t>::max())
                    pipeendpoints_count = 0;
            } while (pipeendpoints.find(pipeendpoints_count) != pipeendpoints.end());
            ep->pipeendpointid = pipeendpoints_count;
            pipeendpoints.insert(std::make_pair(ep->pipeendpointid, ep));
            return CreatePipeEndpointMxArray(ep->pipeendpointid);
        }

        throw MemberNotFoundException("Member " + membername + " is not a function or objref");
    }

    if (c1 == 3)
    {
        boost::shared_ptr<PipeDefinition> ppdef = boost::dynamic_pointer_cast<PipeDefinition>(memdef);
        boost::shared_ptr<WireDefinition> wdef = boost::dynamic_pointer_cast<WireDefinition>(memdef);
        if (ppdef)
        {
            std::string type2 = mxToString(mxGetField(S, 1, "type"));
            if (type2 != ".")
                throw InvalidArgumentException("Pipe function must use '.' notation twice");
            mxArray* funcname = mxGetField(S, 1, "subs");
            // if(mxGetNumberOfElements(funcname)!=1) throw InvalidArgumentException("Pipe function must use '.'
            // notation twice");

            std::string sfuncname = mxToString(funcname);
            if (sfuncname != "Connect")
                throw InvalidArgumentException("Unknown pipe function " + sfuncname);

            std::string type3 = mxToString(mxGetField(S, 2, "type"));
            if (type3 != "()")
                throw InvalidArgumentException("Pipe function must use '()' notation");
            mxArray* cell_args = mxGetField(S, 2, "subs");
            size_t argc = mxGetNumberOfElements(cell_args);
            if (argc != 1)
                throw InvalidArgumentException("Pipe connect expects one argument");
            int index = 0;
            mxArray* mxindex = mxGetCell(cell_args, 0);
            if (mxGetNumberOfElements(mxindex) != 1)
                throw InvalidArgumentException("Pipe connect expects 1 argument");
            if (mxIsInt32(mxindex))
            {
                index = GetInt32Scalar(mxindex);
            }
            else if (mxIsDouble(mxindex))
            {
                index = (int)(((double*)mxGetData(mxindex)))[0];
            }
            else
                throw InvalidArgumentException("Pipe connect expects 1 int32 or double argument");

            boost::shared_ptr<MexPipeEndpoint> ep = pipes.at(ppdef->Name)->Connect(index);
            boost::recursive_mutex::scoped_lock lock(pipeendpoints_lock);
            do
            {
                pipeendpoints_count++;
                if (pipeendpoints_count >= std::numeric_limits<int32_t>::max())
                    pipeendpoints_count = 0;
            } while (pipeendpoints.find(pipeendpoints_count) != pipeendpoints.end());
            ep->pipeendpointid = pipeendpoints_count;
            pipeendpoints.insert(std::make_pair(ep->pipeendpointid, ep));
            return CreatePipeEndpointMxArray(ep->pipeendpointid);
        }

        if (wdef)
        {
            std::string type2 = mxToString(mxGetField(S, 1, "type"));
            if (type2 != ".")
                throw InvalidArgumentException("Wire function must use '.' notation twice");
            mxArray* funcname = mxGetField(S, 1, "subs");
            // if(mxGetNumberOfElements(funcname)!=1) throw InvalidArgumentException("Pipe function must use '.'
            // notation twice");

            std::string sfuncname = mxToString(funcname);
            if (sfuncname == "Connect")
            {
                std::string type3 = mxToString(mxGetField(S, 2, "type"));
                if (type3 != "()")
                    throw InvalidArgumentException("Wire function must use '()' notation");
                mxArray* cell_args = mxGetField(S, 2, "subs");
                size_t argc = mxGetNumberOfElements(cell_args);
                if (argc != 0)
                    throw InvalidArgumentException("Wire connect expects zero arguments");

                boost::shared_ptr<MexWireConnection> ep = wires.at(wdef->Name)->Connect();
                boost::recursive_mutex::scoped_lock lock(wireconnections_lock);
                do
                {
                    wireconnections_count++;
                    if (wireconnections_count >= std::numeric_limits<int32_t>::max())
                        wireconnections_count = 0;
                } while (wireconnections.find(wireconnections_count) != wireconnections.end());
                ep->wireconnectionid = wireconnections_count;
                wireconnections.insert(std::make_pair(ep->wireconnectionid, ep));
                return CreateWireConnectionMxArray(ep->wireconnectionid);
            }

            if (sfuncname == "PeekInValue")
            {
                std::string type3 = mxToString(mxGetField(S, 2, "type"));
                if (type3 != "()")
                    throw InvalidArgumentException("Wire function must use '()' notation");
                mxArray* cell_args = mxGetField(S, 2, "subs");
                size_t argc = mxGetNumberOfElements(cell_args);
                if (argc != 0)
                    throw InvalidArgumentException("Wire PeekInValue expects zero arguments");

                return wires.at(wdef->Name)->PeekInValue();
            }

            if (sfuncname == "PeekOutValue")
            {
                std::string type3 = mxToString(mxGetField(S, 2, "type"));
                if (type3 != "()")
                    throw InvalidArgumentException("Wire function must use '()' notation");
                mxArray* cell_args = mxGetField(S, 2, "subs");
                size_t argc = mxGetNumberOfElements(cell_args);
                if (argc != 0)
                    throw InvalidArgumentException("Wire PeekOutValue expects zero arguments");

                return wires.at(wdef->Name)->PeekOutValue();
            }

            if (sfuncname == "PokeOutValue")
            {
                std::string type3 = mxToString(mxGetField(S, 2, "type"));
                if (type3 != "()")
                    throw InvalidArgumentException("Wire function must use '()' notation");
                mxArray* cell_args = mxGetField(S, 2, "subs");
                size_t argc = mxGetNumberOfElements(cell_args);
                if (argc != 1)
                    throw InvalidArgumentException("Wire PokeOutValue expects one argument");
                const mxArray* mxVal = ::mxGetCell(cell_args, 0);

                wires.at(wdef->Name)->PokeOutValue(mxVal);
                return mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
            }

            throw InvalidArgumentException("Unknown wire function " + sfuncname);
        }

        throw MemberNotFoundException("Member " + membername + " is not a pipe or wire");
    }

    throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");
}

void MexServiceStub::subsasgn(const mxArray* S, const mxArray* value)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);
    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteurMex error");

    std::string type = mxToString(mxGetField(S, 0, "type"));

    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    if (!mxIsChar(mxGetField(S, 0, "subs")))
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    boost::shared_ptr<MemberDefinition> memdef;

    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, RR_objecttype->Members)
    {
        if ((e)->Name == membername)
        {
            memdef = e;
            break;
        }
    }

    if (!memdef)
        throw MemberNotFoundException("RobotRaconteur member " + membername + " not found in object type " +
                                      RR_objecttype->Name);

    if (c1 == 1)
    {
        // Property Set
        boost::shared_ptr<PropertyDefinition> pdef = boost::dynamic_pointer_cast<PropertyDefinition>(memdef);
        boost::shared_ptr<CallbackDefinition> cdef = boost::dynamic_pointer_cast<CallbackDefinition>(memdef);
        if (!pdef && !cdef)
            throw MemberNotFoundException("Member " + membername + " is not a property or callback");
        if (pdef)
        {
            PropertySet(membername, value);
            return;
        }
        // Callback set
        if (cdef)
        {
            if (!mxIsFunctionHandle(value))
                throw InvalidArgumentException("Supplied value is not a function handle");
            boost::recursive_mutex::scoped_lock lock(callback_lock);

            callbacks.erase(cdef->Name);

            if (!mxIsEmpty(value))
            {
                mxArray* value2 = mxDuplicateArray(value);
                mexMakeArrayPersistent(value2);
                callbacks.insert(std::make_pair(cdef->Name, RR_SHARED_PTR<mxArray>(value2, rr_mxDestroyArray)));
            }
            return;
        }
        return;
    }

    throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");
}

RR_INTRUSIVE_PTR<MessageEntry> MexServiceStub::CallbackCall(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{

    boost::shared_ptr<MexCallbackCall> req = boost::make_shared<MexCallbackCall>(m);

    {
        boost::recursive_mutex::scoped_lock lock(callback_lock);
        std::map<std::string, RR_SHARED_PTR<mxArray> >::iterator e1 = callbacks.find(m->MemberName.str().to_string());
        if (e1 == callbacks.end())
        {
            throw InvalidOperationException("MATLAB callback function not set");
        }
        if (e1->second == NULL)
        {
            throw InvalidOperationException("MATLAB callback function not set");
        }

        callback_requests.push_back(req);
    }
    req->ev.WaitOne(10000);

    if (!req->response)
        throw OperationFailedException("Callback was not called");

    return req->response;
}

void MexServiceStub::MexProcessRequests()
{
    {

        std::deque<RR_INTRUSIVE_PTR<MessageEntry> > events;
        RR_INTRUSIVE_PTR<MessageEntry> e;
        {
            boost::recursive_mutex::scoped_lock lock2(events_lock);
            while (!this->events.empty())
            {
                e = this->events.front();
                this->events.pop_front();
                events.push_back(e);
            }
        }

        while (!events.empty())
        {
            e = events.front();
            events.pop_front();
            try
            {
                boost::shared_ptr<EventDefinition> eventdef;
                BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& ee, RR_objecttype->Members)
                {
                    boost::shared_ptr<EventDefinition> eventdef1 = boost::dynamic_pointer_cast<EventDefinition>(ee);
                    if (eventdef1)
                    {
                        if (eventdef1->Name == (e)->MemberName.str())
                        {
                            eventdef = eventdef1;
                            break;
                        }
                    }
                }
                if (eventdef)
                {
                    std::vector<mxArray*> args;
                    BOOST_FOREACH (boost::shared_ptr<TypeDefinition>& ee, eventdef->Parameters)
                    {
                        RR_INTRUSIVE_PTR<MessageElement> arg = (e)->FindElement((ee)->Name);
                        args.push_back(UnpackMessageElementToMxArray(arg, ee, shared_from_this()));
                    }

                    std::map<int32_t, boost::shared_ptr<MexEventConnection> > eventconnections;
                    {
                        boost::recursive_mutex::scoped_lock lock(eventconnections_lock);
                        eventconnections = this->eventconnections;
                    }
                    for (std::map<int32_t, boost::shared_ptr<MexEventConnection> >::iterator ee =
                             eventconnections.begin();
                         ee != eventconnections.end(); ++ee)
                    {
                        std::vector<mxArray*> args2 = args;
                        args2.insert(args2.begin(), ee->second->functionhandle.get());
                        if (ee->second->membername == eventdef->Name)
                        {
                            mxArray* ret = NULL;
                            if ((ret = ::mexCallMATLABWithTrap(0, NULL, (int)args2.size(), &args2[0], "feval")))
                            {
                                ::mexWarnMsgIdAndTxt(mxToString(mxGetProperty(ret, 0, "identifier")).c_str(),
                                                     mxToString(mxGetProperty(ret, 0, "message")).c_str());
                            }
                        }
                    }
                }
            }
            catch (std::exception& e)
            {
                mexWarnMsgTxt((std::string("Warning: error dispatching event: ") + std::string(e.what())).c_str());
            }
        }
    }
    {
        std::deque<boost::shared_ptr<MexCallbackCall> > callback_requests;
        boost::shared_ptr<MexCallbackCall> e;
        {
            boost::recursive_mutex::scoped_lock lock3(callback_lock);

            while (!this->callback_requests.empty())
            {
                e = this->callback_requests.front();
                this->callback_requests.pop_front();
                callback_requests.push_back(e);
            }
        }
        while (!callback_requests.empty())
        {
            e = callback_requests.front();
            callback_requests.pop_front();
            RR_INTRUSIVE_PTR<MessageEntry> ret =
                CreateMessageEntry(MessageEntryType_CallbackCallRet, e->request->MemberName);
            ret->RequestID = e->request->RequestID;
            ret->ServicePath = e->request->ServicePath;
            try
            {

                boost::shared_ptr<CallbackDefinition> calldef;
                BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& ee, RR_objecttype->Members)
                {
                    boost::shared_ptr<CallbackDefinition> calldef1 =
                        boost::dynamic_pointer_cast<CallbackDefinition>(ee);
                    if (calldef1)
                    {
                        if (calldef1->Name == (e)->request->MemberName.str())
                        {
                            calldef = calldef1;
                            break;
                        }
                    }
                }
                if (!calldef)
                    throw MemberNotFoundException("Invalid member name");

                std::vector<mxArray*> args;
                BOOST_FOREACH (boost::shared_ptr<TypeDefinition>& ee, calldef->Parameters)
                {
                    RR_INTRUSIVE_PTR<MessageElement> arg = (e)->request->FindElement((ee)->Name);
                    args.push_back(UnpackMessageElementToMxArray(arg, ee, shared_from_this()));
                }

                std::map<std::string, RR_SHARED_PTR<mxArray> >::iterator e1 =
                    callbacks.find(e->request->MemberName.str().to_string());
                if (e1 == callbacks.end())
                    throw InvalidOperationException("MATLAB callback function not set");

                mxArray* callbackfunc = e1->second.get();
                if (callbackfunc == NULL)
                    throw InvalidOperationException("MATLAB callback function not set");

                std::vector<mxArray*> args2 = args;
                args2.insert(args2.begin(), callbackfunc);

                if (calldef->ReturnType->Type == DataTypes_void_t)
                {
                    mxArray* ret2 = 0;
                    if ((ret2 = ::mexCallMATLABWithTrap(0, NULL, (int)args2.size(), &args2[0], "feval")))
                    {
                        // mexWarnMsgTxt(std::string("Warning: error dispatching event: error occured in MATLAB
                        // callback").c_str()); throw OperationFailedException("Error occured in MATLAB callback");
                        std::string errmsg = mxToString(mxGetProperty(ret2, 0, "message"));
                        ::mexWarnMsgTxt(errmsg.c_str());
                        throw OperationFailedException(errmsg);
                    }

                    ret->AddElement("return", ScalarToRRArray<int32_t>(0));
                }
                else
                {
                    mxArray* lhs[1];
                    mxArray* ret2 = 0;
                    if ((ret2 = ::mexCallMATLABWithTrap(1, lhs, (int)args2.size(), &args2[0], "feval")))
                    {
                        // mexWarnMsgTxt(std::string("Warning: error dispatching event: error occured in MATLAB
                        // callback").c_str());
                        std::string errmsg = mxToString(mxGetProperty(ret2, 0, "message"));
                        ::mexWarnMsgTxt(errmsg.c_str());
                        throw OperationFailedException(errmsg);
                    }

                    RR_INTRUSIVE_PTR<MessageElement> mret =
                        PackMxArrayToMessageElement(lhs[0], calldef->ReturnType, shared_from_this());
                    mret->ElementName = "return";
                    ret->AddElement(mret);
                }
            }
            catch (std::exception& err)
            {
                mexWarnMsgTxt((std::string("Warning: error in callback: ") + std::string(err.what())).c_str());
                RobotRaconteurExceptionUtil::ExceptionToMessageEntry(err, ret);
            }

            e->response = ret;
            e->ev.Set();
        }
    }

    // Dispatch async results
    {
        std::queue<RR_SHARED_PTR<MexAsyncResult> > async_results;
        RR_SHARED_PTR<MexAsyncResult> e;
        {
            boost::mutex::scoped_lock lock5(async_results_lock);
            while (!this->async_results.empty())
            {
                e = this->async_results.front();
                this->async_results.pop();
                async_results.push(e);
            }
        }
        while (!async_results.empty())
        {
            try
            {
                e = async_results.front();
                async_results.pop();

                std::vector<mxArray*> plhs;
                plhs.push_back(mxDuplicateArray(e->handler.get()));
                plhs.push_back(mxDuplicateArray(e->param.get()));

                if (e->return_type)
                {
                    if (e->return_value)
                    {
                        try
                        {
                            plhs.push_back(
                                UnpackMessageElementToMxArray(e->return_value, e->return_type, shared_from_this()));
                        }
                        catch (std::exception& err)
                        {

                            if (plhs.size() < 3)
                            {
                                plhs.push_back(mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL));
                            }
                            plhs.push_back(mxCreateString(
                                (std::string(typeid(err).name()) + " " + std::string(err.what())).c_str()));
                            ::mexCallMATLABWithTrap(0, NULL, (int)plhs.size(), &plhs[0], "feval");
                            continue;
                        }
                    }
                    else
                    {
                        plhs.push_back(mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL));
                    }
                }

                if (e->error)
                {
                    plhs.push_back(mxCreateString(e->error->ToString().c_str()));
                }
                else
                {
                    plhs.push_back(mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL));
                }
                mxArray* ret = NULL;
                if ((ret = ::mexCallMATLABWithTrap(0, NULL, (int)plhs.size(), &plhs[0], "feval")))
                {
                    ::mexWarnMsgIdAndTxt(mxToString(mxGetProperty(ret, 0, "identifier")).c_str(),
                                         mxToString(mxGetProperty(ret, 0, "message")).c_str());
                }
            }
            catch (std::exception& err)
            {
                mexWarnMsgTxt((std::string("Warning: error in async handler: ") + std::string(err.what())).c_str());
            }
        }
    }
}

mxArray* MexServiceStub::addlistener(const mxArray* name, const mxArray* functionhandle)
{

    std::string sname = mxToString(name);

    boost::shared_ptr<EventDefinition> eventdef;
    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& ee, RR_objecttype->Members)
    {
        boost::shared_ptr<EventDefinition> eventdef1 = boost::dynamic_pointer_cast<EventDefinition>(ee);
        if (eventdef1)
        {
            if (eventdef1->Name == sname)
            {
                eventdef = eventdef1;
                break;
            }
        }
    }
    if (!eventdef)
    {
        throw InvalidArgumentException("Unknown event name");
    }

    boost::recursive_mutex::scoped_lock lock(eventconnections_lock);
    eventconnections_count++;

    mxArray* functionhandle2 = mxDuplicateArray(functionhandle);
    mexMakeArrayPersistent(functionhandle2);
    eventconnections.insert(
        std::make_pair(eventconnections_count,
                       boost::make_shared<MexEventConnection>(
                           sname, eventconnections_count, RR_SHARED_PTR<mxArray>(functionhandle2, rr_mxDestroyArray))));

    mxArray* lhs[1];
    mxArray* rhs[3];
    rhs[0] = CreateInt32Scalar(RR_MEX_STUB);
    rhs[1] = CreateInt32Scalar(stubid);
    rhs[2] = CreateInt32Scalar(eventconnections_count);

    if (mexCallMATLAB(1, lhs, 3, rhs, "RobotRaconteurEventListener"))
    {
        throw InternalErrorException("Robot Raconteur Internal error");
    }

    return lhs[0];
}

void MexServiceStub::deletelistener(const mxArray* eventid)
{
    int32_t ieventid = GetInt32Scalar(eventid);
    boost::recursive_mutex::scoped_lock lock(eventconnections_lock);
    if (eventconnections.find(ieventid) != eventconnections.end())
    {
        // mxDestroyArray(eventconnections.at(ieventid)->functionhandle);
        eventconnections.erase(ieventid);
    }
    else
    {
        throw InvalidOperationException("Listener not connected");
    }
}

void convert_indices(mxArray* ind, int64_t& start, int64_t& count)
{
    mxArray* lhs[1];
    mxArray* rhs[1];
    rhs[0] = ind;
    if (::mexCallMATLAB(1, lhs, 1, rhs, "int64"))
        throw InvalidArgumentException("Invalid memory read/write request");
    mxArray* ind2 = lhs[0];

    size_t ninds = mxGetNumberOfElements(ind2);

    int64_t* pind = (int64_t*)mxGetData(ind2);
    if (ninds == 1)
    {
        start = pind[0] - 1;
        count = 1;
        if (start < 0)
            throw InvalidArgumentException("Invalid memory read/write request");

        return;
    }

    for (size_t i = 0; i < (ninds - 1); i++)
    {
        if ((pind[i + 1] - pind[i]) != 1)
            throw InvalidArgumentException("Invalid memory read/write request");
    }

    int64_t i1 = pind[0];
    int64_t i2 = pind[ninds - 1];

    start = i1 - 1;
    count = i2 - i1 + 1;

    if (start < 0)
        throw InvalidArgumentException("Invalid memory read/write request");
    if (count < 0)
        throw InvalidArgumentException("Invalid memory read/write request");
}

mxArray* MexServiceStub::MemoryOp(const mxArray* member, const mxArray* command, const mxArray* S, const mxArray* data)
{
    std::string smember = mxToString(member);
    std::string scommand = mxToString(command);

    if (arraymemories.count(smember) != 0)
    {
        boost::shared_ptr<ArrayMemoryBase> mem = arraymemories.at(smember);

        if (scommand == "numel")
        {
            uint64_t len = mem->Length();
            return CreateDoubleScalar((double)len);
        }

        if (scommand == "size")
        {
            uint64_t len = mem->Length();
            mwSize s[] = {1, 2};
            mxArray* out = mxCreateNumericArray(2, s, mxDOUBLE_CLASS, mxREAL);
            double* outdat = (double*)mxGetData(out);
            outdat[0] = (double)len;
            outdat[1] = 1;
            return out;
        }

        int64_t start = -1;
        int64_t count = -1;
        if (scommand == "read" || scommand == "write")
        {
            if (S == NULL)
                throw InvalidArgumentException("Invalid memory read/write request");

            if (mxGetNumberOfElements(S) != 1)
                throw InvalidArgumentException("Invalid memory read/write request");
            std::string type = mxToString(mxGetField(S, 0, "type"));
            if (type != "()")
                throw InvalidArgumentException("Invalid memory read/write request");
            mxArray* subs = mxGetField(S, 0, "subs");
            int narg = (int)mxGetNumberOfElements(subs);

            if (narg == 0)
            {
                count = -1;
                start = -1;
            }
            else if (narg == 1)
            {
                mxArray* arg = mxGetCell(subs, 0);
                if (mxIsChar(arg))
                {
                    std::string strarg = mxToString(arg);
                    if (strarg != ":")
                        throw InvalidArgumentException("Invalid memory read/write request");
                    count = -1;
                    start = -1;
                }
                else
                {
                    if (!(mxIsNumeric(arg) || mxIsLogical(arg)))
                        throw InvalidArgumentException("Invalid memory read/write request");
                    convert_indices(arg, start, count);
                }
            }
            else
            {
                RR_SHARED_PTR<MexNamedArrayMemoryClient> named_mem =
                    RR_DYNAMIC_POINTER_CAST<MexNamedArrayMemoryClient>(mem);
                if (mxGetNumberOfElements(subs) == 2 && named_mem)
                {

                    mxArray* arg1 = mxGetCell(subs, 0);
                    if (!(mxIsChar(arg1) && mxToString(arg1) == ":"))
                        throw InvalidArgumentException("Invalid memory read/write request for named array");

                    mxArray* arg = mxGetCell(subs, 1);
                    if (mxIsChar(arg))
                    {
                        std::string strarg = mxToString(arg);
                        if (strarg != ":")
                            throw InvalidArgumentException("Invalid memory read/write request");
                        count = -1;
                        start = -1;
                    }
                    else
                    {
                        if (!(mxIsNumeric(arg) || mxIsLogical(arg)))
                            throw InvalidArgumentException("Invalid memory read/write request");
                        convert_indices(arg, start, count);
                    }

                    if (scommand == "read")
                    {
                        if (start < 0)
                            start = 0;
                        if (count < 0)
                            count = boost::numeric_cast<int64_t>(mem->Length());
                        return named_mem->Read(start, 0, count);
                    }

                    if (scommand == "write")
                    {
                        if (data == NULL)
                            throw InvalidArgumentException("Invalid memory read/write request");
                        if (start < 0)
                            start = 0;
                        if (count < 0)
                            count = boost::numeric_cast<int64_t>(::mxGetNumberOfElements(data));
                        if (mxGetN(data) < count)
                            throw OutOfRangeException("Invalid indices");
                        named_mem->Write(start, data, 0, count);
                        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
                    }
                    throw InvalidArgumentException("Unknown memory command");
                }

                throw OutOfRangeException("Index exceeds matrix dimensions");
            }

            if (scommand == "read")
            {
                if (start < 0)
                    start = 0;
                if (count < 0)
                    count = boost::numeric_cast<int64_t>(mem->Length());

                RR_SHARED_PTR<MexPodArrayMemoryClient> pod_mem = RR_DYNAMIC_POINTER_CAST<MexPodArrayMemoryClient>(mem);
                if (pod_mem)
                {
                    return pod_mem->Read(start, 0, count);
                }

                return MexArrayMemoryClientUtil::Read(mem, start, count);
            }

            if (scommand == "write")
            {
                if (data == NULL)
                    throw InvalidArgumentException("Invalid memory read/write request");
                if (start < 0)
                    start = 0;
                if (count < 0)
                    count = boost::numeric_cast<int64_t>(::mxGetNumberOfElements(data));
                RR_SHARED_PTR<MexPodArrayMemoryClient> pod_mem = RR_DYNAMIC_POINTER_CAST<MexPodArrayMemoryClient>(mem);
                if (pod_mem)
                {
                    if (mxGetNumberOfElements(data) < count)
                        throw OutOfRangeException("Invalid indices");
                    pod_mem->Write(start, data, 0, count);
                    return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
                }
                if (!(mxIsNumeric(data) || mxIsLogical(data)))
                    throw InvalidArgumentException("Invalid type for memory write");
                if (count != mxGetNumberOfElements(data) && mxGetNumberOfElements(data) == 1)
                {
                    mxArray* data2 = mxCreateNumericMatrix(boost::numeric_cast<size_t>(count), 1, mxGetClassID(data),
                                                           mxIsComplex(data) ? mxCOMPLEX : mxREAL);
                    size_t s = mxGetElementSize(data);

                    for (size_t i = 0; i < boost::numeric_cast<size_t>(count); i++)
                    {
                        uint8_t* orig = (uint8_t*)mxGetData(data);
                        uint8_t* dest = (uint8_t*)mxGetData(data2);
                        memcpy((dest) + (i * s), orig, s);
                    }
                    MexArrayMemoryClientUtil::Write(mem, start, data2, 0, count);
                    mxDestroyArray(data2);
                }
                else
                {
                    MexArrayMemoryClientUtil::Write(mem, start, data, 0, count);
                }
                return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
            }
        }

        throw InvalidArgumentException("Unknown memory command");
    }

    std::map<std::string, RR_SHARED_PTR<MultiDimArrayMemoryBase> >::iterator e1 = multidimarraymemories.find(smember);
    if (e1 != multidimarraymemories.end())
    {
        boost::shared_ptr<MultiDimArrayMemoryBase> mem = e1->second;

        if (scommand == "numel")
        {
            std::vector<uint64_t> size = mem->Dimensions();
            uint64_t numel = 1;
            for (size_t i = 0; i < size.size(); i++)
            {
                numel *= size[i];
            }

            return CreateDoubleScalar((double)numel);
        }

        if (scommand == "ndims")
        {
            uint64_t ndims = mem->DimCount();
            return CreateDoubleScalar((double)ndims);
        }

        if (scommand == "size")
        {
            std::vector<uint64_t> size = mem->Dimensions();
            if (size.size() == 1)
            {
                mwSize s[] = {1, 2};
                mxArray* out = mxCreateNumericArray(2, s, mxDOUBLE_CLASS, mxREAL);
                double* outdat = (double*)mxGetData(out);
                outdat[0] = (double)size[0];
                outdat[1] = 1;
                return out;
            }
            else
            {
                size_t ndims = size.size();

                mxArray* out = mxCreateNumericMatrix(1, (mwSize)ndims, mxDOUBLE_CLASS, mxREAL);
                double* outdat = (double*)mxGetData(out);
                for (size_t i = 0; i < ndims; i++)
                {
                    outdat[i] = (double)size[i];
                }
                return out;
            }
        }

        std::vector<uint64_t> start;
        std::vector<uint64_t> count;
        if (scommand == "read" || scommand == "write")
        {
            if (S == NULL)
                throw InvalidArgumentException("Invalid memory read/write request");

            if (mxGetNumberOfElements(S) != 1)
                throw InvalidArgumentException("Invalid memory read/write request");
            std::string type = mxToString(mxGetField(S, 0, "type"));
            if (type != "()")
                throw InvalidArgumentException("Invalid memory read/write request");
            mxArray* subs = mxGetField(S, 0, "subs");
            int narg = (int)mxGetNumberOfElements(subs);

            RR_SHARED_PTR<MexNamedMultiDimArrayMemoryClient> named_mem =
                RR_DYNAMIC_POINTER_CAST<MexNamedMultiDimArrayMemoryClient>(mem);
            if (named_mem)
            {
                if (narg < 1)
                    throw InvalidArgumentException("Invalid memory read/write request");
                mxArray* arg = mxGetCell(subs, 0);
                if (!mxIsChar(arg))
                    throw InvalidArgumentException("Invalid memory read/write request");

                std::string strarg = mxToString(arg);
                if (strarg != ":")
                    throw InvalidArgumentException("Invalid memory namedarray read/write request");

                if (narg == 1)
                {
                    count.clear();
                    start.clear();
                }
                else
                {
                    mxArray* arg2 = mxGetCell(subs, 1);
                    if (mxIsChar(arg2))
                    {
                        std::string strarg2 = mxToString(arg2);
                        if (strarg2 != ":")
                            throw InvalidArgumentException("Invalid memory namedarray read/write request");
                        if (narg != 2)
                            throw InvalidArgumentException("Invalid memory namedarray read/write request");
                        count.clear();
                        start.clear();
                    }
                    else
                    {
                        for (size_t i = 1; i < mxGetNumberOfElements(subs); i++)
                        {
                            arg = mxGetCell(subs, i);
                            if (!(mxIsNumeric(arg) || mxIsLogical(arg)))
                                throw InvalidArgumentException("Invalid memory read/write request");
                            int64_t start1 = 0;
                            int64_t count1 = 0;
                            convert_indices(arg, start1, count1);
                            start.push_back(start1);
                            count.push_back(count1);
                        }
                    }
                }
            }
            else
            {

                if (narg == 0)
                {
                    count.clear();
                    start.clear();
                }
                else
                {
                    mxArray* arg = mxGetCell(subs, 0);
                    if (mxIsChar(arg))
                    {
                        std::string strarg = mxToString(arg);
                        if (strarg != ":")
                            throw InvalidArgumentException("Invalid memory read/write request");
                        if (narg != 1)
                            throw InvalidArgumentException("Invalid memory read/write request");
                        count.clear();
                        start.clear();
                    }
                    else
                    {
                        for (size_t i = 0; i < mxGetNumberOfElements(subs); i++)
                        {
                            arg = mxGetCell(subs, i);
                            if (!(mxIsNumeric(arg) || mxIsLogical(arg)))
                                throw InvalidArgumentException("Invalid memory read/write request");
                            int64_t start1 = 0;
                            int64_t count1 = 0;
                            convert_indices(arg, start1, count1);
                            start.push_back(start1);
                            count.push_back(count1);
                        }
                    }
                }
            }

            if (scommand == "read")
            {

                if (count.empty())
                    count = mem->Dimensions();
                if (start.empty())
                    for (size_t i = 0; i < count.size(); i++)
                        start.push_back(0);

                RR_SHARED_PTR<MexPodMultiDimArrayMemoryClient> pod_mem =
                    RR_DYNAMIC_POINTER_CAST<MexPodMultiDimArrayMemoryClient>(mem);
                if (pod_mem)
                {
                    return pod_mem->Read(start, std::vector<uint64_t>(count.size()), count);
                }

                if (named_mem)
                {
                    return named_mem->Read(start, std::vector<uint64_t>(count.size()), count);
                }

                return MexMultiDimArrayMemoryClientUtil::Read(mem, start, count);
            }

            if (scommand == "write")
            {
                if (data == NULL)
                    throw InvalidArgumentException("Invalid memory read/write request");

                if (count.empty())
                {
                    if (mxGetNumberOfElements(data) == 1)
                    {
                        count = mem->Dimensions();
                    }
                    else
                    {
                        for (size_t i = 0; i < ::mxGetNumberOfDimensions(data); i++)
                        {
                            if (!(i == 0 && named_mem))
                            {
                                count.push_back(((mwSize*)::mxGetDimensions(data))[0]);
                            }
                        }
                    }
                }

                uint64_t numcount = 1;
                for (size_t i = 0; i < count.size(); i++)
                    numcount *= count[i];

                if (start.empty())
                    for (size_t i = 0; i < count.size(); i++)
                        start.push_back(0);
                std::vector<uint64_t> bufferpos(count.size());
                std::fill(bufferpos.begin(), bufferpos.end(), 0);

                RR_SHARED_PTR<MexPodMultiDimArrayMemoryClient> pod_mem =
                    RR_DYNAMIC_POINTER_CAST<MexPodMultiDimArrayMemoryClient>(mem);
                if (pod_mem)
                {
                    pod_mem->Write(start, data, std::vector<uint64_t>(count.size()), count);
                    return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
                }

                if (named_mem)
                {
                    named_mem->Write(start, data, std::vector<uint64_t>(count.size()), count);
                    return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
                }

                if (!(mxIsNumeric(data) || mxIsLogical(data)))
                    throw InvalidArgumentException("Invalid type for memory write");

                if (numcount != mxGetNumberOfElements(data) && mxGetNumberOfElements(data) == 1)
                {
                    std::vector<mwSize> count2;
                    for (size_t i = 0; i < count.size(); i++)
                        count2.push_back((mwSize)count[i]);

                    mxClassID cid = mxGetClassID(data);
                    mxComplexity complex = mxIsComplex(data) ? mxCOMPLEX : mxREAL;

                    mxArray* data2 = mxCreateNumericArray(count2.size(), &count2[0], cid, complex);
                    size_t s = mxGetElementSize(data);

                    for (size_t i = 0; i < numcount; i++)
                    {
                        uint8_t* orig = (uint8_t*)mxGetData(data);
                        uint8_t* dest = (uint8_t*)mxGetData(data2);

                        memcpy((dest) + (i * s), orig, s);
                    }
                    MexMultiDimArrayMemoryClientUtil::Write(mem, start, data2, bufferpos, count);
                    mxDestroyArray(data2);
                }
                else
                {
                    MexMultiDimArrayMemoryClientUtil::Write(mem, start, data, bufferpos, count);
                }
                return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
            }
        }

        throw InvalidArgumentException("Unknown memory command");
    }

    throw MemberNotFoundException("Memory " + smember + " not found");
}

void MexServiceStub::LockOp(const mxArray* command)
{
    std::string scommand = mxToString(command);

    if (scommand == "RequestUserLock")
    {
        RobotRaconteurNode::s()->RequestObjectLock(shared_from_this(), RobotRaconteurObjectLockFlags_USER_LOCK);
        return;
    }

    if (scommand == "ReleaseUserLock")
    {
        RobotRaconteurNode::s()->ReleaseObjectLock(shared_from_this());
        return;
    }

    if (scommand == "RequestClientLock")
    {
        RobotRaconteurNode::s()->RequestObjectLock(shared_from_this(), RobotRaconteurObjectLockFlags_CLIENT_LOCK);
        return;
    }

    if (scommand == "ReleaseClientLock")
    {
        RobotRaconteurNode::s()->ReleaseObjectLock(shared_from_this());
        return;
    }

    if (scommand == "MonitorEnter")
    {
        RobotRaconteurNode::s()->MonitorEnter(shared_from_this());
        return;
    }

    if (scommand == "MonitorExit")
    {
        RobotRaconteurNode::s()->MonitorExit(shared_from_this());
        return;
    }

    throw InvalidArgumentException("Unknown LockOp command");
}

// Internal functions

mxArray* ConnectClient(const mxArray* url, const mxArray* username, const mxArray* credentials)
{

    std::string susername;
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > mcredentials;

    if (username != NULL && credentials != NULL)
    {
        susername = mxToString(username);

        mcredentials = mxArrayToRRMap(credentials);
    }

    std::vector<std::string> urls;

    if (mxIsCell(url))
    {
        size_t urlc = mxGetNumberOfElements(url);
        for (size_t i = 0; i < urlc; i++)
        {
            mxArray* urln_a = mxGetCell(url, i);
            urls.push_back(mxToString(urln_a));
        }
    }
    else
    {
        std::string surl1 = mxToString(url);

        boost::split(urls, surl1, boost::is_from_range(',', ','));
    }
    boost::shared_ptr<MexServiceStub> s;

    s = rr_cast<MexServiceStub>(RobotRaconteurNode::s()->ConnectService(urls, susername, mcredentials));

    if (s->stubptr != NULL)
    {
        return s->stubptr.get();
    }
    else
    {
        boost::recursive_mutex::scoped_lock lock(stubs_lock);
        stubcount++;
        int stubid = stubcount;
        s->stubid = stubid;
        mxArray* mxstub = MatlabObjectFromMexStub(s);
        stubs.insert(std::make_pair(stubid, s));
        return mxDuplicateArray(mxstub);
    }
}

mxArray* MatlabObjectFromMexStub(const boost::shared_ptr<MexServiceStub>& stub)
{
    mxArray* matlabret[1];
    mxArray* rhs[2];

    rhs[0] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    rhs[1] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);

    ((int32_t*)mxGetData(rhs[0]))[0] = RR_MEX_STUB;
    ((int32_t*)mxGetData(rhs[1]))[0] = stub->stubid;

    int merror = mexCallMATLAB(1, matlabret, 2, rhs, "RobotRaconteurObject");
    if (merror)
    {

        throw InternalErrorException("Internal error");
    }

    mxArray* mxstub = mxDuplicateArray(matlabret[0]);

    mexMakeArrayPersistent(mxstub);

    stub->stubptr = RR_SHARED_PTR<mxArray>(mxstub, rr_mxDestroyArray);
    return mxstub;
}

static void send_handler(
    uint32_t packetnumber, const RR_SHARED_PTR<RobotRaconteurException>& err,
    boost::function<void(const RR_SHARED_PTR<uint32_t>&, const RR_SHARED_PTR<RobotRaconteurException>&)> handler)
{
    handler(RR_MAKE_SHARED<uint32_t>(packetnumber), err);
}

uint32_t MexPipeEndpoint::SendPacket(const RR_INTRUSIVE_PTR<MessageElement>& packet)
{
    RR_SHARED_PTR<RobotRaconteur::detail::sync_async_handler<uint32_t> > t =
        RR_MAKE_SHARED<RobotRaconteur::detail::sync_async_handler<uint32_t> >();
    AsyncSendPacketBase(
        packet,
        boost::bind(&send_handler, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                    boost::protect(boost::bind(&RobotRaconteur::detail::sync_async_handler<uint32_t>::operator(), t,
                                               RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)))));

    return (*t->end());
}

RR_INTRUSIVE_PTR<MessageElement> MexPipeEndpoint::ReceivePacket()
{
    return rr_cast<MessageElement>(ReceivePacketBase());
}

RR_INTRUSIVE_PTR<MessageElement> MexPipeEndpoint::PeekNextPacket() { return rr_cast<MessageElement>(PeekPacketBase()); }

RR_INTRUSIVE_PTR<MessageElement> MexPipeEndpoint::ReceivePacketWait(int32_t timeout)
{
    return rr_cast<MessageElement>(ReceivePacketBaseWait(timeout));
}

RR_INTRUSIVE_PTR<MessageElement> MexPipeEndpoint::PeekNextPacketWait(int32_t timeout)
{
    return rr_cast<MessageElement>(PeekPacketBaseWait(timeout));
}

bool MexPipeEndpoint::TryReceivePacketWait(RR_INTRUSIVE_PTR<MessageElement>& packet, int32_t timeout, bool peek)
{
    RR_INTRUSIVE_PTR<RRValue> o;
    if (!TryReceivePacketBaseWait(o, timeout, peek))
        return false;

    packet = rr_cast<MessageElement>(o);
    return true;
}

MexPipeEndpoint::MexPipeEndpoint(const RR_SHARED_PTR<PipeBase>& parent, int32_t index, uint32_t endpoint,
                                 const RR_SHARED_PTR<TypeDefinition>& Type, bool unreliable,
                                 MemberDefinition_Direction direction)
    : PipeEndpointBase(parent, index, endpoint, unreliable, direction)
{
    this->Type = Type;
    this->pipeendpointid = 0;
}

void MexPipeEndpoint::fire_PipeEndpointClosedCallback()
{
    if (pipeendpointid != 0)
    {
        boost::recursive_mutex::scoped_lock lock(pipeendpoints_lock);
        try
        {
            pipeendpoints.erase(pipeendpointid);
        }
        catch (...)
        {}
    }
}

void MexPipeEndpoint::Close()
{
    PipeEndpointBase::Close();

    if (pipeendpointid != 0)
    {
        boost::recursive_mutex::scoped_lock lock(pipeendpoints_lock);
        try
        {
            pipeendpoints.erase(pipeendpointid);
        }
        catch (...)
        {}
    }
}

RR_SHARED_PTR<ServiceStub> MexPipeEndpoint::GetStub()
{
    RR_SHARED_PTR<PipeBase> p1 = GetParent();
    RR_SHARED_PTR<PipeClientBase> p2 = RR_DYNAMIC_POINTER_CAST<PipeClientBase>(p1);
    if (!p2)
        return RR_SHARED_PTR<ServiceStub>();
    return p2->GetStub();
}

void MexPipeEndpoint::fire_PacketReceivedEvent() {}

void MexPipeEndpoint::fire_PacketAckReceivedEvent(uint32_t packetnum) {}

mxArray* MexPipeEndpoint::subsref(const mxArray* S)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);

    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string type = mxToString(mxGetField(S, 0, "type"));
    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    if (c1 == 1)
    {
        if (membername == "Available")
        {

            int32_t av = (int)Available();
            return CreateInt32Scalar(av);
        }
        else if (membername == "IsUnreliable")
        {

            mxArray* ret = ::mxCreateNumericMatrix(1, 1, mxLOGICAL_CLASS, mxREAL);
            ((uint8_t*)mxGetData(ret))[0] = IsUnreliable() ? 1 : 0;
            return ret;
        }
        else if (membername == "Direction")
        {
            return CreateInt32Scalar((int32_t)this->Direction());
        }
        else
        {
            throw InvalidArgumentException("Unknown property for PipeEndpoint");
        }
    }

    if (c1 != 2)
        throw InvalidArgumentException("PipeEndpoint expects a function request");

    std::string type2 = mxToString(mxGetField(S, 1, "type"));
    if (type2 != "()")
        throw InvalidArgumentException("PipeEndpoint expects a function request");

    if (!mxIsChar(mxGetField(S, 0, "subs")))
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    mxArray* cell_args = mxGetField(S, 1, "subs");

    if (membername == "ReceivePacket")
    {
        if (mxGetNumberOfElements(cell_args) != 0)
            throw InvalidArgumentException("ReceivePacket expects zero arguments");
        return UnpackMessageElementToMxArray(ReceivePacket(), Type, rr_cast<PipeClientBase>(GetParent())->GetStub());
    }

    if (membername == "PeekNextPacket")
    {
        if (mxGetNumberOfElements(cell_args) != 0)
            throw InvalidArgumentException("PeekNextPacket expects zero arguments");
        return UnpackMessageElementToMxArray(PeekNextPacket(), Type, rr_cast<PipeClientBase>(GetParent())->GetStub());
    }

    if (membername == "ReceivePacketWait")
    {
        int32_t timeout = 0;
        if (mxGetNumberOfElements(cell_args) == 0)
        {
            timeout = RR_TIMEOUT_INFINITE;
        }
        else if (mxGetNumberOfElements(cell_args) == 1)
        {
            timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 0));
        }
        else
        {
            throw InvalidArgumentException("ReceivePacketWait expects zero or one arguments");
        }

        return UnpackMessageElementToMxArray(ReceivePacketWait(timeout), Type,
                                             rr_cast<PipeClientBase>(GetParent())->GetStub());
    }

    if (membername == "PeekNextPacketWait")
    {
        int32_t timeout = 0;
        if (mxGetNumberOfElements(cell_args) == 0)
        {
            timeout = RR_TIMEOUT_INFINITE;
        }
        else if (mxGetNumberOfElements(cell_args) == 1)
        {
            timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 0));
        }
        else
        {
            throw InvalidArgumentException("PeekNextPacketWait expects zero or one arguments");
        }

        return UnpackMessageElementToMxArray(PeekNextPacketWait(timeout), Type,
                                             rr_cast<PipeClientBase>(GetParent())->GetStub());
    }

    if (membername == "TryReceivePacketWait")
    {
        int32_t timeout = RR_TIMEOUT_INFINITE;
        bool peek = false;

        if (mxGetNumberOfElements(cell_args) == 0)
        {}
        else if (mxGetNumberOfElements(cell_args) == 1)
        {
            timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 0));
        }
        else if (mxGetNumberOfElements(cell_args) == 2)
        {
            timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 0));
            peek = GetLogicalScalar(mxGetCell(cell_args, 1));
        }
        else
        {
            throw InvalidArgumentException("TryReceivePacketWait expects zero, one, or two arguments");
        }

        RR_INTRUSIVE_PTR<MessageElement> o;
        bool r = TryReceivePacketWait(o, timeout, peek);
        if (!r)
        {
            mxArray* r2 = mxCreateCellMatrix(2, 1);
            mxSetCell(r2, 0, mxCreateLogicalScalar(false));
            mxSetCell(r2, 1, mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, ::mxREAL));
            return r2;
        }
        else
        {
            mxArray* mx_o = UnpackMessageElementToMxArray(o, Type, rr_cast<PipeClientBase>(GetParent())->GetStub());
            mxArray* r2 = mxCreateCellMatrix(2, 1);
            mxSetCell(r2, 0, mxCreateLogicalScalar(true));
            mxSetCell(r2, 1, mx_o);
            return r2;
        }
    }

    if (membername == "SendPacket")
    {
        if (mxGetNumberOfElements(cell_args) != 1)
            throw InvalidArgumentException("SendPacket expects one argument");
        mxArray* packet = mxGetCell(cell_args, 0);
        SendPacket(PackMxArrayToMessageElement(packet, Type, rr_cast<PipeClientBase>(GetParent())->GetStub()));
        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
    }

    if (membername == "Close")
    {
        if (mxGetNumberOfElements(cell_args) != 0)
            throw InvalidArgumentException("Close expects zero arguments");
        Close();
        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
    }

    throw InvalidArgumentException("Unknown function");
}

void MexPipeEndpoint::subsasgn(const mxArray* S, const mxArray* value)
{
    RR_UNUSED(S);
    RR_UNUSED(value);
    throw InvalidArgumentException("Invalid for pipe endpoint");
}

RR_SHARED_PTR<MexPipeEndpoint> MexPipeClient::Connect(int32_t index)
{

    RR_SHARED_PTR<RobotRaconteur::detail::sync_async_handler<PipeEndpointBase> > t =
        RR_MAKE_SHARED<RobotRaconteur::detail::sync_async_handler<RobotRaconteur::PipeEndpointBase> >();
    AsyncConnect_internal(index,
                          boost::bind(&RobotRaconteur::detail::sync_async_handler<PipeEndpointBase>::operator(), t,
                                      RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                          boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));
    boost::shared_ptr<MexPipeEndpoint> o = boost::dynamic_pointer_cast<MexPipeEndpoint>(t->end());
    o->Type = Type;

    return o;
}

MexPipeClient::MexPipeClient(const std::string& name, const RR_SHARED_PTR<ServiceStub>& stub,
                             const RR_SHARED_PTR<TypeDefinition>& Type, bool unreliable,
                             MemberDefinition_Direction direction)
    : PipeClientBase(name, stub, unreliable, direction)
{
    this->Type = Type;
    this->rawelements = true;
}

RR_SHARED_PTR<PipeEndpointBase> MexPipeClient::CreateNewPipeEndpoint(int32_t index, bool unreliable,
                                                                     MemberDefinition_Direction direction)
{
    return RR_MAKE_SHARED<MexPipeEndpoint>(rr_cast<MexPipeClient>(shared_from_this()), index, 0, Type, unreliable,
                                           direction);
}

// MexWireConnection
RR_INTRUSIVE_PTR<MessageElement> MexWireConnection::GetInValue()
{
    return RRPrimUtil<RR_INTRUSIVE_PTR<MessageElement> >::PreUnpack(GetInValueBase());
}

RR_INTRUSIVE_PTR<MessageElement> MexWireConnection::GetOutValue()
{
    return RRPrimUtil<RR_INTRUSIVE_PTR<MessageElement> >::PreUnpack(GetOutValueBase());
}

void MexWireConnection::SetOutValue(const RR_INTRUSIVE_PTR<MessageElement>& value)
{
    SetOutValueBase(RRPrimUtil<RR_INTRUSIVE_PTR<MessageElement> >::PrePack(value));
}

MexWireConnection::MexWireConnection(const RR_SHARED_PTR<WireBase>& parent, uint32_t endpoint,
                                     const RR_SHARED_PTR<TypeDefinition>& Type, MemberDefinition_Direction direction)
    : WireConnectionBase(parent, endpoint, direction)
{
    this->Type = Type;
    this->wireconnectionid = 0;
}

void MexWireConnection::fire_WireValueChanged(const RR_INTRUSIVE_PTR<RRValue>& value, TimeSpec time) {}

void MexWireConnection::fire_WireClosedCallback()
{
    if (wireconnectionid != 0)
    {
        boost::recursive_mutex::scoped_lock lock(wireconnections_lock);
        try
        {
            wireconnections.erase(wireconnectionid);
        }
        catch (...)
        {}
    }
}

void MexWireConnection::Close()
{
    WireConnectionBase::Close();

    if (wireconnectionid != 0)
    {
        boost::recursive_mutex::scoped_lock lock(wireconnections_lock);
        try
        {
            wireconnections.erase(wireconnectionid);
        }
        catch (...)
        {}
    }
}

RR_SHARED_PTR<ServiceStub> MexWireConnection::GetStub()
{
    RR_SHARED_PTR<WireBase> p1 = GetParent();
    RR_SHARED_PTR<WireClientBase> p2 = RR_DYNAMIC_POINTER_CAST<WireClientBase>(p1);
    if (!p2)
        return RR_SHARED_PTR<ServiceStub>();
    return p2->GetStub();
}

mxArray* TimeSpecToMxArray(TimeSpec& t)
{
    const char* fields[] = {"seconds", "nanoseconds"};
    mwSize s = 1;
    mxArray* mxt = ::mxCreateStructArray(1, &s, 2, fields);
    mxArray* ss = mxCreateNumericMatrix(1, 1, mxINT64_CLASS, mxREAL);
    ((int64_t*)mxGetData(ss))[0] = t.seconds;
    mxArray* n = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    ((int32_t*)mxGetData(n))[0] = t.nanoseconds;

    mxSetField(mxt, 0, "seconds", ss);
    mxSetField(mxt, 0, "nanoseconds", n);

    return mxt;
}

mxArray* MexWireConnection::subsref(const mxArray* S)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);

    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string type = mxToString(mxGetField(S, 0, "type"));
    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    if (c1 == 1)
    {
        if (membername == "InValue")
        {
            return UnpackMessageElementToMxArray(GetInValue(), Type, rr_cast<WireClientBase>(GetParent())->GetStub());
        }

        if (membername == "OutValue")
        {
            return UnpackMessageElementToMxArray(GetOutValue(), Type, rr_cast<WireClientBase>(GetParent())->GetStub());
        }

        if (membername == "LastValueReceivedTime")
        {
            TimeSpec t = GetLastValueReceivedTime();

            return TimeSpecToMxArray(t);
        }

        if (membername == "LastValueSentTime")
        {
            TimeSpec t = GetLastValueSentTime();
            return TimeSpecToMxArray(t);
        }

        if (membername == "InValueValid")
        {
            bool valid = GetInValueValid();
            return mxCreateLogicalScalar(valid);
        }

        if (membername == "OutValueValid")
        {
            bool valid = GetOutValueValid();
            return mxCreateLogicalScalar(valid);
        }

        if (membername == "Direction")
        {
            return CreateInt32Scalar((int32_t)this->Direction());
        }

        if (membername == "InValueLifespan")
        {
            double d = GetInValueLifespan();
            if (d < 0.0)
            {
                return mxCreateDoubleScalar(d);
            }
            else
            {
                return mxCreateDoubleScalar(d / 1000.0);
            }
        }

        if (membername == "OutValueLifespan")
        {
            double d = GetOutValueLifespan();
            if (d < 0.0)
            {
                return mxCreateDoubleScalar(d);
            }
            else
            {
                return mxCreateDoubleScalar(d / 1000.0);
            }
        }

        throw InvalidArgumentException("Unknown property for WireConnection");
    }

    if (c1 != 2)
        throw InvalidArgumentException("WireConnection expects a function request");

    std::string type2 = mxToString(mxGetField(S, 1, "type"));
    if (type2 != "()")
        throw InvalidArgumentException("WireConnection expects a function request");

    if (!mxIsChar(mxGetField(S, 0, "subs")))
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    mxArray* cell_args = mxGetField(S, 1, "subs");

    if (membername == "Close")
    {
        if (mxGetNumberOfElements(cell_args) != 0)
            throw InvalidArgumentException("Close expects zero arguments");
        Close();
        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
    }

    if (membername == "WaitInValueValid")
    {
        int32_t timeout = 0;
        if (mxGetNumberOfElements(cell_args) == 0)
        {
            timeout = RR_TIMEOUT_INFINITE;
        }
        else if (mxGetNumberOfElements(cell_args) == 1)
        {
            timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 0));
        }
        else
        {
            throw InvalidArgumentException("WaitInValueValid expects zero or one arguments");
        }

        bool r = WaitInValueValid(timeout);
        return mxCreateLogicalScalar(r);
    }

    if (membername == "WaitOutValueValid")
    {
        int32_t timeout = 0;
        if (mxGetNumberOfElements(cell_args) == 0)
        {
            timeout = RR_TIMEOUT_INFINITE;
        }
        else if (mxGetNumberOfElements(cell_args) == 1)
        {
            timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 0));
        }
        else
        {
            throw InvalidArgumentException("WaitInValueValid expects zero or one arguments");
        }

        bool r = WaitOutValueValid(timeout);
        return mxCreateLogicalScalar(r);
    }

    throw InvalidArgumentException("Unknown function");
}

void MexWireConnection::subsasgn(const mxArray* S, const mxArray* value)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);

    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string type = mxToString(mxGetField(S, 0, "type"));
    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    if (c1 == 1)
    {

        if (membername == "OutValue")
        {
            SetOutValue(PackMxArrayToMessageElement(value, Type, rr_cast<WireClientBase>(GetParent())->GetStub()));
            return;
        }

        if (membername == "InValueLifespan")
        {
            double d = GetDoubleScalar(value);
            if (d < 0.0)
            {
                SetInValueLifespan(-1);
            }
            else
            {
                SetInValueLifespan(boost::numeric_cast<int32_t>(d * 1000.0));
            }
            return;
        }

        if (membername == "OutValueLifespan")
        {
            double d = GetDoubleScalar(value);
            if (d < 0.0)
            {
                SetOutValueLifespan(-1);
            }
            else
            {
                SetOutValueLifespan(boost::numeric_cast<int32_t>(d * 1000.0));
            }
            return;
        }
    }
    throw InvalidArgumentException("Unknown function");
}

// MexWireClient

RR_SHARED_PTR<MexWireConnection> MexWireClient::Connect()
{
    RR_SHARED_PTR<RobotRaconteur::detail::sync_async_handler<WireConnectionBase> > t =
        RR_MAKE_SHARED<RobotRaconteur::detail::sync_async_handler<WireConnectionBase> >();
    AsyncConnect_internal(boost::bind(&RobotRaconteur::detail::sync_async_handler<WireConnectionBase>::operator(), t,
                                      RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
                          boost::numeric_cast<int32_t>(GetNode()->GetRequestTimeout()));

    boost::shared_ptr<MexWireConnection> o = boost::dynamic_pointer_cast<MexWireConnection>(t->end());
    o->Type = Type;
    return o;
}

MexWireClient::MexWireClient(const std::string& name, const RR_SHARED_PTR<ServiceStub>& stub,
                             const RR_SHARED_PTR<TypeDefinition>& Type, MemberDefinition_Direction direction)
    : WireClientBase(name, stub, direction)
{
    this->Type = Type;
    this->rawelements = true;
}

RR_SHARED_PTR<WireConnectionBase> MexWireClient::CreateNewWireConnection(MemberDefinition_Direction direction)
{
    return RR_MAKE_SHARED<MexWireConnection>(rr_cast<MexWireClient>(shared_from_this()), 0, Type, direction);
}

mxArray* MexWireClient::PeekInValue()
{
    TimeSpec ts;
    RR_INTRUSIVE_PTR<MessageElement> m = RR_DYNAMIC_POINTER_CAST<MessageElement>(PeekInValueBase(ts));
    return UnpackMessageElementToMxArray(m, Type, GetStub());
}

mxArray* MexWireClient::PeekOutValue()
{
    TimeSpec ts;
    RR_INTRUSIVE_PTR<MessageElement> m = RR_DYNAMIC_POINTER_CAST<MessageElement>(PeekOutValueBase(ts));
    return UnpackMessageElementToMxArray(m, Type, GetStub());
}

void MexWireClient::PokeOutValue(const mxArray* value)
{
    RR_INTRUSIVE_PTR<MessageElement> m = PackMxArrayToMessageElement(value, Type, GetStub());
    PokeOutValueBase(m);
}

#define RR_WAMCU_READ_TYPE(type)                                                                                       \
    RR_SHARED_PTR<ArrayMemory<type> > type##_var = boost::dynamic_pointer_cast<ArrayMemory<type> >(mem);               \
    if (type##_var)                                                                                                    \
    {                                                                                                                  \
        mxArray* mxdat = mxCreateNumericMatrix(boost::numeric_cast<size_t>(count), 1,                                  \
                                               rrDataTypeToMxClassID(RRPrimUtil<type>::GetTypeID()), mxREAL);          \
        RR_INTRUSIVE_PTR<RRArray<type> > dat =                                                                         \
            AttachRRArray<type>((type*)mxGetData(mxdat), boost::numeric_cast<size_t>(count), false);                   \
        type##_var->Read(memorypos, dat, 0, count);                                                                    \
        return mxdat;                                                                                                  \
    }

mxArray* MexArrayMemoryClientUtil::Read(const RR_SHARED_PTR<ArrayMemoryBase>& mem, uint64_t memorypos, uint64_t count)
{
    /*RR_SHARED_PTR<ArrayMemory<int8_t> > i8=rr_cast<ArrayMemory<int8_t> >(mem);
    if (i8)
    {
        RR_INTRUSIVE_PTR<RRArray<int8_t> > dat=AllocateRRArray<int8_t>(count);
        i8->Read(memorypos,dat,bufferpos,count);
        return dat;
    }*/

    RR_WAMCU_READ_TYPE(int8_t);
    RR_WAMCU_READ_TYPE(uint8_t);
    RR_WAMCU_READ_TYPE(int16_t);
    RR_WAMCU_READ_TYPE(uint16_t);
    RR_WAMCU_READ_TYPE(int32_t);
    RR_WAMCU_READ_TYPE(uint32_t);
    RR_WAMCU_READ_TYPE(int64_t);
    RR_WAMCU_READ_TYPE(uint64_t);
    RR_WAMCU_READ_TYPE(double);
    RR_WAMCU_READ_TYPE(float);

    RR_SHARED_PTR<ArrayMemory<rr_bool> > rr_bool_var = boost::dynamic_pointer_cast<ArrayMemory<rr_bool> >(mem);
    if (rr_bool_var)
    {
        RR_INTRUSIVE_PTR<RRArray<rr_bool> > dat = AllocateRRArray<rr_bool>(count);
        rr_bool_var->Read(memorypos, dat, 0, count);
        mxArray* mxdat = mxCreateLogicalMatrix(boost::numeric_cast<size_t>(count), 1);
        for (size_t i = 0; i < count; i++)
        {
            ::mxGetLogicals(mxdat)[i] = (*dat)[i] != 0;
        }
        return mxdat;
    }

    RR_SHARED_PTR<ArrayMemory<cdouble> > cdouble_var = boost::dynamic_pointer_cast<ArrayMemory<cdouble> >(mem);
    if (cdouble_var)
    {
        RR_INTRUSIVE_PTR<RRArray<cdouble> > dat = AllocateRRArray<cdouble>(count);
        cdouble_var->Read(memorypos, dat, 0, count);
        mxArray* mxdat = mxCreateNumericMatrix(boost::numeric_cast<size_t>(count), 1, mxDOUBLE_CLASS, mxCOMPLEX);
        mxComplexDouble* mx_complex = mxGetComplexDoubles(mxdat);
        for (size_t i = 0; i < count; i++)
        {
            mx_complex[i].real = (*dat)[i].real;
            mx_complex[i].imag = (*dat)[i].imag;
        }
        return mxdat;
    }

    RR_SHARED_PTR<ArrayMemory<cfloat> > cfloat_var = boost::dynamic_pointer_cast<ArrayMemory<cfloat> >(mem);
    if (cfloat_var)
    {
        RR_INTRUSIVE_PTR<RRArray<cfloat> > dat = AllocateRRArray<cfloat>(count);
        cfloat_var->Read(memorypos, dat, 0, count);
        mxArray* mxdat = mxCreateNumericMatrix(boost::numeric_cast<size_t>(count), 1, mxSINGLE_CLASS, mxCOMPLEX);
        mxComplexSingle* mx_complex = mxGetComplexSingles(mxdat);
        for (size_t i = 0; i < count; i++)
        {
            mx_complex[i].real = (*dat)[i].real;
            mx_complex[i].imag = (*dat)[i].imag;
        }
        return mxdat;
    }

    throw InvalidArgumentException("Invalid memory data type");
}

#define RR_WAMCU_WRITE_TYPE(type)                                                                                      \
    RR_SHARED_PTR<ArrayMemory<type> > type##_var = boost::dynamic_pointer_cast<ArrayMemory<type> >(mem);               \
    if (type##_var)                                                                                                    \
    {                                                                                                                  \
        if (mxGetClassID(buffer) != rrDataTypeToMxClassID(RRPrimUtil<type>::GetTypeID()))                              \
            throw InvalidArgumentException("Memory data type mismatch, expecting " +                                   \
                                           GetRRDataTypeString(RRPrimUtil<type>::GetTypeID()));                        \
        type##_var->Write(memorypos,                                                                                   \
                          AttachRRArray<type>((type*)mxGetData(buffer), mxGetNumberOfElements(buffer), false),         \
                          bufferpos, count);                                                                           \
        return;                                                                                                        \
    }

void MexArrayMemoryClientUtil::Write(const RR_SHARED_PTR<ArrayMemoryBase>& mem, uint64_t memorypos,
                                     const mxArray* buffer, uint64_t bufferpos, uint64_t count)
{
    if (buffer == NULL)
        throw InvalidArgumentException("Data for memory write must not be null");
    size_t ndims = mxGetNumberOfDimensions(buffer);
    const mwSize* dims = mxGetDimensions(buffer);
    if (ndims < 1)
        throw InvalidArgumentException("Invalid memory write dimensions");
    if (dims[0] != count)
        throw InvalidArgumentException("Subscripted assignment dimension mismatch.");
    for (size_t i = 1; i < ndims; i++)
        if (dims[i] != 1)
            throw InvalidArgumentException("Subscripted assignment dimension mismatch.");

    RR_WAMCU_WRITE_TYPE(int8_t);
    RR_WAMCU_WRITE_TYPE(uint8_t);
    RR_WAMCU_WRITE_TYPE(int16_t);
    RR_WAMCU_WRITE_TYPE(uint16_t);
    RR_WAMCU_WRITE_TYPE(int32_t);
    RR_WAMCU_WRITE_TYPE(uint32_t);
    RR_WAMCU_WRITE_TYPE(int64_t);
    RR_WAMCU_WRITE_TYPE(uint64_t);
    RR_WAMCU_WRITE_TYPE(double);
    RR_WAMCU_WRITE_TYPE(float);

    RR_SHARED_PTR<ArrayMemory<rr_bool> > rr_bool_var = boost::dynamic_pointer_cast<ArrayMemory<rr_bool> >(mem);
    if (rr_bool_var)
    {
        if (mxGetClassID(buffer) != mxLOGICAL_CLASS)
            throw InvalidArgumentException("Memory data type mismatch, expecting logical");
        RR_INTRUSIVE_PTR<RRArray<rr_bool> > dat = AllocateRRArray<rr_bool>(mxGetNumberOfElements(buffer));
        for (size_t i = 0; i < dat->size(); i++)
        {
            (*dat)[i] = mxGetLogicals(buffer)[i] ? 1 : 0;
        }
        rr_bool_var->Write(memorypos, dat, bufferpos, count);
        return;
    }

    RR_SHARED_PTR<ArrayMemory<cdouble> > cdouble_var = boost::dynamic_pointer_cast<ArrayMemory<cdouble> >(mem);
    if (cdouble_var)
    {
        if (mxGetClassID(buffer) != mxDOUBLE_CLASS || mxIsComplex(buffer) != mxCOMPLEX)
            throw InvalidArgumentException("Memory data type mismatch, expecting complex double");
        RR_INTRUSIVE_PTR<RRArray<cdouble> > dat = AllocateRRArray<cdouble>(mxGetNumberOfElements(buffer));
        mxComplexDouble* mx_complex = mxGetComplexDoubles(buffer);
        for (size_t i = 0; i < dat->size(); i++)
        {
            (*dat)[i].real = mx_complex[i].real;
            (*dat)[i].imag = mx_complex[i].imag;
        }
        cdouble_var->Write(memorypos, dat, bufferpos, count);
        return;
    }

    RR_SHARED_PTR<ArrayMemory<cfloat> > cfloat_var = boost::dynamic_pointer_cast<ArrayMemory<cfloat> >(mem);
    if (cfloat_var)
    {
        if (mxGetClassID(buffer) != mxSINGLE_CLASS || mxIsComplex(buffer) != mxCOMPLEX)
            throw InvalidArgumentException("Memory data type mismatch, expecting complex single");
        RR_INTRUSIVE_PTR<RRArray<cfloat> > dat = AllocateRRArray<cfloat>(mxGetNumberOfElements(buffer));
        mxComplexSingle* mx_complex = mxGetComplexSingles(buffer);
        for (size_t i = 0; i < dat->size(); i++)
        {
            (*dat)[i].real = mx_complex[i].real;
            (*dat)[i].imag = mx_complex[i].imag;
        }
        cfloat_var->Write(memorypos, dat, bufferpos, count);
        return;
    }

    throw InvalidArgumentException("Invalid memory data type");
}

#define RR_WMDAMCU_READ_TYPE(type)                                                                                     \
    RR_SHARED_PTR<MultiDimArrayMemory<type> > type##_var =                                                             \
        boost::dynamic_pointer_cast<MultiDimArrayMemory<type> >(mem);                                                  \
    if (type##_var)                                                                                                    \
    {                                                                                                                  \
        mxArray* mxdat =                                                                                               \
            mxCreateNumericArray(dims.size(), &dims[0], rrDataTypeToMxClassID(RRPrimUtil<type>::GetTypeID()), mxREAL); \
        RR_INTRUSIVE_PTR<RRArray<type> > realdat = AttachRRArray<type>((type*)mxGetData(mxdat), elems, false);         \
        RR_INTRUSIVE_PTR<RRMultiDimArray<type> > dat =                                                                 \
            AllocateRRMultiDimArray<type>(VectorToRRArray<uint32_t>(count), realdat);                                  \
        type##_var->Read(memorypos, dat, bufferpos, count);                                                            \
        return mxdat;                                                                                                  \
    }

mxArray* MexMultiDimArrayMemoryClientUtil::Read(const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem,
                                                const std::vector<uint64_t>& memorypos,
                                                const std::vector<uint64_t>& count)
{
    size_t elems = 1;
    BOOST_FOREACH (const uint64_t& e, count)
    {
        elems *= boost::numeric_cast<size_t>(e);
    }

    /*RR_SHARED_PTR<MultiDimArrayMemory<int8_t> > i8=rr_cast<MultiDimArrayMemory<int8_t> >(mem);
    if (i8)
    {

        std::vector<mwSize> dims(count.size());
        for (size_t i=0; i<count.size(); i++)
        {
            dims[i]=(mwSize)count[i];
        }
        mxComplexity mxc=i8->Complex() ? mxCOMPLEX : mxREAL;
        mxArray* mxdat= mxCreateNumericArray(dims.size(),
    &dims[0],rrDataTypeToMxClassID(RRPrimUtil<uint8_t>::GetTypeID()),mxc);

        RR_INTRUSIVE_PTR<RRArray<int8_t> > realdat=AttachRRArray<int8_t>((int8_t*)mxGetData(mxdat),elems,false);
        RR_INTRUSIVE_PTR<RRArray<int8_t> > imagdat;
        if (mxc==mxCOMPLEX)
        {
            imagdat=AttachRRArray<int8_t>((int8_t*)mxGetImagData(mxdat),elems,false);
        }

        RR_INTRUSIVE_PTR<RRMultiDimArray<int8_t> > dat=RR_MAKE_SHARED<RRMultiDimArray<int8_t>
    >(VectorToRRArray<int32_t>(count),realdat,imagdat);

        std::vector<uint64_t> bufferpos(count.size());
        std::fill(bufferpos.begin(),bufferpos.end(),0);

        i8->Read(memorypos,dat,bufferpos,count);

        return mxdat;
    }*/

    std::vector<mwSize> dims(count.size());
    for (size_t i = 0; i < count.size(); i++)
    {
        dims[i] = (mwSize)count[i];
    }

    std::vector<uint64_t> bufferpos(count.size());
    std::fill(bufferpos.begin(), bufferpos.end(), 0);

    RR_WMDAMCU_READ_TYPE(int8_t);
    RR_WMDAMCU_READ_TYPE(uint8_t);
    RR_WMDAMCU_READ_TYPE(int16_t);
    RR_WMDAMCU_READ_TYPE(uint16_t);
    RR_WMDAMCU_READ_TYPE(int32_t);
    RR_WMDAMCU_READ_TYPE(uint32_t);
    RR_WMDAMCU_READ_TYPE(int64_t);
    RR_WMDAMCU_READ_TYPE(uint64_t);
    RR_WMDAMCU_READ_TYPE(double);
    RR_WMDAMCU_READ_TYPE(float);

    RR_SHARED_PTR<MultiDimArrayMemory<rr_bool> > rr_bool_var =
        boost::dynamic_pointer_cast<MultiDimArrayMemory<rr_bool> >(mem);
    if (rr_bool_var)
    {
        mxArray* mxdat = mxCreateLogicalArray(dims.size(), &dims[0]);
        RR_INTRUSIVE_PTR<RRArray<rr_bool> > realdat = AllocateRRArray<rr_bool>(elems);
        RR_INTRUSIVE_PTR<RRMultiDimArray<rr_bool> > dat =
            AllocateRRMultiDimArray<rr_bool>(VectorToRRArray<uint32_t>(count), realdat);
        rr_bool_var->Read(memorypos, dat, bufferpos, count);
        for (size_t i = 0; i < elems; i++)
        {
            ::mxGetLogicals(mxdat)[i] = (*realdat)[i] != 0;
        }
        return mxdat;
    }

    RR_SHARED_PTR<MultiDimArrayMemory<cdouble> > cdouble_var =
        boost::dynamic_pointer_cast<MultiDimArrayMemory<cdouble> >(mem);
    if (cdouble_var)
    {
        mxArray* mxdat = mxCreateNumericArray(dims.size(), &dims[0], mxDOUBLE_CLASS, mxCOMPLEX);
        RR_INTRUSIVE_PTR<RRArray<cdouble> > realdat = AllocateRRArray<cdouble>(elems);
        RR_INTRUSIVE_PTR<RRMultiDimArray<cdouble> > dat =
            AllocateRRMultiDimArray<cdouble>(VectorToRRArray<uint32_t>(count), realdat);
        cdouble_var->Read(memorypos, dat, bufferpos, count);
        mxComplexDouble* mx_complex = mxGetComplexDoubles(mxdat);
        for (size_t i = 0; i < elems; i++)
        {
            mx_complex[i].real = (*realdat)[i].real;
            mx_complex[i].imag = (*realdat)[i].imag;
        }
        return mxdat;
    }

    RR_SHARED_PTR<MultiDimArrayMemory<cfloat> > cfloat_var =
        boost::dynamic_pointer_cast<MultiDimArrayMemory<cfloat> >(mem);
    if (cfloat_var)
    {
        mxArray* mxdat = mxCreateNumericArray(dims.size(), &dims[0], mxSINGLE_CLASS, mxCOMPLEX);
        RR_INTRUSIVE_PTR<RRArray<cfloat> > realdat = AllocateRRArray<cfloat>(elems);
        RR_INTRUSIVE_PTR<RRMultiDimArray<cfloat> > dat =
            AllocateRRMultiDimArray<cfloat>(VectorToRRArray<uint32_t>(count), realdat);
        cfloat_var->Read(memorypos, dat, bufferpos, count);
        mxComplexSingle* mx_complex = mxGetComplexSingles(mxdat);
        for (size_t i = 0; i < elems; i++)
        {
            mx_complex[i].real = (*realdat)[i].real;
            mx_complex[i].imag = (*realdat)[i].imag;
        }
        return mxdat;
    }

    throw InvalidArgumentException("Invalid memory data type");
}

#define RR_WMDAMCU_WRITE_TYPE(type)                                                                                    \
    RR_SHARED_PTR<MultiDimArrayMemory<type> > type##_var =                                                             \
        boost::dynamic_pointer_cast<MultiDimArrayMemory<type> >(mem);                                                  \
    if (type##_var)                                                                                                    \
    {                                                                                                                  \
        if (mxGetClassID(buffer) != rrDataTypeToMxClassID(RRPrimUtil<type>::GetTypeID()))                              \
            throw InvalidArgumentException("Memory data type mismatch, expecting " +                                   \
                                           GetRRDataTypeString(RRPrimUtil<uint8_t>::GetTypeID()));                     \
        RR_INTRUSIVE_PTR<RRMultiDimArray<type> > buffer2 = AllocateRRMultiDimArray<type>(                              \
            arrdims, AttachRRArray((type*)mxGetData(buffer), mxGetNumberOfElements(buffer), false));                   \
        type##_var->Write(memorypos, buffer2, bufferpos, count);                                                       \
        return;                                                                                                        \
    }

void MexMultiDimArrayMemoryClientUtil::Write(const RR_SHARED_PTR<MultiDimArrayMemoryBase>& mem,
                                             const std::vector<uint64_t>& memorypos, const mxArray* buffer,
                                             const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
{
    if (buffer == NULL)
        throw InvalidArgumentException("Data for memory write must not be null");

    size_t ndims = mxGetNumberOfDimensions(buffer);
    const mwSize* dims = mxGetDimensions(buffer);
    RR_INTRUSIVE_PTR<RRArray<uint32_t> > arrdims = AllocateRRArray<uint32_t>(count.size());
    for (size_t i = 0; i < count.size(); i++)
    {
        if (i < ndims)
        {
            arrdims->data()[i] = boost::numeric_cast<uint32_t>(dims[i]);
        }
        else
        {
            if (count[i] != 1)
                throw InvalidArgumentException("Index out of range.");
            arrdims->data()[i] = 1;
        }
    }

    /*RR_SHARED_PTR<MultiDimArrayMemory<int8_t> > i8=rr_cast<MultiDimArrayMemory<int8_t> >(mem);
    if (i8)
    {
        if (mxGetClassID(buffer) != rrDataTypeToMxClassID(RRPrimUtil<uint8_t>::GetTypeID())) throw
    InvalidArgumentException("Memory data type mismatch, expecting " +
    GetRRDataTypeString(RRPrimUtil<uint8_t>::GetTypeID())); RR_INTRUSIVE_PTR<RRArray<int8_t> > imag; if
    (::mxIsComplex(buffer) && i8->Complex())
    imag=AttachRRArray((int8_t*)mxGetImagData(buffer),mxGetNumberOfElements(buffer),false); if (!mxIsComplex(buffer) &&
    i8->Complex())
        {
            imag=AllocateRRArray<int8_t>(mxGetNumberOfElements(buffer));
            memset(imag->ptr(),0,imag->Length()*sizeof(int8_t));
        }
        RR_INTRUSIVE_PTR<RRMultiDimArray<int8_t> > buffer2=RR_MAKE_SHARED<RRMultiDimArray<int8_t>
    >(arrdims,AttachRRArray((int8_t*)mxGetData(buffer),mxGetNumberOfElements(buffer),false));
        i8->Write(memorypos,buffer2,bufferpos,count);
    }*/

    RR_WMDAMCU_WRITE_TYPE(int8_t);
    RR_WMDAMCU_WRITE_TYPE(uint8_t);
    RR_WMDAMCU_WRITE_TYPE(int16_t);
    RR_WMDAMCU_WRITE_TYPE(uint16_t);
    RR_WMDAMCU_WRITE_TYPE(int32_t);
    RR_WMDAMCU_WRITE_TYPE(uint32_t);
    RR_WMDAMCU_WRITE_TYPE(int64_t);
    RR_WMDAMCU_WRITE_TYPE(uint64_t);
    RR_WMDAMCU_WRITE_TYPE(double);
    RR_WMDAMCU_WRITE_TYPE(float);

    RR_SHARED_PTR<MultiDimArrayMemory<rr_bool> > rr_bool_var =
        boost::dynamic_pointer_cast<MultiDimArrayMemory<rr_bool> >(mem);
    if (rr_bool_var)
    {
        if (mxGetClassID(buffer) != mxLOGICAL_CLASS)
            throw InvalidArgumentException("Memory data type mismatch, expecting logical");
        RR_INTRUSIVE_PTR<RRMultiDimArray<rr_bool> > buffer2 =
            AllocateRRMultiDimArray<rr_bool>(arrdims, AllocateRRArray<rr_bool>(mxGetNumberOfElements(buffer)));
        for (size_t i = 0; i < mxGetNumberOfElements(buffer); i++)
        {
            (*buffer2->Array)[i] = ::mxGetLogicals(buffer)[i] ? 1 : 0;
        }
        rr_bool_var->Write(memorypos, buffer2, bufferpos, count);
        return;
    }

    RR_SHARED_PTR<MultiDimArrayMemory<cdouble> > cdouble_var =
        boost::dynamic_pointer_cast<MultiDimArrayMemory<cdouble> >(mem);
    if (cdouble_var)
    {
        if (mxGetClassID(buffer) != mxDOUBLE_CLASS || mxIsComplex(buffer) != mxCOMPLEX)
            throw InvalidArgumentException("Memory data type mismatch, expecting complex double");
        RR_INTRUSIVE_PTR<RRMultiDimArray<cdouble> > buffer2 =
            AllocateRRMultiDimArray<cdouble>(arrdims, AllocateRRArray<cdouble>(mxGetNumberOfElements(buffer)));
        mxComplexDouble* mx_complex = mxGetComplexDoubles(buffer);
        for (size_t i = 0; i < mxGetNumberOfElements(buffer); i++)
        {
            (*buffer2->Array)[i].real = mx_complex[i].real;
            (*buffer2->Array)[i].imag = mx_complex[i].imag;
        }
        cdouble_var->Write(memorypos, buffer2, bufferpos, count);
        return;
    }

    RR_SHARED_PTR<MultiDimArrayMemory<cfloat> > cfloat_var =
        boost::dynamic_pointer_cast<MultiDimArrayMemory<cfloat> >(mem);
    if (cfloat_var)
    {
        if (mxGetClassID(buffer) != mxSINGLE_CLASS || mxIsComplex(buffer) != mxCOMPLEX)
            throw InvalidArgumentException("Memory data type mismatch, expecting complex single");
        RR_INTRUSIVE_PTR<RRMultiDimArray<cfloat> > buffer2 =
            AllocateRRMultiDimArray<cfloat>(arrdims, AllocateRRArray<cfloat>(mxGetNumberOfElements(buffer)));
        mxComplexSingle* mx_complex = mxGetComplexSingles(buffer);
        for (size_t i = 0; i < mxGetNumberOfElements(buffer); i++)
        {
            (*buffer2->Array)[i].real = mx_complex[i].real;
            (*buffer2->Array)[i].imag = mx_complex[i].imag;
        }
        cfloat_var->Write(memorypos, buffer2, bufferpos, count);
        return;
    }

    throw InvalidArgumentException("Invalid memory data type");
}

mxArray* ServiceInfo2ToMxArray(const ServiceInfo2& info)
{
    const char* fields[] = {"Name",   "RootObjectType", "RootObjectImplements", "ConnectionURL", "Attributes",
                            "NodeID", "NodeName"};
    mxArray* serviceinfo = ::mxCreateStructMatrix(1, 1, 7, fields);
    mxSetField(serviceinfo, 0, "Name", mxCreateString(info.Name.c_str()));
    mxSetField(serviceinfo, 0, "RootObjectType", mxCreateString(info.RootObjectType.c_str()));

    mxArray* RootObjectImplements = mxCreateCellMatrix(info.RootObjectImplements.size(), 1);
    for (size_t j = 0; j < info.RootObjectImplements.size(); j++)
    {
        mxSetCell(RootObjectImplements, 0, mxCreateString(info.RootObjectImplements[j].c_str()));
    }
    mxSetField(serviceinfo, 0, "RootObjectImplements", RootObjectImplements);

    mxArray* ConnectionURL = mxCreateCellMatrix(info.ConnectionURL.size(), 1);
    for (size_t j = 0; j < info.ConnectionURL.size(); j++)
    {
        mxSetCell(ConnectionURL, j, mxCreateString(info.ConnectionURL[j].c_str()));
    }
    mxSetField(serviceinfo, 0, "ConnectionURL", ConnectionURL);

    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > Attributes = AllocateEmptyRRMap<std::string, RRValue>();
    Attributes->GetStorageContainer() = info.Attributes;
    RR_INTRUSIVE_PTR<MessageElement> mattributes =
        CreateMessageElement("value", RobotRaconteurNode::s()->PackMapType<std::string, RRValue>(Attributes));

    boost::shared_ptr<TypeDefinition> tdef = boost::make_shared<TypeDefinition>();
    tdef->ContainerType = DataTypes_ContainerTypes_map_string;
    tdef->Type = DataTypes_varvalue_t;

    mxSetField(serviceinfo, 0, "Attributes",
               UnpackMessageElementToMxArray(mattributes, tdef, RR_SHARED_PTR<ServiceStub>()));

    mxSetField(serviceinfo, 0, "NodeID", mxCreateString(info.NodeID.ToString().c_str()));
    mxSetField(serviceinfo, 0, "NodeName", mxCreateString(info.NodeName.c_str()));

    return serviceinfo;
}

mxArray* FindService(const mxArray* name)
{
    std::string sname = mxToString(name);
    std::vector<std::string> schemes;
    schemes.push_back("rr+tcp");
    schemes.push_back("rrs+tcp");
    schemes.push_back("rr+local");
    schemes.push_back("rr+usb");
    schemes.push_back("rr+pci");

    std::vector<ServiceInfo2> results = RobotRaconteurNode::s()->FindServiceByType(sname, schemes);
    mxArray* out = mxCreateCellMatrix(results.size(), 1);

    for (size_t i = 0; i < results.size(); i++)
    {
        mxArray* serviceinfo = ServiceInfo2ToMxArray(results[i]);
        mxSetCell(out, i, serviceinfo);
    }

    return out;
}

mxArray* FindNodeByID(const mxArray* name)
{
    std::string sname = mxToString(name);
    std::vector<std::string> schemes;
    schemes.push_back("rr+tcp");
    schemes.push_back("rrs+tcp");
    schemes.push_back("rr+local");
    schemes.push_back("rr+usb");
    schemes.push_back("rr+pci");

    std::vector<NodeInfo2> results = RobotRaconteurNode::s()->FindNodeByID(NodeID(sname), schemes);
    mxArray* out = mxCreateCellMatrix(results.size(), 1);

    for (size_t i = 0; i < results.size(); i++)
    {
        const char* fields[] = {"NodeID", "NodeName", "ConnectionURL"};
        mxArray* serviceinfo = ::mxCreateStructMatrix(1, 1, 3, fields);

        mxArray* ConnectionURL = mxCreateCellMatrix(results[i].ConnectionURL.size(), 1);
        for (size_t j = 0; j < results[i].ConnectionURL.size(); j++)
        {
            mxSetCell(ConnectionURL, j, mxCreateString(results[i].ConnectionURL[j].c_str()));
        }
        mxSetField(serviceinfo, 0, "ConnectionURL", ConnectionURL);

        mxSetField(serviceinfo, 0, "NodeID", mxCreateString(results[i].NodeID.ToString().c_str()));
        mxSetField(serviceinfo, 0, "NodeName", mxCreateString(results[i].NodeName.c_str()));

        mxSetCell(out, i, serviceinfo);
    }

    return out;
}

mxArray* FindNodeByName(const mxArray* name)
{
    std::string sname = mxToString(name);
    std::vector<std::string> schemes;
    schemes.push_back("rr+tcp");
    schemes.push_back("rrs+tcp");
    schemes.push_back("rr+local");
    schemes.push_back("rr+usb");
    schemes.push_back("rr+pci");
    schemes.push_back("rr+bluetooth");

    std::vector<NodeInfo2> results = RobotRaconteurNode::s()->FindNodeByName(sname, schemes);
    mxArray* out = mxCreateCellMatrix(results.size(), 1);

    for (size_t i = 0; i < results.size(); i++)
    {
        const char* fields[] = {"NodeID", "NodeName", "ConnectionURL"};
        mxArray* serviceinfo = ::mxCreateStructMatrix(1, 1, 3, fields);

        mxArray* ConnectionURL = mxCreateCellMatrix(results[i].ConnectionURL.size(), 1);
        for (size_t j = 0; j < results[i].ConnectionURL.size(); j++)
        {
            mxSetCell(ConnectionURL, j, mxCreateString(results[i].ConnectionURL[j].c_str()));
        }
        mxSetField(serviceinfo, 0, "ConnectionURL", ConnectionURL);

        mxSetField(serviceinfo, 0, "NodeID", mxCreateString(results[i].NodeID.ToString().c_str()));
        mxSetField(serviceinfo, 0, "NodeName", mxCreateString(results[i].NodeName.c_str()));

        mxSetCell(out, i, serviceinfo);
    }

    return out;
}

void UpdateDetectedNodes()
{
    std::vector<std::string> schemes;
    schemes.push_back("rr+tcp");
    schemes.push_back("rrs+tcp");
    schemes.push_back("rr+local");
    schemes.push_back("rr+usb");
    schemes.push_back("rr+pci");
    schemes.push_back("rr+bluetooth");
    RobotRaconteurNode::s()->UpdateDetectedNodes(schemes);
}

mxArray* GetDetectedNodes()
{
    std::vector<NodeDiscoveryInfo> o1 = RobotRaconteurNode::s()->GetDetectedNodes();
    mxArray* o = mxCreateCellMatrix(o1.size(), 1);
    for (size_t j = 0; j < o1.size(); j++)
    {
        mxSetCell(o, j, mxCreateString(o1.at(j).NodeID.ToString().c_str()));
    }
    return o;
}

struct constant_type
{
    std::string name;
    mxArray* data;

    constant_type() { data = NULL; }
};

struct obj_constant_type
{
    std::string name;
    std::vector<constant_type> data;
};

constant_type convert_constant(const RR_SHARED_PTR<ConstantDefinition>& c1,
                               std::vector<RR_SHARED_PTR<ConstantDefinition> >& constants)
{
    RR_SHARED_PTR<TypeDefinition> def = c1->Type;

    constant_type c;
    c.name = c1->Name;

    if (def->Type == DataTypes_string_t)
    {
        std::string data_utf8 = c1->ValueToString();
        std::basic_string<uint16_t> data_utf16 = boost::locale::conv::utf_to_utf<uint16_t>(data_utf8);

        mwSize data_size[2];
        data_size[0] = 1;
        data_size[1] = data_utf16.size();

        mxArray* mx_str = mxCreateCharArray(2, data_size);
        uint16_t* mx_str_ptr = (uint16_t*)mxGetData(mx_str);
        memcpy(mx_str_ptr, data_utf16.c_str(), data_size[1] * sizeof(uint16_t));
        c.data = mx_str;
        return c;
    }

    if (def->Type >= DataTypes_double_t && def->Type <= DataTypes_uint64_t)
    {
        if (def->ArrayType != DataTypes_ArrayTypes_multidimarray)
        {
            boost::shared_ptr<TypeDefinition> def2 = def->Clone();
            def2->Name = "";
            def2->RemoveContainers();
            def2->RemoveArray();

            if (def->ArrayType == DataTypes_ArrayTypes_array)
            {
                std::string command =
                    def2->ToString() + "([" + trim_copy_if(c1->Value, boost::is_any_of("{}")) + "])';";

                mxArray* lhs[1];
                mxArray* rhs[1];
                rhs[0] = mxCreateString(command.c_str());
                if (mexCallMATLAB(1, lhs, 1, rhs, "eval"))
                {
                    throw InvalidArgumentException("Invalid constant type");
                }
                c.data = lhs[0];
                return c;
            }
            else
            {
                std::string command = def2->ToString() + "(" + c1->Value + ");";
                mxArray* lhs[1];
                mxArray* rhs[1];
                rhs[0] = mxCreateString(command.c_str());
                if (mexCallMATLAB(1, lhs, 1, rhs, "eval"))
                {
                    throw InvalidArgumentException("Invalid constant type");
                }
                c.data = lhs[0];
                return c;
            }
        }
    }

    if (def->Type == DataTypes_structure_t)
    {
        std::vector<ConstantDefinition_StructField> fields;
        fields = c1->ValueToStructFields();
        boost::shared_array<const char*> fieldnames(new const char*[fields.size()]);
        size_t pos = 0;
        BOOST_FOREACH (ConstantDefinition_StructField& e, fields)
        {
            fieldnames.get()[pos] = e.Name.c_str();
            pos++;
        }

        mxArray* o = ::mxCreateStructMatrix(1, 1, (int)pos, fieldnames.get());

        size_t pos2 = 0;
        BOOST_FOREACH (ConstantDefinition_StructField& e, fields)
        {
            BOOST_FOREACH (RR_SHARED_PTR<ConstantDefinition>& e2, constants)
            {
                if (e2->Name == e.ConstantRefName)
                {
                    constant_type o1 = convert_constant(e2, constants);
                    mxSetFieldByNumber(o, 0, (int)pos2, o1.data);

                    break;
                }
            }

            pos2++;
        }

        c.data = o;
        return c;
    }

    throw InvalidArgumentException("Invalid constant type");
}

mxArray* ServiceDefinitionConstants(const boost::shared_ptr<ServiceDefinition>& def)
{
    std::vector<constant_type> base_consts;
    std::vector<obj_constant_type> obj_consts;

    BOOST_FOREACH (std::string& e, def->Options)
    {
        if (boost::starts_with(e, "constant"))
        {
            try
            {
                RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(def);
                c->FromString(e);

                std::vector<RR_SHARED_PTR<ConstantDefinition> > c2;
                constant_type c1 = convert_constant(c, c2);
                base_consts.push_back(c1);
            }
            catch (...)
            {
                ::mexWarnMsgTxt("Error converting constant");
            }
        }
    }

    BOOST_FOREACH (const RR_SHARED_PTR<ConstantDefinition>& c, def->Constants)
    {
        try
        {
            constant_type c1 = convert_constant(c, def->Constants);
            base_consts.push_back(c1);
        }
        catch (...)
        {
            ::mexWarnMsgTxt("Error converting constant");
        }
    }

    std::vector<boost::shared_ptr<ServiceEntryDefinition> > entries;
    boost::copy(def->NamedArrays, std::back_inserter(entries));
    boost::copy(def->Pods, std::back_inserter(entries));
    boost::copy(def->Structures, std::back_inserter(entries));
    boost::copy(def->Objects, std::back_inserter(entries));
    BOOST_FOREACH (boost::shared_ptr<ServiceEntryDefinition>& ee, entries)
    {
        obj_constant_type c2;
        c2.name = (ee)->Name;
        BOOST_FOREACH (std::string& e, (ee)->Options)
        {

            if (boost::starts_with(e, "constant"))
            {
                try
                {
                    RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(ee);
                    c->FromString(e);

                    std::vector<RR_SHARED_PTR<ConstantDefinition> > c3;
                    constant_type c1 = convert_constant(c, c3);

                    c2.data.push_back(c1);
                }
                catch (...)
                {
                    ::mexWarnMsgTxt("Error converting constant");
                }
            }
        }

        BOOST_FOREACH (const RR_SHARED_PTR<ConstantDefinition>& c, (ee)->Constants)
        {
            try
            {
                constant_type c1 = convert_constant(c, (ee)->Constants);
                c2.data.push_back(c1);
            }
            catch (...)
            {
                ::mexWarnMsgTxt("Error converting constant");
            }
        }

        if (!c2.data.empty())
        {
            obj_consts.push_back(c2);
        }
    }

    boost::shared_array<const char*> fieldnames(new const char*[base_consts.size() + obj_consts.size()]);
    size_t pos = 0;
    BOOST_FOREACH (constant_type& e, base_consts)
    {
        fieldnames.get()[pos] = e.name.c_str();
        pos++;
    }

    BOOST_FOREACH (obj_constant_type& e, obj_consts)
    {
        fieldnames.get()[pos] = e.name.c_str();
        pos++;
    }

    mxArray* o = ::mxCreateStructMatrix(1, 1, (int)pos, fieldnames.get());

    size_t pos2 = 0;
    BOOST_FOREACH (constant_type& e, base_consts)
    {
        mxSetFieldByNumber(o, 0, (int)pos2, e.data);
        pos2++;
    }

    BOOST_FOREACH (obj_constant_type& e, obj_consts)
    {
        boost::shared_array<const char*> fnames2(new const char*[e.data.size()]);
        size_t pos3 = 0;
        BOOST_FOREACH (constant_type& ee, e.data)
        {
            fnames2.get()[pos3] = ee.name.c_str();
            pos3++;
        }

        size_t pos4 = 0;
        mxArray* f = mxCreateStructMatrix(1, 1, (int)e.data.size(), fnames2.get());

        BOOST_FOREACH (constant_type& ee, e.data)
        {
            mxSetFieldByNumber(f, 0, (int)pos4, ee.data);
            pos4++;
        }

        mxSetFieldByNumber(o, 0, (int)pos2, f);
        pos2++;
    }

    return o;
}

void MexServiceSkel::Init(boost::string_ref s, const RR_SHARED_PTR<RRObject>& o, const RR_SHARED_PTR<ServerContext>& c)
{
    {
        boost::mutex::scoped_lock lock(skels_lock);
        skelscount++;
        skelid = skelscount;
        boost::weak_ptr<MexServiceSkel> skelsp = boost::dynamic_pointer_cast<MexServiceSkel>(shared_from_this());
        skels.insert(std::make_pair(skelid, skelsp));
        obj = boost::dynamic_pointer_cast<MexRRObject>(o);
    }

    ServiceSkel::Init(s, o, c);
}

RR_INTRUSIVE_PTR<MessageEntry> MexServiceSkel::CallGetProperty(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{
    boost::mutex::scoped_lock lock(request_lock);

    bool found = false;
    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, obj->type->Members)
    {
        if ((e)->Name == m->MemberName.str() && boost::dynamic_pointer_cast<PropertyDefinition>(e) != 0)
        {
            found = true;
            break;
        }
    }

    if (!found)
        throw MemberNotFoundException("Property " + m->MemberName.str() + " not found");

    property_get_requests.push_back(boost::make_tuple(m, ServerEndpoint::GetCurrentEndpoint()));
    skels_waiting.Set();
    return RR_INTRUSIVE_PTR<MessageEntry>();
}

RR_INTRUSIVE_PTR<MessageEntry> MexServiceSkel::CallSetProperty(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{
    boost::mutex::scoped_lock lock(request_lock);

    bool found = false;
    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, obj->type->Members)
    {
        if ((e)->Name == m->MemberName.str() && boost::dynamic_pointer_cast<PropertyDefinition>(e) != 0)
        {
            found = true;
            break;
        }
    }

    if (!found)
        throw MemberNotFoundException("Property " + m->MemberName.str() + " not found");

    property_set_requests.push_back(boost::make_tuple(m, ServerEndpoint::GetCurrentEndpoint()));
    skels_waiting.Set();
    return RR_INTRUSIVE_PTR<MessageEntry>();
}

RR_INTRUSIVE_PTR<MessageEntry> MexServiceSkel::CallFunction(const RR_INTRUSIVE_PTR<MessageEntry>& m)
{
    boost::mutex::scoped_lock lock(request_lock);

    bool found = false;
    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, obj->type->Members)
    {
        if ((e)->Name == m->MemberName.str() && boost::dynamic_pointer_cast<FunctionDefinition>(e) != 0)
        {
            found = true;
            break;
        }
    }

    if (!found)
        throw MemberNotFoundException("Function " + m->MemberName.str() + " not found");

    function_requests.push_back(boost::make_tuple(m, ServerEndpoint::GetCurrentEndpoint()));
    skels_waiting.Set();
    return RR_INTRUSIVE_PTR<MessageEntry>();
}

void MexServiceSkel::RegisterEvents(const RR_SHARED_PTR<RRObject>& obj1)
{
    RR_UNUSED(obj1);

    BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, obj->type->Members)
    {
        boost::shared_ptr<EventDefinition> def = boost::dynamic_pointer_cast<EventDefinition>(e);
        if (def)
        {
            std::vector<mxArray*> rhs;
            rhs.push_back(obj->mxobj.get());
            mxArray* mxskelid = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
            ((int32_t*)mxGetData(mxskelid))[0] = skelid;
            rhs.push_back(mxCreateString(def->Name.c_str()));
            rhs.push_back(mxskelid);

            ::mexCallMATLAB(0, NULL, (int)rhs.size(), &rhs[0], "RobotRaconteurObjectServerConnectEvent");
        }
    }
}

void MexServiceSkel::DispatchMexEvent(const std::string& name, const mxArray* parameters)
{
    try
    {
        BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, obj->type->Members)
        {
            if ((e)->Name == name)
            {
                boost::shared_ptr<EventDefinition> def = boost::dynamic_pointer_cast<EventDefinition>(e);
                if (def)
                {
                    if (mxGetNumberOfElements(parameters) != def->Parameters.size())
                    {
                        ::mexWarnMsgTxt(("Could not dispatch event " + name + " due to parameter mismatch").c_str());
                        return;
                    }

                    RR_INTRUSIVE_PTR<MessageEntry> mm =
                        CreateMessageEntry(RobotRaconteur::MessageEntryType_EventReq, name);

                    for (size_t i = 0; i < def->Parameters.size(); i++)
                    {
                        try
                        {
                            RR_INTRUSIVE_PTR<MessageElement> el = PackMxArrayToMessageElement(
                                mxGetCell(parameters, i), def->Parameters.at(i), RR_SHARED_PTR<ServiceStub>());
                            mm->AddElement(el);
                        }
                        catch (std::exception&)
                        {
                            ::mexWarnMsgTxt(
                                ("Could not dispatch event " + name + " due to parameter mismatch").c_str());
                            return;
                        }
                    }

                    SendEvent(mm);
                    return;
                }
            }
        }
    }
    catch (std::exception&)
    {
        mexWarnMsgTxt("Could not dispatch event");
        return;
    }
    mexWarnMsgTxt("Invalid event member for object");
}

void MexServiceSkel::ProcessRequests()
{
    while (true)
    {

        boost::tuple<RR_INTRUSIVE_PTR<MessageEntry>, boost::shared_ptr<ServerEndpoint> > pg;
        boost::tuple<RR_INTRUSIVE_PTR<MessageEntry>, boost::shared_ptr<ServerEndpoint> > ps;
        boost::tuple<RR_INTRUSIVE_PTR<MessageEntry>, boost::shared_ptr<ServerEndpoint> > f;

        {
            boost::mutex::scoped_lock lock(request_lock);
            if (!property_get_requests.empty())
            {
                pg = property_get_requests.front();
                property_get_requests.pop_front();
            }

            if (!property_set_requests.empty())
            {
                ps = property_set_requests.front();
                property_set_requests.pop_front();
            }

            if (!function_requests.empty())
            {
                f = function_requests.front();
                function_requests.pop_front();
            }
        }

        if (!pg.get<0>() && !ps.get<0>() && !f.get<0>())
            return;

        if (pg.get<0>())
        {

            try
            {
                RR_INTRUSIVE_PTR<MessageElement> ret;
                boost::shared_ptr<PropertyDefinition> def;

                BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, obj->type->Members)
                {
                    if (pg.get<0>()->MemberName.str() == (e)->Name)
                    {
                        def = boost::dynamic_pointer_cast<PropertyDefinition>(e);
                        break;
                    }
                }

                if (!def)
                    throw MemberNotFoundException("Member not found: " + pg.get<0>()->MemberName.str());

                const char* S_fieldnames[] = {"type", "subs"};

                mxArray* S = mxCreateStructMatrix(1, 1, 2, S_fieldnames);
                mxSetFieldByNumber(S, 0, 0, mxCreateString("."));
                mxSetFieldByNumber(S, 0, 1, mxCreateString(def->Name.c_str()));

                std::vector<mxArray*> args2;
                args2.push_back(obj->mxobj.get());
                args2.push_back(S);
                // args2.push_back(val);

                mxArray* ret2 = NULL;
                mxArray* mxret = NULL;
                if ((ret2 = ::mexCallMATLABWithTrap(1, &mxret, (int)args2.size(), &args2[0], "subsref")))
                {
                    // mexWarnMsgTxt(std::string("Warning: error dispatching event: error occured in MATLAB
                    // callback").c_str()); throw InvalidArgumentException("Error occured in MATLAB callback");
                    std::string errmsg = mxToString(mxGetProperty(ret2, 0, "message"));
                    ::mexWarnMsgTxt(errmsg.c_str());
                    throw OperationFailedException(errmsg);
                }

                // mxArray* mxret=mxGetProperty(obj->mxobj.get(),0,pg.get<0>()->MemberName.c_str());
                ret = PackMxArrayToMessageElement(mxret, def->Type, RR_SHARED_PTR<ServiceStub>());

                EndAsyncCallGetProperty(shared_from_this(), ret, boost::shared_ptr<RobotRaconteurException>(),
                                        pg.get<0>(), pg.get<1>());
            }
            catch (RobotRaconteurException& e)
            {
                boost::shared_ptr<RobotRaconteurException> sp_e = boost::make_shared<RobotRaconteurException>(e);
                EndAsyncCallGetProperty(shared_from_this(), RR_INTRUSIVE_PTR<MessageElement>(), sp_e, pg.get<0>(),
                                        pg.get<1>());
            }
            catch (std::exception& e)
            {
                boost::shared_ptr<RobotRaconteurException> sp_e = boost::make_shared<RobotRaconteurRemoteException>(
                    std::string(typeid(e).name()), std::string(e.what()));
                EndAsyncCallGetProperty(shared_from_this(), RR_INTRUSIVE_PTR<MessageElement>(), sp_e, pg.get<0>(),
                                        pg.get<1>());
            }
        }

        if (ps.get<0>())
        {

            try
            {
                RR_INTRUSIVE_PTR<MessageElement> ret;
                boost::shared_ptr<PropertyDefinition> def;

                BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& e, obj->type->Members)
                {
                    if (ps.get<0>()->MemberName == (e)->Name)
                    {
                        def = boost::dynamic_pointer_cast<PropertyDefinition>(e);
                        break;
                    }
                }

                if (!def)
                    throw MemberNotFoundException("Member not found: " + ps.get<0>()->MemberName.str());

                mxArray* val = UnpackMessageElementToMxArray(ps.get<0>()->FindElement("value"), def->Type,
                                                             RR_SHARED_PTR<ServiceStub>());

                const char* S_fieldnames[] = {"type", "subs"};

                mxArray* S = mxCreateStructMatrix(1, 1, 2, S_fieldnames);
                mxSetFieldByNumber(S, 0, 0, mxCreateString("."));
                mxSetFieldByNumber(S, 0, 1, mxCreateString(def->Name.c_str()));

                std::vector<mxArray*> args2;
                args2.push_back(obj->mxobj.get());
                args2.push_back(S);
                args2.push_back(val);

                mxArray* ret2 = NULL;

                if ((ret2 = ::mexCallMATLABWithTrap(0, NULL, (int)args2.size(), &args2[0], "subsasgn")))
                {
                    // mexWarnMsgTxt(std::string("Warning: error dispatching event: error occured in MATLAB
                    // callback").c_str()); throw OperationFailedException("Error occured in MATLAB callback");
                    std::string errmsg = mxToString(mxGetProperty(ret2, 0, "message"));
                    ::mexWarnMsgTxt(errmsg.c_str());
                    throw OperationFailedException(errmsg);
                }

                // mxSetProperty(obj->mxobj.get(),0,ps.get<0>()->MemberName.c_str(),val);

                EndAsyncCallSetProperty(shared_from_this(), boost::shared_ptr<RobotRaconteurException>(), ps.get<0>(),
                                        ps.get<1>());
            }
            catch (RobotRaconteurException& e)
            {
                boost::shared_ptr<RobotRaconteurException> sp_e = boost::make_shared<RobotRaconteurException>(e);
                EndAsyncCallSetProperty(shared_from_this(), sp_e, ps.get<0>(), ps.get<1>());
            }
            catch (std::exception& e)
            {
                boost::shared_ptr<RobotRaconteurException> sp_e = boost::make_shared<RobotRaconteurRemoteException>(
                    std::string(typeid(e).name()), std::string(e.what()));
                EndAsyncCallSetProperty(shared_from_this(), sp_e, ps.get<0>(), ps.get<1>());
            }
        }

        if (f.get<0>())
        {

            try
            {
                RR_INTRUSIVE_PTR<MessageElement> ret;
                boost::shared_ptr<FunctionDefinition> calldef;
                BOOST_FOREACH (boost::shared_ptr<MemberDefinition>& ee, obj->type->Members)
                {
                    boost::shared_ptr<FunctionDefinition> calldef1 =
                        boost::dynamic_pointer_cast<FunctionDefinition>(ee);
                    if (calldef1)
                    {
                        if (calldef1->Name == f.get<0>()->MemberName.str())
                        {
                            calldef = calldef1;
                            break;
                        }
                    }
                }
                if (!calldef)
                    throw MemberNotFoundException("Invalid member name");

                std::vector<mxArray*> args;
                BOOST_FOREACH (boost::shared_ptr<TypeDefinition>& ee, calldef->Parameters)
                {
                    RR_INTRUSIVE_PTR<MessageElement> arg = f.get<0>()->FindElement((ee)->Name);
                    args.push_back(UnpackMessageElementToMxArray(arg, ee, RR_SHARED_PTR<ServiceStub>()));
                }

                std::vector<mxArray*> args2 = args;
                args2.insert(args2.begin(), obj->mxobj.get());
                std::string member_name = f.get<0>()->MemberName.str().to_string();
                args2.insert(args2.begin(), mxCreateString(member_name.c_str()));

                if (calldef->ReturnType->Type == DataTypes_void_t)
                {
                    mxArray* ret2 = NULL;
                    if ((ret2 = ::mexCallMATLABWithTrap(0, NULL, (int)args2.size(), &args2[0], "feval")))
                    {
                        // mexWarnMsgTxt(std::string("Warning: error dispatching event: error occured in MATLAB
                        // callback").c_str()); throw OperationFailedException("Error occured in MATLAB callback");
                        std::string errmsg = mxToString(mxGetProperty(ret2, 0, "message"));
                        ::mexWarnMsgTxt(errmsg.c_str());
                        throw OperationFailedException(errmsg);
                    }
                }
                else
                {
                    mxArray* lhs[1];
                    mxArray* ret2 = NULL;
                    if ((ret2 = ::mexCallMATLABWithTrap(1, lhs, (int)args2.size(), &args2[0], "feval")))
                    {
                        // mexWarnMsgTxt(std::string("Warning: error dispatching event: error occured in MATLAB
                        // callback").c_str());
                        std::string errmsg = mxToString(mxGetProperty(ret2, 0, "message"));
                        ::mexWarnMsgTxt(errmsg.c_str());
                        throw OperationFailedException(errmsg);
                    }

                    ret = PackMxArrayToMessageElement(lhs[0], calldef->ReturnType, RR_SHARED_PTR<ServiceStub>());
                }

                EndAsyncCallFunction(shared_from_this(), ret, boost::shared_ptr<RobotRaconteurException>(), f.get<0>(),
                                     f.get<1>());
            }
            catch (RobotRaconteurException& e)
            {
                boost::shared_ptr<RobotRaconteurException> sp_e = boost::make_shared<RobotRaconteurException>(e);
                EndAsyncCallFunction(shared_from_this(), RR_INTRUSIVE_PTR<MessageElement>(), sp_e, f.get<0>(),
                                     f.get<1>());
            }
            catch (std::exception& e)
            {
                boost::shared_ptr<RobotRaconteurException> sp_e = boost::make_shared<RobotRaconteurRemoteException>(
                    std::string(typeid(e).name()), std::string(e.what()));
                EndAsyncCallFunction(shared_from_this(), RR_INTRUSIVE_PTR<MessageElement>(), sp_e, f.get<0>(),
                                     f.get<1>());
            }
        }
    }
}

// MexServiceInfo2Subscription

MexServiceInfo2Subscription::MexServiceInfo2Subscription(
    const boost::shared_ptr<ServiceInfo2Subscription>& subscription)
{
    this->subscription = subscription;
    servicesubscriptionid = 0;
}

mxArray* MexServiceInfo2Subscription::subsref(const mxArray* S)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);

    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string type = mxToString(mxGetField(S, 0, "type"));
    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    if (c1 != 2)
        throw InvalidArgumentException("ServiceSubscription expects a function request");

    std::string type2 = mxToString(mxGetField(S, 1, "type"));
    if (type2 != "()")
        throw InvalidArgumentException("ServiceSubscription expects a function request");

    if (!mxIsChar(mxGetField(S, 0, "subs")))
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    mxArray* cell_args = mxGetField(S, 1, "subs");

    if (membername == "GetDetectedServiceInfo2")
    {
        return GetDetectedServiceInfo2();
    }

    if (membername == "Close")
    {
        if (mxGetNumberOfElements(cell_args) != 0)
            throw InvalidArgumentException("Close expects zero arguments");
        Close();
        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
    }

    throw InvalidArgumentException("Unknown function for ServiceSubscription");
}

void MexServiceInfo2Subscription::subsasgn(const mxArray* S, const mxArray* value)
{
    RR_UNUSED(S);
    RR_UNUSED(value);
    throw InvalidArgumentException("Unknown function");
}

mxArray* MexServiceInfo2Subscription::GetDetectedServiceInfo2()
{
    std::map<ServiceSubscriptionClientID, ServiceInfo2> infos = subscription->GetDetectedServiceInfo2();

    mwIndex count = 0;

    mxArray* o = NULL;
    mxArray* prhs[4];
    prhs[0] = mxCreateString("KeyType");
    prhs[1] = mxCreateString("char");
    prhs[2] = mxCreateString("ValueType");
    prhs[3] = mxCreateString("any");

    mexCallMATLAB(1, &o, 4, prhs, "containers.Map");

    typedef std::map<ServiceSubscriptionClientID, ServiceInfo2>::value_type e_type;

    BOOST_FOREACH (e_type e, infos)
    {
        const char* fnames[] = {"type", "subs"};
        mxArray* subs = mxCreateStructMatrix(1, 1, 2, fnames);
        mxSetFieldByNumber(subs, 0, 0, mxCreateString("()"));

        std::string k = e.first.NodeID.ToString("D") + "," + e.first.ServiceName;
        mxArray* mxkey = mxCreateString(k.c_str());
        mxSetFieldByNumber(subs, 0, 1, mxkey);

        mxArray* mxInfo = ServiceInfo2ToMxArray(e.second);

        mxArray* prhs2[3];
        prhs2[0] = o;
        prhs2[1] = subs;
        prhs2[2] = mxInfo;

        mexCallMATLAB(0, NULL, 3, prhs2, "subsasgn");
    }

    return o;
}

void MexServiceInfo2Subscription::Close()
{
    {
        boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
        if (servicesubscriptionid != 0)
        {
            serviceinfo2subscriptions.erase(servicesubscriptionid);
        }
    }

    subscription->Close();
}

// MexServiceSubscription

MexServiceSubscription::MexServiceSubscription() { servicesubscriptionid = 0; }

void MexServiceSubscription::Init(const boost::shared_ptr<ServiceSubscription>& subscription)
{
    RR_WEAK_PTR<MexServiceSubscription> weak_this = shared_from_this();
    this->subscription = subscription;
    subscription->AddClientConnectFailedListener(boost::bind(&MexServiceSubscription::ClientConnectFailed, weak_this,
                                                             RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2),
                                                             RR_BOOST_PLACEHOLDERS(_3), RR_BOOST_PLACEHOLDERS(_4)));
}

void MexServiceSubscription::ClientConnectFailed(RR_WEAK_PTR<MexServiceSubscription> this_,
                                                 const boost::shared_ptr<ServiceSubscription>& subscription,
                                                 const ServiceSubscriptionClientID& id,
                                                 const std::vector<std::string>& url,
                                                 const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    RR_SHARED_PTR<MexServiceSubscription> this1 = this_.lock();
    if (!this1)
        return;
    this1->ClientConnectFailed1(subscription, id, url, err);
}

void MexServiceSubscription::ClientConnectFailed1(const boost::shared_ptr<ServiceSubscription>& subscription,
                                                  const ServiceSubscriptionClientID& id,
                                                  const std::vector<std::string>& url,
                                                  const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    RR_UNUSED(subscription);

    boost::mutex::scoped_lock lock(this_lock);
    std::string msg = "Client connect failed: " + id.NodeID.ToString() + " url: " + boost::join(url, ",") +
                      " error: " + err->ToString();
    connect_failures.push_back(msg);
    while (connect_failures.size() > 50)
    {
        connect_failures.pop_back();
    }
}

mxArray* MexServiceSubscription::subsref(const mxArray* S)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);

    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string type = mxToString(mxGetField(S, 0, "type"));
    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    if (c1 != 2)
        throw InvalidArgumentException("ServiceSubscription expects a function request");

    std::string type2 = mxToString(mxGetField(S, 1, "type"));
    if (type2 != "()")
        throw InvalidArgumentException("ServiceSubscription expects a function request");

    if (!mxIsChar(mxGetField(S, 0, "subs")))
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    mxArray* cell_args = mxGetField(S, 1, "subs");

    if (membername == "GetConnectedClients")
    {
        return GetConnectedClients();
    }

    if (membername == "Close")
    {
        if (mxGetNumberOfElements(cell_args) != 0)
            throw InvalidArgumentException("Close expects zero arguments");
        Close();
        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
    }

    if (membername == "SubscribeWire")
    {
        size_t n_args = mxGetNumberOfElements(cell_args);
        if (n_args != 1 && n_args != 2)
            throw InvalidArgumentException("SubscribeWire expects one argument");

        std::string wire_name = mxToString(mxGetCell(cell_args, 0));
        std::string service_path;
        if (n_args == 2)
        {
            service_path = mxToString(mxGetCell(cell_args, 1));
        }

        boost::shared_ptr<WireSubscription<RR_INTRUSIVE_PTR<MessageElement> > > sub1 =
            subscription->SubscribeWire<RR_INTRUSIVE_PTR<MessageElement> >(wire_name, service_path);
        boost::shared_ptr<MexWireSubscription> sub2 = boost::make_shared<MexWireSubscription>(shared_from_this(), sub1);

        boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
        wiresubscriptions_count++;
        sub2->wiresubscriptionid = wiresubscriptions_count;
        wiresubscriptions.insert(std::make_pair(sub2->wiresubscriptionid, sub2));

        mxArray* matlabret[1];
        mxArray* rhs[2];

        rhs[0] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
        rhs[1] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);

        ((int32_t*)mxGetData(rhs[0]))[0] = RR_MEX_WIRE_SUBSCRIPTION;
        ((int32_t*)mxGetData(rhs[1]))[0] = sub2->wiresubscriptionid;

        int merror = mexCallMATLAB(1, matlabret, 2, rhs, "RobotRaconteurWireSubscription");
        if (merror)
        {
            throw InternalErrorException("Internal error");
        }

        return matlabret[0];
    }

    if (membername == "SubscribePipe")
    {
        size_t n_args = mxGetNumberOfElements(cell_args);
        if (n_args != 1 && n_args != 2)
            throw InvalidArgumentException("SubscribePipe expects one argument");

        std::string pipe_name = mxToString(mxGetCell(cell_args, 0));
        std::string service_path;
        if (n_args == 2)
        {
            service_path = mxToString(mxGetCell(cell_args, 1));
        }

        boost::shared_ptr<PipeSubscription<RR_INTRUSIVE_PTR<MessageElement> > > sub1 =
            subscription->SubscribePipe<RR_INTRUSIVE_PTR<MessageElement> >(pipe_name, service_path);
        boost::shared_ptr<MexPipeSubscription> sub2 = boost::make_shared<MexPipeSubscription>(shared_from_this(), sub1);

        boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
        pipesubscriptions_count++;
        sub2->pipesubscriptionid = pipesubscriptions_count;
        pipesubscriptions.insert(std::make_pair(sub2->pipesubscriptionid, sub2));

        mxArray* matlabret[1];
        mxArray* rhs[2];

        rhs[0] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
        rhs[1] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);

        ((int32_t*)mxGetData(rhs[0]))[0] = RR_MEX_PIPE_SUBSCRIPTION;
        ((int32_t*)mxGetData(rhs[1]))[0] = sub2->pipesubscriptionid;

        int merror = mexCallMATLAB(1, matlabret, 2, rhs, "RobotRaconteurPipeSubscription");
        if (merror)
        {
            throw InternalErrorException("Internal error");
        }

        return matlabret[0];
    }

    if (membername == "ClaimClient")
    {
        if (mxGetNumberOfElements(cell_args) != 1)
            throw InvalidArgumentException("ClaimClient expects one argument");

        mxArray* rrstubid_mx = mxGetField(mxGetCell(cell_args, 0), 0, "rrstubid");

        int32_t rrstubid = ::GetInt32Scalar(rrstubid_mx);

        RR_SHARED_PTR<MexServiceStub> stub;

        {
            boost::recursive_mutex::scoped_lock lock(stubs_lock);
            std::map<int32_t, boost::shared_ptr<MexServiceStub> >::iterator e = stubs.find(rrstubid);
            if (e == stubs.end())
                throw InvalidArgumentException("Invalid client connection");
            stub = e->second;
        }

        subscription->ClaimClient(stub);

        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
    }

    if (membername == "ReleaseClient")
    {
        if (mxGetNumberOfElements(cell_args) != 1)
            throw InvalidArgumentException("ClaimClient expects one argument");

        mxArray* rrstubid_mx = mxGetField(mxGetCell(cell_args, 0), 0, "rrstubid");

        int32_t rrstubid = ::GetInt32Scalar(rrstubid_mx);

        RR_SHARED_PTR<MexServiceStub> stub;

        {
            boost::recursive_mutex::scoped_lock lock(stubs_lock);
            std::map<int32_t, boost::shared_ptr<MexServiceStub> >::iterator e = stubs.find(rrstubid);
            if (e == stubs.end())
                throw InvalidArgumentException("Invalid client connection");
            stub = e->second;
        }

        subscription->ReleaseClient(stub);

        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
    }

    if (membername == "GetDefaultClient")
    {
        if (mxGetNumberOfElements(cell_args) != 0)
            throw InvalidArgumentException("GetDefaultClient expects zero arguments");

        RR_SHARED_PTR<MexServiceStub> stub = subscription->GetDefaultClient<MexServiceStub>();

        mxArray* mxStub = NULL;

        if (stub->stubptr != NULL)
        {
            mxStub = stub->stubptr.get();
        }
        else
        {
            boost::recursive_mutex::scoped_lock lock(stubs_lock);
            stubcount++;
            int stubid = stubcount;
            stub->stubid = stubid;
            mxArray* mxstub1 = MatlabObjectFromMexStub(stub);
            stubs.insert(std::make_pair(stubid, stub));
            mxStub = mxDuplicateArray(mxstub1);
        }

        return mxStub;
    }

    if (membername == "GetDefaultClientWait")
    {
        int32_t timeout = 0;
        if (mxGetNumberOfElements(cell_args) == 0)
        {
            timeout = RR_TIMEOUT_INFINITE;
        }
        else if (mxGetNumberOfElements(cell_args) == 1)
        {
            timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 0));
        }
        else
        {
            throw InvalidArgumentException("GetDefaultClientWait expects zero or one arguments");
        }

        RR_SHARED_PTR<MexServiceStub> stub = subscription->GetDefaultClientWait<MexServiceStub>(timeout);

        mxArray* mxStub = NULL;

        if (stub->stubptr != NULL)
        {
            mxStub = stub->stubptr.get();
        }
        else
        {
            boost::recursive_mutex::scoped_lock lock(stubs_lock);
            stubcount++;
            int stubid = stubcount;
            stub->stubid = stubid;
            mxArray* mxstub1 = MatlabObjectFromMexStub(stub);
            stubs.insert(std::make_pair(stubid, stub));
            mxStub = mxDuplicateArray(mxstub1);
        }

        return mxStub;
    }

    if (membername == "GetClientConnectFailures")
    {
        if (mxGetNumberOfElements(cell_args) != 0)
            throw InvalidArgumentException("GetConnectFailures expects zero arguments");

        mxArray* o = NULL;
        boost::mutex::scoped_lock lock(this_lock);
        o = mxCreateCellMatrix(connect_failures.size(), 1);
        for (size_t i = 0; i < connect_failures.size(); i++)
        {
            mxSetCell(o, i, mxCreateString(connect_failures.at(i).c_str()));
        }
        return o;
    }

    throw InvalidArgumentException("Unknown function for ServiceSubscription");
}

void MexServiceSubscription::subsasgn(const mxArray* S, const mxArray* value)
{
    RR_UNUSED(S);
    RR_UNUSED(value);
    throw InvalidArgumentException("Unknown function");
}

mxArray* MexServiceSubscription::GetConnectedClients()
{
    std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> > clients = subscription->GetConnectedClients();

    mwIndex count = 0;

    mxArray* o = NULL;
    mxArray* prhs[4];
    prhs[0] = mxCreateString("KeyType");
    prhs[1] = mxCreateString("char");
    prhs[2] = mxCreateString("ValueType");
    prhs[3] = mxCreateString("any");

    mexCallMATLAB(1, &o, 4, prhs, "containers.Map");

    typedef std::map<ServiceSubscriptionClientID, RR_SHARED_PTR<RRObject> >::value_type e_type;

    BOOST_FOREACH (e_type e, clients)
    {
        RR_SHARED_PTR<MexServiceStub> stub = RR_DYNAMIC_POINTER_CAST<MexServiceStub>(e.second);
        if (!stub)
            continue;

        const char* fnames[] = {"type", "subs"};
        mxArray* subs = mxCreateStructMatrix(1, 1, 2, fnames);
        mxSetFieldByNumber(subs, 0, 0, mxCreateString("()"));

        std::string k = e.first.NodeID.ToString("D") + "," + e.first.ServiceName;
        mxArray* mxkey = mxCreateString(k.c_str());
        mxSetFieldByNumber(subs, 0, 1, mxkey);

        mxArray* mxStub = NULL;

        if (stub->stubptr != NULL)
        {
            mxStub = stub->stubptr.get();
        }
        else
        {
            boost::recursive_mutex::scoped_lock lock(stubs_lock);
            stubcount++;
            int stubid = stubcount;
            stub->stubid = stubid;
            mxArray* mxstub1 = MatlabObjectFromMexStub(stub);
            stubs.insert(std::make_pair(stubid, stub));
            mxStub = mxDuplicateArray(mxstub1);
        }

        mxArray* prhs2[3];
        prhs2[0] = o;
        prhs2[1] = subs;
        prhs2[2] = mxStub;

        mexCallMATLAB(0, NULL, 3, prhs2, "subsasgn");
    }

    return o;
}

template <typename T>
static void MexServiceSubscription_Close_Clean(std::map<int32_t, boost::shared_ptr<T> >& s,
                                               const boost::shared_ptr<MexServiceSubscription>& service_subscription)
{
    for (typename std::map<int32_t, boost::shared_ptr<T> >::iterator e = s.begin(); e != s.end();)
    {
        boost::shared_ptr<MexServiceSubscription> e1 = e->second->service_subscription.lock();
        if (!e1)
        {
            s.erase(e++);
            continue;
        }

        if (e1 == service_subscription)
        {
            s.erase(e++);
            continue;
        }
        e++;
    }
}

void MexServiceSubscription::Close()
{
    {
        boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
        if (servicesubscriptionid != 0)
        {
            servicesubscriptions.erase(servicesubscriptionid);
        }

        MexServiceSubscription_Close_Clean(wiresubscriptions, shared_from_this());
        MexServiceSubscription_Close_Clean(pipesubscriptions, shared_from_this());
    }

    subscription->Close();
}

MexWireSubscription::MexWireSubscription(
    const boost::shared_ptr<MexServiceSubscription>& service_subscription,
    const boost::shared_ptr<WireSubscription<RR_INTRUSIVE_PTR<MessageElement> > >& subscription)
{
    this->subscription = subscription;
    this->service_subscription = service_subscription;
    wiresubscriptionid = 0;
}

mxArray* MexWireSubscription::subsref(const mxArray* S)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);

    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string type = mxToString(mxGetField(S, 0, "type"));
    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    if (c1 == 1)
    {
        if (membername == "InValue")
        {
            RR_INTRUSIVE_PTR<RRValue> m1;
            TimeSpec time;
            RR_SHARED_PTR<WireConnectionBase> c;
            if (!subscription->TryGetInValueBase(m1, &time, &c))
            {
                throw ValueNotSetException("InValue not valid");
            }
            if (!c)
                throw InvalidOperationException("Could not unpack wire value");
            RR_SHARED_PTR<MexWireConnection> c2 = rr_cast<MexWireConnection>(c);
            return UnpackMessageElementToMxArray(rr_cast<MessageElement>(m1), c2->Type,
                                                 RR_SHARED_PTR<ServiceStub>(c2->GetStub()));
        }
        if (membername == "InValueValid")
        {
            RR_INTRUSIVE_PTR<MessageElement> m;
            TimeSpec time;
            if (!subscription->TryGetInValue(m, &time))
            {
                return mxCreateLogicalScalar(false);
            }

            return mxCreateLogicalScalar(true);
        }

        if (membername == "IgnoreInValue")
        {
            return mxCreateLogicalScalar(subscription->GetIgnoreInValue());
        }

        if (membername == "InValueLifespan")
        {
            double d = subscription->GetInValueLifespan();
            if (d < 0.0)
            {
                return mxCreateDoubleScalar(d);
            }
            else
            {
                return mxCreateDoubleScalar(d / 1000.0);
            }
        }

        throw InvalidArgumentException("Unknown property for ServiceSubscription");
    }

    if (c1 != 2)
        throw InvalidArgumentException("WireSubscription expects a function request");

    std::string type2 = mxToString(mxGetField(S, 1, "type"));
    if (type2 != "()")
        throw InvalidArgumentException("WireSubscription expects a function request");

    if (!mxIsChar(mxGetField(S, 0, "subs")))
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    mxArray* cell_args = mxGetField(S, 1, "subs");

    if (membername == "Close")
    {
        if (mxGetNumberOfElements(cell_args) != 0)
            throw InvalidArgumentException("Close expects zero arguments");
        {
            boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
            wiresubscriptions.erase(wiresubscriptionid);
        }
        subscription->Close();
        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
    }

    if (membername == "WaitInValueValid")
    {
        int32_t timeout = 0;
        if (mxGetNumberOfElements(cell_args) == 0)
        {
            timeout = RR_TIMEOUT_INFINITE;
        }
        else if (mxGetNumberOfElements(cell_args) == 1)
        {
            timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 0));
        }
        else
        {
            throw InvalidArgumentException("WaitInValueValid expects zero or one arguments");
        }

        bool r = subscription->WaitInValueValid(timeout);
        return mxCreateLogicalScalar(r);
    }

    if (membername == "SetOutValueAll")
    {
        if (mxGetNumberOfElements(cell_args) != 1)
            throw InvalidArgumentException("SendPacket expects one argument");
        mxArray* packet = mxGetCell(cell_args, 0);

        RobotRaconteur::detail::WireSubscription_send_iterator iter(subscription);
        while (RR_SHARED_PTR<WireConnectionBase> p1 = iter.Next())
        {
            RR_SHARED_PTR<MexWireConnection> p2 = RR_DYNAMIC_POINTER_CAST<MexWireConnection>(p1);

            iter.SetOutValue(PackMxArrayToMessageElement(packet, p2->Type, p2->GetStub()));
        }
        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
    }

    throw InvalidArgumentException("Unknown function for WireSubscription");
}

void MexWireSubscription::subsasgn(const mxArray* S, const mxArray* value)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);

    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string type = mxToString(mxGetField(S, 0, "type"));
    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    if (c1 == 1)
    {
        if (membername == "IgnoreInValue")
        {
            if (mxIsLogicalScalarTrue(value))
            {
                subscription->SetIgnoreInValue(true);
            }
            else
            {
                subscription->SetIgnoreInValue(false);
            }
            return;
        }

        if (membername == "InValueLifespan")
        {
            double d = GetDoubleScalar(value);
            if (d < 0.0)
            {
                subscription->SetInValueLifespan(-1);
            }
            else
            {
                subscription->SetInValueLifespan(boost::numeric_cast<int32_t>(d * 1000.0));
            }
            return;
        }
    }

    throw InvalidArgumentException("Unknown function for WireSubscription");
}

MexPipeSubscription::MexPipeSubscription(
    const boost::shared_ptr<MexServiceSubscription>& service_subscription,
    const boost::shared_ptr<PipeSubscription<RR_INTRUSIVE_PTR<MessageElement> > >& subscription)
{
    this->subscription = subscription;
    this->service_subscription = service_subscription;
    pipesubscriptionid = 0;
}

mxArray* MexPipeSubscription::subsref(const mxArray* S)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);

    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string type = mxToString(mxGetField(S, 0, "type"));
    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    if (c1 == 1)
    {
        if (membername == "Available")
        {
            int32_t av = (int)subscription->Available();
            return CreateInt32Scalar(av);
        }
        if (membername == "IgnoreInValue")
        {
            return mxCreateLogicalScalar(subscription->GetIgnoreReceived());
        }

        throw InvalidArgumentException("Unknown property for ServiceSubscription");
    }

    if (c1 != 2)
        throw InvalidArgumentException("PipeSubscription expects a function request");

    std::string type2 = mxToString(mxGetField(S, 1, "type"));
    if (type2 != "()")
        throw InvalidArgumentException("PipeSubscription expects a function request");

    if (!mxIsChar(mxGetField(S, 0, "subs")))
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    mxArray* cell_args = mxGetField(S, 1, "subs");

    if (membername == "Close")
    {
        if (mxGetNumberOfElements(cell_args) != 0)
            throw InvalidArgumentException("Close expects zero arguments");
        {
            boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
            pipesubscriptions.erase(pipesubscriptionid);
        }
        subscription->Close();
        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
    }

    if (membername == "ReceivePacket")
    {
        if (mxGetNumberOfElements(cell_args) != 0)
            throw InvalidArgumentException("ReceivePacket expects zero arguments");
        RR_SHARED_PTR<PipeEndpointBase> ep;
        RR_INTRUSIVE_PTR<RRValue> p;
        if (!subscription->TryReceivePacketBaseWait(p, 0, false, &ep))
        {
            throw InvalidOperationException("PipeSubscription receive queue empty");
        }
        if (!ep)
            throw InvalidOperationException("Could not unpack wire value");
        RR_SHARED_PTR<MexPipeEndpoint> ep2 = rr_cast<MexPipeEndpoint>(ep);
        return UnpackMessageElementToMxArray(rr_cast<MessageElement>(p), ep2->Type, RR_SHARED_PTR<ServiceStub>());
    }

    if (membername == "ReceivePacketWait")
    {
        int32_t timeout = 0;
        if (mxGetNumberOfElements(cell_args) == 0)
        {
            timeout = RR_TIMEOUT_INFINITE;
        }
        else if (mxGetNumberOfElements(cell_args) == 1)
        {
            timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 0));
        }
        else
        {
            throw InvalidArgumentException("ReceivePacketWait expects zero or one arguments");
        }

        boost::shared_ptr<TypeDefinition> tc = boost::make_shared<TypeDefinition>();
        tc->Type = DataTypes_varvalue_t;
        RR_INTRUSIVE_PTR<MessageElement> m;
        RR_SHARED_PTR<PipeEndpoint<RR_INTRUSIVE_PTR<MessageElement> > > ep;
        if (!subscription->TryReceivePacketWait(m, timeout, false, &ep))
        {
            throw InvalidArgumentException("PipeSubscription receive timeout");
        }
        if (!ep)
            throw InvalidOperationException("Could not unpack wire value");
        RR_SHARED_PTR<MexPipeEndpoint> ep2 = rr_cast<MexPipeEndpoint>(ep);
        return UnpackMessageElementToMxArray(m, ep2->Type, RR_SHARED_PTR<ServiceStub>());
    }

    if (membername == "SendPacketAll")
    {
        if (mxGetNumberOfElements(cell_args) != 1)
            throw InvalidArgumentException("SendPacket expects one argument");
        mxArray* packet = mxGetCell(cell_args, 0);

        RobotRaconteur::detail::PipeSubscription_send_iterator iter(subscription);
        while (RR_SHARED_PTR<PipeEndpointBase> p1 = iter.Next())
        {
            RR_SHARED_PTR<MexPipeEndpoint> p2 = RR_STATIC_POINTER_CAST<MexPipeEndpoint>(p1);

            iter.AsyncSendPacket(PackMxArrayToMessageElement(packet, p2->Type, p2->GetStub()));
        }
        return mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
    }

    throw InvalidArgumentException("Unknown function for WireSubscription");
}
void MexPipeSubscription::subsasgn(const mxArray* S, const mxArray* value)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);

    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string type = mxToString(mxGetField(S, 0, "type"));
    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    if (c1 == 1)
    {
        if (membername == "IgnoreInValue")
        {
            if (mxIsLogicalScalarTrue(value))
            {
                subscription->SetIgnoreReceived(true);
            }
            else
            {
                subscription->SetIgnoreReceived(false);
            }
        }
    }

    throw InvalidArgumentException("Unknown function for PipeSubscription");
}

// Predicate would result in threading error. Disable until problem resolved.
/*bool MexServiceSubsrciptionPredicate(const boost::shared_ptr<mxArray>& mxFunc, const ServiceInfo2& info)
{
    try
    {
        mxArray* lhs[1];
        mxArray* rhs[2];

        rhs[0] = mxFunc.get();
        rhs[1] = ServiceInfo2ToMxArray(info);
        if (mexCallMATLABWithTrap(1, lhs, 1, rhs, "feval"))
        {
            return false;
        }

        if (mxIsLogical(lhs[0]))
        {
            return GetLogicalScalar(lhs[0]);
        }

        if (mxIsDouble(lhs[0]))
        {
            return GetDoubleScalar(lhs[0]) != 0.0;
        }

        mexWarnMsgTxt("RobotRaconteur: ServiceSubscriptionFilter Predicate Invalid Return Type");

        return false;
    }
    catch (std::exception& exp)
    {
        mexWarnMsgTxt("RobotRaconteur: ServiceSubscriptionFilter Predicate Error: " + exp.what());

        return false;
    }
}*/

static boost::shared_ptr<ServiceSubscriptionFilter> SubscribeService_LoadFilter(const mxArray* filter)
{
    boost::shared_ptr<ServiceSubscriptionFilter> filter2;
    if (filter)
    {
        if (!mxIsStruct(filter))
            throw InvalidArgumentException("Invalid filter specified for SubscribeServiceByType");

        filter2 = boost::make_shared<ServiceSubscriptionFilter>();

        if (mxArray* filter_servicenames1 = ::mxGetField(filter, 0, "ServiceNames"))
        {
            mxToVectorString(filter_servicenames1, filter2->ServiceNames,
                             "Invalid filter.ServiceNames specified for SubscribeServiceByType");
        }

        if (mxArray* filter_schemes1 = ::mxGetField(filter, 0, "TransportSchemes"))
        {
            mxToVectorString(filter_schemes1, filter2->TransportSchemes,
                             "Invalid filter.TransportSchemes specified for SubscribeServiceByType");
        }

        if (mxArray* filter_maxconnections1 = ::mxGetField(filter, 0, "MaxConnections"))
        {
            filter2->MaxConnections = boost::lexical_cast<uint32_t>(GetDoubleScalar(filter_maxconnections1));
        }

        if (mxArray* filter_predicate1 = ::mxGetField(filter, 0, "Predicate"))
        {
            /*boost::shared_ptr<mxArray> mx_predicate = boost::shared_ptr<mxArray>(mxDuplicateArray(filter_predicate1),
            ::mxDestroyArray); mexMakeArrayPersistent(mx_predicate.get()); filter2->Predicate =
            boost::bind(&MexServiceSubsrciptionPredicate, mx_predicate, RR_BOOST_PLACEHOLDERS(_1));*/
            throw InvalidArgumentException("ServiceSubscriptionFilter.Predicate not supported in MATLAB");
        }

        if (mxArray* filter_nodes1 = ::mxGetField(filter, 0, "Nodes"))
        {
            if (!mxIsCell(filter_nodes1))
            {
                throw InvalidArgumentException("Invalid filter.Nodes specified for SubscribeServiceByType");
            }

            for (size_t i = 0; i < mxGetNumberOfElements(filter_nodes1); i++)
            {
                boost::shared_ptr<ServiceSubscriptionFilterNode> filter_node2 =
                    boost::make_shared<ServiceSubscriptionFilterNode>();

                mxArray* filter_nodes1_1 = mxGetCell(filter_nodes1, i);
                if (!mxIsStruct(filter_nodes1_1))
                    throw InvalidArgumentException("Invalid filter.Nodes specified for SubscribeServiceByType");

                if (mxArray* filter_nodes1_nodeid = mxGetField(filter_nodes1_1, 0, "NodeID"))
                {
                    try
                    {
                        filter_node2->NodeID = NodeID(mxToString(filter_nodes1_nodeid));
                    }
                    catch (std::exception&)
                    {
                        throw InvalidArgumentException("Invalid filter.Nodes specified for SubscribeServiceByType");
                    }
                }

                if (mxArray* filter_nodes1_nodename = mxGetField(filter_nodes1_1, 0, "NodeName"))
                {
                    filter_node2->NodeName = mxToString(filter_nodes1_nodename);
                }

                if (mxArray* filter_nodes1_username = mxGetField(filter_nodes1_1, 0, "Username"))
                {
                    filter_node2->Username = mxToString(filter_nodes1_username);
                }

                if (mxArray* filter_nodes1_credentials = mxGetField(filter_nodes1_1, 0, "Credentials"))
                {
                    try
                    {
                        filter_node2->Credentials = mxArrayToRRMap(filter_nodes1_credentials);
                    }
                    catch (std::exception& exp)
                    {
                        throw InvalidArgumentException(
                            "Invalid credentials specified for SubscribeServiceByType.Nodes : " +
                            std::string(exp.what()));
                    }
                }

                filter2->Nodes.push_back(filter_node2);
            }
        }
    }

    return filter2;
}

mxArray* SubscribeServiceInfo2(const mxArray* service_types, const mxArray* filter)
{
    std::vector<std::string> service_types2;

    mxToVectorString(service_types, service_types2, "Invalid service_types specified for SubscribeServiceInfo2");
    // if (service_types2.empty()) throw InvalidArgumentException("Invalid service_types specified for
    // SubscribeServiceByType");

    boost::shared_ptr<ServiceSubscriptionFilter> filter2 = SubscribeService_LoadFilter(filter);

    boost::shared_ptr<ServiceInfo2Subscription> sub =
        RobotRaconteurNode::s()->SubscribeServiceInfo2(service_types2, filter2);

    boost::shared_ptr<MexServiceInfo2Subscription> sub2 = boost::make_shared<MexServiceInfo2Subscription>(sub);

    boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
    serviceinfo2subscriptions_count++;
    sub2->servicesubscriptionid = serviceinfo2subscriptions_count;
    serviceinfo2subscriptions.insert(std::make_pair(sub2->servicesubscriptionid, sub2));

    mxArray* matlabret[1];
    mxArray* rhs[2];

    rhs[0] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    rhs[1] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);

    ((int32_t*)mxGetData(rhs[0]))[0] = RR_MEX_SERVICEINFO2_SUBSCRIPTION;
    ((int32_t*)mxGetData(rhs[1]))[0] = sub2->servicesubscriptionid;

    int merror = mexCallMATLAB(1, matlabret, 2, rhs, "RobotRaconteurServiceInfo2Subscription");
    if (merror)
    {
        throw InternalErrorException("Internal error");
    }

    return matlabret[0];
}

mxArray* SubscribeServiceByType(const mxArray* service_types, const mxArray* filter)
{
    std::vector<std::string> service_types2;

    mxToVectorString(service_types, service_types2, "Invalid service_types specified for SubscribeServiceByType");
    // if (service_types2.empty()) throw InvalidArgumentException("Invalid service_types specified for
    // SubscribeServiceByType");

    boost::shared_ptr<ServiceSubscriptionFilter> filter2 = SubscribeService_LoadFilter(filter);

    boost::shared_ptr<ServiceSubscription> sub =
        RobotRaconteurNode::s()->SubscribeServiceByType(service_types2, filter2);

    boost::shared_ptr<MexServiceSubscription> sub2 = boost::make_shared<MexServiceSubscription>();
    sub2->Init(sub);

    boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
    servicesubscriptions_count++;
    sub2->servicesubscriptionid = servicesubscriptions_count;
    servicesubscriptions.insert(std::make_pair(sub2->servicesubscriptionid, sub2));

    mxArray* matlabret[1];
    mxArray* rhs[2];

    rhs[0] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    rhs[1] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);

    ((int32_t*)mxGetData(rhs[0]))[0] = RR_MEX_SERVICE_SUBSCRIPTION;
    ((int32_t*)mxGetData(rhs[1]))[0] = sub2->servicesubscriptionid;

    int merror = mexCallMATLAB(1, matlabret, 2, rhs, "RobotRaconteurServiceSubscription");
    if (merror)
    {
        throw InternalErrorException("Internal error");
    }

    return matlabret[0];
}

mxArray* SubscribeService(const mxArray* url, const mxArray* username, const mxArray* credentials)
{
    std::string susername;
    RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> > mcredentials;

    if (username != NULL && credentials != NULL)
    {
        susername = mxToString(username);

        mcredentials = mxArrayToRRMap(credentials);
    }

    std::vector<std::string> urls;

    if (mxIsCell(url))
    {
        size_t urlc = mxGetNumberOfElements(url);
        for (size_t i = 0; i < urlc; i++)
        {
            mxArray* urln_a = mxGetCell(url, i);
            urls.push_back(mxToString(urln_a));
        }
    }
    else
    {
        std::string surl1 = mxToString(url);

        boost::split(urls, surl1, boost::is_from_range(',', ','));
    }

    boost::shared_ptr<ServiceSubscription> sub =
        RobotRaconteurNode::s()->SubscribeService(urls, susername, mcredentials);

    boost::shared_ptr<MexServiceSubscription> sub2 = boost::make_shared<MexServiceSubscription>();
    sub2->Init(sub);

    boost::recursive_mutex::scoped_lock lock(servicesubscriptions_lock);
    servicesubscriptions_count++;
    sub2->servicesubscriptionid = servicesubscriptions_count;
    servicesubscriptions.insert(std::make_pair(sub2->servicesubscriptionid, sub2));

    mxArray* matlabret[1];
    mxArray* rhs[2];

    rhs[0] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    rhs[1] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);

    ((int32_t*)mxGetData(rhs[0]))[0] = RR_MEX_SERVICE_SUBSCRIPTION;
    ((int32_t*)mxGetData(rhs[1]))[0] = sub2->servicesubscriptionid;

    int merror = mexCallMATLAB(1, matlabret, 2, rhs, "RobotRaconteurServiceSubscription");
    if (merror)
    {
        throw InternalErrorException("Internal error");
    }

    return matlabret[0];
}

MexGeneratorClient::MexGeneratorClient(const std::string& name, int32_t id, const RR_SHARED_PTR<ServiceStub>& stub,
                                       const RR_SHARED_PTR<TypeDefinition>& return_type,
                                       const RR_SHARED_PTR<TypeDefinition>& param_type)
    : GeneratorClientBase(name, id, stub)
{
    this->return_type = return_type;
    this->param_type = param_type;
    generatorid = 0;
}

std::string MexGeneratorClient::GetFunctionName(const mxArray* S)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);

    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string type = mxToString(mxGetField(S, 0, "type"));
    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    return membername;
}

mxArray* MexGeneratorClient::subsref(const mxArray* S)
{
    if (!mxIsStruct(S))
        throw InvalidArgumentException("RobotRaconteurMex error");

    int c1 = (int)mxGetNumberOfElements(S);

    if (c1 == 0)
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string type = mxToString(mxGetField(S, 0, "type"));
    if (type != ".")
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    std::string membername = mxToString(mxGetField(S, 0, "subs"));

    if (c1 != 2)
        throw InvalidArgumentException("GeneratorClient expects a function request");

    std::string type2 = mxToString(mxGetField(S, 1, "type"));
    if (type2 != "()")
        throw InvalidArgumentException("GeneratorClient expects a function request");

    if (!mxIsChar(mxGetField(S, 0, "subs")))
        throw InvalidArgumentException("RobotRaconteur object expects 'dot' notation");

    mxArray* cell_args = mxGetField(S, 1, "subs");

    if (membername == "Next")
    {
        RR_INTRUSIVE_PTR<MessageElement> param;
        if (mxGetNumberOfElements(cell_args) == 0)
        {
            if (param_type)
                throw "GeneratorClient Next expects a parameter";
        }
        else if (mxGetNumberOfElements(cell_args) == 1)
        {
            if (!param_type)
                throw "GeneratorClient Next does not expect a parameter";
            param = PackMxArrayToMessageElement(mxGetCell(cell_args, 0), param_type, GetStub());
        }
        else
        {
            throw InvalidArgumentException("GeneratorClient Next expects zero or one arguments");
        }
        try
        {
            RR_INTRUSIVE_PTR<MessageElement> res = NextBase(param);
            if (this->return_type)
            {
                return UnpackMessageElementToMxArray(res, return_type, GetStub());
            }
            else
            {
                return mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
            }
        }
        catch (std::exception&)
        {
            boost::mutex::scoped_lock lock(generators_lock);
            generators.erase(generatorid);
            throw;
        }
    }

    if (membername == "TryNext")
    {
        RR_INTRUSIVE_PTR<MessageElement> param;
        if (mxGetNumberOfElements(cell_args) == 0)
        {
            if (param_type)
                throw "GeneratorClient TryNext expects a parameter";
        }
        else if (mxGetNumberOfElements(cell_args) == 1)
        {
            if (!param_type)
                throw "GeneratorClient TryNext does not expect a parameter";
            param = PackMxArrayToMessageElement(mxGetCell(cell_args, 0), param_type, GetStub());
        }
        else
        {
            throw InvalidArgumentException("GeneratorClient TryNext expects zero or one arguments");
        }
        try
        {
            RR_INTRUSIVE_PTR<MessageElement> res = NextBase(param);
            mxArray* ret;
            if (this->return_type)
            {
                ret = UnpackMessageElementToMxArray(res, return_type, GetStub());
            }
            else
            {
                ret = mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
            }

            // Return cell array with "true" and ret
            mxArray* ret2 = mxCreateCellMatrix(1, 2);
            mxSetCell(ret2, 0, mxCreateLogicalScalar(true));
            mxSetCell(ret2, 1, ret);
            return ret2;
        }
        catch (StopIterationException&)
        {
            mxArray* ret2 = mxCreateCellMatrix(1, 2);
            mxSetCell(ret2, 0, mxCreateLogicalScalar(false));
            mxSetCell(ret2, 1, mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL));
            return ret2;
        }
        catch (std::exception&)
        {
            boost::mutex::scoped_lock lock(generators_lock);
            generators.erase(generatorid);
            throw;
        }
    }

    if (membername == "Abort")
    {
        {
            boost::mutex::scoped_lock lock(generators_lock);
            generators.erase(generatorid);
        }

        if (mxGetNumberOfElements(cell_args) != 0)
        {
            throw InvalidArgumentException("GeneratorClient Abort does not expect a parameter");
        }

        Abort();
        return mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
    }

    if (membername == "Close")
    {
        {
            boost::mutex::scoped_lock lock(generators_lock);
            generators.erase(generatorid);
        }

        if (mxGetNumberOfElements(cell_args) != 0)
        {
            throw InvalidArgumentException("GeneratorClient Close does not expect a parameter");
        }

        Close();
        return mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
    }

    if (membername == "async_Next")
    {
        int c2 = (int)mxGetNumberOfElements(cell_args);
        RR_INTRUSIVE_PTR<MessageElement> param;

        int pcount = 0;

        if (param_type)
        {
            if (c2 != 3 && c2 != 4)
                throw InvalidArgumentException("GeneratorClient AsyncNext expects parameter plus 2 or 3 arguments");
            param = PackMxArrayToMessageElement(mxGetCell(cell_args, 0), param_type, GetStub());
            pcount = 1;
        }
        else
        {
            if (c2 != 2 && c2 != 3)
                throw InvalidArgumentException("GeneratorClient AsyncNext expects 2 or 3 arguments");
            pcount = 0;
        }

        RR_SHARED_PTR<mxArray> handler(mxDuplicateArray(mxGetCell(cell_args, pcount)), rr_mxDestroyArray);
        RR_SHARED_PTR<mxArray> async_param(mxDuplicateArray(mxGetCell(cell_args, pcount + 1)), rr_mxDestroyArray);

        int32_t timeout = RR_TIMEOUT_INFINITE;
        if (c2 == pcount + 3)
        {
            timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, pcount + 2));
        }

        mexMakeArrayPersistent(handler.get());
        mexMakeArrayPersistent(async_param.get());

        try
        {
            AsyncNextBase(param,
                          boost::bind(&MexGeneratorClient::EndAsyncNext, shared_from_this(), RR_BOOST_PLACEHOLDERS(_1),
                                      RR_BOOST_PLACEHOLDERS(_2), handler, async_param),
                          timeout);
            return mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
        }
        catch (std::exception&)
        {
            boost::mutex::scoped_lock lock(generators_lock);
            generators.erase(generatorid);
            throw;
        }
    }

    if (membername == "async_Close" || membername == "async_Abort")
    {
        int c2 = (int)mxGetNumberOfElements(cell_args);
        RR_INTRUSIVE_PTR<MessageElement> param;

        int pcount = 0;

        if (c2 != 2 && c2 != 3)
            throw InvalidArgumentException("GeneratorClient " + membername + " expects 2 or 3 arguments");

        RR_SHARED_PTR<mxArray> handler(mxDuplicateArray(mxGetCell(cell_args, 0)), rr_mxDestroyArray);
        RR_SHARED_PTR<mxArray> async_param(mxDuplicateArray(mxGetCell(cell_args, 1)), rr_mxDestroyArray);

        int32_t timeout = RR_TIMEOUT_INFINITE;
        if (c2 == 3)
        {
            timeout = mxToTimeoutAdjusted(mxGetCell(cell_args, 2));
        }

        mexMakeArrayPersistent(handler.get());
        mexMakeArrayPersistent(async_param.get());

        try
        {
            if (membername == "async_Close")
            {
                AsyncClose(boost::bind(&MexGeneratorClient::EndAsyncClose, shared_from_this(),
                                       RR_BOOST_PLACEHOLDERS(_1), handler, async_param),
                           timeout);
            }
            else
            {
                AsyncAbort(boost::bind(&MexGeneratorClient::EndAsyncClose, shared_from_this(),
                                       RR_BOOST_PLACEHOLDERS(_1), handler, async_param),
                           timeout);
            }
            return mxCreateNumericMatrix(0, 1, mxDOUBLE_CLASS, mxREAL);
        }
        catch (std::exception&)
        {
            boost::mutex::scoped_lock lock(generators_lock);
            generators.erase(generatorid);
            throw;
        }
    }

    if (membername == "NextAll")
    {
        RR_INTRUSIVE_PTR<MessageElement> param;
        if (mxGetNumberOfElements(cell_args) != 0)
        {
            throw InvalidArgumentException("GeneratorClient NextAll expects a parameter");
        }
        if (param_type)
        {
            throw InvalidArgumentException("GeneratorClient NextAll invalid generator, expects parameter");
        }

        try
        {
            std::vector<RR_INTRUSIVE_PTR<MessageElement> > res;
            try
            {
                while (true)
                {
                    res.push_back(NextBase(param));
                }
            }
            catch (StopIterationException&)
            {}

            mxArray* mx_ret = mxCreateCellMatrix(res.size(), 1);
            for (size_t i = 0; i < res.size(); i++)
            {
                mxSetCell(mx_ret, i, UnpackMessageElementToMxArray(res.at(i), return_type, GetStub()));
            }

            return mx_ret;
        }
        catch (std::exception&)
        {
            boost::mutex::scoped_lock lock(generators_lock);
            generators.erase(generatorid);
            throw;
        }
    }

    throw InvalidArgumentException("Invalid function for generator client");
}

void MexGeneratorClient::subsasgn(const mxArray* S, const mxArray* value)
{
    RR_UNUSED(S);
    RR_UNUSED(value);
    throw InvalidArgumentException("Invalid for generator client");
}

void MexGeneratorClient::EndAsyncNext(const RR_INTRUSIVE_PTR<MessageElement>& res,
                                      const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err,
                                      const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param)
{
    RR_SHARED_PTR<MexAsyncResult> ares;
    if (err)
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(handler, param, err);
        {
            boost::mutex::scoped_lock lock(generators_lock);
            generators.erase(generatorid);
        }
    }
    else
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(handler, param, RR_SHARED_PTR<RobotRaconteurException>());
    }

    if (return_type)
    {
        ares->return_type = return_type;
        if (!ares->error)
        {
            ares->return_value = res;
        }
    }
    RR_SHARED_PTR<MexServiceStub> stub = rr_cast<MexServiceStub>(GetStub());
    boost::mutex::scoped_lock lock(stub->async_results_lock);
    stub->async_results.push(ares);
}

void MexGeneratorClient::EndAsyncClose(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err,
                                       const RR_SHARED_PTR<mxArray>& handler, const RR_SHARED_PTR<mxArray>& param)
{
    RR_SHARED_PTR<MexAsyncResult> ares;
    if (err)
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(handler, param, err);
        {
            boost::mutex::scoped_lock lock(generators_lock);
            generators.erase(generatorid);
        }
    }
    else
    {
        ares = RR_MAKE_SHARED<MexAsyncResult>(handler, param, RR_SHARED_PTR<RobotRaconteurException>());
    }

    RR_SHARED_PTR<MexServiceStub> stub = rr_cast<MexServiceStub>(GetStub());
    boost::mutex::scoped_lock lock(stub->async_results_lock);
    stub->async_results.push(ares);
}

MexPodArrayMemoryClient::MexPodArrayMemoryClient(const std::string& membername, const RR_SHARED_PTR<ServiceStub>& stub,
                                                 size_t element_size, MemberDefinition_Direction direction,
                                                 const RR_SHARED_PTR<TypeDefinition>& type)
    : ArrayMemoryClientBase(membername, stub, DataTypes_pod_t, element_size, direction)
{
    this->type = type;
}

mxArray* MexPodArrayMemoryClient::Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count)
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > buffer(count);
    ReadBase(memorypos, &buffer, bufferpos, count);
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> o1 =
        CreateMessageElementNestedElementList(DataTypes_pod_array_t, type->TypeString, buffer);
    RR_INTRUSIVE_PTR<MessageElement> o = CreateMessageElement("value", o1);
    o->ElementTypeName = o1->TypeName;
    o->DataCount = o1->Elements.size();

    return UnpackMessageElementToMxArray_pod(o, type, this->GetStub());
}

void MexPodArrayMemoryClient::Write(uint64_t memorypos, const mxArray* buffer, uint64_t bufferpos, uint64_t count)
{
    if (!buffer)
        throw NullValueException("Buffer must not be null");
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> o1 = PackMxArrayToMessageElement_pod(buffer, type, GetStub());
    WriteBase(memorypos, &o1->Elements, bufferpos, count);
}

uint64_t MexPodArrayMemoryClient::Length() { return ArrayMemoryClientBase::Length(); }

void MexPodArrayMemoryClient::UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                               uint64_t bufferpos, uint64_t count)
{
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> res2 = rr_cast<MessageElementNestedElementList>(res);

    if (res2->Elements.size() != count)
    {
        throw InvalidOperationException("Invalid memory read return");
    }

    std::vector<RR_INTRUSIVE_PTR<MessageElement> >* buffer2 =
        static_cast<std::vector<RR_INTRUSIVE_PTR<MessageElement> >*>(buffer);
    for (size_t i = 0; i < count; i++)
    {
        buffer2->at(i + bufferpos) = res2->Elements.at(i);
    }
}

RR_INTRUSIVE_PTR<MessageElementData> MexPodArrayMemoryClient::PackWriteRequest(const void* buffer, uint64_t bufferpos,
                                                                               uint64_t count)
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > buffer2 =
        *static_cast<const std::vector<RR_INTRUSIVE_PTR<MessageElement> >*>(buffer);
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > o(count);

    for (size_t i = 0; i < count; i++)
    {
        o.at(i) = buffer2.at(bufferpos + i);
    }

    std::vector<boost::shared_ptr<ServiceDefinition> > other_defs;
    return CreateMessageElementNestedElementList(
        DataTypes_pod_array_t,
        type->ResolveNamedType(other_defs, RobotRaconteurNode::sp(), GetStub())->ResolveQualifiedName(), o);
}

size_t MexPodArrayMemoryClient::GetBufferLength(const void* buffer)
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > buffer2 =
        *static_cast<const std::vector<RR_INTRUSIVE_PTR<MessageElement> >*>(buffer);
    return buffer2.size();
}

DataTypes MexPodArrayMemoryClient::ElementTypeID() { return DataTypes_pod_t; }

MexPodMultiDimArrayMemoryClient::MexPodMultiDimArrayMemoryClient(const std::string& membername,
                                                                 const RR_SHARED_PTR<ServiceStub>& stub,
                                                                 size_t element_size,
                                                                 MemberDefinition_Direction direction,
                                                                 const RR_SHARED_PTR<TypeDefinition>& type)
    : MultiDimArrayMemoryClientBase(membername, stub, DataTypes_pod_t, element_size, direction)
{
    this->type = type;
}
mxArray* MexPodMultiDimArrayMemoryClient::Read(const std::vector<uint64_t>& memorypos,
                                               const std::vector<uint64_t>& bufferpos,
                                               const std::vector<uint64_t>& count)
{
    uint64_t elemcount = boost::accumulate(count, 1, std::multiplies<uint64_t>());

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > buffer(elemcount);

    ReadBase(memorypos, &buffer, bufferpos, count);

    std::vector<mwSize> mx_dims = detail::ConvertVectorType<mwSize>(count);

    RR_INTRUSIVE_PTR<MessageElement> m = CreateMessageElement(
        "array", CreateMessageElementNestedElementList(DataTypes_pod_array_t, type->TypeString, buffer));

    mxArray* mx_buffer = UnpackMessageElementToMxArray_pod(m, type, GetStub());

    if (mxSetDimensions(mx_buffer, &mx_dims[0], mx_dims.size()))
    {
        throw DataTypeException("Dimensions mismatch for pod multidimarray");
    }

    return mx_buffer;
}
void MexPodMultiDimArrayMemoryClient::Write(const std::vector<uint64_t>& memorypos, const mxArray* buffer,
                                            const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count)
{
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> buffer2 =
        PackMxArrayToMessageElement_pod(buffer, type, GetStub());
    WriteBase(memorypos, &buffer2->Elements, bufferpos, count);
}
std::vector<uint64_t> MexPodMultiDimArrayMemoryClient::Dimensions()
{
    return MultiDimArrayMemoryClientBase::Dimensions();
}
uint64_t MexPodMultiDimArrayMemoryClient::DimCount() { return MultiDimArrayMemoryClientBase::DimCount(); }
DataTypes MexPodMultiDimArrayMemoryClient::ElementTypeID() { return DataTypes_pod_t; }
void MexPodMultiDimArrayMemoryClient::UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                                       const std::vector<uint64_t>& bufferpos,
                                                       const std::vector<uint64_t>& count, uint64_t elemcount)
{
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> res2 = rr_cast<MessageElementNestedElementList>(res);
    std::vector<uint64_t> dims =
        RRArrayToVector<uint64_t>(MessageElement::FindElement(res2->Elements, "dims")->CastData<RRArray<uint32_t> >());

    RR_INTRUSIVE_PTR<MessageElementNestedElementList> array =
        MessageElement::FindElement(res2->Elements, "array")->CastDataToNestedList(DataTypes_pod_array_t);

    if (array->Elements.empty())
    {
        throw InvalidOperationException("Invalid memory read return");
    }

    if (!boost::range::equal(count, dims))
    {
        throw InvalidOperationException("Invalid memory read return");
    }

    if (count.empty())
        throw InvalidOperationException("Invalid read operation");

    std::vector<uint64_t> stride(count.size());
    stride[0] = 1;
    size_t start_ind = bufferpos.at(0);
    for (size_t i = 1; i < stride.size(); i++)
    {
        stride[i] = count[i - 1] * stride[i - 1];
        start_ind += stride[i] * bufferpos.at(i);
    }

    std::vector<RR_INTRUSIVE_PTR<MessageElement> >* buffer2 =
        static_cast<std::vector<RR_INTRUSIVE_PTR<MessageElement> >*>(buffer);

    for (size_t i = 0; i < elemcount; i++)
    {
        buffer2->at(i + start_ind) = array->Elements.at(i);
    }
}
RR_INTRUSIVE_PTR<MessageElementData> MexPodMultiDimArrayMemoryClient::PackWriteRequest(
    const void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
{
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > buffer2 =
        *static_cast<const std::vector<RR_INTRUSIVE_PTR<MessageElement> >*>(buffer);

    std::vector<uint64_t> stride(count.size());
    stride[0] = 1;
    size_t start_ind = bufferpos.at(0);
    for (size_t i = 1; i < stride.size(); i++)
    {
        stride[i] = count[i - 1] * stride[i - 1];
        start_ind += stride[i] * bufferpos.at(i);
    }

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > buffer3(elemcount);
    for (size_t i = 0; i < elemcount; i++)
    {
        buffer3.at(i + start_ind) = buffer2.at(i);
    }

    std::vector<boost::shared_ptr<ServiceDefinition> > other_defs;
    std::string qualified_name =
        type->ResolveNamedType(other_defs, RobotRaconteurNode::sp(), GetStub())->ResolveQualifiedName();
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> buffer4 =
        CreateMessageElementNestedElementList(DataTypes_pod_array_t, qualified_name, buffer3);
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > buffer5;
    buffer5.push_back(CreateMessageElement("dims", VectorToRRArray<uint32_t>(count)));
    buffer5.push_back(CreateMessageElement("array", buffer4));
    return CreateMessageElementNestedElementList(DataTypes_pod_multidimarray_t, qualified_name, buffer5);
}

MexNamedArrayMemoryClient::MexNamedArrayMemoryClient(const std::string& membername,
                                                     const RR_SHARED_PTR<ServiceStub>& stub,
                                                     boost::tuple<DataTypes, size_t> array_info,
                                                     MemberDefinition_Direction direction,
                                                     const RR_SHARED_PTR<TypeDefinition>& type)
    : ArrayMemoryClientBase(membername, stub, DataTypes_namedarray_t,
                            RRArrayElementSize(array_info.get<0>()) * array_info.get<1>(), direction)
{
    array_elementtype = DataTypes_void_t;
    array_elementcount = 0;
    this->type = type;
    boost::tie(this->array_elementtype, this->array_elementcount) = array_info;
    type_string = type->ResolveNamedType()->ResolveQualifiedName();
}

mxArray* MexNamedArrayMemoryClient::Read(uint64_t memorypos, uint64_t bufferpos, uint64_t count)
{
    RR_INTRUSIVE_PTR<RRBaseArray> buffer = AllocateRRArrayByType(array_elementtype, count * array_elementcount);
    ReadBase(memorypos, &buffer, bufferpos, count);
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > o2;
    o2.push_back(CreateMessageElement("array", buffer));
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> o1 =
        CreateMessageElementNestedElementList(DataTypes_namedarray_array_t, type->TypeString, o2);
    RR_INTRUSIVE_PTR<MessageElement> o = CreateMessageElement("value", o1);
    o->ElementTypeName = o1->TypeName;
    o->DataCount = o1->Elements.size();

    return UnpackMessageElementToMxArray(o, type, this->GetStub());
}

void MexNamedArrayMemoryClient::Write(uint64_t memorypos, const mxArray* buffer, uint64_t bufferpos, uint64_t count)
{
    if (!buffer)
        throw NullValueException("Buffer must not be null");
    if (mxGetM(buffer) != array_elementcount)
        throw InvalidArgumentException("Invalid shape for namedarray memory");
    if (mxGetNumberOfDimensions(buffer) != 2)
        throw InvalidArgumentException("Invalid shape for namedarray memory");
    RR_INTRUSIVE_PTR<RRBaseArray> buffer1 = GetRRArrayFromMxArray(buffer);
    WriteBase(memorypos, &buffer1, bufferpos, count);
}

uint64_t MexNamedArrayMemoryClient::Length() { return ArrayMemoryClientBase::Length(); }

void MexNamedArrayMemoryClient::UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                                 uint64_t bufferpos, uint64_t count)
{
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> res2 = rr_cast<MessageElementNestedElementList>(res);
    if (res2->TypeName != type_string)
        throw DataTypeException("Data type mismatch");
    RR_INTRUSIVE_PTR<RRBaseArray> res3 = MessageElement::FindElement(res2->Elements, "array")->CastData<RRBaseArray>();
    if (!res3)
        throw DataTypeException("Data type mismatch");
    if (res3->size() != count * array_elementcount)
        throw InvalidOperationException("Invalid memory read return");
    if (res3->GetTypeID() != array_elementtype)
        throw InvalidOperationException("Invalid memory read return");

    RR_INTRUSIVE_PTR<RRBaseArray>* buffer2 = static_cast<RR_INTRUSIVE_PTR<RRBaseArray>*>(buffer);
    memcpy(((uint8_t*)(*buffer2)->void_ptr()) + (bufferpos * element_size), res3->void_ptr(), count * element_size);
}

RR_INTRUSIVE_PTR<MessageElementData> MexNamedArrayMemoryClient::PackWriteRequest(const void* buffer, uint64_t bufferpos,
                                                                                 uint64_t count)
{
    RR_INTRUSIVE_PTR<RRBaseArray> buffer2 = *static_cast<const RR_INTRUSIVE_PTR<RRBaseArray>*>(buffer);
    RR_INTRUSIVE_PTR<RRBaseArray> o1 = AllocateRRArrayByType(array_elementtype, count * array_elementcount);

    memcpy(o1->void_ptr(), ((uint8_t*)(buffer2)->void_ptr()) + (bufferpos * element_size), count * element_size);

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > o2;
    o2.push_back(CreateMessageElement("array", o1));

    return CreateMessageElementNestedElementList(DataTypes_namedarray_array_t, type_string, o2);
}

size_t MexNamedArrayMemoryClient::GetBufferLength(const void* buffer) { return mxGetN((const mxArray*)buffer); }

DataTypes MexNamedArrayMemoryClient::ElementTypeID() { return DataTypes_namedarray_t; }

struct MexNamedMultiDimArrayMemoryClient_buffer
{
    std::vector<uint32_t> dims;
    RR_INTRUSIVE_PTR<RRBaseArray> buffer;
};

MexNamedMultiDimArrayMemoryClient::MexNamedMultiDimArrayMemoryClient(const std::string& membername,
                                                                     const RR_SHARED_PTR<ServiceStub>& stub,
                                                                     boost::tuple<DataTypes, size_t> array_info,
                                                                     MemberDefinition_Direction direction,
                                                                     const RR_SHARED_PTR<TypeDefinition>& type)
    : MultiDimArrayMemoryClientBase(membername, stub, DataTypes_namedarray_t,
                                    RRArrayElementSize(array_info.get<0>()) * array_info.get<1>(), direction)
{
    array_elementtype = DataTypes_void_t;
    array_elementcount = 0;
    this->type = type;
    boost::tie(this->array_elementtype, this->array_elementcount) = array_info;
    type_string = type->ResolveNamedType()->ResolveQualifiedName();
}
mxArray* MexNamedMultiDimArrayMemoryClient::Read(const std::vector<uint64_t>& memorypos,
                                                 const std::vector<uint64_t>& bufferpos,
                                                 const std::vector<uint64_t>& count)
{
    uint64_t elemcount = boost::accumulate(count, 1, std::multiplies<uint64_t>()) * array_elementcount;

    MexNamedMultiDimArrayMemoryClient_buffer buffer;
    buffer.dims = detail::ConvertVectorType<uint32_t>(count);
    buffer.buffer = AllocateRRArrayByType(array_elementtype, elemcount);

    ReadBase(memorypos, &buffer, bufferpos, count);

    std::vector<mwSize> mx_dims = detail::ConvertVectorType<mwSize>(count);

    mx_dims.insert(mx_dims.begin(), array_elementcount);
    mxArray* r = GetMxArrayFromRRArray(buffer.buffer, mx_dims);
    mxSetDimensions(r, &mx_dims[0], mx_dims.size());

    return r;
}
void MexNamedMultiDimArrayMemoryClient::Write(const std::vector<uint64_t>& memorypos, const mxArray* buffer,
                                              const std::vector<uint64_t>& bufferpos,
                                              const std::vector<uint64_t>& count)
{
    if (mxGetM(buffer) != array_elementcount)
        throw InvalidArgumentException("Invalid namedarray shape");
    if (mxGetNumberOfDimensions(buffer) > (count.size() + 1))
        throw InvalidArgumentException("Invalid namedarray assignment dimensions");
    const mwSize* mx_dims = mxGetDimensions(buffer);
    for (size_t i = 0; i < count.size(); i++)
    {
        if (i + 1 < mxGetNumberOfDimensions(buffer))
        {
            if (mx_dims[i + 1] != (count.at(i)))
                throw InvalidArgumentException("Invalid namedarray assignment dimensions");
        }
        else
        {
            if (count.at(i) != 1)
                throw InvalidArgumentException("Invalid namedarray assignment dimensions");
        }
    }

    RR_INTRUSIVE_PTR<RRBaseArray> buffer2 = GetRRArrayFromMxArray(buffer);
    MexNamedMultiDimArrayMemoryClient_buffer buffer3;
    buffer3.dims = detail::ConvertVectorType<uint32_t>(count);
    buffer3.buffer = buffer2;

    WriteBase(memorypos, &buffer3, bufferpos, count);
}
std::vector<uint64_t> MexNamedMultiDimArrayMemoryClient::Dimensions()
{
    return MultiDimArrayMemoryClientBase::Dimensions();
}
uint64_t MexNamedMultiDimArrayMemoryClient::DimCount() { return MultiDimArrayMemoryClientBase::DimCount(); }
DataTypes MexNamedMultiDimArrayMemoryClient::ElementTypeID() { return DataTypes_namedarray_t; }
void MexNamedMultiDimArrayMemoryClient::UnpackReadResult(const RR_INTRUSIVE_PTR<MessageElementData>& res, void* buffer,
                                                         const std::vector<uint64_t>& bufferpos,
                                                         const std::vector<uint64_t>& count, uint64_t elemcount)
{
    RR_UNUSED(elemcount);
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> res2 = rr_cast<MessageElementNestedElementList>(res);
    std::vector<uint64_t> dims =
        RRArrayToVector<uint64_t>(MessageElement::FindElement(res2->Elements, "dims")->CastData<RRArray<uint32_t> >());

    RR_INTRUSIVE_PTR<MessageElementNestedElementList> array =
        MessageElement::FindElement(res2->Elements, "array")->CastDataToNestedList(DataTypes_namedarray_array_t);

    if (array->Elements.size() != 1)
    {
        throw InvalidOperationException("Invalid memory read return");
    }

    if (!boost::range::equal(count, dims))
    {
        throw InvalidOperationException("Invalid memory read return");
    }

    if (count.empty())
        throw InvalidOperationException("Invalid read operation");

    if (array->TypeName != type_string)
        throw InvalidOperationException("Invalid memory read return");

    RR_INTRUSIVE_PTR<RRBaseArray> rr_array =
        MessageElement::FindElement(array->Elements, "array")->CastData<RRBaseArray>();

    MexNamedMultiDimArrayMemoryClient_buffer* buffer3 = static_cast<MexNamedMultiDimArrayMemoryClient_buffer*>(buffer);
    std::vector<uint32_t>& mema_dims = buffer3->dims;

    std::vector<uint32_t> zero_dims(count.size());
    std::vector<uint32_t> bufferpos1 = detail::ConvertVectorType<uint32_t>(bufferpos);
    std::vector<uint32_t> count1 = detail::ConvertVectorType<uint32_t>(count);

    // NOLINTNEXTLINE(readability-suspicious-call-argument)
    RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter =
        // NOLINTNEXTLINE(readability-suspicious-call-argument)
        detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, bufferpos1, count1, zero_dims, count1);

    uint32_t len = 0;
    uint32_t indexa = 0;
    uint32_t indexb = 0;

    while (iter->Next(indexa, indexb, len))
    {
        memcpy((((uint8_t*)buffer3->buffer->void_ptr()) + indexb * element_size),
               (((uint8_t*)rr_array->void_ptr()) + indexa * element_size), len * element_size);
    }
}
RR_INTRUSIVE_PTR<MessageElementData> MexNamedMultiDimArrayMemoryClient::PackWriteRequest(
    const void* buffer, const std::vector<uint64_t>& bufferpos, const std::vector<uint64_t>& count, uint64_t elemcount)
{
    const MexNamedMultiDimArrayMemoryClient_buffer* buffer1 =
        static_cast<const MexNamedMultiDimArrayMemoryClient_buffer*>(buffer);
    RR_INTRUSIVE_PTR<RRBaseArray> buffer2 = buffer1->buffer;

    RR_INTRUSIVE_PTR<RRBaseArray> buffer3 = AllocateRRArrayByType(array_elementtype, elemcount * array_elementcount);

    const std::vector<uint32_t>& mema_dims = buffer1->dims;
    std::vector<uint32_t> zero_dims(count.size());
    std::vector<uint32_t> bufferpos1 = detail::ConvertVectorType<uint32_t>(bufferpos);
    std::vector<uint32_t> count1 = detail::ConvertVectorType<uint32_t>(count);

    // NOLINTNEXTLINE(readability-suspicious-call-argument)
    RR_SHARED_PTR<detail::MultiDimArray_CalculateCopyIndicesIter> iter =
        // NOLINTNEXTLINE(readability-suspicious-call-argument)
        detail::MultiDimArray_CalculateCopyIndicesBeginIter(mema_dims, bufferpos1, count1, zero_dims, count1);

    uint32_t len = 0;
    uint32_t indexa = 0;
    uint32_t indexb = 0;

    while (iter->Next(indexa, indexb, len))
    {
        memcpy((((uint8_t*)buffer3->void_ptr()) + indexb * element_size),
               (((uint8_t*)buffer2->void_ptr()) + indexa * element_size), len * element_size);
    }

    std::vector<RR_INTRUSIVE_PTR<MessageElement> > buffer4;
    buffer4.push_back(CreateMessageElement("array", buffer3));
    RR_INTRUSIVE_PTR<MessageElementNestedElementList> buffer5 =
        CreateMessageElementNestedElementList(DataTypes_namedarray_array_t, type_string, buffer4);
    std::vector<RR_INTRUSIVE_PTR<MessageElement> > buffer6;
    buffer6.push_back(CreateMessageElement("dims", VectorToRRArray<uint32_t>(count)));
    buffer6.push_back(CreateMessageElement("array", buffer5));
    return CreateMessageElementNestedElementList(DataTypes_namedarray_multidimarray_t, type_string, buffer6);
}

std::vector<mwSize> CreateEmptyValue_default_array_dims(const boost::shared_ptr<TypeDefinition>& type1)
{
    std::vector<mwSize> dims;
    switch (type1->ArrayType)
    {
    case DataTypes_ArrayTypes_none: {
        dims.push_back(1);
        return dims;
    }
    case DataTypes_ArrayTypes_array: {

        if (!type1->ArrayLength.empty() && !type1->ArrayVarLength)
        {
            dims.push_back(type1->ArrayLength[0]);
        }
        else
        {
            dims.push_back(0);
        }
        return dims;
    }
    case DataTypes_ArrayTypes_multidimarray: {
        std::vector<mwSize> dims;
        if (!type1->ArrayVarLength)
        {
            dims = detail::ConvertVectorType<mwSize>(type1->ArrayLength);
        }
        else
        {
            dims.push_back(0);
        }
        return dims;
    }
    default:
        throw InvalidArgumentException("Invalid array type");
    }
}

mxArray* CreateEmptyValue(const boost::shared_ptr<TypeDefinition>& type1, const boost::shared_ptr<ServiceStub>& obj);

mxArray* CreateEmptyValue_pod(const boost::shared_ptr<TypeDefinition>& type1, const boost::shared_ptr<ServiceStub>& obj)
{
    std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
    RR_SHARED_PTR<NamedTypeDefinition> nt = type1->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), obj);

    if (nt->RRDataType() != DataTypes_pod_t)
    {
        throw DataTypeException("Invalid pod type");
    }

    boost::shared_ptr<ServiceEntryDefinition> d = rr_cast<ServiceEntryDefinition>(nt);
    boost::shared_array<const char*> fieldnames(new const char*[d->Members.size()]);
    for (size_t i = 0; i < d->Members.size(); i++)
    {
        fieldnames[i] = d->Members[i]->Name.c_str(); // NOLINT
    }

    std::vector<mwSize> dims = CreateEmptyValue_default_array_dims(type1);
    mxArray* o = mxCreateStructArray(dims.size(), &dims[0], (int)d->Members.size(), fieldnames.get());

    size_t num_elements = mxGetNumberOfElements(o);
    for (size_t j = 0; j < num_elements; j++)
    {
        for (size_t i = 0; i < d->Members.size(); i++)
        {
            boost::shared_ptr<PropertyDefinition> p = rr_cast<PropertyDefinition>(d->Members[i]);
            mxSetFieldByNumber(o, j, (int)i, CreateEmptyValue(p->Type, obj));
        }
    }

    return o;
}

mxArray* CreateEmptyValue(const boost::shared_ptr<TypeDefinition>& type1, const boost::shared_ptr<ServiceStub>& obj)
{
    if (type1->ContainerType != DataTypes_ContainerTypes_none || type1->Type == DataTypes_varvalue_t)
    {
        mxArray* o = NULL;
        mexCallMATLAB(1, &o, 0, NULL, "missing");
        return o;
    }

    if (IsTypeNumeric(type1->Type))
    {
        mxClassID matlab_type;
        mxComplexity matlab_complexity = mxREAL;
        switch (type1->Type)
        {
        case DataTypes_csingle_t:
            matlab_complexity = mxCOMPLEX;
            matlab_type = mxSINGLE_CLASS;
            break;
        case DataTypes_cdouble_t:
            matlab_complexity = mxCOMPLEX;
            matlab_type = mxDOUBLE_CLASS;
            break;
        default:
            matlab_type = rrDataTypeToMxClassID(type1->Type);
            break;
        }

        std::vector<mwSize> dims = CreateEmptyValue_default_array_dims(type1);
        mxArray* o = mxCreateNumericArray(dims.size(), &dims[0], matlab_type, matlab_complexity);
        return o;
    }

    if (type1->Type == DataTypes_string_t)
    {
        mxArray* o = mxCreateString("");
        return o;
    }

    if (type1->Type == DataTypes_enum_t)
    {
        mxArray* o = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
        return o;
    }

    if (type1->Type == DataTypes_namedtype_t)
    {
        std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
        RR_SHARED_PTR<NamedTypeDefinition> nt = type1->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), obj);
        switch (nt->RRDataType())
        {
        case DataTypes_structure_t: {
            mxArray* o = NULL;
            mexCallMATLAB(1, &o, 0, NULL, "missing");
            return o;
        }

        case DataTypes_namedarray_t: {
            RR_SHARED_PTR<ServiceEntryDefinition> namedarray_def = rr_cast<ServiceEntryDefinition>(nt);
            boost::tuple<DataTypes, size_t> namedarray_info =
                GetNamedArrayElementTypeAndCount(namedarray_def, empty_defs, RobotRaconteurNode::sp(), obj);

            std::vector<mwSize> dims = CreateEmptyValue_default_array_dims(type1);
            dims.insert(dims.begin(), (int)namedarray_info.get<1>());
            mxArray* o = mxCreateNumericArray(dims.size(), &dims[0], mxDOUBLE_CLASS, mxREAL);
            return o;
        }

        case DataTypes_pod_t: {
            return CreateEmptyValue_pod(type1, obj);
        }

        default:
            throw DataTypeException("Invalid type");
        }
    }

    throw DataTypeException("Invalid type");
}

mxArray* CreateEmptyStructure(const boost::shared_ptr<TypeDefinition>& type1, const boost::shared_ptr<ServiceStub>& obj)
{
    std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
    RR_SHARED_PTR<NamedTypeDefinition> nt = type1->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), obj);

    if (nt->RRDataType() != DataTypes_structure_t)
    {
        throw DataTypeException("Invalid structure type");
    }

    boost::shared_ptr<ServiceEntryDefinition> d = rr_cast<ServiceEntryDefinition>(nt);
    boost::shared_array<const char*> fieldnames(new const char*[d->Members.size()]);
    for (size_t i = 0; i < d->Members.size(); i++)
    {
        fieldnames[i] = d->Members[i]->Name.c_str(); // NOLINT
    }

    mwSize dims = 1;
    mxArray* o = mxCreateStructArray(1, &dims, (int)d->Members.size(), fieldnames.get());

    for (size_t i = 0; i < d->Members.size(); i++)
    {
        boost::shared_ptr<PropertyDefinition> p = rr_cast<PropertyDefinition>(d->Members[i]);
        mxSetFieldByNumber(o, 0, (int)i, CreateEmptyValue(p->Type, obj));
    }

    return o;
}

mxArray* CreateEmptyStructure(const std::string& type_str, const boost::shared_ptr<ServiceStub>& obj)
{
    boost::shared_ptr<TypeDefinition> type1 = boost::make_shared<TypeDefinition>();
    type1->FromString(type_str);
    // resolve named type
    std::vector<RR_SHARED_PTR<ServiceDefinition> > empty_defs;
    boost::shared_ptr<NamedTypeDefinition> nt = type1->ResolveNamedType(empty_defs, RobotRaconteurNode::sp(), obj);
    if (nt->RRDataType() != DataTypes_structure_t)
    {
        throw DataTypeException("Invalid structure type");
    }
    return CreateEmptyStructure(type1, obj);
}

std::map<int32_t, boost::shared_ptr<MexServiceStub> > stubs;
int stubcount = 100;
boost::recursive_mutex stubs_lock;

boost::recursive_mutex pipeendpoints_lock;
int32_t pipeendpoints_count = 0;
std::map<int32_t, boost::shared_ptr<MexPipeEndpoint> > pipeendpoints;

boost::recursive_mutex wireconnections_lock;
int32_t wireconnections_count = 0;
std::map<int32_t, boost::shared_ptr<MexWireConnection> > wireconnections;

boost::recursive_mutex rate_lock;
int32_t rate_count;
std::map<int32_t, RR_SHARED_PTR<RobotRaconteur::Rate> > rates;

boost::recursive_mutex servicesubscriptions_lock;
int32_t serviceinfo2subscriptions_count = 0;
std::map<int32_t, boost::shared_ptr<MexServiceInfo2Subscription> > serviceinfo2subscriptions;
int32_t servicesubscriptions_count = 0;
std::map<int32_t, boost::shared_ptr<MexServiceSubscription> > servicesubscriptions;
int32_t wiresubscriptions_count = 0;
std::map<int32_t, boost::shared_ptr<MexWireSubscription> > wiresubscriptions;
int32_t pipesubscriptions_count = 0;
std::map<int32_t, boost::shared_ptr<MexPipeSubscription> > pipesubscriptions;

std::map<int, boost::weak_ptr<MexServiceSkel> > skels;
boost::mutex skels_lock;
int skelscount = 100;
AutoResetEvent skels_waiting;

boost::weak_ptr<TcpTransport> tcp_transport;
boost::weak_ptr<LocalTransport> local_transport;
boost::weak_ptr<HardwareTransport> usb_transport;

boost::mutex generators_lock;
int32_t generators_count = 0;
std::map<int32_t, boost::shared_ptr<MexGeneratorClient> > generators;

// NOLINTEND(cppcoreguidelines-pro-type-cstyle-cast,cppcoreguidelines-avoid-c-arrays)
