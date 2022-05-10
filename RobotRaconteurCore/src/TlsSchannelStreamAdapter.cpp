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

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "TlsSchannelStreamAdapter.h"

#include <boost/bind/placeholders.hpp>
#include <boost/asio.hpp>
#include <boost/bind/protect.hpp>
#include <boost/shared_array.hpp>
#include <WinInet.h>
#include <boost/algorithm/string.hpp>

#include "RobotRaconteurNodeRootCA.h"

#ifdef ROBOTRACONTEUR_USE_SCHANNEL

namespace RobotRaconteur
{
namespace detail
{
static boost::shared_array<uint8_t> unmask_certificate(const uint8_t* masked_cert, size_t cert_len)
{
    boost::shared_array<uint8_t> b2(new uint8_t[cert_len]);

    const uint8_t mask1[] = {0xbb, 0x1b, 0x38, 0x3b};
    const uint8_t mask2[] = {0x99, 0x84, 0xe2, 0xe7};
    const uint8_t mask3[] = {0xe3, 0x51, 0xb5, 0x7};
    const uint8_t mask4[] = {0x42, 0xf7, 0x96, 0xc2};
    const uint8_t mask5[] = {0x22, 0x97, 0x54, 0xd9};
    const uint8_t mask6[] = {0x30, 0x26, 0x90, 0xa1};
    const uint8_t mask7[] = {0x45, 0xec, 0x81, 0x42};
    const uint8_t mask8[] = {0x3d, 0xbd, 0x8e, 0x2b};

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

TlsSchannelAsyncStreamAdapterContext::TlsSchannelAsyncStreamAdapterContext(const NodeID& nodeid)
{
    this->nodeid = nodeid;
    ZeroMemory(&activestore, sizeof(activestore));
    activecertificate = NULL;
    ZeroMemory(&server_credentials, sizeof(server_credentials));
    ZeroMemory(&client_credentials, sizeof(client_credentials));

    // The Root Certificate is masked to prevent program byte level tampering
    boost::shared_array<uint8_t> root_cert_2015_bytes =
        unmask_certificate(ROBOTRACONTEUR_NODE_ROOT_CA_2015, sizeof(ROBOTRACONTEUR_NODE_ROOT_CA_2015));
    boost::shared_array<uint8_t> root_cert_2020_bytes =
        unmask_certificate(ROBOTRACONTEUR_NODE_ROOT_CA_2020, sizeof(ROBOTRACONTEUR_NODE_ROOT_CA_2020));

    /*FILE* f = fopen("root.cer", "wb");
    fwrite(*b2.get(), 1, sizeof(ROBOTRACONTEUR_NODE_ROOT_CA), f);
    fclose(f);*/

    HCERTSTORE root = CertOpenStore(CERT_STORE_PROV_MEMORY, 0, NULL, CERT_STORE_CREATE_NEW_FLAG, &rootcertificate2015);
    if (!root)
        throw InternalErrorException("Internal error");
    stores.push_back(root);
    PCCERT_CONTEXT rootcert2015_1;
    BOOL r1 = CertAddEncodedCertificateToStore(root, X509_ASN_ENCODING, root_cert_2015_bytes.get(),
                                               sizeof(ROBOTRACONTEUR_NODE_ROOT_CA_2015), CERT_STORE_ADD_ALWAYS,
                                               &rootcert2015_1);
    if (!r1)
        throw InternalErrorException("Internal error");
    rootcertificate2015 = rootcert2015_1;
    PCCERT_CONTEXT rootcert2020_1;
    BOOL r1_2 = CertAddEncodedCertificateToStore(root, X509_ASN_ENCODING, root_cert_2020_bytes.get(),
                                                 sizeof(ROBOTRACONTEUR_NODE_ROOT_CA_2020), CERT_STORE_ADD_ALWAYS,
                                                 &rootcert2020_1);
    if (!r1_2)
        throw InternalErrorException("Internal error");
    rootcertificate2020 = rootcert2020_1;

    store = CertOpenStore(CERT_STORE_PROV_COLLECTION, 0, NULL, CERT_STORE_CREATE_NEW_FLAG, 0);
    if (!store)
        throw InternalErrorException("Internal error");
    BOOL r2 = CertAddStoreToCollection(store, root, CERT_PHYSICAL_STORE_ADD_ENABLE_FLAG, 0);
    if (!r2)
        throw InternalErrorException("Internal error");

    // PCCERT_CONTEXT rootcert2015 = CertFindCertificateInStore(root, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING,
    // rootcert2015_1, NULL); if (!rootcert2015) throw InternalErrorException("Internal error"); rootcertificate2015 =
    // rootcert2015;

    // PCCERT_CONTEXT rootcert2020 = CertFindCertificateInStore(root, X509_ASN_ENCODING, 0, CERT_FIND_EXISTING,
    // rootcert2020_1, NULL); if (!rootcert2020) throw InternalErrorException("Internal error"); rootcertificate2020 =
    // rootcert2020;

    char default_env[256];
    char default_env_2015[256];
    char default_env_2020[256];
    DWORD ret1 = ::GetEnvironmentVariable("ROBOTRACONTEUR_NO_DEFAULT_ROOT_CERT", default_env, sizeof(default_env));
    DWORD ret2 =
        ::GetEnvironmentVariable("ROBOTRACONTEUR_NO_DEFAULT_ROOT_CERT_2015", default_env_2015, sizeof(default_env));
    DWORD ret3 =
        ::GetEnvironmentVariable("ROBOTRACONTEUR_NO_DEFAULT_ROOT_CERT_2020", default_env_2020, sizeof(default_env));
    use_root_cert_2015 = true;
    use_root_cert_2020 = true;
    if (ret1 > 0)
    {
        std::string default_env_str(default_env, ret1);
        boost::trim(default_env_str);
        boost::to_lower(default_env_str);
        if (default_env_str == "true" || default_env_str == "1")
        {
            use_root_cert_2015 = false;
            use_root_cert_2020 = false;
        }
    }

    if (ret2 > 0 && use_root_cert_2015)
    {
        std::string default_env_str(default_env_2015, ret2);
        boost::trim(default_env_str);
        boost::to_lower(default_env_str);
        if (default_env_str == "true" || default_env_str == "1")
        {
            use_root_cert_2015 = false;
        }
    }

    if (ret3 > 0 && use_root_cert_2020)
    {
        std::string default_env_str(default_env_2020, ret3);
        boost::trim(default_env_str);
        boost::to_lower(default_env_str);
        if (default_env_str == "true" || default_env_str == "1")
        {
            use_root_cert_2020 = false;
        }
    }
}

TlsSchannelAsyncStreamAdapterContext::~TlsSchannelAsyncStreamAdapterContext()
{
    boost::mutex::scoped_lock lock(mylock);

    if (store)
    {
        CertCloseStore(store, 0);
    }

    if (activestore)
    {
        CertCloseStore(activestore, 0);
    }

    if (rootcertificate2015)
    {
        CertFreeCertificateContext(rootcertificate2015);
    }

    if (rootcertificate2020)
    {
        CertFreeCertificateContext(rootcertificate2020);
    }

    if (activecertificate)
    {
        CertFreeCertificateContext(activecertificate);
    }

    for (std::vector<HCERTSTORE>::iterator e = stores.begin(); e != stores.end(); e++)
    {
        CertCloseStore(*e, 0);
    }

    if (server_credentials.dwLower != 0 || server_credentials.dwUpper != 0)
    {
        FreeCredentialsHandle(&server_credentials);
    }

    if (client_credentials.dwLower != 0 || client_credentials.dwUpper != 0)
    {
        FreeCredentialsHandle(&client_credentials);
    }
}

void TlsSchannelAsyncStreamAdapterContext::LoadCertificateFromMyStore()
{
    boost::mutex::scoped_lock lock(mylock);

    if (activecertificate)
    {
        throw InvalidOperationException("Certificate already loaded");
    }

    HCERTSTORE store = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, NULL, CERT_SYSTEM_STORE_CURRENT_USER, L"My");
    if (!store)
        throw InternalErrorException("Internal error");

    std::string CN = "Robot Raconteur Node " + nodeid.ToString();

    PCCERT_CONTEXT pCertContext = NULL;

    pCertContext = CertFindCertificateInStore(store,                   // hCertStore
                                              X509_ASN_ENCODING,       // dwCertEncodingType
                                              0,                       // dwFindFlags
                                              CERT_FIND_SUBJECT_STR_A, // dwFindType
                                              CN.c_str(),              // *pvFindPara
                                              NULL);

    if (!pCertContext || activecertificate)
    {
        if (pCertContext)
        {
            CertFreeCertificateContext(pCertContext);
        }
        CertCloseStore(store, 0);
        throw ResourceNotFoundException("Could not load node certificate");
    }

    DWORD dType = CERT_X500_NAME_STR | CERT_NAME_STR_REVERSE_FLAG;
    char certname[1024];
    ZeroMemory(certname, sizeof(certname));
    if (FALSE == CertGetNameStringA(pCertContext, CERT_NAME_RDN_TYPE, 0, &dType, certname, sizeof(certname)))
    {
        CertFreeCertificateContext(pCertContext);
        CertCloseStore(store, 0);
        throw ResourceNotFoundException("Could not load node certificate");
    }

    std::string CN2 = "CN=" + CN;
    if (std::string(certname) != CN2)
    {
        CertFreeCertificateContext(pCertContext);
        CertCloseStore(store, 0);
        throw ResourceNotFoundException("Could not load node certificate");
    }

    activecertificate = pCertContext;
    activestore = store;
}

bool TlsSchannelAsyncStreamAdapterContext::IsCertificateLoaded()
{
    boost::mutex::scoped_lock lock(mylock);
    return activecertificate != NULL;
}

CredHandle TlsSchannelAsyncStreamAdapterContext::GetServerCredentials()
{
    boost::mutex::scoped_lock lock(mylock);
    if (server_credentials.dwLower == 0 && server_credentials.dwUpper == 0)
    {
        if (!activecertificate)
            throw InvalidOperationException("Server certificate not set");

        SCHANNEL_CRED SchannelCred;
        ZeroMemory(&SchannelCred, sizeof(SchannelCred));
        SchannelCred.dwVersion = SCHANNEL_CRED_VERSION;
        SchannelCred.grbitEnabledProtocols = SP_PROT_TLS1_0_SERVER | SP_PROT_TLS1_1_SERVER | SP_PROT_TLS1_2_SERVER;

        SchannelCred.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;

        // SchannelCred.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS ;
        // SchannelCred.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION | SCH_SEND_ROOT_CERT;
        SchannelCred.cCreds = 1;
        SchannelCred.paCred = &activecertificate;
        // SchannelCred.hRootStore=store;

        CredHandle phCreds;
        ZeroMemory(&phCreds, sizeof(CredHandle));
        TimeStamp tsExpiry_cred;
        SECURITY_STATUS Status_cred;
        Status_cred = AcquireCredentialsHandleA(NULL, UNISP_NAME_A, SECPKG_CRED_INBOUND, NULL, &SchannelCred, NULL,
                                                NULL, &phCreds, &tsExpiry_cred);
        if (Status_cred != SEC_E_OK)
        {
            if (phCreds.dwLower != 0 || phCreds.dwUpper != 0)
            {
                FreeCredentialsHandle(&server_credentials);
            }
            throw SystemResourceException("Could not initialize server credentials");
        }

        server_credentials = phCreds;
    }

    return server_credentials;
}

CredHandle TlsSchannelAsyncStreamAdapterContext::GetClientCredentials()
{
    boost::mutex::scoped_lock lock(mylock);
    if (client_credentials.dwLower == 0 && client_credentials.dwUpper == 0)
    {
        SCHANNEL_CRED SchannelCred;
        ZeroMemory(&SchannelCred, sizeof(SchannelCred));
        SchannelCred.dwVersion = SCHANNEL_CRED_VERSION;
        SchannelCred.grbitEnabledProtocols = SP_PROT_TLS1_0_CLIENT | SP_PROT_TLS1_1_CLIENT | SP_PROT_TLS1_2_CLIENT;

        SchannelCred.dwFlags |= SCH_CRED_NO_DEFAULT_CREDS;
        SchannelCred.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;
        if (activecertificate)
        {
            SchannelCred.cCreds = 1;
            SchannelCred.paCred = &activecertificate;
            //
        }
        else
        {
            // SchannelCred.dwFlags |= SCH_CRED_USE_DEFAULT_CREDS;
        }
        // SchannelCred.hRootStore=store;
        CredHandle phCreds;
        ZeroMemory(&phCreds, sizeof(CredHandle));
        TimeStamp tsExpiry_cred;
        SECURITY_STATUS Status_cred;
        Status_cred = AcquireCredentialsHandleA(NULL, UNISP_NAME_A, SECPKG_CRED_OUTBOUND, NULL, &SchannelCred, NULL,
                                                NULL, &phCreds, &tsExpiry_cred);
        if (Status_cred != SEC_E_OK)
        {
            if (phCreds.dwLower != 0 || phCreds.dwUpper != 0)
            {
                FreeCredentialsHandle(&server_credentials);
            }
            throw SystemResourceException("Could not initialize server credentials");
        }
        client_credentials = phCreds;
    }
    return client_credentials;
}

bool TlsSchannelAsyncStreamAdapterContext::VerifyCertificateOIDExtension(PCERT_INFO cert1, boost::string_ref searchoid)
{
    bool found_mid_rr_oid = false;
    for (size_t i = 0; i < cert1->cExtension; i++)
    {

        std::string oid(cert1->rgExtension[i].pszObjId);
        if (oid == "2.5.29.15" || oid == "2.5.29.14" || oid == "2.5.29.19" || oid == "2.5.29.35" || oid == "2.5.29.32")
        {
            continue;
        }

        if (oid == searchoid)
        {
            // Extended OIDs
            if (!cert1->rgExtension[i].fCritical)
            {
                continue;
            }

            found_mid_rr_oid = true;
            continue;
        }

        // std::cout << cert1->rgExtension[i].fCritical << ": " << cert1->rgExtension[i].pszObjId << std::endl;

        if (cert1->rgExtension[i].fCritical)
        {
            return false;
            break;
        }
    }

    if (!found_mid_rr_oid)
    {
        return false;
    }
    return true;
}

bool TlsSchannelAsyncStreamAdapterContext::VerifyRemoteNodeCertificate(PCCERT_CONTEXT cert, const NodeID& remote_node)
{

    // TODO: Check the certificate validation

    CERT_CHAIN_ENGINE_CONFIG cec;
    ZeroMemory(&cec, sizeof(cec));
    cec.cbSize = sizeof(cec);
    cec.cAdditionalStore = 1;
    cec.rghAdditionalStore = &store;

    HCERTCHAINENGINE hc;
    if (!CertCreateCertificateChainEngine(&cec, &hc))
    {
        return false;
    }

    CERT_CHAIN_PARA p;
    ZeroMemory(&p, sizeof(p));
    p.cbSize = sizeof(CERT_CHAIN_PARA);
    p.dwUrlRetrievalTimeout = 100;

    // ZeroMemory(&p,sizeof(CERT_CHAIN_PARA));
    PCCERT_CHAIN_CONTEXT pChainContext = NULL;
    if (!CertGetCertificateChain(hc, cert, NULL, cert->hCertStore /*store*/, &p, NULL, NULL, &pChainContext))
    {
        ::CertFreeCertificateChainEngine(hc);
        return false;
    }

    ::CertFreeCertificateChainEngine(hc);
    DWORD okerr = CERT_TRUST_HAS_NOT_SUPPORTED_CRITICAL_EXT | CERT_TRUST_INVALID_EXTENSION |
                  CERT_TRUST_REVOCATION_STATUS_UNKNOWN | CERT_TRUST_IS_OFFLINE_REVOCATION;
    DWORD errdiff = pChainContext->TrustStatus.dwErrorStatus & (~okerr);
    if (errdiff == CERT_TRUST_IS_UNTRUSTED_ROOT)
    {
        if (!use_root_cert_2015 && !use_root_cert_2020)
        {
            CertFreeCertificateChain(pChainContext);
            return false;
        }

        if (pChainContext->cChain != 1 || pChainContext->rgpChain[0]->cElement < 3)
        {
            CertFreeCertificateChain(pChainContext);
            return false;
        }

        PCERT_INFO root_cert =
            pChainContext->rgpChain[0]->rgpElement[pChainContext->rgpChain[0]->cElement - 1]->pCertContext->pCertInfo;

        bool root_cert_match = false;

        if (CertComparePublicKeyInfo(X509_ASN_ENCODING, &root_cert->SubjectPublicKeyInfo,
                                     &rootcertificate2015->pCertInfo->SubjectPublicKeyInfo))
        {
            if (this->use_root_cert_2015)
            {
                root_cert_match = true;
            }
        }

        if (!root_cert_match && CertComparePublicKeyInfo(X509_ASN_ENCODING, &root_cert->SubjectPublicKeyInfo,
                                                         &rootcertificate2020->pCertInfo->SubjectPublicKeyInfo))
        {
            if (this->use_root_cert_2020)
            {
                root_cert_match = true;
            }
        }

        if (!root_cert_match)
        {
            CertFreeCertificateChain(pChainContext);
            return false;
        }
    }
    else if (errdiff != 0)
    {
        CertFreeCertificateChain(pChainContext);
        return false;
    }

    std::string cn = "Robot Raconteur Node " + remote_node.ToString();
    WCHAR* cnw = new WCHAR[cn.size() + 1];
    ZeroMemory(cnw, (cn.size() + 1) * sizeof(WCHAR));
    size_t cnw_len;
    mbstowcs_s(&cnw_len, cnw, cn.size() + 1, cn.c_str(), cn.size());

    CERT_CHAIN_POLICY_STATUS status;
    SSL_EXTRA_CERT_CHAIN_POLICY_PARA policy_para1;
    ZeroMemory(&policy_para1, sizeof(policy_para1));
    policy_para1.cbSize = sizeof(policy_para1);
    policy_para1.dwAuthType = AUTHTYPE_SERVER;
    // policy_para1.fdwChecks=SECURITY_FLAG_IGNORE_UNKNOWN_CA;
    policy_para1.pwszServerName = cnw;

    CERT_CHAIN_POLICY_PARA policy_para;
    ZeroMemory(&policy_para, sizeof(policy_para));
    policy_para.cbSize = sizeof(policy_para);
    policy_para.dwFlags = CERT_CHAIN_POLICY_IGNORE_NOT_SUPPORTED_CRITICAL_EXT_FLAG;
    policy_para.pvExtraPolicyPara = &policy_para1;

    ZeroMemory(&status, sizeof(status));
    status.cbSize = sizeof(status);

    BOOL cv_res = CertVerifyCertificateChainPolicy(CERT_CHAIN_POLICY_SSL, pChainContext,
                                                   (PCERT_CHAIN_POLICY_PARA)&policy_para, &status);

    bool valid = true;

    if ((status.dwError & (~(CRYPT_E_NO_REVOCATION_CHECK | CRYPT_E_REVOCATION_OFFLINE | CERT_E_UNTRUSTEDROOT))) != 0)
    {
        valid = false;
    }

    if (valid)
    {
        if (pChainContext->cChain != 1 || pChainContext->rgpChain[0]->cElement < 3)
        {
            valid = false;
        }
    }

    if (valid)
    {
        if ((pChainContext->rgpChain[0]->TrustStatus.dwErrorStatus &
             (~(CERT_TRUST_HAS_NOT_SUPPORTED_CRITICAL_EXT | CERT_TRUST_INVALID_EXTENSION |
                CERT_TRUST_REVOCATION_STATUS_UNKNOWN | CERT_TRUST_IS_OFFLINE_REVOCATION |
                CERT_TRUST_IS_UNTRUSTED_ROOT))) != 0)
        {
            valid = false;
        }
    }

    if (valid)
    {
        PCERT_INFO root_cert =
            pChainContext->rgpChain[0]->rgpElement[pChainContext->rgpChain[0]->cElement - 1]->pCertContext->pCertInfo;
        if (!VerifyCertificateOIDExtension(root_cert, "1.3.6.1.4.1.45455.1.1.3.1"))
        {
            valid = false;
        }
    }

    if (valid)
    {
        PCERT_INFO cert1 = pChainContext->rgpChain[0]->rgpElement[0]->pCertContext->pCertInfo;
        if (!VerifyCertificateOIDExtension(cert1, "1.3.6.1.4.1.45455.1.1.3.3"))
        {
            valid = false;
        }
    }

    if (valid)
    {
        for (size_t k = 1; k < pChainContext->rgpChain[0]->cElement - 1; k++)
        {
            PCERT_INFO cert1 = pChainContext->rgpChain[0]->rgpElement[k]->pCertContext->pCertInfo;
            if (!VerifyCertificateOIDExtension(cert1, "1.3.6.1.4.1.45455.1.1.3.2"))
            {
                valid = false;
                break;
            }
        }
    }
    /*PCERT_CHAIN_ELEMENT cert1=pChainContext->rgpChain[0]->rgpElement[0];
    PCERT_CHAIN_ELEMENT cert2=pChainContext->rgpChain[0]->rgpElement[1];
    PCERT_CHAIN_ELEMENT cert3=pChainContext->rgpChain[0]->rgpElement[2];*/

    if (valid)
    {
        PCCERT_CONTEXT cert1 = pChainContext->rgpChain[0]->rgpElement[0]->pCertContext;
        DWORD dType = CERT_X500_NAME_STR | CERT_NAME_STR_REVERSE_FLAG;
        char certname[1024];
        ZeroMemory(certname, sizeof(certname));
        if (FALSE == CertGetNameStringA(cert1, CERT_NAME_RDN_TYPE, 0, &dType, certname, sizeof(certname)))
        {
            valid = false;
        }

        if (valid)
        {
            std::string CN2 = "CN=" + cn;
            if (std::string(certname) != CN2)
            {
                valid = false;
            }
        }
    }

    delete[] cnw;
    ::CertFreeCertificateChain(pChainContext);

    return valid;
}

bool TlsSchannelAsyncStreamAdapterContext::VerifyRemoteHostnameCertificate(PCCERT_CONTEXT cert,
                                                                           boost::string_ref hostname)
{

    // TODO: Check the certificate validation

    CERT_CHAIN_ENGINE_CONFIG cec;
    ZeroMemory(&cec, sizeof(cec));
    cec.cbSize = sizeof(cec);
    cec.cAdditionalStore = 0;
    // cec.rghAdditionalStore=&store;

    HCERTCHAINENGINE hc;
    if (!CertCreateCertificateChainEngine(&cec, &hc))
    {
        return false;
    }

    CERT_CHAIN_PARA p;
    ZeroMemory(&p, sizeof(p));
    p.cbSize = sizeof(CERT_CHAIN_PARA);
    p.dwUrlRetrievalTimeout = 100;

    // ZeroMemory(&p,sizeof(CERT_CHAIN_PARA));
    PCCERT_CHAIN_CONTEXT pChainContext = NULL;
    if (!CertGetCertificateChain(hc, cert, NULL, cert->hCertStore /*store*/, &p, NULL, NULL, &pChainContext))
    {
        ::CertFreeCertificateChainEngine(hc);
        return false;
    }

    ::CertFreeCertificateChainEngine(hc);
    DWORD okerr = CERT_TRUST_REVOCATION_STATUS_UNKNOWN | CERT_TRUST_IS_OFFLINE_REVOCATION;
    DWORD errdiff = pChainContext->TrustStatus.dwErrorStatus & (~okerr);
    if (errdiff != 0)
    {
        CertFreeCertificateChain(pChainContext);
        return false;
    }

    std::string cn = RR_MOVE(hostname.to_string());
    WCHAR* cnw = new WCHAR[cn.size() + 1];
    ZeroMemory(cnw, (cn.size() + 1) * sizeof(WCHAR));
    size_t cnw_len;
    mbstowcs_s(&cnw_len, cnw, cn.size() + 1, cn.c_str(), cn.size());

    CERT_CHAIN_POLICY_STATUS status;
    SSL_EXTRA_CERT_CHAIN_POLICY_PARA policy_para1;
    ZeroMemory(&policy_para1, sizeof(policy_para1));
    policy_para1.cbSize = sizeof(policy_para1);
    policy_para1.dwAuthType = AUTHTYPE_SERVER;
    // policy_para1.fdwChecks=SECURITY_FLAG_IGNORE_UNKNOWN_CA;
    policy_para1.pwszServerName = cnw;

    CERT_CHAIN_POLICY_PARA policy_para;
    ZeroMemory(&policy_para, sizeof(policy_para));
    policy_para.cbSize = sizeof(policy_para);
    // policy_para.dwFlags = CERT_CHAIN_POLICY_IGNORE_NOT_SUPPORTED_CRITICAL_EXT_FLAG;
    policy_para.pvExtraPolicyPara = &policy_para1;

    ZeroMemory(&status, sizeof(status));
    status.cbSize = sizeof(status);

    BOOL cv_res = CertVerifyCertificateChainPolicy(CERT_CHAIN_POLICY_SSL, pChainContext,
                                                   (PCERT_CHAIN_POLICY_PARA)&policy_para, &status);

    bool valid = true;

    if (status.dwError != 0)
    {
        valid = false;
    }

    delete[] cnw;
    ::CertFreeCertificateChain(pChainContext);

    return valid;
}

void TlsSchannelAsyncStreamAdapter_ASIO_adapter::close() { next_layer_.close(); }

TlsSchannelAsyncStreamAdapter::TlsSchannelAsyncStreamAdapter(
    RR_BOOST_ASIO_IO_CONTEXT& _io_context_, const boost::shared_ptr<TlsSchannelAsyncStreamAdapterContext>& context,
    direction_type direction, boost::string_ref servername,
    boost::function<void(mutable_buffers&,
                         boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>)>
        async_read_some,
    boost::function<void(const_buffers&,
                         boost::function<void(const boost::system::error_code& error, size_t bytes_transferred)>)>
        async_write_some,
    boost::function<void()> close)
    : _io_context(_io_context_), asio_adapter(*this)
{
    recv_buffer = boost::shared_array<uint8_t>(new uint8_t[max_tls_record_size]);
    recv_buffer_un = boost::shared_array<uint8_t>(new uint8_t[max_tls_record_size]);
    send_buffer = boost::shared_array<uint8_t>(new uint8_t[max_tls_record_size]);
    memset(recv_buffer.get(), 0, max_tls_record_size);
    memset(send_buffer.get(), 0, max_tls_record_size);
    memset(recv_buffer_un.get(), 0, max_tls_record_size);

    send_buffer_end_pos = 0;
    recv_buffer_end_pos = 0;
    send_buffer_transfer_pos = 0;

    recv_buffer_un_end_pos = 0;
    // recv_buffer_transfer_pos=0;

    this->_async_read_some = async_read_some;
    this->_async_write_some = async_write_some;
    this->_close = close;
    open = true;
    this->servername = RR_MOVE(servername.to_string());
    this->direction = direction;
    this->context = context;

    writing = false;
    reading = false;

    request_shutdown = false;
    request_renegotiate = false;

    handshaking = false;
    shutingdown = false;
    mutual_auth = false;
}

void TlsSchannelAsyncStreamAdapter_close_cert_store(void* h) { CertCloseStore(h, 0); }

void TlsSchannelAsyncStreamAdapter::async_handshake(boost::function<void(const boost::system::error_code&)> handler)
{

    boost::mutex::scoped_lock lock(stream_lock);

    if (!open)
    {
        boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
        RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, ec));
        return;
    }

    if (handshaking)
    {
        boost::system::error_code ec(boost::system::errc::operation_not_permitted, boost::system::generic_category());
        RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, ec));
        return;
    }

    if (direction == client)
    {

        if (!hCreds)
        {
            PCredHandle phCreds = new CredHandle();
            ZeroMemory(phCreds, sizeof(CredHandle));
            boost::shared_ptr<CredHandle> hCreds1 = boost::shared_ptr<CredHandle>(
                phCreds, boost::bind(&TlsSchannelAsyncStreamAdapter::release_credentials, RR_BOOST_PLACEHOLDERS(_1)));
            *phCreds = context->GetClientCredentials();
            hCreds = hCreds1;
        }

        SecBufferDesc OutBuffer;
        SecBuffer OutBuffers[1];
        DWORD dwSSPIFlags, dwSSPIOutFlags;
        TimeStamp tsExpiry;
        SECURITY_STATUS scRet;

        dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT | ISC_REQ_CONFIDENTIALITY |
                      ISC_RET_EXTENDED_ERROR | ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_STREAM;

        OutBuffers[0].pvBuffer = NULL;
        OutBuffers[0].BufferType = SECBUFFER_TOKEN;
        OutBuffers[0].cbBuffer = 0;

        OutBuffer.cBuffers = 1;
        OutBuffer.pBuffers = OutBuffers;
        OutBuffer.ulVersion = SECBUFFER_VERSION;

        CtxtHandle* phContext = new CtxtHandle();
        ZeroMemory(phContext, sizeof(CtxtHandle));

        SEC_CHAR* servername_c1 = new SEC_CHAR[servername.size() + 1];
        ZeroMemory(servername_c1, servername.size() + 1);
        strncpy_s(servername_c1, servername.size() + 1, servername.c_str(), servername.size());
        boost::shared_array<SEC_CHAR> servername_c(servername_c1);

        scRet = InitializeSecurityContextA(hCreds.get(), NULL, servername_c.get(), dwSSPIFlags, 0, SECURITY_NATIVE_DREP,
                                           NULL, 0, phContext, &OutBuffer, &dwSSPIOutFlags, &tsExpiry);
        if (scRet != SEC_I_CONTINUE_NEEDED)
        {
            delete phContext;
            throw SystemResourceException("Could not initialize TLS");
        }

        hContext = boost::shared_ptr<CtxtHandle>(
            phContext, boost::bind(&TlsSchannelAsyncStreamAdapter::release_context, RR_BOOST_PLACEHOLDERS(_1)));

        if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
        {
            memcpy(send_buffer.get(), OutBuffers[0].pvBuffer, OutBuffers[0].cbBuffer);
            send_buffer_end_pos = OutBuffers[0].cbBuffer;

            send_buffer_transfer_pos = 0;
            FreeContextBuffer(OutBuffers[0].pvBuffer);
            OutBuffers[0].pvBuffer = NULL;

            const_buffers send_buf;
            send_buf.push_back(boost::asio::const_buffer(send_buffer.get(), send_buffer_end_pos));

            handshaking = true;
            _async_write_some(send_buf,
                              boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake1, shared_from_this(),
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred, boost::protect(handler)));
        }
        else
        {
            boost::system::error_code ec(boost::system::errc::protocol_error, boost::system::generic_category());
            RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, ec));
        }
        return;
    }

    if (direction == server)
    {
        if (!hCreds)
        {
            PCredHandle phCreds = new CredHandle();
            ZeroMemory(phCreds, sizeof(CredHandle));
            boost::shared_ptr<CredHandle> hCreds1 = boost::shared_ptr<CredHandle>(
                phCreds, boost::bind(&TlsSchannelAsyncStreamAdapter::release_credentials, RR_BOOST_PLACEHOLDERS(_1)));
            *phCreds = context->GetServerCredentials();
            hCreds = hCreds1;
        }

        recv_buffer_end_pos = 0;

        handshaking = true;
        mutable_buffers recv_buf;
        recv_buf.push_back(boost::asio::mutable_buffer(recv_buffer.get(), max_tls_record_size));
        _async_read_some(recv_buf,
                         boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake2, shared_from_this(),
                                     boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                     boost::protect(handler), true));

        return;
    }

    throw InvalidOperationException("Invalid direction");
}

void TlsSchannelAsyncStreamAdapter::do_handshake1(const boost::system::error_code& error, size_t bytes_transferred,
                                                  boost::function<void(const boost::system::error_code&)> handler)
{

    boost::mutex::scoped_lock lock(stream_lock);

    if (error)
    {
        handshaking = false;

        do_handshake6(error, handler);
        return;
    }

    if (bytes_transferred == 0)
    {
        handshaking = false;

        boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
        do_handshake6(ec, handler);
        return;
    }

    if (!open)
    {
        handshaking = false;
        boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
        do_handshake6(ec, handler);
        return;
    }

    if ((send_buffer_transfer_pos + bytes_transferred) < send_buffer_end_pos)
    {
        send_buffer_transfer_pos += boost::numeric_cast<uint32_t>(bytes_transferred);
        const_buffers send_b;
        send_b.push_back(boost::asio::const_buffer(send_buffer.get() + send_buffer_transfer_pos,
                                                   send_buffer_end_pos - send_buffer_transfer_pos));
        _async_write_some(send_b, boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake1, shared_from_this(),
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred, boost::protect(handler)));
        return;
    }

    if (recv_buffer_end_pos != 0)
    {
        uint32_t r = recv_buffer_end_pos;
        recv_buffer_end_pos = 0;

        boost::system::error_code ec;
        RR_BOOST_ASIO_POST(_io_context, boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake1, shared_from_this(),
                                                    ec, r, boost::protect(handler)));
        return;
    }
    else
    {

        recv_buffer_end_pos = 0;
        mutable_buffers recv_b;
        recv_b.push_back(boost::asio::mutable_buffer(recv_buffer.get(), max_tls_record_size));
        _async_read_some(recv_b,
                         boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake2, shared_from_this(),
                                     boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                     boost::protect(handler), true));
        return;
    }
}

void TlsSchannelAsyncStreamAdapter::do_handshake2(const boost::system::error_code& error, size_t bytes_transferred,
                                                  boost::function<void(const boost::system::error_code&)> handler,
                                                  bool doread)
{
    boost::mutex::scoped_lock lock(stream_lock);
    do_handshake3(error, bytes_transferred, handler, doread);
}

void TlsSchannelAsyncStreamAdapter::do_handshake3(const boost::system::error_code& error, size_t bytes_transferred,
                                                  boost::function<void(const boost::system::error_code&)> handler,
                                                  bool doread)
{

    handshaking = true;
    request_renegotiate = false;
    if (error)
    {
        handshaking = false;
        do_handshake6(error, handler);
        return;
    }

    if (bytes_transferred == 0 && doread)
    {
        handshaking = false;

        boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
        do_handshake6(ec, handler);
        return;
    }

    if (!open)
    {
        handshaking = false;
        boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
        do_handshake6(ec, handler);
        return;
    }

    TimeStamp tsExpiry;
    SECURITY_STATUS scRet;
    DWORD dwSSPIFlags, dwSSPIOutFlags;

    recv_buffer_end_pos += boost::numeric_cast<uint32_t>(bytes_transferred);

    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT | ISC_REQ_CONFIDENTIALITY | ISC_RET_EXTENDED_ERROR |
                  ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_STREAM;

    SecBufferDesc OutBuffer, InBuffer;
    SecBuffer InBuffers[2], OutBuffers[1];
    SecBufferDesc* pInBuffer = NULL;

    if (doread)
    {
        InBuffers[0].pvBuffer = recv_buffer.get();
        InBuffers[0].cbBuffer = recv_buffer_end_pos;
        InBuffers[0].BufferType = SECBUFFER_TOKEN;

        InBuffers[1].pvBuffer = NULL;
        InBuffers[1].cbBuffer = 0;
        InBuffers[1].BufferType = SECBUFFER_EMPTY;

        InBuffer.cBuffers = 2;
        InBuffer.pBuffers = InBuffers;
        InBuffer.ulVersion = SECBUFFER_VERSION;

        pInBuffer = &InBuffer;
    }
    OutBuffers[0].pvBuffer = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer = 0;

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    if (direction != server)
    {

        // Call InitializeSecurityContext.
        scRet = InitializeSecurityContextA(hCreds.get(), hContext.get(), NULL, dwSSPIFlags, 0, SECURITY_NATIVE_DREP,
                                           pInBuffer, 0, hContext.get(), &OutBuffer, &dwSSPIOutFlags, &tsExpiry);
    }
    else
    {
        if (mutual_auth)
        {
            dwSSPIFlags |= ASC_REQ_MUTUAL_AUTH;
        }
        if (!hContext)
        {
            CtxtHandle* phContext = new CtxtHandle();
            ZeroMemory(phContext, sizeof(CtxtHandle));
            scRet = AcceptSecurityContext(hCreds.get(), NULL, pInBuffer, dwSSPIFlags, 0, phContext, &OutBuffer,
                                          &dwSSPIOutFlags, &tsExpiry);

            if (scRet == SEC_E_OK || scRet == SEC_I_CONTINUE_NEEDED)
            {
                hContext = boost::shared_ptr<CtxtHandle>(
                    phContext, boost::bind(&TlsSchannelAsyncStreamAdapter::release_context, RR_BOOST_PLACEHOLDERS(_1)));
            }
            else
            {
                delete phContext;
            }
        }
        else
        {
            scRet = AcceptSecurityContext(hCreds.get(), hContext.get(), pInBuffer, dwSSPIFlags, 0, hContext.get(),
                                          &OutBuffer, &dwSSPIOutFlags, &tsExpiry);
        }
    }

    if (scRet == SEC_E_INCOMPLETE_MESSAGE)
    {
        mutable_buffers recv_b;
        recv_b.push_back(boost::asio::mutable_buffer(recv_buffer.get() + recv_buffer_end_pos,
                                                     max_tls_record_size - recv_buffer_end_pos));
        _async_read_some(recv_b,
                         boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake2, shared_from_this(),
                                     boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                     boost::protect(handler), true));
        //_async_read_some(boost::asio::mutable_buffer(recv_buffer,max_tls_record_size),boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake2,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred,boost::protect(handler)));
        return;
    }

    send_buffer_end_pos = 0;
    send_buffer_transfer_pos = 0;

    if (scRet == SEC_E_OK || scRet == SEC_I_CONTINUE_NEEDED)
    {
        if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != NULL)
        {
            memcpy(send_buffer.get(), OutBuffers[0].pvBuffer, OutBuffers[0].cbBuffer);
            send_buffer_end_pos = OutBuffers[0].cbBuffer;
            FreeContextBuffer(OutBuffers[0].pvBuffer);
            OutBuffers[0].pvBuffer = NULL;
        }
    }

    if (doread)
    {
        if (InBuffers[1].BufferType == SECBUFFER_EXTRA)
        {
            MoveMemory(recv_buffer.get(), recv_buffer.get() + (recv_buffer_end_pos - InBuffers[1].cbBuffer),
                       InBuffers[1].cbBuffer);
            recv_buffer_end_pos = InBuffers[1].cbBuffer;
        }
        else
        {
            recv_buffer_end_pos = 0;
        }
    }

    if (FAILED(scRet) && (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR))
    {
        handshaking = false;
        boost::system::error_code ec(boost::system::errc::permission_denied, boost::system::generic_category());
        do_handshake6(ec, handler);
        return;
    }

    if (FAILED(scRet))
    {
        handshaking = false;
        boost::system::error_code ec(boost::system::errc::permission_denied, boost::system::generic_category());
        do_handshake6(ec, handler);
        return;
    }

    if (send_buffer_end_pos != 0)
    {
        const_buffers send_buf;
        send_buf.push_back(boost::asio::const_buffer(send_buffer.get(), send_buffer_end_pos));
        if (scRet == SEC_E_OK)
        {
            _async_write_some(send_buf,
                              boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake4, shared_from_this(),
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred, boost::protect(handler)));
        }
        else
        {
            _async_write_some(send_buf,
                              boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake1, shared_from_this(),
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred, boost::protect(handler)));
        }
        return;
    }

    if (scRet == SEC_E_OK)
    {
        handshaking = false;

        SECURITY_STATUS scRet = QueryContextAttributes(hContext.get(), SECPKG_ATTR_STREAM_SIZES, &TlsStreamSizes);
        if (scRet != SEC_E_OK)
        {

            boost::system::error_code ec2(boost::system::errc::io_error, boost::system::generic_category());
            do_handshake6(ec2, handler);
            return;
        }

        if (TlsStreamSizes.cbMaximumMessage > 16 * 1024)
        {
            TlsStreamSizes.cbMaximumMessage = 16 * 1024;
        }

        boost::system::error_code ec1;
        do_handshake6(ec1, handler);
        return;
    }

    if (recv_buffer_end_pos != 0)
    {
        uint32_t r = recv_buffer_end_pos;
        recv_buffer_end_pos = 0;

        boost::system::error_code ec;
        RR_BOOST_ASIO_POST(_io_context, boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake2, shared_from_this(),
                                                    ec, r, boost::protect(handler), true));
        return;
    }
    else
    {

        recv_buffer_end_pos = 0;

        mutable_buffers recv_b;
        recv_b.push_back(boost::asio::mutable_buffer(recv_buffer.get(), max_tls_record_size));
        _async_read_some(recv_b,
                         boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake2, shared_from_this(),
                                     boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                     boost::protect(handler), true));
        return;
    }
}

void TlsSchannelAsyncStreamAdapter::do_handshake4(const boost::system::error_code& error, size_t bytes_transferred,
                                                  boost::function<void(const boost::system::error_code&)> handler)
{
    boost::mutex::scoped_lock lock(stream_lock);
    if (error)
    {
        handshaking = false;

        do_handshake6(error, handler);
        return;
    }

    if (bytes_transferred == 0)
    {
        handshaking = false;

        boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
        do_handshake6(ec, handler);
        return;
    }

    if (!open)
    {
        handshaking = false;
        boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
        do_handshake6(ec, handler);
        return;
    }

    if ((send_buffer_transfer_pos + bytes_transferred) < send_buffer_end_pos)
    {
        send_buffer_transfer_pos += boost::numeric_cast<uint32_t>(bytes_transferred);
        const_buffers send_b;
        send_b.push_back(boost::asio::const_buffer(send_buffer.get() + send_buffer_transfer_pos,
                                                   send_buffer_end_pos - send_buffer_transfer_pos));
        _async_write_some(send_b, boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake1, shared_from_this(),
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred, boost::protect(handler)));
        return;
    }

    handshaking = false;

    if (async_write_op)
    {
        boost::function<void()> async_write_op1 = async_write_op;
        async_write_op.clear();
        RR_BOOST_ASIO_POST(_io_context, async_write_op1);
    }

    SECURITY_STATUS scRet = QueryContextAttributes(hContext.get(), SECPKG_ATTR_STREAM_SIZES, &TlsStreamSizes);
    if (scRet != SEC_E_OK)
    {
        handshaking = false;

        boost::system::error_code ec2(boost::system::errc::io_error, boost::system::generic_category());
        do_handshake6(ec2, handler);
        return;
    }

    if (TlsStreamSizes.cbMaximumMessage > 16 * 1024)
    {
        TlsStreamSizes.cbMaximumMessage = 16 * 1024;
    }

    boost::system::error_code ec1;
    do_handshake6(ec1, handler);
}

void TlsSchannelAsyncStreamAdapter::do_handshake5(boost::function<void(const boost::system::error_code&)> handler)
{
    if (direction != server)
    {
        boost::system::error_code ec2;

        do_handshake3(ec2, 0, boost::protect(handler), false);
    }
    else
    {
        if (recv_buffer_end_pos != 0)
        {
            boost::system::error_code ec2;
            do_handshake3(ec2, recv_buffer_end_pos, boost::protect(handler), true);
        }
        else
        {
            handshaking = true;
            mutable_buffers recv_buf;
            recv_buf.push_back(boost::asio::mutable_buffer(recv_buffer.get(), max_tls_record_size));
            _async_read_some(recv_buf,
                             boost::bind(&TlsSchannelAsyncStreamAdapter::do_handshake2, shared_from_this(),
                                         boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                         boost::protect(handler), true));
        }
    }
}

void TlsSchannelAsyncStreamAdapter::do_handshake6(const boost::system::error_code& error,
                                                  boost::function<void(const boost::system::error_code&)> handler)
{
    try
    {
        if (request_shutdown)
        {
            do_shutdown1();
        }
    }
    catch (std::exception&)
    {}

    if (async_write_op && !shutingdown)
    {
        boost::function<void()> async_write_op1 = async_write_op;
        async_write_op.clear();
        RR_BOOST_ASIO_POST(_io_context, async_write_op1);
    }

    RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, error));
}

static void TlsSchannelAsyncStreamAdapter_async_write_some_buf_adaptor(
    const RR_SHARED_PTR<TlsSchannelAsyncStreamAdapter>& t, const RR_SHARED_PTR<const_buffers>& b,
    const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{
    t->async_write_some(*b, handler);
}

void TlsSchannelAsyncStreamAdapter::async_write_some(
    const_buffers& b, const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{
    boost::mutex::scoped_lock lock(stream_lock);

    if (!open)
    {
        boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
        async_write_some2(ec, handler);
        return;
    }

    /*if (request_shutdown && async_shutdown_handler_op)
    {
        boost::function<void (const boost::system::error_code&)> async_shutdown_handler1=async_shutdown_handler_op;
        async_shutdown_handler_op.clear();
        boost::system::error_code ec1(boost::system::errc::broken_pipe,boost::system::generic_category);
        async_write_op=(boost::bind(handler,ec1,0));
        do_shutdown1(boost::bind(async_shutdown_handler1,RR_BOOST_PLACEHOLDERS(_1)));

        return;
    }*/

    if (handshaking || shutingdown)
    {
        if (async_write_op)
        {
            boost::system::error_code ec(boost::system::errc::operation_not_permitted,
                                         boost::system::generic_category());
            async_write_some2(ec, handler);
            return;
        }

        // workaround for small_vector alignment issue
        RR_SHARED_PTR<const_buffers> b2 = RR_MAKE_SHARED<const_buffers>(b);

        async_write_op = boost::bind(&TlsSchannelAsyncStreamAdapter_async_write_some_buf_adaptor, shared_from_this(),
                                     b2, boost::protect(handler));

        return;
    }

    send_buffer_end_pos = 0;
    send_buffer_transfer_pos = 0;

    uint32_t s = boost::numeric_cast<uint32_t>(boost::asio::buffer_size(b));
    if (s > TlsStreamSizes.cbMaximumMessage)
        s = TlsStreamSizes.cbMaximumMessage;

    SECURITY_STATUS scRet;
    SecBufferDesc Message;
    SecBuffer Buffers[4];

    uint8_t* buffer_dat = send_buffer.get() + TlsStreamSizes.cbHeader;

    Buffers[0].pvBuffer = send_buffer.get();
    Buffers[0].cbBuffer = TlsStreamSizes.cbHeader;
    Buffers[0].BufferType = SECBUFFER_STREAM_HEADER;

    Buffers[1].pvBuffer = buffer_dat;
    Buffers[1].cbBuffer = s;
    Buffers[1].BufferType = SECBUFFER_DATA;

    Buffers[2].pvBuffer = buffer_dat + s;
    Buffers[2].cbBuffer = TlsStreamSizes.cbTrailer;
    Buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;

    Buffers[3].pvBuffer = SECBUFFER_EMPTY;
    Buffers[3].cbBuffer = SECBUFFER_EMPTY;
    Buffers[3].BufferType = SECBUFFER_EMPTY;

    Message.ulVersion = SECBUFFER_VERSION;
    Message.cBuffers = 4;
    Message.pBuffers = Buffers;

    boost::asio::buffer_copy(boost::asio::buffer(buffer_dat, s), b);

    // memcpy(buffer_dat,b2,s);

    scRet = EncryptMessage(hContext.get(), 0, &Message, 0);

    if (FAILED(scRet))
    {
        boost::system::error_code ec1(boost::system::errc::protocol_error, boost::system::generic_category());
        async_write_some2(ec1, handler);
        return;
    }

    uint32_t s2 = Buffers[0].cbBuffer + Buffers[1].cbBuffer + Buffers[2].cbBuffer;

    const_buffers buf2;
    buf2.push_back(boost::asio::const_buffer(send_buffer.get(), s2));

    send_buffer_end_pos = s2;
    writing = true;
    _async_write_some(buf2, boost::bind(&TlsSchannelAsyncStreamAdapter::async_write_some1, shared_from_this(),
                                        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                        Buffers[1].cbBuffer, boost::protect(handler)));
}

void TlsSchannelAsyncStreamAdapter::async_write_some1(
    const boost::system::error_code& error, size_t bytes_transferred, size_t len,
    const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{

    boost::mutex::scoped_lock lock(stream_lock);
    if (error)
    {
        writing = false;
        // lock.unlock();
        async_write_some2(error, handler);
        return;
    }

    if (!open)
    {
        writing = false;
        boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
        async_write_some2(ec, handler);
        return;
    }

    send_buffer_transfer_pos += boost::numeric_cast<uint32_t>(bytes_transferred);
    size_t s = send_buffer_transfer_pos;

    if (send_buffer_transfer_pos >= send_buffer_end_pos)
    {
        send_buffer_transfer_pos = 0;
        send_buffer_end_pos = 0;

        writing = false;

        try
        {
            if (request_shutdown && async_shutdown_handler_op)
            {
                do_shutdown1();
            }
        }
        catch (std::exception&)
        {}

        try
        {
            if (request_renegotiate && async_handshake_handler_op)
            {
                boost::function<void(const boost::system::error_code&)> async_handshake_handler1 =
                    async_handshake_handler_op;
                async_handshake_handler_op.clear();
                boost::system::error_code ec2;
                do_handshake5(boost::bind(async_handshake_handler1, RR_BOOST_PLACEHOLDERS(_1)));
            }
        }
        catch (std::exception&)
        {}

        lock.unlock();

        boost::system::error_code ec1;
        handler(ec1, len);
        return;
    }

    const_buffers buf2;
    buf2.push_back(boost::asio::const_buffer(send_buffer.get() + send_buffer_transfer_pos,
                                             send_buffer_end_pos - send_buffer_transfer_pos));
    _async_write_some(buf2, boost::bind(&TlsSchannelAsyncStreamAdapter::async_write_some1, shared_from_this(),
                                        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                        len, boost::protect(handler)));
}

void TlsSchannelAsyncStreamAdapter::async_write_some2(
    const boost::system::error_code& error,
    const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{
    // Error handler for async_write_some

    try
    {
        if (request_shutdown)
        {
            do_shutdown1();
        }
    }
    catch (std::exception&)
    {}

    try
    {
        if (request_renegotiate && async_handshake_handler_op)
        {
            boost::function<void(const boost::system::error_code&)> async_handshake_handler1 =
                async_handshake_handler_op;
            async_handshake_handler_op.clear();
            request_renegotiate = false;
            RR_BOOST_ASIO_POST(_io_context, boost::bind(async_handshake_handler1, error));
        }
    }
    catch (std::exception&)
    {}

    RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, error, 0));
}

void TlsSchannelAsyncStreamAdapter::async_read_some(
    mutable_buffers& b, const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{

    boost::mutex::scoped_lock lock(stream_lock);

    if (!open || shutingdown)
    {
        boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
        RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, ec, 0));
        return;
    }

    // TODO: use more than first buffer
    boost::asio::mutable_buffer b3 =
        boost::asio::detail::buffer_sequence_adapter<boost::asio::mutable_buffer, mutable_buffers>::first(b);

    if (recv_buffer_un_end_pos > 0)
    {
        uint32_t diff = recv_buffer_un_end_pos;
        if (diff > boost::asio::buffer_size(b3))
        {
            size_t d = boost::asio::buffer_size(b3);
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
            memcpy(RR_BOOST_ASIO_BUFFER_CAST(void*, b3), recv_buffer_un.get(), d);
            size_t p2 = d;
            memmove(recv_buffer_un.get(), recv_buffer_un.get() + d, recv_buffer_un_end_pos - d);

            recv_buffer_un_end_pos = boost::numeric_cast<uint32_t>(recv_buffer_un_end_pos - d);
            boost::system::error_code ec;
            RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, ec, d));
            return;
        }
        else
        {
            size_t d = recv_buffer_un_end_pos;
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
            memcpy(RR_BOOST_ASIO_BUFFER_CAST(void*, b3), recv_buffer_un.get(), d);
            recv_buffer_un_end_pos = 0;
            boost::system::error_code ec;
            RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, ec, d));
            return;
        }
    }

    if (recv_buffer_end_pos > 0)
    {
        size_t d = recv_buffer_end_pos;
        recv_buffer_end_pos = 0;
        lock.unlock();
        boost::system::error_code ec1;
        RR_BOOST_ASIO_POST(_io_context, boost::bind(&TlsSchannelAsyncStreamAdapter::async_read_some1,
                                                    shared_from_this(), b3, ec1, d, handler));
        return;
    }
    else
    {
        reading = true;
        mutable_buffers recv_b;
        recv_b.push_back(boost::asio::buffer(recv_buffer.get(), max_tls_record_size));
        _async_read_some(recv_b, boost::bind(&TlsSchannelAsyncStreamAdapter::async_read_some1, shared_from_this(), b3,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred, boost::protect(handler)));
        return;
    }
}

void TlsSchannelAsyncStreamAdapter::async_read_some1(
    boost::asio::mutable_buffer& b, const boost::system::error_code& error, size_t bytes_transferred,
    const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{
    boost::mutex::scoped_lock lock(stream_lock);
    if (bytes_transferred == 0)
    {
        reading = false;
        lock.unlock();
        handler(error, bytes_transferred);
        return;
    }

    if (error)
    {
        reading = false;
        lock.unlock();

        handler(error, 0);
        return;
    }

    if (!open)
    {
        reading = false;
        boost::system::error_code ec(boost::system::errc::broken_pipe, boost::system::generic_category());
        RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, ec, 0));
        return;
    }

    // memcpy(recv_buffer + recv_buffer_end_pos, boost::asio::buffer_cast<void*>(b),bytes_transferred);
    recv_buffer_end_pos += boost::numeric_cast<uint32_t>(bytes_transferred);

    bool keepgoing = true;

    SECURITY_STATUS scRet;
    SecBufferDesc Message;
    SecBuffer Buffers[4];

    SecBuffer* pDataBuffer = NULL;
    SecBuffer* pExtraBuffer = NULL;

    while (keepgoing)
    {
        ZeroMemory(Buffers, sizeof(Buffers));

        Buffers[0].pvBuffer = recv_buffer.get();
        Buffers[0].cbBuffer = recv_buffer_end_pos;
        Buffers[0].BufferType = SECBUFFER_DATA;
        Buffers[1].BufferType = SECBUFFER_EMPTY;
        Buffers[2].BufferType = SECBUFFER_EMPTY;
        Buffers[3].BufferType = SECBUFFER_EMPTY;

        Message.ulVersion = SECBUFFER_VERSION;
        Message.cBuffers = 4;
        Message.pBuffers = Buffers;

        scRet = DecryptMessage(hContext.get(), &Message, 0, NULL);

        if (scRet == SEC_E_INCOMPLETE_MESSAGE)
        {
            mutable_buffers recv_b;
            recv_b.push_back(boost::asio::buffer(recv_buffer.get() + recv_buffer_end_pos,
                                                 max_tls_record_size - recv_buffer_end_pos));
            _async_read_some(recv_b,
                             boost::bind(&TlsSchannelAsyncStreamAdapter::async_read_some1, shared_from_this(), b,
                                         boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                                         boost::protect(handler)));
            return;
        }

        pDataBuffer = NULL;
        pExtraBuffer = NULL;

        for (size_t i = 1; i < 4; i++)
        {
            if (pDataBuffer == NULL && Buffers[i].BufferType == SECBUFFER_DATA)
                pDataBuffer = &Buffers[i];
            if (pExtraBuffer == NULL && Buffers[i].BufferType == SECBUFFER_EXTRA)
                pExtraBuffer = &Buffers[i];
        }

        DWORD cbDataBuffer = 0;
        if (pDataBuffer != NULL)
        {
            cbDataBuffer = pDataBuffer->cbBuffer;
        }

        keepgoing = false;
        if ((scRet == SEC_E_OK || scRet == SEC_I_RENEGOTIATE) && pExtraBuffer != NULL)
        {
            if (cbDataBuffer == 0 && pExtraBuffer->cbBuffer != 0)
            {
                MoveMemory(recv_buffer.get(), pExtraBuffer->pvBuffer, pExtraBuffer->cbBuffer);
                recv_buffer_end_pos = pExtraBuffer->cbBuffer;
                //_async_read_some(boost::asio::buffer(recv_buffer+recv_buffer_end_pos,max_tls_record_size-recv_buffer_end_pos),boost::bind(&TlsSchannelAsyncStreamAdapter::async_read_some1,shared_from_this(),
                // b, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                // boost::protect(handler)));

                if (scRet != SEC_I_RENEGOTIATE)
                {
                    keepgoing = true;
                }
            }
        }
    }

    if (scRet == SEC_I_RENEGOTIATE)
    {
        if (writing)
        {
            async_handshake_handler_op =
                boost::bind(&TlsSchannelAsyncStreamAdapter::async_read_some2, shared_from_this(),
                            RR_BOOST_PLACEHOLDERS(_1), b, boost::protect(handler));
            request_renegotiate = true;
            return;
        }
        else
        {

            do_handshake5(boost::bind(&TlsSchannelAsyncStreamAdapter::async_read_some2, shared_from_this(),
                                      RR_BOOST_PLACEHOLDERS(_1), b, handler));
            return;
        }

        /*boost::system::error_code ec1(boost::system::errc::broken_pipe, boost::system::generic_category);
        lock.unlock();
        handler(ec1,0);
        return;*/
    }

    recv_buffer_end_pos = 0;

    if (scRet == SEC_I_CONTEXT_EXPIRED)
    {
        boost::system::error_code ec1;
        if (shutingdown || !open || async_shutdown_handler_rd)
        {
            lock.unlock();
            handler(ec1, 0);
            return;
        }

        async_shutdown_handler_rd = boost::bind(handler, ec1, 0);

        if (writing)
        {
            request_shutdown = true;
            return;
        }
        do_shutdown1();
        return;
    }

    if (scRet != SEC_E_OK)
    {
        boost::system::error_code ec1(scRet, boost::system::generic_category());
        reading = false;
        lock.unlock();
        handler(ec1, 0);
        return;
    }

    if (!pDataBuffer)
    {
        boost::system::error_code ec1;
        reading = false;
        lock.unlock();
        handler(ec1, 0);
        return;
    }

    if (pDataBuffer->cbBuffer <= boost::asio::buffer_size(b))
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        MoveMemory(RR_BOOST_ASIO_BUFFER_CAST(void*, b), pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);
        if (pExtraBuffer)
        {
            MoveMemory(recv_buffer.get(), pExtraBuffer->pvBuffer, pExtraBuffer->cbBuffer);

            recv_buffer_end_pos = pExtraBuffer->cbBuffer;
        }
        else
        {
            recv_buffer_end_pos = 0;
        }
        recv_buffer_un_end_pos = 0;
        reading = false;
        lock.unlock();
        boost::system::error_code ec1;
        handler(ec1, pDataBuffer->cbBuffer);
        return;
    }
    else
    {
        size_t bsize = boost::asio::buffer_size(b);
        size_t extra_size = pDataBuffer->cbBuffer - bsize;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
        MoveMemory(RR_BOOST_ASIO_BUFFER_CAST(void*, b), pDataBuffer->pvBuffer, bsize);
        MoveMemory(recv_buffer_un.get(), ((uint8_t*)pDataBuffer->pvBuffer) + bsize, extra_size);
        recv_buffer_un_end_pos = boost::numeric_cast<uint32_t>(extra_size);
        if (pExtraBuffer)
        {
            MoveMemory(recv_buffer.get(), pExtraBuffer->pvBuffer, pExtraBuffer->cbBuffer);

            recv_buffer_end_pos = pExtraBuffer->cbBuffer;
        }
        else
        {
            recv_buffer_end_pos = 0;
        }
        reading = false;
        lock.unlock();
        boost::system::error_code ec1;
        handler(ec1, bsize);
        return;
    }
}

void TlsSchannelAsyncStreamAdapter::async_read_some2(
    const boost::system::error_code& error, boost::asio::mutable_buffer& b,
    const boost::function<void(const boost::system::error_code&, size_t)>& handler)
{
    {
        // boost::mutex::scoped_lock lock(stream_lock);

        if (error)
        {
            // reading=false;
            // lock.unlock();
            handler(error, 0);
            return;
        }
        mutable_buffers recv_b;
        recv_b.push_back(b);
        async_read_some(recv_b, handler);
    }
}

void TlsSchannelAsyncStreamAdapter::async_shutdown(boost::function<void(const boost::system::error_code&)> handler)
{
    boost::mutex::scoped_lock lock(stream_lock);

    /*boost::system::error_code ec;
    _io_context.post(boost::bind(handler,ec));
    return;*/

    if (!open)
    {
        boost::system::error_code ec;
        RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, ec));
        return;
    }

    if (async_shutdown_handler_op)
    {
        boost::system::error_code ec(boost::system::errc::operation_not_permitted, boost::system::generic_category());
        RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, ec));
        return;
    }

    if (shutingdown)
    {
        boost::system::error_code ec;
        RR_BOOST_ASIO_POST(_io_context, boost::bind(handler, ec));
        return;
    }

    async_shutdown_handler_op = handler;

    if (writing || handshaking)
    {
        request_shutdown = true;
        return;
    }

    do_shutdown1();
}

void TlsSchannelAsyncStreamAdapter::do_shutdown1()
{
    if (!open)
    {
        boost::system::error_code ec1(boost::system::errc::protocol_error, boost::system::generic_category());
        do_shutdown3(ec1);
        return;
    }
    shutingdown = true;

    request_shutdown = false;

    void* pbMessage;
    DWORD dwType, dwSSPIFlags, dwSSPIOutFlags, cbMessage, Status;
    SecBufferDesc OutBuffer;
    SecBuffer OutBuffers[1];
    TimeStamp tsExpiry;

    dwType = SCHANNEL_SHUTDOWN; // Notify schannel that we are about to close the connection.

    OutBuffers[0].pvBuffer = &dwType;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer = sizeof(dwType);

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    Status = ApplyControlToken(hContext.get(), &OutBuffer);
    if (FAILED(Status))
    {
        boost::system::error_code ec1(boost::system::errc::protocol_error, boost::system::generic_category());
        do_shutdown3(ec1);
        return;
    }

    // Build an SSL close notify message.
    dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT | ISC_REQ_CONFIDENTIALITY | ISC_RET_EXTENDED_ERROR |
                  ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_STREAM;

    OutBuffers[0].pvBuffer = NULL;
    OutBuffers[0].BufferType = SECBUFFER_TOKEN;
    OutBuffers[0].cbBuffer = 0;

    OutBuffer.cBuffers = 1;
    OutBuffer.pBuffers = OutBuffers;
    OutBuffer.ulVersion = SECBUFFER_VERSION;

    if (direction == server)
    {
        Status = AcceptSecurityContext(hCreds.get(), hContext.get(), NULL, dwSSPIFlags, 0, hContext.get(), &OutBuffer,
                                       &dwSSPIOutFlags, &tsExpiry);
    }
    else
    {
        Status = InitializeSecurityContextA(hCreds.get(), hContext.get(), NULL, dwSSPIFlags, 0, SECURITY_NATIVE_DREP,
                                            NULL, 0, hContext.get(), &OutBuffer, &dwSSPIOutFlags, &tsExpiry);
    }

    // if(FAILED(Status)) { printf("**** Error 0x%x returned by InitializeSecurityContext\n", Status); goto cleanup; }

    if (FAILED(Status))
    {
        boost::system::error_code ec1(boost::system::errc::protocol_error, boost::system::generic_category());
        do_shutdown3(ec1);
        return;
    }

    pbMessage = OutBuffers[0].pvBuffer;
    cbMessage = OutBuffers[0].cbBuffer;

    // Send the close notify message to the server.
    memcpy(send_buffer.get(), pbMessage, cbMessage);
    send_buffer_end_pos = cbMessage;
    send_buffer_transfer_pos = 0;
    FreeContextBuffer(pbMessage);

    const_buffers send_b;
    send_b.push_back(boost::asio::const_buffer(send_buffer.get(), send_buffer_end_pos));
    _async_write_some(send_b,
                      boost::bind(&TlsSchannelAsyncStreamAdapter::do_shutdown2, shared_from_this(),
                                  boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void TlsSchannelAsyncStreamAdapter::do_shutdown2(const boost::system::error_code& error, size_t bytes_transferred)
{

    boost::mutex::scoped_lock lock(stream_lock);

    if (!open)
    {
        shutingdown = false;
        boost::system::error_code ec1(boost::system::errc::broken_pipe, boost::system::generic_category());
        do_shutdown3(ec1);
        return;
    }

    if (error || bytes_transferred == 0)
    {
        open = false;
        shutingdown = false;
        do_shutdown3(error);
        return;
    }

    if ((send_buffer_transfer_pos + bytes_transferred) < send_buffer_end_pos)
    {
        send_buffer_transfer_pos += boost::numeric_cast<uint32_t>(bytes_transferred);
        const_buffers send_b;
        send_b.push_back(boost::asio::const_buffer(send_buffer.get() + send_buffer_transfer_pos,
                                                   send_buffer_end_pos - send_buffer_transfer_pos));
        _async_write_some(send_b,
                          boost::bind(&TlsSchannelAsyncStreamAdapter::do_shutdown2, shared_from_this(),
                                      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        return;
    }

    shutingdown = false;
    open = false;

    boost::system::error_code ec1;
    do_shutdown3(ec1);

    /*if (async_shutdown_handler_rd)
    {
        boost::function<void ()> async_shutdown_handler_rd1=async_shutdown_handler_rd;
        async_write_op.clear();
        _io_context.post(async_shutdown_handler_rd1);
    }*/

    //_async_write_some.clear();
    //_async_read_some.clear();
}

void TlsSchannelAsyncStreamAdapter::do_shutdown3(const boost::system::error_code& error)
{

    if (request_renegotiate && async_handshake_handler_op)
    {
        boost::function<void(const boost::system::error_code&)> async_handshake_handler1 = async_handshake_handler_op;
        async_handshake_handler_op.clear();
        boost::system::error_code ec1(boost::system::errc::broken_pipe, boost::system::generic_category());
        RR_BOOST_ASIO_POST(_io_context, boost::bind(async_handshake_handler1, ec1));
    }

    if (async_write_op)
    {
        boost::function<void()> async_write_op1 = async_write_op;
        async_write_op.clear();
        RR_BOOST_ASIO_POST(_io_context, async_write_op1);
    }

    if (async_shutdown_handler_op)
    {
        boost::function<void(const boost::system::error_code&)> async_shutdown_handler1 = async_shutdown_handler_op;
        async_shutdown_handler_op.clear();
        RR_BOOST_ASIO_POST(_io_context, boost::bind(async_shutdown_handler1, error));
    }

    if (async_shutdown_handler_rd)
    {
        boost::function<void()> async_shutdown_handler1 = async_shutdown_handler_rd;
        async_shutdown_handler_rd.clear();
        RR_BOOST_ASIO_POST(_io_context, boost::bind(async_shutdown_handler1));
    }
}

TlsSchannelAsyncStreamAdapter::~TlsSchannelAsyncStreamAdapter()
{
    try
    {
        boost::mutex::scoped_lock lock(stream_lock);
        // if (!open) return;

        _async_write_some.clear();
        _async_read_some.clear();
        _close.clear();
        open = false;
    }
    catch (std::exception&)
    {}
}

void TlsSchannelAsyncStreamAdapter::release_context(PCtxtHandle phContext)
{
    if (!phContext)
        return;

    DeleteSecurityContext(phContext);
    delete phContext;
}

void TlsSchannelAsyncStreamAdapter::release_credentials(PCredHandle phCred)
{
    if (!phCred)
        return;

    // The credentials are owned by the context (confusing terminology between SChannel and OpenSSL)
    // Don't free them as they are shared by all sessions
    // FreeCredentialsHandle(phCred);
    delete phCred;
}

bool TlsSchannelAsyncStreamAdapter::VerifyRemoteNodeCertificate(const NodeID& remote_node)
{
    boost::mutex::scoped_lock lock(stream_lock);
    if (!hContext)
        throw InvalidOperationException("Stream not connected");
    PCCERT_CONTEXT c;
    ZeroMemory(&c, sizeof(c));

    if (QueryContextAttributes(hContext.get(), SECPKG_ATTR_REMOTE_CERT_CONTEXT, &c) != SEC_E_OK)
    {
        return false;
    }

    bool res = context->VerifyRemoteNodeCertificate(c, remote_node);
    ::CertFreeCertificateContext(c);

    return res;
}

bool TlsSchannelAsyncStreamAdapter::VerifyRemoteHostnameCertificate(boost::string_ref hostname)
{
    boost::mutex::scoped_lock lock(stream_lock);
    if (!hContext)
        throw InvalidOperationException("Stream not connected");
    PCCERT_CONTEXT c;
    ZeroMemory(&c, sizeof(c));

    if (QueryContextAttributes(hContext.get(), SECPKG_ATTR_REMOTE_CERT_CONTEXT, &c) != SEC_E_OK)
    {
        return false;
    }

    bool res = context->VerifyRemoteHostnameCertificate(c, hostname);
    ::CertFreeCertificateContext(c);

    return res;
}

boost::tuple<std::string, std::string> TlsSchannelAsyncStreamAdapter::GetTlsPublicKeys()
{
    boost::mutex::scoped_lock lock(stream_lock);
    PCCERT_CONTEXT c_l;
    ZeroMemory(&c_l, sizeof(c_l));

    if (QueryContextAttributes(hContext.get(), SECPKG_ATTR_LOCAL_CERT_CONTEXT, &c_l) != SEC_E_OK)
    {
        throw InvalidOperationException("Connection is not secure");
    }

    CRYPT_BIT_BLOB c_l_pk = c_l->pCertInfo->SubjectPublicKeyInfo.PublicKey;
    DWORD c_l_s = 0;
    if (!CryptBinaryToStringA(c_l_pk.pbData, c_l_pk.cbData, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCR, NULL, &c_l_s))
    {
        ::CertFreeCertificateContext(c_l);
        throw InternalErrorException("Internal error");
    }

    std::string c_l_64;

    c_l_64.resize(c_l_s);
    if (!CryptBinaryToStringA(c_l_pk.pbData, c_l_pk.cbData, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCR, &c_l_64[0],
                              &c_l_s))
    {
        ::CertFreeCertificateContext(c_l);
        throw InternalErrorException("Internal error");
    }
    ::CertFreeCertificateContext(c_l);

    // Remote
    PCCERT_CONTEXT c_r;
    ZeroMemory(&c_r, sizeof(c_r));

    if (QueryContextAttributes(hContext.get(), SECPKG_ATTR_REMOTE_CERT_CONTEXT, &c_r) != SEC_E_OK)
    {
        throw InvalidOperationException("Connection is not secure");
    }

    CRYPT_BIT_BLOB c_r_pk = c_r->pCertInfo->SubjectPublicKeyInfo.PublicKey;
    DWORD c_r_s = 0;
    if (!CryptBinaryToStringA(c_r_pk.pbData, c_r_pk.cbData, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCR, NULL, &c_r_s))
    {
        ::CertFreeCertificateContext(c_r);
        throw InternalErrorException("Internal error");
    }

    std::string c_r_64;
    c_r_64.resize(c_r_s);
    if (!CryptBinaryToStringA(c_r_pk.pbData, c_r_pk.cbData, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCR, &c_r_64[0],
                              &c_r_s))
    {
        ::CertFreeCertificateContext(c_r);
        throw InternalErrorException("Internal error");
    }
    ::CertFreeCertificateContext(c_r);

    /*std::cout << c_l_64 << std::endl << std::endl;

    std::cout << c_r_64 << std::endl << std::endl;*/

    return boost::make_tuple(c_l_64, c_r_64);
}

bool TlsSchannelAsyncStreamAdapter::get_mutual_auth() { return this->mutual_auth; }
void TlsSchannelAsyncStreamAdapter::set_mutual_auth(bool mutual_auth) { this->mutual_auth = mutual_auth; }

void TlsSchannelAsyncStreamAdapter::close()
{
    boost::mutex::scoped_lock lock(stream_lock);
    _close();
}
} // namespace detail
} // namespace RobotRaconteur

#endif
