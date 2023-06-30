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
class JavaServiceLangGen
{
  public:
    static std::string fix_name(const std::string& name);

    static std::string fix_qualified_name(const std::string& name);

    struct convert_type_result
    {
        std::string name;
        std::string java_type;
        std::string java_arr_type;
    };

    static convert_type_result convert_type(const TypeDefinition& tdef, bool reftypes = false);

    static convert_type_result convert_type_array(TypeDefinition tdef);

    static std::string str_pack_delegate(
        const std::vector<RR_SHARED_PTR<TypeDefinition> >& l,
        const boost::shared_ptr<TypeDefinition>& rettype = boost::shared_ptr<TypeDefinition>());

    static std::string str_pack_parameters(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l, bool inclass);

    static std::string str_pack_parameters_delegate(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l, bool inclass);

    static std::string str_pack_message_element(const std::string& elementname, const std::string& varname,
                                                const RR_SHARED_PTR<TypeDefinition>& t,
                                                const std::string& packer = "context");

    static std::string str_unpack_message_element(const std::string& varname, const RR_SHARED_PTR<TypeDefinition>& t,
                                                  const std::string& packer = "context");

    static std::string convert_constant(ConstantDefinition* constant,
                                        std::vector<RR_SHARED_PTR<ConstantDefinition> >& c2, ServiceDefinition* def);

    struct convert_generator_result
    {
        std::string generator_java_type;
        std::string generator_java_base_type;
        std::string generator_java_template_params;
        std::vector<RR_SHARED_PTR<TypeDefinition> > params;
    };

    static convert_generator_result convert_generator(FunctionDefinition* f);

    static bool GetObjRefIndType(RR_SHARED_PTR<ObjRefDefinition>& m, std::string& indtype);

    static void GenerateStructure(ServiceEntryDefinition* e, std::ostream* w);

    static void GeneratePod(ServiceEntryDefinition* e, std::ostream* w);

    static void GenerateNamedArray(const RR_SHARED_PTR<ServiceEntryDefinition>& e, std::ostream* w);

    static void GenerateInterface(ServiceEntryDefinition* e, std::ostream* w);

    static void GenerateAsyncInterface(ServiceEntryDefinition* e, std::ostream* w);

    static void GenerateServiceFactory(ServiceDefinition* d, const std::string& defstring, std::ostream* w);

    static void GenerateStructureStub(ServiceEntryDefinition* e, std::ostream* w);

    static void GeneratePodStub(ServiceEntryDefinition* e, std::ostream* w);

    static void GenerateNamedArrayStub(const RR_SHARED_PTR<ServiceEntryDefinition>& e, std::ostream* w);

    static void GenerateStub(ServiceEntryDefinition* e, std::ostream* w);

    static void GenerateSkel(ServiceEntryDefinition* e, std::ostream* w);

    static void GenerateDefaultImpl(ServiceEntryDefinition* e, std::ostream* w);

    static void GenerateConstants(ServiceDefinition* d, std::ostream* w);

    static std::string GetDefaultValue(const TypeDefinition& tdef);

    static void GenerateDocString(const std::string& docstring, const std::string& prefix, std::ostream* w);

    // File generators

    static void GenerateExceptionFile(ExceptionDefinition* exp, ServiceDefinition* d, std::ostream* w);

    static void GenerateEnumFile(EnumDefinition* e, ServiceDefinition* d, std::ostream* w);

    static void GenerateServiceFactoryFile(ServiceDefinition* d, const std::string& defstring, std::ostream* w);

    static void GenerateInterfaceFile(ServiceEntryDefinition* d, std::ostream* w);

    static void GenerateAsyncInterfaceFile(ServiceEntryDefinition* d, std::ostream* w);

    static void GenerateStructureFile(ServiceEntryDefinition* d, std::ostream* w);

    static void GenerateStructureStubFile(ServiceEntryDefinition* d, std::ostream* w);

    static void GeneratePodFile(ServiceEntryDefinition* d, std::ostream* w);

    static void GenerateNamedArrayFile(const RR_SHARED_PTR<ServiceEntryDefinition>& d, std::ostream* w);

    static void GeneratePodStubFile(ServiceEntryDefinition* d, std::ostream* w);

    static void GenerateNamedArrayStubFile(const RR_SHARED_PTR<ServiceEntryDefinition>& d, std::ostream* w);

    static void GenerateStubFile(ServiceEntryDefinition* d, std::ostream* w);

    static void GenerateSkelFile(ServiceEntryDefinition* d, std::ostream* w);

    static void GenerateDefaultImplFile(ServiceEntryDefinition* d, std::ostream* w);

    static void GenerateConstantsFile(ServiceDefinition* d, std::ostream* w);

    static void GenerateFiles(const RR_SHARED_PTR<ServiceDefinition>& d, const std::string& servicedef,
                              const std::string& path = ".");
};

} // namespace RobotRaconteurGen