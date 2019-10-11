// Copyright 2011-2019 Wason Technology, LLC
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

std::string ReadFile(const std::string& fname)
{
	ifstream file(fname.c_str(),ios_base::in);

	if (!file.is_open()) throw std::runtime_error("File not found");

	int8_t bom1, bom2, bom3;
	file >> bom1 >> bom2 >> bom3;
	if (!(bom1==-17 && bom2==-69 && bom3==-65))
	{
		file.seekg(0, ifstream::beg);
	}

	stringstream buffer;
	buffer << file.rdbuf();
	file.close();

	return buffer.str();

}

void GenerateCPPFiles(RR_SHARED_PTR<ServiceDefinition> d, std::string def_str, std::string output_dir)
{
	//cout << str << endl;
		
	CPPServiceLangGen::GenerateFiles(d,def_str,output_dir);
}

void GenerateCSharpFiles(RR_SHARED_PTR<ServiceDefinition> d, std::string def_str, std::string output_dir)
{	
	CSharpServiceLangGen::GenerateFiles(d,def_str,output_dir);
}

void GenerateVBNETFiles(boost::shared_ptr<ServiceDefinition> d, string str)
{
	
	
	//VBNETServiceLangGen::GenerateFiles(d,str);
}

void GenerateJavaFiles(RR_SHARED_PTR<ServiceDefinition> d, std::string def_str, std::string output_dir)
{	
	JavaServiceLangGen::GenerateFiles(d,def_str, output_dir);
}

void PullServiceDefinition(string url)
{
	ParseConnectionURLResult url_res = ParseConnectionURL(url);
	std::string url2 = boost::regex_replace(url, boost::regex("(service=[^&]+)"), "service=RobotRaconteurServiceIndex");

	RR_SHARED_PTR<TcpTransport> t=RR_MAKE_SHARED<TcpTransport>();
	RobotRaconteurNode::s()->RegisterTransport(t);
	RobotRaconteurNode::s()->RegisterServiceType(RR_MAKE_SHARED<RobotRaconteurServiceIndex::RobotRaconteurServiceIndexFactory>());

	RR_SHARED_PTR<RobotRaconteurServiceIndex::ServiceIndex> o=rr_cast<RobotRaconteurServiceIndex::ServiceIndex>(RobotRaconteurNode::s()->ConnectService(url2));

	RR_INTRUSIVE_PTR<RRMap<int32_t, RobotRaconteurServiceIndex::ServiceInfo> > ret=o->GetLocalNodeServices();

	if (!ret) throw std::runtime_error("Error retrieving definition");

	string type="";

	for (std::map<int32_t, RR_INTRUSIVE_PTR<RobotRaconteurServiceIndex::ServiceInfo> >::const_iterator ii = ret->begin(); ii != ret->end(); ++ii)
	{
		if (ii->second->Name==url_res.service)
		{
			type=ii->second->RootObjectType;
		}
	}

	if (type=="") throw std::runtime_error("Service not found: " + url_res.service);

	std::vector<RR_SHARED_PTR<ServiceDefinition> > o2=rr_cast<ServiceStub>(o)->GetContext()->PullServiceDefinitionAndImports(SplitQualifiedName(type).get<0>());

	for(std::vector<RR_SHARED_PTR<ServiceDefinition> >::iterator e=o2.begin(); e!=o2.end(); e++)
	{
		std::ofstream f(((*e)->Name + ".robdef").c_str());
		f << trim_copy((*e)->ToString()) << endl;
		f.close();
	}
}

void PrintUsage()
{
	cout << "Robot Raconteur version " << ROBOTRACONTEUR_VERSION_TEXT << endl << endl;
	cout << ("Usage:  RobotRaconteurGen command files") << endl << endl;
	cout << ("Generate thunk source files") << endl;
	cout << ("    RobotRaconteurGen --lang=csharp --thunksource servicedefinition1.robdef servicedefinition2.robdef") << endl << endl;
	cout << ("Generate a new unique node id") << endl;
	cout << ("    RobotRaconteurGen --newnodeid") << endl << endl;
	cout << ("Generate an md5 hash for password files") << endl;
	cout << ("    RobotRaconteurGen --md5passwordhash password") << endl;
	cout << ("Get current version") << endl;
	cout << ("    RobotRaconteurGen --version") << endl;
	
}

void PrintVersion()
{
	cout << "Robot Raconteur version " << ROBOTRACONTEUR_VERSION_TEXT << endl << endl;

}

int main(int argc, char* argv[])
{
	namespace po = boost::program_options;
	namespace fs = boost::filesystem;

	std::string command="";
	std::string libname="";
	std::string lang = "";
	std::string output_dir;
	bool thunksource = false;
	bool newnodeid = false;
	bool md5passwordhash = false;
	bool pullservicedef = false;
	std::vector<std::string> string_vector;
	std::vector<std::string> include_dirs;
	std::vector<std::string> import_vector;
	std::string master_header;
	std::string out_file;

	try
	{
		po::options_description generic_("Allowed options");
		generic_.add_options()
			("help,h", "print usage message")
			("version", "print program version")
			("output-dir", po::value(&output_dir)->default_value("."), "directory for output")
			("thunksource", po::bool_switch(&thunksource), "generate RR thunk source files")
			("newnodeid", po::bool_switch(&newnodeid), "generate a new NodeID")
			("md5passwordhash", po::bool_switch(&md5passwordhash), "hash a password using MD5 algorithm")
			("pullservicedef", po::bool_switch(&pullservicedef), "pull a service definition from a service URL")
			("lang", po::value(&lang), "language to generate thunk code for")
			("include-path,I", po::value(&include_dirs)->composing(), "include path")
			("import", po::value(&import_vector)->composing(), "input file for use in imports")
			("master-header", po::value(&master_header), "master header file for generated cpp files")
			("outfile", po::value(&out_file), "unified output file (csharp only)")

			;

		po::options_description hidden("Hidden options");
		hidden.add_options()
			("input-file", po::value(&string_vector), "input file")
			;

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

		//Find command
		if (thunksource)
		{
			command = "thunksource";
		}
		if (newnodeid)
		{
			if (!command.empty()) throw std::logic_error("more than one command specified");
			command = "newnodeid";
		}
		if (md5passwordhash)
		{
			if (!command.empty()) throw std::logic_error("more than one command specified");
			command = "md5passwordhash";
		}
		if (pullservicedef)
		{
			if (!command.empty()) throw std::logic_error("more than one command specified");
			command = "pullservicedef";
		}
		//End find command

		if (command == "")
		{
			cout << "error: no command specified" << endl;
			cout << generic_ << endl;
			return 1;

		}

		if (command == "newnodeid")
		{
			if (!string_vector.empty()) throw std::logic_error("invalid options for newnodeid");
			if (!lang.empty()) throw std::logic_error("invalid options for newnodeid");
			cout << NodeID::NewUniqueID().ToString() << endl;
			return 0;
		}

		if (command == "md5passwordhash")
		{
			if (string_vector.size() != 1) throw std::logic_error("password not specified");
			if (!lang.empty()) throw std::logic_error("invalid options for md5passwordhash");
			cout << "Password hash for password " << string_vector[0] << " is " << PasswordFileUserAuthenticator::MD5Hash(string_vector[0]) << endl;
			return 0;
		}

		if (command == "pullservicedef")
		{
			if (string_vector.size() != 1) throw std::logic_error("service URL not specified");
			if (!lang.empty()) throw std::logic_error("invalid options for md5passwordhash");
			PullServiceDefinition(string_vector[0]);
			return 0;
		}

		if (command != "thunksource") throw std::logic_error("invalid command specified");
		if (string_vector.empty()) throw std::logic_error("no files specified for thunksource");

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

				BOOST_FOREACH(std::string& s, env_dirs)
				{
					include_dirs.push_back(boost::trim_copy(s));
				}
			}
		}

#ifdef ROBOTRACONTEUR_ROS
		std::vector<std::string> ros_pkg_names;
		if (ros::package::getAll(ros_pkg_names))
		{
			BOOST_FOREACH(const std::string& ros_pkg_name, ros_pkg_names)
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
		BOOST_FOREACH(std::string& s, boost::range::join(sources,imports))
		{
			fs::path s2(s);
			if (!fs::exists(s2) && s2.is_relative())
			{
				BOOST_FOREACH(std::string& p, include_dirs)
				{
					fs::path s3 = fs::path(p) / fs::path(s);
					if (fs::exists(s3))
					{
						s=s3.string();
						break;
					}
				}
			}
		}

		BOOST_FOREACH(std::string& s, boost::range::join(sources, imports))
		{
			if (!fs::exists(s))
			{
				throw std::logic_error("input file not found: " + s);
			}
		}

		if (!fs::exists(output_dir))
		{
			throw std::logic_error("output directory not found: " + output_dir);
		}
		
		std::vector<boost::shared_ptr<ServiceDefinition> > sdefs;
		std::vector<std::string> sdefs_str;
		std::vector<boost::shared_ptr<ServiceDefinition> > alldefs;

		BOOST_FOREACH(std::string& e, boost::range::join(sources, imports))
		{
			try
			{
				boost::shared_ptr<ServiceDefinition> d = boost::make_shared<ServiceDefinition>();
				string str = ReadFile(e);
				d->FromString(str);
				d->CheckVersion();

				alldefs.push_back(d);

				if (boost::range::find(sources, e) != sources.end())
				{
					sdefs.push_back(d);
					sdefs_str.push_back(str);
				}

			}
			catch (std::exception& ee)
			{
				cout << "error: " << string(ee.what()) << " in file " << string(e) << endl;
				cout << "error: Could not open service definition file" << endl;
				return 2;
			}

		}

		try
		{
			std::vector<RobotRaconteurParseException> warnings;
			VerifyServiceDefinitions(alldefs,warnings);
			BOOST_FOREACH(RobotRaconteurParseException w, warnings)
			{
				cout << "warning: " << w.what() << endl;
			}
		}
		catch (std::exception& e)
		{
			cout << "error: " << e.what() << endl;;
			cout << "error: error verifying service definition set" << endl;
			return 2;

		}

		if (lang=="cpp")
		{
			
			for (size_t i=0; i< sdefs.size(); i++)
			{				
				try
				{					
					GenerateCPPFiles(sdefs.at(i), sdefs_str.at(i) ,output_dir);
				}
				catch (std::exception& ee)
				{
					cout << "error: " <<  string(ee.what()) << " in file " << string(sources.at(i)) << endl;
					cout << "error: Could not open service definition file" << endl;
					return 2;
				}
			}

			if (!master_header.empty())
			{
				boost::filesystem::path master_file_path(boost::filesystem::path(output_dir) / master_header);
				std::ofstream master_file_w(master_file_path.c_str());

				CPPServiceLangGen::GenerateMasterHeaderFile(alldefs, &master_file_w);
			}

			return 0;

		}

		if (lang=="csharp")
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
						cout << "error: " << string(ee.what()) << " in file " << string(sources.at(i)) << endl;
						cout << "error: Could not open service definition file" << endl;
						return 2;
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
						cout << "error: " << string(ee.what()) << " in file " << string(sources.at(i)) << endl;
						cout << "error: Could not open service definition file" << endl;
						return 2;
					}
				}

				csharp_w.close();
			}

			return 0;

		}
		
		if (lang=="java")
		{
			for (size_t i = 0; i< sdefs.size(); i++)
			{
				try
				{
					GenerateJavaFiles(sdefs.at(i), sdefs_str.at(i), output_dir);
				}
				catch (std::exception& ee)
				{
					cout << "error: " << string(ee.what()) << " in file " << string(sources.at(i)) << endl;
					cout << "error: Could not open service definition file" << endl;
					return 2;
				}
			}

			return 0;			
		}

		throw std::runtime_error("Unknown language");




	}
	catch (std::exception& e)
	{
		cout << "error: " + string(e.what()) << endl;
		return 1;
	}

	
	
	cout << "unknown error" << endl;
	return -1;
}

