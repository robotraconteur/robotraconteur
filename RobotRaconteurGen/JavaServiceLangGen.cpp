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

#include "JavaServiceLangGen.h"

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <set>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/locale.hpp>

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
        for (std::vector<RR_SHARED_PTR<MemberDefinition> >::const_iterator m1 = e->Members.begin();                    \
             m1 != e->Members.end(); ++m1)                                                                             \
            if (dynamic_cast<TYPE*>(m1->get()) != 0)                                                                   \
            {                                                                                                          \
                RR_SHARED_PTR<TYPE> m = boost::dynamic_pointer_cast<TYPE>(*m1);
// NOLINTEND(bugprone-macro-parentheses)

namespace RobotRaconteurGen
{

std::string JavaServiceLangGen::fix_name(const std::string& name)
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
    const char* res_str[] = {"abstract", "assert",     "boolean",      "break",         "byte",        "case",
                             "catch",    "char",       "class",        "const",         "continue",    "default",
                             "do",       "double",     "else",         "enum",          "extends",     "final",
                             "finally",  "float",      "for",          "goto",          "if",          "implements",
                             "import",   "instanceof", "int",          "interface",     "long",        "native",
                             "new",      "package",    "private",      "protected",     "public",      "return",
                             "short",    "static",     "strictfp",     "super",         "switch",      "synchronized",
                             "this",     "throw",      "throws",       "transient",     "try",         "void",
                             "volatile", "while",      "UnsignedByte", "UnsignedShort", "UnsignedInt", "UnsignedLong"};
    // NOLINTEND(cppcoreguidelines-avoid-c-arrays)

    std::vector<std::string> reserved(res_str, res_str + sizeof(res_str) / (sizeof(res_str[0])));

    if (std::find(reserved.begin(), reserved.end(), name) != reserved.end())
    {
        std::cout << "warning: name " << name << " replaced by " << name << "_ due to keyword conflict" << std::endl;
        return name + "_";
    }

    return name;
}

std::string JavaServiceLangGen::fix_qualified_name(const std::string& name) { return fix_name(name); }

JavaServiceLangGen::convert_type_result JavaServiceLangGen::convert_type_array(TypeDefinition tdef)
{
    if (tdef.Type == DataTypes_namedtype_t)
    {
        RR_SHARED_PTR<NamedTypeDefinition> nt = tdef.ResolveNamedType_cache.lock();
        if (!nt)
            throw DataTypeException("Data type not resolved");
        if (nt->RRDataType() == DataTypes_enum_t)
        {

            JavaServiceLangGen::convert_type_result o1 = convert_type(tdef);

            JavaServiceLangGen::convert_type_result o;
            o.name = tdef.Name;
            o.java_type = o1.java_type;
            o.java_arr_type = "[]";
            return o;
        }

        if ((nt->RRDataType() == DataTypes_pod_t || nt->RRDataType() == DataTypes_namedarray_t) &&
            tdef.ArrayType == DataTypes_ArrayTypes_none)
        {
            tdef.ArrayType = DataTypes_ArrayTypes_array;
        }
    }

    if (IsTypeNumeric(tdef.Type) && tdef.ArrayType == DataTypes_ArrayTypes_none)
    {
        tdef.ArrayType = DataTypes_ArrayTypes_array;
    }
    return convert_type(tdef);
}

JavaServiceLangGen::convert_type_result JavaServiceLangGen::convert_type(const TypeDefinition& tdef, bool reftypes)
{
    if (tdef.ContainerType != DataTypes_ContainerTypes_none)
        reftypes = false;

    convert_type_result o;
    DataTypes t = tdef.Type;
    o.name = fix_name(tdef.Name);
    o.java_arr_type = tdef.ArrayType == DataTypes_ArrayTypes_array ? "[]" : "";

    switch (t)
    {
    case DataTypes_void_t:
        o.java_type = "void";
        break;
    case DataTypes_double_t:
        o.java_type = "double";
        break;
    case DataTypes_single_t:
        o.java_type = "float";
        break;
    case DataTypes_int8_t:
        o.java_type = "byte";
        break;
    case DataTypes_uint8_t:
        o.java_type = "UnsignedByte";
        o.java_arr_type = tdef.ArrayType == DataTypes_ArrayTypes_array ? "s" : "";
        break;
    case DataTypes_int16_t:
        o.java_type = "short";
        break;
    case DataTypes_uint16_t:
        o.java_type = "UnsignedShort";
        o.java_arr_type = tdef.ArrayType == DataTypes_ArrayTypes_array ? "s" : "";
        break;
    case DataTypes_int32_t:
        o.java_type = "int";
        break;
    case DataTypes_uint32_t:
        o.java_type = "UnsignedInt";
        o.java_arr_type = tdef.ArrayType == DataTypes_ArrayTypes_array ? "s" : "";
        break;
    case DataTypes_int64_t:
        o.java_type = "long";
        break;
    case DataTypes_uint64_t:
        o.java_type = "UnsignedLong";
        o.java_arr_type = tdef.ArrayType == DataTypes_ArrayTypes_array ? "s" : "";
        break;
    case DataTypes_string_t:
        o.java_type = "String";
        break;
    case DataTypes_cdouble_t:
        o.java_type = "CDouble";
        break;
    case DataTypes_csingle_t:
        o.java_type = "CSingle";
        break;
    case DataTypes_bool_t:
        o.java_type = "boolean";
        break;
    case DataTypes_namedtype_t:
    case DataTypes_object_t:
        o.java_type = fix_qualified_name(tdef.TypeString);
        break;
    case DataTypes_varvalue_t:
        o.java_type = "Object";
        break;
    default:
        throw InvalidArgumentException("");
    }

    if (tdef.ArrayType == DataTypes_ArrayTypes_none && reftypes)
    {
        if (o.java_type == "double")
            o.java_type = "Double";
        if (o.java_type == "float")
            o.java_type = "Float";
        if (o.java_type == "byte")
            o.java_type = "Byte";
        if (o.java_type == "short")
            o.java_type = "Short";
        if (o.java_type == "int")
            o.java_type = "Integer";
        if (o.java_type == "long")
            o.java_type = "Long";
        if (o.java_type == "boolean")
            o.java_type = "Boolean";
    }

    if (tdef.ArrayType == DataTypes_ArrayTypes_multidimarray)
    {
        if (IsTypeNumeric(tdef.Type))
        {
            o.java_type = "MultiDimArray";
            o.java_arr_type = "";
        }
        else if (tdef.Type == DataTypes_namedtype_t)
        {
            RR_SHARED_PTR<NamedTypeDefinition> nt = tdef.ResolveNamedType_cache.lock();
            if (!nt)
                throw DataTypeException("Data type not resolved");
            switch (nt->RRDataType())
            {
            case DataTypes_pod_t: {
                o.java_type = "PodMultiDimArray";
                o.java_arr_type = "";
                break;
            }
            case DataTypes_namedarray_t: {
                o.java_type = "NamedMultiDimArray";
                o.java_arr_type = "";
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

    if (tdef.ContainerType == DataTypes_ContainerTypes_none)
    {
        return o;
    }

    if (IsTypeNumeric(t) && tdef.ArrayType == DataTypes_ArrayTypes_none)
    {
        if (t == DataTypes_uint8_t || t == DataTypes_uint16_t || t == DataTypes_uint32_t || t == DataTypes_uint64_t)
        {
            o.java_arr_type = "s";
        }
        else
        {
            o.java_arr_type = "[]";
        }
    }

    RR_SHARED_PTR<NamedTypeDefinition> nt = tdef.ResolveNamedType_cache.lock();
    if (nt)
    {
        if ((nt->RRDataType() == DataTypes_pod_t || nt->RRDataType() == DataTypes_namedarray_t) &&
            tdef.ArrayType == DataTypes_ArrayTypes_none)
        {
            o.java_arr_type = "[]";
        }
    }

    switch (tdef.ContainerType)
    {
    case DataTypes_ContainerTypes_none:
        break;
    case DataTypes_ContainerTypes_list:
        o.java_type = "List<" + o.java_type + o.java_arr_type + ">";
        o.java_arr_type = "";
        break;
    case DataTypes_ContainerTypes_map_int32:
        o.java_type = "Map<Integer," + o.java_type + o.java_arr_type + ">";
        o.java_arr_type = "";
        break;
    case DataTypes_ContainerTypes_map_string:
        o.java_type = "Map<String," + o.java_type + o.java_arr_type + ">";
        o.java_arr_type = "";
        break;
    default:
        throw DataTypeException("Invalid container type");
    }

    return o;
}

std::string JavaServiceLangGen::str_pack_parameters(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l, bool inclass)
{
    std::vector<std::string> o(l.size());

    for (size_t i = 0; i < o.size(); i++)
    {
        convert_type_result t = convert_type(*l[i]);
        if (inclass)
            o[i] = t.java_type + t.java_arr_type + " " + t.name;
        else
            o[i] = t.name;
    }

    return boost::join(o, ", ");
}

std::string JavaServiceLangGen::str_pack_parameters_delegate(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l,
                                                             bool inclass)
{
    std::vector<std::string> o(l.size());

    for (size_t i = 0; i < o.size(); i++)
    {
        if (inclass)
        {
            convert_type_result t = convert_type(*l[i], true);
            o[i] = t.java_type + t.java_arr_type + " " + t.name;
        }
        else
        {
            convert_type_result t = convert_type(*l[i]);
            o[i] = t.name;
            if (t.java_arr_type.empty())
            {
                if (t.java_type == "double")
                    o[i] = "Double.valueOf(" + t.name + ")";
                if (t.java_type == "float")
                    o[i] = "Float.valueOf(" + t.name + ")";
                if (t.java_type == "byte")
                    o[i] = "Byte.valueOf(" + t.name + ")";
                if (t.java_type == "short")
                    o[i] = "Short.valueOf(" + t.name + ")";
                if (t.java_type == "int")
                    o[i] = "Integer.valueOf(" + t.name + ")";
                if (t.java_type == "long")
                    o[i] = "Long.valueOf(" + t.name + ")";
                if (t.java_type == "boolean")
                    o[i] = "Boolean.valueOf(" + t.name + ")";
            }
        }
    }

    return boost::join(o, ", ");
}

std::string JavaServiceLangGen::str_pack_delegate(const std::vector<RR_SHARED_PTR<TypeDefinition> >& l,
                                                  const boost::shared_ptr<TypeDefinition>& rettype)
{
    if (!rettype || rettype->Type == DataTypes_void_t)
    {
        if (l.empty())
        {
            return "Action";
        }
        else
        {
            std::vector<std::string> paramtypes;
            for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator e = l.begin(); e != l.end(); ++e)
            {
                convert_type_result t = convert_type(*(*e), true);

                paramtypes.push_back(t.java_type + t.java_arr_type);
            }

            return "Action" + boost::lexical_cast<std::string>(paramtypes.size()) + "<" +
                   boost::join(paramtypes, ", ") + ">";
        }
    }
    else
    {
        std::vector<std::string> paramtypes;

        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator e = l.begin(); e != l.end(); ++e)
        {
            convert_type_result t = convert_type(*(*e), true);

            paramtypes.push_back(t.java_type + t.java_arr_type);
        }

        convert_type_result t2 = convert_type(*rettype, true);

        paramtypes.push_back(t2.java_type + t2.java_arr_type);

        if (paramtypes.size() > 1)
        {
            return "Func" + boost::lexical_cast<std::string>(paramtypes.size() - 1) + "<" +
                   boost::join(paramtypes, ",") + ">";
        }
        else
        {
            return "Func<" + boost::join(paramtypes, ",") + ">";
        }
    }
}

// Code to pack and unpack message elements

static std::string JavaServiceLangGen_VerifyArrayLength(TypeDefinition& t, const std::string& varname)
{
    std::string s;
    if (t.ContainerType != DataTypes_ContainerTypes_none)
    {
        if (t.ArrayType == DataTypes_ArrayTypes_array)
        {
            s = "1";
        }
        else
        {
            if (IsTypeNumeric(t.Type))
            {
                s = "2";
            }
            else
            {
                if (t.Type == DataTypes_pod_t)
                {
                    s = "3";
                }
                else
                {
                    s = "4";
                }
            }
        }
    }

    if (t.ArrayType == DataTypes_ArrayTypes_array && t.ArrayLength.at(0) != 0)
    {
        return "DataTypeUtil.verifyArrayLength" + s + "(" + varname + ", " +
               boost::lexical_cast<std::string>(t.ArrayLength.at(0)) + ", " + (t.ArrayVarLength ? "true" : "false") +
               ")";
    }
    if (t.ArrayType == DataTypes_ArrayTypes_multidimarray && !t.ArrayLength.empty() && !t.ArrayVarLength)
    {
        int32_t n_elems = boost::accumulate(t.ArrayLength, 1, std::multiplies<int32_t>());
        return "DataTypeUtil.verifyArrayLength" + s + "(" + varname + "," + boost::lexical_cast<std::string>(n_elems) +
               ",new int[] {" +
               boost::join(t.ArrayLength | boost::adaptors::transformed(boost::lexical_cast<std::string, int32_t>),
                           ",") +
               "})";
    }
    return varname;
}

std::string JavaServiceLangGen::str_pack_message_element(const std::string& elementname, const std::string& varname,
                                                         const RR_SHARED_PTR<TypeDefinition>& t,
                                                         const std::string& packer)
{
    RR_UNUSED(packer);
    TypeDefinition t1;
    t->CopyTo(t1);
    t1.RemoveContainers();
    convert_type_result tt1 = convert_type_array(t1);

    switch (t->ContainerType)
    {
    case DataTypes_ContainerTypes_none: {
        if (IsTypeNumeric(t->Type))
        {

            switch (t->ArrayType)
            {
            case DataTypes_ArrayTypes_none: {
                convert_type_result ts = convert_type(*t);
                if (t->Type == DataTypes_uint8_t || t->Type == DataTypes_uint16_t || t->Type == DataTypes_uint32_t ||
                    t->Type == DataTypes_uint64_t)
                {
                    return "MessageElementUtil.<" + ts.java_type + ts.java_arr_type + "s>packArray(\"" + elementname +
                           "\"," + varname + ".array())";
                }
                else
                {
                    return "MessageElementUtil.<" + ts.java_type + ts.java_arr_type + "[]>packArray(\"" + elementname +
                           "\"," + "new " + ts.java_type + "[] {" + varname + "})";
                }
                break;
            }
            case DataTypes_ArrayTypes_array: {
                convert_type_result ts = convert_type(*t);
                return "MessageElementUtil.<" + ts.java_type + ts.java_arr_type + ">packArray(\"" + elementname +
                       "\"," + JavaServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                break;
            }
            case DataTypes_ArrayTypes_multidimarray: {
                convert_type_result ts = convert_type(*t);
                return "MessageElementUtil.packMultiDimArray(\"" + elementname + "\",(MultiDimArray)" +
                       JavaServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                break;
            }
            default:
                throw DataTypeException("Invalid array type");
            }
        }
        else if (t->Type == DataTypes_string_t)
        {
            return "MessageElementUtil.packString(\"" + elementname + "\"," + varname + ")";
        }
        else if (t->Type == DataTypes_varvalue_t)
        {
            return "MessageElementUtil.packVarType(\"" + elementname + "\"," + varname + ")";
        }
        else if (t->Type == DataTypes_namedtype_t)
        {
            RR_SHARED_PTR<NamedTypeDefinition> nt = t->ResolveNamedType();
            switch (nt->RRDataType())
            {
            case DataTypes_structure_t:
                return "MessageElementUtil.packStructure(\"" + elementname + "\"," + varname + ")";
                break;
            case DataTypes_enum_t:
                return "MessageElementUtil.<int[]>packArray(\"" + elementname + "\", new int[] {((int)" + varname +
                       ".getValue())})";
                break;
            case DataTypes_pod_t:
                switch (t->ArrayType)
                {
                case DataTypes_ArrayTypes_none: {
                    convert_type_result ts = convert_type(*t);
                    return "MessageElementUtil.<" + ts.java_type + ">packPodToArray(\"" + elementname + "\"," +
                           JavaServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                    break;
                }
                case DataTypes_ArrayTypes_array: {
                    convert_type_result ts = convert_type(*t);
                    return "MessageElementUtil.<" + ts.java_type + ">packPodArray(\"" + elementname + "\"," +
                           JavaServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                    break;
                }
                case DataTypes_ArrayTypes_multidimarray: {
                    convert_type_result ts = convert_type(*t);
                    return "MessageElementUtil.<" + ts.java_type + ">packPodMultiDimArray(\"" + elementname + "\"," +
                           JavaServiceLangGen_VerifyArrayLength(*t, varname) + ")";
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
                    convert_type_result ts = convert_type(*t);
                    return "MessageElementUtil.<" + ts.java_type + ">packNamedArrayToArray(\"" + elementname + "\"," +
                           JavaServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                    break;
                }
                case DataTypes_ArrayTypes_array: {
                    convert_type_result ts = convert_type(*t);
                    return "MessageElementUtil.<" + ts.java_type + ">packNamedArray(\"" + elementname + "\"," +
                           JavaServiceLangGen_VerifyArrayLength(*t, varname) + ")";
                    break;
                }
                case DataTypes_ArrayTypes_multidimarray: {
                    convert_type_result ts = convert_type(*t);
                    return "MessageElementUtil.<" + ts.java_type + ">packNamedMultiDimArray(\"" + elementname + "\"," +
                           JavaServiceLangGen_VerifyArrayLength(*t, varname) + ")";
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
        break;
    }
    case DataTypes_ContainerTypes_list:
        return "MessageElementUtil.<" + tt1.java_type + tt1.java_arr_type + ">packListType(\"" + elementname + "\"," +
               JavaServiceLangGen_VerifyArrayLength(*t, varname) + "," + tt1.java_type + tt1.java_arr_type + ".class)";
    case DataTypes_ContainerTypes_map_int32:
        return "MessageElementUtil.<Integer," + tt1.java_type + tt1.java_arr_type + ">packMapType(\"" + elementname +
               "\"," + JavaServiceLangGen_VerifyArrayLength(*t, varname) + ",Integer.class," + tt1.java_type +
               tt1.java_arr_type + ".class)";
    case DataTypes_ContainerTypes_map_string:
        return "MessageElementUtil.<String," + tt1.java_type + tt1.java_arr_type + ">packMapType(\"" + elementname +
               "\"," + JavaServiceLangGen_VerifyArrayLength(*t, varname) + ",String.class," + tt1.java_type +
               tt1.java_arr_type + ".class)";
    default:
        throw DataTypeException("Invalid container type");
    }
    throw DataTypeException("Invalid data type");
}

std::string JavaServiceLangGen::str_unpack_message_element(const std::string& varname,
                                                           const RR_SHARED_PTR<TypeDefinition>& t,
                                                           const std::string& packer)
{
    RR_UNUSED(packer);
    TypeDefinition t1;
    t->CopyTo(t1);
    t1.RemoveContainers();

    convert_type_result tt = convert_type(t1);

    std::string structunpackstring;

    convert_type_result tt1 = convert_type_array(t1);

    if (IsTypeNumeric(t->Type))
    {
        switch (t->ArrayType)
        {
        case DataTypes_ArrayTypes_none: {
            if (t->Type == DataTypes_uint8_t || t->Type == DataTypes_uint16_t || t->Type == DataTypes_uint32_t ||
                t->Type == DataTypes_uint64_t)
            {
                structunpackstring = "MessageElementUtil.<" + tt.java_type + tt.java_arr_type +
                                     "s>castDataAndDispose(" + varname + ").get(0)";
            }
            else
            {
                structunpackstring = "(MessageElementUtil.<" + tt.java_type + tt.java_arr_type +
                                     "[]>castDataAndDispose(" + varname + "))[0]";
            }
            break;
        }
        case DataTypes_ArrayTypes_array: {
            structunpackstring = JavaServiceLangGen_VerifyArrayLength(
                *t, "MessageElementUtil.<" + tt.java_type + tt.java_arr_type + ">unpackArray(" + varname + ")");
            break;
        }
        case DataTypes_ArrayTypes_multidimarray: {
            structunpackstring =
                JavaServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.unpackMultiDimArray(" + varname + ")");
            break;
        }
        default:
            throw DataTypeException("Invalid array type");
        }
    }
    else if (t->Type == DataTypes_string_t)
    {
        structunpackstring = "MessageElementUtil.unpackString(" + varname + ")";
    }
    else if (t->Type == DataTypes_namedtype_t)
    {
        RR_SHARED_PTR<NamedTypeDefinition> nt = t->ResolveNamedType();
        switch (nt->RRDataType())
        {
        case DataTypes_structure_t:
            structunpackstring = "MessageElementUtil.<" + tt.java_type + ">unpackStructure(" + varname + ")";
            break;
        case DataTypes_enum_t:
            structunpackstring =
                "" + tt.java_type + ".intToEnum((MessageElementUtil.<int[]>castDataAndDispose(" + varname + ")[0]))";
            break;
        case DataTypes_pod_t:
            switch (t->ArrayType)
            {
            case DataTypes_ArrayTypes_none: {
                structunpackstring =
                    "MessageElementUtil.<" + fix_qualified_name(t->TypeString) + ">unpackPodFromArray(" + varname + ")";
                break;
            }
            case DataTypes_ArrayTypes_array: {
                structunpackstring = JavaServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.<" +
                                                                                  fix_qualified_name(t->TypeString) +
                                                                                  ">unpackPodArray(" + varname + ")");
                break;
            }
            case DataTypes_ArrayTypes_multidimarray: {
                structunpackstring = JavaServiceLangGen_VerifyArrayLength(
                    *t, "MessageElementUtil.unpackPodMultiDimArray(" + varname + ")");
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
                structunpackstring = "MessageElementUtil.<" + fix_qualified_name(t->TypeString) +
                                     ">unpackNamedArrayFromArray(" + varname + ")";
                break;
            }
            case DataTypes_ArrayTypes_array: {
                structunpackstring = JavaServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.<" +
                                                                                  fix_qualified_name(t->TypeString) +
                                                                                  ">unpackNamedArray(" + varname + ")");
                break;
            }
            case DataTypes_ArrayTypes_multidimarray: {
                structunpackstring = JavaServiceLangGen_VerifyArrayLength(
                    *t, "MessageElementUtil.unpackNamedMultiDimArray(" + varname + ")");
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
        structunpackstring = "MessageElementUtil.unpackVarType(" + varname + ")";
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
        return JavaServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.<" + tt1.java_type + tt1.java_arr_type +
                                                            ">unpackListType(" + varname + ")");
    case DataTypes_ContainerTypes_map_int32:
        return JavaServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.<Integer," + tt1.java_type +
                                                            tt1.java_arr_type + ">unpackMapType(" + varname + ")");
    case DataTypes_ContainerTypes_map_string:
        return JavaServiceLangGen_VerifyArrayLength(*t, "MessageElementUtil.<String," + tt1.java_type +
                                                            tt1.java_arr_type + ">unpackMapType(" + varname + ")");
    default:
        throw DataTypeException("Invalid container type");
    }
}

bool JavaServiceLangGen::GetObjRefIndType(RR_SHARED_PTR<ObjRefDefinition>& m, std::string& indtype)
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
            indtype = "String";
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

void JavaServiceLangGen::GenerateStructure(ServiceEntryDefinition* e, std::ostream* w)
{
    std::ostream& w2 = *w;

    GenerateDocString(e->DocString, "", w);

    w2 << "public class " << fix_name(e->Name) << " implements RRStructure" << std::endl << "{" << std::endl;

    MEMBER_ITER2(PropertyDefinition)
    GenerateDocString(m->DocString, "    ", w);
    convert_type_result t = convert_type(*m->Type);
    t.name = fix_name(m->Name);
    w2 << "    public " << t.java_type << t.java_arr_type << " " << t.name << ";" << std::endl;
    MEMBER_ITER_END()
    w2 << "}" << std::endl << std::endl;
}

static RR_SHARED_PTR<TypeDefinition> JavaServiceLangGen_RemoveMultiDimArray(const TypeDefinition& t)
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

void JavaServiceLangGen::GeneratePod(ServiceEntryDefinition* e, std::ostream* w)
{
    std::ostream& w2 = *w;

    GenerateDocString(e->DocString, "", w);

    w2 << "public class " << fix_name(e->Name) << " implements RRPod " << std::endl << "{" << std::endl;

    MEMBER_ITER2(PropertyDefinition)
    GenerateDocString(m->DocString, "    ", w);
    TypeDefinition t2 = *JavaServiceLangGen_RemoveMultiDimArray(*m->Type);
    convert_type_result t = convert_type(t2);
    t.name = fix_name(m->Name);
    w2 << "    public " << t.java_type << t.java_arr_type << " " << t.name << ";" << std::endl;
    MEMBER_ITER_END()
    w2 << "}" << std::endl << std::endl;
}

void JavaServiceLangGen::GenerateNamedArray(const RR_SHARED_PTR<ServiceEntryDefinition>& e, std::ostream* w)
{
    std::ostream& w2 = *w;

    GenerateDocString(e->DocString, "", w);

    w2 << "public class " << fix_name(e->Name) << " implements RRNamedArray " << std::endl << "{" << std::endl;

    MEMBER_ITER2(PropertyDefinition)
    GenerateDocString(m->DocString, "    ", w);
    TypeDefinition t2 = *JavaServiceLangGen_RemoveMultiDimArray(*m->Type);
    convert_type_result t = convert_type(t2);
    t.name = fix_name(m->Name);
    w2 << "    public " << t.java_type << t.java_arr_type << " " << t.name << ";" << std::endl;
    MEMBER_ITER_END()

    w2 << "    public " << fix_name(e->Name) << "()" << std::endl << "    {" << std::endl;
    MEMBER_ITER2(PropertyDefinition)
    TypeDefinition t2 = *JavaServiceLangGen_RemoveMultiDimArray(*m->Type);
    convert_type_result t = convert_type(t2);
    t.name = fix_name(m->Name);

    if (IsTypeNumeric(t2.Type))
    {
        bool unsigned_int = (t2.Type == DataTypes_uint8_t || t2.Type == DataTypes_uint16_t ||
                             t2.Type == DataTypes_uint32_t || t2.Type == DataTypes_uint64_t);

        if (t2.ArrayType == DataTypes_ArrayTypes_none)
        {
            w2 << "    " << t.name << " = " << GetDefaultValue(t2) << ";" << std::endl;
        }
        else
        {
            if (!unsigned_int)
            {
                w2 << "    " << t.name << " = new " << t.java_type << "[" << t2.ArrayLength.at(0) << "];" << std::endl;
            }
            else
            {
                w2 << "    " << t.name << " = new " << t.java_type << "s(" << t2.ArrayLength.at(0) << ");" << std::endl;
            }
        }
    }
    else
    {
        if (t2.ArrayType == DataTypes_ArrayTypes_none)
        {
            w2 << "    " << t.name << " = new " << t.java_type << "();" << std::endl;
        }
        else
        {
            w2 << "    " << t.name << " = new " << t.java_type << "[" << t2.ArrayLength.at(0) << "];" << std::endl;
            w2 << "    for (int i=0; i<" << t2.ArrayLength.at(0) << "; i++) " << t.name << "[i] = new " << t.java_type
               << "();" << std::endl;
        }
    }
    MEMBER_ITER_END()
    w2 << "    }" << std::endl;

    boost::tuple<DataTypes, size_t> t4 = GetNamedArrayElementTypeAndCount(e);
    TypeDefinition t5;
    t5.Type = t4.get<0>();
    t5.ArrayType = DataTypes_ArrayTypes_array;
    convert_type_result t6 = convert_type(t5);

    bool unsigned_int = (t5.Type == DataTypes_uint8_t || t5.Type == DataTypes_uint16_t ||
                         t5.Type == DataTypes_uint32_t || t5.Type == DataTypes_uint64_t);

    w2 << "    public " << t6.java_type << t6.java_arr_type << " getNumericArray()" << std::endl
       << "    {" << std::endl;

    w2 << "    " << t6.java_type << t6.java_arr_type << " a = new " << t6.java_type;
    if (!unsigned_int)
    {
        w2 << "[" << t4.get<1>() << "];" << std::endl;
    }
    else
    {
        w2 << t6.java_arr_type << "(" << t4.get<1>() << ");" << std::endl;
    }
    w2 << "    getNumericArray(a,0);" << std::endl;
    w2 << "    return a;" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public void getNumericArray(" << t6.java_type << t6.java_arr_type << " buffer, int offset)" << std::endl
       << "    {" << std::endl;
    {

        int32_t i = 0;
        MEMBER_ITER2(PropertyDefinition)
        TypeDefinition t7 = *JavaServiceLangGen_RemoveMultiDimArray(*m->Type);
        convert_type_result t8 = convert_type(t7);
        t8.name = fix_name(m->Name);
        if (IsTypeNumeric(m->Type->Type))
        {
            if (m->Type->ArrayType == DataTypes_ArrayTypes_none)
            {
                if (!unsigned_int)
                {
                    w2 << "    buffer[offset + " << i << "] = " << t8.name << ";" << std::endl;
                }
                else
                {
                    w2 << "    buffer.value[offset + " << i << "] = " << t8.name << ".value;" << std::endl;
                }
                i++;
            }
            else
            {
                if (!unsigned_int)
                {
                    w2 << "    System.arraycopy";
                }
                else
                {
                    w2 << "    DataTypeUtil.arraycopy";
                }
                w2 << "(" << t8.name << ", 0, buffer, offset + " << i << ", " << t7.ArrayLength.at(0) << "); "
                   << std::endl;
                i += t7.ArrayLength.at(0);
            }
        }
        else
        {
            RR_SHARED_PTR<ServiceEntryDefinition> e2 = rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType());
            boost::tuple<DataTypes, size_t> t9 = GetNamedArrayElementTypeAndCount(e2);
            size_t e2_count = m->Type->ArrayType == DataTypes_ArrayTypes_none ? 1 : t7.ArrayLength.at(0);

            if (m->Type->ArrayType == DataTypes_ArrayTypes_none)
            {
                w2 << "    " << t8.name << ".getNumericArray(buffer, offset + " << i << ");" << std::endl;
            }
            else
            {
                w2 << "    " << m->Type->TypeString << ".getNumericArray(" << t8.name << ", buffer, offset + " << i
                   << ");" << std::endl;
            }
            i += boost::numeric_cast<int32_t>(t9.get<1>() * e2_count);
        }
        // w2 << "    public " + t8.cs_type + t8.cs_arr_type + " " + t8.name + ";" << std::endl;
        MEMBER_ITER_END()
    }
    w2 << "    }" << std::endl;

    w2 << "    public void assignFromNumericArray(" << t6.java_type << t6.java_arr_type << " buffer, int offset)"
       << std::endl
       << "    {" << std::endl;
    {
        int32_t i = 0;
        MEMBER_ITER2(PropertyDefinition)
        TypeDefinition t7 = *JavaServiceLangGen_RemoveMultiDimArray(*m->Type);
        convert_type_result t8 = convert_type(t7);
        t8.name = fix_name(m->Name);
        if (IsTypeNumeric(m->Type->Type))
        {
            if (m->Type->ArrayType == DataTypes_ArrayTypes_none)
            {
                if (!unsigned_int)
                {
                    w2 << "    " << t8.name << " = buffer[offset + " << i << "];" << std::endl;
                }
                else
                {
                    w2 << "    " << t8.name << " = buffer.get(offset + " << i << ");" << std::endl;
                }
                i++;
            }
            else
            {
                if (!unsigned_int)
                {
                    w2 << "    System.arraycopy";
                }
                else
                {
                    w2 << "    DataTypeUtil.arraycopy";
                }
                w2 << "(buffer, offset + " << i << ", " << t8.name << ", 0, " << t7.ArrayLength.at(0) << "); "
                   << std::endl;
                i += t7.ArrayLength.at(0);
            }
        }
        else
        {
            RR_SHARED_PTR<ServiceEntryDefinition> e2 = rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType());
            boost::tuple<DataTypes, size_t> t9 = GetNamedArrayElementTypeAndCount(e2);
            size_t e2_count = m->Type->ArrayType == DataTypes_ArrayTypes_none ? 1 : t7.ArrayLength.at(0);

            if (m->Type->ArrayType == DataTypes_ArrayTypes_none)
            {
                w2 << "    " << t8.name << ".assignFromNumericArray(buffer, offset + " << i << ");" << std::endl;
            }
            else
            {
                w2 << "    " << m->Type->TypeString << ".assignFromNumericArray(" << t8.name << ", buffer, offset + "
                   << i << ");" << std::endl;
            }
            i += boost::numeric_cast<int32_t>(t9.get<1>() * e2_count);
        }
        // w2 << "    public " + t8.cs_type + t8.cs_arr_type + " " + t8.name + ";" << std::endl;
        MEMBER_ITER_END()
        w2 << "    }" << std::endl;

        w2 << "    public static " << t6.java_type << t6.java_arr_type << " getNumericArray(" << fix_name(e->Name)
           << "[] s)" << std::endl
           << "    {" << std::endl;
        w2 << "    " << t6.java_type << t6.java_arr_type << " a = new " << t6.java_type;
        if (!unsigned_int)
        {
            w2 << "[" << t4.get<1>() << " * s.length];" << std::endl;
        }
        else
        {
            w2 << t6.java_arr_type << "(" << t4.get<1>() << " * s.length);" << std::endl;
        }
        w2 << "    getNumericArray(s,a,0);" << std::endl;
        w2 << "    return a;" << std::endl;
        w2 << "    }" << std::endl;

        w2 << "    public static void getNumericArray(" << fix_name(e->Name) << "[] s, " << t6.java_type
           << t6.java_arr_type << " a, int offset)" << std::endl
           << "    {" << std::endl;
        // w2 << "    if(a.Count < " << t4.get<1>() << " * s.Length) throw new ArgumentException(\"ArraySegment invalid
        // length\");" << std::endl;
        w2 << "    for (int i=0; i<s.length; i++)" << std::endl << "    {" << std::endl;
        w2 << "    s[i].getNumericArray(a, offset + " << t4.get<1>() << " * i);" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;

        w2 << "    public static void assignFromNumericArray(" << fix_name(e->Name) << "[] s, " << t6.java_type
           << t6.java_arr_type << " a, int offset)" << std::endl
           << "    {" << std::endl;
        // w2 << "    if(a.Count < " << t1.get<1>() << " * s.Length) throw new ArgumentException(\"ArraySegment invalid
        // length\");" << std::endl;

        w2 << "    for (int i=0; i<s.length; i++)" << std::endl << "    {" << std::endl;
        w2 << "    s[i].assignFromNumericArray(a, offset + " << t4.get<1>() << " * i);" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;
    }

    w2 << "}" << std::endl << std::endl;
}

void JavaServiceLangGen::GenerateInterface(ServiceEntryDefinition* e, std::ostream* w)
{
    std::ostream& w2 = *w;

    GenerateDocString(e->DocString, "", w);

    std::vector<std::string> implements2;

    for (std::vector<std::string>::iterator ee = e->Implements.begin(); ee != e->Implements.end(); ee++)
    {
        implements2.push_back(fix_qualified_name(*ee));
    }

    std::string implements = boost::join(implements2, ", ");
    if (!e->Implements.empty())
        implements = " extends " + implements;

    w2 << "@RobotRaconteurServiceObjectInterface" << std::endl;
    w2 << "public interface " << fix_name(e->Name) << implements << std::endl << "{" << std::endl;

    MEMBER_ITER2(PropertyDefinition)
    GenerateDocString(m->DocString, "    ", w);
    convert_type_result t = convert_type(*m->Type);
    t.name = fix_name(m->Name);
    if (m->Direction() != MemberDefinition_Direction_writeonly)
    {
        w2 << "    " << t.java_type << t.java_arr_type << " get_" << t.name << "();" << std::endl;
    }
    if (m->Direction() != MemberDefinition_Direction_readonly)
    {
        w2 << "    void set_" << t.name << "(" << t.java_type << t.java_arr_type << " value);" << std::endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(FunctionDefinition)
    GenerateDocString(m->DocString, "    ", w);
    if (!m->IsGenerator())
    {
        convert_type_result t = convert_type(*m->ReturnType);
        std::string params = str_pack_parameters(m->Parameters, true);
        w2 << "    " << t.java_type << t.java_arr_type << " " << fix_name(m->Name) << "(" << params << ");"
           << std::endl;
    }
    else
    {
        convert_generator_result t = convert_generator(m.get());
        std::string params = str_pack_parameters(t.params, true);
        w2 << "    " << t.generator_java_type << " " << fix_name(m->Name) << "(" << params << ");" << std::endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(EventDefinition)
    GenerateDocString(m->DocString, "    ", w);
    w2 << "    void "
       << " add" << fix_name(m->Name) << "Listener(" << str_pack_delegate(m->Parameters) << " listener); " << std::endl;

    w2 << "    void "
       << " remove" << fix_name(m->Name) << "Listener(" << str_pack_delegate(m->Parameters) << " listener); "
       << std::endl;

    MEMBER_ITER_END()

    MEMBER_ITER2(ObjRefDefinition)
    GenerateDocString(m->DocString, "    ", w);
    std::string objtype = fix_qualified_name(m->ObjectType);
    if (objtype == "varobject")
        objtype = "Object";
    std::string indtype;
    if (GetObjRefIndType(m, indtype))
    {
        w2 << "    " << objtype << " get_" << fix_name(m->Name) << "(" << indtype << " ind);" << std::endl;
    }
    else
    {
        w2 << "    " << objtype << " get_" << fix_name(m->Name) << "();" << std::endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(PipeDefinition)
    GenerateDocString(m->DocString, "    ", w);
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    Pipe<" << t.java_type << t.java_arr_type << "> get_" << fix_name(m->Name) << "();" << std::endl;
    w2 << "    void set_" << fix_name(m->Name) << "(Pipe<" << t.java_type << t.java_arr_type << "> value);"
       << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(CallbackDefinition)
    GenerateDocString(m->DocString, "    ", w);
    w2 << "    Callback<" << str_pack_delegate(m->Parameters, m->ReturnType) << "> get_" << fix_name(m->Name) << "();"
       << std::endl;
    w2 << "    void set_" << fix_name(m->Name) << "(Callback<" << str_pack_delegate(m->Parameters, m->ReturnType)
       << "> value);" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    GenerateDocString(m->DocString, "    ", w);
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    Wire<" << t.java_type << t.java_arr_type << "> get_" << fix_name(m->Name) << "();" << std::endl;
    w2 << "    void set_" << fix_name(m->Name) << "(Wire<" << t.java_type << t.java_arr_type << "> value);"
       << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(MemoryDefinition)
    GenerateDocString(m->DocString, "    ", w);
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type_array(t2);
    std::string c;
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
        w2 << "    " << c << "ArrayMemory<" << t.java_type << t.java_arr_type << "> get_" << fix_name(m->Name) << "();"
           << std::endl;
        break;
    case DataTypes_ArrayTypes_multidimarray:
        w2 << "    " << c << "MultiDimArrayMemory<" << t.java_type << t.java_arr_type << "> get_" << fix_name(m->Name)
           << "();" << std::endl;
        break;
    default:
        throw DataTypeException("Invalid memory definition");
    }
    MEMBER_ITER_END()

    w2 << "}" << std::endl << std::endl;
}

void JavaServiceLangGen::GenerateAsyncInterface(ServiceEntryDefinition* e, std::ostream* w)
{
    std::ostream& w2 = *w;

    std::vector<std::string> implements2;

    for (std::vector<std::string>::iterator ee = e->Implements.begin(); ee != e->Implements.end(); ee++)
    {
        std::string iname = fix_qualified_name(*ee);
        if (iname.find('.') != std::string::npos)
        {
            boost::tuple<boost::string_ref, boost::string_ref> i1 = SplitQualifiedName(iname);
            iname = i1.get<0>() + ".async_" + i1.get<1>();
        }
        else
        {
            iname = "async_" + iname; // NOLINT(performance-inefficient-string-concatenation)
        }
        implements2.push_back(iname);
    }

    std::string implements = boost::join(implements2, ", ");
    if (!e->Implements.empty())
        implements = " extends " + implements;

    w2 << "public interface async_" << fix_name(e->Name) << implements << std::endl << "{" << std::endl;

    MEMBER_ITER2(PropertyDefinition)
    if (m->Direction() != MemberDefinition_Direction_writeonly)
    {
        convert_type_result t = convert_type(*m->Type, true);
        t.name = fix_name(m->Name);
        w2 << "    void async_get_" << t.name << "(Action2<" << t.java_type << t.java_arr_type
           << ",RuntimeException> rr_handler, int rr_timeout);" << std::endl;
    }
    if (m->Direction() != MemberDefinition_Direction_readonly)
    {
        convert_type_result t = convert_type(*m->Type, false);
        t.name = fix_name(m->Name);
        w2 << "    void async_set_" << t.name << "(" << t.java_type << t.java_arr_type
           << " value, Action1<RuntimeException> rr_handler, int rr_timeout);" << std::endl;
    }
    // w2 << "    " + t[1] + t[2] + " " + t[0] + " { get; set; }" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(FunctionDefinition)
    if (!m->IsGenerator())
    {
        convert_type_result t = convert_type(*m->ReturnType, true);
        std::string params = str_pack_parameters(m->Parameters, true);

        std::vector<std::string> t2;
        if (!m->Parameters.empty())
        {
            t2.push_back(params);
        }
        if (m->ReturnType->Type == DataTypes_void_t)
        {
            t2.push_back("Action1<RuntimeException> rr_handler");
        }
        else
        {
            t2.push_back("Action2<" + t.java_type + t.java_arr_type + ",RuntimeException> rr_handler");
        }
        t2.push_back("int rr_timeout");

        w2 << "    void async_" << fix_name(m->Name) << "(" << boost::join(t2, ",") << ");" << std::endl;
    }
    else
    {
        convert_generator_result t = convert_generator(m.get());
        std::string params = str_pack_parameters(t.params, true);

        std::vector<std::string> t2;
        if (!t.params.empty())
        {
            t2.push_back(params);
        }

        t2.push_back("Action2<" + t.generator_java_type + ", RuntimeException> rr_handler");

        t2.push_back("int rr_timeout");

        w2 << "    void async_" << fix_name(m->Name) << "(" << boost::join(t2, ",") << ");" << std::endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(ObjRefDefinition)
    std::string objtype = fix_qualified_name(m->ObjectType);
    if (objtype == "varobject")
        objtype = "Object";
    std::string indtype;
    if (GetObjRefIndType(m, indtype))
    {
        w2 << "    void async_get_" << fix_name(m->Name) << "(" << indtype << " ind, Action2<" << objtype
           << ",RuntimeException> handler, int timeout);" << std::endl;
    }
    else
    {
        w2 << "    void async_get_" << fix_name(m->Name) << "(Action2<" << objtype
           << ",RuntimeException> handler, int timeout);" << std::endl;
    }
    MEMBER_ITER_END()

    w2 << "}" << std::endl << std::endl;
}

void JavaServiceLangGen::GenerateServiceFactory(ServiceDefinition* d, const std::string& defstring, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "public class " << boost::replace_all_copy(fix_name(d->Name), ".", "__") << "Factory extends ServiceFactory"
       << std::endl
       << "{" << std::endl;
    w2 << "    public String defString()" << std::endl << "{" << std::endl;
    w2 << "    String out=\"";
    std::vector<std::string> lines;
    std::string s = defstring;
    boost::split(lines, s, boost::is_from_range('\n', '\n'));
    for (std::vector<std::string>::iterator e = lines.begin(); e != lines.end(); ++e)
    {
        std::string l = boost::replace_all_copy(*e, "\\", "\\\\");
        boost::replace_all(l, "\"", "\\\"");
        boost::replace_all(l, "\r", "");
        boost::trim(l);
        w2 << l << "\\n";
    }
    w2 << "\";" << std::endl;
    w2 << "    return out;";
    w2 << "    }" << std::endl;
    w2 << "    public String getServiceName() {return \"" << d->Name << "\";}" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        w2 << "    public " << fix_name((*e)->Name) << "_stub " << fix_name((*e)->Name) << "_stubentry;" << std::endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        w2 << "    public " << fix_name((*e)->Name) << "_stub " << fix_name((*e)->Name) << "_stubentry;" << std::endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        w2 << "    public " << fix_name((*e)->Name) << "_stub " << fix_name((*e)->Name) << "_stubentry;" << std::endl;
    }
    w2 << "    public " << boost::replace_all_copy(fix_name(d->Name), ".", "__") << "Factory()" << std::endl
       << "{" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        w2 << "    " << fix_name((*e)->Name) << "_stubentry=new " << fix_name((*e)->Name) << "_stub(this);"
           << std::endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        w2 << "    " << fix_name((*e)->Name) << "_stubentry=new " << fix_name((*e)->Name) << "_stub(this);"
           << std::endl;
    }
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        w2 << "    " << fix_name((*e)->Name) << "_stubentry=new " << fix_name((*e)->Name) << "_stub();" << std::endl;
    }
    w2 << "    }" << std::endl;

    w2 << "    public IStructureStub findStructureStub(String objecttype)" << std::endl << "    {" << std::endl;
    // w2 << "    String objshort=removePath(objecttype);" << std::endl;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        w2 << "    if (objecttype.equals(\"" << (*e)->Name << "\"))";
        w2 << "    return " << fix_name((*e)->Name) << "_stubentry;" << std::endl;
    }
    w2 << "    throw new DataTypeException(\"Cannot find appropriate structure stub\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public IPodStub findPodStub(String objecttype)" << std::endl << "    {" << std::endl;
    // w2 << "    String objshort=removePath(objecttype);" << std::endl;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end();
         ++e)
    {
        w2 << "    if (objecttype.equals(\"" << (*e)->Name << "\"))";
        w2 << "    return " << fix_name((*e)->Name) << "_stubentry;" << std::endl;
    }
    w2 << "    throw new DataTypeException(\"Cannot find appropriate structure stub\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public INamedArrayStub findNamedArrayStub(String objecttype)" << std::endl << "    {" << std::endl;
    // w2 << "    String objshort=removePath(objecttype);" << std::endl;

    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        w2 << "    if (objecttype.equals(\"" << (*e)->Name << "\"))";
        w2 << "    return " << fix_name((*e)->Name) << "_stubentry;" << std::endl;
    }
    w2 << "    throw new DataTypeException(\"Cannot find appropriate structure stub\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public ServiceStub createStub(WrappedServiceStub innerstub) {" << std::endl;
    w2 << "    String objecttype=innerstub.getRR_objecttype().getServiceDefinition().getName() + \".\" + "
          "innerstub.getRR_objecttype().getName();"
       << std::endl;
    w2 << "    if (RobotRaconteurNode.splitQualifiedName(objecttype)[0].equals( \"" << d->Name << "\")) {" << std::endl;
    w2 << "    String objshort=removePath(objecttype);" << std::endl;
    // w2 << "    switch (objshort) {" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        std::string objname = (*e)->Name;
        w2 << "    if(objshort.equals( \"" << objname << "\"))" << std::endl;
        w2 << "    return new " << fix_name(objname) << "_stub(innerstub);" << std::endl;
    }
    // w2 << "    default:" << std::endl;
    // w2 << "    break;" << std::endl;
    // w2 << "    }" << std::endl;
    w2 << "    } else {" << std::endl;
    w2 << "    String ext_service_type=(RobotRaconteurNode.splitQualifiedName(objecttype)[0]);" << std::endl;
    w2 << "    return RobotRaconteurNode.s().getServiceType(ext_service_type).createStub(innerstub);" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    throw new ServiceException(\"Could not create stub\");" << std::endl;

    w2 << "    }" << std::endl;

    w2 << "    public ServiceSkel createSkel(Object obj) {" << std::endl;
    w2 << "    String objtype=ServiceSkelUtil.findParentInterface(obj.getClass()).getName().toString();" << std::endl;
    w2 << "    if ((RobotRaconteurNode.splitQualifiedName(objtype.toString())[0]).equals( \"" << d->Name << "\")) {"
       << std::endl;
    w2 << "    String sobjtype=removePath(objtype);" << std::endl;
    // w2 << "    switch(sobjtype) {" << std::endl;
    for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        std::string objname = (*e)->Name;
        w2 << "    if(sobjtype.equals( \"" << objname << "\"))" << std::endl;
        w2 << "    return new " << fix_name(objname) << "_skel((" << fix_name(objname) << ")obj);" << std::endl;
    }
    // w2 << "    default:" << std::endl;
    // w2 << "    break;" << std::endl;
    // w2 << "    }" << std::endl;
    w2 << "    } else {" << std::endl;
    w2 << "    String ext_service_type=(RobotRaconteurNode.splitQualifiedName(objtype.toString())[0]);" << std::endl;
    w2 << "    return RobotRaconteurNode.s().getServiceType(ext_service_type).createSkel(obj);" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    throw new ServiceException(\"Could not create skel\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public RobotRaconteurException downCastException(RobotRaconteurException rr_exp)"
       << "{" << std::endl;
    w2 << "    if (rr_exp==null) return rr_exp;" << std::endl;
    w2 << "    String rr_type=rr_exp.error;" << std::endl;
    w2 << "    if (!rr_type.contains(\".\")) return rr_exp;" << std::endl;
    w2 << "    String[] rr_stype = RobotRaconteurNode.splitQualifiedName(rr_type);" << std::endl;
    w2 << "    if (!rr_stype[0].equals(\"" << d->Name << "\")) return RobotRaconteurNode.s().downCastException(rr_exp);"
       << std::endl;
    BOOST_FOREACH (const RR_SHARED_PTR<ExceptionDefinition>& e, d->Exceptions)
    {
        w2 << "    if (rr_stype[1].equals(\"" << e->Name << "\")) return new " << fix_name(e->Name)
           << "(rr_exp.getMessage(),rr_exp.errorSubName,rr_exp.errorParam);" << std::endl;
    }
    w2 << "    return rr_exp;" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "}" << std::endl;
}

void JavaServiceLangGen::GenerateStructureStub(ServiceEntryDefinition* e, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "public class " << fix_name(e->Name) << "_stub implements IStructureStub {" << std::endl;
    w2 << "    public " << fix_name(e->Name) << "_stub("
       << boost::replace_all_copy(fix_name(e->ServiceDefinition_.lock()->Name), ".", "__") + "Factory d) {def=d;}"
       << std::endl;
    w2 << "    private " << boost::replace_all_copy(fix_name(e->ServiceDefinition_.lock()->Name), ".", "__")
       << "Factory def;" << std::endl;
    w2 << "    public MessageElementNestedElementList packStructure(Object s1) {" << std::endl;

    w2 << "    vectorptr_messageelement m=new vectorptr_messageelement();" << std::endl;
    w2 << "    try {" << std::endl;
    w2 << "    if (s1 ==null) return null;" << std::endl;
    w2 << "    " << fix_name(e->Name) << " s = (" << fix_name(e->Name) << ")s1;" << std::endl;
    MEMBER_ITER2(PropertyDefinition)
    w2 << "    MessageElementUtil.addMessageElementDispose(m,"
       << str_pack_message_element(m->Name, "s." + fix_name(m->Name), m->Type, "def") << ");" << std::endl;
    MEMBER_ITER_END()
    w2 << "    return new MessageElementNestedElementList(DataTypes.DataTypes_structure_t,\""
       << e->ServiceDefinition_.lock()->Name << "." << e->Name << "\",m);" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    finally {" << std::endl;
    w2 << "    m.delete();" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;

    // Write Read
    w2 << "    public <T> T unpackStructure(MessageElementData m) {" << std::endl;

    w2 << "    if (m == null ) return null;" << std::endl;
    w2 << "    MessageElementNestedElementList m2 = (MessageElementNestedElementList)m;" << std::endl;
    w2 << "    vectorptr_messageelement mm=m2.getElements();" << std::endl;
    w2 << "    try {" << std::endl;
    w2 << "    " << fix_name(e->Name) << " s=new " << fix_name(e->Name) << "();" << std::endl;
    MEMBER_ITER2(PropertyDefinition)
    convert_type_result t = convert_type(*m->Type);
    t.name = m->Name;

    w2 << "    s." << fix_name(t.name) << " ="
       << str_unpack_message_element("MessageElement.findElement(mm,\"" + t.name + "\")", m->Type, "def") << ";"
       << std::endl;

    MEMBER_ITER_END()
    // w2 << "    if ((s as T)==null) throw new DataTypeException(\"Incorrect structure cast\");");
    w2 << "    T st; try {st=(T)s;} catch (Exception e) {throw new RuntimeException(new "
          "DataTypeMismatchException(\"Wrong structuretype\"));}"
       << std::endl;
    w2 << "    return st;" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    finally {" << std::endl;
    w2 << "    if (mm!=null) mm.delete();" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "}" << std::endl;
}

void JavaServiceLangGen::GeneratePodStub(ServiceEntryDefinition* e, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "public class " << fix_name(e->Name) << "_stub extends PodStub<" << fix_name(e->Name) << "> {" << std::endl;
    w2 << "    public " << fix_name(e->Name) << "_stub("
       << boost::replace_all_copy(fix_name(e->ServiceDefinition_.lock()->Name), ".", "__") << "Factory d) {def=d;}"
       << std::endl;
    w2 << "    private " << boost::replace_all_copy(fix_name(e->ServiceDefinition_.lock()->Name), ".", "__")
       << "Factory def;" << std::endl;
    w2 << "    public MessageElementNestedElementList packPod(RRPod s1) {" << std::endl;
    w2 << "    vectorptr_messageelement m=new vectorptr_messageelement();" << std::endl;
    w2 << "    try {" << std::endl;
    w2 << "    " << fix_qualified_name(e->Name) << " s = (" << fix_qualified_name(e->Name) << ")s1;" << std::endl;
    MEMBER_ITER2(PropertyDefinition)
    RR_SHARED_PTR<TypeDefinition> t2 = JavaServiceLangGen_RemoveMultiDimArray(*m->Type);
    w2 << "    MessageElementUtil.addMessageElementDispose(m,"
       << str_pack_message_element(m->Name, "s." + fix_name(m->Name), t2, "def") << ");" << std::endl;
    MEMBER_ITER_END()
    w2 << "    return new MessageElementNestedElementList(DataTypes.DataTypes_pod_t,\"\",m);" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    finally {" << std::endl;
    w2 << "    m.delete();" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;

    // Write Read
    w2 << "    public " << fix_name(e->Name) << " unpackPod(MessageElementNestedElementList m) {" << std::endl;

    w2 << "    if (m == null ) throw new NullPointerException(\"Pod must not be null\");" << std::endl;
    w2 << "    vectorptr_messageelement mm=m.getElements();" << std::endl;
    w2 << "    try {" << std::endl;
    w2 << "    " << fix_name(e->Name) << " s = new " << fix_name(e->Name) << "();" << std::endl;
    MEMBER_ITER2(PropertyDefinition)
    convert_type_result t = convert_type(*m->Type);
    t.name = m->Name;
    RR_SHARED_PTR<TypeDefinition> t2 = JavaServiceLangGen_RemoveMultiDimArray(*m->Type);
    w2 << "    s." << fix_name(t.name) << " ="
       << str_unpack_message_element("MessageElement.findElement(mm,\"" + t.name + "\")", t2, "def") << ";"
       << std::endl;

    MEMBER_ITER_END()

    w2 << "    return s;" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    finally {" << std::endl;
    w2 << "    mm.delete();" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public String getTypeName() {return \"" << fix_qualified_name(e->ServiceDefinition_.lock()->Name) << "."
       << fix_name(e->Name) << "\";}" << std::endl;
    ;
    w2 << "    protected " << fix_name(e->Name) << "[] createArray(int count) { return new " << fix_name(e->Name)
       << "[count]; }" << std::endl;
    w2 << "}" << std::endl;
}

void JavaServiceLangGen::GenerateNamedArrayStub(const RR_SHARED_PTR<ServiceEntryDefinition>& e, std::ostream* w)
{
    std::ostream& w2 = *w;

    boost::tuple<DataTypes, size_t> t4 = GetNamedArrayElementTypeAndCount(e);
    TypeDefinition t5;
    t5.Type = t4.get<0>();
    t5.ArrayType = DataTypes_ArrayTypes_array;
    convert_type_result t6 = convert_type(t5);

    bool unsigned_int = (t5.Type == DataTypes_uint8_t || t5.Type == DataTypes_uint16_t ||
                         t5.Type == DataTypes_uint32_t || t5.Type == DataTypes_uint64_t);

    w2 << "public class " << fix_name(e->Name) << "_stub extends NamedArrayStub<" << fix_name(e->Name) << ","
       << t6.java_type << t6.java_arr_type << "> {" << std::endl;
    w2 << "    public " << t6.java_type << t6.java_arr_type << " getNumericArrayFromNamedArrayStruct("
       << fix_name(e->Name) << " s) {" << std::endl;
    w2 << "    return s.getNumericArray();" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    public " << fix_name(e->Name) << " getNamedArrayStructFromNumericArray(" << t6.java_type
       << t6.java_arr_type << " m) {" << std::endl;
    // w2 << "    if (m.lengthength != " << t4.get<1>() << ") throw new DataTypeException(\"Invalid namedarray
    // array\");" << std::endl;
    w2 << "    " << fix_name(e->Name) << " s = new " << fix_name(e->Name) << "();" << std::endl;
    w2 << "    s.assignFromNumericArray(m,0);" << std::endl;
    w2 << "    return s;" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    public " << t6.java_type << t6.java_arr_type << " getNumericArrayFromNamedArray(" << fix_name(e->Name)
       << "[] s) {" << std::endl;
    w2 << "    return " << fix_name(e->Name) << ".getNumericArray(s);" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    public " << fix_name(e->Name) << "[] getNamedArrayFromNumericArray(" << t6.java_type << t6.java_arr_type
       << " m) {" << std::endl;
    // w2 << "    if (m.Length % " << t4.get<1>() << " != 0) throw new DataTypeException(\"Invalid namedarray array\");"
    // << std::endl;
    w2 << "    " << fix_name(e->Name) << "[] s = new " << fix_name(e->Name);
    if (!unsigned_int)
    {
        w2 << "[m.length / " << t4.get<1>() << "];" << std::endl;
    }
    else
    {
        w2 << "[m.value.length / " << t4.get<1>() << "];" << std::endl;
    }
    w2 << "    for (int i=0; i<s.length; i++) s[i] = new " << fix_name(e->Name) << "();" << std::endl;
    w2 << "    " << fix_name(e->Name) << ".assignFromNumericArray(s,m,0);" << std::endl;
    w2 << "    return s;" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    public String getTypeName() { return \"" << e->ServiceDefinition_.lock()->Name << "." << e->Name
       << "\"; }";

    w2 << "}" << std::endl;
}

void JavaServiceLangGen::GenerateStub(ServiceEntryDefinition* e, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "public class " << fix_name(e->Name) << "_stub extends ServiceStub implements " << fix_name(e->Name)
       << ", async_" << fix_name(e->Name) << " {" << std::endl;

    MEMBER_ITER2(CallbackDefinition)
    w2 << "    private CallbackClient<" << str_pack_delegate(m->Parameters, m->ReturnType) << "> rr_"
       << fix_name(m->Name) << ";" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(PipeDefinition)
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    private Pipe<" << t.java_type << t.java_arr_type << "> rr_" << fix_name(m->Name) << ";" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    private Wire<" << t.java_type << t.java_arr_type << "> rr_" << fix_name(m->Name) << ";" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(MemoryDefinition)

    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type_array(t2);
    std::string c;
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
        w2 << "    private " << c << "ArrayMemory<" << t.java_type << t.java_arr_type << "> rr_" << fix_name(m->Name)
           << ";" << std::endl;
        break;
    case DataTypes_ArrayTypes_multidimarray:
        w2 << "    private " << c << "MultiDimArrayMemory<" << t.java_type << t.java_arr_type << "> rr_"
           << fix_name(m->Name) << ";" << std::endl;
        break;
    default:
        throw DataTypeException("Invalid memory definition");
    }

    MEMBER_ITER_END()

    w2 << "    public " << fix_name(e->Name) << "_stub(WrappedServiceStub innerstub) {" << std::endl;
    w2 << "        super(innerstub); " << std::endl;
    MEMBER_ITER2(CallbackDefinition)
    w2 << "    rr_" << fix_name(m->Name) << "=new CallbackClient<" << str_pack_delegate(m->Parameters, m->ReturnType)
       << ">(\"" << m->Name << "\");" << std::endl;
    MEMBER_ITER_END()
    MEMBER_ITER2(PipeDefinition)
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    rr_" << fix_name(m->Name) << "=new Pipe<" << t.java_type << t.java_arr_type << ">(innerstub.getPipe(\""
       << m->Name << "\"));" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    rr_" << fix_name(m->Name) << "=new Wire<" << t.java_type << t.java_arr_type << ">(innerstub.getWire(\""
       << m->Name << "\"));" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type_array(t2);
    std::string c;
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
        w2 << "    rr_" << fix_name(m->Name) << "=new " << c << "ArrayMemoryClient<" << t.java_type << t.java_arr_type
           << ">(innerstub.get" << c << "ArrayMemory(\"" << m->Name << "\"));" << std::endl;
        break;
    case DataTypes_ArrayTypes_multidimarray:
        w2 << "    rr_" << fix_name(m->Name) << "=new " << c << "MultiDimArrayMemoryClient<" << t.java_type
           << t.java_arr_type << ">(innerstub.get" << c << "MultiDimArrayMemory(\"" << m->Name << "\"));" << std::endl;
        break;
    default:
        throw DataTypeException("Invalid memory definition");
    }
    MEMBER_ITER_END()

    w2 << "    }" << std::endl;

    MEMBER_ITER2(PropertyDefinition)
    convert_type_result t = convert_type(*m->Type);
    t.name = m->Name;
    if (m->Direction() != MemberDefinition_Direction_writeonly)
    {
        w2 << "    public " << t.java_type << t.java_arr_type << " get_" << fix_name(t.name) << "() {" << std::endl;
        w2 << "    return " << str_unpack_message_element("rr_innerstub.propertyGet(\"" + m->Name + "\")", m->Type)
           << ";" << std::endl;
        w2 << "    }" << std::endl;
    }
    if (m->Direction() != MemberDefinition_Direction_readonly)
    {
        w2 << "    public void set_" << fix_name(t.name) << "(" << t.java_type << t.java_arr_type << " value) {"
           << std::endl;
        w2 << "    MessageElement m=null;" << std::endl;
        w2 << "    try {" << std::endl;
        w2 << "    m=" << str_pack_message_element("value", "value", m->Type) << ";" << std::endl;
        w2 << "    rr_innerstub.propertySet(\"" + m->Name + "\", m);" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    finally {" << std::endl;
        w2 << "    if (m!=null) m.delete();" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;
    }

    MEMBER_ITER_END()

    MEMBER_ITER2(FunctionDefinition)
    if (!m->IsGenerator())
    {
        convert_type_result t = convert_type(*m->ReturnType);
        std::string params = str_pack_parameters(m->Parameters, true);
        w2 << "    public " << t.java_type << t.java_arr_type << " " << fix_name(m->Name) << "(" << params << ") {"
           << std::endl;
        w2 << "    vectorptr_messageelement rr_param=new vectorptr_messageelement();" << std::endl;
        w2 << "    MessageElement rr_me=null;" << std::endl;
        w2 << "    try {" << std::endl;
        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin();
             p != m->Parameters.end(); ++p)
        {
            w2 << "    MessageElementUtil.addMessageElementDispose(rr_param, "
               << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << std::endl;
        }
        w2 << "    rr_me=rr_innerstub.functionCall(\"" << m->Name << "\",rr_param);" << std::endl;
        if (m->ReturnType->Type != DataTypes_void_t)
        {
            w2 << "    return " << str_unpack_message_element("rr_me", m->ReturnType) << ";" << std::endl;
        }
        w2 << "    }" << std::endl;
        w2 << "    finally {" << std::endl;
        w2 << "    rr_param.delete();" << std::endl;
        w2 << "    if (rr_me!=null) rr_me.delete();" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;
    }
    else
    {
        convert_generator_result t = convert_generator(m.get());
        std::string params = str_pack_parameters(t.params, true);
        w2 << "    public " << t.generator_java_type << " " << fix_name(m->Name) << "(" << params << ") {" << std::endl;
        w2 << "    vectorptr_messageelement rr_param=new vectorptr_messageelement();" << std::endl;
        w2 << "    try {" << std::endl;
        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = t.params.begin(); p != t.params.end(); ++p)
        {
            w2 << "    MessageElementUtil.addMessageElementDispose(rr_param,"
               << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << std::endl;
        }
        w2 << "    WrappedGeneratorClient generator_client = rr_innerstub.generatorFunctionCall(\"" << m->Name
           << "\",rr_param);" << std::endl;

        w2 << "    return new " << t.generator_java_base_type << "Client<" << t.generator_java_template_params
           << ">(generator_client);" << std::endl;

        w2 << "    }" << std::endl;
        w2 << "    finally {" << std::endl;
        w2 << "    rr_param.delete();" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(EventDefinition)
    std::string params = str_pack_parameters(m->Parameters, true);
    w2 << "    private  Vector<" << str_pack_delegate(m->Parameters) << ">"
       << " rr_" << fix_name(m->Name) << "=new Vector<" << str_pack_delegate(m->Parameters) << ">();" << std::endl;
    w2 << "    public void "
       << " add" << fix_name(m->Name) << "Listener(" << str_pack_delegate(m->Parameters) << " listener) {" << std::endl;
    w2 << "    synchronized(rr_" << fix_name(m->Name) << ") {" << std::endl;
    w2 << "    rr_" << fix_name(m->Name) << ".add(listener);" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    public void "
       << " remove" << fix_name(m->Name) << "Listener(" << str_pack_delegate(m->Parameters) << " listener) {"
       << std::endl;
    w2 << "    synchronized(rr_" << fix_name(m->Name) << ") {" << std::endl;
    w2 << "    rr_" << fix_name(m->Name) << ".remove(listener);" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;
    MEMBER_ITER_END()

    w2 << "    public void dispatchEvent(String rr_membername, vectorptr_messageelement rr_m) {" << std::endl;
    // w2 << "    switch (rr_membername) {" << std::endl;
    MEMBER_ITER2(EventDefinition)
    std::string params = str_pack_parameters_delegate(m->Parameters, false);
    w2 << "    if(rr_membername.equals( \"" << m->Name << "\"))" << std::endl << "    {" << std::endl;

    for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin();
         p != m->Parameters.end(); ++p)
    {
        convert_type_result t3 = convert_type(*(*p));
        w2 << "    " << t3.java_type << t3.java_arr_type << " " << fix_name((*p)->Name) << "="
           << str_unpack_message_element("vectorptr_messageelement_util.findElement(rr_m,\"" + (*p)->Name + "\")", *p)
           << ";" << std::endl;
        ;
    }
    w2 << "    for (" << str_pack_delegate(m->Parameters) << " rr_e : this.rr_" << fix_name(m->Name) << ") {"
       << std::endl;
    w2 << "    rr_e.action(" << params << ");" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    return;" << std::endl;
    w2 << "    }" << std::endl;
    MEMBER_ITER_END()
    // w2 << "    default:" << std::endl;
    // w2 << "    break;" << std::endl;
    // w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;

    MEMBER_ITER2(ObjRefDefinition)
    std::string objtype = fix_qualified_name(m->ObjectType);
    if (objtype == "varobject")
    {
        objtype = "Object";
        std::string indtype;
        if (GetObjRefIndType(m, indtype))
        {
            w2 << "    public " << objtype << " get_" << fix_name(m->Name) << "(" << indtype << " ind) {" << std::endl;
            w2 << "    return (" << objtype << ")findObjRef(\"" << m->Name << "\",String.valueOf(ind));" << std::endl;
            w2 << "    }" << std::endl;
        }
        else
        {
            w2 << "    public " << objtype << " get_" << fix_name(m->Name) << "() {" << std::endl;
            w2 << "    return (" << objtype << ")findObjRef(\"" << m->Name << "\");" << std::endl;
            w2 << "    }" << std::endl;
        }
    }
    else
    {
        boost::shared_ptr<ServiceDefinition> d = e->ServiceDefinition_.lock();
        if (!d)
            throw DataTypeException("Invalid object type name");

        std::string objecttype2;

        if (m->ObjectType.find('.') == std::string::npos)
        {
            objecttype2 = fix_name(d->Name) + "." + fix_name(m->ObjectType);
        }
        else
        {
            objecttype2 = fix_name(m->ObjectType);
        }

        std::string indtype;
        if (GetObjRefIndType(m, indtype))
        {
            w2 << "    public " << objtype << " get_" << fix_name(m->Name) << "(" << indtype << " ind) {" << std::endl;
            w2 << "    return (" << objtype << ")findObjRefTyped(\"" << m->Name << "\",String.valueOf(ind),\""
               << objecttype2 << "\");" << std::endl;
            w2 << "    }" << std::endl;
        }
        else
        {
            w2 << "    public " << objtype << " get_" << fix_name(m->Name) << "() {" << std::endl;
            w2 << "    return (" << objtype << ")findObjRefTyped(\"" << m->Name << "\",\"" << objecttype2 << "\");"
               << std::endl;
            w2 << "    }" << std::endl;
        }
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(PipeDefinition)
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    public Pipe<" << t.java_type << t.java_arr_type << "> get_" << fix_name(m->Name) << "()" << std::endl;
    w2 << "    { return rr_" << m->Name << ";  }" << std::endl;
    w2 << "    public void set_" << fix_name(m->Name) << "(Pipe<" << t.java_type << t.java_arr_type << "> value)"
       << std::endl;
    w2 << "    { throw new RuntimeException();}" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(CallbackDefinition)
    w2 << "    public Callback<" << str_pack_delegate(m->Parameters, m->ReturnType) << "> get_" << fix_name(m->Name)
       << "()" << std::endl;
    w2 << "    { return rr_" << fix_name(m->Name) << ";  }" << std::endl;
    w2 << "    public void set_" << fix_name(m->Name) << "(Callback<" << str_pack_delegate(m->Parameters, m->ReturnType)
       << "> value)" << std::endl;
    w2 << "    { throw new RuntimeException();}" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    public Wire<" << t.java_type << t.java_arr_type << "> get_" << fix_name(m->Name) << "()" << std::endl;
    w2 << "    { return rr_" << fix_name(m->Name) << ";  }" << std::endl;
    w2 << "    public void set_" << fix_name(m->Name) << "(Wire<" << t.java_type << t.java_arr_type << "> value)"
       << std::endl;
    w2 << "    { throw new RuntimeException();}" << std::endl;
    MEMBER_ITER_END()

    w2 << "    public MessageElement callbackCall(String rr_membername, vectorptr_messageelement rr_m) {" << std::endl;
    // w2 << "    switch (rr_membername) {" << std::endl;
    MEMBER_ITER2(CallbackDefinition)
    std::string params = str_pack_parameters_delegate(m->Parameters, false);
    w2 << "    if(rr_membername.equals( \"" << m->Name << "\"))" << std::endl << "    {" << std::endl;

    for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin();
         p != m->Parameters.end(); ++p)
    {
        convert_type_result t3 = convert_type(*(*p));
        w2 << "    " << t3.java_type << t3.java_arr_type << " " << fix_name((*p)->Name) << "="
           << str_unpack_message_element("vectorptr_messageelement_util.findElement(rr_m,\"" + (*p)->Name + "\")", *p)
           << ";" << std::endl;
        ;
    }
    if (m->ReturnType->Type == DataTypes_void_t)
    {
        w2 << "    get_" << fix_name(m->Name) << "().getFunction().action(" << params << ");" << std::endl;
        w2 << "    return new MessageElement(\"return\",new int[] {0});" << std::endl;
    }
    else
    {
        convert_type_result t = convert_type(*m->ReturnType);
        w2 << "    " << t.java_type << t.java_arr_type << " rr_ret=get_" << fix_name(m->Name)
           << "().getFunction().func(" << params << ");" << std::endl;
        w2 << "    return " << str_pack_message_element("return", "rr_ret", m->ReturnType) << ";" << std::endl;
    }
    w2 << "    }" << std::endl;
    MEMBER_ITER_END()
    // w2 << "    default:" << std::endl;
    // w2 << "    break;" << std::endl;
    // w2 << "    }" << std::endl;
    w2 << "    throw new MemberNotFoundException(\"Member not found\");" << std::endl;
    w2 << "    }" << std::endl;

    MEMBER_ITER2(MemoryDefinition)

    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type_array(t2);
    std::string c;
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
        w2 << "    public " << c << "ArrayMemory<" << t.java_type << t.java_arr_type << "> get_" << fix_name(m->Name)
           << "()" << std::endl;
        break;
    case DataTypes_ArrayTypes_multidimarray:
        w2 << "    public " << c << "MultiDimArrayMemory<" << t.java_type << t.java_arr_type << "> get_"
           << fix_name(m->Name) << "()" << std::endl;
        break;
    default:
        throw DataTypeException("Invalid memory definition");
    }
    w2 << "    { return rr_" << fix_name(m->Name) << "; }" << std::endl;

    MEMBER_ITER_END()

    // Async functions

    MEMBER_ITER2(PropertyDefinition)
    convert_type_result t = convert_type(*m->Type, true);
    t.name = fix_name(m->Name);
    if (m->Direction() != MemberDefinition_Direction_writeonly)
    {
        w2 << "    public void async_get_" << t.name << "(Action2<" << t.java_type << t.java_arr_type
           << ",RuntimeException> rr_handler, int rr_timeout)" << std::endl
           << "    {" << std::endl;
        w2 << "    rr_async_PropertyGet(\"" << m->Name << "\",new rrend_async_get_" << t.name
           << "(),rr_handler,rr_timeout);" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    protected class rrend_async_get_" << t.name
           << " implements Action3<MessageElement,RuntimeException,Object> {" << std::endl;
        w2 << "    public void action(MessageElement value ,RuntimeException err,Object param)" << std::endl
           << "    {" << std::endl;
        w2 << "    Action2<" << t.java_type << t.java_arr_type << ",RuntimeException> rr_handler=(Action2<"
           << t.java_type << t.java_arr_type << ",RuntimeException>)param;" << std::endl;
        w2 << "    if (err!=null)" << std::endl
           << "    {" << std::endl
           << "    rr_handler.action(" << GetDefaultValue(*m->Type) << ",err);" << std::endl
           << "    return;" << std::endl
           << "    }" << std::endl;
        w2 << "    " << t.java_type << t.java_arr_type << " rr_ret;" << std::endl;
        w2 << "    try {" << std::endl;
        w2 << "    rr_ret=" << str_unpack_message_element("value", m->Type) << ";" << std::endl;
        w2 << "    } catch (RuntimeException err2) {" << std::endl;
        w2 << "    rr_handler.action(" << GetDefaultValue(*m->Type) << ",err2);" << std::endl;
        w2 << "    return;" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    rr_handler.action(rr_ret,null);" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;
    }
    if (m->Direction() != MemberDefinition_Direction_readonly)
    {
        convert_type_result t = convert_type(*m->Type, false);
        t.name = fix_name(m->Name);
        w2 << "    public void async_set_" << t.name << "(" << t.java_type << t.java_arr_type
           << " value, Action1<RuntimeException> rr_handler, int rr_timeout)" << std::endl
           << "    {" << std::endl;
        w2 << "    MessageElement m=null;" << std::endl;
        w2 << "    try {" << std::endl;
        w2 << "    m=" + str_pack_message_element("value", "value", m->Type) << ";" << std::endl;
        w2 << "    rr_async_PropertySet(\"" << m->Name << "\",m,new rrend_async_set_" << t.name
           << "(),rr_handler,rr_timeout);" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    finally {" << std::endl;
        w2 << "    if (m!=null) m.delete();" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;

        w2 << "    protected class rrend_async_set_" << t.name
           << " implements Action3<MessageElement,RuntimeException,Object> {" << std::endl;
        w2 << "    public void action(MessageElement m ,RuntimeException err,Object param)" << std::endl
           << "    {" << std::endl;
        w2 << "    Action1<RuntimeException> rr_handler=(Action1<RuntimeException>)param;" << std::endl;
        w2 << "    if (err!=null)" << std::endl
           << "    {" << std::endl
           << "    rr_handler.action(err);" << std::endl
           << "    return;" << std::endl
           << "    }" << std::endl;
        w2 << "    rr_handler.action(null);" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;
    }
    // w2 << "    " + t[1] + t[2] + " " + t[0] + " { get; set; }" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(FunctionDefinition)
    if (!m->IsGenerator())
    {
        convert_type_result t = convert_type(*m->ReturnType, true);
        std::string params = str_pack_parameters(m->Parameters, true);

        std::vector<std::string> t2;
        if (!m->Parameters.empty())
        {
            t2.push_back(params);
        }
        if (m->ReturnType->Type == DataTypes_void_t)
        {
            t2.push_back("Action1<RuntimeException> rr_handler");
        }
        else
        {
            t2.push_back("Action2<" + t.java_type + t.java_arr_type + ",RuntimeException> rr_handler");
        }
        t2.push_back("int rr_timeout");

        w2 << "    public void async_" << fix_name(m->Name) << "(" << boost::join(t2, ",") << ")" << std::endl
           << "    {" << std::endl;
        w2 << "    vectorptr_messageelement rr_param=new vectorptr_messageelement();" << std::endl;
        w2 << "    try {" << std::endl;
        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin();
             p != m->Parameters.end(); ++p)
        {
            w2 << "    MessageElementUtil.addMessageElementDispose(rr_param,"
               << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << std::endl;
        }

        w2 << "    rr_async_FunctionCall(\"" << m->Name << "\",rr_param,new rrend_async_" << fix_name(m->Name)
           << "(),rr_handler,rr_timeout);" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    finally {" << std::endl;
        w2 << "    rr_param.delete();" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    protected class rrend_async_" << fix_name(m->Name)
           << " implements Action3<MessageElement,RuntimeException,Object> {" << std::endl;
        w2 << "    public void action(MessageElement ret ,RuntimeException err,Object param)" << std::endl
           << "    {" << std::endl;
        if (m->ReturnType->Type == DataTypes_void_t)
        {
            w2 << "    Action1<RuntimeException> rr_handler=(Action1<RuntimeException>)param;" << std::endl;
            w2 << "    if (err!=null)" << std::endl
               << "    {" << std::endl
               << "    rr_handler.action(err);" << std::endl
               << "    return;" << std::endl
               << "    }" << std::endl;
            w2 << "    rr_handler.action(null);" << std::endl;
        }
        else
        {
            w2 << "    Action2<" << t.java_type << t.java_arr_type << ",RuntimeException> rr_handler=(Action2<"
               << t.java_type << t.java_arr_type << ",RuntimeException>)param;" << std::endl;
            w2 << "    if (err!=null)" << std::endl
               << "    {" << std::endl
               << "    rr_handler.action(" << GetDefaultValue(*m->ReturnType) << ",err);" << std::endl
               << "    return;" << std::endl
               << "    }" << std::endl;
            w2 << "    " << t.java_type << t.java_arr_type << " rr_ret;" << std::endl;
            w2 << "    try {" << std::endl;
            w2 << "    rr_ret=" << str_unpack_message_element("ret", m->ReturnType) << ";" << std::endl;
            w2 << "    } catch (RuntimeException err2) {" << std::endl;
            w2 << "    rr_handler.action(" << GetDefaultValue(*m->ReturnType) << ",err2);" << std::endl;
            w2 << "    return;" << std::endl;
            w2 << "    }" << std::endl;
            w2 << "    rr_handler.action(rr_ret, null);" << std::endl;
        }
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;
    }
    else
    {
        convert_generator_result t = convert_generator(m.get());
        std::string params = str_pack_parameters(t.params, true);

        std::vector<std::string> t2;
        if (!t.params.empty())
        {
            t2.push_back(params);
        }

        t2.push_back("Action2<" + t.generator_java_type + ", RuntimeException> rr_handler");

        t2.push_back("int rr_timeout");

        w2 << "    public void async_" << fix_name(m->Name) << "(" << boost::join(t2, ",") << ")" << std::endl
           << "    {" << std::endl;
        w2 << "    vectorptr_messageelement rr_param=new vectorptr_messageelement();" << std::endl;
        w2 << "    try {" << std::endl;
        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = t.params.begin(); p != t.params.end(); ++p)
        {
            w2 << "    MessageElementUtil.addMessageElementDispose(rr_param,"
               << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << std::endl;
        }

        w2 << "    rr_async_GeneratorFunctionCall(\"" << m->Name << "\",rr_param,new rrend_async_" << fix_name(m->Name)
           << "(),rr_handler,rr_timeout);" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    finally {" << std::endl;
        w2 << "    rr_param.delete();" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    protected class rrend_async_" << fix_name(m->Name)
           << " implements Action3<WrappedGeneratorClient, RuntimeException, Object> {" << std::endl;
        w2 << "    public void action(WrappedGeneratorClient ret, RuntimeException err, Object param)" << std::endl
           << "    {" << std::endl;

        w2 << "    Action2<" << t.generator_java_type << ",RuntimeException> rr_handler=(Action2<"
           << t.generator_java_type << ",RuntimeException>)param;" << std::endl;
        w2 << "    if (err!=null)" << std::endl
           << "    {" << std::endl
           << "    rr_handler.action(null,err);" << std::endl
           << "    return;" << std::endl
           << "    }" << std::endl;
        w2 << "    " << t.generator_java_base_type << "Client< " << t.generator_java_template_params << "> rr_ret=new "
           << t.generator_java_base_type << "Client< " << t.generator_java_template_params << ">(ret);" << std::endl;
        w2 << "    rr_handler.action(rr_ret,null);" << std::endl;
        w2 << "    }" << std::endl;
        w2 << "    }" << std::endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(ObjRefDefinition)
    std::string objtype = fix_qualified_name(m->ObjectType);
    if (objtype == "varobject")
    {
        objtype = "Object";
        std::string indtype;
        if (GetObjRefIndType(m, indtype))
        {
            w2 << "    public void async_get_" << fix_name(m->Name) << "(" << indtype << " ind, Action2<" << objtype
               << ",RuntimeException> handler, int timeout) {" << std::endl;
            w2 << "    asyncFindObjRef(\"" << m->Name << "\",String.valueOf(ind),handler,timeout);" << std::endl;
            w2 << "    }" << std::endl;
        }
        else
        {
            w2 << "    public void async_get_" << fix_name(m->Name) << "(Action2<" << objtype
               << ",RuntimeException> handler, int timeout) {" << std::endl;
            w2 << "    asyncFindObjRef(\"" << m->Name << "\",handler,timeout);" << std::endl;
            w2 << "    }" << std::endl;
        }
    }
    else
    {

        boost::shared_ptr<ServiceDefinition> d = e->ServiceDefinition_.lock();
        if (!d)
            throw DataTypeException("Invalid object type name");

        std::string objecttype2;

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
            w2 << "    public void async_get_" << fix_name(m->Name) << "(" << indtype << " ind, Action2<" << objtype
               << ",RuntimeException> handler, int timeout) {" << std::endl;
            w2 << "    asyncFindObjRefTyped(\"" << fix_name(m->Name) << "\",String.valueOf(ind),\"" << objecttype2
               << "\",handler,timeout);" << std::endl;
            w2 << "    }" << std::endl;
        }
        else
        {
            w2 << "    public void async_get_" << fix_name(m->Name) << "(Action2<" << objtype
               << ",RuntimeException> handler, int timeout) {" << std::endl;
            w2 << "    asyncFindObjRefTyped(\"" << m->Name << "\",\"" << objecttype2 << "\",handler,timeout);"
               << std::endl;
            w2 << "    }" << std::endl;
        }
    }
    MEMBER_ITER_END()

    w2 << "}" << std::endl;
}

void JavaServiceLangGen::GenerateSkel(ServiceEntryDefinition* e, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "public class " << fix_name(e->Name) << "_skel extends ServiceSkel {" << std::endl;
    w2 << "    protected " << fix_name(e->Name) << " obj;" << std::endl;
    w2 << "    public " << fix_name(e->Name) << "_skel(Object o) { " << std::endl;
    w2 << "    super(o);" << std::endl;
    w2 << "    obj=(" << fix_name(e->Name) << ")o;" << std::endl << "    }" << std::endl;
    w2 << "    public void releaseCastObject() { " << std::endl;
    w2 << "    obj=null;" << std::endl;
    w2 << "    super.releaseCastObject();" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public MessageElement callGetProperty(String membername) {" << std::endl;
    // w2 << "    switch (membername) {" << std::endl;
    MEMBER_ITER2(PropertyDefinition)
    if (m->Direction() != MemberDefinition_Direction_writeonly)
    {
        w2 << "    if(membername.equals( \"" << m->Name << "\"))" << std::endl << "    {" << std::endl;

        convert_type_result t = convert_type(*m->Type);
        w2 << "    " << t.java_type << t.java_arr_type << " ret=obj.get_" << fix_name(m->Name) << "();" << std::endl;
        w2 << "    return " << str_pack_message_element("return", "ret", m->Type) << ";" << std::endl;

        w2 << "    }" << std::endl;
    }
    MEMBER_ITER_END()
    // w2 << "    default:" << std::endl;
    // w2 << "    break;" << std::endl;
    // w2 << "    }" << std::endl;
    w2 << "    throw new MemberNotFoundException(\"Member not found\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public void callSetProperty(String membername, MessageElement m) {" << std::endl;
    // w2 << "    switch (membername) {" << std::endl;
    MEMBER_ITER2(PropertyDefinition)
    if (m->Direction() != MemberDefinition_Direction_readonly)
    {
        w2 << "    if(membername.equals( \"" << m->Name << "\"))" << std::endl << "    {" << std::endl;

        w2 << "    obj.set_" << fix_name(m->Name) << "(" << str_unpack_message_element("m", m->Type) << ");"
           << std::endl;
        w2 << "    return;" << std::endl;
        w2 << "    }" << std::endl;
    }
    MEMBER_ITER_END()
    // w2 << "    default:" << std::endl;
    // w2 << "    break;" << std::endl;
    // w2 << "    }" << std::endl;
    w2 << "    throw new MemberNotFoundException(\"Member not found\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public MessageElement callFunction(String rr_membername, vectorptr_messageelement rr_m) {" << std::endl;
    // w2 << "    switch (rr_membername) {" << std::endl;
    MEMBER_ITER2(FunctionDefinition)
    if (!m->IsGenerator())
    {
        std::string params = str_pack_parameters(m->Parameters, false);
        w2 << "    if(rr_membername.equals( \"" << m->Name << "\"))" << std::endl << "    {" << std::endl;

        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin();
             p != m->Parameters.end(); ++p)
        {
            convert_type_result t3 = convert_type(*(*p));
            w2 << "    " << t3.java_type << t3.java_arr_type << " " << fix_name((*p)->Name) << "="
               << str_unpack_message_element("vectorptr_messageelement_util.findElement(rr_m,\"" + (*p)->Name + "\")",
                                             *p) +
                      ";"
               << std::endl;
            ;
        }
        if (m->ReturnType->Type == DataTypes_void_t)
        {
            w2 << "    this.obj." << fix_name(m->Name) << "(" << params << ");" << std::endl;
            w2 << "    return new MessageElement(\"return\",new int[] {0});" << std::endl;
        }
        else
        {
            convert_type_result t = convert_type(*m->ReturnType);
            w2 << "    " << t.java_type << t.java_arr_type << " rr_ret=obj." << fix_name(m->Name) << "(" << params
               << ");" << std::endl;
            w2 << "    return " << str_pack_message_element("return", "rr_ret", m->ReturnType) << ";" << std::endl;
        }
        w2 << "    }" << std::endl;
    }
    else
    {
        w2 << "    if(rr_membername.equals( \"" << m->Name << "\"))" << std::endl << "    {" << std::endl;
        convert_generator_result t4 = convert_generator(m.get());
        std::string params = str_pack_parameters(t4.params, false);
        for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = t4.params.begin(); p != t4.params.end();
             ++p)
        {
            convert_type_result t3 = convert_type(*(*p));
            w2 << "    " << t3.java_type << t3.java_arr_type << " " << fix_name((*p)->Name) << "="
               << str_unpack_message_element("vectorptr_messageelement_util.findElement(rr_m,\"" + (*p)->Name + "\")",
                                             *p)
               << ";" << std::endl;
            ;
        }
        w2 << "    " << t4.generator_java_type << " rr_ret=this.obj." << fix_name(m->Name) << "(" << params << ");"
           << std::endl;
        w2 << "    int generator_index = innerskel.registerGeneratorServer(\"" << m->Name << "\", new Wrapped"
           << t4.generator_java_base_type << "ServerDirectorJava<" << t4.generator_java_template_params << ">(rr_ret));"
           << std::endl;
        w2 << "    return new MessageElement(\"index\",generator_index);" << std::endl;
        w2 << "    }" << std::endl;
    }
    MEMBER_ITER_END()
    // w2 << "    default:" << std::endl;
    // w2 << "    break;" << std::endl;
    // w2 << "    }" << std::endl;
    w2 << "    throw new MemberNotFoundException(\"Member not found\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public Object getSubObj(String name, String ind) {" << std::endl;
    // w2 << "    switch (name) {" << std::endl;
    MEMBER_ITER2(ObjRefDefinition)
    w2 << "    if(name.equals( \"" << m->Name << "\")) {" << std::endl;
    std::string indtype;
    if (GetObjRefIndType(m, indtype))
    {
        if (indtype == "int")
        {
            w2 << "    return obj.get_" << fix_name(m->Name) << "(Integer.valueOf(ind));" << std::endl;
        }
        else
        {
            w2 << "    return obj.get_" << fix_name(m->Name) << "(ind);" << std::endl;
        }
    }
    else
    {
        w2 << "    return obj.get_" << fix_name(m->Name) << "();" << std::endl;
    }
    w2 << "    }" << std::endl;
    MEMBER_ITER_END()
    //	w2 << "    default:" << std::endl;
    // w2 << "    break;" << std::endl;
    // w2 << "    }" << std::endl;
    w2 << "    throw new MemberNotFoundException(\"\");" << std::endl;
    w2 << "    }" << std::endl;

    MEMBER_ITER2(EventDefinition)
    w2 << "    rr_" << fix_name(m->Name) << " rrvar_" << fix_name(m->Name) << "=new rr_" << fix_name(m->Name) << "();"
       << std::endl;
    MEMBER_ITER_END()

    w2 << "    public void registerEvents(Object obj1) {" << std::endl;
    w2 << "    obj=(" << fix_name(e->Name) << ")obj1;" << std::endl;
    MEMBER_ITER2(EventDefinition)
    w2 << "    obj.add" << fix_name(m->Name) << "Listener(rrvar_" << fix_name(m->Name) << ");" << std::endl;
    MEMBER_ITER_END()
    w2 << "    }" << std::endl;

    w2 << "    public void unregisterEvents(Object obj1) {" << std::endl;
    w2 << "    obj=(" << fix_name(e->Name) << ")obj1;" << std::endl;
    MEMBER_ITER2(EventDefinition)
    w2 << "    obj.remove" << fix_name(m->Name) << "Listener(rrvar_" << fix_name(m->Name) << ");" << std::endl;
    MEMBER_ITER_END()
    w2 << "    }" << std::endl;

    MEMBER_ITER2(EventDefinition)

    std::string params = str_pack_parameters_delegate(m->Parameters, true);
    w2 << "    private class rr_" << fix_name(m->Name) << " implements " << str_pack_delegate(m->Parameters) << "{"
       << std::endl;
    w2 << "    public void action(" << params << ") {" << std::endl;
    w2 << "    vectorptr_messageelement rr_param=new vectorptr_messageelement();" << std::endl;
    w2 << "    try {" << std::endl;
    for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin();
         p != m->Parameters.end(); ++p)
    {
        w2 << "    MessageElementUtil.addMessageElementDispose(rr_param,"
           << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << std::endl;
    }
    w2 << "    " << fix_name(e->Name) << "_skel"
       << ".this.innerskel.wrappedDispatchEvent(\"" << m->Name << "\",rr_param);" << std::endl;

    w2 << "    }" << std::endl;
    w2 << "    finally {" << std::endl;
    w2 << "    rr_param.delete();" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(CallbackDefinition)
    w2 << "    class rr_" << fix_name(m->Name) << " implements " << str_pack_delegate(m->Parameters, m->ReturnType)
       << " {" << std::endl;
    w2 << "    long endpoint;" << std::endl;
    w2 << "    public rr_" << fix_name(m->Name) << "(long endpoint) { this.endpoint=endpoint; }" << std::endl;
    if (m->ReturnType->Type == DataTypes_void_t)
    {
        w2 << "    public void action(" << str_pack_parameters_delegate(m->Parameters, true) << ") {" << std::endl;
        ;
    }
    else
    {
        convert_type_result t2 = convert_type(*m->ReturnType, true);
        w2 << "    public " << t2.java_type << t2.java_arr_type << " func("
           << str_pack_parameters_delegate(m->Parameters, true) << ") {" << std::endl;
        ;
    }
    w2 << "    vectorptr_messageelement rr_param=new vectorptr_messageelement();" << std::endl;
    w2 << "    MessageElement rr_me=null;" << std::endl;
    w2 << "    try {" << std::endl;
    for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator p = m->Parameters.begin();
         p != m->Parameters.end(); ++p)
    {
        w2 << "    MessageElementUtil.addMessageElementDispose(rr_param,"
           << str_pack_message_element((*p)->Name, fix_name((*p)->Name), *p) << ");" << std::endl;
    }
    w2 << "    rr_me=" << fix_name(e->Name) << "_skel"
       << ".this.innerskel.wrappedCallbackCall(\"" << m->Name << "\",this.endpoint,rr_param);" << std::endl;
    if (m->ReturnType->Type != DataTypes_void_t)
    {
        w2 << "    return " << str_unpack_message_element("rr_me", m->ReturnType) << ";" << std::endl;
    }

    w2 << "    }" << std::endl;
    w2 << "    finally {" << std::endl;
    w2 << "    rr_param.delete();" << std::endl;
    w2 << "    if (rr_me!=null) rr_me.delete();" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;

    MEMBER_ITER_END()

    w2 << "    public Object getCallbackFunction(long endpoint, String membername) {" << std::endl;
    // w2 << "    switch (membername) {" << std::endl;
    MEMBER_ITER2(CallbackDefinition)
    convert_type_result t = convert_type(*m->ReturnType);
    std::string params = str_pack_parameters(m->Parameters, true);
    w2 << "    if(membername.equals( \"" << m->Name << "\")) {" << std::endl;
    w2 << "    return new rr_" << fix_name(m->Name) << "(endpoint);" << std::endl;
    w2 << "    }" << std::endl;
    MEMBER_ITER_END()
    //	w2 << "    default:" << std::endl;
    // w2 << "    break;" << std::endl;
    // w2 << "    }" << std::endl;
    w2 << "    throw new MemberNotFoundException(\"Member not found\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public  void initPipeServers(Object obj1) {" << std::endl;
    w2 << "    obj=(" << fix_name(e->Name) << ")obj1;" << std::endl;
    MEMBER_ITER2(PipeDefinition)
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    obj.set_" << fix_name(m->Name) << "(new Pipe<" << t.java_type << t.java_arr_type
       << ">(innerskel.getPipe(\"" << m->Name + "\")));" << std::endl;
    MEMBER_ITER_END()
    w2 << "    }" << std::endl;

    w2 << "    public void initCallbackServers(Object obj1) {" << std::endl;
    w2 << "    obj=(" << fix_name(e->Name) << ")obj1;" << std::endl;
    MEMBER_ITER2(CallbackDefinition)

    w2 << "    obj.set_" << fix_name(m->Name) << "(new CallbackServer<"
       << str_pack_delegate(m->Parameters, m->ReturnType) << ">(\"" << m->Name << "\",this));" << std::endl;
    MEMBER_ITER_END()
    w2 << "    }" << std::endl;

    w2 << "    public void initWireServers(Object obj1) {" << std::endl;
    w2 << "    obj=(" << fix_name(e->Name) << ")obj1;" << std::endl;
    MEMBER_ITER2(WireDefinition)
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    obj.set_" << fix_name(m->Name) << "(new Wire<" << t.java_type << t.java_arr_type
       << ">(innerskel.getWire(\"" << m->Name << "\")));" << std::endl;
    MEMBER_ITER_END()
    w2 << "    }" << std::endl;

    w2 << "    public WrappedArrayMemoryDirector getArrayMemory(String name) {" << std::endl;
    // w2 << "    switch (name) {" << std::endl;
    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type_array(t2);
    if (!IsTypeNumeric(m->Type->Type))
        continue;
    switch (m->Type->ArrayType)
    {
    case DataTypes_ArrayTypes_array:
        w2 << "    if(name.equals( \"" << m->Name << "\")) {" << std::endl;
        w2 << "    WrappedArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type
           << "> dir=new  WrappedArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type << ">(obj.get_"
           << fix_name(m->Name) + "());" << std::endl;
        /*w2 << "    int id=RRObjectHeap.AddObject(dir); " << std::endl;
        w2 << "    dir.memoryid=id;" << std::endl;
        w2 << "    dir.disown();" << std::endl;*/
        w2 << "    return dir;" << std::endl;
        w2 << "    }" << std::endl;
        break;
    case DataTypes_ArrayTypes_multidimarray:
        break;
    default:
        throw DataTypeException("Invalid memory definition");
    }
    MEMBER_ITER_END()
    //	w2 << "    default:" << std::endl;
    // w2 << "    break;" << std::endl;
    // w2 << "    }" << std::endl;
    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public WrappedMultiDimArrayMemoryDirector getMultiDimArrayMemory(String name) {" << std::endl;
    // w2 << "    switch (name) {" << std::endl;
    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type_array(t2);
    if (!IsTypeNumeric(m->Type->Type))
        continue;
    switch (m->Type->ArrayType)
    {
    case DataTypes_ArrayTypes_array:
        break;
    case DataTypes_ArrayTypes_multidimarray:

        w2 << "    if(name.equals( \"" << m->Name << "\")) {" << std::endl;
        w2 << "    WrappedMultiDimArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type
           << "> dir=new  WrappedMultiDimArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type << ">(obj.get_"
           << fix_name(m->Name) << "());" << std::endl;
        /*w2 << "    int id=RRObjectHeap.AddObject(dir); " << std::endl;
        w2 << "    dir.memoryid=id;" << std::endl;
        w2 << "    dir.disown();" << std::endl;*/
        w2 << "    return dir;" << std::endl;
        w2 << "    }" << std::endl;
        break;

    default:
        throw DataTypeException("Invalid memory definition");
    }
    MEMBER_ITER_END()
    //	w2 << "    default:" << std::endl;
    // w2 << "    break;" << std::endl;
    // w2 << "    }" << std::endl;
    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public WrappedPodArrayMemoryDirector getPodArrayMemory(String name) {" << std::endl;

    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type_array(t2);
    if (IsTypeNumeric(m->Type->Type))
        continue;
    DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();
    if (entry_type != DataTypes_pod_t)
        continue;
    if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
    {
        w2 << "    if(name.equals( \"" << m->Name << "\")) {" << std::endl;
        w2 << "    WrappedPodArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type
           << "> dir=new  WrappedPodArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type << ">(obj.get_"
           << fix_name(m->Name) << "(), " << t.java_type << ".class);" << std::endl;
        // w2 << "    int id=RRObjectHeap.AddObject(dir); " << std::endl;
        // w2 << "    dir.memoryid=id;" << std::endl;
        // w2 << "    dir.Disown();" << std::endl;
        w2 << "    return dir;" << std::endl;
        w2 << "    }" << std::endl;
    }
    MEMBER_ITER_END()

    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public WrappedPodMultiDimArrayMemoryDirector getPodMultiDimArrayMemory(String name) {" << std::endl;

    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type_array(t2);
    if (IsTypeNumeric(m->Type->Type))
        continue;
    DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();
    if (entry_type != DataTypes_pod_t)
        continue;
    if (m->Type->ArrayType == DataTypes_ArrayTypes_multidimarray)
    {
        w2 << "    if(name.equals( \"" << m->Name << "\")) {" << std::endl;
        w2 << "    WrappedPodMultiDimArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type
           << "> dir=new  WrappedPodMultiDimArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type << ">(obj.get_"
           << fix_name(m->Name) << "(), " << t.java_type << ".class);" << std::endl;
        // w2 << "    int id=RRObjectHeap.AddObject(dir); " << std::endl;
        // w2 << "    dir.memoryid=id;" << std::endl;
        // w2 << "    dir.Disown();" << std::endl;
        w2 << "    return dir;" << std::endl;
        w2 << "    }" << std::endl;
    }
    MEMBER_ITER_END()
    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << std::endl;
    w2 << "    }" << std::endl;

    // namedarray memories

    w2 << "    public WrappedNamedArrayMemoryDirector getNamedArrayMemory(String name) {" << std::endl;

    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type_array(t2);
    if (IsTypeNumeric(m->Type->Type))
        continue;
    DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();
    if (entry_type != DataTypes_namedarray_t)
        continue;
    if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
    {
        w2 << "    if(name.equals( \"" << m->Name << "\")) {" << std::endl;
        w2 << "    WrappedNamedArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type
           << "> dir=new  WrappedNamedArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type << ">(obj.get_"
           << fix_name(m->Name) << "(), " << t.java_type << ".class);" << std::endl;
        // w2 << "    int id=RRObjectHeap.AddObject(dir); " << std::endl;
        // w2 << "    dir.memoryid=id;" << std::endl;
        // w2 << "    dir.Disown();" << std::endl;
        w2 << "    return dir;" << std::endl;
        w2 << "    }" << std::endl;
    }
    MEMBER_ITER_END()

    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public WrappedNamedMultiDimArrayMemoryDirector getNamedMultiDimArrayMemory(String name) {" << std::endl;

    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type_array(t2);
    if (IsTypeNumeric(m->Type->Type))
        continue;
    DataTypes entry_type = m->Type->ResolveNamedType()->RRDataType();
    if (entry_type != DataTypes_namedarray_t)
        continue;
    if (m->Type->ArrayType == DataTypes_ArrayTypes_multidimarray)
    {
        w2 << "    if(name.equals( \"" << m->Name << "\")) {" << std::endl;
        w2 << "    WrappedNamedMultiDimArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type
           << "> dir=new  WrappedNamedMultiDimArrayMemoryDirectorJava<" << t.java_type << t.java_arr_type
           << ">(obj.get_" << fix_name(m->Name) << "(), " << t.java_type << ".class);" << std::endl;
        // w2 << "    int id=RRObjectHeap.AddObject(dir); " << std::endl;
        // w2 << "    dir.memoryid=id;" << std::endl;
        // w2 << "    dir.Disown();" << std::endl;
        w2 << "    return dir;" << std::endl;
        w2 << "    }" << std::endl;
    }
    MEMBER_ITER_END()
    w2 << "    throw new MemberNotFoundException(\"Member Not Found\");" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "    public String getRRType() { return \"" << e->ServiceDefinition_.lock()->Name << "." << e->Name << "\"; }"
       << std::endl;

    w2 << "}" << std::endl;
}

void JavaServiceLangGen::GenerateDefaultImpl(ServiceEntryDefinition* e, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "public class " << fix_name(e->Name) << "_default_impl implements " << fix_name(e->Name) << "{" << std::endl;

    MEMBER_ITER2(CallbackDefinition)
    w2 << "    protected Callback<" << str_pack_delegate(m->Parameters, m->ReturnType) << "> rrvar_"
       << fix_name(m->Name) << ";" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(PipeDefinition)
    if (m->Direction() == MemberDefinition_Direction_readonly)
    {
        convert_type_result t = convert_type_array(*m->Type);
        w2 << "    protected PipeBroadcaster<" << t.java_type << t.java_arr_type << "> rrvar_" << fix_name(m->Name)
           << ";" << std::endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    if (m->Direction() == MemberDefinition_Direction_readonly)
    {
        convert_type_result t = convert_type_array(*m->Type);
        w2 << "    protected WireBroadcaster<" << t.java_type << t.java_arr_type << "> rrvar_" << fix_name(m->Name)
           << ";" << std::endl;
    }
    if (m->Direction() == MemberDefinition_Direction_writeonly)
    {
        convert_type_result t = convert_type_array(*m->Type);
        w2 << "    protected WireUnicastReceiver<" << t.java_type << t.java_arr_type << "> rrvar_" << fix_name(m->Name)
           << ";" << std::endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(PropertyDefinition)
    convert_type_result t = convert_type(*m->Type);
    t.name = fix_name(m->Name);
    w2 << "    protected " << t.java_type << t.java_arr_type << " rrvar_" << t.name << ";" << std::endl;
    w2 << "    public " << t.java_type << t.java_arr_type << " get_" << t.name << "() { return rrvar_" << t.name
       << "; }" << std::endl;
    w2 << "    public void set_" << t.name << "(" << t.java_type << t.java_arr_type << " value) { rrvar_" << t.name
       << " = value; }" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(FunctionDefinition)
    if (!m->IsGenerator())
    {
        convert_type_result t = convert_type(*m->ReturnType);
        std::string params = str_pack_parameters(m->Parameters, true);
        w2 << "    public " << t.java_type << t.java_arr_type << " " << fix_name(m->Name) << "(" << params << ") {"
           << std::endl;
    }
    else
    {
        convert_generator_result t = convert_generator(m.get());
        std::string params = str_pack_parameters(t.params, true);
        w2 << "    public " << t.generator_java_type << " " << fix_name(m->Name) << "(" << params << ") {" << std::endl;
    }
    w2 << "    throw new UnsupportedOperationException();";
    w2 << "    }" << std::endl;

    MEMBER_ITER_END()

    MEMBER_ITER2(EventDefinition)
    std::string params = str_pack_parameters(m->Parameters, true);
    w2 << "    protected  Vector<" << str_pack_delegate(m->Parameters) << ">"
       << " rrvar_" << fix_name(m->Name) << "=new Vector<" << str_pack_delegate(m->Parameters) << ">();" << std::endl;
    w2 << "    public void "
       << " add" << fix_name(m->Name) << "Listener(" << str_pack_delegate(m->Parameters) << " listener) {" << std::endl;
    w2 << "    synchronized(rrvar_" << fix_name(m->Name) << ") {" << std::endl;
    w2 << "    rrvar_" << fix_name(m->Name) << ".add(listener);" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    public void "
       << " remove" << fix_name(m->Name) << "Listener(" << str_pack_delegate(m->Parameters) << " listener) {"
       << std::endl;
    w2 << "    synchronized(rrvar_" << fix_name(m->Name) << ") {" << std::endl;
    w2 << "    rrvar_" << fix_name(m->Name) << ".remove(listener);" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    }" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(ObjRefDefinition)
    std::string objtype = fix_qualified_name(m->ObjectType);
    if (objtype == "varobject")
        objtype = "Object";
    std::string indtype;
    if (GetObjRefIndType(m, indtype))
    {
        w2 << "    public " << objtype << " get_" << fix_name(m->Name) << "(" << indtype << " ind) {" << std::endl;
        w2 << "    throw new UnsupportedOperationException();" << std::endl;
        w2 << "    }" << std::endl;
    }
    else
    {
        w2 << "    public " << objtype << " get_" << fix_name(m->Name) << "() {" << std::endl;
        w2 << "    throw new UnsupportedOperationException();" << std::endl;
        w2 << "    }" << std::endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(PipeDefinition)
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    public Pipe<" << t.java_type << t.java_arr_type << "> get_" << fix_name(m->Name) << "()" << std::endl;
    if (m->Direction() == MemberDefinition_Direction_readonly)
    {
        w2 << "    { return rrvar_" << fix_name(m->Name) << ".getPipe();  }" << std::endl;
    }
    else
    {
        w2 << "    { throw new UnsupportedOperationException(); }" << std::endl;
    }
    w2 << "    public void set_" << fix_name(m->Name) << "(Pipe<" << t.java_type << t.java_arr_type << "> value)"
       << std::endl;
    if (m->Direction() == MemberDefinition_Direction_readonly)
    {
        w2 << "    {" << std::endl;
        w2 << "    if (rrvar_" << fix_name(m->Name) << "!=null) throw new IllegalStateException(\"Pipe already set\");"
           << std::endl;
        w2 << "    rrvar_" << fix_name(m->Name) << "= new PipeBroadcaster<" << t.java_type << t.java_arr_type
           << ">(value);" << std::endl;
        w2 << "    }" << std::endl;
    }
    else
    {
        w2 << "    { throw new IllegalStateException();}" << std::endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(CallbackDefinition)
    w2 << "    public Callback<" << str_pack_delegate(m->Parameters, m->ReturnType) << "> get_" << fix_name(m->Name)
       << "()" << std::endl;
    w2 << "    { return rrvar_" << fix_name(m->Name) << ";  }" << std::endl;
    w2 << "    public void set_" << fix_name(m->Name) << "(Callback<" << str_pack_delegate(m->Parameters, m->ReturnType)
       << "> value)" << std::endl;
    w2 << "    {" << std::endl;
    w2 << "    if (rrvar_" << fix_name(m->Name) << "!=null) throw new IllegalStateException(\"Callback already set\");"
       << std::endl;
    w2 << "    rrvar_" << fix_name(m->Name) << "= value;" << std::endl;
    w2 << "    }" << std::endl;
    MEMBER_ITER_END()

    MEMBER_ITER2(WireDefinition)
    convert_type_result t = convert_type_array(*m->Type);
    w2 << "    public Wire<" << t.java_type << t.java_arr_type << "> get_" << fix_name(m->Name) << "()" << std::endl;
    if (m->Direction() == MemberDefinition_Direction_readonly || m->Direction() == MemberDefinition_Direction_writeonly)
    {
        w2 << "    { return rrvar_" << fix_name(m->Name) << ".getWire();  }" << std::endl;
    }
    else
    {
        w2 << "    { throw new UnsupportedOperationException(); }" << std::endl;
    }
    w2 << "    public void set_" << fix_name(m->Name) << "(Wire<" << t.java_type << t.java_arr_type << "> value)"
       << std::endl;
    if (m->Direction() == MemberDefinition_Direction_readonly)
    {
        w2 << "    {" << std::endl;
        w2 << "    if (rrvar_" << fix_name(m->Name) << "!=null) throw new IllegalStateException(\"Pipe already set\");"
           << std::endl;
        w2 << "    rrvar_" << fix_name(m->Name) << "= new WireBroadcaster<" << t.java_type << t.java_arr_type
           << ">(value);" << std::endl;
        w2 << "    }" << std::endl;
    }
    else if (m->Direction() == MemberDefinition_Direction_writeonly)
    {
        w2 << "    {" << std::endl;
        w2 << "    if (rrvar_" << fix_name(m->Name) << "!=null) throw new IllegalStateException(\"Pipe already set\");"
           << std::endl;
        w2 << "    rrvar_" << fix_name(m->Name) << "= new WireUnicastReceiver<" << t.java_type << t.java_arr_type
           << ">(value);" << std::endl;
        w2 << "    }" << std::endl;
    }
    else
    {
        w2 << "    { throw new UnsupportedOperationException();}" << std::endl;
    }
    MEMBER_ITER_END()

    MEMBER_ITER2(MemoryDefinition)
    TypeDefinition t2;
    m->Type->CopyTo(t2);
    t2.RemoveArray();
    convert_type_result t = convert_type_array(t2);
    std::string c;
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
        w2 << "    public " << c << "ArrayMemory<" << t.java_type << t.java_arr_type << "> get_" << fix_name(m->Name)
           << "()" << std::endl;
        break;
    case DataTypes_ArrayTypes_multidimarray:
        w2 << "    public " << c << "MultiDimArrayMemory<" << t.java_type << t.java_arr_type << "> get_"
           << fix_name(m->Name) << "()" << std::endl;
        break;
    default:
        throw DataTypeException("Invalid memory definition");
    }
    w2 << "    { throw new UnsupportedOperationException(); }" << std::endl;
    MEMBER_ITER_END()

    w2 << "}" << std::endl;
}

void JavaServiceLangGen::GenerateServiceFactoryFile(ServiceDefinition* d, const std::string& defstring, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateServiceFactory(d, defstring, w);
}

void JavaServiceLangGen::GenerateStructureFile(ServiceEntryDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->ServiceDefinition_.lock()->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateStructure(d, w);
}

void JavaServiceLangGen::GenerateStructureStubFile(ServiceEntryDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->ServiceDefinition_.lock()->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateStructureStub(d, w);
}

void JavaServiceLangGen::GeneratePodFile(ServiceEntryDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->ServiceDefinition_.lock()->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GeneratePod(d, w);
}

void JavaServiceLangGen::GenerateNamedArrayFile(const RR_SHARED_PTR<ServiceEntryDefinition>& d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->ServiceDefinition_.lock()->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateNamedArray(d, w);
}

void JavaServiceLangGen::GeneratePodStubFile(ServiceEntryDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->ServiceDefinition_.lock()->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GeneratePodStub(d, w);
}

void JavaServiceLangGen::GenerateNamedArrayStubFile(const RR_SHARED_PTR<ServiceEntryDefinition>& d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->ServiceDefinition_.lock()->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateNamedArrayStub(d, w);
}

void JavaServiceLangGen::GenerateInterfaceFile(ServiceEntryDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->ServiceDefinition_.lock()->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateInterface(d, w);
}

void JavaServiceLangGen::GenerateAsyncInterfaceFile(ServiceEntryDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->ServiceDefinition_.lock()->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateAsyncInterface(d, w);
}

void JavaServiceLangGen::GenerateStubFile(ServiceEntryDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->ServiceDefinition_.lock()->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateStub(d, w);
}

template <typename T>
static void null_deleter(T*)
{}

static std::string JavaServiceLangGen_EscapeString_Formatter(const boost::smatch& match)
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

std::string JavaServiceLangGen::convert_constant(ConstantDefinition* c,
                                                 std::vector<RR_SHARED_PTR<ConstantDefinition> >& c2,
                                                 ServiceDefinition* def)
{
    RR_SHARED_PTR<ServiceDefinition> def2(def, null_deleter<ServiceDefinition>);

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
                             JavaServiceLangGen_EscapeString_Formatter, boost::match_default | boost::format_all);

        return "public static final String " + fix_name(c->Name) + "=\"" + t.str() + "\";";
    }

    if (t->ArrayType == DataTypes_ArrayTypes_none)
    {
        return "public static final " + c1.java_type + " " + fix_name(c->Name) + "=" + c->Value + ";";
    }
    else
    {
        return "public static final " + c1.java_type + "[] " + fix_name(c->Name) + "=" + c->Value + ";";
    }
}

JavaServiceLangGen::convert_generator_result JavaServiceLangGen::convert_generator(FunctionDefinition* f)
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
        convert_type_result t = convert_type_array(*r_type);
        RR_SHARED_PTR<TypeDefinition> p_type = f->Parameters.back()->Clone();
        p_type->RemoveContainers();
        convert_type_result t2 = convert_type_array(*p_type);
        std::copy(f->Parameters.begin(), --f->Parameters.end(), std::back_inserter(o.params));
        o.generator_java_base_type = "Generator1";
        o.generator_java_template_params = t.java_type + t.java_arr_type + "," + t2.java_type + t2.java_arr_type;
        o.generator_java_type = o.generator_java_base_type + "<" + o.generator_java_template_params + ">";
        return o;
    }

    if (param_generator)
    {
        RR_SHARED_PTR<TypeDefinition> p_type = f->Parameters.back()->Clone();
        p_type->RemoveContainers();
        convert_type_result t2 = convert_type_array(*p_type);
        std::copy(f->Parameters.begin(), --f->Parameters.end(), std::back_inserter(o.params));
        o.generator_java_base_type = "Generator3";
        o.generator_java_template_params = t2.java_type + t2.java_arr_type;
        o.generator_java_type = o.generator_java_base_type + "<" + o.generator_java_template_params + ">";
        return o;
    }
    else
    {
        RR_SHARED_PTR<TypeDefinition> r_type = f->ReturnType->Clone();
        r_type->RemoveContainers();
        convert_type_result t = convert_type_array(*r_type);
        boost::range::copy(f->Parameters, std::back_inserter(o.params));
        o.generator_java_base_type = "Generator2";
        o.generator_java_template_params = t.java_type + t.java_arr_type;
        o.generator_java_type = o.generator_java_base_type + "<" + o.generator_java_template_params + ">";
        return o;
    }
}

void JavaServiceLangGen::GenerateConstants(ServiceDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    bool hasconstants = false;

    for (std::vector<std::string>::iterator e = d->Options.begin(); e != d->Options.end(); ++e)
    {
        if (boost::starts_with(*e, "constant"))
            hasconstants = true;
    }

    if (!d->Enums.empty() || !d->Constants.empty())
        hasconstants = true;

    std::vector<boost::shared_ptr<ServiceEntryDefinition> > entries;
    boost::copy(d->NamedArrays, std::back_inserter(entries));
    boost::copy(d->Pods, std::back_inserter(entries));
    boost::copy(d->Structures, std::back_inserter(entries));
    boost::copy(d->Objects, std::back_inserter(entries));

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

    w2 << "class " << boost::replace_all_copy(fix_name(d->Name), ".", "__") << "Constants " << std::endl
       << "{" << std::endl;

    for (std::vector<std::string>::iterator e = d->Options.begin(); e != d->Options.end(); ++e)
    {
        if (boost::starts_with(*e, "constant"))
        {
            RR_SHARED_PTR<ServiceDefinition> def2(d, null_deleter<ServiceDefinition>);
            RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(def2);
            c->FromString(*e);
            w2 << "    " << convert_constant(c.get(), d->Constants, d) << std::endl;
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

        if (!(*ee)->Constants.empty())
            objhasconstants = true;

        if (objhasconstants || !(*ee)->Constants.empty())
        {
            w2 << "    public static class " << fix_name((*ee)->Name) << std::endl << "    {" << std::endl;
            for (std::vector<std::string>::iterator e = (*ee)->Options.begin(); e != (*ee)->Options.end(); ++e)
            {
                if (boost::starts_with(*e, "constant"))
                {
                    RR_SHARED_PTR<ServiceDefinition> def2(d, null_deleter<ServiceDefinition>);
                    RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(def2);
                    c->FromString(*e);
                    w2 << "    " << convert_constant(c.get(), (*ee)->Constants, d) << std::endl;
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

    BOOST_FOREACH (RR_SHARED_PTR<EnumDefinition>& e, d->Enums)
    {}

    w2 << "}" << std::endl;
}

void JavaServiceLangGen::GenerateExceptionFile(ExceptionDefinition* exp, ServiceDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateDocString(exp->DocString, "    ", w);

    w2 << "public class " << fix_name(exp->Name) << " extends RobotRaconteurRemoteException" << std::endl
       << "{" << std::endl;
    w2 << "    public " << fix_name(exp->Name) << "(String message)  {" << std::endl;
    w2 << "    super(\"" << d->Name << "." << exp->Name << "\",message);" << std::endl;
    w2 << "    }" << std::endl << std::endl;
    w2 << "    public " << fix_name(exp->Name) << "(String message, String subname, Object param_)  {" << std::endl;
    w2 << "    super(\"" << d->Name << "." << exp->Name << "\",message, subname, param_);" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "};" << std::endl;
}

void JavaServiceLangGen::GenerateEnumFile(EnumDefinition* e, ServiceDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->Name) << ";" << std::endl;
    // w2 << "import java.util.*;" << std::endl;
    // w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateDocString(e->DocString, "", w);
    w2 << "public enum " << fix_name(e->Name) << std::endl;
    w2 << "{" << std::endl;
    for (size_t i = 0; i < e->Values.size(); i++)
    {
        EnumDefinitionValue& v = e->Values[i];
        GenerateDocString(v.DocString, "    ", w);

        w2 << "    " << fix_name(v.Name) << "(" << v.Value << ")";

        if (i < e->Values.size() - 1)
        {
            w2 << "," << std::endl;
        }
        else
        {
            w2 << ";" << std::endl;
        }
    }

    w2 << "    private int value;" << std::endl;
    w2 << "    private " << fix_name(e->Name) << "(int value)" << std::endl;
    w2 << "    {" << std::endl;
    w2 << "    this.value = value;" << std::endl;
    w2 << "    }" << std::endl;
    w2 << "    public int getValue() { return value; }" << std::endl;

    w2 << "    public static " << fix_name(e->Name) << " intToEnum(int value) {" << std::endl;
    w2 << "	    " << fix_name(e->Name) << "[] values = " << fix_name(e->Name) << ".class.getEnumConstants();"
       << std::endl;
    w2 << "	    if (value < values.length && value >= 0 && values[value].value == value)" << std::endl;
    w2 << "		    return values[value];" << std::endl;
    w2 << "	    for (" << fix_name(e->Name) << " enum_ : values)" << std::endl;
    w2 << "	    	if (enum_.value == value)" << std::endl;
    w2 << "	    		return enum_;" << std::endl;
    w2 << "	    throw new IllegalArgumentException(\"No enum \" + " << fix_name(e->Name)
       << ".class + \" with value \" + value);" << std::endl;
    w2 << "    }" << std::endl;

    w2 << "};" << std::endl;
}

void JavaServiceLangGen::GenerateConstantsFile(ServiceDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateConstants(d, w);
}

void JavaServiceLangGen::GenerateSkelFile(ServiceEntryDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->ServiceDefinition_.lock()->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateSkel(d, w);
}

void JavaServiceLangGen::GenerateDefaultImplFile(ServiceEntryDefinition* d, std::ostream* w)
{
    std::ostream& w2 = *w;

    w2 << "//This file is automatically generated. DO NOT EDIT!" << std::endl;
    w2 << "package " << fix_name(d->ServiceDefinition_.lock()->Name) << ";" << std::endl;
    w2 << "import java.util.*;" << std::endl;
    w2 << "import com.robotraconteur.*;" << std::endl;
    // w2 << "using System.Collections.Generic;" << std::endl << std::endl;

    GenerateDefaultImpl(d, w);
}

void JavaServiceLangGen::GenerateFiles(const RR_SHARED_PTR<ServiceDefinition>& d, const std::string& servicedef,
                                       const std::string& path)
{
#ifdef _WIN32
    const std::string os_pathsep("\\");
#else
    const std::string os_pathsep("/");
#endif

    std::string dname1 = fix_name(d->Name);
    std::vector<std::string> dname2;
    boost::split(dname2, dname1, boost::is_from_range('.', '.'));

    boost::filesystem::path p = boost::filesystem::path(path);

    for (std::vector<std::string>::iterator e = dname2.begin(); e != dname2.end(); e++)
    {
        p = p /= boost::filesystem::path(*e);
        if (!boost::filesystem::is_directory(p))
            boost::filesystem::create_directory(p);
    }

    std::ofstream f1(
        (p.string() + os_pathsep + boost::replace_all_copy(fix_name(d->Name), ".", "__") + "Factory.java").c_str());
    GenerateServiceFactoryFile(d.get(), servicedef, &f1);
    f1.close();

    BOOST_FOREACH (const RR_SHARED_PTR<ExceptionDefinition>& e, d->Exceptions)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((e->Name)) + ".java").c_str());
        GenerateExceptionFile(e.get(), d.get(), &f2);
        f2.close();
    }

    for (std::vector<RR_SHARED_PTR<EnumDefinition> >::iterator e = d->Enums.begin(); e != d->Enums.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((*e)->Name) + ".java").c_str());
        GenerateEnumFile(e->get(), d.get(), &f2);
        f2.close();
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((*e)->Name) + ".java").c_str());
        GenerateStructureFile(e->get(), &f2);
        f2.close();
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e = d->Structures.begin();
         e != d->Structures.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((*e)->Name) + "_stub.java").c_str());
        GenerateStructureStubFile(e->get(), &f2);
        f2.close();
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e = d->Pods.begin(); e != d->Pods.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((*e)->Name) + ".java").c_str());
        GeneratePodFile(e->get(), &f2);
        f2.close();
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e = d->Pods.begin(); e != d->Pods.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((*e)->Name) + "_stub.java").c_str());
        GeneratePodStubFile(e->get(), &f2);
        f2.close();
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((*e)->Name) + ".java").c_str());
        GenerateNamedArrayFile(*e, &f2);
        f2.close();
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e = d->NamedArrays.begin();
         e != d->NamedArrays.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((*e)->Name) + "_stub.java").c_str());
        GenerateNamedArrayStubFile(*e, &f2);
        f2.close();
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((*e)->Name) + ".java").c_str());
        GenerateInterfaceFile(e->get(), &f2);
        f2.close();
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + "async_" + fix_name((*e)->Name) + ".java").c_str());
        GenerateAsyncInterfaceFile(e->get(), &f2);
        f2.close();
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((*e)->Name) + "_stub.java").c_str());
        GenerateStubFile(e->get(), &f2);
        f2.close();
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((*e)->Name) + "_skel.java").c_str());
        GenerateSkelFile(e->get(), &f2);
        f2.close();
    }

    for (std::vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator e = d->Objects.begin();
         e != d->Objects.end(); ++e)
    {
        std::ofstream f2((p.string() + os_pathsep + fix_name((*e)->Name) + "_default_impl.java").c_str());
        GenerateDefaultImplFile(e->get(), &f2);
        f2.close();
    }

    std::ofstream f3(
        (p.string() + os_pathsep + boost::replace_all_copy(fix_name(d->Name), ".", "__") + "Constants.java").c_str());
    GenerateConstantsFile(d.get(), &f3);
    f3.close();
}

std::string JavaServiceLangGen::GetDefaultValue(const TypeDefinition& tdef)
{
    if (tdef.Type == DataTypes_void_t)
        throw InternalErrorException("Internal error");
    if (tdef.ArrayType == DataTypes_ArrayTypes_none && tdef.ContainerType == DataTypes_ContainerTypes_none)
    {

        if (tdef.Type == DataTypes_double_t)
            return "0.0";
        if (tdef.Type == DataTypes_single_t)
            return "(float)0.0";
        if (tdef.Type == DataTypes_int8_t)
            return "(byte)0";
        if (tdef.Type == DataTypes_uint8_t)
            return "new UnsignedByte((byte)0)";
        if (tdef.Type == DataTypes_int16_t)
            return "(short)0";
        if (tdef.Type == DataTypes_uint16_t)
            return "new UnsignedShort((short)0)";
        if (tdef.Type == DataTypes_int32_t)
            return "(int)0";
        if (tdef.Type == DataTypes_uint32_t)
            return "new UnsignedInt((int)0)";
        if (tdef.Type == DataTypes_int64_t)
            return "(long)0";
        if (tdef.Type == DataTypes_uint64_t)
            return "new UnsignedLong((long)0)";
        if (tdef.Type == DataTypes_cdouble_t)
            return "new CDouble(0.0,0.0)";
        if (tdef.Type == DataTypes_csingle_t)
            return "new CSingle((float)0.0,(float)0.0)";
        if (tdef.Type == DataTypes_bool_t)
            return "false";

        if (tdef.Type == DataTypes_string_t)
            return "\"\"";
    }
    return "null";
}

void JavaServiceLangGen::GenerateDocString(const std::string& docstring, const std::string& prefix, std::ostream* w)
{
    if (docstring.empty())
    {
        return;
    }

    std::ostream& w2 = *w;

    std::vector<std::string> docstring_v;
    boost::split(docstring_v, docstring, boost::is_any_of("\n"));
    w2 << prefix << "/**" << std::endl;
    BOOST_FOREACH (const std::string& s, docstring_v)
    {
        w2 << prefix << " * " << s << std::endl;
    }
    w2 << prefix << " */" << std::endl;
}
}
