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

#include "CSharpServiceLangGen.h"

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <set>
#include <fstream>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/locale.hpp>

using namespace std;
using namespace RobotRaconteur;

#define MEMBER_ITER(TYPE)                                                                                              \
    {                                                                                                                  \
        for (std::vector<RR_SHARED_PTR<MemberDefinition> >::const_iterator m1 = (*e)->Members.begin();                 \
             m1 != (*e)->Members.end(); ++m1)                                                                          \
            if (dynamic_cast<TYPE*>(m1->get()) != 0)                                                                   \
            {                                                                                                          \
                RR_SHARED_PTR<TYPE> m = dynamic_pointer_cast<TYPE>(*m1);
#define MEMBER_ITER_END()                                                                                              \
    }                                                                                                                  \
    }

#define MEMBER_ITER2(TYPE)                                                                                             \
    {                                                                                                                  \
        for (std::vector<RR_SHARED_PTR<MemberDefinition> >::const_iterator m1 = e->Members.begin();                    \
             m1 != e->Members.end(); ++m1)                                                                             \
            if (dynamic_cast<TYPE*>(m1->get()) != 0)                                                                   \
            {                                                                                                          \
                RR_SHARED_PTR<TYPE> m = dynamic_pointer_cast<TYPE>(*m1);

namespace RobotRaconteurGen
{
std::string CSharpServiceLangGen::fix_name(const std::string& name)
{
    if (name.find('.') != std::string::npos)
    {
        vector<string> s1;
        vector<string> s2;
        boost::split(s1, name, boost::is_from_range('.', '.'));
        for (vector<string>::iterator e = s1.begin(); e != s1.end(); e++)
        {
            s2.push_back(fix_name(*e));
        }

        return join(s2, ".");
    }

    const char* res_str[] = {
        "abstract",  "as",        "async",    "await",    "base",    "bool",     "break",      "byte",     "case",
        "catch",     "char",      "checked",  "class",    "const",   "continue", "decimal",    "default",  "delegate",
        "do",        "double",    "dynamic",  "else",     "enum",    "event",    "explicit",   "extern",   "false",
        "finally",   "fixed",     "float",    "for",      "foreach", "goto",     "if",         "implicit", "in",
        "int",       "interface", "internal", "is",       "lock",    "long",     "namespace",  "new",      "null",
        "object",    "operator",  "out",      "override", "params",  "private",  "protected",  "public",   "readonly",
        "ref",       "return",    "sbyte",    "sealed",   "short",   "sizeof",   "stackalloc", "static",   "string",
        "struct",    "switch",    "this",     "throw",    "true",    "try",      "typeof",     "uint",     "ulong",
        "unchecked", "unsafe",    "ushort",   "using",    "virtual", "void",     "volatile",   "while",    "value"};

    std::vector<std::string> reserved(res_str, res_str + sizeof(res_str) / (sizeof(res_str[0])));

    if (std::find(reserved.begin(), reserved.end(), name) != reserved.end())
    {
        if (name != "value")
            std::cout << "warning: name " << name << " replaced by " << name << "_ due to keyword conflict"
                      << std::endl;
        return name + "_";
    }

    return name;
}

std::string CSharpServiceLangGen::fix_qualified_name(const std::string& name) { return fix_name(name); }

CSharpServiceLangGen::convert_type_result CSharpServiceLangGen::convert_type(const TypeDefinition& tdef)
{
    convert_type_result o;
    DataTypes t = tdef.Type;
    o.name = fix_name(tdef.Name);
    o.cs_arr_type = tdef.ArrayType == DataTypes_ArrayTypes_array ? "[]" : "";

    switch (t)
    {
    case DataTypes_void_t:
        o.cs_type = "void";
        break;
    case DataTypes_double_t:
        o.cs_type = "double";
        break;
    case DataTypes_single_t:
        o.cs_type = "float";
        break;
    case DataTypes_int8_t:
        o.cs_type = "sbyte";
        break;
    case DataTypes_uint8_t:
        o.cs_type = "byte";
        break;
    case DataTypes_int16_t:
        o.cs_type = "short";
        break;
    case DataTypes_uint16_t:
        o.cs_type = "ushort";
        break;
    case DataTypes_int32_t:
        o.cs_type = "int";
        break;
    case DataTypes_uint32_t:
        o.cs_type = "uint";
        break;
    case DataTypes_int64_t:
        o.cs_type = "long";
        break;
    case DataTypes_uint64_t:
        o.cs_type = "ulong";
        break;
    case DataTypes_string_t:
        o.cs_type = "string";
        break;
    case DataTypes_cdouble_t:
        o.cs_type = "CDouble";
        break;
    case DataTypes_csingle_t:
        o.cs_type = "CSingle";
        break;
    case DataTypes_bool_t:
        o.cs_type = "bool";
        break;
    case DataTypes_namedtype_t:
    case DataTypes_object_t:
        o.cs_type = fix_qualified_name(tdef.TypeString);
        break;
    case DataTypes_varvalue_t:
        o.cs_type = "object";
        break;
    default:
        throw InvalidArgumentException("");
    }

    RR_SHARED_PTR<NamedTypeDefinition> nt = tdef.ResolveNamedType_cache.lock();

    if (tdef.ArrayType == DataTypes_ArrayTypes_multidimarray)
    {
        if (IsTypeNumeric(tdef.Type))
        {
            o.cs_type = "MultiDimArray";
            o.cs_arr_type = "";
        }
        else if (tdef.Type == DataTypes_namedtype_t)
        {

            if (!nt)
                throw DataTypeException("Data type not resolved");
            switch (nt->RRDataType())
            {
            case DataTypes_pod_t: {
                o.cs_type = "PodMultiDimArray";
                o.cs_arr_type = "";
                break;
            }
            case DataTypes_namedarray_t: {
                o.cs_type = "NamedMultiDimArray";
                o.cs_arr_type = "";
                break;
            }
            default:
                throw InvalidArgumentException("Invalid multidimarray type");
            }
        }
        else
        {
            throw InvalidArgumentException("Invalid multidimarray type");
        }
    }

    switch (tdef.ContainerType)
    {
    case DataTypes_ContainerTypes_none:
        break;
    case DataTypes_ContainerTypes_list:
        o.cs_type = "List<" + o.cs_type + o.cs_arr_type + ">";
        o.cs_arr_type = "";
        break;
    case DataTypes_ContainerTypes_map_int32:
        o.cs_type = "Dictionary<int," + o.cs_type + o.cs_arr_type + ">";
        o.cs_arr_type = "";
        break;
    case DataTypes_ContainerTypes_map_string:
        o.cs_type = "Dictionary<string," + o.cs_type + o.cs_arr_type + ">";
        o.cs_arr_type = "";
        break;
    default:
        throw DataTypeException("Invalid container type");
    }

    return o;
}

CSharpServiceLangGen::convert_generator_result CSharpServiceLangGen::convert_generator(FunctionDefinition* f)
{
    if (!f->IsGenerator())
        throw InternalErrorException("");

    convert_generator_result o;

    bool return_generator = f->ReturnType->ContainerType == DataTypes_ContainerTypes_generator;
    bool param_generator =
        !f->Parameters.empty() && f->Parameters.back()->ContainerType == DataTypes_ContainerTypes_generator;

    if (return_generator && param_generator)
    {
        RR_SHARED_PTR<TypeDefinition> r_type = f->ReturnType->Clone();
        r_type->RemoveContainers();
        convert_type_result t = convert_type(*r_type);
        RR_SHARED_PTR<TypeDefinition> p_type = f->Parameters.back()->Clone();
        p_type->RemoveContainers();
        convert_type_result t2 = convert_type(*p_type);
        std::copy(f->Parameters.begin(), --f->Parameters.end(), std::back_inserter(o.params));
        o.generator_csharp_base_type = "Generator1";
        o.generator_csharp_template_params = t.cs_type + t.cs_arr_type + "," + t2.cs_type + t2.cs_arr_type;
        o.generator_csharp_type = o.generator_csharp_base_type + "<" + o.generator_csharp_template_params + ">";
        return o;
    }

    if (param_generator)
    {
        RR_SHARED_PTR<TypeDefinition> p_type = f->Parameters.back()->Clone();
        p_type->RemoveContainers();
        convert_type_result t2 = convert_type(*p_type);
        std::copy(f->Parameters.begin(), --f->Parameters.end(), std::back_inserter(o.params));
        o.generator_csharp_base_type = "Generator3";
        o.generator_csharp_template_params = t2.cs_type + t2.cs_arr_type;
        o.generator_csharp_type = o.generator_csharp_base_type + "<" + o.generator_csharp_template_params + ">";
        return o;
    }
    else
    {
        RR_SHARED_PTR<TypeDefinition> r_type = f->ReturnType->Clone();
        r_type->RemoveContainers();
        convert_type_result t = convert_type(*r_type);
        boost::range::copy(f->Parameters, std::back_inserter(o.params));
        o.generator_csharp_base_type = "Generator2";
        o.generator_csharp_template_params = t.cs_type + t.cs_arr_type;
        o.generator_csharp_type = o.generator_csharp_base_type + "<" + o.generator_csharp_template_params + ">";
        return o;
    }
}

std::string CSharpServiceLangGen::str_pack_parameters(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l,
                                                      bool inclass)
{
    std::vector<std::string> o(l.size());

    for (size_t i = 0; i < o.size(); i++)
    {
        convert_type_result t = convert_type(*l[i]);
        if (inclass)
            o[i] = t.cs_type + t.cs_arr_type + " " + t.name;
        else
            o[i] = t.name;
    }

    return boost::join(o, ", ");
}

std::string CSharpServiceLangGen::str_pack_delegate(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l,
                                                    boost::shared_ptr<TypeDefinition> rettype)
{
    if (!rettype || rettype->Type == DataTypes_void_t)
    {
        if (l.size() == 0)
        {
            return "Action";
        }
        else
        {
            vector<string> paramtypes;
            for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator e = l.begin(); e != l.end(); ++e)
            {
                convert_type_result t = convert_type(*(*e));
                paramtypes.push_back(t.cs_type + t.cs_arr_type);
            }

            return "Action<" + boost::join(paramtypes, ", ") + ">";
        }
    }
    else
    {
        vector<string> paramtypes;

        for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator e = l.begin(); e != l.end(); ++e)
        {
            convert_type_result t = convert_type(*(*e));
            paramtypes.push_back(t.cs_type + t.cs_arr_type);
        }

        convert_type_result t2 = convert_type(*rettype);
        paramtypes.push_back(t2.cs_type + t2.cs_arr_type);

        return "Func<" + boost::join(paramtypes, ",") + ">";
    }
}

// Code to pack and unpack message elements

static std::string CSharpServiceLangGen_VerifyArrayLength(TypeDefinition& t, std::string varname)
{
    if (t.ArrayType == DataTypes_ArrayTypes_array && t.ArrayLength.at(0) != 0)
    {
        return "DataTypeUtil.VerifyArrayLength(" + varname + ", " +
               boost::lexical_cast<std::string>(t.ArrayLength.at(0)) + ", " + (t.ArrayVarLength ? "true" : "false") +
               ")";
    }
    if (t.ArrayType == DataTypes_ArrayTypes_multidimarray && t.ArrayLength.size() != 0 && !t.ArrayVarLength)
    {
        int32_t n_elems = boost::accumulate(t.ArrayLength, 1, std::multiplies<int32_t>());
        return "DataTypeUtil.VerifyArrayLength(" + varname + "," + boost::lexical_cast<std::string>(n_elems) +
               ",new uint[] {" +
               boost::join(t.ArrayLength | boost::adaptors::transformed(boost::lexical_cast<std::string, int32_t>),
                           ",") +
               "})";
    }
    return varname;
}

std::string CSharpServiceLangGen::str_pack_message_element(const std::string& elementname, const std::string& varname,
                                                           const RR_SHARED_PTR<TypeDefinition>& t,
                                                           const std::string& packer)
{
    TypeDefinition t1;
    t->CopyTo(t1);
    t1.RemoveContainers();
    convert_type_result tt1 = convert_type(t1);

    switch (t->ContainerType)
    {
    case DataTypes_ContainerTypes_none: {
        if (IsTypeNumeric(t->Type))
        {
            switch (t->ArrayType)
            {
            case DataTypes_ArrayTypes_none: {
                convert_type_result ts = convert_type(*t);
                return "MessageElementUtil.PackScalar<" + ts.cs_type + ">(\"" + elementname + "\"," + varname + ")";
                break;
            }
            case DataTypes_ArrayTypes_array: {
                convert_type_result ts = convert_type(*t);
                return "MessageElementUtil.PackArray<" + ts.cs_type + ">(\"" + elementname + "\"," +
                       CSharpServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                break;
            }
            case DataTypes_ArrayTypes_multidimarray: {
                convert_type_result ts = convert_type(*t);
                return "MessageElementUtil.PackMultiDimArray(\"" + elementname + "\",(MultiDimArray)" +
                       CSharpServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                break;
            }
            default:
                throw DataTypeException("Invalid array type");
            }
        }
        else if (t->Type == DataTypes_string_t)
        {
            return "MessageElementUtil.PackString(\"" + elementname + "\"," + varname + ")";
        }
        else if (t->Type == DataTypes_varvalue_t)
        {
            return "MessageElementUtil.PackVarType(\"" + elementname + "\"," + varname + ")";
        }
        else if (t->Type == DataTypes_namedtype_t)
        {
            RR_SHARED_PTR<NamedTypeDefinition> nt = t->ResolveNamedType();
            switch (nt->RRDataType())
            {
            case DataTypes_structure_t:
                return "MessageElementUtil.PackStructure(\"" + elementname + "\"," + varname + ")";
                break;
            case DataTypes_enum_t: {
                convert_type_result ts = convert_type(*t);
                return "MessageElementUtil.PackEnum<" + fix_qualified_name(t->TypeString) + ">(\"" + elementname +
                       "\"," + varname + ")";
                break;
            }
            case DataTypes_pod_t:
                switch (t->ArrayType)
                {
                case DataTypes_ArrayTypes_none: {
                    return "MessageElementUtil.PackPodToArray<" + fix_qualified_name(t->TypeString) + ">(\"" +
                           elementname + "\",ref " + varname + ")";
                    break;
                }
                case DataTypes_ArrayTypes_array: {
                    return "MessageElementUtil.PackPodArray<" + fix_qualified_name(t->TypeString) + ">(\"" +
                           elementname + "\"," + CSharpServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                    break;
                }
                case DataTypes_ArrayTypes_multidimarray: {
                    return "MessageElementUtil.PackPodMultiDimArray<" + fix_qualified_name(t->TypeString) + ">(\"" +
                           elementname + "\"," + CSharpServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                    break;
                }
                default:
                    throw DataTypeException("Invalid array type");
                }
                break;
            case DataTypes_namedarray_t:
                switch (t->ArrayType)
                {
                case DataTypes_ArrayTypes_none: {
                    return "MessageElementUtil.PackNamedArrayToArray<" + fix_qualified_name(t->TypeString) + ">(\"" +
                           elementname + "\",ref " + varname + ")";
                    break;
                }
                case DataTypes_ArrayTypes_array: {
                    return "MessageElementUtil.PackNamedArray<" + fix_qualified_name(t->TypeString) + ">(\"" +
                           elementname + "\"," + CSharpServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                    break;
                }
                case DataTypes_ArrayTypes_multidimarray: {
                    return "MessageElementUtil.PackNamedMultiDimArray<" + fix_qualified_name(t->TypeString) + ">(\"" +
                           elementname + "\"," + CSharpServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                    break;
                }
                default:
                    throw DataTypeException("Invalid array type");
                }
                break;
            default:
                throw DataTypeException("Unknown named type id");
            }
        }
        else
        {
            throw DataTypeException("Unknown type");
        }
    }
    case DataTypes_ContainerTypes_list:
        return "MessageElementUtil.PackListType<" + tt1.cs_type + tt1.cs_arr_type + ">(\"" + elementname + "\"," +
               CSharpServiceLangGen_VerifyArrayLength(*t, varname) + ")";
    case DataTypes_ContainerTypes_map_int32:
        return "MessageElementUtil.PackMapType<int," + tt1.cs_type + tt1.cs_arr_type + ">(\"" + elementname + "\"," +
               CSharpServiceLangGen_VerifyArrayLength(*t, varname) + ")";
    case DataTypes_ContainerTypes_map_string:
        return "MessageElementUtil.PackMapType<string," + tt1.cs_type + tt1.cs_arr_type + ">(\"" + elementname + "\"," +
               CSharpServiceLangGen_VerifyArrayLength(*t, varname) + ")";
    default:
        throw DataTypeException("Invalid container type");
    }
}

std::string CSharpServiceLangGen::str_unpack_message_element(const std::string& varname,
                                                             const RR_SHARED_PTR<TypeDefinition>& t,
                                                             const std::string& packer)
{

    TypeDefinition t1;
    t->CopyTo(t1);
    t1.RemoveContainers();
    convert_type_result tt = convert_type(t1);
    if (t1.ArrayType == DataTypes_ArrayTypes_array)
        tt.cs_arr_type = "[]";
    std::string structunpackstring = "";

    convert_type_result tt1 = convert_type(t1);

    if (IsTypeNumeric(t->Type))
    {
        switch (t->ArrayType)
        {
        case DataTypes_ArrayTypes_none:
            structunpackstring =
                "(MessageElementUtil.UnpackScalar<" + tt.cs_type + tt.cs_arr_type + ">(" + varname + "))";
            break;
        case DataTypes_ArrayTypes_array:
            structunpackstring = CSharpServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.UnpackArray<" +
                                                                                tt.cs_type + ">(" + varname + ")");
            break;
        case DataTypes_ArrayTypes_multidimarray:
            structunpackstring =
                CSharpServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.UnpackMultiDimArray(" + varname + ")");
            break;
        default:
            throw DataTypeException("Invalid array type");
        }
    }
    else if (t->Type == DataTypes_string_t)
    {
        structunpackstring = "MessageElementUtil.UnpackString(" + varname + ")";
    }
    else if (t->Type == DataTypes_namedtype_t)
    {
        RR_SHARED_PTR<NamedTypeDefinition> nt = t->ResolveNamedType();
        switch (nt->RRDataType())
        {
        case DataTypes_structure_t:
            structunpackstring = "MessageElementUtil.UnpackStructure<" + tt.cs_type + ">(" + varname + ")";
            break;
        case DataTypes_enum_t:
            structunpackstring = "MessageElementUtil.UnpackEnum<" + tt.cs_type + ">(" + varname + ")";
            break;
        case DataTypes_pod_t:
            switch (t->ArrayType)
            {
            case DataTypes_ArrayTypes_none: {
                structunpackstring =
                    "MessageElementUtil.UnpackPodFromArray<" + fix_qualified_name(t->TypeString) + ">(" + varname + ")";
                break;
            }
            case DataTypes_ArrayTypes_array: {
                structunpackstring = CSharpServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.UnpackPodArray<" +
                                                                                    fix_qualified_name(t->TypeString) +
                                                                                    ">(" + varname + ")");
                break;
            }
            case DataTypes_ArrayTypes_multidimarray: {
                structunpackstring = CSharpServiceLangGen_VerifyArrayLength(
                    *t, "MessageElementUtil.UnpackPodMultiDimArray<" + fix_qualified_name(t->TypeString) + ">(" +
                            varname + ")");
                break;
            }
            default:
                throw DataTypeException("Invalid array type");
            }
            break;
        case DataTypes_namedarray_t:
            switch (t->ArrayType)
            {
            case DataTypes_ArrayTypes_none: {
                structunpackstring = "MessageElementUtil.UnpackNamedArrayFromArray<" +
                                     fix_qualified_name(t->TypeString) + ">(" + varname + ")";
                break;
            }
            case DataTypes_ArrayTypes_array: {
                structunpackstring = CSharpServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.UnpackNamedArray<" +
                                                                                    fix_qualified_name(t->TypeString) +
                                                                                    ">(" + varname + ")");
                break;
            }
            case DataTypes_ArrayTypes_multidimarray: {
                structunpackstring = CSharpServiceLangGen_VerifyArrayLength(
                    *t, "MessageElementUtil.UnpackNamedMultiDimArray<" + fix_qualified_name(t->TypeString) + ">(" +
                            varname + ")");
                break;
            }
            default:
                throw DataTypeException("Invalid array type");
            }
            break;
        default:
            throw DataTypeException("Unknown named type id");
        }
    }

    else if (t->Type == DataTypes_varvalue_t)
    {
        structunpackstring = "MessageElementUtil.UnpackVarType(" + varname + ")";
    }
    else
    {
        throw InvalidArgumentException("Unknown type");
    }

    switch (t->ContainerType)
    {
    case DataTypes_ContainerTypes_none:
        return structunpackstring;
    case DataTypes_ContainerTypes_list:
        return CSharpServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.UnpackList<" + tt.cs_type +
                                                              tt.cs_arr_type + ">(" + varname + ")");
    case DataTypes_ContainerTypes_map_int32:
        return CSharpServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.UnpackMap<int," + tt.cs_type +
                                                              tt.cs_arr_type + ">(" + varname + ")");
    case DataTypes_ContainerTypes_map_string:
        return CSharpServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.UnpackMap<string," + tt.cs_type +
                                                              tt.cs_arr_type + ">(" + varname + ")");
    default:
        throw DataTypeException("Invalid container type");
    }
}

bool CSharpServiceLangGen::GetObjRefIndType(RR_SHARED_PTR<ObjRefDefinition>& m, std::string& indtype)
{
    switch (m->ArrayType)
    {
    case DataTypes_ArrayTypes_none:
        switch (m->ContainerType)
        {
        case DataTypes_ContainerTypes_none:
            indtype.clear();
            return false;
        case DataTypes_ContainerTypes_map_int32:
            indtype = "int";
            return true;
        case DataTypes_ContainerTypes_map_string:
            indtype = "string";
            return true;
        default:
            throw DataTypeException("Unknown object container type");
        }
    case DataTypes_ArrayTypes_array: {
        if (m->ContainerType != DataTypes_ContainerTypes_none)
        {
            throw DataTypeException("Invalid object container type");
        }
        indtype = "int";
        return true;
    }
    default:
        throw DataTypeException("Invalid object array type");
    }
}

void CSharpServiceLangGen::GenerateStructure(ServiceEntryDefinition* e, ostream* w)
{
    ostream& w2 = *w;

    GenerateDocString(e->DocString, "", w);
    w2 << "public class " + fix_name(e->Name) << endl << "{" << endl;

    MEMBER_ITER2(PropertyDefinition)
    GenerateDocString(m->DocString, "    ", w);
    convert_type_result t = convert_type(*m->Type);
    t.name = fix_name(m->Name);
    w2 << "    public " + t.cs_type + t.cs_arr_type + " " + t.name + ";" << endl;
    MEMBER_ITER_END()
    w2 << "}" << endl << endl;
}

static RR_SHARED_PTR<TypeDefinition> CSharpServiceLangGen_RemoveMultiDimArray(const TypeDefinition& t)
{
    RR_SHARED_PTR<TypeDefinition> t2 = RR_MAKE_SHARED<TypeDefinition>();
    t.CopyTo(*t2);

    if (t.ArrayType != DataTypes_ArrayTypes_multidimarray)
        return t2;

    t2->ArrayType = DataTypes_ArrayTypes_array;
    t2->ArrayLength.clear();
    t2->ArrayLength.push_back(boost::accumulate(t.ArrayLength, 1, std::multiplies<int32_t>()));
    return t2;
}

void CSharpServiceLangGen::GeneratePod(RR_SHARED_PTR<ServiceEntryDefinition> e, ostream* w)
{
    ostream& w2 = *w;

    GenerateDocString(e->DocString, "", w);

    if (e->EntryType == DataTypes_namedarray_t)
    {
        boost::tuple<DataTypes, size_t> t4 = GetNamedArrayElementTypeAndCount(e);
        TypeDefinition t5;
        t5.Type = t4.get<0>();
        convert_type_result t6 = convert_type(t5);
        w2 << "[NamedArrayElementTypeAndCount(typeof(" << t6.cs_type << "), " << t4.get<1>() << ")]" << endl;
    }

    w2 << "public struct " + fix_name(e->Name) << endl << "{" << endl;

    MEMBER_ITER2(PropertyDefinition)
    GenerateDocString(m->DocString, "    ", w);
    TypeDefinition t2 = *CSharpServiceLangGen_RemoveMultiDimArray(*m->Type);
    convert_type_result t = convert_type(t2);
    t.name = fix_name(m->Name);
    w2 << "    public " + t.cs_type + t.cs_arr_type + " " + t.name + ";" << endl;
    MEMBER_ITER_END()

    if (e->EntryType == DataTypes_namedarray_t)
    {
        boost::tuple<DataTypes, size_t> t4 = GetNamedArrayElementTypeAndCount(e);
        TypeDefinition t5;
        t5.Type = t4.get<0>();
        convert_type_result t6 = convert_type(t5);

        w2 << "    public " + t6.cs_type + "[] GetNumericArray()" << endl << "    {" << endl;
        w2 << "    var a=new ArraySegment<" << t6.cs_type
           << ">(new " + t6.cs_type + "[" + boost::lexical_cast<std::string>(t4.get<1>()) + "]);" << endl;
        w2 << "    GetNumericArray(ref a);" << endl;
        w2 << "    return a.Array;" << endl;
        w2 << "    }" << endl;

        w2 << "    public void GetNumericArray(ref ArraySegment<" + t6.cs_type + "> rr_a)" << endl << "    {" << endl;
        {
            w2 << "    if(rr_a.Count < " << t4.get<1>()
               << ") throw new ArgumentException(\"ArraySegment invalid length\");" << endl;
            int i = 0;
            MEMBER_ITER2(PropertyDefinition)
            TypeDefinition t7 = *CSharpServiceLangGen_RemoveMultiDimArray(*m->Type);
            convert_type_result t8 = convert_type(t7);
            t8.name = fix_name(m->Name);
            if (IsTypeNumeric(m->Type->Type))
            {
                if (m->Type->ArrayType == DataTypes_ArrayTypes_none)
                {
                    w2 << "    rr_a.Array[rr_a.Offset + " << i << "] = " << t8.name << ";" << endl;
                    i++;
                }
                else
                {
                    w2 << "    Array.Copy(" << t8.name << ", 0, rr_a.Array, rr_a.Offset + " << i << ", "
                       << t7.ArrayLength.at(0) << ");" << endl;
                    i += t7.ArrayLength.at(0);
                }
            }
            else
            {
                RR_SHARED_PTR<ServiceEntryDefinition> e2 = rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType());
                boost::tuple<DataTypes, size_t> t9 = GetNamedArrayElementTypeAndCount(e2);
                size_t e2_count = m->Type->ArrayType == DataTypes_ArrayTypes_none ? 1 : t7.ArrayLength.at(0);

                w2 << "    var rr_a" << i << " = new ArraySegment<" << t6.cs_type << ">(rr_a.Array, rr_a.Offset + " << i
                   << ", " << t9.get<1>() * e2_count << ");" << endl;
                w2 << "    " << t8.name << ".GetNumericArray(ref rr_a" << i << ");" << endl;
                i += t9.get<1>() * e2_count;
            }
            // w2 << "    public " + t8.cs_type + t8.cs_arr_type + " " + t8.name + ";" << endl;
            MEMBER_ITER_END()
        }
        w2 << "    }" << endl;

        w2 << "    public void AssignFromNumericArray(ref ArraySegment<" + t6.cs_type + "> rr_a)" << endl
           << "    {" << endl;
        {
            w2 << "    if(rr_a.Count < " << t4.get<1>()
               << ") throw new ArgumentException(\"ArraySegment invalid length\");" << endl;
            int i = 0;
            MEMBER_ITER2(PropertyDefinition)
            TypeDefinition t7 = *CSharpServiceLangGen_RemoveMultiDimArray(*m->Type);
            convert_type_result t8 = convert_type(t7);
            t8.name = fix_name(m->Name);
            if (IsTypeNumeric(m->Type->Type))
            {
                if (m->Type->ArrayType == DataTypes_ArrayTypes_none)
                {
                    w2 << "    " << t8.name << " = rr_a.Array[rr_a.Offset + " << i << "]"
                       << ";" << endl;
                    i++;
                }
                else
                {
                    w2 << "    Array.Copy(rr_a.Array, rr_a.Offset + " << i << ", " << t8.name << ", 0, "
                       << t7.ArrayLength.at(0) << ");" << endl;
                    i += t7.ArrayLength.at(0);
                }
            }
            else
            {
                RR_SHARED_PTR<ServiceEntryDefinition> e2 = rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType());
                boost::tuple<DataTypes, size_t> t9 = GetNamedArrayElementTypeAndCount(e2);
                size_t e2_count = m->Type->ArrayType == DataTypes_ArrayTypes_none ? 1 : t7.ArrayLength.at(0);

                w2 << "    var rr_a" << i << " = new ArraySegment<" << t6.cs_type << ">(rr_a.Array, rr_a.Offset + " << i
                   << ", " << t9.get<1>() * e2_count << ");" << endl;
                w2 << "    " << t8.name << ".AssignFromNumericArray(ref rr_a" << i << ");" << endl;
                i += t9.get<1>() * e2_count;
            }
            // w2 << "    public " + t8.cs_type + t8.cs_arr_type + " " + t8.name + ";" << endl;
            MEMBER_ITER_END()
            w2 << "    }" << endl;
        }
    }

    w2 << "}" << endl << endl;
}

void CSharpServiceLangGen::GenerateNamedArrayExtensions(RR_SHARED_PTR<ServiceEntryDefinition> e, ostream* w)
{
    ostream& w2 = *w;

    boost::tuple<DataTypes, size_t> t1 = GetNamedArrayElementTypeAndCount(e);
    TypeDefinition t2;
    t2.Type = t1.get<0>();
    convert_type_result t3 = convert_type(t2);

    w2 << "    public static " + t3.cs_type + "[] GetNumericArray(this " + fix_name(e->Name) + "[] s)" << endl
       << "    {" << endl;
    w2 << "    var a=new ArraySegment<" << t3.cs_type
       << ">(new " + t3.cs_type + "[" + boost::lexical_cast<std::string>(t1.get<1>()) + " * s.Length]);" << endl;
    w2 << "    s.GetNumericArray(ref a);" << endl;
    w2 << "    return a.Array;" << endl;
    w2 << "    }" << endl;

    w2 << "    public static void GetNumericArray(this " + fix_name(e->Name) + "[] s, ref ArraySegment<" + t3.cs_type +
              "> a)"
       << endl
       << "    {" << endl;
    w2 << "    if(a.Count < " << t1.get<1>()
       << " * s.Length) throw new ArgumentException(\"ArraySegment invalid length\");" << endl;
    w2 << "    for (int i=0; i<s.Length; i++)" << endl << "    {" << endl;
    w2 << "    var a1 = new ArraySegment<" << t3.cs_type << ">(a.Array, a.Offset + " << t1.get<1>() << "*i,"
       << t1.get<1>() << ");" << endl;
    w2 << "    s[i].GetNumericArray(ref a1);" << endl;
    w2 << "    }" << endl;
    w2 << "    }" << endl;

    w2 << "    public static void AssignFromNumericArray(this " + fix_name(e->Name) + "[] s, ref ArraySegment<" +
              t3.cs_type + "> a)"
       << endl
       << "    {" << endl;
    w2 << "    if(a.Count < " << t1.get<1>()
       << " * s.Length) throw new ArgumentException(\"ArraySegment invalid length\");" << endl;

    w2 << "    for (int i=0; i<s.Length; i++)" << endl << "    {" << endl;
    w2 << "    var a1 = new ArraySegment<" << t3.cs_type << ">(a.Array, a.Offset + " << t1.get<1>() << "*i,"
       << t1.get<1>() << ");" << endl;
    w2 << "    s[i].AssignFromNumericArray(ref a1);" << endl;
    w2 << "    }" << endl;

    w2 << "    }" << endl;
}

void CSharpServiceLangGen::GenerateInterface(ServiceEntryDefinition* e, ostream* w)
{
    ostream& w2 = *w;

    GenerateDocString(e->DocString, "", w);

    w2 << "[RobotRaconteurServiceObjectInterface()]" << endl;

    std::vector<std::string> implements2;

    for (std::vector<std::string>::iterator ee = e->Implements.begin(); ee != e->Implements.end(); ee++)
    {
        implements2.push_back(fix_qualified_name(*ee));
    }

    std::string implements = boost::join(implements2, ", ");
    if (e->Implements.size() > 0)
        implements = " : " + implements;

    w2 << "public interface " + fix_name(e->Name) << implements << endl << "{" << endl;

    MEMBER_ITER2(PropertyDefinition)
    GenerateDocString(m->DocString, "    ", w);
    convert_type_result t = convert_type(*m->Type);
    t.name = fix_name(m->Name);
    w2 << "    " + t.cs_type + t.cs_arr_type + " " + t.name + " {";
    if (m->Direction() != MemberDefinition_Direction_writeonly)
    {
        w2 << " get; ";
    }
    if (m->Direction() != MemberDefinition_Direction_readonly)
    {
        w2 << " set; ";
    }
    w2 << "	}" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(FunctionDefinition)
    GenerateDocString(m->DocString, "    ", w);
    if (!m->IsGenerator())
    {
        convert_type_result t = convert_type(*m->ReturnType);
        string params = str_pack_parameters(m->Parameters, true);
        w2 << "    " + t.cs_type + t.cs_arr_type + " " + fix_name(m->Name) + "(" + params + ");" << endl;
    }
    else
    {
        convert_generator_result t = convert_generator(m.get());
        string params = str_pack_parameters(t.params, true);
        w2 << "    " + t.generator_csharp_type + " " + fix_name(m->Name) + "(" + params + ");" << endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(EventDefinition)
    GenerateDocString(m->DocString, "    ", w);
    w2 << "    event " << str_pack_delegate(m->Parameters) << " " << fix_name(m->Name) << ";" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(ObjRefDefinition)
    GenerateDocString(m->DocString, "    ", w);
    string objtype = fix_qualified_name(m->ObjectType);
    if (objtype == "varobject")
        objtype = "object";
    std::string indtype;
    if (GetObjRefIndType(m, indtype))
    {
        w2 << "    " + objtype + " get_" + fix_name(m->Name) + "(" + indtype + " ind);" << endl;
    }
    else
    {
        w2 << "    " + objtype + " get_" + fix_name(m->Name) + "();" << endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(PipeDefinition)
    GenerateDocString(m->DocString, "    ", w);
    convert_type_result t = convert_type(*m->Type);
    w2 << "    Pipe<" + t.cs_type + t.cs_arr_type + "> " << fix_name(m->Name) << "{ get; set; }" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(CallbackDefinition)
    GenerateDocString(m->DocString, "    ", w);
    w2 << "    Callback<" + str_pack_delegate(m->Parameters, m->ReturnType) + "> " + fix_name(m->Name) + " {get; set;}"
       << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    GenerateDocString(m->DocString, "    ", w);
    convert_type_result t = convert_type(*m->Type);
    w2 << "    Wire<" + t.cs_type + t.cs_arr_type + "> " << fix_name(m->Name) << "{ get; set; }" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(MemoryDefinition)
    GenerateDocString(m->DocString, "    ", w);
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type(t2);
    std::string c = "";
    if (!IsTypeNumeric(m->Type->Type))
    {
        DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();
        if (entry_type != DataTypes_namedarray_t)
        {
            c = "Pod";
        }
        else
        {
            c = "Named";
        }
    }
    switch (m->Type->ArrayType)
    {
    case DataTypes_ArrayTypes_array:
        w2 << "    " << c << "ArrayMemory<" + t.cs_type + "> " + fix_name(m->Name) + " { get; }" << endl;
        break;
    case DataTypes_ArrayTypes_multidimarray:
        w2 << "    " << c << "MultiDimArrayMemory<" + t.cs_type + "> " + fix_name(m->Name) + " { get; }" << endl;
        break;
    default:
        throw DataTypeException("Invalid memory definition");
    }
    MEMBER_ITER_END()

    w2 << "}" << endl << endl;
}

void CSharpServiceLangGen::GenerateInterfaceFile(ServiceDefinition* d, ostream* w, bool header)
{
    ostream& w2 = *w;

    if (header)
    {
        w2 << "//This file is automatically generated. DO NOT EDIT!" << endl;
        w2 << "using System;" << endl;
        w2 << "using RobotRaconteur;" << endl;
        w2 << "using System.Collections.Generic;" << endl << endl;
        w2 << "#pragma warning disable 0108" << endl << endl;
    }

    if (!d->DocString.empty())
    {
        GenerateDocString(d->DocString, "", w);
    }
    w2 << "namespace " << fix_name(d->Name) << endl << "{" << endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        GenerateStructure(e->get(), w);
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        GeneratePod(*e, w);
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        GeneratePod(*e, w);
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        GenerateInterface(e->get(), w);
    }

    GenerateConstants(d, w);

    BOOST_FOREACH (RR_SHARED_PTR<ExceptionDefinition> e, d->Exceptions)
    {
        GenerateDocString(e->DocString, "    ", w);
        w2 << "public class " << fix_name(e->Name) << " : RobotRaconteurRemoteException" << endl << "{" << endl;

        w2 << "    public " << fix_name(e->Name)
           << "(string message,string errorsubname=null,object param=null) : base(\"" << d->Name << "." << e->Name
           << "\",message,errorsubname,param) {}" << endl;
        w2 << "};" << endl;
    }

    w2 << "}" << endl;
}

void CSharpServiceLangGen::GenerateStubSkelFile(ServiceDefinition* d, std::string defstring, ostream* w, bool header)
{
    ostream& w2 = *w;

    if (header)
    {
        w2 << "//This file is automatically generated. DO NOT EDIT!" << endl;
        w2 << "using System;" << endl;
        w2 << "using RobotRaconteur;" << endl;
        w2 << "using System.Collections.Generic;" << endl;
        w2 << "using System.Threading.Tasks;" << endl << endl;
        w2 << "#pragma warning disable 0108" << endl << endl;
    }
    w2 << "namespace " << fix_name(d->Name) << endl << "{" << endl;
    GenerateServiceFactory(d, defstring, w);
    w2 << endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        GenerateStructureStub(e->get(), w);
        w2 << endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        GeneratePodStub(e->get(), w);
        w2 << endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        GenerateNamedArrayStub(*e, w);
        w2 << endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        GenerateStub(e->get(), w);
    }

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        GenerateSkel(e->get(), w);
    }

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        GenerateDefaultImpl(e->get(), w);
    }

    w2 << "public static class RRExtensions"
       << "{" << endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        GenerateNamedArrayExtensions(*e, w);
    }

    w2 << "}" << endl;

    w2 << "}" << endl;
}

void CSharpServiceLangGen::GenerateServiceFactory(ServiceDefinition* d, std::string defstring, ostream* w)
{
    ostream& w2 = *w;

    w2 << "public class " << boost::replace_all_copy(fix_name(d->Name), ".", "__") + "Factory : ServiceFactory" << endl
       << "{" << endl;
    w2 << "    public override string DefString()" << endl << "{" << endl;
    w2 << "    const string s=\"";
    vector<string> lines;
    std::string s = defstring;
    boost::split(lines, s, boost::is_from_range('\n', '\n'));
    for (vector<string>::iterator e = lines.begin(); e != lines.end(); ++e)
    {
        std::string l = boost::replace_all_copy(*e, "\\", "\\\\");
        boost::replace_all(l, "\"", "\\\"");
        boost::replace_all(l, "\r", "");
        boost::trim(l);
        w2 << l << "\\n";
    }
    w2 << "\";" << std::endl;
    w2 << "    return s;" << endl;
    ;
    w2 << "    }" << endl;
    w2 << "    public override string GetServiceName() {return \"" + d->Name + "\";}" << endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        w2 << "    public " + fix_name((*e)->Name) + "_stub " << fix_name((*e)->Name) + "_stubentry;" << endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        w2 << "    public " + fix_name((*e)->Name) + "_stub " << fix_name((*e)->Name) + "_stubentry;" << endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        w2 << "    public " + fix_name((*e)->Name) + "_stub " << fix_name((*e)->Name) + "_stubentry;" << endl;
    }
    w2 << "    public " + boost::replace_all_copy(fix_name(d->Name), ".", "__") + "Factory()" << endl << "{" << endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        w2 << "    " << fix_name((*e)->Name) + "_stubentry=new " << fix_name((*e)->Name) + "_stub(this);" << endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        w2 << "    " << fix_name((*e)->Name) + "_stubentry=new " << fix_name((*e)->Name) + "_stub(this);" << endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        w2 << "    " << fix_name((*e)->Name) + "_stubentry=new " << fix_name((*e)->Name) + "_stub();" << endl;
    }
    w2 << "    }" << endl;

    w2 << "    public override IStructureStub FindStructureStub(string objecttype)" << endl << "    {" << endl;
    // w2 << "    string objshort=RemovePath(objecttype);" << endl;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        w2 << "    if (objecttype==\"" + (*e)->Name + "\")";
        w2 << "    return " << fix_name((*e)->Name) + "_stubentry;" << endl;
    }
    w2 << "    throw new DataTypeException(\"Cannot find appropriate structure stub\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override IPodStub FindPodStub(string objecttype)" << endl << "    {" << endl;
    // w2 << "    string objshort=RemovePath(objecttype);" << endl;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        w2 << "    if (objecttype==\"" + (*e)->Name + "\")";
        w2 << "    return " << fix_name((*e)->Name) + "_stubentry;" << endl;
    }
    w2 << "    throw new DataTypeException(\"Cannot find appropriate pod stub\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override INamedArrayStub FindNamedArrayStub(string objecttype)" << endl << "    {" << endl;
    // w2 << "    string objshort=RemovePath(objecttype);" << endl;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        w2 << "    if (objecttype==\"" + (*e)->Name + "\")";
        w2 << "    return " << fix_name((*e)->Name) + "_stubentry;" << endl;
    }
    w2 << "    throw new DataTypeException(\"Cannot find appropriate pod stub\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override ServiceStub CreateStub(WrappedServiceStub innerstub) {" << endl;
    w2 << "    string objecttype=innerstub.RR_objecttype.GetServiceDefinition().Name + \".\" + "
          "innerstub.RR_objecttype.Name;";
    w2 << "    string objshort;" << endl;
    w2 << "    if (CompareNamespace(objecttype, out objshort)) {" << endl;
    w2 << "    switch (objshort) {" << endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        string objname = (*e)->Name;
        w2 << "    case \"" + objname + "\":" << endl;
        w2 << "    return new " + fix_name(objname) + "_stub(innerstub);" << endl;
    }
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    } else {" << endl;
    w2 << "    string ext_service_type=RobotRaconteurNode.SplitQualifiedName(objecttype).Item1;" << endl;
    w2 << "    return RobotRaconteurNode.s.GetServiceType(ext_service_type).CreateStub(innerstub);" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new ServiceException(\"Could not create stub\");" << endl;

    w2 << "    }" << endl;

    w2 << "    public override ServiceSkel CreateSkel(object obj) {" << endl;
    w2 << "    string objtype = RobotRaconteurNode.GetTypeString(ServiceSkelUtil.FindParentInterface(obj.GetType()));"
       << endl;
    w2 << "    string objshort;" << endl;

    w2 << "    if (CompareNamespace(objtype, out objshort)) {" << endl;

    w2 << "    switch(objshort) {" << endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        string objname = (*e)->Name;
        w2 << "    case \"" + objname + "\":" << endl;
        w2 << "    return new " + fix_name(objname) + "_skel((" + fix_name(objname) + ")obj);" << endl;
    }
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    } else {" << endl;
    w2 << "    string ext_service_type=RobotRaconteurNode.SplitQualifiedName(objtype).Item1;" << endl;
    w2 << "    return RobotRaconteurNode.s.GetServiceFactory(ext_service_type).CreateSkel(obj);" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new ServiceException(\"Could not create skel\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override RobotRaconteurException DownCastException(RobotRaconteurException rr_exp)"
       << "{" << endl;
    w2 << "    if (rr_exp==null) return rr_exp;" << endl;
    w2 << "    string rr_type=rr_exp.Error;" << endl;
    w2 << "    if (!rr_type.Contains(\".\")) return rr_exp;" << endl;
    w2 << "    string rr_stype;" << endl;
    w2 << "    if (CompareNamespace(rr_type, out rr_stype)) {" << endl;
    BOOST_FOREACH (RR_SHARED_PTR<ExceptionDefinition> e, d->Exceptions)
    {
        w2 << "    if (rr_stype==\"" << e->Name << "\") return new " << fix_name(e->Name)
           << "(rr_exp.Message,rr_exp.ErrorSubName,rr_exp.ErrorParam);" << endl;
    }
    w2 << "    } else {" << endl;
    w2 << "    return RobotRaconteurNode.s.DownCastException(rr_exp); " << endl;
    w2 << "    }" << endl;
    w2 << "    return rr_exp;" << endl;
    w2 << "    }" << endl;

    w2 << "}" << endl;
}

void CSharpServiceLangGen::GenerateStructureStub(ServiceEntryDefinition* e, ostream* w)
{
    ostream& w2 = *w;

    w2 << "public class " + fix_name(e->Name) + "_stub : IStructureStub {" << endl;
    w2 << "    public " + fix_name(e->Name) + "_stub(" +
              boost::replace_all_copy(fix_name(e->ServiceDefinition_.lock()->Name), ".", "__") + "Factory d) {def=d;}"
       << endl;
    w2 << "    private " + boost::replace_all_copy(fix_name(e->ServiceDefinition_.lock()->Name), ".", "__") +
              "Factory def;"
       << endl;
    w2 << "    public MessageElementNestedElementList PackStructure(object s1) {" << endl;

    w2 << "    using(vectorptr_messageelement m=new vectorptr_messageelement())" << endl << "    {" << endl;
    w2 << "    if (s1 ==null) return null;" << endl;
    w2 << "    " + fix_qualified_name(e->Name) + " s = (" + fix_qualified_name(e->Name) + ")s1;" << endl;
    MEMBER_ITER2(PropertyDefinition)
    w2 << "    MessageElementUtil.AddMessageElementDispose(m," +
              str_pack_message_element(m->Name, "s." + fix_name(m->Name), m->Type, "def") + ");"
       << endl;
    MEMBER_ITER_END()
    w2 << "    return new MessageElementNestedElementList(DataTypes.structure_t,\"" +
              e->ServiceDefinition_.lock()->Name + "." + e->Name + "\",m);"
       << endl;
    w2 << "    }" << endl;
    w2 << "    }" << endl;

    // Write Read
    w2 << "    public T UnpackStructure<T>(MessageElementNestedElementList m) {" << endl;

    w2 << "    if (m == null ) return default(T);" << endl;

    w2 << "    " + fix_name(e->Name) + " s=new " + fix_name(e->Name) + "();" << endl;
    w2 << "    using(vectorptr_messageelement mm=m.Elements)" << endl << "    {" << endl;
    MEMBER_ITER2(PropertyDefinition)
    convert_type_result t = convert_type(*m->Type);
    t.name = m->Name;

    w2 << "    s." + fix_name(t.name) + " =" +
              str_unpack_message_element("MessageElement.FindElement(mm,\"" + t.name + "\")", m->Type, "def") + ";"
       << endl;

    MEMBER_ITER_END()
    // w2 << "    if ((s as T)==null) throw new DataTypeException(\"Incorrect structure cast\");");
    w2 << "    T st; try {st=(T)((object)s);} catch (InvalidCastException) {throw new "
          "DataTypeMismatchException(\"Wrong structuretype\");}"
       << endl;
    w2 << "    return st;" << endl;
    w2 << "    }" << endl;
    w2 << "    }" << endl;

    w2 << "}" << endl;
}

void CSharpServiceLangGen::GeneratePodStub(ServiceEntryDefinition* e, ostream* w)
{
    ostream& w2 = *w;

    w2 << "public class " + fix_name(e->Name) + "_stub : PodStub<" << fix_name(e->Name) << "> {" << endl;
    w2 << "    public " + fix_name(e->Name) + "_stub(" +
              boost::replace_all_copy(fix_name(e->ServiceDefinition_.lock()->Name), ".", "__") + "Factory d) {def=d;}"
       << endl;
    w2 << "    private " + boost::replace_all_copy(fix_name(e->ServiceDefinition_.lock()->Name), ".", "__") +
              "Factory def;"
       << endl;
    w2 << "    public override MessageElementNestedElementList PackPod(ref " << fix_name(e->Name) << " s1) {" << endl;
    w2 << "    using(vectorptr_messageelement m=new vectorptr_messageelement())" << endl << "    {" << endl;
    w2 << "    " + fix_qualified_name(e->Name) + " s = (" + fix_qualified_name(e->Name) + ")s1;" << endl;
    MEMBER_ITER2(PropertyDefinition)
    RR_SHARED_PTR<TypeDefinition> t2 = CSharpServiceLangGen_RemoveMultiDimArray(*m->Type);
    w2 << "    MessageElementUtil.AddMessageElementDispose(m," +
              str_pack_message_element(m->Name, "s." + fix_name(m->Name), t2, "def") + ");"
       << endl;
    MEMBER_ITER_END()
    w2 << "    return new MessageElementNestedElementList(DataTypes.pod_t,\"\",m);" << endl;
    w2 << "    }" << endl;
    w2 << "    }" << endl;

    // Write Read
    w2 << "    public override " << fix_name(e->Name) << " UnpackPod(MessageElementNestedElementList m) {" << endl;

    w2 << "    if (m == null ) throw new NullReferenceException(\"Pod must not be null\");" << endl;
    w2 << "    using(vectorptr_messageelement mm=m.Elements)" << endl << "    {" << endl;
    w2 << "    " << fix_name(e->Name) << " s = new " << fix_name(e->Name) << "();" << endl;
    MEMBER_ITER2(PropertyDefinition)
    convert_type_result t = convert_type(*m->Type);
    t.name = m->Name;
    RR_SHARED_PTR<TypeDefinition> t2 = CSharpServiceLangGen_RemoveMultiDimArray(*m->Type);
    w2 << "    s." + fix_name(t.name) + " =" +
              str_unpack_message_element("MessageElement.FindElement(mm,\"" + t.name + "\")", t2, "def") + ";"
       << endl;

    MEMBER_ITER_END()

    w2 << "    return s;" << endl;
    w2 << "    }" << endl;
    w2 << "    }" << endl;

    w2 << "    public override string TypeName { get { return \"" << e->ServiceDefinition_.lock()->Name << "."
       << e->Name << "\"; } }";

    w2 << "}" << endl;
}

void CSharpServiceLangGen::GenerateNamedArrayStub(RR_SHARED_PTR<ServiceEntryDefinition> e, ostream* w)
{
    ostream& w2 = *w;

    boost::tuple<DataTypes, size_t> t4 = GetNamedArrayElementTypeAndCount(e);
    TypeDefinition t5;
    t5.Type = t4.get<0>();
    convert_type_result t6 = convert_type(t5);

    w2 << "public class " + fix_name(e->Name) + "_stub : NamedArrayStub<" << fix_name(e->Name) << "," << t6.cs_type
       << "> {" << endl;
    w2 << "    public override " << t6.cs_type << "[] GetNumericArrayFromNamedArrayStruct(ref " << fix_name(e->Name)
       << " s) {" << endl;
    w2 << "    return s.GetNumericArray();" << endl;
    w2 << "    }" << endl;
    w2 << "    public override " << fix_name(e->Name) << " GetNamedArrayStructFromNumericArray(" << t6.cs_type
       << "[] m) {" << endl;
    w2 << "    if (m.Length != " << t4.get<1>() << ") throw new DataTypeException(\"Invalid namedarray array\");"
       << endl;
    w2 << "    var s = new " << fix_name(e->Name) << "();" << endl;
    w2 << "    var a = new ArraySegment<" << t6.cs_type << ">(m);" << endl;
    w2 << "    s.AssignFromNumericArray(ref a);" << endl;
    w2 << "    return s;" << endl;
    w2 << "    }" << endl;
    w2 << "    public override " << t6.cs_type << "[] GetNumericArrayFromNamedArray(" << fix_name(e->Name) << "[] s) {"
       << endl;
    w2 << "    return s.GetNumericArray();" << endl;
    w2 << "    }" << endl;
    w2 << "    public override " << fix_name(e->Name) << "[] GetNamedArrayFromNumericArray(" << t6.cs_type << "[] m) {"
       << endl;
    w2 << "    if (m.Length % " << t4.get<1>() << " != 0) throw new DataTypeException(\"Invalid namedarray array\");"
       << endl;
    w2 << "    " << fix_name(e->Name) << "[] s = new " << fix_name(e->Name) << "[m.Length / " << t4.get<1>() << "];"
       << endl;
    w2 << "    var a = new ArraySegment<" << t6.cs_type << ">(m);" << endl;
    w2 << "    s.AssignFromNumericArray(ref a);" << endl;
    w2 << "    return s;" << endl;
    w2 << "    }" << endl;
    w2 << "    public override string TypeName { get { return \"" << e->ServiceDefinition_.lock()->Name << "."
       << e->Name << "\"; } }";

    w2 << "}" << endl;
}

void CSharpServiceLangGen::GenerateStub(ServiceEntryDefinition* e, ostream* w)
{
    ostream& w2 = *w;

    std::vector<std::string> implements2;

    for (std::vector<std::string>::iterator ee = e->Implements.begin(); ee != e->Implements.end(); ee++)
    {
        std::string iname = fix_qualified_name(*ee);
        if (iname.find(".") != std::string::npos)
        {
            boost::tuple<boost::string_ref, boost::string_ref> i1 = SplitQualifiedName(iname);

            iname = i1.get<0>() + ".async_" + i1.get<1>();
        }
        else
        {
            iname = "async_" + iname;
        }
        implements2.push_back(iname);
    }

    std::string implements = boost::join(implements2, ", ");
    if (e->Implements.size() > 0)
        implements = " : " + implements;

    w2 << "public interface async_" + fix_name(e->Name) << implements << endl << "{" << endl;

    MEMBER_ITER2(PropertyDefinition)
    convert_type_result t = convert_type(*m->Type);
    t.name = fix_name(m->Name);
    if (m->Direction() != MemberDefinition_Direction_writeonly)
    {
        w2 << "    Task<" << t.cs_type + t.cs_arr_type << "> async_get_" << t.name
           << "(int rr_timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE);" << endl;
    }
    if (m->Direction() != MemberDefinition_Direction_readonly)
    {
        w2 << "    Task async_set_" << t.name << "(" << t.cs_type + t.cs_arr_type
           << " value, int rr_timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE);" << endl;
    }
    // w2 << "    " + t[1] + t[2] + " " + t[0] + " { get; set; }" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(FunctionDefinition)
    if (!m->IsGenerator())
    {
        convert_type_result t = convert_type(*m->ReturnType);
        string params = str_pack_parameters(m->Parameters, true);

        vector<string> t2;
        if (m->Parameters.size() > 0)
        {
            t2.push_back(params);
        }
        std::string task_type;
        if (m->ReturnType->Type == DataTypes_void_t)
        {
            // t2.push_back("Action<Exception> rr_handler");
            task_type = "Task";
        }
        else
        {
            // t2.push_back("Action<" + t.cs_type + t.cs_arr_type + ",Exception> rr_handler");
            task_type = "Task<" + t.cs_type + t.cs_arr_type + ">";
        }
        t2.push_back("int rr_timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE");

        w2 << "    " << task_type << " async_" + fix_name(m->Name) + "(" + boost::join(t2, ",") + ");" << endl;
    }
    else
    {
        convert_generator_result t = convert_generator(m.get());
        string params = str_pack_parameters(t.params, true);

        vector<string> t2;
        if (t.params.size() > 0)
        {
            t2.push_back(params);
        }

        // t2.push_back("Action<" + t.generator_csharp_type + ",Exception> rr_handler");

        t2.push_back("int rr_timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE");

        w2 << "    Task<" + t.generator_csharp_type + "> async_" + fix_name(m->Name) + "(" + boost::join(t2, ",") + ");"
           << endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(ObjRefDefinition)
    string objtype = fix_qualified_name(m->ObjectType);
    if (objtype == "varobject")
        objtype = "object";
    std::string indtype;
    if (GetObjRefIndType(m, indtype))
    {
        w2 << "    Task<" + objtype + "> async_get_" + fix_name(m->Name) + "(" + indtype +
                  " ind, int timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE);"
           << endl;
    }
    else
    {
        w2 << "    Task<" + objtype + "> async_get_" + fix_name(m->Name) +
                  "(int timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE);"
           << endl;
    }
    MEMBER_ITER_END()

    w2 << "}" << endl;

    w2 << "public class " + fix_name(e->Name) + "_stub : ServiceStub , " + fix_name(e->Name) + ", async_" +
              fix_name(e->Name) + "{"
       << endl;

    MEMBER_ITER2(CallbackDefinition)
    w2 << "    private CallbackClient<" + str_pack_delegate(m->Parameters, m->ReturnType) + "> rr_" +
              fix_name(m->Name) + ";"
       << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(PipeDefinition)
    convert_type_result t = convert_type(*m->Type);
    w2 << "    private Pipe<" + t.cs_type + t.cs_arr_type + "> rr_" + fix_name(m->Name) + ";" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    convert_type_result t = convert_type(*m->Type);
    w2 << "    private Wire<" + t.cs_type + t.cs_arr_type + "> rr_" + fix_name(m->Name) + ";" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type(t2);
    std::string c = "";
    if (!IsTypeNumeric(m->Type->Type))
    {
        DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();
        if (entry_type != DataTypes_namedarray_t)
        {
            c = "Pod";
        }
        else
        {
            c = "Named";
        }
    }
    switch (m->Type->ArrayType)
    {
    case DataTypes_ArrayTypes_array:
        w2 << "    private " << c << "ArrayMemory<" + t.cs_type + "> rr_" + fix_name(m->Name) + ";" << endl;
        break;
    case DataTypes_ArrayTypes_multidimarray:
        w2 << "    private " << c << "MultiDimArrayMemory<" + t.cs_type + "> rr_" + fix_name(m->Name) + ";" << endl;
        break;
    default:
        throw DataTypeException("Invalid memory definition");
    }

    MEMBER_ITER_END()

    w2 << "    public " + fix_name(e->Name) + "_stub(WrappedServiceStub innerstub) : base(innerstub) {" << endl;
    MEMBER_ITER2(CallbackDefinition)
    w2 << "    rr_" + fix_name(m->Name) + "=new CallbackClient<" + str_pack_delegate(m->Parameters, m->ReturnType) +
              ">(\"" + m->Name + "\");"
       << endl;
    MEMBER_ITER_END()
    MEMBER_ITER2(PipeDefinition)
    convert_type_result t = convert_type(*m->Type);
    w2 << "    rr_" + fix_name(m->Name) + "=new Pipe<" + t.cs_type + t.cs_arr_type + ">(innerstub.GetPipe(\"" +
              m->Name + "\"));"
       << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    convert_type_result t = convert_type(*m->Type);
    w2 << "    rr_" + fix_name(m->Name) + "=new Wire<" + t.cs_type + t.cs_arr_type + ">(innerstub.GetWire(\"" +
              m->Name + "\"));"
       << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type(t2);
    std::string c = "";
    if (!IsTypeNumeric(m->Type->Type))
    {
        DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();
        if (entry_type != DataTypes_namedarray_t)
        {
            c = "Pod";
        }
        else
        {
            c = "Named";
        }
    }
    switch (m->Type->ArrayType)
    {
    case DataTypes_ArrayTypes_array:
        w2 << "    rr_" << fix_name(m->Name) << "=new " << c << "ArrayMemoryClient<" + t.cs_type + ">(innerstub.Get"
           << c << "ArrayMemory(\"" + m->Name + "\"));" << endl;
        break;
    case DataTypes_ArrayTypes_multidimarray:
        w2 << "    rr_" << fix_name(m->Name) << "=new " << c
           << "MultiDimArrayMemoryClient<" + t.cs_type + ">(innerstub.Get" << c
           << "MultiDimArrayMemory(\"" + m->Name + "\"));" << endl;
        break;
    default:
        throw DataTypeException("Invalid memory definition");
    }
    MEMBER_ITER_END()

    w2 << "    }" << endl;

    MEMBER_ITER2(PropertyDefinition)
    convert_type_result t = convert_type(*m->Type);
    t.name = fix_name(m->Name);
    w2 << "    public " + t.cs_type + t.cs_arr_type + " " + t.name + " {" << endl;
    if (m->Direction() != MemberDefinition_Direction_writeonly)
    {
        w2 << "    get {" << endl;
        w2 << "    return " + str_unpack_message_element("rr_innerstub.PropertyGet(\"" + m->Name + "\")", m->Type) + ";"
           << endl;
        w2 << "    }" << endl;
    }
    if (m->Direction() != MemberDefinition_Direction_readonly)
    {
        w2 << "    set {" << endl;
        w2 << "    using(MessageElement m=" + str_pack_message_element("value", "value", m->Type) + ")" << endl
           << "    {" << endl;
        w2 << "    rr_innerstub.PropertySet(\"" + m->Name + "\", m);" << endl;
        w2 << "    }" << endl;
        w2 << "    }" << endl;
    }
    w2 << "    }" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(FunctionDefinition)
    if (!m->IsGenerator())
    {
        convert_type_result t = convert_type(*m->ReturnType);
        string params = str_pack_parameters(m->Parameters, true);
        w2 << "    public " + t.cs_type + t.cs_arr_type + " " + fix_name(m->Name) + "(" + params + ") {" << endl;
        w2 << "    using(vectorptr_messageelement rr_param=new vectorptr_messageelement())" << endl << "    {" << endl;
        for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin(); p != m->Parameters.end();
             ++p)
        {
            w2 << "    MessageElementUtil.AddMessageElementDispose(rr_param,"
               << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << endl;
        }
        w2 << "    using(MessageElement rr_me=rr_innerstub.FunctionCall(\"" + m->Name + "\",rr_param))" << endl
           << "    {" << endl;
        if (m->ReturnType->Type != DataTypes_void_t)
        {
            w2 << "    return " << str_unpack_message_element("rr_me", m->ReturnType) + ";" << endl;
        }
        w2 << "    }" << endl;
        w2 << "    }" << endl;
        w2 << "    }" << endl;
    }
    else
    {
        convert_generator_result t = convert_generator(m.get());
        string params = str_pack_parameters(t.params, true);
        w2 << "    public " + t.generator_csharp_type + " " + fix_name(m->Name) + "(" + params + ") {" << endl;
        w2 << "    using(vectorptr_messageelement rr_param=new vectorptr_messageelement())" << endl << "    {" << endl;
        for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = t.params.begin(); p != t.params.end(); ++p)
        {
            w2 << "    MessageElementUtil.AddMessageElementDispose(rr_param,"
               << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << endl;
        }
        w2 << "    WrappedGeneratorClient generator_client = rr_innerstub.GeneratorFunctionCall(\"" + m->Name +
                  "\",rr_param);"
           << endl;

        w2 << "    return new " << t.generator_csharp_base_type << "Client<" << t.generator_csharp_template_params
           << ">(generator_client);" << endl;

        w2 << "    }" << endl;
        w2 << "    }" << endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(EventDefinition)
    string params = str_pack_parameters(m->Parameters, true);
    w2 << "    public event " << str_pack_delegate(m->Parameters) << " " << fix_name(m->Name) << ";" << endl;
    MEMBER_ITER_END()

    w2 << "    public override void DispatchEvent(string rr_membername, vectorptr_messageelement rr_m) {" << endl;
    w2 << "    switch (rr_membername) {" << endl;
    MEMBER_ITER2(EventDefinition)
    string params = str_pack_parameters(m->Parameters, false);
    w2 << "    case \"" + m->Name + "\":" << endl << "    {" << endl;
    w2 << "    if (" + fix_name(m->Name) + " != null) { " << endl;
    for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin(); p != m->Parameters.end();
         ++p)
    {
        convert_type_result t3 = convert_type(*(*p));
        w2 << "    " + t3.cs_type + t3.cs_arr_type + " " + fix_name((*p)->Name) + "=" +
                  str_unpack_message_element("vectorptr_messageelement_util.FindElement(rr_m,\"" + (*p)->Name + "\")",
                                             *p) +
                  ";"
           << endl;
        ;
    }
    w2 << "    " + fix_name(m->Name) + "(" + params + ");" << endl;
    w2 << "    }" << endl;
    w2 << "    return;" << endl;
    w2 << "    }" << endl;
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    }" << endl;

    MEMBER_ITER2(ObjRefDefinition)
    string objtype = fix_qualified_name(m->ObjectType);
    if (objtype == "varobject")
    {
        objtype = "object";
        std::string indtype;
        if (GetObjRefIndType(m, indtype))
        {
            w2 << "    public " + objtype + " get_" + fix_name(m->Name) + "(" + indtype + " ind) {" << endl;
            w2 << "    return (" + objtype + ")FindObjRef(\"" + m->Name + "\",ind.ToString());" << endl;
            w2 << "    }" << endl;
        }
        else
        {
            w2 << "    public " + objtype + " get_" + fix_name(m->Name) + "() {" << endl;
            w2 << "    return (" + objtype + ")FindObjRef(\"" + m->Name + "\");" << endl;
            w2 << "    }" << endl;
        }
    }
    else
    {

        boost::shared_ptr<ServiceDefinition> d = e->ServiceDefinition_.lock();
        if (!d)
            throw DataTypeException("Invalid object type name");

        string objecttype2 = "";

        std::string s2 = fix_qualified_name(m->ObjectType);

        if (s2.find('.') == std::string::npos)
        {
            objecttype2 = fix_name(d->Name) + "." + s2;
        }
        else
        {
            objecttype2 = s2;
        }

        std::string indtype;
        if (GetObjRefIndType(m, indtype))
        {
            w2 << "    public " + objtype + " get_" + fix_name(m->Name) + "(" + indtype + " ind) {" << endl;
            w2 << "    return (" + objtype + ")FindObjRefTyped(\"" + fix_name(m->Name) + "\",ind.ToString(),\""
               << objecttype2 << "\");" << endl;
            w2 << "    }" << endl;
        }
        else
        {
            w2 << "    public " + objtype + " get_" + fix_name(m->Name) + "() {" << endl;
            w2 << "    return (" + objtype + ")FindObjRefTyped(\"" + m->Name + "\",\"" << objecttype2 << "\");" << endl;
            w2 << "    }" << endl;
        }
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(PipeDefinition)
    convert_type_result t = convert_type(*m->Type);
    w2 << "    public Pipe<" + t.cs_type + t.cs_arr_type + "> " + fix_name(m->Name) + " {" << endl;
    w2 << "    get { return rr_" + fix_name(m->Name) + ";  }" << endl;
    w2 << "    set { throw new InvalidOperationException();}" << endl;
    w2 << "    }" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(CallbackDefinition)
    w2 << "    public Callback<" + str_pack_delegate(m->Parameters, m->ReturnType) + "> " + fix_name(m->Name) + " {"
       << endl;
    w2 << "    get { return rr_" + fix_name(m->Name) + ";  }" << endl;
    w2 << "    set { throw new InvalidOperationException();}" << endl;
    w2 << "    }" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    convert_type_result t = convert_type(*m->Type);
    w2 << "    public Wire<" + t.cs_type + t.cs_arr_type + "> " + fix_name(m->Name) + " {" << endl;
    w2 << "    get { return rr_" + fix_name(m->Name) + ";  }" << endl;
    w2 << "    set { throw new InvalidOperationException();}" << endl;
    w2 << "    }" << endl;
    MEMBER_ITER_END()

    w2 << "    public override MessageElement CallbackCall(string rr_membername, vectorptr_messageelement rr_m) {"
       << endl;
    w2 << "    switch (rr_membername) {" << endl;
    MEMBER_ITER2(CallbackDefinition)
    string params = str_pack_parameters(m->Parameters, false);
    w2 << "    case \"" + m->Name + "\":" << endl << "    {" << endl;

    for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin(); p != m->Parameters.end();
         ++p)
    {
        convert_type_result t3 = convert_type(*(*p));
        w2 << "    " + t3.cs_type + t3.cs_arr_type + " " + fix_name((*p)->Name) + "=" +
                  str_unpack_message_element("vectorptr_messageelement_util.FindElement(rr_m,\"" + (*p)->Name + "\")",
                                             *p) +
                  ";"
           << endl;
        ;
    }
    if (m->ReturnType->Type == DataTypes_void_t)
    {
        w2 << "    this." + fix_name(m->Name) + ".Function(" + params + ");" << endl;
        w2 << "    return new MessageElement(\"return\",(int)0);" << endl;
    }
    else
    {
        convert_type_result t = convert_type(*m->ReturnType);
        w2 << "    " + t.cs_type + t.cs_arr_type + " ret=" + fix_name(m->Name) + ".Function(" + params + ");" << endl;
        w2 << "    return " + str_pack_message_element("return", "ret", m->ReturnType) + ";" << endl;
    }
    w2 << "    }" << endl;
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"Member not found\");" << endl;
    w2 << "    }" << endl;

    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type(t2);

    std::string c = "";
    if (!IsTypeNumeric(m->Type->Type))
    {
        DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();
        if (entry_type != DataTypes_namedarray_t)
        {
            c = "Pod";
        }
        else
        {
            c = "Named";
        }
    }
    switch (m->Type->ArrayType)
    {
    case DataTypes_ArrayTypes_array:
        w2 << "    public " << c << "ArrayMemory<" + t.cs_type + "> " + fix_name(m->Name) + " { " << endl;
        break;
    case DataTypes_ArrayTypes_multidimarray:

        w2 << "    public " << c << "MultiDimArrayMemory<" + t.cs_type + "> " + fix_name(m->Name) + " {" << endl;
        break;
    default:
        throw DataTypeException("Invalid memory definition");
    }
    w2 << "    get { return rr_" + fix_name(m->Name) + "; }" << endl;

    w2 << "    }" << endl;
    MEMBER_ITER_END()

    // Async functions

    MEMBER_ITER2(PropertyDefinition)
    convert_type_result t = convert_type(*m->Type);
    t.name = fix_name(m->Name);
    if (m->Direction() != MemberDefinition_Direction_writeonly)
    {
        w2 << "    public virtual async Task<" << t.cs_type + t.cs_arr_type << "> async_get_" << t.name
           << "(int rr_timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE)" << endl
           << "    {" << endl;
        w2 << "    using(var rr_value = await rr_async_PropertyGet(\"" + m->Name + "\",rr_timeout)) {" << endl;
        w2 << "    var rr_ret=" << str_unpack_message_element("rr_value", m->Type) << ";" << endl;
        w2 << "    return rr_ret;" << endl;
        w2 << "    } }" << endl;
    }
    if (m->Direction() != MemberDefinition_Direction_readonly)
    {
        w2 << "    public virtual async Task async_set_" << t.name << "(" << t.cs_type + t.cs_arr_type
           << " value, int rr_timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE)" << endl
           << "    {" << endl;
        w2 << "    using(MessageElement mm=" << str_pack_message_element("value", "value", m->Type) << ")" << endl
           << "    {" << endl;
        w2 << "    await rr_async_PropertySet(\"" + m->Name + "\",mm,rr_timeout);" << endl;
        w2 << "    }" << endl;
        w2 << "    }" << endl;
    }

    // w2 << "    " + t[1] + t[2] + " " + t[0] + " { get; set; }" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(FunctionDefinition)
    if (!m->IsGenerator())
    {
        convert_type_result t = convert_type(*m->ReturnType);
        string params = str_pack_parameters(m->Parameters, true);

        vector<string> t2;
        if (m->Parameters.size() > 0)
        {
            t2.push_back(params);
        }
        std::string task_type;
        if (m->ReturnType->Type == DataTypes_void_t)
        {
            // t2.push_back("Action<Exception> rr_handler");
            task_type = "Task";
        }
        else
        {
            // t2.push_back("Action<" + t.cs_type + t.cs_arr_type + ",Exception> rr_handler");
            task_type = "Task<" + t.cs_type + t.cs_arr_type + ">";
        }
        t2.push_back("int rr_timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE");

        w2 << "    public virtual async " << task_type
           << " async_" + fix_name(m->Name) + "(" + boost::join(t2, ",") + ")" << endl
           << "    {" << endl;
        w2 << "    using(vectorptr_messageelement rr_param=new vectorptr_messageelement())" << endl << "    {" << endl;
        for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin(); p != m->Parameters.end();
             ++p)
        {
            w2 << "    MessageElementUtil.AddMessageElementDispose(rr_param,"
               << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << endl;
        }

        w2 << "    using(var rr_return = await rr_async_FunctionCall(\"" + m->Name + "\",rr_param,rr_timeout)) {"
           << endl;
        if (m->ReturnType->Type == DataTypes_void_t)
        {}
        else
        {
            w2 << "    var rr_ret=" << str_unpack_message_element("rr_return", m->ReturnType) << ";" << endl;
            w2 << "    return rr_ret;" << endl;
        }
        w2 << "    } } }" << endl;
    }
    else
    {
        convert_generator_result t = convert_generator(m.get());
        string params = str_pack_parameters(t.params, true);

        vector<string> t2;
        if (t.params.size() > 0)
        {
            t2.push_back(params);
        }

        // t2.push_back("Action<" + t.generator_csharp_type + ",Exception> rr_handler");

        t2.push_back("int rr_timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE");

        w2 << "    public virtual async Task<" + t.generator_csharp_type + "> async_" + fix_name(m->Name) + "(" +
                  boost::join(t2, ",") + ")"
           << endl
           << "    {" << endl;
        w2 << "    using(vectorptr_messageelement rr_param=new vectorptr_messageelement())" << endl << "    {" << endl;
        for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = t.params.begin(); p != t.params.end(); ++p)
        {
            w2 << "    MessageElementUtil.AddMessageElementDispose(rr_param,"
               << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << endl;
        }

        w2 << "    var rr_return = await rr_async_GeneratorFunctionCall(\"" + m->Name + "\",rr_param,rr_timeout);"
           << endl;

        w2 << "    " << t.generator_csharp_base_type << "Client< " << t.generator_csharp_template_params
           << "> rr_ret=new " << t.generator_csharp_base_type << "Client< " << t.generator_csharp_template_params
           << ">(rr_return);" << endl;
        w2 << "    return rr_ret;" << endl;
        w2 << "    } }" << endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(ObjRefDefinition)
    string objtype = fix_qualified_name(m->ObjectType);
    if (objtype == "varobject")
    {
        objtype = "object";
        std::string indtype;
        if (GetObjRefIndType(m, indtype))
        {
            w2 << "    public Task<" + objtype + "> async_get_" + fix_name(m->Name) + "(" + indtype +
                      " ind, int timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE) {"
               << endl;
            w2 << "    return AsyncFindObjRef(\"" + m->Name + "\",ind.ToString(),timeout);" << endl;
            w2 << "    }" << endl;
        }
        else
        {
            w2 << "    public Task<" + objtype + "> async_get_" + fix_name(m->Name) +
                      "(int timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE) {"
               << endl;
            w2 << "    return AsyncFindObjRef(\"" + m->Name + "\",timeout);" << endl;
            w2 << "    }" << endl;
        }
    }
    else
    {

        boost::shared_ptr<ServiceDefinition> d = e->ServiceDefinition_.lock();
        if (!d)
            throw DataTypeException("Invalid object type name");

        string objecttype2 = "";

        std::string s2 = fix_qualified_name(m->ObjectType);

        if (s2.find('.') == std::string::npos)
        {
            objecttype2 = fix_name(d->Name) + "." + s2;
        }
        else
        {
            objecttype2 = s2;
        }

        std::string indtype;
        if (GetObjRefIndType(m, indtype))
        {
            w2 << "    public Task<" + objtype + ">  async_get_" + fix_name(m->Name) + "(" + indtype +
                      " ind, int timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE)"
               << endl
               << "    {" << endl;
            w2 << "    return AsyncFindObjRefTyped<" + objtype + ">(\"" + fix_name(m->Name) + "\",ind.ToString(),\""
               << objecttype2 << "\",timeout);" << endl;
            w2 << "    }" << endl;
        }
        else
        {
            w2 << "    public Task<" + objtype + "> async_get_" + fix_name(m->Name) +
                      "(int timeout=RobotRaconteurNode.RR_TIMEOUT_INFINITE)"
               << endl
               << "    {" << endl;
            w2 << "    return AsyncFindObjRefTyped<" + objtype + ">(\"" + m->Name + "\",\"" << objecttype2
               << "\",timeout);" << endl;
            w2 << "    }" << endl;
        }
    }
    MEMBER_ITER_END()

    w2 << "}" << endl;
}

void CSharpServiceLangGen::GenerateSkel(ServiceEntryDefinition* e, ostream* w)
{
    ostream& w2 = *w;

    w2 << "public class " + fix_name(e->Name) + "_skel : ServiceSkel {" << endl;
    w2 << "    protected " + fix_name(e->Name) + " obj;" << endl;
    w2 << "    protected async_" + fix_name(e->Name) + " async_obj;" << endl;
    w2 << "    public " + fix_name(e->Name) + "_skel(object o) : base(o)"
       << "    {" << endl;
    w2 << "    obj=(" + fix_name(e->Name) + ")o;" << endl;
    w2 << "    async_obj = o as async_" << fix_name(e->Name) << ";" << endl << "    }" << endl;
    w2 << "    public override void ReleaseCastObject() { " << endl;
    w2 << "    obj=null;" << endl;
    w2 << "    async_obj=null;" << endl;
    w2 << "    base.ReleaseCastObject();" << endl;
    w2 << "    }" << endl;

    w2 << "    public override MessageElement CallGetProperty(string membername, WrappedServiceSkelAsyncAdapter "
          "async_adapter) {"
       << endl;
    w2 << "    switch (membername) {" << endl;
    MEMBER_ITER2(PropertyDefinition)
    if (m->Direction() != MemberDefinition_Direction_writeonly)
    {
        w2 << "    case \"" + m->Name + "\":" << endl << "    {" << endl;

        convert_type_result t = convert_type(*m->Type);
        w2 << "    if (async_obj!=null)"
           << "    {" << endl;
        w2 << "    async_adapter.MakeAsync();" << endl;
        w2 << "    async_obj.async_get_" << fix_name(m->Name) << "().ContinueWith(t => async_adapter.EndTask<"
           << t.cs_type + t.cs_arr_type << ">(t,"
           << "async_ret => " << str_pack_message_element("return", "async_ret", m->Type) << "));" << endl;
        w2 << "    return null;" << endl << "    }" << endl;
        w2 << "    " + t.cs_type + t.cs_arr_type + " ret=obj." + fix_name(m->Name) + ";" << endl;
        w2 << "    return " + str_pack_message_element("return", "ret", m->Type) + ";" << endl;

        w2 << "    }" << endl;
    }
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"Member not found\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override void CallSetProperty(string membername, MessageElement m, "
          "WrappedServiceSkelAsyncAdapter async_adapter) {"
       << endl;
    w2 << "    switch (membername) {" << endl;
    MEMBER_ITER2(PropertyDefinition)
    if (m->Direction() != MemberDefinition_Direction_readonly)
    {
        w2 << "    case \"" + m->Name + "\":" << endl << "    {" << endl;
        w2 << "    if (async_obj!=null)"
           << "    {" << endl;
        w2 << "    async_adapter.MakeAsync();" << endl;
        w2 << "    async_obj.async_set_" << fix_name(m->Name) << "(" << str_unpack_message_element("m", m->Type)
           << ").ContinueWith(t => async_adapter.EndTask(t));" << endl;
        w2 << "    return;" << endl << "    }" << endl;
        w2 << "    obj." + fix_name(m->Name) + "=" + str_unpack_message_element("m", m->Type) + ";" << endl;
        w2 << "    return;" << endl;
        w2 << "    }" << endl;
    }
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"Member not found\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override MessageElement CallFunction(string rr_membername, vectorptr_messageelement rr_m, "
          "WrappedServiceSkelAsyncAdapter rr_async_adapter) {"
       << endl;
    w2 << "    switch (rr_membername) {" << endl;
    MEMBER_ITER2(FunctionDefinition)
    if (!m->IsGenerator())
    {
        w2 << "    case \"" + m->Name + "\":" << endl << "    {" << endl;

        string params = str_pack_parameters(m->Parameters, false);
        for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin(); p != m->Parameters.end();
             ++p)
        {
            convert_type_result t3 = convert_type(*(*p));
            w2 << "    " + t3.cs_type + t3.cs_arr_type + " " + fix_name((*p)->Name) + "=" +
                      str_unpack_message_element(
                          "vectorptr_messageelement_util.FindElement(rr_m,\"" + (*p)->Name + "\")", *p) +
                      ";"
               << endl;
            ;
        }

        w2 << "    if (async_obj!=null)"
           << "    {" << endl;
        w2 << "    rr_async_adapter.MakeAsync();" << endl;
        w2 << "    async_obj.async_" << fix_name(m->Name) << "(" << params << ")";
        if (m->ReturnType->Type == DataTypes_void_t)
        {
            w2 << ".ContinueWith(t => rr_async_adapter.EndTask(t,new MessageElement(\"return\",(int)0)));";
        }
        else
        {
            convert_type_result t = convert_type(*m->ReturnType);
            w2 << ".ContinueWith(t => rr_async_adapter.EndTask<" << t.cs_type + t.cs_arr_type << ">(t,"
               << "async_ret => " << str_pack_message_element("return", "async_ret", m->ReturnType) << "));";
        }
        w2 << endl;
        w2 << "    return null;" << endl << "    }" << endl;

        if (m->ReturnType->Type == DataTypes_void_t)
        {
            w2 << "    this.obj." + fix_name(m->Name) + "(" + params + ");" << endl;
            w2 << "    return new MessageElement(\"return\",(int)0);" << endl;
        }
        else
        {
            convert_type_result t = convert_type(*m->ReturnType);
            w2 << "    " + t.cs_type + t.cs_arr_type + " rr_ret=this.obj." + fix_name(m->Name) + "(" + params + ");"
               << endl;
            w2 << "    return " + str_pack_message_element("return", "rr_ret", m->ReturnType) + ";" << endl;
        }
        w2 << "    }" << endl;
    }
    else
    {
        w2 << "    case \"" + m->Name + "\":" << endl << "    {" << endl;
        convert_generator_result t4 = convert_generator(m.get());
        string params = str_pack_parameters(t4.params, false);
        for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = t4.params.begin(); p != t4.params.end(); ++p)
        {
            convert_type_result t3 = convert_type(*(*p));
            w2 << "    " + t3.cs_type + t3.cs_arr_type + " " + fix_name((*p)->Name) + "=" +
                      str_unpack_message_element(
                          "vectorptr_messageelement_util.FindElement(rr_m,\"" + (*p)->Name + "\")", *p) +
                      ";"
               << endl;
            ;
        }

        w2 << "    " + t4.generator_csharp_type + " rr_ret=this.obj." + fix_name(m->Name) + "(" + params + ");" << endl;
        w2 << "    int generator_index = innerskel.RegisterGeneratorServer(\"" << m->Name << "\", new Wrapped"
           << t4.generator_csharp_base_type << "ServerDirectorNET<" << t4.generator_csharp_template_params
           << ">(rr_ret));" << endl;
        w2 << "    return new MessageElement(\"index\",generator_index);" << endl;
        w2 << "    }" << endl;
    }
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"Member not found\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override object GetSubObj(string name, string ind) {" << endl;
    w2 << "    switch (name) {" << endl;
    MEMBER_ITER2(ObjRefDefinition)
    w2 << "    case \"" + m->Name + "\": {" << endl;
    std::string indtype;
    if (GetObjRefIndType(m, indtype))
    {
        if (indtype == "int")
        {
            w2 << "    return obj.get_" + fix_name(m->Name) + "(Int32.Parse(ind));" << endl;
        }
        else
        {
            w2 << "    return obj.get_" + fix_name(m->Name) + "(ind);" << endl;
        }
    }
    else
    {
        w2 << "    return obj.get_" + fix_name(m->Name) + "();" << endl;
    }
    w2 << "    }" << endl;
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override void RegisterEvents(object rrobj1) {" << endl;
    w2 << "    obj=(" + fix_name(e->Name) + ")rrobj1;" << endl;
    MEMBER_ITER2(EventDefinition)
    w2 << "    obj." + fix_name(m->Name) + "+=rr_" + fix_name(m->Name) + ";" << endl;
    MEMBER_ITER_END()
    w2 << "    }" << endl;

    w2 << "    public override void UnregisterEvents(object rrobj1) {" << endl;
    w2 << "    obj=(" + fix_name(e->Name) + ")rrobj1;" << endl;
    MEMBER_ITER2(EventDefinition)
    w2 << "    obj." + fix_name(m->Name) + "-=rr_" + fix_name(m->Name) + ";" << endl;
    MEMBER_ITER_END()
    w2 << "    }" << endl;

    MEMBER_ITER2(EventDefinition)

    string params = str_pack_parameters(m->Parameters, true);
    w2 << "    public void rr_" + fix_name(m->Name) + "(" + params + ") {" << endl;
    w2 << "    using(vectorptr_messageelement rr_param=new vectorptr_messageelement()) {" << endl;
    for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin(); p != m->Parameters.end();
         ++p)
    {
        w2 << "    MessageElementUtil.AddMessageElementDispose(rr_param,"
           << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << endl;
    }
    w2 << "    this.innerskel.WrappedDispatchEvent(\"" + m->Name + "\",rr_param);" << endl;

    w2 << "    }" << endl;
    w2 << "    }" << endl;
    MEMBER_ITER_END()

    w2 << "    public override object GetCallbackFunction(uint rr_endpoint, string rr_membername) {" << endl;
    w2 << "    switch (rr_membername) {" << endl;
    MEMBER_ITER2(CallbackDefinition)
    convert_type_result t = convert_type(*m->ReturnType);
    string params = str_pack_parameters(m->Parameters, true);
    w2 << "    case \"" + m->Name + "\": {" << endl;
    w2 << "    return new " + str_pack_delegate(m->Parameters, m->ReturnType) + "( delegate(" +
              str_pack_parameters(m->Parameters, true) + ") {"
       << endl;
    ;
    w2 << "    using(vectorptr_messageelement rr_param=new vectorptr_messageelement())" << endl << "    {" << endl;
    for (vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin(); p != m->Parameters.end();
         ++p)
    {
        w2 << "    MessageElementUtil.AddMessageElementDispose(rr_param,"
           << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << endl;
    }
    w2 << "    using(MessageElement rr_me=this.innerskel.WrappedCallbackCall(\"" + m->Name + "\",rr_endpoint,rr_param))"
       << endl
       << "    {" << endl;
    if (m->ReturnType->Type != DataTypes_void_t)
    {
        w2 << "    return " << str_unpack_message_element("rr_me", m->ReturnType) + ";" << endl;
    }
    w2 << "    }" << endl;
    w2 << "    }" << endl;
    w2 << "    });" << endl;
    w2 << "    }" << endl;
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"Member not found\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override void InitPipeServers(object rrobj1) {" << endl;
    w2 << "    obj=(" + fix_name(e->Name) + ")rrobj1;" << endl;
    MEMBER_ITER2(PipeDefinition)
    convert_type_result t = convert_type(*m->Type);
    w2 << "    obj." + fix_name(m->Name) + "=new Pipe<" + t.cs_type + t.cs_arr_type + ">(innerskel.GetPipe(\"" +
              m->Name + "\"));"
       << endl;
    MEMBER_ITER_END()
    w2 << "    }" << endl;

    w2 << "    public override void InitCallbackServers(object rrobj1) {" << endl;
    w2 << "    obj=(" + fix_name(e->Name) + ")rrobj1;" << endl;
    MEMBER_ITER2(CallbackDefinition)

    w2 << "    obj." + fix_name(m->Name) + "=new CallbackServer<" + str_pack_delegate(m->Parameters, m->ReturnType) +
              ">(\"" + m->Name + "\",this);"
       << endl;
    MEMBER_ITER_END()
    w2 << "    }" << endl;

    w2 << "    public override void InitWireServers(object rrobj1) {" << endl;
    w2 << "    obj=(" + fix_name(e->Name) + ")rrobj1;" << endl;
    MEMBER_ITER2(WireDefinition)
    convert_type_result t = convert_type(*m->Type);
    w2 << "    obj." + fix_name(m->Name) + "=new Wire<" + t.cs_type + t.cs_arr_type + ">(innerskel.GetWire(\"" +
              m->Name + "\"));"
       << endl;
    MEMBER_ITER_END()
    w2 << "    }" << endl;

    w2 << "    public override WrappedArrayMemoryDirector GetArrayMemory(string name) {" << endl;
    w2 << "    switch (name) {" << endl;
    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type(t2);
    if (!IsTypeNumeric(m->Type->Type))
        continue;
    if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
    {
        w2 << "    case \"" + m->Name + "\": {" << endl;
        w2 << "    WrappedArrayMemoryDirectorNET<" + t.cs_type + "> dir=new  WrappedArrayMemoryDirectorNET<" +
                  t.cs_type + ">(obj." + fix_name(m->Name) + ");"
           << endl;
        // w2 << "    int id=RRObjectHeap.AddObject(dir); " << endl;
        // w2 << "    dir.memoryid=id;" << endl;
        // w2 << "    dir.Disown();" << endl;
        w2 << "    return dir;" << endl;
        w2 << "    }" << endl;
    }
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override WrappedMultiDimArrayMemoryDirector GetMultiDimArrayMemory(string name) {" << endl;
    w2 << "    switch (name) {" << endl;
    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type(t2);
    if (!IsTypeNumeric(m->Type->Type))
        continue;
    if (m->Type->ArrayType == DataTypes_ArrayTypes_multidimarray)
    {
        w2 << "    case \"" + m->Name + "\": {" << endl;
        w2 << "    WrappedMultiDimArrayMemoryDirectorNET<" + t.cs_type +
                  "> dir=new  WrappedMultiDimArrayMemoryDirectorNET<" + t.cs_type + ">(obj." + fix_name(m->Name) + ");"
           << endl;
        // w2 << "    int id=RRObjectHeap.AddObject(dir); " << endl;
        // w2 << "    dir.memoryid=id;" << endl;
        // w2 << "    dir.Disown();" << endl;
        w2 << "    return dir;" << endl;
        w2 << "    }" << endl;
    }
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override WrappedPodArrayMemoryDirector GetPodArrayMemory(string name) {" << endl;
    w2 << "    switch (name) {" << endl;
    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type(t2);
    if (IsTypeNumeric(m->Type->Type))
        continue;

    DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();
    if (entry_type != DataTypes_pod_t)
        continue;

    if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
    {
        w2 << "    case \"" + m->Name + "\": {" << endl;
        w2 << "    WrappedPodArrayMemoryDirectorNET<" + t.cs_type + "> dir=new  WrappedPodArrayMemoryDirectorNET<" +
                  t.cs_type + ">(obj." + fix_name(m->Name) + ");"
           << endl;
        // w2 << "    int id=RRObjectHeap.AddObject(dir); " << endl;
        // w2 << "    dir.memoryid=id;" << endl;
        // w2 << "    dir.Disown();" << endl;
        w2 << "    return dir;" << endl;
        w2 << "    }" << endl;
    }
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override WrappedPodMultiDimArrayMemoryDirector GetPodMultiDimArrayMemory(string name) {" << endl;
    w2 << "    switch (name) {" << endl;
    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type(t2);
    if (IsTypeNumeric(m->Type->Type))
        continue;
    DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();

    if (entry_type != DataTypes_pod_t)
        continue;

    if (m->Type->ArrayType == DataTypes_ArrayTypes_multidimarray)
    {
        w2 << "    case \"" + m->Name + "\": {" << endl;
        w2 << "    WrappedPodMultiDimArrayMemoryDirectorNET<" + t.cs_type +
                  "> dir=new  WrappedPodMultiDimArrayMemoryDirectorNET<" + t.cs_type + ">(obj." + fix_name(m->Name) +
                  ");"
           << endl;
        // w2 << "    int id=RRObjectHeap.AddObject(dir); " << endl;
        // w2 << "    dir.memoryid=id;" << endl;
        // w2 << "    dir.Disown();" << endl;
        w2 << "    return dir;" << endl;
        w2 << "    }" << endl;
    }
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << endl;
    w2 << "    }" << endl;

    // namedarray
    w2 << "    public override WrappedNamedArrayMemoryDirector GetNamedArrayMemory(string name) {" << endl;
    w2 << "    switch (name) {" << endl;
    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type(t2);
    if (IsTypeNumeric(m->Type->Type))
        continue;

    DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();
    if (entry_type != DataTypes_namedarray_t)
        continue;

    if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
    {
        w2 << "    case \"" + m->Name + "\": {" << endl;
        w2 << "    WrappedNamedArrayMemoryDirectorNET<" + t.cs_type + "> dir=new  WrappedNamedArrayMemoryDirectorNET<" +
                  t.cs_type + ">(obj." + fix_name(m->Name) + ");"
           << endl;
        // w2 << "    int id=RRObjectHeap.AddObject(dir); " << endl;
        // w2 << "    dir.memoryid=id;" << endl;
        // w2 << "    dir.Disown();" << endl;
        w2 << "    return dir;" << endl;
        w2 << "    }" << endl;
    }
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override WrappedNamedMultiDimArrayMemoryDirector GetNamedMultiDimArrayMemory(string name) {"
       << endl;
    w2 << "    switch (name) {" << endl;
    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type(t2);
    if (IsTypeNumeric(m->Type->Type))
        continue;
    DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();

    if (entry_type != DataTypes_namedarray_t)
        continue;

    if (m->Type->ArrayType == DataTypes_ArrayTypes_multidimarray)
    {
        w2 << "    case \"" + m->Name + "\": {" << endl;
        w2 << "    WrappedNamedMultiDimArrayMemoryDirectorNET<" + t.cs_type +
                  "> dir=new  WrappedNamedMultiDimArrayMemoryDirectorNET<" + t.cs_type + ">(obj." + fix_name(m->Name) +
                  ");"
           << endl;
        // w2 << "    int id=RRObjectHeap.AddObject(dir); " << endl;
        // w2 << "    dir.memoryid=id;" << endl;
        // w2 << "    dir.Disown();" << endl;
        w2 << "    return dir;" << endl;
        w2 << "    }" << endl;
    }
    MEMBER_ITER_END()
    w2 << "    default:" << endl;
    w2 << "    break;" << endl;
    w2 << "    }" << endl;
    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << endl;
    w2 << "    }" << endl;

    w2 << "    public override string RRType { get { return \"" + e->ServiceDefinition_.lock()->Name + "." + e->Name +
              "\"; } }"
       << endl;

    w2 << "}" << endl;
}

void CSharpServiceLangGen::GenerateDefaultImpl(ServiceEntryDefinition* e, ostream* w)
{
    ostream& w2 = *w;

    w2 << "public class " + fix_name(e->Name) + "_default_impl : " + fix_name(e->Name) + "{" << endl;

    MEMBER_ITER2(CallbackDefinition)
    w2 << "    protected Callback<" + str_pack_delegate(m->Parameters, m->ReturnType) + "> rrvar_" + fix_name(m->Name) +
              ";"
       << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(PipeDefinition)
    if (m->Direction() == MemberDefinition_Direction_readonly)
    {
        convert_type_result t = convert_type(*m->Type);
        w2 << "    protected PipeBroadcaster<" + t.cs_type + t.cs_arr_type + "> rrvar_" + fix_name(m->Name) + ";"
           << endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    if (m->Direction() == MemberDefinition_Direction_readonly)
    {
        convert_type_result t = convert_type(*m->Type);
        w2 << "    protected WireBroadcaster<" + t.cs_type + t.cs_arr_type + "> rrvar_" + fix_name(m->Name) + ";"
           << endl;
    }
    if (m->Direction() == MemberDefinition_Direction_writeonly)
    {
        convert_type_result t = convert_type(*m->Type);
        w2 << "    protected WireUnicastReceiver<" + t.cs_type + t.cs_arr_type + "> rrvar_" + fix_name(m->Name) + ";"
           << endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(PropertyDefinition)
    convert_type_result t = convert_type(*m->Type);
    t.name = fix_name(m->Name);
    w2 << "    public virtual " + t.cs_type + t.cs_arr_type + " " + t.name + " {get; set;} = "
       << GetDefaultInitializedValue(*m->Type) << ";" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(FunctionDefinition)
    if (!m->IsGenerator())
    {
        convert_type_result t = convert_type(*m->ReturnType);
        string params = str_pack_parameters(m->Parameters, true);
        w2 << "    public virtual " + t.cs_type + t.cs_arr_type + " " + fix_name(m->Name) + "(" + params + ") {"
           << endl;
    }
    else
    {
        convert_generator_result t = convert_generator(m.get());
        string params = str_pack_parameters(t.params, true);
        w2 << "    public virtual " + t.generator_csharp_type + " " + fix_name(m->Name) + "(" + params + ") {" << endl;
    }
    w2 << "    throw new NotImplementedException();";
    w2 << "    }" << endl;

    MEMBER_ITER_END()

    MEMBER_ITER2(EventDefinition)
    string params = str_pack_parameters(m->Parameters, true);
    w2 << "    public virtual event " << str_pack_delegate(m->Parameters) << " " << fix_name(m->Name) << ";" << endl;
    w2 << "    protected virtual void rrfire_" << fix_name(m->Name) << "(" << str_pack_parameters(m->Parameters, true)
       << ") {" << endl;
    w2 << "    " << fix_name(m->Name) << "?.Invoke(" << str_pack_parameters(m->Parameters, false) << ");" << endl;
    w2 << "    }" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(ObjRefDefinition)
    string objtype = fix_qualified_name(m->ObjectType);
    if (objtype == "varobject")
        objtype = "object";
    std::string indtype;
    if (GetObjRefIndType(m, indtype))
    {
        w2 << "    public virtual " + objtype + " get_" + fix_name(m->Name) + "(" + indtype + " ind) {" << endl;
        w2 << "    throw new NotImplementedException();" << endl;
        w2 << "    }" << endl;
    }
    else
    {
        w2 << "    public virtual " + objtype + " get_" + fix_name(m->Name) + "() {" << endl;
        w2 << "    throw new NotImplementedException();" << endl;
        w2 << "    }" << endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(PipeDefinition)
    convert_type_result t = convert_type(*m->Type);
    w2 << "    public virtual Pipe<" + t.cs_type + t.cs_arr_type + "> " + fix_name(m->Name) + " {" << endl;
    if (m->Direction() == MemberDefinition_Direction_readonly)
    {
        w2 << "    get { return rrvar_" + fix_name(m->Name) + ".Pipe;  }" << endl;
    }
    else
    {
        w2 << "    get { throw new NotImplementedException(); }" << endl;
    }
    if (m->Direction() == MemberDefinition_Direction_readonly)
    {
        w2 << "    set {" << endl;
        w2 << "    if (rrvar_" << fix_name(m->Name)
           << "!=null) throw new InvalidOperationException(\"Pipe already set\");" << endl;
        w2 << "    rrvar_" << fix_name(m->Name) << "= new PipeBroadcaster<" << t.cs_type << t.cs_arr_type << ">(value);"
           << endl;
        w2 << "    }" << endl;
    }
    else
    {
        w2 << "    set { throw new InvalidOperationException();}" << endl;
    }
    w2 << "    }" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(CallbackDefinition)
    w2 << "    public virtual Callback<" + str_pack_delegate(m->Parameters, m->ReturnType) + "> " + fix_name(m->Name) +
              " {"
       << endl;
    w2 << "    get { return rrvar_" << fix_name(m->Name) << ";  }" << endl;
    w2 << "    set {" << endl;
    w2 << "    if (rrvar_" << fix_name(m->Name)
       << "!=null) throw new InvalidOperationException(\"Callback already set\");" << endl;
    w2 << "    rrvar_" << fix_name(m->Name) << "= value;" << endl;
    w2 << "    }" << endl;
    w2 << "    }" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    convert_type_result t = convert_type(*m->Type);
    w2 << "    public virtual Wire<" + t.cs_type + t.cs_arr_type + "> " + fix_name(m->Name) + " {" << endl;
    if (m->Direction() == MemberDefinition_Direction_readonly || m->Direction() == MemberDefinition_Direction_writeonly)
    {
        w2 << "    get { return rrvar_" + fix_name(m->Name) + ".Wire;  }" << endl;
    }
    else
    {
        w2 << "    get { throw new NotImplementedException(); }" << endl;
    }
    if (m->Direction() == MemberDefinition_Direction_readonly)
    {
        w2 << "    set {" << endl;
        w2 << "    if (rrvar_" << fix_name(m->Name)
           << "!=null) throw new InvalidOperationException(\"Pipe already set\");" << endl;
        w2 << "    rrvar_" << fix_name(m->Name) << "= new WireBroadcaster<" << t.cs_type << t.cs_arr_type << ">(value);"
           << endl;
        w2 << "    }" << endl;
    }
    else if (m->Direction() == MemberDefinition_Direction_writeonly)
    {
        w2 << "    set {" << endl;
        w2 << "    if (rrvar_" << fix_name(m->Name)
           << "!=null) throw new InvalidOperationException(\"Pipe already set\");" << endl;
        w2 << "    rrvar_" << fix_name(m->Name) << "= new WireUnicastReceiver<" << t.cs_type << t.cs_arr_type
           << ">(value);" << endl;
        w2 << "    }" << endl;
    }
    else
    {
        w2 << "    set { throw new NotImplementedException();}" << endl;
    }
    w2 << "    }" << endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type(t2);

    std::string c = "";
    if (!IsTypeNumeric(m->Type->Type))
    {
        DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();
        if (entry_type != DataTypes_namedarray_t)
        {
            c = "Pod";
        }
        else
        {
            c = "Named";
        }
    }
    switch (m->Type->ArrayType)
    {
    case DataTypes_ArrayTypes_array:
        w2 << "    public virtual " << c << "ArrayMemory<" + t.cs_type + "> " + fix_name(m->Name) + " { " << endl;
        break;
    case DataTypes_ArrayTypes_multidimarray:

        w2 << "    public virtual " << c << "MultiDimArrayMemory<" + t.cs_type + "> " + fix_name(m->Name) + " {"
           << endl;
        break;
    default:
        throw DataTypeException("Invalid memory definition");
    }
    w2 << "    get { throw new NotImplementedException(); }" << endl;
    w2 << "    }" << endl;
    MEMBER_ITER_END()

    w2 << "}" << endl;
}

template <typename T>
static void null_deleter(T*)
{}

static std::string CSharpServiceLangGen_EscapeString_Formatter(const boost::smatch& match)
{
    std::string i = match[0].str();

    if (i == "\"")
        return "\\\"";
    if (i == "\\")
        return "\\\\";
    if (i == "/")
        return "/";
    if (i == "\b")
        return "\\b";
    if (i == "\f")
        return "\\f";
    if (i == "\n")
        return "\\n";
    if (i == "\r")
        return "\\r";
    if (i == "\t")
        return "\\t";

    std::basic_string<uint16_t> v = boost::locale::conv::utf_to_utf<uint16_t>(i);

    std::stringstream v2;
    v2 << std::hex << std::setfill('0');
    BOOST_FOREACH (const uint16_t& v3, v)
    {
        v2 << std::setw(0) << "\\u" << std::setw(4) << v3;
    }

    return v2.str();
}

std::string CSharpServiceLangGen::convert_constant(ConstantDefinition* c,
                                                   std::vector<RR_SHARED_PTR<ConstantDefinition> >& c2,
                                                   ServiceDefinition* def)
{

    boost::shared_ptr<TypeDefinition> t = c->Type;
    if (t->ContainerType != DataTypes_ContainerTypes_none)
        throw DataTypeException("Only numbers, primitive number arrays, and strings can be constants");
    switch (t->ArrayType)
    {
    case DataTypes_ArrayTypes_none:
        break;
    case DataTypes_ArrayTypes_array:
        if (t->ArrayVarLength)
            break;
    default:
        throw DataTypeException("Only numbers, primitive number arrays, and strings can be constants");
    }

    if (t->Type == DataTypes_namedtype_t)
    {
        std::vector<ConstantDefinition_StructField> f = c->ValueToStructFields();

        std::string o = "public static class " + fix_name(c->Name) + " { ";

        BOOST_FOREACH (ConstantDefinition_StructField f2, f)
        {
            RR_SHARED_PTR<ConstantDefinition> c3 = TryFindByName(c2, f2.ConstantRefName);
            if (!c3)
                throw ServiceException("Invalid structure cosntant " + c->Name);
            o += convert_constant(c3.get(), c2, def) + " ";
        }

        o += "}";
        return o;
    }

    convert_type_result c1 = convert_type(*t);
    if (t->Type == DataTypes_string_t)
    {
        boost::regex r_replace("(\"|\\\\|\\/|[\\x00-\\x1F]|\\x7F|[\\x80-\\xFF]+)");

        std::ostringstream t(std::ios::out | std::ios::binary);
        std::ostream_iterator<char, char> oi(t);

        const std::string str_value = c->ValueToString();

        boost::regex_replace(oi, str_value.begin(), str_value.end(), r_replace,
                             CSharpServiceLangGen_EscapeString_Formatter, boost::match_default | boost::format_all);

        return "public const string " + fix_name(c->Name) + "=\"" + t.str() + "\";";
    }

    if (t->ArrayType == DataTypes_ArrayTypes_none)
    {
        return "public const " + c1.cs_type + " " + fix_name(c->Name) + "=" + c->Value + ";";
    }
    else
    {
        return "public static readonly " + c1.cs_type + "[] " + fix_name(c->Name) + "=" + c->Value + ";";
    }
}

void CSharpServiceLangGen::GenerateConstants(ServiceDefinition* d, ostream* w)
{
    ostream& w2 = *w;

    bool hasconstants = false;

    for (vector<string>::iterator e = d->Options.begin(); e != d->Options.end(); ++e)
    {
        if (boost::starts_with(*e, "constant"))
            hasconstants = true;
    }

    if (!d->Enums.empty() || !d->Constants.empty())
        hasconstants = true;

    vector<boost::shared_ptr<ServiceEntryDefinition> > entries;
    boost::copy(d->NamedArrays, std::back_inserter(entries));
    boost::copy(d->Pods, std::back_inserter(entries));
    boost::copy(d->Structures, std::back_inserter(entries));
    boost::copy(d->Objects, std::back_inserter(entries));

    for (vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator ee = entries.begin(); ee != entries.end(); ++ee)
    {
        for (vector<string>::iterator e = (*ee)->Options.begin(); e != (*ee)->Options.end(); ++e)
        {
            if (boost::starts_with(*e, "constant"))
                hasconstants = true;
        }

        if (!(*ee)->Constants.empty())
            hasconstants = true;
    }

    if (!hasconstants)
        return;

    w2 << "public static class " << boost::replace_all_copy(fix_name(d->Name), ".", "__") << "Constants " << endl
       << "{" << endl;

    for (vector<string>::iterator e = d->Options.begin(); e != d->Options.end(); ++e)
    {
        if (boost::starts_with(*e, "constant"))
        {
            RR_SHARED_PTR<ServiceDefinition> def2(d, null_deleter<ServiceDefinition>);
            RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(def2);
            c->FromString(*e);
            w2 << "    " << convert_constant(c.get(), d->Constants, d) << endl;
        }
    }

    BOOST_FOREACH (RR_SHARED_PTR<ConstantDefinition>& c, d->Constants)
    {
        GenerateDocString(c->DocString, "    ", w);
        w2 << "    " << convert_constant(c.get(), d->Constants, d) << endl;
    }

    for (vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator ee = entries.begin(); ee != entries.end(); ++ee)
    {
        bool objhasconstants = false;

        for (vector<string>::iterator e = (*ee)->Options.begin(); e != (*ee)->Options.end(); ++e)
        {
            if (boost::starts_with(*e, "constant"))
                objhasconstants = true;
        }

        if (!(*ee)->Constants.empty())
            objhasconstants = true;

        if (objhasconstants || !(*ee)->Constants.empty())
        {
            w2 << "    public static class " << fix_name((*ee)->Name) << endl << "    {" << endl;
            for (vector<string>::iterator e = (*ee)->Options.begin(); e != (*ee)->Options.end(); ++e)
            {
                if (boost::starts_with(*e, "constant"))
                {
                    RR_SHARED_PTR<ServiceDefinition> def2(d, null_deleter<ServiceDefinition>);
                    RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(def2);
                    c->FromString(*e);
                    w2 << "    " << convert_constant(c.get(), (*ee)->Constants, d) << endl;
                }
            }

            BOOST_FOREACH (RR_SHARED_PTR<ConstantDefinition>& c, (*ee)->Constants)
            {
                GenerateDocString(c->DocString, "    ", w);
                w2 << "    " << convert_constant(c.get(), (*ee)->Constants, d) << endl;
            }

            w2 << "    }" << endl;
        }
    }

    w2 << "}" << endl;

    BOOST_FOREACH (RR_SHARED_PTR<EnumDefinition>& e, d->Enums)
    {
        GenerateDocString(e->DocString, "    ", w);
        w2 << "    public enum " << fix_name(e->Name) << std::endl;
        w2 << "    {" << std::endl;
        for (size_t i = 0; i < e->Values.size(); i++)
        {
            EnumDefinitionValue& v = e->Values[i];
            GenerateDocString(v.DocString, "    ", w);
            if (!v.HexValue)
            {
                w2 << "    " << fix_name(v.Name) << " = " << v.Value;
            }
            else
            {
                if (v.Value >= 0)
                {
                    w2 << "    " << fix_name(v.Name) << " = 0x" << std::hex << v.Value << std::dec;
                }
                else
                {
                    w2 << "    " << fix_name(v.Name) << " = -0x" << std::hex << -v.Value << std::dec;
                }
            }
            if (i + 1 < e->Values.size())
            {
                w2 << "," << std::endl;
            }
            else
            {
                w2 << std::endl;
            }
        }
        w2 << "    };" << std::endl;
    }
}

void CSharpServiceLangGen::GenerateFiles(RR_SHARED_PTR<ServiceDefinition> d, std::string servicedef, std::string path)
{
#ifdef _WIN32
    const std::string os_pathsep("\\");
#else
    const std::string os_pathsep("/");
#endif

    ofstream f1((path + os_pathsep + boost::replace_all_copy(fix_name(d->Name), ".", "__") + ".cs").c_str());
    GenerateInterfaceFile(d.get(), &f1);
    f1.close();

    ofstream f2((path + os_pathsep + boost::replace_all_copy(fix_name(d->Name), ".", "__") + "_stubskel.cs").c_str());
    GenerateStubSkelFile(d.get(), servicedef, &f2);
    f2.close();
}

void CSharpServiceLangGen::GenerateOneFileHeader(ostream* w)
{
    ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << endl;
    w2 << "using System;" << endl;
    w2 << "using RobotRaconteur;" << endl;
    w2 << "using System.Collections.Generic;" << endl;
    w2 << "using System.Threading;" << endl;
    w2 << "using System.Threading.Tasks;" << endl << endl;
    w2 << "#pragma warning disable 0108" << endl << endl;
}

void CSharpServiceLangGen::GenerateOneFilePart(RR_SHARED_PTR<ServiceDefinition> d, std::string servicedef, ostream* w)
{
    GenerateInterfaceFile(d.get(), w, false);
    GenerateStubSkelFile(d.get(), servicedef, w, false);
}

std::string CSharpServiceLangGen::GetDefaultValue(const TypeDefinition& tdef)
{

    convert_type_result tt = convert_type(tdef);
    return "default(" + tt.cs_type + tt.cs_arr_type + ")";
}

std::string CSharpServiceLangGen::GetDefaultInitializedValue(const TypeDefinition& tdef)
{
    if (tdef.Type == DataTypes_void_t)
        throw InternalErrorException("Internal error");

    if (tdef.ContainerType == DataTypes_ContainerTypes_none)
    {
        if (IsTypeNumeric(tdef.Type))
        {
            switch (tdef.ArrayType)
            {
            case DataTypes_ArrayTypes_none: {
                return GetDefaultValue(tdef);
            }
            case DataTypes_ArrayTypes_array: {
                RR_SHARED_PTR<TypeDefinition> tdef2 = tdef.Clone();
                tdef2->RemoveContainers();
                tdef2->RemoveArray();
                convert_type_result t = convert_type(*tdef2);
                if (tdef.ArrayVarLength)
                {
                    return "new " + t.cs_type + "[0]";
                }
                else
                {
                    return "new " + t.cs_type + "[" + boost::lexical_cast<std::string>(tdef.ArrayLength.at(0)) + "]";
                }
            }
            case DataTypes_ArrayTypes_multidimarray: {
                RR_SHARED_PTR<TypeDefinition> tdef2 = tdef.Clone();
                tdef2->RemoveContainers();
                tdef2->RemoveArray();
                convert_type_result t = convert_type(*tdef2);
                if (tdef.ArrayVarLength)
                {
                    return "new MultiDimArray(new uint[] {1,0}, new " + t.cs_type + "[0])";
                }
                else
                {
                    int32_t n_elems = boost::accumulate(tdef.ArrayLength, 1, std::multiplies<int32_t>());
                    return "new MultiDimArray(new uint[] {" +
                           boost::join(tdef.ArrayLength |
                                           boost::adaptors::transformed(boost::lexical_cast<std::string, int32_t>),
                                       ",") +
                           "}, new " + t.cs_type + "[" + boost::lexical_cast<std::string>(n_elems) + "])";
                }
            }
            default:
                throw InvalidArgumentException("Invalid array type");
            }
        }
        if (tdef.Type == DataTypes_string_t)
        {
            return "\"\"";
        }
        if (tdef.Type == DataTypes_namedtype_t)
        {
            RR_SHARED_PTR<TypeDefinition> tdef2 = tdef.Clone();
            tdef2->RemoveContainers();
            tdef2->RemoveArray();

            if (tdef2->ResolveNamedType()->RRDataType() == DataTypes_pod_t)
            {
                switch (tdef.ArrayType)
                {
                case DataTypes_ArrayTypes_none: {
                    return GetDefaultValue(tdef);
                }
                case DataTypes_ArrayTypes_array: {
                    convert_type_result t = convert_type(*tdef2);
                    if (tdef.ArrayVarLength)
                    {
                        return "new " + t.cs_type + "[0]";
                    }
                    else
                    {
                        return "new " + t.cs_type + "[" + boost::lexical_cast<std::string>(tdef.ArrayLength.at(0)) +
                               "]";
                    }
                }
                case DataTypes_ArrayTypes_multidimarray: {
                    convert_type_result t = convert_type(*tdef2);
                    if (tdef.ArrayVarLength)
                    {
                        return "new PodMultiDimArray(new uint[] {1,0}, new " + t.cs_type + "[0])";
                    }
                    else
                    {
                        int32_t n_elems = boost::accumulate(tdef.ArrayLength, 1, std::multiplies<int32_t>());
                        return "new PodMultiDimArray(new uint[] {" +
                               boost::join(tdef.ArrayLength |
                                               boost::adaptors::transformed(boost::lexical_cast<std::string, int32_t>),
                                           ",") +
                               "}, new " + t.cs_type + "[" + boost::lexical_cast<std::string>(n_elems) + "])";
                    }
                }
                default:
                    throw InvalidArgumentException("Invalid array type");
                }
            }

            if (tdef2->ResolveNamedType()->RRDataType() == DataTypes_namedarray_t)
            {
                switch (tdef.ArrayType)
                {
                case DataTypes_ArrayTypes_none: {
                    return GetDefaultValue(tdef);
                }
                case DataTypes_ArrayTypes_array: {
                    convert_type_result t = convert_type(*tdef2);
                    if (tdef.ArrayVarLength)
                    {
                        return "new " + t.cs_type + "[0]";
                    }
                    else
                    {
                        return "new " + t.cs_type + "[" + boost::lexical_cast<std::string>(tdef.ArrayLength.at(0)) +
                               "]";
                    }
                }
                case DataTypes_ArrayTypes_multidimarray: {
                    convert_type_result t = convert_type(*tdef2);
                    if (tdef.ArrayVarLength)
                    {
                        return "new NamedMultiDimArray(new uint[] {1,0}, new " + t.cs_type + "[0])";
                    }
                    else
                    {
                        int32_t n_elems = boost::accumulate(tdef.ArrayLength, 1, std::multiplies<int32_t>());
                        return "new NamedMultiDimArray(new uint[] {" +
                               boost::join(tdef.ArrayLength |
                                               boost::adaptors::transformed(boost::lexical_cast<std::string, int32_t>),
                                           ",") +
                               "}, new " + t.cs_type + "[" + boost::lexical_cast<std::string>(n_elems) + "])";
                    }
                }
                default:
                    throw InvalidArgumentException("Invalid array type");
                }
            }
        }
    }

    return GetDefaultValue(tdef);
}

void CSharpServiceLangGen::GenerateDocString(const std::string& docstring, const std::string& prefix, ostream* w)
{
    if (docstring.empty())
    {
        return;
    }

    ostream& w2 = *w;

    std::vector<std::string> docstring_v;
    boost::split(docstring_v, docstring, boost::is_any_of("\n"));
    w2 << prefix << "/// <summary>" << endl;
    BOOST_FOREACH (const std::string& s, docstring_v)
    {
        w2 << prefix << "/// " << s << endl;
    }
    w2 << prefix << "/// </summary>" << endl;
}

}
