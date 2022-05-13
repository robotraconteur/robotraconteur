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

#include "OpenSSLAuthContext.h"
#include <openssl/pkcs12.h>
#include <boost/algorithm/string.hpp>
#include <vector>

#include <fstream>
#include "RobotRaconteurNodeRootCA.h"
#include <boost/shared_array.hpp>

#if BOOST_ASIO_VERSION >= 101009
#define ROBOTRACONTEUR_BOOST_ASIO_TLS_METHOD boost::asio::ssl::context::tls
#else
#define ROBOTRACONTEUR_BOOST_ASIO_TLS_METHOD boost::asio::ssl::context::tlsv11
#endif

namespace RobotRaconteur
{
namespace detail
{
namespace OpenSSLSupport
{
struct bio_cleanup
{
    BIO* p;
    ~bio_cleanup()
    {
        if (p)
            ::BIO_free(p);
    }
};

struct x509_cleanup
{
    X509* p;
    ~x509_cleanup()
    {
        if (p)
            ::X509_free(p);
    }
};

struct x509_stack_cleanup
{
    STACK_OF(X509) * p;
    ~x509_stack_cleanup()
    {
        if (p)
            ::sk_X509_pop_free(p, X509_free);
    }
};

BIO* make_buffer_bio(const boost::asio::const_buffer& b)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    return ::BIO_new_mem_buf(const_cast<void*>(boost::asio::buffer_cast<const void*>(b)),
                             boost::numeric_cast<int>(boost::asio::buffer_size(b)));
}

void add_certificate_authority_x509(const boost::shared_ptr<boost::asio::ssl::context>& context,
                                    boost::asio::const_buffer& buf)
{
    ::ERR_clear_error();

    bio_cleanup bio = {make_buffer_bio(buf)};
    if (bio.p)
    {
        x509_cleanup cert = {::d2i_X509_bio(bio.p, 0)};
        if (cert.p)
        {
            if (X509_STORE* store = ::SSL_CTX_get_cert_store(context->native_handle()))
            {
                if (::X509_STORE_add_cert(store, cert.p) == 1)
                {

                    return;
                }
            }
        }
    }

    throw InternalErrorException("Internal error");
}

bool verify_callback(bool preverified, boost::asio::ssl::verify_context& ctx)
{
    RR_UNUSED(preverified);
    int cert_error = X509_STORE_CTX_get_error(ctx.native_handle());

    if (cert_error && cert_error != X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION)
    {
        // std::cout << "Certificate error: " << cert_error << std::endl;
        return false;
    }

    int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());
    X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    if (!cert)
    {
        // std::cout << "Certificate error: null?" << std::endl;
        return false;
    }

    /*if (depth==0)
    {
        char buf[256];
        memset(buf,0,256);


        X509_NAME_oneline(X509_get_subject_name(cert), buf, 256);

        std::string buf2(buf);
        //std::cout << "Certificate name: " << buf2 << " depth=" << depth << std::endl;


    }*/

    X509_NAME* sub_name = X509_get_subject_name(cert);
    X509_NAME* iss_name = X509_get_issuer_name(cert);

    bool isroot = false;

    if (X509_NAME_cmp(sub_name, iss_name) == 0)
    {

        // if(X509_cmp(cert,root_cert.get())==0)
        isroot = true;
        // else
        //	return false;
    }

    {

        int ext_count = X509_get_ext_count(cert);

        bool found_thisoid = false;

        for (int i = 0; i < ext_count; i++)
        {
            X509_EXTENSION* e = X509_get_ext(cert, i);
            if (!e)
                return false;
            if (::X509_EXTENSION_get_critical(e))
            {
                ASN1_OBJECT* obj = ::X509_EXTENSION_get_object(e);
                if (!obj)
                    return false;
                boost::array<char, 64> buf = {};

                OBJ_obj2txt(buf.data(), 64, obj, 1);
                std::string oid(buf.data());
                if (oid == "2.5.29.15" || oid == "2.5.29.14" || oid == "2.5.29.19" || oid == "2.5.29.35" ||
                    oid == "2.5.29.32")
                {
                    continue;
                }

                std::string thisoid;
                if (!isroot)
                {
                    thisoid = depth == 0 ? "1.3.6.1.4.1.45455.1.1.3.3" : "1.3.6.1.4.1.45455.1.1.3.2";
                }
                else
                {
                    thisoid = "1.3.6.1.4.1.45455.1.1.3.1";
                }

                if (oid == thisoid)
                {
                    found_thisoid = true;
                    continue;
                }

                // Error, unknown extension
                return false;
            }
        }

        // If we don't find the oid for this type of cert return false;
        if (!found_thisoid)
        {

            return false;
        }

        // std::cout << "Not root" << std::endl;
    }

    return true;
}

} // namespace OpenSSLSupport

OpenSSLAuthContext::OpenSSLAuthContext(const NodeID& nodeid) { this->nodeid = nodeid; }

#define UNUSED(expr)                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        (void)(expr);                                                                                                  \
    } while (0)

static boost::shared_array<uint8_t> unmask_certificate(const uint8_t* masked_cert, size_t cert_len)
{
    boost::shared_array<uint8_t> b2(new uint8_t[cert_len]);

    // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
    const uint8_t mask1[] = {0xbb, 0x1b, 0x38, 0x3b};
    const uint8_t mask2[] = {0x99, 0x84, 0xe2, 0xe7};
    const uint8_t mask3[] = {0xe3, 0x51, 0xb5, 0x7};
    const uint8_t mask4[] = {0x42, 0xf7, 0x96, 0xc2};
    const uint8_t mask5[] = {0x22, 0x97, 0x54, 0xd9};
    const uint8_t mask6[] = {0x30, 0x26, 0x90, 0xa1};
    const uint8_t mask7[] = {0x45, 0xec, 0x81, 0x42};
    const uint8_t mask8[] = {0x3d, 0xbd, 0x8e, 0x2b};
    // NOLINTEND(cppcoreguidelines-avoid-c-arrays)

    for (size_t i = 0; i < cert_len; i++)
    {
        size_t j = i % 16;
        if (j < 4)
            b2.get()[i] = masked_cert[i] ^ mask2[j];
        if (j >= 4 && j < 8)
            b2.get()[i] = masked_cert[i] ^ mask3[j - 4];
        if (j >= 8 && j < 10)
            b2.get()[i] = masked_cert[i] ^ mask1[j - 8];
        if (j >= 10 && j < 12)
            b2.get()[i] = masked_cert[i] ^ mask6[j - 9];
        if (j >= 12 && j < 16)
            b2.get()[i] = masked_cert[i] ^ mask7[j - 12];
    }

    return b2;
}

void OpenSSLAuthContext::InitCA(const boost::shared_ptr<boost::asio::ssl::context>& context)
{
    context->set_default_verify_paths();
    X509_STORE* store = SSL_CTX_get_cert_store(context->native_handle());
    X509_LOOKUP* lookup_dir = X509_STORE_add_lookup(store, X509_LOOKUP_hash_dir());
    X509_LOOKUP* lookup_file = X509_STORE_add_lookup(store, X509_LOOKUP_file());

    const char* env_cert_file = getenv("ROBOTRACONTEUR_SSL_CERT_FILE");
    if (env_cert_file != NULL)
    {
        X509_LOOKUP_load_file(lookup_file, env_cert_file, X509_FILETYPE_PEM);
    }
    const char* env_cert_dir = getenv("ROBOTRACONTEUR_SSL_CERT_DIR");
    if (env_cert_dir != NULL)
    {
        X509_LOOKUP_add_dir(lookup_dir, env_cert_dir, X509_FILETYPE_PEM);
    }

    // X509_STORE_add_cert

#ifdef ROBOTRACONTEUR_APPLE
    X509_LOOKUP_add_dir(lookup_dir, "/Library/RobotRaconteur/ca-certificates", X509_FILETYPE_PEM);
    X509_LOOKUP_add_dir(lookup_dir, "/System/Library/RobotRaconteur/ca-certificates", X509_FILETYPE_PEM);
#endif
    bool use_root_cert_2015 = true;
    bool use_root_cert_2020 = true;

    char* default_env = getenv("ROBOTRACONTEUR_NO_DEFAULT_ROOT_CERT");
    char* default_env_2015 = getenv("ROBOTRACONTEUR_NO_DEFAULT_ROOT_CERT_2015");
    char* default_env_2020 = getenv("ROBOTRACONTEUR_NO_DEFAULT_ROOT_CERT_2020");

    if (default_env != NULL)
    {
        std::string default_env_str(default_env);
        boost::trim(default_env_str);
        boost::to_lower(default_env_str);
        if (default_env_str == "true" || default_env_str == "1")
        {
            use_root_cert_2015 = false;
            use_root_cert_2020 = false;
        }
    }

    if (default_env_2015 != NULL && use_root_cert_2015)
    {
        std::string default_env_str(default_env_2015);
        boost::trim(default_env_str);
        boost::to_lower(default_env_str);
        if (default_env_str == "true" || default_env_str == "1")
        {
            use_root_cert_2015 = false;
        }
    }

    if (default_env_2020 != NULL && use_root_cert_2020)
    {
        std::string default_env_str(default_env_2020);
        boost::trim(default_env_str);
        boost::to_lower(default_env_str);
        if (default_env_str == "true" || default_env_str == "1")
        {
            use_root_cert_2020 = false;
        }
    }

    if (use_root_cert_2015)
    {
        // The Root Certificate is masked to prevent program byte level tampering
        boost::shared_array<uint8_t> b2 =
            unmask_certificate(ROBOTRACONTEUR_NODE_ROOT_CA_2015, sizeof(ROBOTRACONTEUR_NODE_ROOT_CA_2015));

        const uint8_t* b3 = b2.get();
        X509* root_cert = d2i_X509(NULL, &b3, sizeof(ROBOTRACONTEUR_NODE_ROOT_CA_2015));

        if (root_cert != NULL)
        {
            X509_STORE_add_cert(store, root_cert);
        }
    }

    if (use_root_cert_2020)
    {
        // The Root Certificate is masked to prevent program byte level tampering
        boost::shared_array<uint8_t> b2 =
            unmask_certificate(ROBOTRACONTEUR_NODE_ROOT_CA_2020, sizeof(ROBOTRACONTEUR_NODE_ROOT_CA_2020));

        const uint8_t* b3 = b2.get();
        X509* root_cert = d2i_X509(NULL, &b3, sizeof(ROBOTRACONTEUR_NODE_ROOT_CA_2020));

        if (root_cert != NULL)
        {
            X509_STORE_add_cert(store, root_cert);
        }
    }
}

void OpenSSLAuthContext::LoadPKCS12FromBuffer(boost::asio::mutable_buffer& buf)
{
    RR_UNUSED(buf);
    throw NotImplementedException("Not implemented");
}

void OpenSSLAuthContext::LoadPKCS12FromFile(boost::string_ref fname)
{
#ifndef OPENSSL_NO_STDIO
    boost::mutex::scoped_lock lock(mylock);
    if (server_context)
        throw InvalidOperationException("Certificate already loaded");

    FILE* fp = NULL;
    EVP_PKEY* pkey = NULL;
    X509* cert = NULL;
    STACK_OF(X509)* ca = NULL;
    PKCS12* p12 = NULL;
    int i = 0;
    std::string fname1 = fname.to_string();
    if (!(fp = fopen(fname1.c_str(), "rb"))) // NOLINT(cppcoreguidelines-owning-memory)
    {
        throw ResourceNotFoundException("Could not load certificate file");
    }

    p12 = d2i_PKCS12_fp(fp, NULL);
    fclose(fp); // NOLINT(cppcoreguidelines-owning-memory)
    if (!p12)
    {
        throw ResourceNotFoundException("Could not load certificate file");
    }

    if (!PKCS12_parse(p12, "", &pkey, &cert, &ca))
    {
        throw ResourceNotFoundException("Could not load certificate file");
    }
    PKCS12_free(p12);

    if (!pkey || !cert)
    {
        if (ca)
            sk_X509_pop_free(ca, X509_free);
        if (cert)
            X509_free(cert);
        if (pkey)
            EVP_PKEY_free(pkey);
        throw ResourceNotFoundException("Could not load certificate file");
    }

    server_context = RR_MAKE_SHARED<boost::asio::ssl::context>(ROBOTRACONTEUR_BOOST_ASIO_TLS_METHOD);
    server_context->set_options(boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3 |
                                boost::asio::ssl::context::no_compression);

    InitCA(server_context);

    // server_context->set_verify_mode(boost::asio::ssl::context::verify_peer |
    // boost::asio::ssl::context::verify_client_once);
    server_context->set_verify_callback(
        boost::bind(&detail::OpenSSLSupport::verify_callback, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));

    // OpenSSLSupport::add_certificate_authority_x509(server_context,boost::asio::buffer(ROBOTRACONTEUR_NODE_CERTIFICATE_AUTHORITY_ROOT,sizeof(ROBOTRACONTEUR_NODE_CERTIFICATE_AUTHORITY_ROOT)));

    bool error = false;

    if (SSL_CTX_use_PrivateKey(server_context->native_handle(), pkey) != 1)
        error = true;
    if (SSL_CTX_use_certificate(server_context->native_handle(), cert) != 1)
        error = true;

    for (i = 0; i < sk_X509_num(ca); i++)
    {
        X509* stack_item = sk_X509_value(ca, i);
        if (SSL_CTX_add_extra_chain_cert(server_context->native_handle(), stack_item) != 1)
        {
            error = true;
            break;
        }
    }

    if (!client_context)
    {
        client_context = RR_MAKE_SHARED<boost::asio::ssl::context>(ROBOTRACONTEUR_BOOST_ASIO_TLS_METHOD);
        client_context->set_options(boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3 |
                                    boost::asio::ssl::context::no_compression);
        client_context->set_verify_mode(boost::asio::ssl::context::verify_peer);
        client_context->set_verify_callback(boost::bind(&detail::OpenSSLSupport::verify_callback,
                                                        RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));

        // OpenSSLSupport::add_certificate_authority_x509(client_context,boost::asio::buffer(ROBOTRACONTEUR_NODE_CERTIFICATE_AUTHORITY_ROOT,sizeof(ROBOTRACONTEUR_NODE_CERTIFICATE_AUTHORITY_ROOT)));

        InitCA(client_context);

        if (SSL_CTX_use_PrivateKey(client_context->native_handle(), (pkey)) != 1)
            error = true;
        if (SSL_CTX_use_certificate(client_context->native_handle(), X509_dup(cert)) != 1)
            error = true;

        for (i = 0; i < sk_X509_num(ca); i++)
        {
            X509* stack_item = X509_dup(sk_X509_value(ca, i));
            if (SSL_CTX_add_extra_chain_cert(client_context->native_handle(), stack_item) != 1)
            {
                error = true;
                break;
            }
        }
    }

    // SSL_CTX_set_client_CA_list(server_context->native_handle(),
    // SSL_load_client_CA_file("/usr/local/share/ca-certificates/Robot_Raconteur_Node_Certificate_Authority_Root.crt"));

    // if (ca) sk_X509_pop_free(ca, X509_free);
    // if (cert) X509_free(cert);
    // if (pkey) EVP_PKEY_free(pkey);

    // RR_SHARED_PTR<OpenSSLSupport::x509_stack_cleanup>p12_ca1=RR_MAKE_SHARED<OpenSSLSupport::x509_stack_cleanup>();
    // p12_ca1->p=ca;
    // p12_ca=p12_ca1;
    // p12_cert.reset(cert,X509_free);
    // p12_key.reset(pkey,EVP_PKEY_free);

    if (error)
        throw ResourceNotFoundException("Could not load certificate file");
#else
    throw ResourceNotFoundException("Could not load certificate file");
#endif
}

boost::shared_ptr<boost::asio::ssl::context> OpenSSLAuthContext::GetServerCredentials()
{
    boost::mutex::scoped_lock lock(mylock);
    if (!server_context)
        throw InvalidOperationException("Node certificate not loaded");
    return server_context;
}

boost::shared_ptr<boost::asio::ssl::context> OpenSSLAuthContext::GetClientCredentials()
{
    boost::mutex::scoped_lock lock(mylock);
    if (!client_context)
    {
        client_context = RR_MAKE_SHARED<boost::asio::ssl::context>(ROBOTRACONTEUR_BOOST_ASIO_TLS_METHOD);
        client_context->set_options(boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3 |
                                    boost::asio::ssl::context::no_compression);
        client_context->set_verify_mode(boost::asio::ssl::context::verify_peer);
        client_context->set_verify_callback(boost::bind(&detail::OpenSSLSupport::verify_callback,
                                                        RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2)));

        // OpenSSLSupport::add_certificate_authority_x509(client_context,boost::asio::buffer(ROBOTRACONTEUR_NODE_CERTIFICATE_AUTHORITY_ROOT,sizeof(ROBOTRACONTEUR_NODE_CERTIFICATE_AUTHORITY_ROOT)));

        InitCA(client_context);
    }

    return client_context;
}

bool OpenSSLAuthContext::IsCertificateLoaded()
{
    boost::mutex::scoped_lock lock(mylock);
    return server_context != NULL;
}

bool OpenSSLAuthContext::VerifyRemoteNodeCertificate(SSL* connection, const NodeID& remote_node)
{
    boost::mutex::scoped_lock lock(mylock);

    long res = SSL_get_verify_result(connection);
    if (res != X509_V_OK && res != X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION)
        return false;
    X509* cert = SSL_get_peer_certificate(connection);
    if (!cert)
        return false;

    boost::array<char, 256> buf = {};
    memset(buf.data(), 0, 256);

    X509_NAME_oneline(X509_get_subject_name(cert), buf.data(), 256);

    std::string buf2(buf.data());

    if (buf2 != "/CN=Robot Raconteur Node " + remote_node.ToString())
    {
        return false;
    }

    int ext_count = X509_get_ext_count(cert);

    bool found_thisoid = false;

    for (int i = 0; i < ext_count; i++)
    {
        X509_EXTENSION* e = X509_get_ext(cert, i);
        if (!e)
            return false;
        if (::X509_EXTENSION_get_critical(e))
        {
            ASN1_OBJECT* obj = ::X509_EXTENSION_get_object(e);
            if (!obj)
                return false;
            boost::array<char, 64> buf3 = {};

            OBJ_obj2txt(buf3.data(), 64, obj, 1);
            std::string oid(buf3.data());
            if (oid == "2.5.29.15" || oid == "2.5.29.14" || oid == "2.5.29.19" || oid == "2.5.29.35" ||
                oid == "2.5.29.32")
            {
                continue;
            }

            std::string thisoid = "1.3.6.1.4.1.45455.1.1.3.3";

            if (oid == thisoid)
            {
                found_thisoid = true;
                continue;
            }

            // Error, unknown extension
            return false;
        }
    }

    // If we don't find the oid for this type of cert return false;
    return found_thisoid;
}

} // namespace detail
} // namespace RobotRaconteur
