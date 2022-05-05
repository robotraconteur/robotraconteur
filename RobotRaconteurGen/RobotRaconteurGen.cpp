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

#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

// RobotRaconteurGen.cpp : Defines the entry point for the console application.
//

#include <fstream>
#include <streambuf>
#include <sstream>
#include <RobotRaconteur.h>
#include "CPPServiceLangGen.h"
#include "CSharpServiceLangGen.h"
#include "JavaServiceLangGen.h"
#include "StringTableGen.h"
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/join.hpp>
#include <RobotRaconteur/RobotRaconteurServiceIndex.h>
#include <RobotRaconteur/RobotRaconteurServiceIndex_stubskel.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#ifdef ROBOTRACONTEUR_ROS
#include "ros/package.h"
#endif

using namespace std;
using namespace RobotRaconteur;
using namespace RobotRaconteurGen;
using std::exception;

bool ReadFile(std::string& file_contents, const std::string& fname)
{
    try
    {
        ifstream file;
        file.open(fname.c_str(), ios_base::in);

        if (!file)
            return false;

        int8_t bom1, bom2, bom3;
        file >> bom1 >> bom2 >> bom3;
        if (!(bom1 == -17 && bom2 == -69 && bom3 == -65))
        {
            file.seekg(0, ifstream::beg);
        }

        stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        file_contents = buffer.str();

        return true;
    }
    catch (std::exception&)
    {
        return false;
    }
}

boost::tuple<RR_SHARED_PTR<ServiceDefinition>, std::string> ReadRobDefFile(const std::string& fname,
                                                                           const std::vector<std::string>& include_dirs,
                                                                           bool try_find = false)
{
    namespace fs = boost::filesystem;

    std::string s = fname;

    fs::path s2(s);
    if (!fs::exists(s2) && s2.is_relative())
    {
        BOOST_FOREACH (const std::string& p, include_dirs)
        {
            fs::path s3 = fs::path(p) / fs::path(s);
            if (fs::exists(s3))
            {
                s = s3.string();
                break;
            }
        }
    }

    if (!fs::exists(s))
    {
        if (try_find)
        {
            return boost::make_tuple(RR_SHARED_PTR<ServiceDefinition>(), "");
        }
        std::cout << "RobotRaconteurGen: fatal error: input file not found " << s << std::endl;
        exit(1002);
    }

    try
    {
        boost::shared_ptr<ServiceDefinition> d = boost::make_shared<ServiceDefinition>();
        std::string file_contents;
        if (!ReadFile(file_contents, s))
        {
            std::cout << "RobotRaconteurGen: fatal error: could not open file " << s << std::endl;
            exit(1004);
        }
        ServiceDefinitionParseInfo parse_info;
        parse_info.RobDefFilePath = s;
        d->FromString(file_contents, &parse_info);
        d->CheckVersion();

        return boost::make_tuple(d, file_contents);
    }
    catch (ServiceDefinitionParseException& ee)
    {
        cout << s << "(" << ee.ParseInfo.LineNumber << "): error: " << ee.ShortMessage << endl;
        exit(1005);
    }
    catch (std::exception& ee)
    {
        cout << s << ": error: " << string(ee.what()) << endl;
        exit(1006);
    }
}

void GenerateCPPFiles(RR_SHARED_PTR<ServiceDefinition> d, std::string def_str,
                      std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs,
                      const std::vector<std::string>& cpp_extra_include, std::string output_dir)
{
    // cout << str << endl;

    CPPServiceLangGen::GenerateFiles(d, def_str, other_defs, cpp_extra_include, output_dir);
}

void GenerateCSharpFiles(RR_SHARED_PTR<ServiceDefinition> d, std::string def_str, std::string output_dir)
{
    CSharpServiceLangGen::GenerateFiles(d, def_str, output_dir);
}

void GenerateVBNETFiles(boost::shared_ptr<ServiceDefinition> d, string str)
{

    // VBNETServiceLangGen::GenerateFiles(d,str);
}

void GenerateJavaFiles(RR_SHARED_PTR<ServiceDefinition> d, std::string def_str, std::string output_dir)
{
    JavaServiceLangGen::GenerateFiles(d, def_str, output_dir);
}

int PullServiceDefinition(string url)
{
    ParseConnectionURLResult url_res;
    try
    {
        url_res = ParseConnectionURL(url);
    }
    catch (std::exception&)
    {
        std::cout << "RobotRaconteurGen: fatal error: could not parse specified URL" << std::endl;
        return 6001;
    }

    std::string url2;
    try
    {
        url2 = boost::regex_replace(url, boost::regex("(service=[^&]+)"), "service=RobotRaconteurServiceIndex");
    }
    catch (std::exception&)
    {
        std::cout << "RobotRaconteurGen: fatal error: invalid service in specified URL" << std::endl;
        return 6002;
    }

    try
    {
        RR_SHARED_PTR<TcpTransport> t = RR_MAKE_SHARED<TcpTransport>();
        RobotRaconteurNode::s()->RegisterTransport(t);
    }
    catch (std::exception& e)
    {
        std::cout << "RobotRaconteurGen: fatal error: could not initialize Robot Raconteur client node: " << e.what()
                  << std::endl;
        return 6003;
    }

    RR_SHARED_PTR<RobotRaconteurServiceIndex::ServiceIndex> o;

    try
    {
        o = rr_cast<RobotRaconteurServiceIndex::ServiceIndex>(RobotRaconteurNode::s()->ConnectService(url2));
    }
    catch (RobotRaconteurException e)
    {
        std::cout << "RobotRaconteurGen: fatal error: could not connect to specified URL: " << e.Message << std::endl;
        return 6004;
    }
    catch (std::exception& e)
    {
        std::cout << "RobotRaconteurGen: fatal error: could not connect to specified URL: " << e.what() << std::endl;
        return 6005;
    }

    RR_INTRUSIVE_PTR<RRMap<int32_t, RobotRaconteurServiceIndex::ServiceInfo> > ret = o->GetLocalNodeServices();

    if (!ret)
    {
        std::cout << "RobotRaconteurGen: error: could not retrieve service definition from " + url;
        return 6006;
    }

    string type = "";

    for (std::map<int32_t, RR_INTRUSIVE_PTR<RobotRaconteurServiceIndex::ServiceInfo> >::const_iterator ii =
             ret->begin();
         ii != ret->end(); ++ii)
    {
        if (ii->second->Name == url_res.service)
        {
            type = ii->second->RootObjectType;
        }
    }

    if (type == "")
    {
        std::cout << "RobotRaconteurGen: error: service not found on remote node: " + url_res.service;
        return 6006;
    }

    std::vector<RR_SHARED_PTR<ServiceDefinition> > o2;
    try
    {
        o2 = rr_cast<ServiceStub>(o)
                 ->GetContext()
                 ->PullServiceDefinitionAndImports(SplitQualifiedName(type).get<0>())
                 .defs;
    }
    catch (RobotRaconteurException e)
    {
        std::cout << "RobotRaconteurGen: fatal error: could not pull service definition: " << e.Message << std::endl;
        return 6007;
    }
    catch (std::exception& e)
    {
        std::cout << "RobotRaconteurGen: fatal error: could not pull service definition: " << e.what() << std::endl;
        return 6008;
    }

    for (std::vector<RR_SHARED_PTR<ServiceDefinition> >::iterator e = o2.begin(); e != o2.end(); e++)
    {
        std::string fname = (*e)->Name + ".robdef";
        try
        {
            std::ofstream f(fname.c_str());
            f << trim_copy((*e)->ToString()) << endl;
            f.close();
        }
        catch (std::exception& e)
        {
            std::cout << "RobotRaconteurGen: fatal error: could not save service definition file " << fname << " : "
                      << e.what() << std::endl;
            return 6009;
        }
    }
}

int main(int argc, char* argv[])
{
    namespace po = boost::program_options;
    namespace fs = boost::filesystem;

    std::string command = "";
    std::string libname = "";
    std::string lang = "";
    std::string output_dir;
    bool thunksource = false;
    bool verify_robdef = false;
    bool string_table = false;
    bool newnodeid = false;
    bool md5passwordhash = false;
    bool pullservicedef = false;
    bool auto_import = false;
    std::vector<std::string> string_vector;
    std::vector<std::string> include_dirs;
    std::vector<std::string> import_vector;
    std::string master_header;
    std::string out_file;
    std::vector<std::string> cpp_extra_include;

    try
    {
        po::options_description generic_("Allowed options");
        generic_.add_options()("help,h", "print usage message")("version", "print program version")(
            "output-dir", po::value(&output_dir)->default_value("."),
            "directory for output")("thunksource", po::bool_switch(&thunksource), "generate RR thunk source files")(
            "verify-robdef", po::bool_switch(&verify_robdef), "verify robdef files")(
            "string-table", po::bool_switch(&string_table), "generate string table entries for robdef")(
            "newnodeid", po::bool_switch(&newnodeid), "generate a new NodeID")(
            "md5passwordhash", po::bool_switch(&md5passwordhash), "hash a password using MD5 algorithm")(
            "pullservicedef", po::bool_switch(&pullservicedef), "pull a service definition from a service URL")(
            "lang", po::value(&lang), "language to generate thunk code for")(
            "include-path,I", po::value(&include_dirs)->composing(),
            "include path")("import", po::value(&import_vector)->composing(), "input file for use in imports")(
            "master-header", po::value(&master_header), "master header file for generated cpp files")(
            "outfile", po::value(&out_file), "unified output file (csharp only)")(
            "auto-import", po::bool_switch(&auto_import),
            "automatically load imported robdef")("cpp-extra-include", po::value(&cpp_extra_include)->composing(),
                                                  "extra include files for C++ thunk headers")

            ;

        po::options_description hidden("Hidden options");
        hidden.add_options()("input-file", po::value(&string_vector), "input file");

        po::positional_options_description p;
        p.add("input-file", -1);

        po::options_description cmdline_options;
        cmdline_options.add(generic_).add(hidden);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            cout << "RobotRaconteurGen version " << ROBOTRACONTEUR_VERSION_TEXT << endl << endl;
            cout << generic_ << endl;
            return 0;
        }

        if (vm.count("version"))
        {
            cout << "RobotRaconteurGen version " << ROBOTRACONTEUR_VERSION_TEXT << endl;
            return 0;
        }

        // Find command
        if (thunksource)
        {
            command = "thunksource";
        }
        else if (verify_robdef)
        {
            command = "verify-robdef";
        }
        else if (string_table)
        {
            command = "string-table";
        }
        if (newnodeid)
        {
            if (!command.empty())
            {
                std::cout << "RobotRaconteurGen: fatal error: more than one command specified" << std::endl;
                return 1;
            }
            command = "newnodeid";
        }
        if (md5passwordhash)
        {
            if (!command.empty())
            {
                std::cout << "RobotRaconteurGen: fatal error: more than one command specified" << std::endl;
                return 1;
            }
            command = "md5passwordhash";
        }
        if (pullservicedef)
        {
            if (!command.empty())
            {
                std::cout << "RobotRaconteurGen: fatal error: more than one command specified" << std::endl;
                return 1;
            }
            command = "pullservicedef";
        }
        // End find command

        if (command == "")
        {

            std::cout << "RobotRaconteurGen: fatal error: no command specified" << std::endl;
            cout << generic_ << endl;
            return 1;
        }

        if (command == "newnodeid")
        {
            if (!string_vector.empty() || !lang.empty())
            {
                std::cout << "RobotRaconteurGen: fatal error: invalid options for newnodeid" << std::endl;
                return 2001;
            }

            cout << NodeID::NewUniqueID().ToString() << endl;
            return 0;
        }

        if (command == "md5passwordhash")
        {
            if (string_vector.size() != 1)
            {
                std::cout << "RobotRaconteurGen: fatal error: password not specified" << std::endl;
                return 3001;
            }
            if (!lang.empty())
            {
                std::cout << "RobotRaconteurGen: fatal error: invalid options for md5passwordhash" << std::endl;
                return 3001;
            }
            cout << "Password hash for password " << string_vector.at(0) << " is "
                 << PasswordFileUserAuthenticator::MD5Hash(string_vector[0]) << endl;
            return 0;
        }

        if (command == "pullservicedef")
        {
            if (string_vector.size() != 1)
            {
                std::cout << "RobotRaconteurGen: fatal error: service url not specified" << std::endl;
                return 4001;
            }
            if (!lang.empty())
            {
                std::cout << "RobotRaconteurGen: fatal error: invalid options for pullservicedef" << std::endl;
                return 4002;
            }
            return PullServiceDefinition(string_vector[0]);
        }

        if (command != "thunksource" && command != "verify-robdef" && command != "string-table")
        {
            std::cout << "RobotRaconteurGen: fatal error: invalid command specified" << std::endl;
            return 1;
        }

        if (string_vector.empty())
        {
            std::cout << "RobotRaconteurGen: fatal error: no files specified for thunksource" << std::endl;
            return 1001;
        }

        char* robdef_path_c = std::getenv("ROBOTRACONTEUR_ROBDEF_PATH");
        if (robdef_path_c)
        {
            std::string robdef_path(robdef_path_c);
            boost::trim(robdef_path);

            std::vector<std::string> env_dirs;
            if (!robdef_path.empty())
            {

#ifdef BOOST_WINDOWS
                boost::split(env_dirs, robdef_path, boost::is_any_of(";"));
#else
                boost::split(env_dirs, robdef_path, boost::is_any_of(":"));
#endif

                BOOST_FOREACH (std::string& s, env_dirs)
                {
                    include_dirs.push_back(boost::trim_copy(s));
                }
            }
        }

#ifdef ROBOTRACONTEUR_ROS
        std::vector<std::string> ros_pkg_names;
        if (ros::package::getAll(ros_pkg_names))
        {
            BOOST_FOREACH (const std::string& ros_pkg_name, ros_pkg_names)
            {
                std::string ros_pkg_path_s = ros::package::getPath(ros_pkg_name);
                if (!ros_pkg_path_s.empty())
                {
                    boost::filesystem::path ros_pkg_path = boost::filesystem::path(ros_pkg_path_s) / "robdef";
                    include_dirs.push_back(ros_pkg_path.string());
                }
            }
        }

#endif

        std::vector<std::string> sources = string_vector;
        std::vector<std::string> imports = import_vector;

        if (!fs::exists(output_dir))
        {
            std::cout << "RobotRaconteurGen: fatal error: output directory not found " << output_dir << std::endl;
            return 1003;
        }

        std::vector<boost::shared_ptr<ServiceDefinition> > sdefs;
        std::vector<std::string> sdefs_str;
        std::vector<boost::shared_ptr<ServiceDefinition> > alldefs;
        std::vector<std::string> alldefs_str;

        std::set<std::string> required_imports;

        BOOST_FOREACH (std::string& e, boost::range::join(sources, imports))
        {
            RR_SHARED_PTR<ServiceDefinition> d;
            std::string file_contents;
            boost::tie(d, file_contents) = ReadRobDefFile(e, include_dirs);
            alldefs.push_back(d);
            alldefs_str.push_back(file_contents);

            if (boost::range::find(sources, e) != sources.end())
            {
                sdefs.push_back(d);
                sdefs_str.push_back(file_contents);
            }

            BOOST_FOREACH (std::string imp, d->Imports)
            {
                required_imports.insert(imp);
            }
        }

        if (auto_import)
        {
            std::set<std::string> missing_imports;
            BOOST_FOREACH (const std::string s, required_imports)
            {
                if (!RobotRaconteur::TryFindByName(alldefs, s))
                {
                    missing_imports.insert(s);
                }
            }

            std::set<std::string> attempted_imports;
            while (!missing_imports.empty())
            {
                std::string e = *missing_imports.begin();
                missing_imports.erase(e);
                RR_SHARED_PTR<ServiceDefinition> d;
                std::string file_contents;
                boost::tie(d, file_contents) = ReadRobDefFile(e + ".robdef", include_dirs, true);
                if (d)
                {
                    alldefs.push_back(d);
                    alldefs_str.push_back(file_contents);

                    BOOST_FOREACH (std::string imp, d->Imports)
                    {
                        if (!RobotRaconteur::TryFindByName(alldefs, imp) && attempted_imports.count(imp) == 0)
                        {
                            missing_imports.insert(imp);
                        }
                    }
                }
                else
                {
                    attempted_imports.insert(e);
                }
            }
        }

        try
        {
            std::vector<ServiceDefinitionParseException> warnings;
            VerifyServiceDefinitions(alldefs, warnings);
            BOOST_FOREACH (ServiceDefinitionParseException w, warnings)
            {
                cout << w.ParseInfo.RobDefFilePath << "(" << w.ParseInfo.LineNumber << "): warning: " << w.ShortMessage
                     << endl;
            }
        }
        catch (ServiceDefinitionParseException& ee)
        {
            cout << ee.ParseInfo.RobDefFilePath << "(" << ee.ParseInfo.LineNumber << "): error: " << ee.ShortMessage
                 << endl;
            return 1007;
        }
        catch (ServiceDefinitionVerifyException& ee)
        {
            cout << ee.ParseInfo.RobDefFilePath << "(" << ee.ParseInfo.LineNumber << "): error: " << ee.ShortMessage
                 << endl;
            return 1008;
        }
        catch (std::exception& ee)
        {
            cout << "RobotRaconteurGen: fatal error: could not verify service definition set " << string(ee.what())
                 << endl;
            return 1009;
        }

        if (string_table)
        {
            std::set<std::string> strings = GenerateStringTable(sdefs, alldefs);
            std::cout << "robdef message strings:" << std::endl << std::endl;
            BOOST_FOREACH (const std::string& s, strings)
            {
                std::cout << s << std::endl;
            }
        }

        if (verify_robdef)
        {
            for (size_t i = 0; i < alldefs.size(); i++)
            {
                RR_SHARED_PTR<ServiceDefinition> d = alldefs.at(i);
                if (d->StdVer < RobotRaconteurVersion(0, 9, 2))
                {
                    cout << d->ParseInfo.RobDefFilePath
                         << "(1): error: stdver 0.9.2 or greater required for service definition verification" << endl;

                    return 1020;
                }

                // TODO: Verify robdef using regex??
                /*std::vector<std::string> robdef_msgs;
                if (!VerifyServiceDefinitionMatchesStandardRegex(alldefs_str.at(i), robdef_msgs))
                {
                    cout << d->ParseInfo.RobDefFilePath << "(1): error: service definition does not match stdver 0.9.2
                syntax regex" << endl; return 1020;
                }*/
            }
        }

        if (!thunksource)
        {
            return 0;
        }

        if (lang == "cpp")
        {

            for (size_t i = 0; i < sdefs.size(); i++)
            {
                try
                {
                    std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs = alldefs;
                    other_defs.erase(boost::range::remove(other_defs, sdefs.at(i)), other_defs.end());

                    GenerateCPPFiles(sdefs.at(i), sdefs_str.at(i), other_defs, cpp_extra_include, output_dir);
                }
                catch (std::exception& ee)
                {
                    cout << sources.at(i) << ": error: could not generate thunksource files " << string(ee.what())
                         << endl;
                    return 1010;
                }
            }

            if (!master_header.empty())
            {
                try
                {
                    boost::filesystem::path master_file_path(boost::filesystem::path(output_dir) / master_header);
                    std::ofstream master_file_w(master_file_path.c_str());

                    CPPServiceLangGen::GenerateMasterHeaderFile(alldefs, &master_file_w);
                }
                catch (std::exception& ee)
                {
                    cout << "RobotRaconteurGen: error: could not generate master header file " << string(ee.what())
                         << endl;
                    return 1011;
                }
            }
            return 0;
        }

        if (lang == "csharp")
        {

            if (out_file.empty())
            {

                for (size_t i = 0; i < sdefs.size(); i++)
                {
                    try
                    {
                        GenerateCSharpFiles(sdefs.at(i), sdefs_str.at(i), output_dir);
                    }
                    catch (std::exception& ee)
                    {
                        cout << sources.at(i) << ": error: could not generate thunksource files " << string(ee.what())
                             << endl;
                        return 1010;
                    }
                }
            }
            else
            {
                std::ofstream csharp_w(out_file.c_str());
                CSharpServiceLangGen::GenerateOneFileHeader(&csharp_w);
                for (size_t i = 0; i < sdefs.size(); i++)
                {
                    try
                    {
                        CSharpServiceLangGen::GenerateOneFilePart(sdefs.at(i), sdefs_str.at(i), &csharp_w);
                    }
                    catch (std::exception& ee)
                    {
                        cout << sources.at(i) << ": error: could not generate thunksource files " << string(ee.what())
                             << endl;
                        return 1010;
                    }
                }

                csharp_w.close();
            }

            return 0;
        }

        if (lang == "java")
        {
            for (size_t i = 0; i < sdefs.size(); i++)
            {
                try
                {
                    GenerateJavaFiles(sdefs.at(i), sdefs_str.at(i), output_dir);
                }
                catch (std::exception& ee)
                {
                    cout << sources.at(i) << ": error: could not generate thunksource files " << string(ee.what())
                         << endl;
                    return 1010;
                }
            }

            return 0;
        }

        cout << "RobotRaconteurGen: fatal error: invalid lang specified for thunksource" << endl;
        return 1012;
    }
    catch (po::error& e)
    {
        cout << "RobotRaconteurGen: error: " << string(e.what()) << endl;
        return 5;
    }
    catch (std::exception& e)
    {
        cout << "RobotRaconteurGen: error: internal error " << string(e.what()) << endl;
        return 6;
    }

    cout << "RobotRaconteurGen: error: unknown internal error" << endl;
    return 7;
}
