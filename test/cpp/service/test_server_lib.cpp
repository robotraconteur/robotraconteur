#include "test_server_lib.h"
#include "service_test_utils.h"

using namespace RobotRaconteur;
using namespace RobotRaconteur::test;

RR_SHARED_PTR<TestServerNodeConfig> node_;

int32_t robotraconteur_test_server_lib_start(const char* nodename, uint32_t nodename_len)
{
    try
    {
        std::string nodename_s(nodename, nodename_len);
        node_ = RR_MAKE_SHARED<TestServerNodeConfig>(nodename_s);
        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "Robot Raconteur test server lib startup error: " << e.what() << std::endl;
        return -1;
    }
}

int32_t robotraconteur_test_server_lib_get_service_url2(const char* service, uint32_t service_len, int8_t* url_out,
                                                        uint32_t* url_out_len)
{
    return robotraconteur_test_server_lib_get_service_url(service, service_len, (char*)url_out, url_out_len);
}

int32_t robotraconteur_test_server_lib_get_service_url(const char* service, uint32_t service_len, char* url_out,
                                                       uint32_t* url_out_len)
{
    if (!node_)
    {
        std::cerr << "Robot Raconteur test server lib not initialized" << std::endl;
        return -1;
    }
    try
    {
        std::string service_s(service, service_len);
        std::string url = node_->GetServiceURL(service_s);
        if (*url_out_len < url.size())
        {
            std::cerr << "Robot Raconteur test server lib get service url buffer too small" << std::endl;
            return -1;
        }
        strcpy(url_out, url.c_str());
        *url_out_len = url.size();
        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "Robot Raconteur test server lib get service url error: " << e.what() << std::endl;
        return -1;
    }
}

int32_t robotraconteur_test_server_lib_shutdown()
{
    try
    {
        if (!node_)
        {
            return 0;
        }
        node_->Shutdown();
        node_.reset();
        return 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "Robot Raconteur test server lib shutdown error: " << e.what() << std::endl;
        return -1;
    }
}