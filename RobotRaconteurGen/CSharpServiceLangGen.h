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

using namespace std;
using namespace boost;
using namespace RobotRaconteur;

namespace RobotRaconteurGen
{
class CSharpServiceLangGen
{
  public:
    static std::string fix_name(const std::string& name);

    static std::string fix_qualified_name(const std::string& name);

    struct convert_type_result
    {
        std::string name;
        std::string cs_type;
        std::string cs_arr_type;
    };

    static convert_type_result convert_type(const TypeDefinition& tdef);

    struct convert_generator_result
    {
        std::string generator_csharp_type;
        std::string generator_csharp_base_type;
        std::string generator_csharp_template_params;
        std::vector<RR_SHARED_PTR<TypeDefinition> > params;
    };

    static convert_generator_result convert_generator(FunctionDefinition* f);

    static string str_pack_delegate(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l,
                                    boost::shared_ptr<TypeDefinition> rettype = boost::shared_ptr<TypeDefinition>());

    static string str_pack_parameters(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l, bool inclass);

    static std::string str_pack_message_element(const std::string& elementname, const std::string& varname,
                                                const RR_SHARED_PTR<TypeDefinition>& t,
                                                const std::string& packer = "context");

    static std::string str_unpack_message_element(const std::string& varname, const RR_SHARED_PTR<TypeDefinition>& t,
                                                  const std::string& packer = "context");

    static std::string convert_constant(ConstantDefinition* constant,
                                        std::vector<RR_SHARED_PTR<ConstantDefinition> >& c2, ServiceDefinition* def);

    static bool GetObjRefIndType(RR_SHARED_PTR<ObjRefDefinition>& m, std::string& indtype);

    static void GenerateStructure(ServiceEntryDefinition* e, ostream* w);

    static void GeneratePod(RR_SHARED_PTR<ServiceEntryDefinition> e, ostream* w);

    static void GenerateNamedArrayExtensions(RR_SHARED_PTR<ServiceEntryDefinition> e, ostream* w);

    static void GenerateInterface(ServiceEntryDefinition* e, ostream* w);

    static void GenerateInterfaceFile(ServiceDefinition* d, ostream* w, bool header = true);

    static void GenerateStubSkelFile(ServiceDefinition* d, std::string defstring, ostream* w, bool header = true);

    static void GenerateServiceFactory(ServiceDefinition* d, std::string defstring, ostream* w);

    static void GenerateStructureStub(ServiceEntryDefinition* e, ostream* w);

    static void GenerateNamedArrayStub(RR_SHARED_PTR<ServiceEntryDefinition> e, ostream* w);

    static void GeneratePodStub(ServiceEntryDefinition* e, ostream* w);

    static void GenerateStub(ServiceEntryDefinition* e, ostream* w);

    static void GenerateSkel(ServiceEntryDefinition* e, ostream* w);

    static void GenerateDefaultImpl(ServiceEntryDefinition* e, ostream* w);

    static void GenerateConstants(ServiceDefinition* d, ostream* w);

    static std::string GetDefaultValue(const TypeDefinition& tdef);

    static std::string GetDefaultInitializedValue(const TypeDefinition& tdef);

    static void GenerateDocString(const std::string& docstring, const std::string& prefix, ostream* w);

    // File generators
    static void GenerateFiles(RR_SHARED_PTR<ServiceDefinition> d, std::string servicedef, std::string path = ".");

    static void GenerateOneFileHeader(ostream* w);

    static void GenerateOneFilePart(RR_SHARED_PTR<ServiceDefinition> d, std::string servicedef, ostream* w);
};

} // namespace RobotRaconteurGen