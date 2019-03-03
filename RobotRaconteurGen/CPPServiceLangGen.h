// Copyright 2011-2018 Wason Technology, LLC
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

#pragma once

#include <string>

#include <vector>
#include <RobotRaconteur.h>
#include <ostream>

using namespace std;
using namespace boost;
using namespace RobotRaconteur;

namespace RobotRaconteurGen
{
	class CPPServiceLangGen
	{
	public:

		static std::string fix_name(const std::string& name);

		static std::string fix_qualified_name(const std::string& name);

		struct convert_type_result
		{
			std::string name; // o[0]
			std::string cpp_type; //o[1]
			//std::string array_star //o[2] (why?)
		};

		static convert_type_result convert_type(const TypeDefinition& tdef);

		static string remove_RR_INTRUSIVE_PTR(const string& vartype);

		struct get_variable_type_result
		{
			std::string name; //o[0]
			std::string cpp_type; //o[1]
			std::string cpp_param_type; //o[2]
		};

		static get_variable_type_result get_variable_type(const TypeDefinition& tdef, bool usescalar=true);

		static string str_pack_parameters(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l, bool inclass=true);

		static std::string str_pack_message_element(const std::string &elementname, const std::string &varname, const RR_SHARED_PTR<TypeDefinition> &t, const std::string &packer = "context");

		static std::string str_unpack_message_element(const std::string &varname, const RR_SHARED_PTR<TypeDefinition> &t, const std::string &packer = "context");

		static std::string convert_constant(ConstantDefinition* constant, std::vector<RR_SHARED_PTR<ConstantDefinition> >& c2, ServiceDefinition* def);

		struct convert_generator_result
		{
			std::string generator_cpp_type;
			std::string return_type;
			std::string param_type;
			std::vector<RR_SHARED_PTR<TypeDefinition> > params;
		};

		static convert_generator_result convert_generator(FunctionDefinition* f);

		static void GenerateInterfaceHeaderFile(ServiceDefinition* d, ostream* w);

		static string GetPropertyDeclaration(PropertyDefinition* d, bool inclass=true);

		static string SetPropertyDeclaration(PropertyDefinition* d, bool inclass=true);
		
		static string FunctionDeclaration(FunctionDefinition *d, bool inclass=true);

		static string GetPropertyDeclaration_async(PropertyDefinition* d, bool inclass=true);

		static string SetPropertyDeclaration_async(PropertyDefinition* d, bool inclass=true);

		static string FunctionDeclaration_async(FunctionDefinition *d, bool inclass=true);

		static string EventDeclaration(EventDefinition* d, bool inclass=true, bool var=false);

		static string ObjRefDeclaration(ObjRefDefinition* d, bool inclass=true);

		static string ObjRefDeclaration_async(ObjRefDefinition* d, bool inclass=true);

		static string GetPipeDeclaration(PipeDefinition* d, bool inclass=true);

		static string SetPipeDeclaration(PipeDefinition* d, bool inclass=true);

		static string GetCallbackDeclaration(CallbackDefinition* d, bool inclass=true, bool var=false);

		static string SetCallbackDeclaration(CallbackDefinition* d, bool inclass=true);

		static string GetWireDeclaration(WireDefinition* d, bool inclass=true);

		static string SetWireDeclaration(WireDefinition* d, bool inclass=true);

		static string MemoryDeclaration(MemoryDefinition* d, bool inclass=true);

		static void GenerateStubSkelHeaderFile(ServiceDefinition* d, ostream* w);

		static void GenerateStubSkelFile(ServiceDefinition* d, ostream* w, string servicedef);

		static void GenerateServiceFactoryHeader(ServiceDefinition* d, ostream* w);

		static void GenerateServiceFactory(ServiceDefinition* d, ostream* w, string defstring);

		static void GenerateStubHeader(ServiceDefinition *d, ostream* w);

		static void GenerateSkelHeader(ServiceDefinition *d, ostream* w);

		static void GenerateStubDefinition(ServiceDefinition *d, ostream* w);

		static void GenerateSkelDefinition(ServiceDefinition *d, ostream* w);
	
		static void GenerateConstants(ServiceDefinition*d, ostream* w);

		static std::string GetDefaultValue(const TypeDefinition& tdef);

		static std::string GetDefaultInitializedValue(const TypeDefinition& tdef);

		static void GenerateDefaultImplHeader(ServiceDefinition *d, ostream* w);

		static void GenerateDefaultImplDefinition(ServiceDefinition* d, ostream* w);

		//File generators
		static void GenerateFiles(RR_SHARED_PTR<ServiceDefinition> d, std::string servicedef, std::string path=".");

		//Master header file
		static void GenerateMasterHeaderFile(std::vector<RR_SHARED_PTR<ServiceDefinition> > d, ostream* w);
		
	};



}