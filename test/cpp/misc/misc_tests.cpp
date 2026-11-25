#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <boost/atomic.hpp>
#include <RobotRaconteur.h>
#include <RobotRaconteur/NodeDirectories.h>
#include <boost/filesystem.hpp>
#ifndef ROBOTRACONTEUR_WINDOWS
#include <sys/stat.h>
#endif
#include <RobotRaconteur/Tap.h>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <RobotRaconteur/RobotRaconteurServiceIndex.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#ifndef ROBOTRACONTEUR_WINDOWS
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif
#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#ifndef ROBOTRACONTEUR_WINDOWS
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifndef _WIN32
#include <pwd.h>
#include <unistd.h>
#endif

using namespace RobotRaconteur;

TEST(RobotRaconteurMisc, LogMacros)
{
    RR_WEAK_PTR<RobotRaconteurNode> node = RobotRaconteurNode::sp();
    RobotRaconteurNode::s()->SetNodeID(NodeID("8cded4c3-5b4a-4682-8b0e-5a01f8eb3378"));
    RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Trace);

    ROBOTRACONTEUR_LOG_FATAL(node, "Testing ROBOTRACONTEUR_LOG_ERROR");
    ROBOTRACONTEUR_LOG_ERROR_DEFAULT("Testing ROBOTRACONTEUR_LOG_ERROR_DEFAULT");
    ROBOTRACONTEUR_LOG_WARNING_COMPONENT(node, User, 294573, "Testing ROBOTRACONTEUR_LOG_ERROR_COMPONENT");
    ROBOTRACONTEUR_LOG_INFO_COMPONENT_PATH(node, User, 294573, "service.path", "my_member",
                                           "Testing ROBOTRACONTEUR_LOG_ERROR_COMPONENT_PATH");
    ROBOTRACONTEUR_LOG_DEBUG_COMPONENTNAME(node, User, "my_component", "my_object", 294573,
                                           "Testing ROBOTRACONTEUR_LOG_ERROR_COMPONENTNAME");
    ROBOTRACONTEUR_LOG_TRACE_COMPONENTNAME_PATH(node, User, "my_component", "my_object", 294573, "service.path",
                                                "my_member", "Testing ROBOTRACONTEUR_LOG_ERROR_COMPONENTNAME_PATH");
    ROBOTRACONTEUR_LOG_TRACE_THROTTLE(node, User, 294573, 100, "Testing ROBOTRACONTEUR_LOG_ERROR_THROTTLE");
    ROBOTRACONTEUR_LOG_TRACE_THROTTLE_PATH(node, User, 294573, "service.path", "my_member", 101,
                                           "Testing ROBOTRACONTEUR_LOG_ERROR_THROTTLE_PATH");
    ROBOTRACONTEUR_LOG_TRACE_THROTTLE_COMPONENTNAME_PATH(node, User, "my_component", "my_object", 294573,
                                                         "service.path", "my_member", 102,
                                                         "Testing ROBOTRACONTEUR_LOG_ERROR_COMPONENTNAME_PATH");

    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Default, -1, "default component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Node, -1, "node component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Transport, -1, "transport component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Message, -1, "message component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Client, -1, "client component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Service, -1, "service component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Member, -1, "member component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Pack, -1, "pack component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Unpack, -1, "unpack component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, ServiceDefinition, -1, "servicedefinition component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Discovery, -1, "discovery component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Subscription, -1, "subscription component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, NodeSetup, -1, "nodesetup component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, Utility, -1, "utility component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, RobDefLib, -1, "robdeflib component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, User, -1, "user component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, UserClient, -1, "user_client component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, UserService, -1, "user_service component")
    ROBOTRACONTEUR_LOG_TRACE_COMPONENT(node, ThirdParty, -1, "third_party component")
}

TEST(RobotRaconteurMisc, NodeDirectories)
{
    NodeDirectories dirs = NodeDirectoriesUtil::GetDefaultNodeDirectories();
    std::cout << "system_data_dir: " << dirs.system_data_dir << std::endl;
    std::cout << "system_config_dir: " << dirs.system_config_dir << std::endl;
    std::cout << "system_state_dir: " << dirs.system_state_dir << std::endl;
    std::cout << "system_cache_dir: " << dirs.system_cache_dir << std::endl;
    std::cout << "system_run_dir: " << dirs.system_run_dir << std::endl;
    std::cout << "user_data_dir: " << dirs.user_data_dir << std::endl;
    std::cout << "user_config_dir: " << dirs.user_config_dir << std::endl;
    std::cout << "user_state_dir: " << dirs.user_state_dir << std::endl;
    std::cout << "user_cache_dir: " << dirs.user_cache_dir << std::endl;
    std::cout << "user_run_dir: " << dirs.user_run_dir << std::endl;
}

#ifndef _WIN32
TEST(RobotRaconteurMisc, GetLogonUserName)
{
    // Run as a UID with no passwd entry (e.g. podman run --user 54321) to exercise the error path
    struct passwd* pw = getpwuid(getuid());
    if (pw == NULL || std::string(pw->pw_name).empty())
    {
        EXPECT_THROW(NodeDirectoriesUtil::GetLogonUserName(), SystemResourceException);
    }
    else
    {
        EXPECT_EQ(NodeDirectoriesUtil::GetLogonUserName(), std::string(pw->pw_name));
    }
}
#endif

TEST(RobotRaconteurMisc, EncodeServicePathIndexTest)
{
    EXPECT_EQ(EncodeServicePathIndex("alphaname1"), "alphaname1");
    EXPECT_EQ(DecodeServicePathIndex("beta2name3l"), "beta2name3l");
    EXPECT_EQ(EncodeServicePathIndex("gamma_with_special!%$moreWords()*.[]"),
              "gamma%5fwith%5fspecial%21%25%24moreWords%28%29%2a%2e%5b%5d");
    EXPECT_EQ(DecodeServicePathIndex("delta%5fwith%5fspecial%3F%0A%5C%2d%21%25%24More%2E%5B%5Dwords%28%29%2A"),
              "delta_with_special?\n\\-!%$More.[]words()*");
    std::string epsilon = "29471``\"'epsilon_with_special?\n\\-!%$more.[]words()*long_string_of_garbage\t\r";
    std::string epsilon_encoded = EncodeServicePathIndex(epsilon);
    std::string epsilon_decoded = DecodeServicePathIndex(epsilon_encoded);
    EXPECT_EQ(epsilon_decoded, epsilon);
    boost::regex re("^[a-zA-Z0-9%]+$");
    EXPECT_TRUE(boost::regex_match(epsilon_encoded, re));
}

TEST(RobotRaconteurMisc, EncodeServicePathIndexNonAsciiTest)
{
    std::string utf8_index = "caf\xc3\xa9";
    std::string encoded = EncodeServicePathIndex(utf8_index);
    EXPECT_EQ(encoded, "caf%c3%a9");
    EXPECT_EQ(DecodeServicePathIndex(encoded), utf8_index);
}

TEST(RobotRaconteurMisc, BuildServicePathTest)
{
    // cSpell: disable
    std::vector<ServicePathSegment> segments;
    segments.push_back(ServicePathSegment("alpha"));
    segments.push_back(ServicePathSegment("be2_ta"));
    segments.push_back(ServicePathSegment("gamma_31", std::string("gamma_index")));
    segments.push_back(
        ServicePathSegment("delta", std::string("delta_index_with_special!%$morewords()*.[]\n\t4927`\"?-")));
    segments.push_back(ServicePathSegment("epsilon", std::string("38563")));

    std::string path = BuildServicePath(segments);
    EXPECT_EQ(path, "alpha.be2_ta.gamma_31[gamma%5findex].delta[delta%5findex%5fwith%5fspecial%21%25%24morewords%28%29%"
                    "2a%2e%5b%5d%0a%094927%60%22%3f%2d].epsilon[38563]");

    std::vector<ServicePathSegment> segments2 = ParseServicePath(path);
    EXPECT_EQ(segments, segments2);

    std::string path2_str = "zeta[alpha].eta1[%2D0175].Theta_3[gamma%5findex].iota[%5B%5D%21bunch%2Dof%2Dgarbage%5f%"
                            "0928576%0A%21%40%23%24%25%5E%26%2A%28%29%2e%0D%5c%2f].kappa";
    std::vector<ServicePathSegment> segments3 = ParseServicePath(path2_str);
    ASSERT_EQ(segments3.size(), 5);
    EXPECT_EQ(segments3[0], ServicePathSegment("zeta", std::string("alpha")));
    EXPECT_EQ(segments3[1], ServicePathSegment("eta1", std::string("-0175")));
    EXPECT_EQ(segments3[2], ServicePathSegment("Theta_3", std::string("gamma_index")));
    EXPECT_EQ(segments3[3], ServicePathSegment("iota", std::string("[]!bunch-of-garbage_\t28576\n!@#$%^&*().\r\\/")));
    EXPECT_EQ(segments3[4], ServicePathSegment("kappa"));

    std::string path2 = BuildServicePath(segments3);
    EXPECT_EQ(boost::to_lower_copy(path2), boost::to_lower_copy(path2_str));
    // cSpell: enable
}

TEST(RobotRaconteurMisc, ParseConnectionURLPortRange)
{
    const char* bad_urls[] = {"rr+tcp://localhost:0/?service=s",        "rr+tcp://localhost:65536/?service=s",
                              "rr+tcp://localhost:70000/?service=s",    "rr+tcp://localhost:-1/?service=s",
                              "rr+wss://localhost:0/?service=s",        "tcp://localhost:0/MyNode/MyService",
                              "tcp://localhost:65536/MyNode/MyService", "tcp://localhost:-1/MyNode/MyService"};
    for (size_t i = 0; i < sizeof(bad_urls) / sizeof(bad_urls[0]); i++)
    {
        EXPECT_THROW(ParseConnectionURL(bad_urls[i]), ConnectionException) << bad_urls[i];
    }

    EXPECT_EQ(ParseConnectionURL("rr+tcp://localhost:1/?service=s").port, 1);
    EXPECT_EQ(ParseConnectionURL("rr+tcp://localhost:65535/?service=s").port, 65535);
    EXPECT_EQ(ParseConnectionURL("tcp://localhost:65535/MyNode/MyService").port, 65535);
    EXPECT_EQ(ParseConnectionURL("rr+tcp://localhost/?service=s").port, 48653);
    EXPECT_EQ(ParseConnectionURL("rr+wss://localhost/?service=s").port, 443);
}

TEST(RobotRaconteurMisc, CreateIdentifierAttributeFilterTest)
{
    ServiceSubscriptionFilterAttribute attr1 = CreateServiceSubscriptionFilterAttributeIdentifier("identifier_1", "");
    EXPECT_TRUE(attr1.IsMatch("identifier_1"));
    EXPECT_FALSE(attr1.IsMatch("identifier_2"));
    EXPECT_TRUE(attr1.IsMatch("identifier_1|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_FALSE(attr1.IsMatch("identifier_2|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));

    ServiceSubscriptionFilterAttribute attr2 =
        CreateServiceSubscriptionFilterAttributeIdentifier("", "cbfea7a4-0361-4aad-95bb-d4dcd967047a");
    EXPECT_FALSE(attr2.IsMatch("identifier_1"));
    EXPECT_FALSE(attr2.IsMatch("identifier_2"));
    EXPECT_TRUE(attr2.IsMatch("identifier_1|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_TRUE(attr2.IsMatch("identifier_2|{cbfea7a4-0361-4aAD-95bb-d4dcd967047a}"));
    EXPECT_TRUE(attr2.IsMatch("identifier_2|cbfea7a403614aAD95bbd4dcd967047a"));
    EXPECT_FALSE(attr2.IsMatch("identifier_2|{cbfea7a4-0361-1aad-95bb-d4dcd967047a}"));

    ServiceSubscriptionFilterAttribute attr3 =
        CreateServiceSubscriptionFilterAttributeIdentifier("identifier_1", "{00000000-0000-0000-0000-000000000000}");
    EXPECT_TRUE(attr3.IsMatch("identifier_1"));
    EXPECT_FALSE(attr3.IsMatch("identifier_2"));
    EXPECT_TRUE(attr3.IsMatch("identifier_1|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_FALSE(attr3.IsMatch("identifier_2|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));

    ServiceSubscriptionFilterAttribute attr4 =
        CreateServiceSubscriptionFilterAttributeIdentifier("identifier_1", "cbfea7a4-0361-4aad-95bb-d4dcd967047a");

    EXPECT_FALSE(attr4.IsMatch("identifier_1"));
    EXPECT_FALSE(attr4.IsMatch("identifier_2"));
    EXPECT_TRUE(attr4.IsMatch("identifier_1|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_FALSE(attr4.IsMatch("identifier_2|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));

    ServiceSubscriptionFilterAttribute attr5 = CreateServiceSubscriptionFilterAttributeIdentifier(
        "identifier_1.with.do_ts", "cbfea7a4-0361-4aad-95bb-d4dcd967047a");
    EXPECT_FALSE(attr5.IsMatch("identifier_1.with.do_ts"));
    EXPECT_FALSE(attr5.IsMatch("identifier_2.WIT_H.dot4s"));
    EXPECT_FALSE(attr5.IsMatch("identifier_1"));
    EXPECT_FALSE(attr5.IsMatch("identifier_2"));
    EXPECT_TRUE(attr5.IsMatch("identifier_1.with.do_ts|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_TRUE(attr5.IsMatch("identifier_1.with.do_ts|{cbfea7a4-0361-4AAD95bb-d4dcd967047a}"));
    EXPECT_FALSE(attr5.IsMatch("identifier_2.WIT_H.dot4s|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_FALSE(attr5.IsMatch("identifier_1.with.do_ts|cbfea7a4-0361-41ad-95bb-d4dcd967047a"));
}

TEST(RobotRaconteurMisc, MessageStringPtrLessThan)
{
    MessageStringPtr a(std::string("alpha"));
    MessageStringPtr a2(std::string("alpha"));
    MessageStringPtr b(std::string("beta"));
    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);
    EXPECT_FALSE(a < a2);
    EXPECT_FALSE(a2 < a);
}

static void AutoResetEventTest_waiter(AutoResetEvent* ev, boost::atomic<bool>* stop, boost::atomic<int>* successes)
{
    while (!stop->load())
    {
        if (ev->WaitOne(1))
        {
            (*successes)++;
        }
    }
}

static void AutoResetEventTest_set_after(AutoResetEvent* ev, int ms)
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(ms));
    ev->Set();
}

TEST(RobotRaconteurMisc, AutoResetEventTimedWait)
{
    AutoResetEvent ev;

    // Timed wait with no Set() must time out, not hang
    boost::posix_time::ptime t1 = boost::posix_time::microsec_clock::universal_time();
    EXPECT_FALSE(ev.WaitOne(50));
    boost::posix_time::time_duration d1 = boost::posix_time::microsec_clock::universal_time() - t1;
    EXPECT_GE(d1.total_milliseconds(), 40);
    EXPECT_LT(d1.total_milliseconds(), 2000);

    // Set() from another thread must release a blocked timed wait promptly
    boost::thread t(boost::bind(&AutoResetEventTest_set_after, &ev, 20));
    boost::posix_time::ptime t2 = boost::posix_time::microsec_clock::universal_time();
    EXPECT_TRUE(ev.WaitOne(5000));
    boost::posix_time::time_duration d2 = boost::posix_time::microsec_clock::universal_time() - t2;
    EXPECT_LT(d2.total_milliseconds(), 2000);
    t.join();

    // Event is auto-reset: the Set() was consumed
    EXPECT_FALSE(ev.WaitOne(10));
}

TEST(RobotRaconteurMisc, AutoResetEventTimedWaitMultipleWaiters)
{
    // Several threads in timed WaitOne() with repeated Set() must not deadlock, and each Set() must
    // wake exactly one waiter
    AutoResetEvent ev;
    boost::atomic<bool> stop(false);
    boost::atomic<int> successes(0);
    const int set_count = 2000;

    std::vector<RR_SHARED_PTR<boost::thread> > threads;
    for (size_t i = 0; i < 8; i++)
    {
        threads.push_back(
            RR_MAKE_SHARED<boost::thread>(boost::bind(&AutoResetEventTest_waiter, &ev, &stop, &successes)));
    }

    bool timed_out = false;
    for (int i = 0; i < set_count && !timed_out; i++)
    {
        ev.Set();
        boost::posix_time::ptime deadline =
            boost::posix_time::microsec_clock::universal_time() + boost::posix_time::seconds(5);
        while (successes.load() < i + 1)
        {
            if (boost::posix_time::microsec_clock::universal_time() > deadline)
            {
                timed_out = true;
                break;
            }
            boost::this_thread::yield();
        }
    }

    stop.store(true);
    BOOST_FOREACH (RR_SHARED_PTR<boost::thread>& t, threads)
    {
        t->join();
    }

    EXPECT_FALSE(timed_out);
    EXPECT_EQ(successes.load(), set_count);
}

class ThreadPoolShutdownTestPool : public ThreadPool
{
  public:
    ThreadPoolShutdownTestPool(const RR_SHARED_PTR<RobotRaconteurNode>& node) : ThreadPool(node) {}
    size_t GetThreadObjectCount()
    {
        boost::mutex::scoped_lock lock(queue_mutex);
        return threads.size();
    }
};

TEST(RobotRaconteurMisc, ThreadPoolShutdownReleasesThreads)
{
    RR_SHARED_PTR<ThreadPoolShutdownTestPool> pool =
        RR_MAKE_SHARED<ThreadPoolShutdownTestPool>(RobotRaconteurNode::sp());
    pool->SetThreadPoolCount(2);
    EXPECT_EQ(pool->GetThreadObjectCount(), 2u);
    pool->Shutdown();
    EXPECT_EQ(pool->GetThreadObjectCount(), 0u);
}

TEST(RobotRaconteurMisc, WallRateDefaultNode)
{
    // WallRate with the default (null) node argument should use RobotRaconteurNode::sp()
    RR_SHARED_PTR<WallRate> rate;
    ASSERT_NO_THROW(rate = RR_MAKE_SHARED<WallRate>(1000.0));
    EXPECT_NO_THROW(rate->Sleep());
    rate.reset();
    // Shut the default node down here: if it is still running when the process exits, its thread pool
    // can outlive objects destroyed during static destruction
    RobotRaconteurNode::s()->Shutdown();
}

TEST(RobotRaconteurMisc, ParseConnectionURLTcpNoPath)
{
    EXPECT_THROW(ParseConnectionURL("tcp://localhost:1234"), ConnectionException);
    ParseConnectionURLResult r = ParseConnectionURL("tcp://localhost:1234/MyNode/MyService");
    EXPECT_EQ(r.nodename, "MyNode");
    EXPECT_EQ(r.service, "MyService");
}

TEST(RobotRaconteurMisc, FilterAttributeGroupSplitStringDelimiter)
{
    ServiceSubscriptionFilterAttributeGroup group(ServiceSubscriptionFilterAttributeGroupOperation_OR);
    group.Attributes.push_back(ServiceSubscriptionFilterAttribute("beta"));
    group.SplitStringDelimiter = ';';
    EXPECT_TRUE(group.IsMatch(boost::string_ref("alpha;beta")));
    EXPECT_FALSE(group.IsMatch(boost::string_ref("alpha,beta")));
}

TEST(RobotRaconteurMisc, RobotRaconteurExceptionInnerExceptionWhat)
{
    std::runtime_error inner("inner error");
    RobotRaconteurException e("outer message", inner);
    EXPECT_EQ(e.Message, "outer message");
    EXPECT_EQ(e.ErrorCode, MessageErrorType_UnknownError);
    EXPECT_NE(std::string(e.what()).find("outer message"), std::string::npos);
}

TEST(RobotRaconteurMisc, TransportCapabilityPageMaskAlias)
{
    // The misspelled TranspartCapabilityCode_PAGE_MASK is kept as an alias for compatibility
    EXPECT_EQ(TransportCapabilityCode_PAGE_MASK, 0xFFF00000u);
    EXPECT_EQ(TranspartCapabilityCode_PAGE_MASK, TransportCapabilityCode_PAGE_MASK);
}

TEST(RobotRaconteurMisc, IsStringIdentifier)
{
    EXPECT_TRUE(IsStringIdentifier("identifier_1"));
    EXPECT_TRUE(IsStringIdentifier("identifier_1.with.dots"));
    EXPECT_TRUE(IsStringIdentifier("cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_TRUE(IsStringIdentifier("{cbfea7a4-0361-4aad-95bb-d4dcd967047a}"));
    EXPECT_TRUE(IsStringIdentifier("identifier_1|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_TRUE(IsStringIdentifier("identifier_1|{cbfea7a4-0361-4aad-95bb-d4dcd967047a}"));
    // A '|' must be followed by a UUID
    EXPECT_FALSE(IsStringIdentifier("identifier_1|"));
    EXPECT_FALSE(IsStringIdentifier("|cbfea7a4-0361-4aad-95bb-d4dcd967047a"));
    EXPECT_FALSE(IsStringIdentifier("identifier_1|not_a_uuid"));
    EXPECT_FALSE(IsStringIdentifier(""));
}

#if !defined(ROBOTRACONTEUR_WINDOWS) && !defined(ROBOTRACONTEUR_APPLE)
static NodeDirectories bughunt_temp_node_dirs(const boost::filesystem::path& base,
                                              const boost::filesystem::path& user_run_dir)
{
    NodeDirectories d;
    d.system_data_dir = base / "sys_data";
    d.system_config_dir = base / "sys_config";
    d.system_state_dir = base / "sys_state";
    d.system_cache_dir = base / "sys_cache";
    d.system_run_dir = base / "sys_run";
    d.user_data_dir = base / "user_data";
    d.user_config_dir = base / "user_config";
    d.user_state_dir = base / "user_state";
    d.user_cache_dir = base / "user_cache";
    d.user_run_dir = user_run_dir;
    return d;
}

static bool bughunt_start_local_server(const NodeDirectories& dirs)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->SetNodeDirectories(dirs);
    RR_SHARED_PTR<LocalTransport> t = RR_MAKE_SHARED<LocalTransport>(node);
    node->RegisterTransport(t);
    bool ok = true;
    try
    {
        t->StartServerAsNodeName("bughunt_tmp_run_dir");
    }
    catch (std::exception&)
    {
        ok = false;
    }
    node->Shutdown();
    return ok;
}

// The /tmp/robotraconteur-run-<uid> fallback user run dir must be private to the current user
TEST(RobotRaconteurMisc, LocalTransportTmpUserRunDirSecure)
{
    boost::filesystem::path run_dir = "/tmp/robotraconteur-run-" + boost::lexical_cast<std::string>(getuid());
    boost::filesystem::path base = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::path other = base / "other";
    boost::filesystem::create_directories(other);
    NodeDirectories dirs = bughunt_temp_node_dirs(base, run_dir);

    // Symlink planted by another user: rejected, nothing created in the link target
    boost::filesystem::remove_all(run_dir);
    boost::filesystem::create_directory_symlink(other, run_dir);
    EXPECT_FALSE(bughunt_start_local_server(dirs));
    EXPECT_FALSE(boost::filesystem::exists(other / "transport"));
    EXPECT_FALSE(boost::filesystem::exists(other / "nodeids"));
    boost::filesystem::remove(run_dir);

    // Directory owned by another user: rejected
    if (geteuid() == 0)
    {
        boost::filesystem::create_directory(run_dir);
        ASSERT_EQ(chown(run_dir.c_str(), 12345, 12345), 0);
        chmod(run_dir.c_str(), 0777);
        EXPECT_FALSE(bughunt_start_local_server(dirs));
        boost::filesystem::remove_all(run_dir);
    }

    // Own directory with permissive mode: accepted, mode tightened to 0700
    boost::filesystem::create_directory(run_dir);
    chmod(run_dir.c_str(), 0777);
    EXPECT_TRUE(bughunt_start_local_server(dirs));
    struct stat st = {};
    ASSERT_EQ(lstat(run_dir.c_str(), &st), 0);
    EXPECT_EQ(st.st_mode & 0777, 0700u);
    boost::filesystem::remove_all(run_dir);

    // Missing directory: created with mode 0700
    EXPECT_TRUE(bughunt_start_local_server(dirs));
    ASSERT_EQ(lstat(run_dir.c_str(), &st), 0);
    EXPECT_TRUE(S_ISDIR(st.st_mode));
    EXPECT_EQ(st.st_mode & 0777, 0700u);

    boost::filesystem::remove_all(run_dir);
    boost::filesystem::remove_all(base);
}
#endif

#ifndef ROBOTRACONTEUR_WINDOWS
static std::string bughunt_read_file(const boost::filesystem::path& p)
{
    std::ifstream f(p.string().c_str());
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// Pid/info files in run directories must not follow a pre-placed symlink
TEST(RobotRaconteurMisc, NodeDirectoriesPidInfoFileNoFollow)
{
    boost::filesystem::path base = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::create_directories(base);
    boost::filesystem::path victim = base / "victim.txt";
    {
        std::ofstream f(victim.string().c_str());
        f << "original";
    }

    boost::filesystem::path pid_link = base / "node.pid";
    boost::filesystem::path info_link = base / "node.info";
    boost::filesystem::create_symlink(victim, pid_link);
    boost::filesystem::create_symlink(victim, info_link);

    RR_SHARED_PTR<NodeDirectoriesFD> h_pid;
    EXPECT_ANY_THROW(h_pid = NodeDirectoriesUtil::CreatePidFile(pid_link));
    std::map<std::string, std::string> info;
    info.insert(std::make_pair("nodename", "bughunt"));
    RR_SHARED_PTR<NodeDirectoriesFD> h_info;
    EXPECT_ANY_THROW(h_info = NodeDirectoriesUtil::CreateInfoFile(info_link, info));
    h_pid.reset();
    h_info.reset();

    EXPECT_EQ(bughunt_read_file(victim), "original");

    // Regular (non-link) files still work
    RR_SHARED_PTR<NodeDirectoriesFD> h_pid2;
    EXPECT_NO_THROW(h_pid2 = NodeDirectoriesUtil::CreatePidFile(base / "node2.pid"));
    h_pid2.reset();

    boost::filesystem::remove_all(base);
}
#endif

#ifndef ROBOTRACONTEUR_WINDOWS
static void bughunt_read_info_thread(boost::filesystem::path p, bool* done)
{
    std::map<std::string, std::string> info;
    NodeDirectoriesUtil::ReadInfoFile(p, info);
    *done = true;
}

// ReadInfoFile must cap the file size (like the Windows branch) and must not block on a FIFO
TEST(RobotRaconteurMisc, NodeDirectoriesReadInfoFileLimits)
{
    boost::filesystem::path base = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::create_directories(base);

    boost::filesystem::path small_info = base / "small.info";
    {
        std::ofstream f(small_info.string().c_str());
        f << "nodename: bughunt\n";
    }
    std::map<std::string, std::string> info;
    EXPECT_TRUE(NodeDirectoriesUtil::ReadInfoFile(small_info, info));
    EXPECT_EQ(info["nodename"], "bughunt");

    boost::filesystem::path big_info = base / "big.info";
    {
        std::ofstream f(big_info.string().c_str());
        f << "nodename: " << std::string(32 * 1024, 'a') << "\n";
    }
    std::map<std::string, std::string> info2;
    EXPECT_FALSE(NodeDirectoriesUtil::ReadInfoFile(big_info, info2));

    boost::filesystem::path fifo_info = base / "fifo.info";
    ASSERT_EQ(mkfifo(fifo_info.c_str(), 0600), 0);
    bool done = false;
    boost::thread t(boost::bind(&bughunt_read_info_thread, fifo_info, &done));
    bool finished = t.timed_join(boost::posix_time::seconds(2));
    if (!finished)
    {
        // Unblock the reader stuck in open()
        int w = open(fifo_info.c_str(), O_WRONLY | O_NONBLOCK);
        if (w >= 0)
            close(w);
        t.join();
    }
    EXPECT_TRUE(finished);

    boost::filesystem::remove_all(base);
}
#endif

#if defined(ROBOTRACONTEUR_LINUX) && !defined(ROBOTRACONTEUR_ANDROID)
static NodeDirectories bughunt_discovery_node_dirs(const boost::filesystem::path& base)
{
    NodeDirectories d;
    d.system_data_dir = base / "sys_data";
    d.system_config_dir = base / "sys_config";
    d.system_state_dir = base / "sys_state";
    d.system_cache_dir = base / "sys_cache";
    d.system_run_dir = base / "sys_run";
    d.user_data_dir = base / "user_data";
    d.user_config_dir = base / "user_config";
    d.user_state_dir = base / "user_state";
    d.user_cache_dir = base / "user_cache";
    d.user_run_dir = base / "user_run";
    return d;
}

// Removing the private by-nodeid directory while LocalTransport discovery runs must not terminate the process
TEST(RobotRaconteurMisc, LocalTransportDiscoveryPrivateDirRemoved)
{
    boost::filesystem::path base = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::create_directories(base);

    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->SetNodeDirectories(bughunt_discovery_node_dirs(base));
    RR_SHARED_PTR<LocalTransport> t = RR_MAKE_SHARED<LocalTransport>(node);
    node->RegisterTransport(t);
    t->EnableNodeDiscoveryListening();

    boost::filesystem::path bynodeid = base / "user_run" / "transport" / "local" / "by-nodeid";
    for (int i = 0; i < 50 && !boost::filesystem::exists(bynodeid); i++)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }
    ASSERT_TRUE(boost::filesystem::exists(bynodeid));
    boost::this_thread::sleep(boost::posix_time::milliseconds(500));

    boost::filesystem::remove_all(bynodeid);
    boost::this_thread::sleep(boost::posix_time::milliseconds(2000));

    t->DisableNodeDiscoveryListening();
    node->Shutdown();
    boost::filesystem::remove_all(base);
}
#endif

#if defined(ROBOTRACONTEUR_LINUX) && !defined(ROBOTRACONTEUR_ANDROID)
static size_t bughunt_count_open_fds()
{
    size_t n = 0;
    for (boost::filesystem::directory_iterator it("/proc/self/fd"); it != boost::filesystem::directory_iterator(); ++it)
    {
        n++;
    }
    return n;
}

// LocalTransport discovery must close the inotify fd of each per-user public directory watcher it drops
TEST(RobotRaconteurMisc, LocalTransportDiscoveryInotifyFdLeak)
{
    boost::filesystem::path base = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::path pub = base / "sys_run" / "transport" / "local";
    boost::filesystem::create_directories(pub);

    NodeDirectories d;
    d.system_data_dir = base / "sys_data";
    d.system_config_dir = base / "sys_config";
    d.system_state_dir = base / "sys_state";
    d.system_cache_dir = base / "sys_cache";
    d.system_run_dir = base / "sys_run";
    d.user_data_dir = base / "user_data";
    d.user_config_dir = base / "user_config";
    d.user_state_dir = base / "user_state";
    d.user_cache_dir = base / "user_cache";
    d.user_run_dir = base / "user_run";

    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->SetNodeDirectories(d);
    RR_SHARED_PTR<LocalTransport> t = RR_MAKE_SHARED<LocalTransport>(node);
    node->RegisterTransport(t);
    t->EnableNodeDiscoveryListening();
    boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

    size_t fds_before = bughunt_count_open_fds();
    const int cycles = 5;
    for (int i = 0; i < cycles; i++)
    {
        boost::filesystem::path user_dir = pub / ("bughunt_user" + boost::lexical_cast<std::string>(i));
        boost::filesystem::create_directory(user_dir);
        boost::this_thread::sleep(boost::posix_time::milliseconds(800));
        boost::filesystem::remove_all(user_dir);
        boost::this_thread::sleep(boost::posix_time::milliseconds(800));
    }
    size_t fds_after = bughunt_count_open_fds();
    EXPECT_LT(fds_after, fds_before + 2) << "fds before: " << fds_before << " after: " << fds_after;

    t->DisableNodeDiscoveryListening();
    node->Shutdown();
    boost::filesystem::remove_all(base);
}
#endif

// Client transport that fails every connection attempt by invoking the handler synchronously, like
// LocalTransport does when attaching the stream fails
class SyncFailClientTransport : public Transport
{
  public:
    SyncFailClientTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node) : Transport(node) {}
    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE { RR_UNUSED(endpoint); }
    RR_OVIRTUAL bool IsClient() const RR_OVERRIDE { return true; }
    RR_OVIRTUAL bool IsServer() const RR_OVERRIDE { return false; }
    RR_OVIRTUAL std::string GetUrlSchemeString() const RR_OVERRIDE { return "rr+syncfail"; }
    RR_OVIRTUAL std::vector<std::string> GetServerListenUrls() RR_OVERRIDE { return std::vector<std::string>(); }
    RR_OVIRTUAL bool CanConnectService(boost::string_ref url) RR_OVERRIDE
    {
        return boost::starts_with(url, "rr+syncfail://");
    }
    RR_OVIRTUAL RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE
    {
        RR_UNUSED(url);
        RR_UNUSED(e);
        throw ConnectionException("Connection failed");
    }
    RR_OVIRTUAL void AsyncCreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(url);
        RR_UNUSED(e);
        handler(RR_SHARED_PTR<ITransportConnection>(), RR_MAKE_SHARED<ConnectionException>("Connection failed"));
    }
    RR_OVIRTUAL void CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE { RR_UNUSED(e); }
    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE
    {
        RR_UNUSED(m);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(m);
        RR_UNUSED(handler);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE { RR_UNUSED(m); }
};

struct SyncFailConnectResult
{
    boost::mutex lock;
    boost::condition_variable cv;
    bool done;
    RR_SHARED_PTR<RobotRaconteurException> err;
    SyncFailConnectResult() : done(false) {}
};

static void SyncFailConnectHandler(const RR_SHARED_PTR<SyncFailConnectResult>& res, const RR_SHARED_PTR<RRObject>& obj,
                                   const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    RR_UNUSED(obj);
    boost::mutex::scoped_lock lock(res->lock);
    res->done = true;
    res->err = err;
    res->cv.notify_all();
}

// A transport that invokes the connect handler synchronously must not deadlock the connector
// (RobotRaconteurNode_connector::connect2 held active_lock across AsyncCreateTransportConnection)
TEST(RobotRaconteurMisc, ConnectorSyncTransportFailure)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->RegisterTransport(RR_MAKE_SHARED<SyncFailClientTransport>(node));

    RR_SHARED_PTR<SyncFailConnectResult> res = RR_MAKE_SHARED<SyncFailConnectResult>();
    boost::function<void(const RR_SHARED_PTR<ClientContext>&, ClientServiceListenerEventType,
                         const RR_SHARED_PTR<void>&)>
        listener;
    node->AsyncConnectService(
        "rr+syncfail://localhost/?nodename=test&service=test", "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(),
        listener, "", boost::bind(&SyncFailConnectHandler, res, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
        30000);

    bool done = false;
    {
        boost::mutex::scoped_lock lock(res->lock);
        boost::system_time deadline = boost::get_system_time() + boost::posix_time::seconds(10);
        while (!res->done)
        {
            if (!res->cv.timed_wait(lock, deadline))
                break;
        }
        done = res->done;
    }

    if (!done)
    {
        // A thread pool thread is deadlocked; Shutdown would hang, so leak the node
        new RR_SHARED_PTR<RobotRaconteurNode>(node);
        FAIL() << "Connect handler not called: connector deadlocked";
    }

    EXPECT_TRUE(res->err);
    node->Shutdown();
}

static void ThreadPoolPostTestNoop() {}

TEST(RobotRaconteurMisc, ThreadPoolPostDoesNotKeepNodeAlive)
{
    // A pool without threads never runs posted ops, like an op posted while the pool shuts down
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    RR_WEAK_PTR<RobotRaconteurNode> weak_node = node;
    RR_SHARED_PTR<ThreadPool> pool = RR_MAKE_SHARED<ThreadPool>(node);
    EXPECT_NO_THROW(pool->Post(&ThreadPoolPostTestNoop));
    EXPECT_TRUE(pool->TryPost(&ThreadPoolPostTestNoop));
    node.reset();
    EXPECT_TRUE(weak_node.expired());
    pool->Shutdown();
}

namespace
{
struct ThreadPoolRunningOpState
{
    boost::mutex lock;
    boost::condition_variable cv;
    bool started;
    bool released;
    bool done;
    bool node_alive;
    RR_WEAK_PTR<RobotRaconteurNode> weak_node;
    ThreadPoolRunningOpState() : started(false), released(false), done(false), node_alive(false) {}
};

void ThreadPoolRunningOp(RR_SHARED_PTR<ThreadPoolRunningOpState> s)
{
    boost::mutex::scoped_lock lock(s->lock);
    s->started = true;
    s->cv.notify_all();
    while (!s->released)
        s->cv.wait(lock);
    s->node_alive = !s->weak_node.expired();
    s->done = true;
    s->cv.notify_all();
}
} // namespace

TEST(RobotRaconteurMisc, ThreadPoolPostKeepsNodeAliveWhileOpRuns)
{
    // Dropping the last other reference while a posted op runs must not destroy the node under the op
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    RR_SHARED_PTR<ThreadPoolRunningOpState> s = RR_MAKE_SHARED<ThreadPoolRunningOpState>();
    s->weak_node = node;
    RR_SHARED_PTR<ThreadPool> pool = RR_MAKE_SHARED<ThreadPool>(node);
    pool->SetThreadPoolCount(1);
    pool->Post(boost::bind(&ThreadPoolRunningOp, s));
    {
        boost::mutex::scoped_lock lock(s->lock);
        while (!s->started)
            s->cv.wait(lock);
    }
    node.reset();
    {
        boost::mutex::scoped_lock lock(s->lock);
        s->released = true;
        s->cv.notify_all();
        while (!s->done)
            s->cv.wait(lock);
    }
    EXPECT_TRUE(s->node_alive);
    pool->Shutdown();
    EXPECT_TRUE(s->weak_node.expired());
}

class CaptureLogRecordHandler : public LogRecordHandler
{
  public:
    std::vector<RRLogRecord> records;
    RR_OVIRTUAL void HandleLogRecord(const RRLogRecord& record) RR_OVERRIDE { records.push_back(record); }
};

// RobotRaconteurNode::LogMessage must set the record time like the logging macros do
TEST(RobotRaconteurMisc, LogMessageTime)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    RR_SHARED_PTR<CaptureLogRecordHandler> handler = RR_MAKE_SHARED<CaptureLogRecordHandler>();
    node->SetLogRecordHandler(handler);
    node->SetLogLevel(RobotRaconteur_LogLevel_Info);
    node->LogMessage(RobotRaconteur_LogLevel_Warning, "test LogMessage time");
    node->SetLogRecordHandler(RR_SHARED_PTR<LogRecordHandler>());
    ASSERT_EQ(handler->records.size(), 1u);
    EXPECT_FALSE(handler->records[0].Time.is_special());
    node->Shutdown();
}

TEST(RobotRaconteurMisc, ArrayMemoryRangeOverflow)
{
    // memorypos/bufferpos + count must not wrap around and pass the range check
    RR_INTRUSIVE_PTR<RRArray<double> > data = AllocateRRArray<double>(16);
    ArrayMemory<double> mem(data);
    RR_INTRUSIVE_PTR<RRArray<double> > buf = AllocateRRArray<double>(4);
    uint64_t wrap_pos = std::numeric_limits<uint64_t>::max() - 1;

    EXPECT_THROW(mem.Read(wrap_pos, buf, 0, 4), OutOfRangeException);
    EXPECT_THROW(mem.Read(0, buf, wrap_pos, 4), OutOfRangeException);
    EXPECT_THROW(mem.Write(0, buf, wrap_pos, 4), OutOfRangeException);
    EXPECT_THROW(mem.Read(17, buf, 0, 0), OutOfRangeException);

    EXPECT_NO_THROW(mem.Read(12, buf, 0, 4));
    EXPECT_NO_THROW(mem.Write(12, buf, 0, 4));
    EXPECT_NO_THROW(mem.Read(16, buf, 4, 0));
}

TEST(RobotRaconteurMisc, ServiceInfo2NullMembers)
{
    // Fields sent as void by a remote service index unpack to null and must not be dereferenced
    RobotRaconteurServiceIndex::ServiceInfo info;
    info.Name = "service1";
    info.RootObjectType = "com.example.obj";
    RobotRaconteurServiceIndex::NodeInfo ninfo;
    ninfo.NodeName = "node1";
    ninfo.NodeID = ArrayToRRArray<uint8_t>(NodeID::NewUniqueID().ToByteArray());

    ServiceInfo2 si(info, ninfo);
    EXPECT_EQ(si.Name, "service1");
    EXPECT_TRUE(si.Attributes.empty());
    EXPECT_TRUE(si.ConnectionURL.empty());
    EXPECT_TRUE(si.RootObjectImplements.empty());
}

// SetLogLevelFromEnvVariable leaves the level unchanged when the variable is not set and returns the current level
TEST(RobotRaconteurMisc, SetLogLevelFromEnvVariableUnset)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->SetLogLevel(RobotRaconteur_LogLevel_Error);
    RobotRaconteur_LogLevel ret = node->SetLogLevelFromEnvVariable("ROBOTRACONTEUR_TEST_LOG_LEVEL_NOT_SET_8d1f6a");
    EXPECT_EQ(node->GetLogLevel(), RobotRaconteur_LogLevel_Error);
    EXPECT_EQ(ret, RobotRaconteur_LogLevel_Error);
}

static size_t CountLocalSocketFiles(const boost::filesystem::path& dir)
{
    size_t n = 0;
    if (!boost::filesystem::is_directory(dir))
        return 0;
    for (boost::filesystem::directory_iterator e(dir); e != boost::filesystem::directory_iterator(); ++e)
    {
        if (e->path().extension() == ".sock")
            n++;
    }
    return n;
}

// StartServerAsNodeID must start a working server and leave one socket file
TEST(RobotRaconteurMisc, LocalTransportStartServerAsNodeID)
{
    boost::filesystem::path base =
        boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("rr-test-%%%%-%%%%");
    boost::filesystem::create_directories(base);

    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    NodeDirectories dirs = NodeDirectoriesUtil::GetDefaultNodeDirectories(node);
    dirs.user_run_dir = base / "run";
    node->SetNodeDirectories(dirs);
    RR_SHARED_PTR<LocalTransport> t = RR_MAKE_SHARED<LocalTransport>(node);
    node->RegisterTransport(t);

    RR_SHARED_PTR<RobotRaconteurNode> client_node = RR_MAKE_SHARED<RobotRaconteurNode>();
    client_node->Init();
    client_node->SetNodeDirectories(dirs);
    RR_SHARED_PTR<LocalTransport> client_t = RR_MAKE_SHARED<LocalTransport>(client_node);
    client_node->RegisterTransport(client_t);

    NodeID id = NodeID::NewUniqueID();
    EXPECT_NO_THROW(t->StartServerAsNodeID(id));
    EXPECT_EQ(node->NodeID(), id);
    EXPECT_EQ(CountLocalSocketFiles(base / "run" / "socket"), 1u);

    RR_SHARED_PTR<RRObject> c;
    EXPECT_NO_THROW(c = client_node->ConnectService("rr+local:///?nodeid=" + id.ToString("D") +
                                                    "&service=RobotRaconteurServiceIndex"));
    if (c)
    {
        EXPECT_NO_THROW(client_node->DisconnectService(c));
    }

    client_node->Shutdown();
    node->Shutdown();
    boost::system::error_code ec;
    boost::filesystem::remove_all(base, ec);
}

// Connection stand-in that counts Close() calls
class NcCountingTransportConnection : public ITransportConnection
{
  public:
    RR_WEAK_PTR<RobotRaconteurNode> node;
    RR_SHARED_PTR<boost::atomic<int> > close_count;
    NcCountingTransportConnection(const RR_SHARED_PTR<RobotRaconteurNode>& node)
        : node(node), close_count(RR_MAKE_SHARED<boost::atomic<int> >(0))
    {}
    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE
    {
        RR_UNUSED(m);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(m);
        RR_UNUSED(handler);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void Close() RR_OVERRIDE { (*close_count)++; }
    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE { RR_UNUSED(endpoint); }
    RR_OVIRTUAL uint32_t GetLocalEndpoint() RR_OVERRIDE { return 0; }
    RR_OVIRTUAL uint32_t GetRemoteEndpoint() RR_OVERRIDE { return 0; }
    RR_OVIRTUAL NodeID GetRemoteNodeID() RR_OVERRIDE { return NodeID::GetAny(); }
    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteurNode> GetNode() RR_OVERRIDE { return node.lock(); }
    RR_OVIRTUAL bool CheckCapabilityActive(uint32_t flag) RR_OVERRIDE
    {
        RR_UNUSED(flag);
        return true;
    }
    RR_OVIRTUAL RR_SHARED_PTR<Transport> GetTransport() RR_OVERRIDE { return RR_SHARED_PTR<Transport>(); }
};

// Client transport whose connections are created, but sending the first request throws synchronously
// (like a peer that closed right after the transport handshake)
class NcSendFailClientTransport : public Transport
{
  public:
    RR_SHARED_PTR<NcCountingTransportConnection> connection;
    boost::atomic<int> close_count;
    NcSendFailClientTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node) : Transport(node), close_count(0)
    {
        connection = RR_MAKE_SHARED<NcCountingTransportConnection>(node);
    }
    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE { RR_UNUSED(endpoint); }
    RR_OVIRTUAL bool IsClient() const RR_OVERRIDE { return true; }
    RR_OVIRTUAL bool IsServer() const RR_OVERRIDE { return false; }
    RR_OVIRTUAL std::string GetUrlSchemeString() const RR_OVERRIDE { return "rr+ncsendfail"; }
    RR_OVIRTUAL std::vector<std::string> GetServerListenUrls() RR_OVERRIDE { return std::vector<std::string>(); }
    RR_OVIRTUAL bool CanConnectService(boost::string_ref url) RR_OVERRIDE
    {
        return boost::starts_with(url, "rr+ncsendfail://");
    }
    RR_OVIRTUAL RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE
    {
        RR_UNUSED(url);
        RR_UNUSED(e);
        return connection;
    }
    RR_OVIRTUAL void AsyncCreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(url);
        RR_UNUSED(e);
        RR_SHARED_PTR<ITransportConnection> c = connection;
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, c, RR_SHARED_PTR<RobotRaconteurException>()),
                                                true);
    }
    RR_OVIRTUAL void CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE { RR_UNUSED(e); }
    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE
    {
        RR_UNUSED(m);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(m);
        RR_UNUSED(handler);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE { RR_UNUSED(m); }
    RR_OVIRTUAL void Close() RR_OVERRIDE { close_count++; }
};

struct NcConnectResult
{
    boost::mutex lock;
    boost::condition_variable cv;
    bool done;
    RR_SHARED_PTR<RobotRaconteurException> err;
    NcConnectResult() : done(false) {}
};

static void NcConnectHandler(const RR_SHARED_PTR<NcConnectResult>& res, const RR_SHARED_PTR<RRObject>& obj,
                             const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    RR_UNUSED(obj);
    boost::mutex::scoped_lock lock(res->lock);
    res->done = true;
    res->err = err;
    res->cv.notify_all();
}

// When connecting a service on a candidate connection fails synchronously, the connector must close that
// connection, not the whole transport (which would drop every connection and listener of the node)
TEST(RobotRaconteurMisc, ConnectorFailureClosesConnectionNotTransport)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    RR_SHARED_PTR<NcSendFailClientTransport> t = RR_MAKE_SHARED<NcSendFailClientTransport>(node);
    node->RegisterTransport(t);

    RR_SHARED_PTR<NcConnectResult> res = RR_MAKE_SHARED<NcConnectResult>();
    boost::function<void(const RR_SHARED_PTR<ClientContext>&, ClientServiceListenerEventType,
                         const RR_SHARED_PTR<void>&)>
        listener;
    node->AsyncConnectService(
        "rr+ncsendfail://localhost/?nodename=test&service=test", "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(),
        listener, "", boost::bind(&NcConnectHandler, res, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)), 30000);

    bool done = false;
    {
        boost::mutex::scoped_lock lock(res->lock);
        boost::system_time deadline = boost::get_system_time() + boost::posix_time::seconds(10);
        while (!res->done)
        {
            if (!res->cv.timed_wait(lock, deadline))
                break;
        }
        done = res->done;
    }

    EXPECT_TRUE(done);
    EXPECT_TRUE(res->err);
    EXPECT_EQ(t->close_count.load(), 0);
    EXPECT_GE(t->connection->close_count->load(), 1);
    node->Shutdown();
}

// Transport whose node discovery query throws synchronously
class NcDiscoveryThrowTransport : public Transport
{
  public:
    NcDiscoveryThrowTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node) : Transport(node) {}
    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE { RR_UNUSED(endpoint); }
    RR_OVIRTUAL bool IsClient() const RR_OVERRIDE { return true; }
    RR_OVIRTUAL bool IsServer() const RR_OVERRIDE { return false; }
    RR_OVIRTUAL std::string GetUrlSchemeString() const RR_OVERRIDE { return "rr+ncdiscthrow"; }
    RR_OVIRTUAL std::vector<std::string> GetServerListenUrls() RR_OVERRIDE { return std::vector<std::string>(); }
    RR_OVIRTUAL bool CanConnectService(boost::string_ref url) RR_OVERRIDE
    {
        RR_UNUSED(url);
        return false;
    }
    RR_OVIRTUAL RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE
    {
        RR_UNUSED(url);
        RR_UNUSED(e);
        throw ConnectionException("Connection failed");
    }
    RR_OVIRTUAL void AsyncCreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(url);
        RR_UNUSED(e);
        RR_UNUSED(handler);
        throw ConnectionException("Connection failed");
    }
    RR_OVIRTUAL void CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE { RR_UNUSED(e); }
    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE
    {
        RR_UNUSED(m);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(m);
        RR_UNUSED(handler);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE { RR_UNUSED(m); }
    RR_OVIRTUAL void AsyncGetDetectedNodes(
        const std::vector<std::string>& schemes,
        const boost::function<void(const RR_SHARED_PTR<std::vector<NodeDiscoveryInfo> >&)>& handler,
        int32_t timeout) RR_OVERRIDE
    {
        RR_UNUSED(schemes);
        RR_UNUSED(handler);
        RR_UNUSED(timeout);
        throw InvalidOperationException("Discovery not available");
    }
};

struct NcUpdateDetectedNodesHandler
{
    RR_SHARED_PTR<AutoResetEvent> ev;
    NcUpdateDetectedNodesHandler() : ev(RR_MAKE_SHARED<AutoResetEvent>()) {}
    void operator()() { ev->Set(); }
};

// UpdateDetectedNodes must complete when a transport's AsyncGetDetectedNodes throws (no timeout given)
TEST(RobotRaconteurMisc, UpdateDetectedNodesTransportThrows)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->RegisterTransport(RR_MAKE_SHARED<NcDiscoveryThrowTransport>(node));

    NcUpdateDetectedNodesHandler h;
    std::vector<std::string> schemes;
    schemes.push_back("rr+ncdiscthrow");
    node->AsyncUpdateDetectedNodes(schemes, h, RR_TIMEOUT_INFINITE);
    EXPECT_TRUE(h.ev->WaitOne(5000));
    node->Shutdown();
}

// SubscribeService / UpdateServiceURL must reject URLs that point to different services
TEST(RobotRaconteurMisc, SubscribeServiceUrlsDifferentServices)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();

    std::vector<std::string> urls;
    urls.push_back("rr+tcp://localhost:1/?nodename=test_node&service=service1");
    urls.push_back("rr+tcp://localhost:1/?nodename=test_node&service=service2");
    RR_SHARED_PTR<ServiceSubscription> sub1;
    EXPECT_THROW(sub1 = node->SubscribeService(urls), InvalidArgumentException);
    if (sub1)
        sub1->Close();

    std::vector<std::string> urls2;
    urls2.push_back("rr+tcp://localhost:1/?nodename=test_node&service=service1");
    RR_SHARED_PTR<ServiceSubscription> sub2 = node->SubscribeService(urls2);
    EXPECT_THROW(sub2->UpdateServiceURL(urls), InvalidArgumentException);
    sub2->Close();

    node->Shutdown();
}

class NcNotAStubObject : public RRObject
{
  public:
    RR_OVIRTUAL std::string RRType() RR_OVERRIDE { return "test::NcNotAStubObject"; }
};

// ClaimClient / ReleaseClient with an object that is not a service stub must throw, not dereference null
TEST(RobotRaconteurMisc, SubscriptionClaimClientNotAStub)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();

    RR_SHARED_PTR<ServiceSubscription> sub =
        node->SubscribeService("rr+tcp://localhost:1/?nodename=test_node&service=service1");
    RR_SHARED_PTR<RRObject> obj = RR_MAKE_SHARED<NcNotAStubObject>();
    EXPECT_THROW(sub->ClaimClient(obj), InvalidArgumentException);
    EXPECT_THROW(sub->ReleaseClient(obj), InvalidArgumentException);
    sub->Close();

    node->Shutdown();
}

#ifdef BOOST_ASIO_HAS_LOCAL_SOCKETS
// Recording to a LocalMessageTap after Close() must not crash
TEST(RobotRaconteurMisc, LocalMessageTapRecordAfterClose)
{
    std::string tap_name = "rrtest" + NodeID::NewUniqueID().ToString("N").substr(0, 12);
    RR_SHARED_PTR<LocalMessageTap> tap = RR_MAKE_SHARED<LocalMessageTap>(tap_name);
    ASSERT_NO_THROW(tap->Open());

    NodeDirectories dirs = NodeDirectoriesUtil::GetDefaultNodeDirectories();
    boost::filesystem::path sock_path = dirs.user_run_dir / "tap" / "all" / (tap_name + ".sock");

    RR_BOOST_ASIO_IO_CONTEXT io;
    boost::asio::local::stream_protocol::socket reader(io);
    ASSERT_NO_THROW(reader.connect(boost::asio::local::stream_protocol::endpoint(sock_path.string())));

    RR_INTRUSIVE_PTR<Message> m = CreateMessage();
    m->header = CreateMessageHeader();
    m->entries.push_back(CreateMessageEntry(MessageEntryType_ConnectionTest, ""));

    // Record until the tap has accepted the reader and data arrives
    size_t available = 0;
    for (size_t i = 0; i < 100 && available == 0; i++)
    {
        tap->RecordMessage(m);
        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
        available = reader.available();
    }
    ASSERT_GT(available, 0u);
    // Let the pending write complete
    boost::this_thread::sleep(boost::posix_time::milliseconds(200));

    tap->Close();

    tap->RecordMessage(m);
    tap->RecordMessage(m);

    boost::system::error_code ec;
    reader.close(ec);
}
#endif

TEST(RobotRaconteurMisc, PasswordVerifiedClientWithoutTransport)
{
    // testpass1
    std::string authdata = "testuser1 0b91dec4fe98266a03b136b59219d0d6 objectlock\n";
    RR_SHARED_PTR<PasswordFileUserAuthenticator> p = RR_MAKE_SHARED<PasswordFileUserAuthenticator>(authdata, true);
    std::map<std::string, RR_INTRUSIVE_PTR<RRValue> > credentials;
    credentials.insert(std::make_pair("password", stringToRRArray("testpass1")));
    // With require_verified_client, a missing transport connection must not skip the verification
    EXPECT_THROW(p->AuthenticateUser("testuser1", credentials, RR_SHARED_PTR<ServerContext>(),
                                     RR_SHARED_PTR<ITransportConnection>()),
                 AuthenticationException);
}

namespace
{
class WireUnicastReceiverPeekTest : public WireUnicastReceiver<int32_t>
{
  public:
    WireUnicastReceiverPeekTest() { node = RobotRaconteurNode::sp(); }
    int32_t Peek() { return ClientPeekOutValue(); }
    void Poke(int32_t v) { ClientPokeOutValue(v, TimeSpec(), 1); }
};
} // namespace

TEST(RobotRaconteurMisc, WireUnicastReceiverPeekLifespan)
{
    RR_SHARED_PTR<WireUnicastReceiverPeekTest> r = RR_MAKE_SHARED<WireUnicastReceiverPeekTest>();
    EXPECT_THROW(r->Peek(), ValueNotSetException);
    r->Poke(5);
    EXPECT_EQ(r->Peek(), 5);
    r->SetInValueLifespan(1);
    boost::this_thread::sleep(boost::posix_time::milliseconds(20));
    TimeSpec ts;
    uint32_t ep = 0;
    EXPECT_THROW(r->GetInValue(ts, ep), ValueNotSetException);
    // A client peek must apply the same lifespan as GetInValue
    EXPECT_THROW(r->Peek(), ValueNotSetException);
}

// Client transport whose connection attempts fail right away (handler posted with an error)
class NcFastFailClientTransport : public Transport
{
  public:
    NcFastFailClientTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node) : Transport(node) {}
    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE { RR_UNUSED(endpoint); }
    RR_OVIRTUAL bool IsClient() const RR_OVERRIDE { return true; }
    RR_OVIRTUAL bool IsServer() const RR_OVERRIDE { return false; }
    RR_OVIRTUAL std::string GetUrlSchemeString() const RR_OVERRIDE { return "rr+ncfastfail"; }
    RR_OVIRTUAL std::vector<std::string> GetServerListenUrls() RR_OVERRIDE { return std::vector<std::string>(); }
    RR_OVIRTUAL bool CanConnectService(boost::string_ref url) RR_OVERRIDE
    {
        return boost::starts_with(url, "rr+ncfastfail://");
    }
    RR_OVIRTUAL RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE
    {
        RR_UNUSED(url);
        RR_UNUSED(e);
        throw ConnectionException("Connection refused");
    }
    RR_OVIRTUAL void AsyncCreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(url);
        RR_UNUSED(e);
        RR_SHARED_PTR<RobotRaconteurException> err = RR_MAKE_SHARED<ConnectionException>("Connection refused");
        RobotRaconteurNode::TryPostToThreadPool(node, boost::bind(handler, RR_SHARED_PTR<ITransportConnection>(), err),
                                                true);
    }
    RR_OVIRTUAL void CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE { RR_UNUSED(e); }
    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE
    {
        RR_UNUSED(m);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(m);
        RR_UNUSED(handler);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE { RR_UNUSED(m); }
    RR_OVIRTUAL void Close() RR_OVERRIDE {}
};

struct NcFastFailResult
{
    boost::mutex lock;
    boost::condition_variable cv;
    bool done;
    RR_SHARED_PTR<RobotRaconteurException> err;
    NcFastFailResult() : done(false) {}
};

static void NcFastFailHandler(const RR_SHARED_PTR<NcFastFailResult>& res, const RR_SHARED_PTR<RRObject>& obj,
                              const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    RR_UNUSED(obj);
    boost::mutex::scoped_lock lock(res->lock);
    res->done = true;
    res->err = err;
    res->cv.notify_all();
}

// When every candidate URL fails before the connector finishes starting them, the connect must still complete
// with the candidate's error, not wait for the connect timeout (or forever with RR_TIMEOUT_INFINITE).
// The failure is a race, so repeat the connect many times; each must complete well before the 5 s timeout.
TEST(RobotRaconteurMisc, ConnectorFastCandidateFailureCompletes)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    RR_SHARED_PTR<NcFastFailClientTransport> t = RR_MAKE_SHARED<NcFastFailClientTransport>(node);
    node->RegisterTransport(t);

    boost::function<void(const RR_SHARED_PTR<ClientContext>&, ClientServiceListenerEventType,
                         const RR_SHARED_PTR<void>&)>
        listener;
    int late = 0;
    for (int i = 0; i < 200; i++)
    {
        RR_SHARED_PTR<NcFastFailResult> res = RR_MAKE_SHARED<NcFastFailResult>();
        node->AsyncConnectService(
            "rr+ncfastfail://localhost/?nodename=test&service=test", "",
            RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(), listener, "",
            boost::bind(&NcFastFailHandler, res, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)), 5000);
        boost::mutex::scoped_lock lock(res->lock);
        boost::system_time deadline = boost::get_system_time() + boost::posix_time::seconds(2);
        while (!res->done)
        {
            if (!res->cv.timed_wait(lock, deadline))
                break;
        }
        if (!res->done)
        {
            late++;
            // let the connect timer finish this attempt before the next one
            deadline = boost::get_system_time() + boost::posix_time::seconds(10);
            while (!res->done)
            {
                if (!res->cv.timed_wait(lock, deadline))
                    break;
            }
        }
        else
        {
            // The message may be "Connection timed out": handle_error resets the connect timer, and the cancelled
            // timer's callback can complete the connect first (it ignores the cancellation by design)
            ASSERT_TRUE(res->err);
        }
        if (late > 0)
            break;
    }
    EXPECT_EQ(late, 0);
    node->Shutdown();
}

// Transport connection that fails as soon as a client tries to use it, so ClientContext::AsyncConnectService
// throws after the transport reported the connection as made
class CapabilityThrowConnection : public ITransportConnection
{
  public:
    RR_WEAK_PTR<RobotRaconteurNode> node;
    RR_WEAK_PTR<Transport> transport;
    CapabilityThrowConnection(const RR_SHARED_PTR<RobotRaconteurNode>& node, const RR_SHARED_PTR<Transport>& t)
        : node(node), transport(t)
    {}
    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE
    {
        RR_UNUSED(m);
        throw ConnectionException("Connection lost");
    }
    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(m);
        RR_UNUSED(handler);
        throw ConnectionException("Connection lost");
    }
    RR_OVIRTUAL void Close() RR_OVERRIDE {}
    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE
    {
        RR_UNUSED(endpoint);
        throw ConnectionException("Connection lost");
    }
    RR_OVIRTUAL uint32_t GetLocalEndpoint() RR_OVERRIDE { return 0; }
    RR_OVIRTUAL uint32_t GetRemoteEndpoint() RR_OVERRIDE { return 0; }
    RR_OVIRTUAL NodeID GetRemoteNodeID() RR_OVERRIDE { return NodeID(); }
    RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteurNode> GetNode() RR_OVERRIDE { return node.lock(); }
    RR_OVIRTUAL bool CheckCapabilityActive(uint32_t flag) RR_OVERRIDE
    {
        RR_UNUSED(flag);
        throw ConnectionException("Connection lost");
    }
    RR_OVIRTUAL RR_SHARED_PTR<Transport> GetTransport() RR_OVERRIDE { return transport.lock(); }
};

// Client transport that reports a connection (from the thread pool) that fails on first use
class CapabilityThrowClientTransport : public Transport,
                                       public RR_ENABLE_SHARED_FROM_THIS<CapabilityThrowClientTransport>
{
  public:
    CapabilityThrowClientTransport(const RR_SHARED_PTR<RobotRaconteurNode>& node) : Transport(node) {}
    RR_OVIRTUAL void CheckConnection(uint32_t endpoint) RR_OVERRIDE { RR_UNUSED(endpoint); }
    RR_OVIRTUAL bool IsClient() const RR_OVERRIDE { return true; }
    RR_OVIRTUAL bool IsServer() const RR_OVERRIDE { return false; }
    RR_OVIRTUAL std::string GetUrlSchemeString() const RR_OVERRIDE { return "rr+capthrow"; }
    RR_OVIRTUAL std::vector<std::string> GetServerListenUrls() RR_OVERRIDE { return std::vector<std::string>(); }
    RR_OVIRTUAL bool CanConnectService(boost::string_ref url) RR_OVERRIDE
    {
        return boost::starts_with(url, "rr+capthrow://");
    }
    RR_OVIRTUAL RR_SHARED_PTR<ITransportConnection> CreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE
    {
        RR_UNUSED(url);
        RR_UNUSED(e);
        throw ConnectionException("Not supported");
    }
    RR_OVIRTUAL void AsyncCreateTransportConnection(
        boost::string_ref url, const RR_SHARED_PTR<Endpoint>& e,
        boost::function<void(const RR_SHARED_PTR<ITransportConnection>&,
                             const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(url);
        RR_UNUSED(e);
        RR_SHARED_PTR<RobotRaconteurNode> n = GetNode();
        RR_SHARED_PTR<ITransportConnection> c = RR_MAKE_SHARED<CapabilityThrowConnection>(n, shared_from_this());
        RobotRaconteurNode::TryPostToThreadPool(n, boost::bind(handler, c, RR_SHARED_PTR<RobotRaconteurException>()));
    }
    RR_OVIRTUAL void CloseTransportConnection(const RR_SHARED_PTR<Endpoint>& e) RR_OVERRIDE { RR_UNUSED(e); }
    RR_OVIRTUAL void SendMessage(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE
    {
        RR_UNUSED(m);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void AsyncSendMessage(
        const RR_INTRUSIVE_PTR<Message>& m,
        const boost::function<void(const RR_SHARED_PTR<RobotRaconteurException>&)>& handler) RR_OVERRIDE
    {
        RR_UNUSED(m);
        RR_UNUSED(handler);
        throw ConnectionException("Not connected");
    }
    RR_OVIRTUAL void MessageReceived(const RR_INTRUSIVE_PTR<Message>& m) RR_OVERRIDE { RR_UNUSED(m); }
};

struct CapabilityThrowConnectResult
{
    boost::mutex lock;
    boost::condition_variable cv;
    bool done;
    RR_SHARED_PTR<RobotRaconteurException> err;
    CapabilityThrowConnectResult() : done(false) {}
};

static void CapabilityThrowConnectHandler(const RR_SHARED_PTR<CapabilityThrowConnectResult>& res,
                                          const RR_SHARED_PTR<RRObject>& obj,
                                          const RR_SHARED_PTR<RobotRaconteurException>& err)
{
    RR_UNUSED(obj);
    boost::mutex::scoped_lock lock(res->lock);
    res->done = true;
    res->err = err;
    res->cv.notify_all();
}

// AsyncConnectService throwing after the transport connected must report the error and leave the connector's
// connecting_lock consistent (connected_transport's catch blocks unlocked the mutex behind its scoped_lock, so it
// was unlocked twice; ThreadSanitizer reports "unlock of an unlocked mutex")
TEST(RobotRaconteurMisc, ConnectorServiceConnectThrows)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->RegisterTransport(RR_MAKE_SHARED<CapabilityThrowClientTransport>(node));

    // Short connect timeout: without the CORE-GO-G1 connector fix, an error that arrives while connect2 still
    // holds its own key is only reported when the connect timer fires
    for (int i = 0; i < 3; i++)
    {
        RR_SHARED_PTR<CapabilityThrowConnectResult> res = RR_MAKE_SHARED<CapabilityThrowConnectResult>();
        boost::function<void(const RR_SHARED_PTR<ClientContext>&, ClientServiceListenerEventType,
                             const RR_SHARED_PTR<void>&)>
            listener;
        node->AsyncConnectService(
            "rr+capthrow://localhost/?nodename=test&service=test", "", RR_INTRUSIVE_PTR<RRMap<std::string, RRValue> >(),
            listener, "",
            boost::bind(&CapabilityThrowConnectHandler, res, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)),
            2000);

        boost::mutex::scoped_lock lock(res->lock);
        boost::system_time deadline = boost::get_system_time() + boost::posix_time::seconds(10);
        while (!res->done)
        {
            if (!res->cv.timed_wait(lock, deadline))
                break;
        }
        ASSERT_TRUE(res->done) << "Connect handler not called";
        EXPECT_TRUE(res->err);
    }

    node->Shutdown();
}

static void AutoResetEventNegativeTimeout_set_after(AutoResetEvent* ev, int ms)
{
    boost::this_thread::sleep(boost::posix_time::milliseconds(ms));
    ev->Set();
}

TEST(RobotRaconteurMisc, AutoResetEventNegativeTimeoutWaits)
{
    // WaitOne(-1) waits until Set(), like WaitOne() (on POSIX it returned false at once)
    AutoResetEvent ev;
    boost::thread t(boost::bind(&AutoResetEventNegativeTimeout_set_after, &ev, 200));
    boost::posix_time::ptime t1 = boost::posix_time::microsec_clock::universal_time();
    EXPECT_TRUE(ev.WaitOne(-1));
    EXPECT_GE((boost::posix_time::microsec_clock::universal_time() - t1).total_milliseconds(), 150);
    t.join();
}

// SetLogLevelFromString reports invalid input instead of silently keeping the current level
TEST(RobotRaconteurMisc, SetLogLevelFromStringInvalid)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    EXPECT_EQ(node->SetLogLevelFromString("INFO"), RobotRaconteur_LogLevel_Info);
    EXPECT_THROW(node->SetLogLevelFromString("NOT_A_LEVEL"), InvalidArgumentException);
    EXPECT_EQ(node->GetLogLevel(), RobotRaconteur_LogLevel_Info);
    node->Shutdown();
}

// Announces for a known node must not grow its candidate URL list without bound
TEST(RobotRaconteurMisc, DiscoveryURLCap)
{
    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    NodeID id = NodeID::NewUniqueID();
    for (int i = 0; i < 500; i++)
    {
        NodeDiscoveryInfo info;
        info.NodeID = id;
        info.NodeName = "discovery_url_cap";
        NodeDiscoveryInfoURL u;
        u.URL = "rr+tcp://127.0.0.1:" + boost::lexical_cast<std::string>(10000 + i) + "/?nodeid=" + id.ToString("D") +
                "&service=RobotRaconteurServiceIndex";
        u.LastAnnounceTime = node->NowNodeTime();
        info.URLs.push_back(u);
        node->NodeDetected(info);
    }
    std::vector<NodeDiscoveryInfo> nodes = node->GetDetectedNodes();
    bool found = false;
    BOOST_FOREACH (NodeDiscoveryInfo& n, nodes)
    {
        if (n.NodeID == id)
        {
            found = true;
            EXPECT_GE(n.URLs.size(), 1u);
            EXPECT_LE(n.URLs.size(), 64u);
        }
    }
    EXPECT_TRUE(found);
    node->Shutdown();
}

#if !defined(ROBOTRACONTEUR_WINDOWS) && !defined(ROBOTRACONTEUR_APPLE)
// Private (non-public) LocalTransport server sockets are owner-only
TEST(RobotRaconteurMisc, LocalTransportPrivateSocketMode)
{
    boost::filesystem::path base = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::create_directories(base);

    NodeDirectories d;
    d.system_data_dir = base / "sys_data";
    d.system_config_dir = base / "sys_config";
    d.system_state_dir = base / "sys_state";
    d.system_cache_dir = base / "sys_cache";
    d.system_run_dir = base / "sys_run";
    d.user_data_dir = base / "user_data";
    d.user_config_dir = base / "user_config";
    d.user_state_dir = base / "user_state";
    d.user_cache_dir = base / "user_cache";
    d.user_run_dir = base / "user_run";

    RR_SHARED_PTR<RobotRaconteurNode> node = RR_MAKE_SHARED<RobotRaconteurNode>();
    node->Init();
    node->SetNodeDirectories(d);
    RR_SHARED_PTR<LocalTransport> t = RR_MAKE_SHARED<LocalTransport>(node);
    node->RegisterTransport(t);
    t->StartServerAsNodeName("bughunt_private_socket");

    int n_sock = 0;
    for (boost::filesystem::directory_iterator it(base / "user_run" / "socket");
         it != boost::filesystem::directory_iterator(); ++it)
    {
        if (it->path().extension() != ".sock")
            continue;
        n_sock++;
        struct stat st = {};
        ASSERT_EQ(lstat(it->path().c_str(), &st), 0);
        EXPECT_EQ(st.st_mode & 0777, 0700u) << it->path();
    }
    EXPECT_EQ(n_sock, 1);

    node->Shutdown();
    boost::filesystem::remove_all(base);
}
#endif

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    int ret = RUN_ALL_TESTS();

    return ret;
}
