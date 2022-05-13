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

#pragma once

#include <string>

#include <vector>
#include <RobotRaconteur.h>
#include <ostream>

using namespace RobotRaconteur;

namespace RobotRaconteurGen
{
class CPPServiceLangGen
{
  public:
    static std::string fix_name(const std::string& name);

    static std::string fix_qualified_name(const std::string& name);

    static std::string export_definition(ServiceDefinition* def);

    struct convert_type_result
    {
        std::string name;     // o[0]
        std::string cpp_type; // o[1]
        // std::string array_star //o[2] (why?)
    };

    static convert_type_result convert_type(const TypeDefinition& tdef);

    static std::string remove_RR_INTRUSIVE_PTR(const std::string& vartype);

    struct get_variable_type_result
    {
        std::string name;           // o[0]
        std::string cpp_type;       // o[1]
        std::string cpp_param_type; // o[2]
    };

    static get_variable_type_result get_variable_type(const TypeDefinition& tdef, bool usescalar = true);

    static std::string str_pack_parameters(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l, bool inclass = true);

    static std::string str_pack_message_element(const std::string& elementname, const std::string& varname,
                                                const RR_SHARED_PTR<TypeDefinition>& t,
                                                const std::string& packer = "context");

    static std::string str_unpack_message_element(const std::string& varname, const RR_SHARED_PTR<TypeDefinition>& t,
                                                  const std::string& packer = "context");

    static std::string convert_constant(ConstantDefinition* constant,
                                        std::vector<RR_SHARED_PTR<ConstantDefinition> >& c2, ServiceDefinition* def);

    struct convert_generator_result
    {
        std::string generator_cpp_type;
        std::string return_type;
        std::string param_type;
        std::vector<RR_SHARED_PTR<TypeDefinition> > params;
    };

    static convert_generator_result convert_generator(FunctionDefinition* f);

    static bool is_member_override(MemberDefinition* m, const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs);

    static void GenerateInterfaceHeaderFile(ServiceDefinition* d,
                                            const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs,
                                            const std::vector<std::string>& extra_include, std::ostream* w);

    static std::string GetPropertyDeclaration(PropertyDefinition* d, bool inclass = true);

    static std::string SetPropertyDeclaration(PropertyDefinition* d, bool inclass = true);

    static std::string FunctionDeclaration(FunctionDefinition* d, bool inclass = true);

    static std::string GetPropertyDeclaration_async(PropertyDefinition* d, bool inclass = true);

    static std::string SetPropertyDeclaration_async(PropertyDefinition* d, bool inclass = true);

    static std::string FunctionDeclaration_async(FunctionDefinition* d, bool inclass = true);

    static std::string EventDeclaration(EventDefinition* d, bool inclass = true, bool var = false);

    static std::string ObjRefDeclaration(ObjRefDefinition* d, bool inclass = true);

    static std::string ObjRefDeclaration_async(ObjRefDefinition* d, bool inclass = true);

    static std::string GetPipeDeclaration(PipeDefinition* d, bool inclass = true);

    static std::string SetPipeDeclaration(PipeDefinition* d, bool inclass = true);

    static std::string GetCallbackDeclaration(CallbackDefinition* d, bool inclass = true, bool var = false);

    static std::string SetCallbackDeclaration(CallbackDefinition* d, bool inclass = true);

    static std::string GetWireDeclaration(WireDefinition* d, bool inclass = true);

    static std::string SetWireDeclaration(WireDefinition* d, bool inclass = true);

    static std::string MemoryDeclaration(MemoryDefinition* d, bool inclass = true);

    static void GenerateStubSkelHeaderFile(ServiceDefinition* d,
                                           const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs, std::ostream* w);

    static void GenerateStubSkelFile(ServiceDefinition* d, const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs,
                                     std::ostream* w, const std::string& servicedef);

    static void GenerateServiceFactoryHeader(ServiceDefinition* d, std::ostream* w);

    static void GenerateServiceFactory(ServiceDefinition* d, std::ostream* w, const std::string& defstring);

    static void GenerateStubHeader(ServiceDefinition* d,
                                   const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs, std::ostream* w);

    static void GenerateSkelHeader(ServiceDefinition* d,
                                   const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs, std::ostream* w);

    static void GenerateStubDefinition(ServiceDefinition* d,
                                       const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                       std::ostream* w);

    static void GenerateSkelDefinition(ServiceDefinition* d,
                                       const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                       std::ostream* w);

    static void GenerateConstants(ServiceDefinition* d, std::ostream* w);

    static std::string GetDefaultValue(const TypeDefinition& tdef);

    static std::string GetDefaultInitializedValue(const TypeDefinition& tdef);

    static void GenerateDefaultImplHeader(ServiceDefinition* d,
                                          const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                          bool is_abstract, std::ostream* w);

    static void GenerateDefaultImplDefinition(ServiceDefinition* d,
                                              const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                              bool is_abstract, std::ostream* w);

    static void GenerateDocString(const std::string& docstring, const std::string& prefix, std::ostream* w);

    // File generators
    static void GenerateFiles(const RR_SHARED_PTR<ServiceDefinition>& d, const std::string& servicedef,
                              const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                              const std::vector<std::string>& extra_include, const std::string& path = ".");

    // Master header file
    static void GenerateMasterHeaderFile(const std::vector<RR_SHARED_PTR<ServiceDefinition> >& d, std::ostream* w);
};

} // namespace RobotRaconteurGen