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

#include "CPPServiceLangGen.h"

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <set>
#include <fstream>
#include <boost/locale.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptor/transformed.hpp>

using namespace RobotRaconteur;

// NOLINTBEGIN(bugprone-macro-parentheses)
#define MEMBER_ITER(TYPE)                                                                                              \
    {                                                                                                                  \
        for (std::vector<RR_SHARED_PTR<MemberDefinition> >::const_iterator m1 = (*e)->Members.begin();                 \
             m1 != (*e)->Members.end(); ++m1)                                                                          \
            if (dynamic_cast<TYPE*>(m1->get()) != 0)                                                                   \
            {                                                                                                          \
                RR_SHARED_PTR<TYPE> m = boost::dynamic_pointer_cast<TYPE>(*m1);
#define MEMBER_ITER_END()                                                                                              \
    }                                                                                                                  \
    }

#define MEMBER_ITER2(TYPE)                                                                                             \
    {                                                                                                                  \
        for (std::vector<RR_SHARED_PTR<MemberDefinition> >::const_iterator m1 = (*e)->Members.begin();                 \
             m1 != (*e)->Members.end(); ++m1)                                                                          \
            if (dynamic_cast<TYPE*>(m1->get()) != 0 && !is_member_override(m1->get(), other_defs))                     \
            {                                                                                                          \
                RR_SHARED_PTR<TYPE> m = boost::dynamic_pointer_cast<TYPE>(*m1);
#define MEMBER_ITER2_END()                                                                                             \
    }                                                                                                                  \
    }

#define MEMBER_ITER3(TYPE)                                                                                             \
    {                                                                                                                  \
        for (std::vector<RR_SHARED_PTR<MemberDefinition> >::const_iterator m1 = (*e)->Members.begin();                 \
             m1 != (*e)->Members.end(); ++m1)                                                                          \
            if (dynamic_cast<TYPE*>(m1->get()) != 0 && !(is_member_override(m1->get(), other_defs) && is_abstract))    \
            {                                                                                                          \
                RR_SHARED_PTR<TYPE> m = boost::dynamic_pointer_cast<TYPE>(*m1);
#define MEMBER_ITER3_END()                                                                                             \
    }                                                                                                                  \
    }

#define OBJREF_ARRAY_CONTAINER_CMD(d, scalar_cmd, array_cmd, map_int32_cmd, map_string_cmd)                            \
    switch ((d)->ArrayType)                                                                                            \
    {                                                                                                                  \
    case DataTypes_ArrayTypes_none: {                                                                                  \
        switch ((d)->ContainerType)                                                                                    \
        {                                                                                                              \
        case DataTypes_ContainerTypes_none: {                                                                          \
            scalar_cmd;                                                                                                \
        }                                                                                                              \
        break;                                                                                                         \
        case DataTypes_ContainerTypes_map_int32: {                                                                     \
            map_int32_cmd;                                                                                             \
        }                                                                                                              \
        break;                                                                                                         \
        case DataTypes_ContainerTypes_map_string: {                                                                    \
            map_string_cmd;                                                                                            \
        }                                                                                                              \
        break;                                                                                                         \
        default:                                                                                                       \
            throw DataTypeException("Unknown object container type");                                                  \
        }                                                                                                              \
        break;                                                                                                         \
    }                                                                                                                  \
    case DataTypes_ArrayTypes_array: {                                                                                 \
        if ((d)->ContainerType != DataTypes_ContainerTypes_none)                                                       \
        {                                                                                                              \
            throw DataTypeException("Invalid object container type");                                                  \
        }                                                                                                              \
        {                                                                                                              \
            array_cmd;                                                                                                 \
        }                                                                                                              \
        break;                                                                                                         \
    }                                                                                                                  \
    default:                                                                                                           \
        throw DataTypeException("Invalid object array type");                                                          \
    }
// NOLINTEND(bugprone-macro-parentheses)

namespace RobotRaconteurGen
{

std::string CPPServiceLangGen::fix_name(const std::string& name)
{
    if (name.find('.') != std::string::npos)
    {
        std::vector<std::string> s1;
        std::vector<std::string> s2;
        boost::split(s1, name, boost::is_from_range('.', '.'));
        for (std::vector<std::string>::iterator e = s1.begin(); e != s1.end(); e++)
        {
            s2.push_back(fix_name(*e));
        }

        return boost::join(s2, ".");
    }

    // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
    const char* res_str[] = {
        "alignas",   "alignof",  "and",      "and_eq",    "asm",        "auto",          "bitand",
        "bitor",     "bool",     "break",    "case",      "catch",      "char",          "char16_t",
        "char32_t",  "class",    "compl",    "const",     "constnexpr", "const_cast",    "continue",
        "decltype",  "default",  "delete",   "do",        "double",     "dynamic_cast",  "else",
        "enum",      "explicit", "export",   "extern",    "false",      "float",         "for",
        "friend",    "goto",     "if",       "inline",    "int",        "long",          "mutable",
        "namespace", "new",      "noexcept", "not",       "noteq",      "nullptr",       "operator",
        "or",        "or_eq",    "private",  "protected", "public",     "register",      "reinterpret_cast",
        "return",    "short",    "signed",   "sizeof",    "static",     "static_assert", "static_cast",
        "struct",    "switch",   "switch",   "template",  "this",       "thread_local",  "throw",
        "true",      "try",      "typedef",  "typeid",    "typename",   "union",         "unsigned",
        "using",     "virtual",  "void",     "volatile",  "wchar_t",    "while",         "xor",
        "xor_eq",    "override", "final",    "uint8_t",   "int8_t",     "int16_t",       "uint16_t",
        "int32_t",   "uint32_t", "int64_t",  "uint64_t",  "pascal"};
    // NOLINTEND(cppcoreguidelines-avoid-c-arrays)

    std::vector<std::string> reserved(res_str, res_str + sizeof(res_str) / (sizeof(res_str[0])));

    if (std::find(reserved.begin(), reserved.end(), name) != reserved.end())
    {
        std::cout << "warning: name " << name << " replaced by " << name << "_ due to keyword conflict" << std::endl;
        return name + "_";
    }

    return name;
}

std::string CPPServiceLangGen::fix_qualified_name(const std::string& name)
{
    std::vector<std::string> v;
    boost::split(v, name, boost::is_from_range('.', '.'));

    if (v.empty())
        throw InternalErrorException("Internal error");

    std::vector<std::string> vret;

    for (std::vector<std::string>::iterator e = v.begin(); e != v.end(); e++)
    {
        vret.push_back(fix_name(*e));
    }

    return boost::join(vret, "::");
}

std::string CPPServiceLangGen::export_definition(ServiceDefinition* def)
{
    return "ROBOTRACONTEUR_ROBDEF_THUNK____" + fix_name(boost::replace_all_copy(def->Name, ".", "__")) + "____EXPORT";
}

CPPServiceLangGen::convert_type_result CPPServiceLangGen::convert_type(const TypeDefinition& tdef)
{
    convert_type_result o;
    DataTypes t = tdef.Type;
    o.name = fix_name(tdef.Name);
    // o[2]=tdef.IsArray ? "*" : "";

    switch (t)
    {

    case DataTypes_double_t:
        o.cpp_type = "double";
        break;
    case DataTypes_single_t:
        o.cpp_type = "float";
        break;
    case DataTypes_int8_t:
        o.cpp_type = "int8_t";
        break;
    case DataTypes_uint8_t:
        o.cpp_type = "uint8_t";
        break;
    case DataTypes_int16_t:
        o.cpp_type = "int16_t";
        break;
    case DataTypes_uint16_t:
        o.cpp_type = "uint16_t";
        break;
    case DataTypes_int32_t:
        o.cpp_type = "int32_t";
        break;
    case DataTypes_uint32_t:
        o.cpp_type = "uint32_t";
        break;
    case DataTypes_int64_t:
        o.cpp_type = "int64_t";
        break;
    case DataTypes_uint64_t:
        o.cpp_type = "uint64_t";
        break;
    case DataTypes_string_t:
        o.cpp_type = "char";
        break;
    case DataTypes_cdouble_t:
        o.cpp_type = "RobotRaconteur::cdouble";
        break;
    case DataTypes_csingle_t:
        o.cpp_type = "RobotRaconteur::cfloat";
        break;
    case DataTypes_bool_t:
        o.cpp_type = "RobotRaconteur::rr_bool";
        break;
    case DataTypes_namedtype_t:
    case DataTypes_object_t:
        o.cpp_type = fix_qualified_name(tdef.TypeString);
        break;
    case DataTypes_varvalue_t:
        o.cpp_type = "RRValue";
        break;
    default:
        throw InvalidArgumentException("");
    }

    return o;
}

std::string CPPServiceLangGen::remove_RR_INTRUSIVE_PTR(const std::string& vartype1)
{
    std::string vartype = boost::trim_copy(vartype1);
    if (!boost::starts_with(vartype, "RR_INTRUSIVE_PTR"))
        return vartype;

    std::string b = "RR_INTRUSIVE_PTR<";
    int32_t start = boost::numeric_cast<int32_t>(b.length());

    std::string ret = vartype.substr(start);
    ret = ret.substr(0, ret.length() - 1);
    return ret;
}

CPPServiceLangGen::get_variable_type_result CPPServiceLangGen::get_variable_type(const TypeDefinition& tdef,
                                                                                 bool usescalar)
{

    if (tdef.Type == DataTypes_void_t)
    {
        get_variable_type_result o;
        o.name = fix_name(tdef.Name);
        o.cpp_type = "void";
        o.cpp_param_type = "void";
        return o;
    }

    if (tdef.ContainerType != DataTypes_ContainerTypes_none)
    {
        TypeDefinition tdef2;
        tdef.CopyTo(tdef2);
        tdef2.RemoveContainers();

        get_variable_type_result s1 = get_variable_type(tdef2);
        get_variable_type_result s2 = get_variable_type(tdef2, false);

        switch (tdef.ContainerType)
        {
        case DataTypes_ContainerTypes_list: {
            get_variable_type_result o;
            o.name = fix_name(tdef.Name);
            o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRList<" + remove_RR_INTRUSIVE_PTR(s2.cpp_type) + " > >";
            o.cpp_param_type = "const " + o.cpp_type + "&";
            return o;
        }
        case DataTypes_ContainerTypes_map_int32: {
            get_variable_type_result o;
            o.name = fix_name(tdef.Name);
            o.cpp_type =
                "RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t," + remove_RR_INTRUSIVE_PTR(s2.cpp_type) + " > >";
            o.cpp_param_type = "const " + o.cpp_type + "&";
            return o;
        }
        case DataTypes_ContainerTypes_map_string: {
            get_variable_type_result o;
            o.name = fix_name(tdef.Name);
            o.cpp_type =
                "RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string," + remove_RR_INTRUSIVE_PTR(s2.cpp_type) + " > >";
            o.cpp_param_type = "const " + o.cpp_type + "&";
            return o;
        }
        default:
            throw DataTypeException("Invalid container type");
        }
    }

    if (IsTypeNumeric(tdef.Type))
    {
        convert_type_result c = convert_type(tdef);

        switch (tdef.ArrayType)
        {
        case DataTypes_ArrayTypes_none: {
            if (usescalar)
            {
                get_variable_type_result o;
                o.name = c.name;
                o.cpp_type = c.cpp_type;
                o.cpp_param_type = o.cpp_type;
                return o;
            }
        }
        case DataTypes_ArrayTypes_array: {
            get_variable_type_result o;
            o.name = c.name;
            o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<" + c.cpp_type + " > >";
            o.cpp_param_type = "const " + o.cpp_type + "&";
            return o;
        }
        case DataTypes_ArrayTypes_multidimarray: {
            get_variable_type_result o;
            o.name = c.name;
            o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<" + c.cpp_type + " > >";
            o.cpp_param_type = "const " + o.cpp_type + "&";
            return o;
        }
        default:
            throw DataTypeException("Invalid numeric type type");
        }
    }
    else if (tdef.Type == DataTypes_string_t)
    {
        convert_type_result c = convert_type(tdef);
        if (usescalar)
        {
            get_variable_type_result o;
            o.name = c.name;
            o.cpp_type = "std::string";
            o.cpp_param_type = "const std::string&";
            return o;
        }
        else
        {
            get_variable_type_result o;
            o.name = c.name;
            o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<char> >";
            o.cpp_param_type = "const " + o.cpp_type + "&";
            return o;
        }
    }
    else if (tdef.Type == DataTypes_namedtype_t)
    {
        get_variable_type_result o;
        RR_SHARED_PTR<NamedTypeDefinition> nt = tdef.ResolveNamedType_cache.lock();
        if (!nt)
            throw DataTypeException("Data type not resolved");
        switch (nt->RRDataType())
        {
        case DataTypes_structure_t:
            o.name = fix_name(tdef.Name);
            o.cpp_type = "RR_INTRUSIVE_PTR<" + fix_qualified_name(tdef.TypeString) + " >";
            o.cpp_param_type = "const " + o.cpp_type + "&";
            break;
        case DataTypes_pod_t:
        case DataTypes_namedarray_t: {

            std::string a = nt->RRDataType() == DataTypes_pod_t ? "Pod" : "Named";

            switch (tdef.ArrayType)
            {
            case DataTypes_ArrayTypes_none:
                if (usescalar)
                {
                    o.name = fix_name(tdef.Name);
                    o.cpp_type = fix_qualified_name(tdef.TypeString);
                    o.cpp_param_type = "const " + o.cpp_type + "&";
                }
                else
                {
                    o.name = fix_name(tdef.Name);
                    o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RR" + a + "Array<" +
                                 fix_qualified_name(tdef.TypeString) + "> >";
                    o.cpp_param_type = "const " + o.cpp_type + "&";
                }
                break;
            case DataTypes_ArrayTypes_array:
                o.name = fix_name(tdef.Name);
                o.cpp_type =
                    "RR_INTRUSIVE_PTR<RobotRaconteur::RR" + a + "Array<" + fix_qualified_name(tdef.TypeString) + "> >";
                o.cpp_param_type = "const " + o.cpp_type + "&";
                break;
            case DataTypes_ArrayTypes_multidimarray:
                o.name = fix_name(tdef.Name);
                o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RR" + a + "MultiDimArray<" +
                             fix_qualified_name(tdef.TypeString) + "> >";
                o.cpp_param_type = "const " + o.cpp_type + "&";
                break;
            default:
                throw InternalErrorException("Invalid namedarray or pod type");
            }
            break;
        }
        case DataTypes_enum_t:
            o.name = fix_name(tdef.Name);
            if (!boost::contains(tdef.TypeString, "."))
            {
                o.cpp_type = fix_name(tdef.TypeString) + "::" + fix_name(tdef.TypeString);
                o.cpp_param_type = o.cpp_type;
            }
            else
            {
                boost::string_ref enum_type_name;
                boost::tie(boost::tuples::ignore, enum_type_name) = SplitQualifiedName(tdef.TypeString);
                o.cpp_type = fix_qualified_name(tdef.TypeString) + "::" + fix_name(enum_type_name.to_string());
                o.cpp_param_type = o.cpp_type;
            }
            if (!usescalar)
            {
                o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> >";
                o.cpp_param_type = "const " + o.cpp_type + "&";
            }

            break;
        default:
            throw DataTypeException("Unknown named type id");
        }
        return o;
    }
    else if (tdef.Type == DataTypes_multidimarray_t)
    {
        convert_type_result c = convert_type(tdef);

        get_variable_type_result o;
        o.name = c.name;
        o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<" + c.cpp_type + " > >";
        o.cpp_param_type = "const " + o.cpp_type + "&";
        return o;
    }
    else if (tdef.Type == DataTypes_varvalue_t)
    {
        get_variable_type_result o;
        o.name = fix_name(tdef.Name);
        o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>";
        o.cpp_param_type = "const " + o.cpp_type + "&";
        return o;
    }

    throw DataTypeException("Unknown data type");
}

std::string CPPServiceLangGen::str_pack_parameters(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l, bool inclass)
{
    std::vector<std::string> o(l.size());

    for (size_t i = 0; i < o.size(); i++)
    {
        get_variable_type_result t = get_variable_type(*l[i]);
        if (inclass)
            o[i] = t.cpp_param_type + " " + t.name;
        else
            o[i] = t.name;
    }

    return boost::join(o, ", ");
}

// Code to pack and unpack message elements

static std::string CPPServiceLangGen_VerifyArrayLength(TypeDefinition& t, const std::string& varname)
{
    if (t.ArrayType == DataTypes_ArrayTypes_array && t.ArrayLength.at(0) != 0)
    {
        return "RobotRaconteur::VerifyRRArrayLength(" + varname + ", " +
               boost::lexical_cast<std::string>(t.ArrayLength.at(0)) + ", " + (t.ArrayVarLength ? "true" : "false") +
               ")";
    }
    if (t.ArrayType == DataTypes_ArrayTypes_multidimarray && !t.ArrayLength.empty() && !t.ArrayVarLength)
    {
        int32_t n_elems = boost::accumulate(t.ArrayLength, 1, std::multiplies<int32_t>());
        return "RobotRaconteur::VerifyRRMultiDimArrayLength<" + boost::lexical_cast<std::string>(t.ArrayLength.size()) +
               ">(" + varname + "," + boost::lexical_cast<std::string>(n_elems) + ",boost::assign::list_of(" +
               boost::join(t.ArrayLength | boost::adaptors::transformed(boost::lexical_cast<std::string, int32_t>),
                           ")(") +
               "))";
    }
    return varname;
}

static bool CPPServiceLangGen_UseVerifyArrayLength(TypeDefinition& t)
{
    if (t.ArrayType == DataTypes_ArrayTypes_array && t.ArrayLength.at(0) != 0)
    {
        return true;
    }
    if (t.ArrayType == DataTypes_ArrayTypes_multidimarray && !t.ArrayLength.empty() && !t.ArrayVarLength)
    {
        return true;
    }
    return false;
}

static std::string pure_virtual(const std::string& definition)
{
    std::string res1_1 = boost::replace_first_copy(definition, "RR_OVIRTUAL", "virtual");
    return boost::trim_copy(boost::replace_last_copy(res1_1, "RR_OVERRIDE", ""));
}

std::string CPPServiceLangGen::str_pack_message_element(const std::string& elementname, const std::string& varname,
                                                        const RR_SHARED_PTR<TypeDefinition>& t,
                                                        const std::string& packer)
{
    RR_UNUSED(packer);
    TypeDefinition t1;
    t->CopyTo(t1);
    t1.RemoveContainers();
    get_variable_type_result tt = get_variable_type(t1, false);

    switch (t->ContainerType)
    {
    case DataTypes_ContainerTypes_none: {
        if (IsTypeNumeric(t->Type))
        {
            switch (t->ArrayType)
            {
            case DataTypes_ArrayTypes_none: {
                convert_type_result ts = convert_type(*t);
                return "RobotRaconteur::MessageElement_PackScalarElement<" + ts.cpp_type + " >(\"" + elementname +
                       "\"," + varname + ")";
                break;
            }
            case DataTypes_ArrayTypes_array: {
                convert_type_result ts = convert_type(*t);
                return "RobotRaconteur::MessageElement_PackArrayElement<" + ts.cpp_type + " >(\"" + elementname +
                       "\"," + CPPServiceLangGen_VerifyArrayLength(*t, varname) + ")";

                break;
            }
            case DataTypes_ArrayTypes_multidimarray: {
                convert_type_result ts = convert_type(*t);
                return "RobotRaconteur::MessageElement_PackMultiDimArrayElement<" + ts.cpp_type +
                       " >(RRGetNodeWeak(),\"" + elementname + "\"," +
                       CPPServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                break;
            }
            default:
                throw DataTypeException("Invalid array type");
            }
        }
        else if (t->Type == DataTypes_string_t)
        {
            return "RobotRaconteur::MessageElement_PackStringElement(\"" + elementname + "\"," + varname + ")";
        }
        else if (t->Type == DataTypes_varvalue_t)
        {
            return "RobotRaconteur::MessageElement_PackVarTypeElement(RRGetNodeWeak(),\"" + elementname + "\"," +
                   varname + ")";
        }
        else if (t->Type == DataTypes_namedtype_t)
        {
            RR_SHARED_PTR<NamedTypeDefinition> nt = t->ResolveNamedType();
            switch (nt->RRDataType())
            {
            case DataTypes_structure_t:
                return "RobotRaconteur::MessageElement_PackStructElement(RRGetNodeWeak(),\"" + elementname + "\"," +
                       varname + ")";
                break;
            case DataTypes_enum_t:
                return "RobotRaconteur::MessageElement_PackEnumElement(\"" + elementname + "\"," + varname + ")";
                break;
            case DataTypes_pod_t:
            case DataTypes_namedarray_t: {
                std::string a = nt->RRDataType() == DataTypes_pod_t ? "Pod" : "Named";
                switch (t->ArrayType)
                {
                case DataTypes_ArrayTypes_none:
                    if (a == "Named")
                    {
                        return "RobotRaconteur::MessageElement_Pack" + a + "ArrayToArrayElement(\"" + elementname +
                               "\"," + varname + ")";
                    }
                    else
                    {
                        return "RobotRaconteur::MessageElement_Pack" + a + "ToArrayElement(\"" + elementname + "\"," +
                               varname + ")";
                    }
                    break;
                case DataTypes_ArrayTypes_array:
                    return "RobotRaconteur::MessageElement_Pack" + a + "ArrayElement(\"" + elementname + "\"," +
                           CPPServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                    break;
                case DataTypes_ArrayTypes_multidimarray:
                    return "RobotRaconteur::MessageElement_Pack" + a + "MultiDimArrayElement(\"" + elementname + "\"," +
                           CPPServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                    break;
                default:
                    throw InternalErrorException("Invalid pod type");
                }
                break;
            }
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
        return "RobotRaconteur::MessageElement_PackListElement<" + remove_RR_INTRUSIVE_PTR(tt.cpp_type) +
               " >(RRGetNodeWeak(),\"" + elementname + "\"," + CPPServiceLangGen_VerifyArrayLength(*t, varname) + ")";
    case DataTypes_ContainerTypes_map_int32:
        return "RobotRaconteur::MessageElement_PackMapElement<int32_t," + remove_RR_INTRUSIVE_PTR(tt.cpp_type) +
               " >(RRGetNodeWeak(),\"" + elementname + "\"," + CPPServiceLangGen_VerifyArrayLength(*t, varname) + ")";
    case DataTypes_ContainerTypes_map_string:
        return "RobotRaconteur::MessageElement_PackMapElement<std::string," + remove_RR_INTRUSIVE_PTR(tt.cpp_type) +
               " >(RRGetNodeWeak(),\"" + elementname + "\"," + CPPServiceLangGen_VerifyArrayLength(*t, varname) + ")";
    default:
        throw DataTypeException("Invalid container type");
    }
}

std::string CPPServiceLangGen::str_unpack_message_element(const std::string& varname,
                                                          const RR_SHARED_PTR<TypeDefinition>& t,
                                                          const std::string& packer)
{
    RR_UNUSED(packer);
    TypeDefinition t1;
    t->CopyTo(t1);
    t1.RemoveContainers();

    convert_type_result tt = convert_type(t1);
    std::string structunpackstring;

    get_variable_type_result tt1 = get_variable_type(t1, false);

    if (IsTypeNumeric(t->Type))
    {
        switch (t->ArrayType)
        {
        case DataTypes_ArrayTypes_none: {
            structunpackstring = "RobotRaconteur::MessageElement_UnpackScalar<" + tt.cpp_type + " >(" + varname + ")";
            break;
        }
        case DataTypes_ArrayTypes_array: {
            structunpackstring = CPPServiceLangGen_VerifyArrayLength(*t, "RobotRaconteur::MessageElement_UnpackArray<" +
                                                                             tt.cpp_type + " >(" + varname + ")");
            break;
        }
        case DataTypes_ArrayTypes_multidimarray: {
            structunpackstring =
                CPPServiceLangGen_VerifyArrayLength(*t, "RobotRaconteur::MessageElement_UnpackMultiDimArray<" +
                                                            tt.cpp_type + " >(RRGetNodeWeak()," + varname + ")");
            break;
        }
        default:
            throw DataTypeException("Invalid array type");
        }
    }
    else if (t->Type == DataTypes_string_t)
    {
        structunpackstring = "RobotRaconteur::MessageElement_UnpackString(" + varname + ")";
    }
    else if (t->Type == DataTypes_namedtype_t)
    {
        RR_SHARED_PTR<NamedTypeDefinition> nt = t->ResolveNamedType();
        switch (nt->RRDataType())
        {
        case DataTypes_structure_t:
            structunpackstring = "RobotRaconteur::MessageElement_UnpackStructure<" + fix_qualified_name(tt.cpp_type) +
                                 " >(RRGetNodeWeak(), " + varname + ")";
            break;
        case DataTypes_enum_t: {
            get_variable_type_result tt2 = get_variable_type(t1, true);
            structunpackstring =
                "RobotRaconteur::MessageElement_UnpackEnum<" + fix_qualified_name(tt2.cpp_type) + ">(" + varname + ")";
            break;
        }
        case DataTypes_pod_t:
        case DataTypes_namedarray_t: {
            std::string a = nt->RRDataType() == DataTypes_pod_t ? "Pod" : "Named";
            switch (t->ArrayType)
            {
            case DataTypes_ArrayTypes_none:
                if (a == "Named")
                {
                    structunpackstring = "RobotRaconteur::MessageElement_Unpack" + a + "ArrayFromArray<" +
                                         fix_qualified_name(tt.cpp_type) + ">(" + varname + ")";
                }
                else
                {
                    structunpackstring = "RobotRaconteur::MessageElement_Unpack" + a + "FromArray<" +
                                         fix_qualified_name(tt.cpp_type) + ">(" + varname + ")";
                }
                break;
            case DataTypes_ArrayTypes_array:
                structunpackstring =
                    CPPServiceLangGen_VerifyArrayLength(*t, "RobotRaconteur::MessageElement_Unpack" + a + "Array<" +
                                                                fix_qualified_name(tt.cpp_type) + ">(" + varname + ")");
                break;
            case DataTypes_ArrayTypes_multidimarray:
                structunpackstring = CPPServiceLangGen_VerifyArrayLength(
                    *t, "RobotRaconteur::MessageElement_Unpack" + a + "MultiDimArray<" +
                            fix_qualified_name(tt.cpp_type) + ">(" + varname + ")");
                break;
            default:
                throw InternalErrorException("Invalid pod type");
            }
            break;
        }
        default:
            throw DataTypeException("Unknown named type id");
        }
    }
    else if (t->Type == DataTypes_varvalue_t)
    {
        structunpackstring = "RobotRaconteur::MessageElement_UnpackVarValue(RRGetNodeWeak()," + varname + ")";
    }
    else
    {
        throw DataTypeException("Unknown type");
    }

    switch (t->ContainerType)
    {
    case DataTypes_ContainerTypes_none:
        return structunpackstring;
    case DataTypes_ContainerTypes_list:
        return CPPServiceLangGen_VerifyArrayLength(*t, "RobotRaconteur::MessageElement_UnpackList<" +
                                                           remove_RR_INTRUSIVE_PTR(tt1.cpp_type) +
                                                           " >(RRGetNodeWeak()," + varname + ")");
    case DataTypes_ContainerTypes_map_int32:
        return CPPServiceLangGen_VerifyArrayLength(*t, "RobotRaconteur::MessageElement_UnpackMap<int32_t," +
                                                           remove_RR_INTRUSIVE_PTR(tt1.cpp_type) +
                                                           " >(RRGetNodeWeak()," + varname + ")");
    case DataTypes_ContainerTypes_map_string:
        return CPPServiceLangGen_VerifyArrayLength(*t, "RobotRaconteur::MessageElement_UnpackMap<std::string," +
                                                           remove_RR_INTRUSIVE_PTR(tt1.cpp_type) +
                                                           " >(RRGetNodeWeak()," + varname + ")");
    default:
        throw DataTypeException("Invalid container type");
    }
}

// Code to generate the various types of generators

std::string CPPServiceLangGen::GetPropertyDeclaration(PropertyDefinition* d, bool inclass)
{
    get_variable_type_result t = get_variable_type(*d->Type);
    t.name = fix_name(d->Name);
    if (inclass)
    {
        return "RR_OVIRTUAL " + t.cpp_type + " get_" + t.name + "() RR_OVERRIDE";
    }
    else
    {
        return "get_" + t.name + "()";
    }
}

std::string CPPServiceLangGen::SetPropertyDeclaration(PropertyDefinition* d, bool inclass)
{
    get_variable_type_result t = get_variable_type(*d->Type);
    t.name = fix_name(d->Name);
    if (inclass)
    {
        return "RR_OVIRTUAL void set_" + t.name + "(" + t.cpp_param_type + " value) RR_OVERRIDE";
    }
    else
    {
        return "set_" + t.name + "(value)";
    }
}

std::string CPPServiceLangGen::FunctionDeclaration(FunctionDefinition* d, bool inclass)
{
    std::string ret_type;
    std::string name = fix_name(d->Name);
    std::string param_str;

    if (!d->IsGenerator())
    {
        get_variable_type_result t = get_variable_type(*d->ReturnType);
        ret_type = t.cpp_type;
        param_str = str_pack_parameters(d->Parameters, inclass);
    }
    else
    {
        convert_generator_result t = convert_generator(d);
        ret_type = t.generator_cpp_type;
        param_str = str_pack_parameters(t.params, inclass);
    }

    if (inclass)
        return "RR_OVIRTUAL " + ret_type + " " + name + "(" + param_str + ") RR_OVERRIDE";
    else
        return name + "(" + param_str + ")";
}

// Async properties and functions

std::string CPPServiceLangGen::GetPropertyDeclaration_async(PropertyDefinition* d, bool inclass)
{
    get_variable_type_result t = get_variable_type(*d->Type);
    t.name = fix_name(d->Name);
    if (inclass)
    {
        return "RR_OVIRTUAL void async_get_" + t.name + "(boost::function<void (" + t.cpp_param_type +
               ",const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t "
               "rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE";
    }
    else
    {
        return "async_get_" + t.name + "(handler)";
    }
}

std::string CPPServiceLangGen::SetPropertyDeclaration_async(PropertyDefinition* d, bool inclass)
{
    get_variable_type_result t = get_variable_type(*d->Type);
    t.name = fix_name(d->Name);
    if (inclass)
    {
        return "RR_OVIRTUAL void async_set_" + t.name + "(" + t.cpp_param_type +
               " value,boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > "
               "rr_handler, "
               "int32_t rr_timeout=RR_TIMEOUT_INFINITE) RR_OVERRIDE";
    }
    else
    {
        return "async_set_" + t.name + "(value,rr_handler)";
    }
}

std::string CPPServiceLangGen::FunctionDeclaration_async(FunctionDefinition* d, bool inclass)
{
    if (!d->IsGenerator())
    {
        get_variable_type_result t = get_variable_type(*d->ReturnType);

        t.name = fix_name(d->Name);

        std::string param_str = str_pack_parameters(d->Parameters, inclass);

        std::vector<std::string> s1;

        if (!d->Parameters.empty())
        {
            s1.push_back(param_str);
        }

        if (d->ReturnType->Type == DataTypes_void_t)
        {
            s1.push_back(
                "boost::function<void (const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, "
                "int32_t rr_timeout=RR_TIMEOUT_INFINITE");
        }
        else
        {
            s1.push_back("boost::function<void (" + t.cpp_param_type +
                         ", const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t "
                         "rr_timeout=RR_TIMEOUT_INFINITE");
        }

        std::string s2 = boost::join(s1, ",");

        if (inclass)
        {
            return "RR_OVIRTUAL void async_" + t.name + "(" + s2 + ") RR_OVERRIDE";
        }
        else
            return "async_" + t.name + "(" + param_str + ", rr_handler)";
    }
    else
    {
        std::vector<std::string> s1;
        convert_generator_result t = convert_generator(d);

        std::string param_str = str_pack_parameters(t.params, inclass);
        if (!t.params.empty())
        {
            s1.push_back(param_str);
        }

        s1.push_back("boost::function<void (" + t.generator_cpp_type +
                     ", const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > rr_handler, int32_t "
                     "rr_timeout=RR_TIMEOUT_INFINITE");

        std::string s2 = boost::join(s1, ",");

        std::string name = fix_name(d->Name);

        if (inclass)
        {
            return "RR_OVIRTUAL void async_" + name + "(" + s2 + ") RR_OVERRIDE";
        }
        else
            return "async_" + name + "(" + param_str + ", rr_handler)";
    }
}

// Rest of definitions

std::string CPPServiceLangGen::EventDeclaration(EventDefinition* d, bool inclass, bool var)
{
    std::vector<std::string> pm(d->Parameters.size());
    for (size_t i = 0; i < d->Parameters.size(); i++)
    {
        get_variable_type_result t = get_variable_type(*d->Parameters[i]);
        pm[i] = t.cpp_type;
    }

    std::string plist = boost::join(pm, ", ");

    if (inclass)
        if (!var)
            return ("RR_OVIRTUAL boost::signals2::signal<void (" + plist + ")>& get_" + fix_name(d->Name) +
                    "() RR_OVERRIDE");
        else
            return ("boost::signals2::signal<void (" + plist + ")> rrvar_" + d->Name);
    else
        return "get_" + fix_name(d->Name) + "()(" + str_pack_parameters(d->Parameters, false) + ")";
}

std::string CPPServiceLangGen::ObjRefDeclaration(ObjRefDefinition* d, bool inclass)
{
    std::string member_name = fix_name(d->Name);
    std::string object_type = fix_qualified_name(d->ObjectType);

    if (d->ObjectType == "varobject")
        object_type = "RobotRaconteur::RRObject";

    std::string indexer;

    OBJREF_ARRAY_CONTAINER_CMD(d, , indexer = inclass ? "int32_t ind" : "int32_t",
                               indexer = inclass ? "int32_t ind" : "int32_t",
                               indexer = inclass ? "const std::string& ind" : "std::string")

    if (inclass)
    {
        return "RR_OVIRTUAL RR_SHARED_PTR<" + object_type + " > get_" + member_name + "(" + indexer + ") RR_OVERRIDE";
    }
    else
    {
        return "get_" + member_name + "(" + indexer + ")";
    }
}

std::string CPPServiceLangGen::ObjRefDeclaration_async(ObjRefDefinition* d, bool inclass)
{
    std::string member_name = fix_name(d->Name);
    std::string object_type = fix_qualified_name(d->ObjectType);

    if (d->ObjectType == "varobject")
        object_type = "RobotRaconteur::RRObject";

    std::string indexer;

    OBJREF_ARRAY_CONTAINER_CMD(d, , indexer = inclass ? "int32_t ind" : "int32_t",
                               indexer = inclass ? "int32_t ind" : "int32_t",
                               indexer = inclass ? "const std::string& ind" : "std::string")

    std::string param;
    if (indexer.empty())
    {
        param = "boost::function<void(RR_SHARED_PTR<" + object_type +
                ">,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)> handler, int32_t "
                "timeout=RR_TIMEOUT_INFINITE";
    }
    else
    {
        param = indexer + ", boost::function<void(RR_SHARED_PTR<" + object_type +
                ">,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&)> handler, int32_t "
                "timeout=RR_TIMEOUT_INFINITE";
    }
    if (inclass)
    {
        return "RR_OVIRTUAL void async_get_" + member_name + "(" + param + ") RR_OVERRIDE";
    }
    else
    {
        return "async_get_" + member_name + "(" + param + ")";
    }
}

std::string CPPServiceLangGen::GetPipeDeclaration(PipeDefinition* d, bool inclass)
{
    get_variable_type_result t = get_variable_type(*d->Type);
    t.name = fix_name(d->Name);
    if (inclass)
    {
        return "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Pipe<" + t.cpp_type + " > > get_" + t.name + "() RR_OVERRIDE";
    }
    else
    {
        return "get_" + t.name + "()";
    }
}

std::string CPPServiceLangGen::SetPipeDeclaration(PipeDefinition* d, bool inclass)
{
    get_variable_type_result t = get_variable_type(*d->Type);
    t.name = fix_name(d->Name);
    if (inclass)
    {
        return "RR_OVIRTUAL void set_" + t.name + "(const RR_SHARED_PTR<RobotRaconteur::Pipe<" + t.cpp_type +
               " > >& value) RR_OVERRIDE";
    }
    else
    {
        return "set_" + t.name + "(value)";
    }
}

std::string CPPServiceLangGen::GetCallbackDeclaration(CallbackDefinition* d, bool inclass, bool var)
{
    std::vector<std::string> pm(d->Parameters.size());
    for (size_t i = 0; i < d->Parameters.size(); i++)
    {
        get_variable_type_result t = get_variable_type(*d->Parameters[i]);
        pm[i] = t.cpp_type;
    }

    get_variable_type_result ret = get_variable_type(*d->ReturnType);

    std::string plist = boost::join(pm, ", ");
    if (inclass)
    {
        if (!var)
            return "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<" + ret.cpp_type + "(" + plist +
                   ")" + " > > > get_" + fix_name(d->Name) + "() RR_OVERRIDE";
        else
            return "boost::function<" + ret.cpp_type + "(" + plist + ")" + " >";
    }
    else
    {
        return "get_" + fix_name(d->Name) + "()";
    }
}

std::string CPPServiceLangGen::SetCallbackDeclaration(CallbackDefinition* d, bool inclass)
{
    std::vector<std::string> pm(d->Parameters.size());
    for (size_t i = 0; i < d->Parameters.size(); i++)
    {
        get_variable_type_result t = get_variable_type(*d->Parameters[i]);
        pm[i] = t.cpp_type;
    }

    get_variable_type_result ret = get_variable_type(*d->ReturnType);

    std::string plist = boost::join(pm, ", ");
    if (inclass)
    {
        return "RR_OVIRTUAL void set_" + fix_name(d->Name) +
               "(const RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<" + ret.cpp_type + "(" + plist +
               ")> > >& value) RR_OVERRIDE";
    }
    else
    {
        return "set_" + fix_name(d->Name) + "(value)";
    }
}

std::string CPPServiceLangGen::GetWireDeclaration(WireDefinition* d, bool inclass)
{
    get_variable_type_result t = get_variable_type(*d->Type);
    t.name = fix_name(d->Name);
    if (inclass)
    {
        return "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Wire<" + t.cpp_type + " > > get_" + t.name + "() RR_OVERRIDE";
    }
    else
    {
        return "get_" + t.name + "()";
    }
}

std::string CPPServiceLangGen::SetWireDeclaration(WireDefinition* d, bool inclass)
{
    get_variable_type_result t = get_variable_type(*d->Type);
    t.name = fix_name(d->Name);
    if (inclass)
    {
        return "RR_OVIRTUAL void set_" + t.name + "(const RR_SHARED_PTR<RobotRaconteur::Wire<" + t.cpp_type +
               " > >& value) RR_OVERRIDE";
    }
    else
    {
        return "set_" + t.name + "(value)";
    }
}

std::string CPPServiceLangGen::MemoryDeclaration(MemoryDefinition* d, bool inclass)
{
    convert_type_result t = convert_type(*d->Type);
    t.name = fix_name(d->Name);
    if (inclass)
    {
        if (IsTypeNumeric(d->Type->Type))
        {
            switch (d->Type->ArrayType)
            {
            case DataTypes_ArrayTypes_array:
                return "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ArrayMemory<" + t.cpp_type + " > > get_" + t.name +
                       "() RR_OVERRIDE";
            case DataTypes_ArrayTypes_multidimarray:
                return "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<" + t.cpp_type + " > > get_" +
                       t.name + "() RR_OVERRIDE";
            default:
                throw DataTypeException("Invalid memory definition");
            }
        }
        else
        {
            std::string c = "Pod";
            if (d->Type->ResolveNamedType()->RRDataType() == DataTypes_namedarray_t)
            {
                c = "Named";
            }
            switch (d->Type->ArrayType)
            {
            case DataTypes_ArrayTypes_array:
                return "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::" + c + "ArrayMemory<" + t.cpp_type + " > > get_" +
                       t.name + "() RR_OVERRIDE";
            case DataTypes_ArrayTypes_multidimarray:
                return "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::" + c + "MultiDimArrayMemory<" + t.cpp_type +
                       " > > get_" + t.name + "() RR_OVERRIDE";
            default:
                throw DataTypeException("Invalid memory definition");
            }
        }
    }
    else
    {
        return "get_" + t.name + "()";
    }
}

void CPPServiceLangGen::GenerateInterfaceHeaderFile(ServiceDefinition* d,
                                                    const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                                    const std::vector<std::string>& extra_include, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl << std::endl;

    if (!d->DocString.empty())
    {
        GenerateDocString(d->DocString, "", w);
        w2 << std::endl;
    }
    w2 << "#include <RobotRaconteur.h>" << std::endl;
    // w2 << "#include <memory>" << std::endl;
    // w2 << "#include <boost/shared_ptr.hpp>" << std::endl;
    // w2 << "#include <boost/make_shared.hpp>" << std::endl;
    // w2 << "#include <std::string>" << std::endl;
    w2 << "#include <boost/signals2.hpp>" << std::endl;
    w2 << "#include <boost/array.hpp>" << std::endl;
    w2 << "#include <boost/container/static_vector.hpp>" << std::endl;

    std::set<std::string> importedheaders;

    for (std::vector<std::string>::const_iterator e = d->Imports.begin(); e != d->Imports.end(); ++e)
    {
        importedheaders.insert(*e);
    }

    for (std::set<std::string>::const_iterator e = importedheaders.begin(); e != importedheaders.end(); ++e)
    {
        w2 << "#include \"" << boost::replace_all_copy(fix_name(*e), ".", "__") << ".h\"" << std::endl;
    }

    for (std::vector<std::string>::const_iterator e = extra_include.begin(); e != extra_include.end(); ++e)
    {
        w2 << "#include \"" << *e << "\"" << std::endl;
    }

    w2 << "#pragma once" << std::endl << std::endl;

    std::string export_macro = export_definition(d);

    w2 << "#ifndef " << export_macro << std::endl;
    w2 << "// NOLINTNEXTLINE" << std::endl;
    w2 << "#define " << export_macro << std::endl;
    w2 << "#endif" << std::endl << std::endl;

    w2 << "// NOLINTBEGIN" << std::endl;

    std::vector<std::string> namespace_vec;
    split(namespace_vec, d->Name, boost::is_from_range('.', '.'));

    for (std::vector<std::string>::iterator ns_e = namespace_vec.begin(); ns_e != namespace_vec.end(); ns_e++)
    {
        w2 << "namespace " << fix_name(*ns_e) << std::endl << "{" << std::endl;
    }

    w2 << std::endl;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        w2 << "class " << export_definition(d) << " " << fix_name((*e)->Name) << ";" << std::endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        w2 << "class " << export_definition(d) << " " << fix_name((*e)->Name) << ";" << std::endl;
    }

    w2 << std::endl;

    GenerateConstants(d, w);

    w2 << std::endl;

    std::map<std::string, RR_SHARED_PTR<ServiceEntryDefinition> > pods;
    BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& e, d->NamedArrays)
    {
        pods.insert(std::make_pair(e->Name, e));
    }

    BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& e, d->Pods)
    {
        pods.insert(std::make_pair(e->Name, e));
    }

    while (!pods.empty())
    {
        RR_SHARED_PTR<ServiceEntryDefinition> e2;
        BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& e, pods | boost::adaptors::map_values)
        {
            bool local_found = false;
            BOOST_FOREACH (const RR_SHARED_PTR<MemberDefinition>& m, e->Members)
            {
                RR_SHARED_PTR<PropertyDefinition> p = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(m);
                if (!p)
                    throw InternalErrorException("");

                if (p->Type->Type == DataTypes_namedtype_t)
                {
                    if (!boost::contains(p->Type->TypeString, "."))
                    {
                        if (pods.find(p->Type->TypeString) != pods.end())
                        {
                            local_found = true;
                            break;
                        }
                    }
                }
            }

            if (!local_found)
            {
                e2 = e;
                break;
            }
        }

        if (!e2)
            throw ServiceDefinitionException("Recursive pod detected");

        pods.erase(e2->Name);

        GenerateDocString(e2->DocString, "", w);

        if (e2->EntryType == DataTypes_pod_t)
        {
            w2 << "class " << export_definition(d) << " " << fix_name(e2->Name) << " : public RobotRaconteur::RRPod {"
               << std::endl;
            w2 << "public:" << std::endl;
        }
        else
        {
            w2 << "union " << fix_name(e2->Name) << "{" << std::endl;
            boost::tuple<DataTypes, size_t> namedarray_t = GetNamedArrayElementTypeAndCount(e2);
            TypeDefinition tdef;
            tdef.Type = namedarray_t.get<0>();
            convert_type_result t = convert_type(tdef);
            w2 << "boost::array<" << t.cpp_type << "," << namedarray_t.get<1>() << "> a;" << std::endl;
            w2 << "struct s_type {" << std::endl;
        }
        BOOST_FOREACH (const RR_SHARED_PTR<MemberDefinition>& m, e2->Members)
        {
            RR_SHARED_PTR<PropertyDefinition> p = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(m);
            if (!p)
                throw InternalErrorException("");
            GenerateDocString(m->DocString, "", w);
            TypeDefinition t2;
            p->Type->CopyTo(t2);
            t2.RemoveArray();
            get_variable_type_result t = get_variable_type(t2);
            t.name = fix_name((m)->Name);
            if (p->Type->ArrayType == DataTypes_ArrayTypes_none)
            {
                w2 << t.cpp_type << " " << t.name << ";" << std::endl;
            }
            else
            {
                int32_t array_count = boost::accumulate(p->Type->ArrayLength, 1, std::multiplies<int32_t>());
                if (e2->EntryType == DataTypes_pod_t)
                {
                    if (!p->Type->ArrayVarLength)
                    {
                        w2 << "RobotRaconteur::pod_field_array<" << t.cpp_type << "," << array_count << ",false> "
                           << t.name << ";" << std::endl;
                    }
                    else
                    {
                        w2 << "RobotRaconteur::pod_field_array<" << t.cpp_type << "," << array_count << ",true> "
                           << t.name << ";" << std::endl;
                    }
                }
                else
                {
                    w2 << "boost::array<" << t.cpp_type << "," << array_count << "> " << t.name << ";" << std::endl;
                }
            }
        }

        if (e2->EntryType == DataTypes_pod_t)
        {
            w2 << std::endl
               << "RR_OVIRTUAL std::string RRType() {return \"" << d->Name << "." << e2->Name << "\";  }" << std::endl;
        }
        else
        {
            w2 << "} s;" << std::endl;
        }

        w2 << "};" << std::endl;

        if (e2->EntryType == DataTypes_namedarray_t)
        {
            boost::tuple<DataTypes, size_t> namedarray_t = GetNamedArrayElementTypeAndCount(e2);
            w2 << "BOOST_STATIC_ASSERT(sizeof(" << fix_name(e2->Name)
               << ") == " << namedarray_t.get<1>() * RRArrayElementSize(namedarray_t.get<0>()) << ");" << std::endl;
        }

        w2 << std::endl;
    }

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        GenerateDocString((*e)->DocString, "", w);
        w2 << "class " << export_definition(d) << " " << fix_name((*e)->Name)
           << " : public RobotRaconteur::RRStructure {" << std::endl;
        w2 << "public:" << std::endl;
        MEMBER_ITER(PropertyDefinition)
        GenerateDocString(m->DocString, "", w);
        get_variable_type_result t = get_variable_type(*m->Type);
        t.name = fix_name((m)->Name);
        w2 << t.cpp_type << " " << t.name << ";" << std::endl;

        MEMBER_ITER_END()

        w2 << std::endl
           << "RR_OVIRTUAL std::string RRType() RR_OVERRIDE  {return \"" << d->Name << "." << (*e)->Name << "\";  }"
           << std::endl;

        w2 << "};" << std::endl << std::endl;
        w2 << "#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES" << std::endl;
        w2 << "using " << fix_name((*e)->Name) << "Ptr = RR_INTRUSIVE_PTR<" << fix_name((*e)->Name) << ">;"
           << std::endl;
        w2 << "#endif" << std::endl << std::endl;
    }

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {

        std::vector<std::string> implements;

        for (std::vector<std::string>::const_iterator ee = (*e)->Implements.begin(); ee != (*e)->Implements.end(); ++ee)
        {
            implements.push_back("public virtual " + fix_qualified_name(*ee));
        }

        if (implements.empty())
            implements.push_back("public virtual RobotRaconteur::RRObject");

        GenerateDocString((*e)->DocString, "", w);
        w2 << "class " << export_definition(d) << " " << fix_name((*e)->Name) << " : " << boost::join(implements, ", ")
           << std::endl;
        w2 << "{" << std::endl;
        w2 << "// NOLINTBEGIN" << std::endl;
        w2 << "public:" << std::endl;
        MEMBER_ITER2(PropertyDefinition)
        GenerateDocString(m->DocString, "", w);
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << pure_virtual(GetPropertyDeclaration(m.get(), true)) << "=0;" << std::endl;
        }
        if (m->Direction() != MemberDefinition_Direction_readonly)
        {
            w2 << pure_virtual(SetPropertyDeclaration(m.get(), true)) << "=0;" << std::endl;
        }
        w2 << std::endl;
        MEMBER_ITER2_END()

        MEMBER_ITER2(FunctionDefinition)
        GenerateDocString(m->DocString, "", w);
        w2 << pure_virtual(FunctionDeclaration(m.get(), true)) << "=0;" << std::endl << std::endl;
        MEMBER_ITER2_END()

        MEMBER_ITER2(EventDefinition)
        GenerateDocString(m->DocString, "", w);
        w2 << pure_virtual(EventDeclaration(m.get(), true)) << "=0;" << std::endl << std::endl;
        MEMBER_ITER2_END()

        MEMBER_ITER2(ObjRefDefinition)
        GenerateDocString(m->DocString, "", w);
        w2 << pure_virtual(ObjRefDeclaration(m.get(), true)) << "=0;" << std::endl << std::endl;
        MEMBER_ITER2_END()

        MEMBER_ITER2(PipeDefinition)
        GenerateDocString(m->DocString, "", w);
        w2 << pure_virtual(GetPipeDeclaration(m.get(), true)) << "=0;" << std::endl;
        w2 << pure_virtual(SetPipeDeclaration(m.get(), true)) << "=0;" << std::endl << std::endl;
        MEMBER_ITER2_END()

        MEMBER_ITER2(CallbackDefinition)
        GenerateDocString(m->DocString, "", w);
        w2 << pure_virtual(GetCallbackDeclaration(m.get(), true)) << "=0;" << std::endl;
        w2 << pure_virtual(SetCallbackDeclaration(m.get(), true)) << "=0;" << std::endl << std::endl;
        MEMBER_ITER2_END()

        MEMBER_ITER2(WireDefinition)
        GenerateDocString(m->DocString, "", w);
        w2 << pure_virtual(GetWireDeclaration(m.get(), true)) << "=0;" << std::endl;
        w2 << pure_virtual(SetWireDeclaration(m.get(), true)) << "=0;" << std::endl << std::endl;
        MEMBER_ITER2_END()

        MEMBER_ITER2(MemoryDefinition)
        GenerateDocString(m->DocString, "", w);
        w2 << pure_virtual(MemoryDeclaration(m.get(), true)) << "=0;" << std::endl << std::endl;
        MEMBER_ITER2_END()
        w2 << "// NOLINTEND" << std::endl;

        w2 << "RR_OVIRTUAL std::string RRType() RR_OVERRIDE  {return \"" << d->Name << "." << (*e)->Name << "\";  }"
           << std::endl;

        w2 << "};" << std::endl << std::endl;
        w2 << "#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES" << std::endl;
        w2 << "using " << fix_name((*e)->Name) << "Ptr = RR_SHARED_PTR<" << fix_name((*e)->Name) << ">;" << std::endl;
        w2 << "#endif" << std::endl << std::endl;
    }

    BOOST_FOREACH (const RR_SHARED_PTR<ExceptionDefinition>& e, d->Exceptions)
    {
        GenerateDocString(e->DocString, "", w);
        w2 << "class " << export_definition(d) << " " << fix_name(e->Name)
           << " : public RobotRaconteur::RobotRaconteurRemoteException" << std::endl
           << "{" << std::endl;
        w2 << "    public:" << std::endl;
        w2 << "    " << fix_name(e->Name)
           << "(const std::string& message, const std::string& sub_name = \"\", const "
              "RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& "
              "param_ = RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>()) : RobotRaconteur::RobotRaconteurRemoteException(\""
           << d->Name << "." << e->Name << "\",message,sub_name,param_) {}" << std::endl;
        w2 << "};" << std::endl;
        w2 << "#ifndef BOOST_NO_CXX11_TEMPLATE_ALIASES" << std::endl;
        w2 << "using " << fix_name(e->Name) << "Ptr = RR_SHARED_PTR<" << fix_name(e->Name) << ">;" << std::endl;
        w2 << "#endif" << std::endl;
    }

    for (std::vector<std::string>::iterator ns_e = namespace_vec.begin(); ns_e != namespace_vec.end(); ns_e++)
    {
        w2 << "}" << std::endl;
    }
    w2 << std::endl;

    if (!(d->Pods.empty() && d->NamedArrays.empty() && d->Enums.empty()))
    {
        w2 << "namespace RobotRaconteur" << std::endl << "{" << std::endl;
        BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& e, d->Pods)
        {
            w2 << "RRPrimUtilPod(" << fix_qualified_name(d->Name) << "::" << fix_name(e->Name) << ", \"" << d->Name
               << "." << e->Name << "\");" << std::endl;
        }
        BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& e, d->NamedArrays)
        {
            boost::tuple<DataTypes, size_t> namedarray_t = GetNamedArrayElementTypeAndCount(e);
            TypeDefinition tdef;
            tdef.Type = namedarray_t.get<0>();
            convert_type_result t = convert_type(tdef);
            w2 << "RRPrimUtilNamedArray(" << fix_qualified_name(d->Name) << "::" << fix_name(e->Name) << ", \""
               << d->Name << "." << e->Name << "\"," << t.cpp_type << ");" << std::endl;
            w2 << "RRPodStubNamedArrayType(" << fix_qualified_name(d->Name) << "::" << fix_name(e->Name) << ");"
               << std::endl;
        }
        BOOST_FOREACH (const RR_SHARED_PTR<EnumDefinition>& e, d->Enums)
        {
            w2 << "RRPrimUtilEnum(" << fix_qualified_name(d->Name) << "::" << fix_name(e->Name)
               << "::" << fix_name(e->Name) << ");" << std::endl;
        }
        w2 << "}" << std::endl;
    }
    w2 << "// NOLINTEND" << std::endl;
}

void CPPServiceLangGen::GenerateStubSkelHeaderFile(ServiceDefinition* d,
                                                   const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                                   std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl << std::endl;
    w2 << "#include \"" << boost::replace_all_copy(fix_name(d->Name), ".", "__") << ".h\"" << std::endl;
    w2 << "// NOLINTBEGIN" << std::endl;
    std::set<std::string> importedheaders;

    for (std::vector<std::string>::const_iterator e = d->Imports.begin(); e != d->Imports.end(); ++e)
    {
        importedheaders.insert(*e);
    }

    for (std::set<std::string>::const_iterator e = importedheaders.begin(); e != importedheaders.end(); ++e)
    {
        w2 << "#include \"" << boost::replace_all_copy(fix_name(*e), ".", "__") << "_stubskel.h\"" << std::endl;
    }

    w2 << "#pragma once" << std::endl << std::endl;

    std::vector<std::string> namespace_vec;
    split(namespace_vec, d->Name, boost::is_from_range('.', '.'));

    for (std::vector<std::string>::iterator ns_e = namespace_vec.begin(); ns_e != namespace_vec.end(); ns_e++)
    {
        w2 << "namespace " << fix_name(*ns_e) << std::endl << "{" << std::endl;
    }

    w2 << std::endl;
    GenerateServiceFactoryHeader(d, w);
    w2 << std::endl;
    GenerateStubHeader(d, other_defs, w);
    w2 << std::endl;
    GenerateSkelHeader(d, other_defs, w);
    w2 << std::endl;
    GenerateDefaultImplHeader(d, other_defs, false, w);
    w2 << std::endl;
    GenerateDefaultImplHeader(d, other_defs, true, w);
    w2 << std::endl;

    for (std::vector<std::string>::iterator ns_e = namespace_vec.begin(); ns_e != namespace_vec.end(); ns_e++)
    {
        w2 << "}" << std::endl;
    }

    w2 << std::endl;
    if (!d->Pods.empty())
    {
        w2 << "namespace RobotRaconteur" << std::endl;
        w2 << "{" << std::endl;

        BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& e, d->Pods)
        {
            std::string q_name = fix_qualified_name(d->Name) + "::" + fix_name(e->Name);
            w2 << "template<>" << std::endl;
            w2 << "class " << export_definition(d) << " PodStub<" << q_name << ">" << std::endl;
            w2 << "{" << std::endl;
            w2 << "public:" << std::endl;
            w2 << "    template<typename U>" << std::endl;
            w2 << "    static void PackField(const " << q_name << "& v, MessageStringRef name, U& out)" << std::endl;
            w2 << "    {" << std::endl;
            w2 << "    out.push_back(RobotRaconteur::CreateMessageElement(name, PodStub_PackPodToArray(v)));"
               << std::endl;
            w2 << "    }" << std::endl;
            w2 << "    template<typename U>" << std::endl;
            w2 << "    static void UnpackField(" << q_name << "& v, MessageStringRef name, U& in)" << std::endl;
            w2 << "    {" << std::endl;
            w2 << "    PodStub_UnpackPodFromArray(v, MessageElement::FindElement(in, "
                  "name)->CastDataToNestedList(DataTypes_pod_array_t));"
               << std::endl;
            w2 << "    }" << std::endl;
            w2 << "    static RR_INTRUSIVE_PTR<MessageElementNestedElementList> PackToMessageElementPod(const "
               << q_name << "& v)" << std::endl;
            w2 << "    {" << std::endl;
            w2 << "    std::vector<RR_INTRUSIVE_PTR<MessageElement> > o;" << std::endl;
            w2 << "    o.reserve(" << e->Members.size() << ");" << std::endl;
            BOOST_FOREACH (const RR_SHARED_PTR<MemberDefinition>& m, e->Members)
            {
                RR_SHARED_PTR<PropertyDefinition> p = rr_cast<PropertyDefinition>(m);
                w2 << "    PodStub_PackField(v." << fix_name(p->Name) << ", \"" << p->Name << "\", o);" << std::endl;
            }
            w2 << "    return RobotRaconteur::CreateMessageElementNestedElementList(DataTypes_pod_t,\"\",RR_MOVE(o));"
               << std::endl;
            w2 << "    }" << std::endl;
            w2 << "    static void UnpackFromMessageElementPod(" << q_name
               << "& v, const RR_INTRUSIVE_PTR<MessageElementNestedElementList>& m)" << std::endl;
            w2 << "    {" << std::endl;
            w2 << "    if (!m) throw NullValueException(\"Unexpected null value for pod unpack\");" << std::endl;
            // w2 << "    if (m->Type != \"" << d->Name << "." << e->Name << "\") throw DataTypeException(\"Pod type
            // mismatch\");" << std::endl;
            w2 << "    std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> >& i = m->Elements;" << std::endl;
            BOOST_FOREACH (const RR_SHARED_PTR<MemberDefinition>& m, e->Members)
            {
                RR_SHARED_PTR<PropertyDefinition> p = rr_cast<PropertyDefinition>(m);
                w2 << "    PodStub_UnpackField(v." << fix_name(p->Name) << ", \"" << p->Name << "\", i);" << std::endl;
            }
            w2 << "    }" << std::endl;
            w2 << "};" << std::endl;
        }
        w2 << "}" << std::endl;
    }
    w2 << "// NOLINTEND" << std::endl;
    w2 << std::endl;
}

void CPPServiceLangGen::GenerateStubSkelFile(ServiceDefinition* d,
                                             const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                             std::ostream* w, const std::string& servicedef)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl << std::endl;
    w2 << "#ifndef ROBOTRACONTEUR_USE_STDAFX" << std::endl;
    w2 << "#include \"" << boost::replace_all_copy(fix_name(d->Name), ".", "__") << "_stubskel.h\"" << std::endl;
    w2 << "#include <boost/algorithm/string.hpp>" << std::endl;
    w2 << "#include <boost/lexical_cast.hpp>" << std::endl;
    w2 << "#include <boost/assign/list_of.hpp>" << std::endl;
    w2 << "#else" << std::endl;
    w2 << "#include \"stdafx.h\"" << std::endl;
    w2 << "#endif" << std::endl;

    w2 << "// NOLINTBEGIN" << std::endl;
    std::vector<std::string> namespace_vec;
    split(namespace_vec, d->Name, boost::is_from_range('.', '.'));

    for (std::vector<std::string>::iterator ns_e = namespace_vec.begin(); ns_e != namespace_vec.end(); ns_e++)
    {
        w2 << "namespace " << fix_name(*ns_e) << std::endl << "{" << std::endl;
    }

    GenerateServiceFactory(d, w, servicedef);
    w2 << std::endl;
    GenerateStubDefinition(d, other_defs, w);
    w2 << std::endl;
    GenerateSkelDefinition(d, other_defs, w);
    w2 << std::endl;
    GenerateDefaultImplDefinition(d, other_defs, false, w);
    w2 << std::endl;
    GenerateDefaultImplDefinition(d, other_defs, true, w);
    w2 << std::endl;
    for (std::vector<std::string>::iterator ns_e = namespace_vec.begin(); ns_e != namespace_vec.end(); ns_e++)
    {
        w2 << "}" << std::endl;
    }
    w2 << "// NOLINTEND" << std::endl;
    w2 << std::endl;
}

void CPPServiceLangGen::GenerateServiceFactoryHeader(ServiceDefinition* d, std::ostream* w)
{

    std::string factory_name = fix_name(boost::replace_all_copy(d->Name, ".", "__")) + "Factory";

    std::ostream& w2 = *w;
    w2 << "class " << export_definition(d) << " " << factory_name << " : public virtual RobotRaconteur::ServiceFactory"
       << std::endl
       << "{" << std::endl;
    w2 << "public:" << std::endl;

    w2 << "RR_OVIRTUAL std::string GetServiceName() RR_OVERRIDE;" << std::endl;

    w2 << "RR_OVIRTUAL std::string DefString() RR_OVERRIDE;" << std::endl;

    w2 << "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::StructureStub> FindStructureStub(boost::string_ref s) RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> "
          "PackStructure(const RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure>& structin) RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> "
          "UnpackStructure(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& mstructin) "
          "RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> "
          "PackPodArray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray>& structure) RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> "
          "UnpackPodArray(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) "
          "RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> "
          "PackPodMultiDimArray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray>& structure) RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> "
          "UnpackPodMultiDimArray(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) "
          "RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> "
          "PackNamedArray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray>& structure) RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> "
          "UnpackNamedArray(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& structure) "
          "RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> "
          "PackNamedMultiDimArray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray>& structure) "
          "RR_OVERRIDE ;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> "
          "UnpackNamedMultiDimArray(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& "
          "structure) RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ServiceStub> CreateStub(boost::string_ref objecttype, "
          "boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::ClientContext>& context) RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ServiceSkel> CreateSkel(boost::string_ref objecttype, "
          "boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::RRObject>& obj, "
          "const RR_SHARED_PTR<RobotRaconteur::ServerContext>& context) RR_OVERRIDE;"
       << std::endl;

    // w2 << "virtual RR_SHARED_PTR<RobotRaconteur::ServiceDefinition> ServiceDef();" << std::endl;

    // w2 << "virtual std::string RemovePath(const std::string &path);" << std::endl;

    w2 << "RR_OVIRTUAL void DownCastAndThrowException(RobotRaconteur::RobotRaconteurException& exp) RR_OVERRIDE;"
       << std::endl;

    w2 << "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> "
          "DownCastException(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& exp) RR_OVERRIDE;"
       << std::endl;

    w2 << "};" << std::endl;
}

void CPPServiceLangGen::GenerateServiceFactory(ServiceDefinition* d, std::ostream* w, const std::string& defstring)
{
    std::string factory_name = fix_name(boost::replace_all_copy(d->Name, ".", "__")) + "Factory";
    std::ostream& w2 = *w;

    w2 << "std::string " << factory_name << "::GetServiceName()" << std::endl;
    w2 << "{" << std::endl << "return \"" << d->Name << "\";" << std::endl << "}" << std::endl;

    w2 << "std::string " << factory_name << "::DefString()" << std::endl;
    w2 << "{" << std::endl << "std::string out(" << std::endl;
    std::vector<std::string> lines;
    boost::split(lines, defstring, boost::is_from_range('\n', '\n'));
    for (std::vector<std::string>::iterator e = lines.begin(); e != lines.end(); ++e)
    {
        std::string l = boost::replace_all_copy(*e, "\\", "\\\\");
        boost::replace_all(l, "\"", "\\\"");
        boost::trim_if(l, boost::is_any_of("\r\n"));
        w2 << "\"" << l << "\\n\"" << std::endl;
    }
    w2 << ");" << std::endl;
    w2 << "return out;" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_SHARED_PTR<RobotRaconteur::StructureStub> " << factory_name << "::FindStructureStub(boost::string_ref s)"
       << std::endl
       << "{" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(s);" << std::endl;

    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        w2 << "if (objecttype==\"" << (*e)->Name
           << "\") return RobotRaconteur::rr_cast<RobotRaconteur::StructureStub>(RR_MAKE_SHARED<"
           << fix_name((*e)->Name) << "_stub>(GetNode()));" << std::endl;
    }
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid structure stub type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> " << factory_name
       << "::PackStructure(const RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure>& structin)" << std::endl
       << "{" << std::endl;
    w2 << "std::string type=structin->RRType();";
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type);"
       << std::endl;

    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->PackStructure(structin);" << std::endl;
    w2 << "RR_SHARED_PTR<RobotRaconteur::StructureStub> stub=FindStructureStub(type);" << std::endl;
    w2 << "return stub->PackStructure(structin);" << std::endl;
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid structure stub type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> " << factory_name
       << "::UnpackStructure(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& mstructin)"
       << std::endl
       << "{" << std::endl;
    w2 << "RobotRaconteur::MessageStringPtr type=mstructin->GetTypeString();" << std::endl;
    ;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type.str());"
       << std::endl;

    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->UnpackStructure(mstructin);" << std::endl;
    w2 << "RR_SHARED_PTR<RobotRaconteur::StructureStub> stub=FindStructureStub(type.str());" << std::endl;
    w2 << "return stub->UnpackStructure(mstructin);" << std::endl;
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid structure stub type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> " << factory_name
       << "::PackPodArray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray>& structin)" << std::endl
       << "{" << std::endl;
    w2 << "boost::string_ref type=structin->RRElementTypeString();" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type);"
       << std::endl;

    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->PackPodArray(structin);" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        w2 << "if (objecttype==\"" << (*e)->Name
           << "\") return RobotRaconteur::PodStub_PackPodArray(RobotRaconteur::rr_cast<RobotRaconteur::RRPodArray<"
           << fix_name((*e)->Name) << "> >(structin));" << std::endl;
    }
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid pod type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> " << factory_name
       << "::UnpackPodArray(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& mstructin)"
       << std::endl
       << "{" << std::endl;
    w2 << "RobotRaconteur::MessageStringPtr type=mstructin->GetTypeString();" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type.str());"
       << std::endl;
    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->UnpackPodArray(mstructin);" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::PodStub_UnpackPodArray<"
           << fix_name((*e)->Name) << ">(mstructin);" << std::endl;
    }
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid pod type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> " << factory_name
       << "::PackPodMultiDimArray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray>& structin)"
       << std::endl
       << "{" << std::endl;
    w2 << "boost::string_ref type=structin->RRElementTypeString();" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type);"
       << std::endl;
    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->PackPodMultiDimArray(structin);" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        w2 << "if (objecttype==\"" << (*e)->Name
           << "\") return "
              "RobotRaconteur::PodStub_PackPodMultiDimArray(RobotRaconteur::rr_cast<RobotRaconteur::RRPodMultiDimArray<"
           << fix_name((*e)->Name) << "> >(structin));" << std::endl;
    }
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid pod type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> " << factory_name
       << "::UnpackPodMultiDimArray(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& mstructin)"
       << std::endl
       << "{" << std::endl;
    w2 << "RobotRaconteur::MessageStringPtr type=mstructin->GetTypeString();" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type.str());"
       << std::endl;
    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->UnpackPodMultiDimArray(mstructin);" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::PodStub_UnpackPodMultiDimArray<"
           << fix_name((*e)->Name) << ">(mstructin);" << std::endl;
    }
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid pod type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> " << factory_name
       << "::PackNamedArray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray>& structin)" << std::endl
       << "{" << std::endl;
    w2 << "boost::string_ref type=structin->RRElementTypeString();" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type);"
       << std::endl;

    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->PackNamedArray(structin);" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        w2 << "if (objecttype==\"" << (*e)->Name
           << "\") return "
              "RobotRaconteur::NamedArrayStub_PackNamedArray(RobotRaconteur::rr_cast<RobotRaconteur::RRNamedArray<"
           << fix_name((*e)->Name) << "> >(structin));" << std::endl;
    }
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid namedarray type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> " << factory_name
       << "::UnpackNamedArray(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& mstructin)"
       << std::endl
       << "{" << std::endl;
    w2 << "RobotRaconteur::MessageStringPtr type=mstructin->GetTypeString();" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type.str());"
       << std::endl;
    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->UnpackNamedArray(mstructin);" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::NamedArrayStub_UnpackNamedArray<"
           << fix_name((*e)->Name) << ">(mstructin);" << std::endl;
    }
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid namedarray type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> " << factory_name
       << "::PackNamedMultiDimArray(const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray>& structin)"
       << std::endl
       << "{" << std::endl;
    w2 << "boost::string_ref type=structin->RRElementTypeString();" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type);"
       << std::endl;
    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->PackNamedMultiDimArray(structin);" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        w2 << "if (objecttype==\"" << (*e)->Name
           << "\") return "
              "RobotRaconteur::NamedArrayStub_PackNamedMultiDimArray(RobotRaconteur::rr_cast<RobotRaconteur::"
              "RRNamedMultiDimArray<"
           << fix_name((*e)->Name) << "> >(structin));" << std::endl;
    }
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid namedarray type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> " << factory_name
       << "::UnpackNamedMultiDimArray(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& "
          "mstructin)"
       << std::endl
       << "{" << std::endl;
    w2 << "RobotRaconteur::MessageStringPtr type=mstructin->GetTypeString();" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type.str());"
       << std::endl;
    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->UnpackNamedMultiDimArray(mstructin);"
       << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        w2 << "if (objecttype==\"" << (*e)->Name
           << "\") return RobotRaconteur::NamedArrayStub_UnpackNamedMultiDimArray<" << fix_name((*e)->Name)
           << ">(mstructin);" << std::endl;
    }
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid namedarray type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_SHARED_PTR<RobotRaconteur::ServiceStub> " << factory_name
       << "::CreateStub(boost::string_ref type, boost::string_ref path, const "
          "RR_SHARED_PTR<RobotRaconteur::ClientContext>& "
          "context)"
       << std::endl
       << "{" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type);"
       << std::endl;

    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name
       << "\") return GetNode()->GetServiceType(servicetype)->CreateStub(type,path,context);" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        w2 << "if (objecttype==\"" << (*e)->Name << "\") { RR_SHARED_PTR<" << fix_name((*e)->Name)
           << "_stub> o=(RR_MAKE_SHARED<" << fix_name((*e)->Name)
           << "_stub>(path,context)); o->RRInitStub(); return o; }" << std::endl;
    }
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid structure stub type.\");" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_SHARED_PTR<RobotRaconteur::ServiceSkel> " << factory_name
       << "::CreateSkel(boost::string_ref type, const boost::string_ref path, const "
          "RR_SHARED_PTR<RobotRaconteur::RRObject>& "
          "obj, const RR_SHARED_PTR<RobotRaconteur::ServerContext>& context)"
       << std::endl
       << "{" << std::endl;

    w2 << "boost::tuple<boost::string_ref,boost::string_ref> res=RobotRaconteur::SplitQualifiedName(type);"
       << std::endl;

    w2 << "boost::string_ref servicetype=res.get<0>();" << std::endl;
    w2 << "boost::string_ref objecttype=res.get<1>();" << std::endl;
    w2 << "if (servicetype != \"" << d->Name
       << "\") return GetNode()->GetServiceType(servicetype)->CreateSkel(type,path,obj,context);" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        w2 << "if (objecttype==\"" << (*e)->Name << "\") {RR_SHARED_PTR<" << fix_name((*e)->Name)
           << "_skel> o=RR_MAKE_SHARED<" << fix_name((*e)->Name) << "_skel>(); o->Init(path,obj,context); return o; }"
           << std::endl;
    }
    w2 << "throw RobotRaconteur::ServiceException(\"Invalid structure skel type.\");" << std::endl;

    w2 << "return RR_SHARED_PTR<RobotRaconteur::ServiceSkel>();" << std::endl;
    w2 << "}" << std::endl;

    w2 << "void " << factory_name << "::DownCastAndThrowException(RobotRaconteur::RobotRaconteurException& rr_exp)"
       << std::endl
       << "{" << std::endl;
    w2 << "std::string rr_type=rr_exp.Error;" << std::endl;
    w2 << "if (rr_type.find('.')==std::string::npos)" << std::endl;
    w2 << "{" << std::endl;
    w2 << "	return;" << std::endl;
    w2 << "}" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> rr_res=RobotRaconteur::SplitQualifiedName(rr_type);"
       << std::endl;
    w2 << "if (rr_res.get<0>() != \"" << d->Name << "\") GetNode()->DownCastAndThrowException(rr_exp);" << std::endl;
    BOOST_FOREACH (const RR_SHARED_PTR<ExceptionDefinition>& e, d->Exceptions)
    {
        w2 << "if (rr_res.get<1>()==\"" << e->Name << "\") throw " << fix_name(e->Name)
           << "(rr_exp.Message,rr_exp.ErrorSubName,rr_exp.ErrorParam);" << std::endl;
    }
    w2 << "return;" << std::endl;
    w2 << "}" << std::endl;

    w2 << "RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> " << factory_name
       << "::DownCastException(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& rr_exp)"
       << "{" << std::endl;
    w2 << "if (!rr_exp) return rr_exp;" << std::endl;
    w2 << "std::string rr_type=rr_exp->Error;" << std::endl;
    w2 << "if (rr_type.find('.')==std::string::npos)" << std::endl;
    w2 << "{" << std::endl;
    w2 << "	return rr_exp;" << std::endl;
    w2 << "}" << std::endl;
    w2 << "boost::tuple<boost::string_ref,boost::string_ref> rr_res=RobotRaconteur::SplitQualifiedName(rr_type);"
       << std::endl;
    w2 << "if (rr_res.get<0>() != \"" << d->Name << "\") return GetNode()->DownCastException(rr_exp);" << std::endl;
    BOOST_FOREACH (const RR_SHARED_PTR<ExceptionDefinition>& e, d->Exceptions)
    {
        w2 << "if (rr_res.get<1>()==\"" << e->Name << "\") return RR_MAKE_SHARED<" << fix_name(e->Name)
           << ">(rr_exp->Message,rr_exp->ErrorSubName,rr_exp->ErrorParam);" << std::endl;
    }
    w2 << "return rr_exp;" << std::endl;
    w2 << "}" << std::endl;
}

void CPPServiceLangGen::GenerateStubHeader(ServiceDefinition* d,
                                           const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                           std::ostream* w)
{
    RR_UNUSED(other_defs);
    std::ostream& w2 = *w;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        w2 << "class " << export_definition(d) << " " << fix_name((*e)->Name)
           << "_stub : public virtual RobotRaconteur::StructureStub" << std::endl
           << "{" << std::endl;
        w2 << "public:" << std::endl;
        w2 << "" << fix_name((*e)->Name)
           << "_stub(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurNode>& node) : "
              "RobotRaconteur::StructureStub(node) {}"
           << std::endl;
        w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> "
              "PackStructure(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& s) RR_OVERRIDE ;"
           << std::endl;
        w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> "
              "UnpackStructure(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& m) RR_OVERRIDE "
              ";"
           << std::endl;
        w2 << "};" << std::endl << std::endl;
    }

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        std::vector<std::string> implements;

        for (std::vector<std::string>::const_iterator ee = (*e)->Implements.begin(); ee != (*e)->Implements.end(); ++ee)
        {
            std::string iname = fix_qualified_name(*ee);
            if (iname.find("::") != std::string::npos)
            {
                size_t pos = iname.rfind("::");

                iname = iname.substr(0, pos) + "::async_" + iname.substr(pos + 2, iname.size() - pos - 2);
            }
            implements.push_back("public virtual " + iname);
        }

        w2 << "class " << export_definition(d) << " async_" << fix_name((*e)->Name);
        if (!implements.empty())
        {
            w2 << " : " << boost::join(implements, ", ");
        }

        w2 << std::endl;
        w2 << "{" << std::endl;
        w2 << "// NOLINTBEGIN" << std::endl;
        w2 << "public:" << std::endl;

        MEMBER_ITER(PropertyDefinition)
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << pure_virtual(GetPropertyDeclaration_async(m.get(), true)) << " = 0;" << std::endl;
        }
        if (m->Direction() != MemberDefinition_Direction_readonly)
        {
            w2 << pure_virtual(SetPropertyDeclaration_async(m.get(), true)) << " = 0;" << std::endl << std::endl;
        }

        w2 << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(FunctionDefinition)
        w2 << pure_virtual(FunctionDeclaration_async(m.get(), true)) << " = 0;" << std::endl << std::endl;

        w2 << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(ObjRefDefinition)
        w2 << pure_virtual(ObjRefDeclaration_async(m.get(), true)) << "=0;" << std::endl << std::endl;

        MEMBER_ITER_END()
        w2 << "// NOLINTEND" << std::endl;
        w2 << "};" << std::endl;
    }

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        w2 << "class " << export_definition(d) << " " << fix_name((*e)->Name) << "_stub : public virtual "
           << fix_name((*e)->Name) << ", public virtual async_" << fix_name((*e)->Name)
           << ", public virtual RobotRaconteur::ServiceStub" << std::endl;
        w2 << "{" << std::endl;
        w2 << "public:" << std::endl;

        w2 << fix_name((*e)->Name) << "_stub(boost::string_ref, const RR_SHARED_PTR<RobotRaconteur::ClientContext>& c);"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL void RRInitStub() RR_OVERRIDE ;" << std::endl;

        MEMBER_ITER(PropertyDefinition)
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << GetPropertyDeclaration(m.get(), true) << ";" << std::endl;
        }
        if (m->Direction() != MemberDefinition_Direction_readonly)
        {
            w2 << SetPropertyDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        }
        MEMBER_ITER_END()

        MEMBER_ITER(FunctionDefinition)
        w2 << FunctionDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(EventDefinition)
        w2 << EventDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(ObjRefDefinition)
        w2 << ObjRefDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(PipeDefinition)
        w2 << GetPipeDeclaration(m.get(), true) << ";" << std::endl;
        w2 << SetPipeDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(CallbackDefinition)
        w2 << GetCallbackDeclaration(m.get(), true) << ";" << std::endl;
        w2 << SetCallbackDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(WireDefinition)
        w2 << GetWireDeclaration(m.get(), true) << ";" << std::endl;
        w2 << SetWireDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(MemoryDefinition)
        w2 << MemoryDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER_END()

        w2 << std::endl;
        w2 << "RR_OVIRTUAL void DispatchEvent(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;"
           << std::endl;
        w2 << "RR_OVIRTUAL void DispatchPipeMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) "
              "RR_OVERRIDE;"
           << std::endl;
        w2 << "RR_OVIRTUAL void DispatchWireMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) "
              "RR_OVERRIDE;"
           << std::endl;
        w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "CallbackCall(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;"
           << std::endl;
        w2 << "RR_OVIRTUAL void RRClose() RR_OVERRIDE;" << std::endl;
        w2 << "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::PipeClientBase> RRGetPipeClient(boost::string_ref membername) "
              "RR_OVERRIDE;"
           << std::endl;
        w2 << "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::WireClientBase> RRGetWireClient(boost::string_ref membername) "
              "RR_OVERRIDE;"
           << std::endl;

        w2 << "private:" << std::endl;

        MEMBER_ITER(EventDefinition)
        w2 << EventDeclaration(m.get(), true, true) << ";" << std::endl << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(PipeDefinition)
        w2 << "RR_SHARED_PTR<RobotRaconteur::PipeClient<" << get_variable_type(*m->Type).cpp_type << " > > rrvar_"
           << m->Name << ";" << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(CallbackDefinition)
        w2 << "RR_SHARED_PTR<RobotRaconteur::CallbackClient<" << GetCallbackDeclaration(m.get(), true, true)
           << " > > rrvar_" << m->Name << ";" << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(WireDefinition)
        w2 << "RR_SHARED_PTR<RobotRaconteur::WireClient<" << get_variable_type(*m->Type).cpp_type << " > > rrvar_"
           << m->Name << ";" << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(MemoryDefinition)
        if (IsTypeNumeric(m->Type->Type))
        {
            if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
                w2 << "RR_SHARED_PTR<RobotRaconteur::ArrayMemoryClient<" << convert_type(*m->Type).cpp_type
                   << " > > rrvar_" << m->Name << ";" << std::endl;
            else
                w2 << "RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemoryClient<" << convert_type(*m->Type).cpp_type
                   << " > > rrvar_" << m->Name << ";" << std::endl;
        }
        else
        {
            std::string c = "Pod";
            if (m->Type->ResolveNamedType()->RRDataType() == DataTypes_namedarray_t)
            {
                c = "Named";
            }
            if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
                w2 << "RR_SHARED_PTR<RobotRaconteur::" << c << "ArrayMemoryClient<" << convert_type(*m->Type).cpp_type
                   << " > > rrvar_" << m->Name << ";" << std::endl;
            else
                w2 << "RR_SHARED_PTR<RobotRaconteur::" << c << "MultiDimArrayMemoryClient<"
                   << convert_type(*m->Type).cpp_type << " > > rrvar_" << m->Name << ";" << std::endl;
        }
        MEMBER_ITER_END()

        MEMBER_ITER(PropertyDefinition)
        get_variable_type_result t = get_variable_type(*m->Type);
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << GetPropertyDeclaration_async(m.get(), true) << ";" << std::endl;
        }
        if (m->Direction() != MemberDefinition_Direction_readonly)
        {
            w2 << SetPropertyDeclaration_async(m.get(), true) << ";" << std::endl << std::endl;
        }
        w2 << "protected:" << std::endl;
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << "virtual void rrend_get_" << fix_name(m->Name)
               << "(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                  "const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, boost::function< void ("
               << t.cpp_param_type << " ,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > handler);"
               << std::endl;
        }
        if (m->Direction() != MemberDefinition_Direction_readonly)
        {
            w2 << "virtual void rrend_set_" << fix_name(m->Name)
               << "(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                  "const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, boost::function< void "
                  "(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > handler);"
               << std::endl;
        }

        w2 << "public:" << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(FunctionDefinition)
        w2 << FunctionDeclaration_async(m.get(), true) << ";" << std::endl << std::endl;
        if (!m->IsGenerator())
        {
            w2 << "protected:" << std::endl;
            if (m->ReturnType->Type == DataTypes_void_t)
            {

                w2 << "virtual void rrend_" << fix_name(m->Name)
                   << "(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                      "const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, boost::function< void "
                      "(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > handler);"
                   << std::endl;
            }
            else
            {
                get_variable_type_result t = get_variable_type(*m->ReturnType);
                w2 << "virtual void rrend_" << fix_name(m->Name)
                   << "(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                      "const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, boost::function< void ("
                   << t.cpp_param_type << " ,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > handler);"
                   << std::endl;
            }

            w2 << "public:" << std::endl;
        }
        else
        {
            convert_generator_result t = convert_generator(m.get());
            w2 << "virtual void rrend_" << fix_name(m->Name)
               << "(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                  "const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, boost::function< void ("
               << t.generator_cpp_type << " ,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > handler);"
               << std::endl;
        }
        MEMBER_ITER_END()

        MEMBER_ITER(ObjRefDefinition)
        w2 << ObjRefDeclaration_async(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER_END()

        w2 << "RR_OVIRTUAL std::string RRType() RR_OVERRIDE;" << std::endl;
        w2 << "};" << std::endl << std::endl;
    }
}

void CPPServiceLangGen::GenerateSkelHeader(ServiceDefinition* d,
                                           const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                           std::ostream* w)
{
    RR_UNUSED(other_defs);
    std::ostream& w2 = *w;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        w2 << "class " << export_definition(d) << " " << fix_name((*e)->Name) << "_skel : "
           << "public virtual RobotRaconteur::ServiceSkel" << std::endl;
        w2 << "{" << std::endl;
        w2 << "public:" << std::endl;
        w2 << "RR_OVIRTUAL void Init(boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::RRObject>& object, "
              "const RR_SHARED_PTR<RobotRaconteur::ServerContext>& context) RR_OVERRIDE;"
           << std::endl;
        w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "CallGetProperty(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "CallSetProperty(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "CallFunction(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL void ReleaseCastObject() RR_OVERRIDE;" << std::endl << std::endl;
        w2 << "RR_OVIRTUAL void RegisterEvents(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL void UnregisterEvents(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::RRObject> GetSubObj(boost::string_ref, boost::string_ref) "
              "RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL void InitPipeServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL void InitWireServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL void DispatchPipeMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t "
              "e) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL void DispatchWireMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t "
              "e) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL void InitCallbackServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& o) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "CallPipeFunction(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "CallWireFunction(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL RR_SHARED_PTR<void> GetCallbackFunction(uint32_t endpoint, boost::string_ref) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "CallMemoryFunction(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
              "const RR_SHARED_PTR<RobotRaconteur::Endpoint>& e) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL bool IsRequestNoLock(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m) RR_OVERRIDE;"
           << std::endl
           << std::endl;
        w2 << "RR_OVIRTUAL std::string GetObjectType() RR_OVERRIDE;" << std::endl;
        w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
           << " > get_obj();" << std::endl
           << std::endl;
        w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::"
           << "async_" << fix_name((*e)->Name) << " > get_asyncobj();" << std::endl
           << std::endl;
        w2 << "protected:" << std::endl;
        MEMBER_ITER(PropertyDefinition)
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << "static void rr_get_" << fix_name(m->Name) << "(RR_WEAK_PTR<"
               << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
               << "_skel> skel, " << get_variable_type(*m->Type, true).cpp_type
               << " value, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, "
                  "const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, const "
                  "RR_SHARED_PTR<RobotRaconteur::ServerEndpoint>& ep);"
               << std::endl;
        }
        MEMBER_ITER_END()

        MEMBER_ITER(FunctionDefinition)

        std::vector<std::string> v1;
        if (!m->IsGenerator())
        {
            if (m->ReturnType->Type != DataTypes_void_t)
            {
                v1.push_back(get_variable_type(*m->ReturnType, true).cpp_type + " ret");
            }
            v1.push_back("const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, "
                         "const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                         "RR_SHARED_PTR<RobotRaconteur::ServerEndpoint> ep");
            w2 << "static void rr_" << fix_name(m->Name) << "(RR_WEAK_PTR<"
               << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
               << "_skel> skel, " << boost::join(v1, ", ") << ");" << std::endl;
        }
        else
        {
            convert_generator_result t = convert_generator(m.get());
            w2 << "static void rr_" << fix_name(m->Name) << "(RR_WEAK_PTR<"
               << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
               << "_skel> skel, " << t.generator_cpp_type
               << " ret, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, "
                  "const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                  "RR_SHARED_PTR<RobotRaconteur::ServerEndpoint> ep"
               << ");" << std::endl;
        }
        MEMBER_ITER_END()
        w2 << " public:" << std::endl;

        MEMBER_ITER(EventDefinition)
        std::vector<std::string> params;
        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee = m->Parameters.begin();
             ee != m->Parameters.end(); ++ee)
            params.push_back(get_variable_type(*(*ee)).cpp_type + " " + fix_name((*ee)->Name));
        w2 << "void rr_" << fix_name(m->Name) << "_Handler(" << boost::join(params, ", ") << ");" << std::endl;
        MEMBER_ITER_END()

        w2 << "protected:";
        MEMBER_ITER(EventDefinition)

        w2 << "boost::signals2::connection " << fix_name(m->Name) << "_rrconnection;" << std::endl;
        MEMBER_ITER_END()

        w2 << "bool rr_InitPipeServersRun;" << std::endl;
        w2 << "bool rr_InitWireServersRun;" << std::endl;
        MEMBER_ITER(PipeDefinition)
        w2 << "RR_SHARED_PTR<RobotRaconteur::PipeServer<" << get_variable_type(*m->Type).cpp_type << " > > rr_"
           << m->Name << "_pipe;" << std::endl;
        MEMBER_ITER_END()
        MEMBER_ITER(WireDefinition)
        w2 << "RR_SHARED_PTR<RobotRaconteur::WireServer<" << get_variable_type(*m->Type).cpp_type << " > > rr_"
           << m->Name << "_wire;" << std::endl;
        MEMBER_ITER_END()

        w2 << "public: " << std::endl;
        MEMBER_ITER(CallbackDefinition)
        std::vector<std::string> p;
        p.push_back("uint32_t rrendpoint");
        if (!m->Parameters.empty())
            p.push_back(str_pack_parameters(m->Parameters));
        w2 << get_variable_type(*m->ReturnType).cpp_type << " rr_" << m->Name << "_callback(" << boost::join(p, ", ")
           << ");" << std::endl;
        MEMBER_ITER_END();
        w2 << "private:" << std::endl;
        MEMBER_ITER(MemoryDefinition)
        convert_type_result t = convert_type(*m->Type);
        t.name = m->Name;
        if (IsTypeNumeric(m->Type->Type))
        {
            if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
            {
                w2 << "RR_SHARED_PTR<RobotRaconteur::ArrayMemoryServiceSkel<" << t.cpp_type << " > > rr_" << t.name
                   << "_mem;" << std::endl;
            }
            else
            {
                w2 << "RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemoryServiceSkel<" << t.cpp_type << " > > rr_"
                   << t.name << "_mem;" << std::endl;
            }
        }
        else
        {
            std::string c = "Pod";
            if (m->Type->ResolveNamedType()->RRDataType() == DataTypes_namedarray_t)
            {
                c = "Named";
            }
            if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
            {
                w2 << "RR_SHARED_PTR<RobotRaconteur::" << c << "ArrayMemoryServiceSkel<" << t.cpp_type << " > > rr_"
                   << t.name << "_mem;" << std::endl;
            }
            else
            {
                w2 << "RR_SHARED_PTR<RobotRaconteur::" << c << "MultiDimArrayMemoryServiceSkel<" << t.cpp_type
                   << " > > rr_" << t.name << "_mem;" << std::endl;
            }
        }
        MEMBER_ITER_END()

        w2 << std::endl << "};" << std::endl << std::endl;
    }
}

std::string dforc(const std::string& definition, const std::string& clas)
{
    std::string res1_1 = boost::replace_first_copy(definition, "virtual", "");
    std::string res1_2 = boost::replace_last_copy(res1_1, "RR_OVERRIDE", "");
    std::string res1 = boost::trim_copy(boost::replace_first_copy(res1_2, "RR_OVIRTUAL", ""));

    std::string rettype;
    std::string function;

    int ccount = 0;
    size_t pos = 0;

    for (; pos < res1.size(); pos++)
    {
        if (res1[pos] == '<')
            ccount++;
        if (res1[pos] == '>')
            ccount--;

        if (ccount == 0 && (res1[pos] == ' ' || res1[pos] == '\t'))
        {
            break;
        }
    }

    rettype = boost::trim_copy(res1.substr(0, pos));
    function = boost::trim_copy(res1.substr(pos));

    if (function.length() == 0)
        throw InternalErrorException("Internal error");

    return boost::trim_copy(rettype) + " " + clas + "::" + boost::trim_copy(function);
}

static std::string CPPServiceLangGen_unreliable_str(bool unreliable) { return unreliable ? "true" : "false"; }

static std::string CPPServiceLangGen_direction_str(MemberDefinition_Direction direction)
{
    switch (direction)
    {
    case MemberDefinition_Direction_both:
        return "RobotRaconteur::MemberDefinition_Direction_both";
    case MemberDefinition_Direction_readonly:
        return "RobotRaconteur::MemberDefinition_Direction_readonly";
    case MemberDefinition_Direction_writeonly:
        return "RobotRaconteur::MemberDefinition_Direction_writeonly";
    default:
        throw InvalidArgumentException("Invalid member direction");
    }
}

void CPPServiceLangGen::GenerateStubDefinition(ServiceDefinition* d,
                                               const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                               std::ostream* w)
{

    RR_UNUSED(other_defs);
    std::ostream& w2 = *w;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList> " << fix_name((*e)->Name)
           << "_stub::PackStructure(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& s)" << std::endl
           << "{" << std::endl;
        w2 << "RR_INTRUSIVE_PTR<" << fix_qualified_name((*e)->Name) << " > s2=RobotRaconteur::rr_cast<"
           << fix_qualified_name((*e)->Name) << " >(s);" << std::endl;
        w2 << "std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> > vret;" << std::endl;
        size_t field_count = 0;
        MEMBER_ITER(PropertyDefinition)
        field_count++;
        MEMBER_ITER_END()
        w2 << "vret.reserve(" << field_count << ");" << std::endl;
        MEMBER_ITER(PropertyDefinition)
        w2 << "vret.push_back(" << str_pack_message_element(m->Name, "s2->" + fix_name(m->Name), m->Type) << ");"
           << std::endl;
        MEMBER_ITER_END()
        w2 << "return RobotRaconteur::CreateMessageElementNestedElementList(RobotRaconteur::DataTypes_structure_t,\""
           << d->Name << "." << (*e)->Name << "\",RR_MOVE(vret));" << std::endl;
        w2 << "}" << std::endl;

        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> " << fix_name((*e)->Name)
           << "_stub::UnpackStructure(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNestedElementList>& m)"
           << std::endl
           << "{" << std::endl;
        w2 << "std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> >& i = m->Elements;" << std::endl;
        w2 << "RR_INTRUSIVE_PTR<" << fix_qualified_name((*e)->Name) << " > ret(new " << fix_qualified_name((*e)->Name)
           << "());" << std::endl;
        MEMBER_ITER(PropertyDefinition)
        w2 << "ret->" << fix_name(m->Name) << "="
           << str_unpack_message_element("RobotRaconteur::MessageElement::FindElement(i,\"" + m->Name + "\")", m->Type)
           << ";" << std::endl;
        MEMBER_ITER_END()
        w2 << "return ret;" << std::endl;
        w2 << "}" << std::endl << std::endl;
    }

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        MEMBER_ITER(PipeDefinition)
        if (CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
        {
            w2 << "static void " << fix_name((*e)->Name) << "_stub_rrverify_" << m->Name
               << "(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)" << std::endl
               << "{" << std::endl;
            w2 << CPPServiceLangGen_VerifyArrayLength(
                      *m->Type, "RobotRaconteur::rr_cast<" +
                                    remove_RR_INTRUSIVE_PTR(get_variable_type(*m->Type).cpp_type) + " >(value)")
               << ";" << std::endl;
            w2 << "}" << std::endl;
        }
        MEMBER_ITER_END()
        MEMBER_ITER(WireDefinition)
        if (CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
        {
            w2 << "static void " << fix_name((*e)->Name) << "_stub_rrverify_" << m->Name
               << "(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)" << std::endl
               << "{" << std::endl;
            w2 << CPPServiceLangGen_VerifyArrayLength(
                      *m->Type, "RobotRaconteur::rr_cast<" +
                                    remove_RR_INTRUSIVE_PTR(get_variable_type(*m->Type).cpp_type) + " >(value)")
               << ";" << std::endl;
            w2 << "}" << std::endl;
        }
        MEMBER_ITER_END()

        // Constructor
        w2 << fix_name((*e)->Name) << "_stub::" << fix_name((*e)->Name)
           << "_stub(boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::ClientContext>& c) : "
              "RobotRaconteur::ServiceStub(path,c)"
           << "{ }" << std::endl;

        w2 << "void " << fix_name((*e)->Name) << "_stub::RRInitStub()" << std::endl << "{" << std::endl;
        MEMBER_ITER(PipeDefinition)
        std::string unreliable_str = CPPServiceLangGen_unreliable_str(m->IsUnreliable());
        std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
        if (!CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
        {
            w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::PipeClient<"
               << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(), "
               << unreliable_str << "," << direction_str << ") ;" << std::endl;
        }
        else
        {
            w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::PipeClient<"
               << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(), "
               << unreliable_str << "," << direction_str << ",&" << fix_name((*e)->Name) << "_stub_rrverify_" << m->Name
               << ") ;" << std::endl;
        }
        MEMBER_ITER_END()

        MEMBER_ITER(CallbackDefinition)
        w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::CallbackClient<"
           << GetCallbackDeclaration(m.get(), true, true) << " > >(\"" << m->Name << "\") ;" << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(WireDefinition)
        std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
        if (!CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
        {
            w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::WireClient<"
               << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(),"
               << direction_str << ");" << std::endl;
        }
        else
        {
            w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::WireClient<"
               << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(),"
               << direction_str << ",&" << fix_name((*e)->Name) << "_stub_rrverify_" << m->Name << "); " << std::endl;
        }
        MEMBER_ITER_END()
        MEMBER_ITER(MemoryDefinition)
        std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
        if (IsTypeNumeric(m->Type->Type))
        {
            if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
            {
                w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::ArrayMemoryClient<"
                   << convert_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(),"
                   << direction_str << ");" << std::endl;
            }
            else
            {
                w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::MultiDimArrayMemoryClient<"
                   << convert_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(),"
                   << direction_str << ");" << std::endl;
            }
        }
        else
        {
            size_t elem_size = 0;

            std::string c = "Pod";
            if (m->Type->ResolveNamedType()->RRDataType() == DataTypes_namedarray_t)
            {
                c = "Named";
                boost::tuple<DataTypes, size_t> namedarray_t =
                    GetNamedArrayElementTypeAndCount(rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType()));
                elem_size = namedarray_t.get<1>();
            }
            else
            {
                elem_size = EstimatePodPackedElementSize(rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType()));
            }

            if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
            {
                w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::" << c << "ArrayMemoryClient<"
                   << convert_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << elem_size
                   << "," << direction_str << ");" << std::endl;
            }
            else
            {
                w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::" << c << "MultiDimArrayMemoryClient<"
                   << convert_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << elem_size
                   << "," << direction_str << ");" << std::endl;
            }
        }
        MEMBER_ITER_END()
        w2 << "}" << std::endl << std::endl;

        // Access functions
        MEMBER_ITER(PropertyDefinition)
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << dforc(GetPropertyDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl
               << "{" << std::endl;
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
                  "m=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertyGetReq,\""
               << m->Name << "\");" << std::endl;
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr=ProcessRequest(m);" << std::endl;
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=mr->FindElement(\"value\");" << std::endl;
            w2 << "return " << str_unpack_message_element("me", m->Type) << ";" << std::endl;
            w2 << "}" << std::endl;
        }
        if (m->Direction() != MemberDefinition_Direction_readonly)
        {
            w2 << dforc(SetPropertyDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl
               << "{" << std::endl;
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
                  "req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertySetReq,\""
               << m->Name << "\");" << std::endl;
            w2 << "req->AddElement(" << str_pack_message_element("value", "value", m->Type) << ");" << std::endl;
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr=ProcessRequest(req);" << std::endl;
            w2 << "}" << std::endl << std::endl;
        }
        MEMBER_ITER_END()

        MEMBER_ITER(FunctionDefinition)
        w2 << dforc(FunctionDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl
           << "{" << std::endl;
        if (!m->IsGenerator())
        {
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
                  "rr_req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq,\""
               << m->Name << "\");" << std::endl;
            for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator ee = m->Parameters.begin();
                 ee != m->Parameters.end(); ++ee)
            {
                w2 << "rr_req->AddElement(" << str_pack_message_element((*ee)->Name, fix_name((*ee)->Name), (*ee))
                   << ");" << std::endl;
            }
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_ret=ProcessRequest(rr_req);" << std::endl;
            if (m->ReturnType->Type != DataTypes_void_t)
            {
                w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> rr_me=rr_ret->FindElement(\"return\");"
                   << std::endl;
                w2 << "return " << str_unpack_message_element("rr_me", m->ReturnType) << ";" << std::endl;
            }
        }
        else
        {
            convert_generator_result t = convert_generator(m.get());
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
                  "rr_req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq,\""
               << m->Name << "\");" << std::endl;
            for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator ee = t.params.begin();
                 ee != t.params.end(); ++ee)
            {
                w2 << "rr_req->AddElement(" << str_pack_message_element((*ee)->Name, fix_name((*ee)->Name), (*ee))
                   << ");" << std::endl;
            }
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_ret=ProcessRequest(rr_req);" << std::endl;
            w2 << "return RR_MAKE_SHARED<RobotRaconteur::GeneratorClient<" << t.return_type << "," << t.param_type
               << " > >(\"" << m->Name
               << "\", "
                  "RobotRaconteur::RRArrayToScalar(rr_ret->FindElement(\"index\")->CastData<RobotRaconteur::RRArray<"
                  "int32_t> >()),shared_from_this());"
               << std::endl;
        }
        w2 << "}" << std::endl << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(EventDefinition)
        w2 << dforc(EventDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl << "{" << std::endl;
        w2 << "return rrvar_" << m->Name << ";" << std::endl;
        w2 << "}" << std::endl << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(ObjRefDefinition)
        w2 << dforc(ObjRefDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl << "{" << std::endl;
        std::string objecttype = fix_qualified_name(m->ObjectType);

        if (m->ObjectType == "varobject")
        {
            objecttype = "RobotRaconteur::RRObject";
            OBJREF_ARRAY_CONTAINER_CMD(m,
                                       w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRef(\""
                                          << m->Name << "\"));" << std::endl,
                                       w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRef(\""
                                          << m->Name << "\", boost::lexical_cast<std::string>(ind)));" << std::endl,
                                       w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRef(\""
                                          << m->Name << "\", boost::lexical_cast<std::string>(ind)));" << std::endl,
                                       w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRef(\""
                                          << m->Name << "\", ind));" << std::endl)
        }
        else
        {
            std::string objecttype2;
            if (m->ObjectType.find('.') == std::string::npos)
            {
                objecttype2 = d->Name + "." + m->ObjectType;
            }
            else
            {
                objecttype2 = m->ObjectType;
            }

            OBJREF_ARRAY_CONTAINER_CMD(
                m,
                w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRefTyped(\"" << m->Name << "\",\""
                   << objecttype2 << "\"));" << std::endl,
                w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRefTyped(\"" << m->Name
                   << "\", boost::lexical_cast<std::string>(ind),\"" << objecttype2 << "\"));" << std::endl,
                w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRefTyped(\"" << m->Name
                   << "\", boost::lexical_cast<std::string>(ind),\"" << objecttype2 << "\"));" << std::endl,
                w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRefTyped(\"" << m->Name
                   << "\", ind,\"" << objecttype2 << "\"));" << std::endl)
        }

        w2 << "}" << std::endl << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(PipeDefinition)
        w2 << dforc(GetPipeDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl << "{" << std::endl;
        get_variable_type_result t = get_variable_type(*m->Type);
        w2 << "RR_SHARED_PTR<RobotRaconteur::PipeClient<" << t.cpp_type << " > > value=rrvar_" << m->Name << ";"
           << std::endl;
        w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");" << std::endl;
        w2 << "return value;" << std::endl;
        w2 << "}" << std::endl;
        w2 << dforc(SetPipeDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl << "{" << std::endl;
        w2 << "throw RobotRaconteur::InvalidOperationException(\"Not valid for client\");" << std::endl;
        w2 << "}" << std::endl << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(CallbackDefinition)
        w2 << dforc(GetCallbackDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl
           << "{" << std::endl;
        w2 << "RR_SHARED_PTR<RobotRaconteur::CallbackClient<" << GetCallbackDeclaration(m.get(), true, true)
           << " > > value=rrvar_" << m->Name << ";" << std::endl;
        w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");" << std::endl;
        w2 << "return value;" << std::endl;
        w2 << "}" << std::endl;
        w2 << dforc(SetCallbackDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl
           << "{" << std::endl;
        w2 << "throw RobotRaconteur::InvalidOperationException(\"Not valid for client\");" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(WireDefinition)
        w2 << dforc(GetWireDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl << "{" << std::endl;
        get_variable_type_result t = get_variable_type(*m->Type);
        w2 << "RR_SHARED_PTR<RobotRaconteur::WireClient<" << t.cpp_type << " > > value=rrvar_" << m->Name << ";"
           << std::endl;
        w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");" << std::endl;
        w2 << "return value;" << std::endl;
        w2 << "}" << std::endl;
        w2 << dforc(SetWireDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl << "{" << std::endl;
        w2 << "throw RobotRaconteur::InvalidOperationException(\"Not valid for client\");" << std::endl;
        w2 << "}" << std::endl << std::endl;

        MEMBER_ITER_END()

        MEMBER_ITER(MemoryDefinition)
        w2 << dforc(MemoryDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << std::endl << "{" << std::endl;
        if (IsTypeNumeric(m->Type->Type))
        {
            if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
            {
                w2 << "RR_SHARED_PTR<RobotRaconteur::ArrayMemoryClient<" << convert_type(*m->Type).cpp_type
                   << " > > value=rrvar_" << m->Name << ";" << std::endl;
                w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");"
                   << std::endl;
                w2 << "return value;" << std::endl;
            }
            else
            {
                w2 << "RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemoryClient<" << convert_type(*m->Type).cpp_type
                   << " > > value=rrvar_" << m->Name << ";" << std::endl;
                w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");"
                   << std::endl;
                w2 << "return value;" << std::endl;
            }
        }
        else
        {
            std::string c = "Pod";
            if (m->Type->ResolveNamedType()->RRDataType() == DataTypes_namedarray_t)
            {
                c = "Named";
            }
            if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
            {
                w2 << "RR_SHARED_PTR<RobotRaconteur::" << c << "ArrayMemoryClient<" << convert_type(*m->Type).cpp_type
                   << " > > value=rrvar_" << m->Name << ";" << std::endl;
                w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");"
                   << std::endl;
                w2 << "return value;" << std::endl;
            }
            else
            {
                w2 << "RR_SHARED_PTR<RobotRaconteur::" << c << "MultiDimArrayMemoryClient<"
                   << convert_type(*m->Type).cpp_type << " > > value=rrvar_" << m->Name << ";" << std::endl;
                w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");"
                   << std::endl;
                w2 << "return value;" << std::endl;
            }
        }

        w2 << "}" << std::endl;
        MEMBER_ITER_END()

        w2 << "void " << fix_name((*e)->Name)
           << "_stub::DispatchEvent(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& rr_m)" << std::endl
           << "{" << std::endl;
        // w2 << "boost::shared_lock<boost::shared_mutex> lock(context_lock);" << std::endl;
        MEMBER_ITER(EventDefinition)
        w2 << "if (rr_m->MemberName==\"" << m->Name << "\")" << std::endl << "{" << std::endl;
        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee = m->Parameters.begin();
             ee != m->Parameters.end(); ++ee)
        {
            w2 << get_variable_type(*(*ee)).cpp_type << " " << fix_name((*ee)->Name) << "="
               << str_unpack_message_element("rr_m->FindElement(\"" + (*ee)->Name + "\")", (*ee)) << ";" << std::endl;
        }
        w2 << EventDeclaration(m.get(), false) << ";" << std::endl;
        w2 << "return;" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END()
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl;

        w2 << "void " << fix_name((*e)->Name)
           << "_stub::DispatchPipeMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m)" << std::endl
           << "{" << std::endl;
        // w2 << "boost::shared_lock<boost::shared_mutex> lock(context_lock);" << std::endl;
        MEMBER_ITER(PipeDefinition)
        w2 << "if (m->MemberName==\"" << m->Name << "\")" << std::endl << "{" << std::endl;
        w2 << "rrvar_" << m->Name << "->"
           << "PipePacketReceived(m);" << std::endl;
        w2 << "return;" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END()
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl;

        w2 << "void " << fix_name((*e)->Name)
           << "_stub::DispatchWireMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m)" << std::endl
           << "{" << std::endl;
        // w2 << "boost::shared_lock<boost::shared_mutex> lock(context_lock);" << std::endl;
        MEMBER_ITER(WireDefinition)
        w2 << "if (m->MemberName==\"" << m->Name << "\")" << std::endl << "{" << std::endl;
        w2 << "rrvar_" << m->Name << "->"
           << "WirePacketReceived(m);" << std::endl;
        w2 << "return;" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END()
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl;

        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>" << fix_name((*e)->Name)
           << "_stub::CallbackCall(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& rr_m)" << std::endl
           << "{" << std::endl;
        // w2 << "boost::shared_lock<boost::shared_mutex> lock(context_lock);" << std::endl;
        w2 << "RobotRaconteur::MessageStringPtr& ename=rr_m->MemberName;" << std::endl;
        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "rr_mr=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_CallbackCallRet, ename);"
           << std::endl;
        w2 << "rr_mr->ServicePath=rr_m->ServicePath;" << std::endl;
        w2 << "rr_mr->RequestID=rr_m->RequestID;" << std::endl;
        MEMBER_ITER(CallbackDefinition)
        w2 << "if (rr_m->MemberName==\"" << m->Name << "\")" << std::endl << "{" << std::endl;
        std::vector<std::string> cvarnames;
        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee = m->Parameters.begin();
             ee != m->Parameters.end(); ++ee)
        {
            w2 << get_variable_type(*(*ee)).cpp_type << " " << fix_name((*ee)->Name) << "="
               << str_unpack_message_element("rr_m->FindElement(\"" + (*ee)->Name + "\")", (*ee)) << ";" << std::endl;
            cvarnames.push_back(fix_name((*ee)->Name));
        }
        if (m->ReturnType->Type == DataTypes_void_t)
        {
            w2 << "rrvar_" << m->Name << "->GetFunction()(" << boost::join(cvarnames, ", ") << ")"
               << ";" << std::endl;
            w2 << "rr_mr->AddElement(\"return\",RobotRaconteur::ScalarToRRArray<int32_t>(0));" << std::endl;
        }
        else
        {
            w2 << get_variable_type(*m->ReturnType).cpp_type << " rr_ret=rrvar_" << m->Name << "->GetFunction()("
               << boost::join(cvarnames, ", ") << ")"
               << ";" << std::endl;
            w2 << "rr_mr->AddElement(" << str_pack_message_element("return", "rr_ret", m->ReturnType) << ");"
               << std::endl;
        }
        w2 << "return rr_mr;" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END()
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl;

        w2 << std::endl;
        w2 << "std::string " << fix_name((*e)->Name) << "_stub::RRType()" << std::endl << "{" << std::endl;
        w2 << "return \"" << d->Name << "." << (*e)->Name << "\";" << std::endl;
        w2 << "}" << std::endl;

        w2 << "void " << fix_name((*e)->Name) << "_stub::RRClose()" << std::endl << "{" << std::endl;
        MEMBER_ITER(PipeDefinition)
        w2 << "rrvar_" << m->Name << "->"
           << "Shutdown();" << std::endl;
        // w2 << "rrvar_" << m->Name << ".reset();" << std::endl;
        MEMBER_ITER_END()
        MEMBER_ITER(WireDefinition)
        w2 << "rrvar_" << m->Name << "->"
           << "Shutdown();" << std::endl;
        // w2 << "rrvar_" << m->Name << ".reset();" << std::endl;
        MEMBER_ITER_END()
        MEMBER_ITER(MemoryDefinition)
        w2 << "if (rrvar_" << m->Name << ")" << std::endl << "{" << std::endl;
        w2 << "rrvar_" << m->Name << "->"
           << "Shutdown();" << std::endl;
        // w2 << "rrvar_" << m->Name << ".reset();" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(EventDefinition)
        w2 << "rrvar_" << m->Name << "."
           << "disconnect_all_slots();" << std::endl;
        // w2 << "rrvar_" << m->Name << ".reset();" << std::endl;
        MEMBER_ITER_END()

        MEMBER_ITER(CallbackDefinition)
        w2 << "if (rrvar_" << m->Name << ")" << std::endl << "{" << std::endl;
        w2 << "rrvar_" << m->Name << "->Shutdown();" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END()

        w2 << "ServiceStub::RRClose();" << std::endl;
        w2 << "}" << std::endl << std::endl;

        w2 << "RR_SHARED_PTR<RobotRaconteur::PipeClientBase> " << fix_name((*e)->Name)
           << "_stub::RRGetPipeClient(boost::string_ref membername)" << std::endl
           << "{" << std::endl;
        MEMBER_ITER(PipeDefinition)
        w2 << "if (membername ==\"" << m->Name << "\") return rrvar_" << m->Name << ";" << std::endl;
        MEMBER_ITER_END()
        w2 << "return ServiceStub::RRGetPipeClient(membername);" << std::endl;
        w2 << "}" << std::endl << std::endl;

        w2 << "RR_SHARED_PTR<RobotRaconteur::WireClientBase> " << fix_name((*e)->Name)
           << "_stub::RRGetWireClient(boost::string_ref membername)" << std::endl
           << "{" << std::endl;
        MEMBER_ITER(WireDefinition)
        w2 << "if (membername ==\"" << m->Name << "\") return rrvar_" << m->Name << ";" << std::endl;
        MEMBER_ITER_END()
        w2 << "return ServiceStub::RRGetWireClient(membername);" << std::endl;
        w2 << "}" << std::endl << std::endl;

        // Asynchronous operations

        MEMBER_ITER(PropertyDefinition)
        get_variable_type_result t = get_variable_type(*m->Type);
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << boost::replace_last_copy(
                      dforc(GetPropertyDeclaration_async(m.get(), true), fix_name((*e)->Name) + "_stub"),
                      "rr_timeout=RR_TIMEOUT_INFINITE", "rr_timeout")
               << std::endl
               << "{" << std::endl;
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
                  "m=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertyGetReq,\""
               << fix_name(m->Name) << "\");" << std::endl;
            w2 << "AsyncProcessRequest(m,boost::bind(&" << fix_name((*e)->Name) << "_stub::rrend_get_"
               << fix_name(m->Name) << ", RobotRaconteur::rr_cast<" << fix_name((*e)->Name)
               << "_stub>(shared_from_this()),RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),rr_handler "
                  "),rr_timeout);"
               << std::endl;
            w2 << "}" << std::endl;
            w2 << "void " << fix_name((*e)->Name) << "_stub::rrend_get_" << fix_name(m->Name)
               << "(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                  "const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, boost::function< void ("
               << t.cpp_param_type << " ,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > handler)"
               << std::endl;
            w2 << "{" << std::endl;
            /*w2 << "if (err.value()==boost::system::errc::timed_out)" << std::endl;
            w2 << "{" << std::endl;
            w2 << "handler(" << GetDefaultValue(*m->Type) <<
            ",RR_MAKE_SHARED<RobotRaconteur::RequestTimeoutException>(\"Request Timeout\"));" << std::endl; w2 <<
            "return;"
            << std::endl; w2 << "}" << std::endl;*/
            w2 << "if (err)" << std::endl;
            w2 << "{" << std::endl;
            w2 << "handler(" << GetDefaultValue(*m->Type) << ",err);" << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            w2 << "if (m->Error != RobotRaconteur::MessageErrorType_None)" << std::endl;
            w2 << "{" << std::endl;
            w2 << "handler(" << GetDefaultValue(*m->Type)
               << ",RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));" << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            w2 << get_variable_type(*m->Type).cpp_type << " rr_ret;" << std::endl;
            w2 << "try" << std::endl;
            w2 << "{" << std::endl;
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"value\");" << std::endl;
            w2 << "rr_ret=" << str_unpack_message_element("me", m->Type) << ";" << std::endl;
            w2 << "}" << std::endl;
            w2 << "catch (std::exception& err2)" << std::endl << "{" << std::endl;
            w2 << "RobotRaconteur::detail::InvokeHandlerWithException(node, handler, err2, "
                  "RobotRaconteur::MessageErrorType_DataTypeError);"
               << std::endl;
            w2 << "}" << std::endl;
            /*w2 << "catch (RobotRaconteur::RobotRaconteurException& err)" << std::endl;
            w2 << "{" << std::endl;
            w2 << "handler(" << GetDefaultValue(*m->Type) <<
            ",RobotRaconteur::RobotRaconteurExceptionUtil::DownCastException(err));" << std::endl; w2 << "return;" <<
            std::endl; w2 << "}" << std::endl; w2 << "catch (std::exception& err)" << std::endl; w2 << "{" << std::endl;
            w2 << "handler(" << GetDefaultValue(*m->Type) <<
            ",RR_MAKE_SHARED<RobotRaconteur::RobotRaconteurRemoteException>(std::string(typeid(err).name()),err.what()));"
            << std::endl; w2 << "return;" << std::endl; w2 << "}" << std::endl;*/
            // w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"value\");" << std::endl;
            w2 << "handler(rr_ret, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());" << std::endl;
            w2 << "}" << std::endl;
        }
        if (m->Direction() != MemberDefinition_Direction_readonly)
        {
            w2 << boost::replace_last_copy(
                      dforc(SetPropertyDeclaration_async(m.get(), true), fix_name((*e)->Name) + "_stub"),
                      "rr_timeout=RR_TIMEOUT_INFINITE", "rr_timeout")
               << std::endl
               << "{" << std::endl;
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
                  "req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertySetReq,\""
               << fix_name(m->Name) << "\");" << std::endl;
            w2 << "req->AddElement(" << str_pack_message_element("value", "value", m->Type) << ");" << std::endl;
            w2 << "AsyncProcessRequest(req,boost::bind(&" << fix_name((*e)->Name) << "_stub::rrend_set_"
               << fix_name(m->Name) << ", RobotRaconteur::rr_cast<" << fix_name((*e)->Name)
               << "_stub>(shared_from_this()),RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),rr_handler "
                  "),rr_timeout);"
               << std::endl;
            w2 << "}" << std::endl;
            w2 << "void " << fix_name((*e)->Name) << "_stub::rrend_set_" << fix_name(m->Name)
               << "(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                  "const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, boost::function< void "
                  "(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > handler)"
               << std::endl;
            w2 << "{" << std::endl;
            /*w2 << "if (err.value()==boost::system::errc::timed_out)" << std::endl;
            w2 << "{" << std::endl;
            w2 << "handler(RR_MAKE_SHARED<RobotRaconteur::RequestTimeoutException>(\"Request Timeout\"));" << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;*/
            w2 << "if (err)" << std::endl;
            w2 << "{" << std::endl;
            w2 << "handler(err);" << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            w2 << "if (m->Error != RobotRaconteur::MessageErrorType_None)" << std::endl;
            w2 << "{" << std::endl;
            w2 << "handler(RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));" << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            // w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"value\");" << std::endl;
            w2 << "handler(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());" << std::endl;
            w2 << "}" << std::endl;
        }
        MEMBER_ITER_END()

        MEMBER_ITER(FunctionDefinition)
        w2 << boost::replace_last_copy(dforc(FunctionDeclaration_async(m.get(), true), fix_name((*e)->Name) + "_stub"),
                                       "rr_timeout=RR_TIMEOUT_INFINITE", "rr_timeout")
           << std::endl
           << "{" << std::endl;
        if (!m->IsGenerator())
        {
            get_variable_type_result t = get_variable_type(*m->ReturnType);
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
                  "rr_req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq,\""
               << m->Name << "\");" << std::endl;
            for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator ee = m->Parameters.begin();
                 ee != m->Parameters.end(); ++ee)
            {
                w2 << "rr_req->AddElement(" << str_pack_message_element((*ee)->Name, fix_name((*ee)->Name), (*ee))
                   << ");" << std::endl;
            }
            w2 << "AsyncProcessRequest(rr_req,boost::bind(&" << fix_name((*e)->Name) << "_stub::rrend_"
               << fix_name(m->Name) << ", RobotRaconteur::rr_cast<" << fix_name((*e)->Name)
               << "_stub>(shared_from_this()),RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),rr_handler "
                  "),rr_timeout);"
               << std::endl;

            w2 << "}" << std::endl << std::endl;

            if (m->ReturnType->Type == DataTypes_void_t)
            {

                w2 << "void " << fix_name((*e)->Name) << "_stub::rrend_" << fix_name(m->Name)
                   << "(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                      "const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, boost::function< void "
                      "(const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > handler)"
                   << std::endl;
            }
            else
            {
                w2 << "void " << fix_name((*e)->Name) << "_stub::rrend_" << fix_name(m->Name)
                   << "(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                      "const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, boost::function< void ("
                   << t.cpp_param_type << " ,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > handler)"
                   << std::endl;
            }

            w2 << "{" << std::endl;
            /*w2 << "if (err.value()==boost::system::errc::timed_out)" << std::endl;
            w2 << "{" << std::endl;
            if (m->ReturnType->Type==DataTypes_void_t)
            {
                w2 << "handler(RR_MAKE_SHARED<RobotRaconteur::RequestTimeoutException>(\"Request Timeout\"));" <<
            std::endl;
            }
            else
            {
                w2 << "handler(" << GetDefaultValue(*m->ReturnType) <<
            ",RR_MAKE_SHARED<RobotRaconteur::RequestTimeoutException>(\"Request Timeout\"));" << std::endl;
            }
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;*/
            w2 << "if (err)" << std::endl;
            w2 << "{" << std::endl;
            if (m->ReturnType->Type == DataTypes_void_t)
            {
                w2 << "handler(err);" << std::endl;
            }
            else
            {
                w2 << "handler(" << GetDefaultValue(*m->ReturnType) << ",err);" << std::endl;
            }
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            w2 << "if (m->Error != RobotRaconteur::MessageErrorType_None)" << std::endl;
            w2 << "{" << std::endl;
            if (m->ReturnType->Type == DataTypes_void_t)
            {
                w2 << "handler(RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));" << std::endl;
            }
            else
            {
                w2 << "handler(" << GetDefaultValue(*m->ReturnType)
                   << ",RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));" << std::endl;
            }
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;

            if (m->ReturnType->Type == DataTypes_void_t)
            {
                w2 << "handler(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());" << std::endl;
            }
            else
            {
                w2 << get_variable_type(*m->ReturnType).cpp_type << " rr_ret;" << std::endl;
                w2 << "try" << std::endl;
                w2 << "{" << std::endl;
                w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"return\");" << std::endl;
                w2 << "rr_ret=" << str_unpack_message_element("me", m->ReturnType) << ";" << std::endl;
                w2 << "}" << std::endl;
                w2 << "catch (std::exception& err2)" << std::endl << "{" << std::endl;
                w2 << "RobotRaconteur::detail::InvokeHandlerWithException(node, handler, err2, "
                      "RobotRaconteur::MessageErrorType_DataTypeError);"
                   << std::endl;
                w2 << "}" << std::endl;
                /*w2 << "catch (RobotRaconteur::RobotRaconteurException& err)" << std::endl;
                w2 << "{" << std::endl;
                w2 << "handler(" << GetDefaultValue(*m->ReturnType) <<
                ",RobotRaconteur::RobotRaconteurExceptionUtil::DownCastException(err));" << std::endl; w2 << "return;"
                << std::endl; w2 << "}" << std::endl; w2 << "catch (std::exception& err)" << std::endl; w2 << "{" <<
                std::endl; w2 << "handler(" << GetDefaultValue(*m->ReturnType) <<
                ",RR_MAKE_SHARED<RobotRaconteur::RobotRaconteurRemoteException>(std::string(typeid(err).name()),err.what()));"
                << std::endl; w2 << "return;" << std::endl; w2 << "}" << std::endl;*/
                // w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"value\");" << std::endl;
                w2 << "handler(rr_ret, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());" << std::endl;

                // w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"return\");" << std::endl;
                // w2 << "handler(" << str_unpack_message_element("me",m->ReturnType) << ",
                // RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());" << std::endl;
            }
        }
        else
        {
            convert_generator_result t = convert_generator(m.get());
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
                  "rr_req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq,\""
               << m->Name << "\");" << std::endl;
            for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator ee = t.params.begin();
                 ee != t.params.end(); ++ee)
            {
                w2 << "rr_req->AddElement(" << str_pack_message_element((*ee)->Name, fix_name((*ee)->Name), (*ee))
                   << ");" << std::endl;
            }
            w2 << "AsyncProcessRequest(rr_req,boost::bind(&" << fix_name((*e)->Name) << "_stub::rrend_"
               << fix_name(m->Name) << ", RobotRaconteur::rr_cast<" << fix_name((*e)->Name)
               << "_stub>(shared_from_this()),RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),rr_handler "
                  "),rr_timeout);"
               << std::endl;
            w2 << "}" << std::endl << std::endl;

            w2 << "void " << fix_name((*e)->Name) << "_stub::rrend_" << fix_name(m->Name)
               << "(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                  "const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, boost::function< void ("
               << t.generator_cpp_type << " ,const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>&) > handler)"
               << std::endl;
            w2 << "{" << std::endl;
            w2 << "if (err)" << std::endl;
            w2 << "{" << std::endl;
            w2 << "handler(" << t.generator_cpp_type << "(),err);" << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            w2 << "if (m->Error != RobotRaconteur::MessageErrorType_None)" << std::endl;
            w2 << "{" << std::endl;
            w2 << "handler(" << t.generator_cpp_type
               << "(),RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));" << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;

            w2 << "int32_t index;" << std::endl;
            w2 << "try" << std::endl;
            w2 << "{" << std::endl;
            w2 << "index=RobotRaconteur::RRArrayToScalar(m->FindElement(\"index\")->CastData<RobotRaconteur::RRArray<"
                  "int32_t> >());"
               << std::endl;
            w2 << "}" << std::endl;
            w2 << "catch (std::exception& err2)" << std::endl << "{" << std::endl;
            w2 << "RobotRaconteur::detail::InvokeHandlerWithException(node, handler, err2, "
                  "RobotRaconteur::MessageErrorType_DataTypeError);"
               << std::endl;
            w2 << "}" << std::endl;
            w2 << "handler(RR_MAKE_SHARED<RobotRaconteur::GeneratorClient<" << t.return_type << "," << t.param_type
               << " > >(\"" << m->Name
               << "\", index, shared_from_this()), RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());"
               << std::endl;
        }

        w2 << "}" << std::endl;

        MEMBER_ITER_END()

        MEMBER_ITER(ObjRefDefinition)
        w2 << boost::replace_all_copy(dforc(ObjRefDeclaration_async(m.get(), true), fix_name((*e)->Name) + "_stub"),
                                      "int32_t timeout=RR_TIMEOUT_INFINITE", "int32_t timeout")
           << std::endl
           << "{" << std::endl;
        std::string objecttype = fix_qualified_name(m->ObjectType);
        if (m->ObjectType == "varobject")
        {
            objecttype = "RobotRaconteur::RRObject";

            OBJREF_ARRAY_CONTAINER_CMD(
                m,
                w2 << "AsyncFindObjRef(\"" << m->Name
                   << "\", boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype
                   << " >,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),handler) ,timeout);" << std::endl,
                w2 << "AsyncFindObjRef(\"" << m->Name
                   << "\", boost::lexical_cast<std::string>(ind), "
                      "boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<"
                   << objecttype << " >,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),handler) ,timeout);"
                   << std::endl,
                w2 << "AsyncFindObjRef(\"" << m->Name
                   << "\", boost::lexical_cast<std::string>(ind), "
                      "boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<"
                   << objecttype << " >,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),handler) ,timeout);"
                   << std::endl,
                w2 << "AsyncFindObjRef(\"" << m->Name
                   << "\", ind, boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype
                   << " >,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),handler) ,timeout);" << std::endl)
        }
        else
        {
            std::string objecttype2;

            if (m->ObjectType.find('.') == std::string::npos)
            {
                objecttype2 = d->Name + "." + m->ObjectType;
            }
            else
            {
                objecttype2 = m->ObjectType;
            }

            OBJREF_ARRAY_CONTAINER_CMD(
                m,
                w2 << "AsyncFindObjRefTyped(\"" << m->Name << "\", \"" << objecttype2
                   << "\", boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype
                   << " >,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),handler) ,timeout);" << std::endl,
                w2 << "AsyncFindObjRefTyped(\"" << m->Name << "\", boost::lexical_cast<std::string>(ind),\""
                   << objecttype2 << "\", boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype
                   << " >,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),handler) ,timeout);" << std::endl,
                w2 << "AsyncFindObjRefTyped(\"" << m->Name << "\", boost::lexical_cast<std::string>(ind),\""
                   << objecttype2 << "\", boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype
                   << " >,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),handler) ,timeout);" << std::endl,
                w2 << "AsyncFindObjRefTyped(\"" << m->Name << "\", ind, \"" << objecttype2
                   << "\", boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype
                   << " >,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),handler) ,timeout);" << std::endl)
        }
        w2 << "}" << std::endl << std::endl;
        MEMBER_ITER_END()
    }
}

void CPPServiceLangGen::GenerateSkelDefinition(ServiceDefinition* d,
                                               const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                               std::ostream* w)
{
    RR_UNUSED(other_defs);
    std::ostream& w2 = *w;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        w2 << "void " << fix_name((*e)->Name)
           << "_skel::Init(boost::string_ref path, const RR_SHARED_PTR<RobotRaconteur::RRObject>& object, "
              "const RR_SHARED_PTR<RobotRaconteur::ServerContext>& context)"
           << std::endl
           << "{" << std::endl;
        w2 << "uncastobj=object;" << std::endl;
        w2 << "rr_InitPipeServersRun=false;" << std::endl;
        w2 << "rr_InitWireServersRun=false;" << std::endl;
        MEMBER_ITER(MemoryDefinition)
        w2 << "rr_" << m->Name << "_mem.reset();" << std::endl;
        MEMBER_ITER_END()
        w2 << "ServiceSkel::Init(path,object,context);" << std::endl;
        w2 << "}" << std::endl;
        w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
           << " > " << fix_name((*e)->Name) << "_skel::"
           << "get_obj()" << std::endl
           << "{" << std::endl;
        w2 << "return RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name), ".", "::")
           << "::" << fix_name((*e)->Name) << " >(uncastobj);" << std::endl;
        w2 << "}" << std::endl;

        w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::async_"
           << fix_name((*e)->Name) << " > " << fix_name((*e)->Name) << "_skel::"
           << "get_asyncobj()" << std::endl
           << "{" << std::endl;
        w2 << "return RR_DYNAMIC_POINTER_CAST<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::async_"
           << fix_name((*e)->Name) << " >(uncastobj);" << std::endl;
        w2 << "}" << std::endl;

        w2 << "void " << fix_name((*e)->Name) << "_skel::"
           << "ReleaseCastObject() " << std::endl
           << "{" << std::endl;
        MEMBER_ITER(PipeDefinition)
        w2 << "rr_" << m->Name << "_pipe->"
           << "Shutdown();" << std::endl;
        // w2 << "rr_" << m->Name << "_pipe.reset();" << std::endl;
        MEMBER_ITER_END()
        MEMBER_ITER(WireDefinition)
        w2 << "rr_" << m->Name << "_wire->"
           << "Shutdown();" << std::endl;
        // w2 << "rr_" << m->Name << "_wire.reset();" << std::endl;
        MEMBER_ITER_END()
        MEMBER_ITER(CallbackDefinition)
        w2 << "get_obj()->get_" << fix_name(m->Name) << "()->"
           << "Shutdown();" << std::endl;
        // w2 << "rr_" << m->Name << "_callback.reset();" << std::endl;
        MEMBER_ITER_END()
        w2 << "}" << std::endl;
        w2 << "std::string " << fix_name((*e)->Name) << "_skel::"
           << "GetObjectType()" << std::endl
           << "{" << std::endl
           << "return \"" << d->Name << "." << (*e)->Name << "\";" << std::endl
           << "}" << std::endl;

        // properties
        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name) << "_skel::"
           << "CallGetProperty(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m)" << std::endl
           << "{" << std::endl;
        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "mr=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertyGetRes,m->MemberName);"
           << std::endl;
        w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::"
           << "async_" << fix_name((*e)->Name) << " > async_obj=get_asyncobj();" << std::endl;
        MEMBER_ITER(PropertyDefinition)
        w2 << "if (m->MemberName == \"" << m->Name << "\")" << std::endl << "{" << std::endl;
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << "if (async_obj)" << std::endl << "{" << std::endl;
            w2 << "RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::")
               << "::" << fix_name((*e)->Name) << "_skel> wp=RobotRaconteur::rr_cast<"
               << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
               << "_skel>(shared_from_this());" << std::endl;
            w2 << "async_obj->async_get_" << fix_name(m->Name) << "(boost::bind(&"
               << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
               << "_skel::rr_get_" << fix_name(m->Name)
               << ",wp,RR_BOOST_PLACEHOLDERS(_1),RR_BOOST_PLACEHOLDERS(_2),m,RobotRaconteur::ServerEndpoint::"
                  "GetCurrentEndpoint()));"
               << std::endl;
            w2 << "return RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>();" << std::endl;
            w2 << "}" << std::endl;
            w2 << "else" << std::endl;
            w2 << "{" << std::endl;
            w2 << get_variable_type(*m->Type, true).cpp_type << " value=get_obj()->"
               << GetPropertyDeclaration(m.get(), false) << ";" << std::endl;
            w2 << "mr->AddElement(" << str_pack_message_element("value", "value", m->Type) << ");" << std::endl;
            w2 << "return mr;" << std::endl;
            w2 << "}" << std::endl;
        }
        else
        {
            w2 << "throw RobotRaconteur::WriteOnlyMemberException(\"Write only property\");" << std::endl;
        }
        w2 << "}" << std::endl;
        MEMBER_ITER_END()
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl << std::endl;

        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name) << "_skel::"
           << "CallSetProperty(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m)" << std::endl
           << "{" << std::endl;
        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "mr=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertySetRes,m->MemberName);"
           << std::endl;
        w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::"
           << "async_" << fix_name((*e)->Name) << " > async_obj=get_asyncobj();" << std::endl;
        MEMBER_ITER(PropertyDefinition)
        w2 << "if (m->MemberName == \"" << m->Name << "\")" << std::endl << "{" << std::endl;
        if (m->Direction() != MemberDefinition_Direction_readonly)
        {
            w2 << get_variable_type(*m->Type, true).cpp_type
               << " value=" << str_unpack_message_element("m->FindElement(\"value\")", m->Type) << ";" << std::endl;
            w2 << "if (async_obj)" << std::endl << "{" << std::endl;
            w2 << "RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::")
               << "::" << fix_name((*e)->Name) << "_skel> wp=RobotRaconteur::rr_cast<"
               << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
               << "_skel>(shared_from_this());" << std::endl;
            w2 << "async_obj->async_set_" << fix_name(m->Name)
               << "(value,boost::bind(&RobotRaconteur::ServiceSkel::EndAsyncCallSetProperty,wp,RR_BOOST_PLACEHOLDERS(_"
                  "1),m,RobotRaconteur::ServerEndpoint::GetCurrentEndpoint()));"
               << std::endl;
            w2 << "return RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>();" << std::endl;
            w2 << "}" << std::endl;
            w2 << "else" << std::endl;
            w2 << "{" << std::endl;
            w2 << "get_obj()->" << SetPropertyDeclaration(m.get(), false) << ";" << std::endl;
            w2 << "return mr;" << std::endl;
            w2 << "}" << std::endl;
        }
        else
        {
            w2 << "throw RobotRaconteur::ReadOnlyMemberException(\"Read only property\");" << std::endl;
        }
        w2 << "}" << std::endl;
        MEMBER_ITER_END()
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl << std::endl;

        MEMBER_ITER(PropertyDefinition)
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << "void " << fix_name((*e)->Name) << "_skel::rr_get_" << fix_name(m->Name) << "(RR_WEAK_PTR<"
               << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
               << "_skel> skel," << get_variable_type(*m->Type, true).cpp_type
               << " value, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, "
                  "const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, const "
                  "RR_SHARED_PTR<RobotRaconteur::ServerEndpoint>& ep)"
               << std::endl
               << "{" << std::endl;
            w2 << "if(err)" << std::endl << "{" << std::endl;
            w2 << "EndAsyncCallGetProperty(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),err,m, ep);"
               << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> mr;" << std::endl;
            w2 << "try" << std::endl << "{" << std::endl;
            w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::")
               << "::" << fix_name((*e)->Name) << "_skel> skel1=skel.lock();" << std::endl;
            w2 << "if (!skel1) throw RobotRaconteur::InvalidOperationException(\"skel release\");" << std::endl;
            w2 << "mr="
               << boost::replace_all_copy(str_pack_message_element("value", "value", m->Type), "RRGetNodeWeak()",
                                          "skel1->RRGetNodeWeak()")
               << ";" << std::endl;
            w2 << "}" << std::endl;
            w2 << "catch (std::exception& err2)" << std::endl << "{" << std::endl;
            w2 << "EndAsyncCallGetProperty(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RobotRaconteur::"
                  "RobotRaconteurExceptionUtil::ExceptionToSharedPtr(err2, "
                  "RobotRaconteur::MessageErrorType_DataTypeError),m, ep);"
               << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            w2 << "EndAsyncCallGetProperty(skel, mr, err, m,ep);" << std::endl;
            /*w2 << "catch (RobotRaconteur::RobotRaconteurException& err2)" << std::endl << "{" << std::endl;
            w2 <<
            "EndAsyncCallGetProperty(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RobotRaconteur::RobotRaconteurExceptionUtil::DownCastException(err2),m,
            ep);" << std::endl; w2 << "}" << std::endl; w2 << "catch (std::exception& err2)" << std::endl << "{" <<
            std::endl; w2 <<
            "EndAsyncCallGetProperty(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RR_MAKE_SHARED<RobotRaconteur::DataTypeException>(err2.what()),m,
            ep);" << std::endl; w2 << "}" << std::endl;*/
            w2 << "}" << std::endl;
        }
        MEMBER_ITER_END()

        // functions
        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name) << "_skel::"
           << "CallFunction(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& rr_m)" << std::endl
           << "{" << std::endl;
        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "rr_mr=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallRes,rr_m->"
              "MemberName);"
           << std::endl;
        w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::"
           << "async_" << fix_name((*e)->Name) << " > async_obj=get_asyncobj();" << std::endl;
        MEMBER_ITER(FunctionDefinition)
        w2 << "if (rr_m->MemberName == \"" << m->Name << "\")" << std::endl << "{" << std::endl;
        if (!m->IsGenerator())
        {
            std::vector<std::string> v1;

            for (std::vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee = m->Parameters.begin();
                 ee != m->Parameters.end(); ++ee)
            {
                w2 << get_variable_type(*(*ee), true).cpp_type << " " << fix_name((*ee)->Name) << " ="
                   << str_unpack_message_element("rr_m->FindElement(\"" + (*ee)->Name + "\")", (*ee)) << ";"
                   << std::endl;
                v1.push_back(fix_name((*ee)->Name));
            }

            if (m->ReturnType->Type == DataTypes_void_t)
            {
                v1.push_back(
                    "boost::bind(&" + boost::replace_all_copy(fix_name(d->Name), ".", "::") +
                    "::" + fix_name((*e)->Name) + "_skel::rr_" + fix_name(m->Name) +
                    ",rr_wp, RR_BOOST_PLACEHOLDERS(_1), rr_m, RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())");
            }
            else
            {
                v1.push_back("boost::bind(&" + boost::replace_all_copy(fix_name(d->Name), ".", "::") +
                             "::" + fix_name((*e)->Name) + "_skel::rr_" + fix_name(m->Name) +
                             ", rr_wp, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), rr_m, "
                             "RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())");
            }

            w2 << "if (async_obj)" << std::endl << "{" << std::endl;
            w2 << "RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::")
               << "::" << fix_name((*e)->Name) << "_skel> rr_wp=RobotRaconteur::rr_cast<"
               << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
               << "_skel>(shared_from_this());" << std::endl;
            w2 << "async_obj->async_" << fix_name(m->Name) << "(" << boost::join(v1, ", ") << ");" << std::endl;
            w2 << "return RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>();" << std::endl;
            w2 << "}" << std::endl;
            w2 << "else" << std::endl;
            w2 << "{" << std::endl;
            if (m->ReturnType->Type != DataTypes_void_t)
            {
                w2 << get_variable_type(*m->ReturnType).cpp_type << " rr_return=get_obj()->"
                   << FunctionDeclaration(m.get(), false) << ";" << std::endl;
                w2 << "rr_mr->AddElement(" << str_pack_message_element("return", "rr_return", m->ReturnType) << ");"
                   << std::endl;
            }
            else
            {
                w2 << "get_obj()->" << FunctionDeclaration(m.get(), false) << ";" << std::endl;
                w2 << "rr_mr->AddElement(\"return\",RobotRaconteur::ScalarToRRArray<int32_t>(0));" << std::endl;
            }
            w2 << "return rr_mr;" << std::endl;
            w2 << "}" << std::endl;
        }
        else
        {
            std::vector<std::string> v1;

            convert_generator_result t = convert_generator(m.get());

            for (std::vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee = t.params.begin(); ee != t.params.end();
                 ++ee)
            {
                w2 << get_variable_type(*(*ee), true).cpp_type << " " << fix_name((*ee)->Name) << " ="
                   << str_unpack_message_element("rr_m->FindElement(\"" + (*ee)->Name + "\")", (*ee)) << ";"
                   << std::endl;
                v1.push_back(fix_name((*ee)->Name));
            }

            /*if (m->ReturnType->Type == DataTypes_void_t)
            {
                v1.push_back("boost::bind(&" + boost::replace_all_copy(fix_name(d->Name), ".", "::") + "::" +
            fix_name((*e)->Name) + "_skel::rr_" + fix_name(m->Name) + ",rr_wp, RR_BOOST_PLACEHOLDERS(_1), rr_m,
            RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())");

            }
            else
            {
                v1.push_back("boost::bind(&" + boost::replace_all_copy(fix_name(d->Name), ".", "::") + "::" +
            fix_name((*e)->Name) + "_skel::rr_" + fix_name(m->Name) + ", rr_wp, RR_BOOST_PLACEHOLDERS(_1),
            RR_BOOST_PLACEHOLDERS(_2), rr_m, RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())");
            }

            w2 << "if (async_obj)" << std::endl << "{" << std::endl;
            w2 << "RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" <<
            fix_name((*e)->Name) << "_skel> rr_wp=RobotRaconteur::rr_cast<" <<
            boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) <<
            "_skel>(shared_from_this());" << std::endl; w2 << "async_obj->async_" << fix_name(m->Name) << "(" <<
            boost::join(v1, ", ") << ");" << std::endl; w2 << "return RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>();"
            << std::endl; w2 << "}" << std::endl; w2 << "else" << std::endl;*/
            w2 << "{" << std::endl;

            w2 << t.generator_cpp_type << " rr_return=get_obj()->" << FunctionDeclaration(m.get(), false) << ";"
               << std::endl;
            w2 << "int32_t rr_index = 0;" << std::endl;
            w2 << "{" << std::endl;
            w2 << "boost::mutex::scoped_lock lock(generators_lock);" << std::endl;
            w2 << "rr_index = get_new_generator_index();" << std::endl;
            w2 << "generators.insert(std::make_pair(rr_index,RR_MAKE_SHARED<RobotRaconteur::GeneratorServer<"
               << t.return_type << "," << t.param_type << " > >(rr_return, \"" << m->Name
               << "\",rr_index, shared_from_this(), RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())));"
               << std::endl;
            w2 << "}" << std::endl;
            w2 << "rr_mr->AddElement(\"index\", RobotRaconteur::ScalarToRRArray(rr_index));" << std::endl;

            w2 << "return rr_mr;" << std::endl;
            w2 << "}" << std::endl;
        }
        w2 << "}" << std::endl;
        MEMBER_ITER_END()
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl << std::endl;

        MEMBER_ITER(FunctionDefinition)
        if (!m->IsGenerator())
        {
            std::vector<std::string> v1;
            if (m->ReturnType->Type != DataTypes_void_t)
            {
                v1.push_back(get_variable_type(*m->ReturnType, true).cpp_type + " ret");
            }
            v1.push_back("const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, "
                         "const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                         "RR_SHARED_PTR<RobotRaconteur::ServerEndpoint> ep");
            w2 << "void " << fix_name((*e)->Name) << "_skel::rr_" << fix_name(m->Name) << "(RR_WEAK_PTR<"
               << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
               << "_skel> skel, " << boost::join(v1, ", ") << ")" << std::endl
               << "{" << std::endl;
            w2 << "if(err)" << std::endl << "{" << std::endl;
            w2 << "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),err,m, ep);"
               << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> mr;" << std::endl;
            w2 << "try" << std::endl << "{" << std::endl;
            if (m->ReturnType->Type != DataTypes_void_t)
            {
                w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::")
                   << "::" << fix_name((*e)->Name) << "_skel> skel1=skel.lock();" << std::endl;
                w2 << "if (!skel1) throw RobotRaconteur::InvalidOperationException(\"skel release\");" << std::endl;

                w2 << "mr="
                   << boost::replace_all_copy(str_pack_message_element("return", "ret", m->ReturnType),
                                              "RRGetNodeWeak()", "skel1->RRGetNodeWeak()")
                   << ";" << std::endl;
            }
            else
            {
                w2 << "mr=RobotRaconteur::CreateMessageElement(\"return\",RobotRaconteur::ScalarToRRArray<int32_t>(0));"
                   << std::endl;
            }
            w2 << "}" << std::endl;
            w2 << "catch (std::exception& err2)" << std::endl << "{" << std::endl;
            w2 << "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RobotRaconteur::"
                  "RobotRaconteurExceptionUtil::ExceptionToSharedPtr(err2, "
                  "RobotRaconteur::MessageErrorType_DataTypeError),m, ep);"
               << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            w2 << "EndAsyncCallFunction(skel, mr, err, m,ep);" << std::endl; // TODO: move this
            /*w2 << "catch (RobotRaconteur::RobotRaconteurException& err2)" << std::endl << "{" << std::endl;
            w2 <<
            "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RobotRaconteur::RobotRaconteurExceptionUtil::DownCastException(err2),m,
            ep);" << std::endl; w2 << "}" << std::endl; w2 << "catch (std::exception& err2)" << std::endl << "{" <<
            std::endl; w2 <<
            "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RR_MAKE_SHARED<RobotRaconteur::DataTypeException>(err2.what()),m,
            ep);" << std::endl; w2 << "}" << std::endl;*/
            w2 << "}" << std::endl;
        }
        else
        {
            convert_generator_result t = convert_generator(m.get());
            w2 << "void " << fix_name((*e)->Name) << "_skel::rr_" << fix_name(m->Name) << "(RR_WEAK_PTR<"
               << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
               << "_skel> skel, " << t.generator_cpp_type
               << " ret, const RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>& err, "
                  "const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
                  "RR_SHARED_PTR<RobotRaconteur::ServerEndpoint> ep"
               << ")" << std::endl
               << "{" << std::endl;
            w2 << "if(err)" << std::endl << "{" << std::endl;
            w2 << "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),err,m, ep);"
               << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> mr;" << std::endl;
            w2 << "try" << std::endl << "{" << std::endl;

            w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::")
               << "::" << fix_name((*e)->Name) << "_skel> skel1=skel.lock();" << std::endl;
            w2 << "if (!skel1) throw RobotRaconteur::InvalidOperationException(\"skel release\");" << std::endl;
            w2 << "int32_t rr_index = 0;" << std::endl;
            w2 << "{" << std::endl;
            w2 << "boost::mutex::scoped_lock lock(skel1->generators_lock);" << std::endl;
            w2 << "rr_index = skel1->get_new_generator_index();" << std::endl;
            w2 << "skel1->generators.insert(std::make_pair(rr_index,RR_MAKE_SHARED<RobotRaconteur::GeneratorServer<"
               << t.return_type << "," << t.param_type << " > >(ret, \"" << m->Name
               << "\",rr_index, skel1, RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())));" << std::endl;
            w2 << "}" << std::endl;
            w2 << "mr = RobotRaconteur::CreateMessageElement(\"index\",RobotRaconteur::ScalarToRRArray(rr_index));"
               << std::endl;
            w2 << "}" << std::endl;
            w2 << "catch (std::exception& err2)" << std::endl << "{" << std::endl;
            w2 << "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RobotRaconteur::"
                  "RobotRaconteurExceptionUtil::ExceptionToSharedPtr(err2, "
                  "RobotRaconteur::MessageErrorType_DataTypeError),m, ep);"
               << std::endl;
            w2 << "return;" << std::endl;
            w2 << "}" << std::endl;
            w2 << "EndAsyncCallFunction(skel, mr, err, m,ep);" << std::endl;
            w2 << "}" << std::endl;
        }
        MEMBER_ITER_END()

        // events
        MEMBER_ITER(EventDefinition)
        std::vector<std::string> params;
        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee = m->Parameters.begin();
             ee != m->Parameters.end(); ++ee)
            params.push_back(get_variable_type(*(*ee)).cpp_type + " " + fix_name((*ee)->Name));
        w2 << "void " << fix_name((*e)->Name) << "_skel::rr_" << fix_name(m->Name) << "_Handler("
           << boost::join(params, ", ") << ")" << std::endl
           << "{" << std::endl;
        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "rr_mm=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_EventReq,\""
           << m->Name << "\");" << std::endl;
        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee = m->Parameters.begin();
             ee != m->Parameters.end(); ++ee)
        {
            w2 << "rr_mm->AddElement(" << str_pack_message_element((*ee)->Name, fix_name((*ee)->Name), *ee) << ");"
               << std::endl;
        }
        w2 << "SendEvent(rr_mm);" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END()

        w2 << "void " << fix_name((*e)->Name)
           << "_skel::RegisterEvents(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1)" << std::endl
           << "{" << std::endl;
        w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
           << " > obj=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name), ".", "::")
           << "::" << fix_name((*e)->Name) << " >(rrobj1);" << std::endl;
        MEMBER_ITER(EventDefinition)
        std::vector<std::string> p;
        p.push_back("&" + fix_name((*e)->Name) + "_skel::rr_" + m->Name + "_Handler");
        p.push_back("RobotRaconteur::rr_cast<" + fix_name((*e)->Name) + "_skel>(shared_from_this())");
        for (size_t i = 0; i < m->Parameters.size(); i++)
        {
            p.push_back("RR_BOOST_PLACEHOLDERS(_" + boost::lexical_cast<std::string>(i + 1) + ")");
        }

        w2 << m->Name << "_rrconnection=obj->get_" << fix_name(m->Name) << "().connect(boost::bind("
           << boost::join(p, ", ") << "));" << std::endl;
        MEMBER_ITER_END()
        w2 << "RobotRaconteur::ServiceSkel::RegisterEvents(rrobj1);" << std::endl;
        w2 << "}" << std::endl << std::endl;

        w2 << "void " << fix_name((*e)->Name)
           << "_skel::UnregisterEvents(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1)" << std::endl
           << "{" << std::endl;
        MEMBER_ITER(EventDefinition)
        w2 << m->Name << "_rrconnection.disconnect();" << std::endl;
        MEMBER_ITER_END()
        w2 << "RobotRaconteur::ServiceSkel::UnregisterEvents(rrobj1);" << std::endl;
        w2 << "}" << std::endl << std::endl;

        // objrefs
        w2 << "RR_SHARED_PTR<RobotRaconteur::RRObject> " << fix_name((*e)->Name)
           << "_skel::GetSubObj(boost::string_ref name, boost::string_ref ind)" << std::endl
           << "{" << std::endl;
        MEMBER_ITER(ObjRefDefinition)
        w2 << "if (name==\"" << m->Name << "\")" << std::endl << "{" << std::endl;
        OBJREF_ARRAY_CONTAINER_CMD(m,
                                   w2 << "return RobotRaconteur::rr_cast<RobotRaconteur::RRObject>(get_obj()->get_"
                                      << fix_name(m->Name) << "());" << std::endl,
                                   w2 << "return RobotRaconteur::rr_cast<RobotRaconteur::RRObject>(get_obj()->get_"
                                      << fix_name(m->Name) << "(boost::lexical_cast<int32_t>(ind)));" << std::endl,
                                   w2 << "return RobotRaconteur::rr_cast<RobotRaconteur::RRObject>(get_obj()->get_"
                                      << fix_name(m->Name) << "(boost::lexical_cast<int32_t>(ind)));" << std::endl,
                                   w2 << "return RobotRaconteur::rr_cast<RobotRaconteur::RRObject>(get_obj()->get_"
                                      << fix_name(m->Name) << "(ind.to_string()));" << std::endl)

        w2 << "}" << std::endl;
        MEMBER_ITER_END()
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl << std::endl;

        // Pipes

        MEMBER_ITER(PipeDefinition)
        if (CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
        {
            w2 << "static void " << fix_name((*e)->Name) << "_skel_rrverify_" << m->Name
               << "(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)" << std::endl
               << "{" << std::endl;
            w2 << CPPServiceLangGen_VerifyArrayLength(
                      *m->Type, "RobotRaconteur::rr_cast<" +
                                    remove_RR_INTRUSIVE_PTR(get_variable_type(*m->Type).cpp_type) + " >(value)")
               << ";" << std::endl;
            w2 << "}" << std::endl;
        }
        MEMBER_ITER_END()

        w2 << "void " << fix_name((*e)->Name)
           << "_skel::InitPipeServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1)" << std::endl
           << "{" << std::endl;
        w2 << "if (rr_InitPipeServersRun) return;" << std::endl;
        w2 << "rr_InitPipeServersRun=true;" << std::endl;
        w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
           << " > obj=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name), ".", "::")
           << "::" << fix_name((*e)->Name) << " >(rrobj1);" << std::endl;
        MEMBER_ITER(PipeDefinition)
        std::string unreliable_str = CPPServiceLangGen_unreliable_str(m->IsUnreliable());
        std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
        if (!CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
        {
            w2 << "rr_" << m->Name << "_pipe=RR_MAKE_SHARED<RobotRaconteur::PipeServer<"
               << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(),"
               << unreliable_str << "," << direction_str << ");" << std::endl;
        }
        else
        {
            w2 << "rr_" << m->Name << "_pipe=RR_MAKE_SHARED<RobotRaconteur::PipeServer<"
               << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(),"
               << unreliable_str << "," << direction_str << ",&" << fix_name((*e)->Name) << "_skel_rrverify_" << m->Name
               << ");" << std::endl;
        }
        MEMBER_ITER_END()
        MEMBER_ITER(PipeDefinition)
        w2 << "obj->set_" << fix_name(m->Name) << "(rr_" << m->Name << "_pipe);" << std::endl;
        MEMBER_ITER_END()
        w2 << "}" << std::endl << std::endl;

        w2 << "void " << fix_name((*e)->Name)
           << "_skel::DispatchPipeMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e)"
           << std::endl
           << "{" << std::endl;
        MEMBER_ITER(PipeDefinition)
        w2 << "if (m->MemberName==\"" << m->Name << "\")" << std::endl << "{" << std::endl;
        w2 << "rr_" << m->Name << "_pipe->PipePacketReceived(m,e);" << std::endl;
        w2 << "return;" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END();
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl << std::endl;

        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name)
           << "_skel::CallPipeFunction(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e)"
           << std::endl
           << "{" << std::endl;
        MEMBER_ITER(PipeDefinition)
        w2 << "if (m->MemberName==\"" << m->Name << "\")" << std::endl << "{" << std::endl;
        w2 << "return rr_" << m->Name << "_pipe->PipeCommand(m,e);" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END();
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl << std::endl;

        // wires

        MEMBER_ITER(WireDefinition)
        if (CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
        {
            w2 << "static void " << fix_name((*e)->Name) << "_skel_rrverify_" << m->Name
               << "(const RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)" << std::endl
               << "{" << std::endl;
            w2 << CPPServiceLangGen_VerifyArrayLength(
                      *m->Type, "RobotRaconteur::rr_cast<" +
                                    remove_RR_INTRUSIVE_PTR(get_variable_type(*m->Type).cpp_type) + " >(value)")
               << ";" << std::endl;
            w2 << "}" << std::endl;
        }
        MEMBER_ITER_END()

        w2 << "void " << fix_name((*e)->Name)
           << "_skel::InitWireServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& rrobj1)" << std::endl
           << "{" << std::endl;
        w2 << "if (rr_InitWireServersRun) return;" << std::endl;
        w2 << "rr_InitWireServersRun=true;" << std::endl;
        w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
           << " > obj=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name), ".", "::")
           << "::" << fix_name((*e)->Name) << " >(rrobj1);" << std::endl;
        MEMBER_ITER(WireDefinition)
        std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
        if (!CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
        {
            w2 << "rr_" << m->Name << "_wire=RR_MAKE_SHARED<RobotRaconteur::WireServer<"
               << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(),"
               << direction_str << ");" << std::endl;
        }
        else
        {
            w2 << "rr_" << m->Name << "_wire=RR_MAKE_SHARED<RobotRaconteur::WireServer<"
               << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(),"
               << direction_str << ",&" << fix_name((*e)->Name) << "_skel_rrverify_" << m->Name << ");" << std::endl;
        }
        MEMBER_ITER_END()
        MEMBER_ITER(WireDefinition)
        w2 << "obj->set_" << fix_name(m->Name) << "(rr_" << m->Name << "_wire);" << std::endl;
        MEMBER_ITER_END()
        w2 << "}" << std::endl << std::endl;

        w2 << "void " << fix_name((*e)->Name)
           << "_skel::DispatchWireMessage(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e)"
           << std::endl
           << "{" << std::endl;
        MEMBER_ITER(WireDefinition)
        w2 << "if (m->MemberName==\"" << m->Name << "\")" << std::endl << "{" << std::endl;
        w2 << "rr_" << m->Name << "_wire->WirePacketReceived(m,e);" << std::endl;
        w2 << "return;" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END();
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl << std::endl;

        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name)
           << "_skel::CallWireFunction(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, uint32_t e)"
           << std::endl
           << "{" << std::endl;
        MEMBER_ITER(WireDefinition)
        w2 << "if (m->MemberName==\"" << m->Name << "\")" << std::endl << "{" << std::endl;
        w2 << "return rr_" << m->Name << "_wire->WireCommand(m,e);" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END();
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl << std::endl;

        w2 << "void " << fix_name((*e)->Name)
           << "_skel::InitCallbackServers(const RR_SHARED_PTR<RobotRaconteur::RRObject>& o)" << std::endl
           << "{" << std::endl;
        w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name)
           << " > obj=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name), ".", "::")
           << "::" << fix_name((*e)->Name) << " >(o);" << std::endl;
        MEMBER_ITER(CallbackDefinition)
        w2 << "obj->set_" << fix_name(m->Name) << "(RR_MAKE_SHARED<RobotRaconteur::CallbackServer<"
           << GetCallbackDeclaration(m.get(), true, true) << " > >(\"" << m->Name << "\",shared_from_this()));"
           << std::endl;
        MEMBER_ITER_END()
        w2 << "}" << std::endl;
        MEMBER_ITER(CallbackDefinition)
        std::vector<std::string> p;
        p.push_back("uint32_t rrendpoint");
        if (!m->Parameters.empty())
            p.push_back(str_pack_parameters(m->Parameters));
        w2 << get_variable_type(*m->ReturnType).cpp_type << " " << fix_name((*e)->Name) << "_skel::rr_" << m->Name
           << "_callback(" << boost::join(p, ", ") << ")"
           << "{" << std::endl;
        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> "
              "rr_req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_CallbackCallReq,\""
           << m->Name << "\");" << std::endl;
        w2 << "rr_req->ServicePath=GetServicePath();" << std::endl;
        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator ee = m->Parameters.begin();
             ee != m->Parameters.end(); ++ee)
        {
            w2 << "rr_req->AddElement(" << str_pack_message_element((*ee)->Name, fix_name((*ee)->Name), (*ee)) << ");"
               << std::endl;
        }
        w2 << "RR_SHARED_PTR<RobotRaconteur::ServerContext> rr_s=GetContext();" << std::endl;
        w2 << "if (rr_s==0) throw RobotRaconteur::InvalidOperationException(\"Service has been closed\");" << std::endl;
        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_ret=rr_s->ProcessCallbackRequest(rr_req,rrendpoint);"
           << std::endl;
        if (m->ReturnType->Type != DataTypes_void_t)
        {
            w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> rr_me=rr_ret->FindElement(\"return\");"
               << std::endl;
            w2 << "return " << str_unpack_message_element("rr_me", m->ReturnType) << ";" << std::endl;
        }
        w2 << "}" << std::endl << std::endl;
        MEMBER_ITER_END()

        w2 << "RR_SHARED_PTR<void> " << fix_name((*e)->Name)
           << "_skel::GetCallbackFunction(uint32_t endpoint, boost::string_ref membername)" << std::endl
           << "{" << std::endl;
        MEMBER_ITER(CallbackDefinition)
        w2 << "if (membername==\"" << m->Name << "\")" << std::endl << "{" << std::endl;
        std::vector<std::string> p;
        p.push_back("&" + fix_name((*e)->Name) + "_skel::rr_" + m->Name + "_callback");
        p.push_back("RobotRaconteur::rr_cast<" + fix_name((*e)->Name) + "_skel>(shared_from_this())");
        p.push_back("endpoint");
        for (size_t i = 0; i < m->Parameters.size(); i++)
        {
            p.push_back("RR_BOOST_PLACEHOLDERS(_" + boost::lexical_cast<std::string>(i + 1) + ")");
        }
        w2 << "return RR_MAKE_SHARED<" << GetCallbackDeclaration(m.get(), true, true) << " >(boost::bind("
           << boost::join(p, ", ") << "));" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END()
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl << std::endl;

        w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name)
           << "_skel::CallMemoryFunction(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m, "
              "const RR_SHARED_PTR<RobotRaconteur::Endpoint>& e)"
           << std::endl
           << "{" << std::endl;

        MEMBER_ITER(MemoryDefinition)
        w2 << "if (m->MemberName==\"" << m->Name << "\")" << std::endl << "{" << std::endl;
        w2 << "if (rr_" << m->Name << "_mem==0) ";

        convert_type_result t = convert_type(*m->Type);
        t.name = m->Name;
        std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
        if (IsTypeNumeric(m->Type->Type))
        {
            if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
            {
                w2 << "rr_" << t.name << "_mem=RR_MAKE_SHARED<RobotRaconteur::ArrayMemoryServiceSkel<" << t.cpp_type
                   << " > >(\"" << m->Name << "\",shared_from_this()," << direction_str << ");" << std::endl;
            }
            else
            {
                w2 << "rr_" << t.name << "_mem=RR_MAKE_SHARED<RobotRaconteur::MultiDimArrayMemoryServiceSkel<"
                   << t.cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << direction_str << ");"
                   << std::endl;
            }
        }
        else
        {
            std::string c = "Pod";
            size_t elem_size = 0;
            if (m->Type->ResolveNamedType()->RRDataType() == DataTypes_namedarray_t)
            {
                c = "Named";

                boost::tuple<DataTypes, size_t> namedarray_t =
                    GetNamedArrayElementTypeAndCount(rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType()));
                elem_size = namedarray_t.get<1>();
            }
            else
            {
                elem_size = EstimatePodPackedElementSize(rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType()));
            }

            if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
            {
                w2 << "rr_" << t.name << "_mem=RR_MAKE_SHARED<RobotRaconteur::" << c << "ArrayMemoryServiceSkel<"
                   << t.cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << elem_size << ","
                   << direction_str << ");" << std::endl;
            }
            else
            {
                w2 << "rr_" + t.name << "_mem=RR_MAKE_SHARED<RobotRaconteur::" << c << "MultiDimArrayMemoryServiceSkel<"
                   << t.cpp_type + " > >(\"" << m->Name << "\",shared_from_this()," << elem_size << "," << direction_str
                   << ");" << std::endl;
            }
        }
        w2 << "return rr_" << m->Name << "_mem->CallMemoryFunction(m,e,get_obj()->get_" << fix_name(m->Name) << "());"
           << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER_END()
        w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << std::endl;
        w2 << "}" << std::endl;

        w2 << "bool " << fix_name((*e)->Name)
           << "_skel::IsRequestNoLock(const RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>& m)" << std::endl
           << "{" << std::endl;
        BOOST_FOREACH (const RR_SHARED_PTR<MemberDefinition>& m, (*e)->Members)
        {
            if (m->NoLock() == MemberDefinition_NoLock_all)
            {
                w2 << "if (m->MemberName == \"" << m->Name << "\")" << std::endl << "  return true;" << std::endl;
            }

            if (m->NoLock() == MemberDefinition_NoLock_read)
            {
                RR_SHARED_PTR<PropertyDefinition> m1 = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(m);
                if (m1)
                {
                    w2 << "if (m->MemberName == \"" << m->Name
                       << "\" && m->EntryType == RobotRaconteur::MessageEntryType_PropertyGetReq)" << std::endl
                       << "  return true;" << std::endl;
                }

                RR_SHARED_PTR<MemoryDefinition> m2 = RR_DYNAMIC_POINTER_CAST<MemoryDefinition>(m);
                if (m2)
                {
                    w2 << "if (m->MemberName == \"" << m->Name
                       << "\" && (m->EntryType == RobotRaconteur::MessageEntryType_MemoryRead || m->EntryType == "
                          "RobotRaconteur::MessageEntryType_MemoryGetParam))"
                       << std::endl
                       << "  return true;" << std::endl;
                }
            }
        }
        w2 << "return false;" << std::endl;
        w2 << "}" << std::endl;
    }
}

template <typename T>
static void null_deleter(T*)
{}

static std::string CPPServiceLang_convert_constant_EscapeString_Formatter(const boost::smatch& match)
{
    std::string i = match[0].str();

    if (i == "\"")
        return "\\\"";
    if (i == "\\")
        return "\\\\";
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

    std::string& v = i;

    std::stringstream v2;
    v2 << std::hex << std::setfill('0');
    BOOST_FOREACH (const uint8_t& v3, v)
    {
        uint32_t v4 = v3;
        v2 << std::setw(0) << "\\x" << std::setw(2) << v4;
    }

    return v2.str();
}

std::string CPPServiceLangGen::convert_constant(ConstantDefinition* c,
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

    convert_type_result c1 = convert_type(*t);
    if (t->Type == DataTypes_string_t)
    {
        const std::string v1 = c->ValueToString();
        boost::regex r_replace("(\"|\\\\|[\\x00-\\x1F]|\\x7F|[\\x80-\\xFF]+)");

        std::ostringstream t2(std::ios::out | std::ios::binary);
        std::ostream_iterator<char, char> oi(t2);

        boost::regex_replace(oi, v1.begin(), v1.end(), r_replace,
                             CPPServiceLang_convert_constant_EscapeString_Formatter,
                             boost::match_default | boost::format_all);

        return "static const char* " + fix_name(c->Name) + "=\"" + t2.str() + "\";";
    }

    if (t->Type == DataTypes_namedtype_t)
    {
        std::vector<ConstantDefinition_StructField> f = c->ValueToStructFields();

        std::string o = "namespace " + fix_name(c->Name) + " { ";

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

    if (t->ArrayType == DataTypes_ArrayTypes_none)
    {
        return "static const " + c1.cpp_type + " " + fix_name(c->Name) + "=" + c->Value + ";";
    }
    else
    {
        return "static const " + c1.cpp_type + " " + fix_name(c->Name) + "[]=" + c->Value + ";";
    }
}

CPPServiceLangGen::convert_generator_result CPPServiceLangGen::convert_generator(FunctionDefinition* f)
{
    if (!f->IsGenerator())
        throw InternalErrorException("");

    convert_generator_result o;
    o.return_type = "void";
    o.param_type = "void";
    if (f->ReturnType->ContainerType == DataTypes_ContainerTypes_generator)
    {
        RR_SHARED_PTR<TypeDefinition> r_type = f->ReturnType->Clone();
        r_type->RemoveContainers();
        get_variable_type_result t = get_variable_type(*r_type);
        o.return_type = t.cpp_type;
    }

    if (!f->Parameters.empty() && f->Parameters.back()->ContainerType == DataTypes_ContainerTypes_generator)
    {
        RR_SHARED_PTR<TypeDefinition> p_type = f->Parameters.back()->Clone();
        p_type->RemoveContainers();
        get_variable_type_result t = get_variable_type(*p_type);
        o.param_type = t.cpp_type;
        std::copy(f->Parameters.begin(), --f->Parameters.end(), std::back_inserter(o.params));
    }
    else
    {
        boost::range::copy(f->Parameters, std::back_inserter(o.params));
    }

    o.generator_cpp_type = "RR_SHARED_PTR<RobotRaconteur::Generator<" + o.return_type + "," + o.param_type + " > >";
    return o;
}

bool CPPServiceLangGen::is_member_override(MemberDefinition* m,
                                           const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs)
{
    RR_SHARED_PTR<ServiceEntryDefinition> obj_def = m->ServiceEntry.lock();
    if (!obj_def)
        throw InternalErrorException("Failure in is_member_override");
    RR_SHARED_PTR<ServiceDefinition> parent_def = obj_def->ServiceDefinition_.lock();
    if (!parent_def)
        throw InternalErrorException("Faisure in is_member_override");
    BOOST_FOREACH (const std::string& implements_str, obj_def->Implements)
    {
        if (!boost::contains(implements_str, "."))
        {
            RR_SHARED_PTR<ServiceEntryDefinition> implemented_obj = TryFindByName(parent_def->Objects, implements_str);
            if (!implemented_obj)
                continue;
            if (TryFindByName(implemented_obj->Members, m->Name))
            {
                return true;
            }
        }
        else
        {
            boost::tuple<boost::string_ref, boost::string_ref> split_name = SplitQualifiedName(implements_str);
            RR_SHARED_PTR<ServiceDefinition> imported_def = TryFindByName(defs, split_name.get<0>());
            if (!imported_def)
                continue;
            RR_SHARED_PTR<ServiceEntryDefinition> implemented_obj =
                TryFindByName(imported_def->Objects, split_name.get<1>());
            if (!implemented_obj)
                continue;
            if (TryFindByName(implemented_obj->Members, m->Name))
            {
                return true;
            }
        }
    }
    return false;
}

void CPPServiceLangGen::GenerateConstants(ServiceDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    bool hasconstants = false;

    std::vector<boost::shared_ptr<ServiceEntryDefinition> > entries;
    boost::copy(d->NamedArrays, std::back_inserter(entries));
    boost::copy(d->Pods, std::back_inserter(entries));
    boost::copy(d->Structures, std::back_inserter(entries));
    boost::copy(d->Objects, std::back_inserter(entries));

    for (std::vector<std::string>::iterator e = d->Options.begin(); e != d->Options.end(); ++e)
    {
        if (boost::starts_with(*e, "constant"))
            hasconstants = true;
    }

    if (!d->Enums.empty() || !d->Constants.empty())
        hasconstants = true;

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator ee = entries.begin(); ee != entries.end();
         ++ee)
    {
        for (std::vector<std::string>::iterator e = (*ee)->Options.begin(); e != (*ee)->Options.end(); ++e)
        {
            if (boost::starts_with(*e, "constant"))
                hasconstants = true;
        }

        if (!(*ee)->Constants.empty())
            hasconstants = true;
    }

    if (!hasconstants)
        return;

    w2 << "// NOLINTBEGIN" << std::endl;
    w2 << "namespace " << boost::replace_all_copy(fix_name(d->Name), ".", "__") << "Constants " << std::endl
       << "{" << std::endl;

    for (std::vector<std::string>::iterator e = d->Options.begin(); e != d->Options.end(); ++e)
    {
        if (boost::starts_with(*e, "constant"))
        {
            RR_SHARED_PTR<ServiceDefinition> def2(d, null_deleter<ServiceDefinition>);
            RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(def2);
            c->FromString(*e);

            std::vector<RR_SHARED_PTR<ConstantDefinition> > c2;
            w2 << "    " << convert_constant(c.get(), c2, d) << std::endl;
        }
    }

    BOOST_FOREACH (RR_SHARED_PTR<ConstantDefinition>& c, d->Constants)
    {
        GenerateDocString(c->DocString, "    ", w);
        w2 << "    " << convert_constant(c.get(), d->Constants, d) << std::endl;
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator ee = entries.begin(); ee != entries.end();
         ++ee)
    {
        bool objhasconstants = false;

        for (std::vector<std::string>::iterator e = (*ee)->Options.begin(); e != (*ee)->Options.end(); ++e)
        {
            if (boost::starts_with(*e, "constant"))
                objhasconstants = true;
        }

        if (objhasconstants || !(*ee)->Constants.empty())
        {
            w2 << "    namespace " << fix_name((*ee)->Name) << std::endl << "    {" << std::endl;
            for (std::vector<std::string>::iterator e = (*ee)->Options.begin(); e != (*ee)->Options.end(); ++e)
            {
                if (boost::starts_with(*e, "constant"))
                {
                    RR_SHARED_PTR<ServiceDefinition> def2(d, null_deleter<ServiceDefinition>);
                    RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(def2);
                    c->FromString(*e);
                    std::vector<RR_SHARED_PTR<ConstantDefinition> > c2;
                    w2 << "    " << convert_constant(c.get(), c2, d) << std::endl;
                }
            }

            BOOST_FOREACH (RR_SHARED_PTR<ConstantDefinition>& c, (*ee)->Constants)
            {
                GenerateDocString(c->DocString, "    ", w);
                w2 << "    " << convert_constant(c.get(), (*ee)->Constants, d) << std::endl;
            }

            w2 << "    }" << std::endl;
        }
    }

    w2 << "}" << std::endl;

    BOOST_FOREACH (RR_SHARED_PTR<EnumDefinition>& e, d->Enums)
    {
        w2 << "    namespace " << fix_name(e->Name) << std::endl;
        w2 << "    {" << std::endl;
        GenerateDocString(e->DocString, "    ", w);
        w2 << "    enum " << fix_name(e->Name) << std::endl;
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
        w2 << "    }" << std::endl;
    }
    w2 << "// NOLINTEND" << std::endl;
}

void CPPServiceLangGen::GenerateFiles(const RR_SHARED_PTR<ServiceDefinition>& d, const std::string& servicedef,
                                      const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs,
                                      const std::vector<std::string>& extra_include, const std::string& path)
{
#ifdef _WIN32
    const std::string os_pathsep("\\");
#else
    const std::string os_pathsep("/");
#endif

    std::ofstream f1((path + os_pathsep + boost::replace_all_copy(fix_name(d->Name), ".", "__") + ".h").c_str());
    GenerateInterfaceHeaderFile(d.get(), defs, extra_include, &f1);
    f1.close();

    std::ofstream f2(
        (path + os_pathsep + boost::replace_all_copy(fix_name(d->Name), ".", "__") + "_stubskel.h").c_str());
    GenerateStubSkelHeaderFile(d.get(), defs, &f2);
    f2.close();

    std::ofstream f3(
        (path + os_pathsep + boost::replace_all_copy(fix_name(d->Name), ".", "__") + "_stubskel.cpp").c_str());
    GenerateStubSkelFile(d.get(), defs, &f3, servicedef);
    f3.close();
}

std::string CPPServiceLangGen::GetDefaultValue(const TypeDefinition& tdef)
{
    if (tdef.Type == DataTypes_void_t)
        throw InternalErrorException("Internal error");
    if (tdef.ArrayType == DataTypes_ArrayTypes_none && tdef.ContainerType == DataTypes_ContainerTypes_none)
    {
        if (tdef.Type == DataTypes_double_t || tdef.Type == DataTypes_single_t)
            return "0.0";
        if (tdef.Type >= DataTypes_int8_t && tdef.Type <= DataTypes_uint64_t)
            return "0";
        if (tdef.Type == DataTypes_cdouble_t)
            return "RobotRaconteur::cdouble(0.0,0.0)";
        if (tdef.Type == DataTypes_csingle_t)
            return "RobotRaconteur::cfloat(0.0,0.0)";
        if (tdef.Type == DataTypes_bool_t)
            return "RobotRaconteur::rr_bool(0)";
        if (tdef.Type == DataTypes_string_t)
            return "\"\"";
    }

    return get_variable_type(tdef).cpp_type + "()";
}

std::string CPPServiceLangGen::GetDefaultInitializedValue(const TypeDefinition& tdef)
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
                if (tdef.Type == DataTypes_double_t || tdef.Type == DataTypes_single_t)
                    return "0.0";
                if (tdef.Type >= DataTypes_int8_t && tdef.Type <= DataTypes_uint64_t)
                    return "0";
                if (tdef.Type == DataTypes_cdouble_t)
                    return "RobotRaconteur::cdouble(0.0,0.0)";
                if (tdef.Type == DataTypes_csingle_t)
                    return "RobotRaconteur::cfloat(0.0,0.0)";
                if (tdef.Type == DataTypes_bool_t)
                    return "RobotRaconteur::rr_bool(0)";
                throw InvalidArgumentException("Invalid numeric type");
            }
            case DataTypes_ArrayTypes_array: {
                RR_SHARED_PTR<TypeDefinition> tdef2 = tdef.Clone();
                tdef2->RemoveContainers();
                tdef2->RemoveArray();
                convert_type_result t = convert_type(*tdef2);
                if (tdef.ArrayVarLength)
                {
                    return "RobotRaconteur::AllocateEmptyRRArray<" + t.cpp_type + ">(0)";
                }
                else
                {
                    return "RobotRaconteur::AllocateEmptyRRArray<" + t.cpp_type + ">(" +
                           boost::lexical_cast<std::string>(tdef.ArrayLength.at(0)) + ")";
                }
            }
            case DataTypes_ArrayTypes_multidimarray: {
                RR_SHARED_PTR<TypeDefinition> tdef2 = tdef.Clone();
                tdef2->RemoveContainers();
                tdef2->RemoveArray();
                convert_type_result t = convert_type(*tdef2);
                if (tdef.ArrayVarLength)
                {
                    return "RobotRaconteur::AllocateEmptyRRMultiDimArray<" + t.cpp_type +
                           ">(boost::assign::list_of(1)(0))";
                }
                else
                {
                    return "RobotRaconteur::AllocateEmptyRRMultiDimArray<" + t.cpp_type + ">(boost::assign::list_of(" +
                           boost::join(tdef.ArrayLength |
                                           boost::adaptors::transformed(boost::lexical_cast<std::string, int32_t>),
                                       ")(") +
                           "))";
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
                    convert_type_result t = convert_type(*tdef2);
                    return t.cpp_type + "()";
                }
                case DataTypes_ArrayTypes_array: {
                    convert_type_result t = convert_type(*tdef2);
                    if (tdef.ArrayVarLength)
                    {
                        return "RobotRaconteur::AllocateEmptyRRPodArray<" + t.cpp_type + ">(0)";
                    }
                    else
                    {
                        return "RobotRaconteur::AllocateEmptyRRPodArray<" + t.cpp_type + ">(" +
                               boost::lexical_cast<std::string>(tdef.ArrayLength.at(0)) + ")";
                    }
                }
                case DataTypes_ArrayTypes_multidimarray: {
                    convert_type_result t = convert_type(*tdef2);
                    if (tdef.ArrayVarLength)
                    {
                        return "RobotRaconteur::AllocateEmptyRRPodMultiDimArray<" + t.cpp_type +
                               ">(boost::assign::list_of(1)(0))";
                    }
                    else
                    {
                        return "RobotRaconteur::AllocateEmptyRRPodMultiDimArray<" + t.cpp_type +
                               ">(boost::assign::list_of(" +
                               boost::join(tdef.ArrayLength |
                                               boost::adaptors::transformed(boost::lexical_cast<std::string, int32_t>),
                                           ")(") +
                               "))";
                    }
                }
                default:
                    throw InvalidArgumentException("Invalid array type");
                }
            }
        }
    }

    return get_variable_type(tdef).cpp_type + "()";
}

void CPPServiceLangGen::GenerateMasterHeaderFile(const std::vector<RR_SHARED_PTR<ServiceDefinition> >& d,
                                                 std::ostream* w)
{
    std::ostream& w2 = *w;
    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl << std::endl;
    BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& e, d)
    {
        w2 << "#include \"" << boost::replace_all_copy(fix_name(e->Name), ".", "__") << "_stubskel.h\"" << std::endl;
    }
    w2 << std::endl;

    std::string type_list = "boost::assign::list_of<RR_SHARED_PTR<RobotRaconteur::ServiceFactory> >";
    // BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& e, d)
    // {
    //     w2 << "#include \"" << boost::replace_all_copy(fix_name(e->Name), ".", "__") << "_stubskel.h\"" << std::endl;
    // }

    BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& e, d)
    {
        std::string n1 = boost::replace_all_copy(fix_name(e->Name), ".", "::");
        std::string n2 = boost::replace_all_copy(fix_name(e->Name), ".", "__");
        // NOLINTBEGIN(performance-inefficient-string-concatenation)
        type_list += "(RR_MAKE_SHARED< ::" + n1 + "::" + n2 + "Factory>())";
        // NOLINTEND(performance-inefficient-string-concatenation)
    }

    type_list += ".convert_to_container<std::vector<RR_SHARED_PTR<RobotRaconteur::ServiceFactory> > >()";

    w2 << "#define ROBOTRACONTEUR_SERVICE_TYPES " << type_list << std::endl;
    w2 << "#pragma once" << std::endl << std::endl;
}

void CPPServiceLangGen::GenerateDefaultImplHeader(ServiceDefinition* d,
                                                  const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                                  bool is_abstract, std::ostream* w)
{
    std::ostream& w2 = *w;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        if (!is_abstract)
        {
            w2 << "class " << export_definition(d) << " " << fix_name((*e)->Name) << "_default_impl : public virtual "
               << fix_name((*e)->Name) << ", public virtual RobotRaconteur::RRObject_default_impl" << std::endl;
        }
        else
        {
            w2 << "class " << export_definition(d) << " " << fix_name((*e)->Name)
               << "_default_abstract_impl : public virtual " << fix_name((*e)->Name)
               << ", public virtual RobotRaconteur::RRObject_default_impl" << std::endl;
        }

        w2 << "{" << std::endl;
        w2 << "protected:" << std::endl;
        MEMBER_ITER3(PropertyDefinition)
        get_variable_type_result t = get_variable_type(*m->Type);
        w2 << t.cpp_type << " rrvar_" << fix_name(m->Name) << ";" << std::endl;
        MEMBER_ITER3_END()
        MEMBER_ITER3(EventDefinition)
        w2 << EventDeclaration(m.get(), true, true) << ";" << std::endl;
        MEMBER_ITER3_END()
        MEMBER_ITER3(PipeDefinition)
        if (m->Direction() == MemberDefinition_Direction_readonly)
        {
            get_variable_type_result t = get_variable_type(*m->Type);
            w2 << "RR_SHARED_PTR<RobotRaconteur::PipeBroadcaster<" << t.cpp_type << " > > rrvar_" << fix_name(m->Name)
               << ";" << std::endl;
        }
        MEMBER_ITER3_END()
        MEMBER_ITER3(CallbackDefinition)
        w2 << "RR_SHARED_PTR<RobotRaconteur::Callback<" << GetCallbackDeclaration(m.get(), true, true) << " > > rrvar_"
           << fix_name(m->Name) << ";" << std::endl;
        MEMBER_ITER3_END()
        MEMBER_ITER3(WireDefinition)
        if (m->Direction() == MemberDefinition_Direction_readonly)
        {
            get_variable_type_result t = get_variable_type(*m->Type);
            w2 << "RR_SHARED_PTR<RobotRaconteur::WireBroadcaster<" << t.cpp_type << " > > rrvar_" << m->Name << ";"
               << std::endl;
        }
        if (m->Direction() == MemberDefinition_Direction_writeonly)
        {
            get_variable_type_result t = get_variable_type(*m->Type);
            w2 << "RR_SHARED_PTR<RobotRaconteur::WireUnicastReceiver<" << t.cpp_type << " > > rrvar_" << m->Name << ";"
               << std::endl;
        }
        MEMBER_ITER3_END()

        w2 << std::endl;
        w2 << "public:" << std::endl;

        if (!is_abstract)
        {
            w2 << fix_name((*e)->Name) << "_default_impl();" << std::endl;
        }
        else
        {
            w2 << fix_name((*e)->Name) << "_default_abstract_impl();" << std::endl;
        }

        MEMBER_ITER3(PropertyDefinition)
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << GetPropertyDeclaration(m.get(), true) << ";" << std::endl;
        }
        if (m->Direction() != MemberDefinition_Direction_readonly)
        {
            w2 << SetPropertyDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        }
        MEMBER_ITER3_END()

        MEMBER_ITER3(FunctionDefinition)
        w2 << FunctionDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(EventDefinition)
        w2 << EventDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(ObjRefDefinition)
        w2 << ObjRefDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(PipeDefinition)
        w2 << GetPipeDeclaration(m.get(), true) << ";" << std::endl;
        w2 << SetPipeDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(CallbackDefinition)
        w2 << GetCallbackDeclaration(m.get(), true) << ";" << std::endl;
        w2 << SetCallbackDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(WireDefinition)
        w2 << GetWireDeclaration(m.get(), true) << ";" << std::endl;
        w2 << SetWireDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(MemoryDefinition)
        w2 << MemoryDeclaration(m.get(), true) << ";" << std::endl << std::endl;
        MEMBER_ITER3_END()

        w2 << "RR_OVIRTUAL std::string RRType() RR_OVERRIDE {return \"" << d->Name << "." << (*e)->Name << "\";  }"
           << std::endl;

        w2 << "};" << std::endl << std::endl;
    }
}

void CPPServiceLangGen::GenerateDefaultImplDefinition(ServiceDefinition* d,
                                                      const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
                                                      bool is_abstract, std::ostream* w)
{
    std::ostream& w2 = *w;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        std::string default_impl_name;
        if (!is_abstract)
        {
            default_impl_name = fix_name((*e)->Name) + "_default_impl";
        }
        else
        {
            default_impl_name = fix_name((*e)->Name) + "_default_abstract_impl";
        }

        w2 << default_impl_name << "::" << default_impl_name << "()" << std::endl;
        w2 << "{" << std::endl;
        MEMBER_ITER3(PropertyDefinition)
        w2 << "rrvar_" << fix_name(m->Name) << "=" << GetDefaultInitializedValue(*m->Type) << ";" << std::endl;
        MEMBER_ITER3_END()
        w2 << "}" << std::endl;

        MEMBER_ITER3(PropertyDefinition)
        if (m->Direction() != MemberDefinition_Direction_writeonly)
        {
            w2 << dforc(GetPropertyDeclaration(m.get(), true), default_impl_name) << std::endl;
            w2 << "{" << std::endl;
            w2 << "boost::mutex::scoped_lock lock(this_lock);" << std::endl;
            w2 << "return rrvar_" << fix_name(m->Name) << ";" << std::endl;
            w2 << "}" << std::endl;
        }
        if (m->Direction() != MemberDefinition_Direction_readonly)
        {
            w2 << dforc(SetPropertyDeclaration(m.get(), true), default_impl_name) << std::endl;
            w2 << "{" << std::endl;
            w2 << "boost::mutex::scoped_lock lock(this_lock);" << std::endl;
            w2 << "rrvar_" << fix_name(m->Name) << " = value;" << std::endl;
            w2 << "}" << std::endl;
        }
        MEMBER_ITER3_END()

        MEMBER_ITER3(FunctionDefinition)
        w2 << dforc(FunctionDeclaration(m.get(), true), default_impl_name) << std::endl;
        w2 << "{" << std::endl;
        w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(EventDefinition)
        w2 << dforc(EventDeclaration(m.get(), true), default_impl_name) << std::endl;
        w2 << "{" << std::endl;
        w2 << "return rrvar_" << fix_name(m->Name) << ";" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(ObjRefDefinition)
        w2 << dforc(ObjRefDeclaration(m.get(), true), default_impl_name) << std::endl;
        w2 << "{" << std::endl;
        w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(PipeDefinition)
        w2 << dforc(GetPipeDeclaration(m.get(), true), default_impl_name) << std::endl;
        w2 << "{" << std::endl;
        if (m->Direction() == MemberDefinition_Direction_readonly)
        {
            w2 << "boost::mutex::scoped_lock lock(this_lock);" << std::endl;
            w2 << "if (!rrvar_" << fix_name(m->Name)
               << ") throw RobotRaconteur::InvalidOperationException(\"Pipe not set\");" << std::endl;
            w2 << "return rrvar_" << fix_name(m->Name) << "->GetPipe();" << std::endl;
        }
        else
        {
            w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << std::endl;
        }
        w2 << "}" << std::endl;
        w2 << dforc(SetPipeDeclaration(m.get(), true), default_impl_name) << std::endl;
        w2 << "{" << std::endl;
        if (m->Direction() == MemberDefinition_Direction_readonly)
        {
            w2 << "boost::mutex::scoped_lock lock(this_lock);" << std::endl;
            get_variable_type_result t = get_variable_type(*m->Type);
            w2 << "if (rrvar_" << fix_name(m->Name)
               << ") throw RobotRaconteur::InvalidOperationException(\"Pipe already set\");" << std::endl;
            w2 << "rrvar_" << fix_name(m->Name) << " = RR_MAKE_SHARED<RobotRaconteur::PipeBroadcaster<" << t.cpp_type
               << " > >();" << std::endl;
            w2 << "rrvar_" << fix_name(m->Name) << "->Init(value);" << std::endl;
        }
        else
        {
            w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << std::endl;
        }
        w2 << "}" << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(CallbackDefinition)
        w2 << dforc(GetCallbackDeclaration(m.get(), true), default_impl_name) << std::endl;
        w2 << "{" << std::endl;
        w2 << "boost::mutex::scoped_lock lock(this_lock);" << std::endl;
        w2 << "return rrvar_" << fix_name(m->Name) << ";" << std::endl;
        w2 << "}" << std::endl;
        w2 << dforc(SetCallbackDeclaration(m.get(), true), default_impl_name) << std::endl;
        w2 << "{" << std::endl;
        w2 << "boost::mutex::scoped_lock lock(this_lock);" << std::endl;
        w2 << "if (rrvar_" << fix_name(m->Name)
           << ") throw RobotRaconteur::InvalidOperationException(\"Callback already set\");" << std::endl;
        w2 << "rrvar_" << fix_name(m->Name) << " = value;" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(WireDefinition)
        w2 << dforc(GetWireDeclaration(m.get(), true), default_impl_name) << std::endl;
        w2 << "{" << std::endl;
        if (m->Direction() == MemberDefinition_Direction_readonly ||
            m->Direction() == MemberDefinition_Direction_writeonly)
        {
            w2 << "boost::mutex::scoped_lock lock(this_lock);" << std::endl;
            w2 << "if (!rrvar_" << fix_name(m->Name)
               << ") throw RobotRaconteur::InvalidOperationException(\"Wire not set\");" << std::endl;
            w2 << "return rrvar_" << fix_name(m->Name) << "->GetWire();" << std::endl;
        }
        else
        {
            w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << std::endl;
        }
        w2 << "}" << std::endl;
        w2 << dforc(SetWireDeclaration(m.get(), true), default_impl_name) << std::endl;
        w2 << "{" << std::endl;
        if (m->Direction() == MemberDefinition_Direction_readonly)
        {
            get_variable_type_result t = get_variable_type(*m->Type);
            w2 << "if (rrvar_" << fix_name(m->Name)
               << ") throw RobotRaconteur::InvalidOperationException(\"Wire already set\");" << std::endl;
            w2 << "rrvar_" << fix_name(m->Name) << " = RR_MAKE_SHARED<RobotRaconteur::WireBroadcaster<" << t.cpp_type
               << " > >();" << std::endl;
            w2 << "rrvar_" << fix_name(m->Name) << "->Init(value);";
        }
        else if (m->Direction() == MemberDefinition_Direction_writeonly)
        {
            get_variable_type_result t = get_variable_type(*m->Type);
            w2 << "if (rrvar_" << fix_name(m->Name)
               << ") throw RobotRaconteur::InvalidOperationException(\"Wire already set\");" << std::endl;
            w2 << "rrvar_" << fix_name(m->Name) << " = RR_MAKE_SHARED<RobotRaconteur::WireUnicastReceiver<"
               << t.cpp_type << " > >();" << std::endl;
            w2 << "rrvar_" << fix_name(m->Name) << "->Init(value);";
        }
        else
        {
            w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << std::endl;
        }
        w2 << "}" << std::endl;
        MEMBER_ITER3_END()

        MEMBER_ITER3(MemoryDefinition)
        w2 << dforc(MemoryDeclaration(m.get(), true), default_impl_name) << std::endl;
        w2 << "{" << std::endl;
        w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << std::endl;
        w2 << "}" << std::endl;
        MEMBER_ITER3_END()
    }
}

void CPPServiceLangGen::GenerateDocString(const std::string& docstring, const std::string& prefix, std::ostream* w)
{
    if (docstring.empty())
    {
        return;
    }

    std::ostream& w2 = *w;

    std::vector<std::string> docstring_v;
    boost::split(docstring_v, docstring, boost::is_any_of("\n"));
    w2 << prefix << "/*" << std::endl;
    BOOST_FOREACH (const std::string& s, docstring_v)
    {
        w2 << prefix << " * " << s << std::endl;
    }
    w2 << prefix << " */" << std::endl;
}

} // namespace RobotRaconteurGen
