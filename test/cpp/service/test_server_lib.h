#ifndef ROBOTRACONTEUR_TEST_SERVER_LIB_H
#define ROBOTRACONTEUR_TEST_SERVER_LIB_H

#pragma once

#include <stdint.h>

#ifdef _WIN32
#ifdef ROBOTRACONTEUR_TEST_SERVER_LIB_EXPORTS
#define ROBOTRACONTEUR_TEST_SERVER_LIB_API __declspec(dllexport)
#elif ROBOTRACONTEUR_TEST_SERVER_LIB_IMPORTS
#define ROBOTRACONTEUR_TEST_SERVER_LIB_API __declspec(dllimport)
#else
#define ROBOTRACONTEUR_TEST_SERVER_LIB_API
#endif
#else
#define ROBOTRACONTEUR_TEST_SERVER_LIB_API
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    ROBOTRACONTEUR_TEST_SERVER_LIB_API int32_t robotraconteur_test_server_lib_start(const char* nodename,
                                                                                    uint32_t nodename_len);

    ROBOTRACONTEUR_TEST_SERVER_LIB_API int32_t robotraconteur_test_server_lib_get_service_url(const char* service,
                                                                                              uint32_t service_len,
                                                                                              char* url_out,
                                                                                              uint32_t* url_out_len);
    ROBOTRACONTEUR_TEST_SERVER_LIB_API int32_t robotraconteur_test_server_lib_get_service_url2(const char* service,
                                                                                               uint32_t service_len,
                                                                                               int8_t* url_out,
                                                                                               uint32_t* url_out_len);

    ROBOTRACONTEUR_TEST_SERVER_LIB_API int32_t robotraconteur_test_server_lib_shutdown();

#ifdef __cplusplus
}
#endif

#endif