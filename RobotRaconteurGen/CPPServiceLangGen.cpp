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

using namespace std;
using namespace RobotRaconteur;


#define MEMBER_ITER(TYPE) {for (std::vector<RR_SHARED_PTR<MemberDefinition> >::const_iterator m1=(*e)->Members.begin(); m1!=(*e)->Members.end(); ++m1) if (dynamic_cast<TYPE*>(m1->get())!=0) { RR_SHARED_PTR<TYPE> m = dynamic_pointer_cast<TYPE>(*m1);   
#define MEMBER_ITER_END() }}

#define OBJREF_ARRAY_CONTAINER_CMD(d, scalar_cmd, array_cmd, map_int32_cmd, map_string_cmd) \
switch (d->ArrayType) \
{ \
	case DataTypes_ArrayTypes_none: \
	{ \
		switch (d->ContainerType) \
		{ \
		case DataTypes_ContainerTypes_none: \
			{scalar_cmd;} \
			break; \
		case DataTypes_ContainerTypes_map_int32: \
			{ map_int32_cmd; } \
			break; \
		case DataTypes_ContainerTypes_map_string: \
			{map_string_cmd;} \
			break; \
		default: \
			throw DataTypeException("Unknown object container type"); \
		} \
		break; \
	} \
	case DataTypes_ArrayTypes_array: \
	{ \
		if (d->ContainerType != DataTypes_ContainerTypes_none) \
		{ \
			throw DataTypeException("Invalid object container type"); \
		} \
		{array_cmd;} \
		break; \
	} \
	default: \
		throw DataTypeException("Invalid object array type"); \
} \

namespace RobotRaconteurGen
{

	std::string CPPServiceLangGen::fix_name(const std::string& name)
	{
		if (name.find('.') != std::string::npos)
		{
			vector<string> s1;
			vector<string> s2;
			boost::split(s1,name,boost::is_from_range('.','.'));
			for (vector<string>::iterator e=s1.begin(); e!=s1.end(); e++)
			{
				s2.push_back(fix_name(*e));
			}

			return join(s2,".");
		}

		const char* res_str[]={"alignas",	"alignof","and","and_eq","asm","auto",
			"bitand","bitor","bool","break","case","catch",
			"char","char16_t","char32_t","class","compl","const",
			"constnexpr","const_cast","continue","decltype","default","delete",
			"do","double","dynamic_cast","else","enum","explicit",
			"export","extern","false","float","for","friend",
			"goto","if","inline","int","long","mutable",
			"namespace","new","noexcept","not","noteq","nullptr",
			"operator","or","or_eq","private","protected","public",
			"register","reinterpret_cast","return","short","signed","sizeof",
			"static","static_assert","static_cast","struct","switch","switch",
			"template","this","thread_local","throw","true","try",
			"typedef","typeid","typename","union","unsigned","using",
			"virtual","void","volatile","wchar_t","while","xor",
			"xor_eq","override","final","uint8_t","int8_t","int16_t",
			"uint16_t","int32_t","uint32_t","int64_t","uint64_t"};

		std::vector<std::string> reserved(res_str,res_str+sizeof(res_str)/(sizeof(res_str[0])));

		if (std::find(reserved.begin(),reserved.end(),name)!=reserved.end())
		{
			std::cout << "warning: name " << name << " replaced by " << name <<"_ due to keyword conflict" << std::endl;
			return name + "_";
		}

		return name;

	}

	std::string CPPServiceLangGen::fix_qualified_name(const std::string& name)
	{
		std::vector<std::string> v;
		boost::split(v,name,boost::is_from_range('.','.'));

		if (v.size()==0) throw InternalErrorException("Internal error");

		std::vector<std::string> vret;

		for (vector<string>::iterator e=v.begin(); e!=v.end(); e++)
		{
			vret.push_back(fix_name(*e));
		}

		return boost::join(vret,"::");


	}

	CPPServiceLangGen::convert_type_result CPPServiceLangGen::convert_type(const TypeDefinition& tdef)
	{
		convert_type_result o;
		DataTypes t=tdef.Type;
		o.name=fix_name(tdef.Name);
		//o[2]=tdef.IsArray ? "*" : "";

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

	std::string CPPServiceLangGen::remove_RR_INTRUSIVE_PTR(const std::string &vartype1)
	{
		string vartype = boost::trim_copy(vartype1);
		if (!boost::starts_with(vartype,"RR_INTRUSIVE_PTR"))
			return vartype;

		std::string b = "RR_INTRUSIVE_PTR<";
		int32_t start = (int32_t)b.length();

		std::string ret = vartype.substr(start);
		ret = ret.substr(0, ret.length() - 1);
		return ret;
	}

	CPPServiceLangGen::get_variable_type_result CPPServiceLangGen::get_variable_type(const TypeDefinition &tdef, bool usescalar)
	{

		if (tdef.Type == DataTypes_void_t)
		{		
			get_variable_type_result o;
			o.name=fix_name(tdef.Name);
			o.cpp_type="void";
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
			case DataTypes_ContainerTypes_list:
			{
				get_variable_type_result o;
				o.name = fix_name(tdef.Name);
				o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRList<" + remove_RR_INTRUSIVE_PTR(s2.cpp_type) + " > >";
				o.cpp_param_type = o.cpp_type;
				return o;
			}
			case DataTypes_ContainerTypes_map_int32:
			{
				get_variable_type_result o;
				o.name = fix_name(tdef.Name);
				o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<int32_t," + remove_RR_INTRUSIVE_PTR(s2.cpp_type) + " > >";
				o.cpp_param_type = o.cpp_type;
				return o;
			}
			case DataTypes_ContainerTypes_map_string:
			{
				get_variable_type_result o;
				o.name = fix_name(tdef.Name);
				o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRMap<std::string," + remove_RR_INTRUSIVE_PTR(s2.cpp_type) + " > >";
				o.cpp_param_type = o.cpp_type;
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
			case DataTypes_ArrayTypes_none:
			{
				if (usescalar)
				{
					get_variable_type_result o;
					o.name = c.name;
					o.cpp_type = c.cpp_type;
					o.cpp_param_type = o.cpp_type;
					return o;
				}
			}
			case DataTypes_ArrayTypes_array:
			{
				get_variable_type_result o;
				o.name = c.name;
				o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<" + c.cpp_type + " > >";
				o.cpp_param_type = o.cpp_type;
				return o;
			}
			case DataTypes_ArrayTypes_multidimarray:
			{
				get_variable_type_result o;
				o.name = c.name;
				o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<" + c.cpp_type + " > >";
				o.cpp_param_type = o.cpp_type;
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
				o.cpp_param_type = o.cpp_type;
				return o;
			}
		}
		else if (tdef.Type == DataTypes_namedtype_t)
		{
			get_variable_type_result o;
			RR_SHARED_PTR<NamedTypeDefinition> nt = tdef.ResolveNamedType_cache.lock();
			if (!nt) throw DataTypeException("Data type not resolved");
			switch (nt->RRDataType())
			{
			case DataTypes_structure_t:				
				o.name = fix_name(tdef.Name);
				o.cpp_type = "RR_INTRUSIVE_PTR<" + fix_qualified_name(tdef.TypeString) + " >";
				o.cpp_param_type = o.cpp_type;
				break;
			case DataTypes_pod_t:
			case DataTypes_namedarray_t:
			{

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
						o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RR" + a +  "Array<" + fix_qualified_name(tdef.TypeString) + "> >";
						o.cpp_param_type = o.cpp_type;
					}
					break;
				case DataTypes_ArrayTypes_array:
					o.name = fix_name(tdef.Name);
					o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RR" + a + "Array<" + fix_qualified_name(tdef.TypeString) + "> >";
					o.cpp_param_type = o.cpp_type;
					break;
				case DataTypes_ArrayTypes_multidimarray:
					o.name = fix_name(tdef.Name);
					o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RR" + a + "MultiDimArray<" + fix_qualified_name(tdef.TypeString) + "> >";
					o.cpp_param_type = o.cpp_type;
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
				}
				else
				{
					std::string enum_type_name;
					boost::tie(boost::tuples::ignore, enum_type_name) = SplitQualifiedName(tdef.TypeString);
					o.cpp_type = fix_qualified_name(tdef.TypeString) + "::" + fix_name(enum_type_name);
				}
				o.cpp_param_type = o.cpp_type;
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
			o.cpp_param_type = o.cpp_type;
			return o;
		}
		else if (tdef.Type == DataTypes_varvalue_t)
		{
			get_variable_type_result o;
			o.name = fix_name(tdef.Name);
			o.cpp_type = "RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>";
			o.cpp_param_type = o.cpp_type;
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

		return boost::join(o,", ");

	}


	//Code to pack and unpack message elements

	static std::string CPPServiceLangGen_VerifyArrayLength(TypeDefinition& t, std::string varname)
	{		
		if (t.ArrayType == DataTypes_ArrayTypes_array && t.ArrayLength.at(0) != 0)
		{
			return "RobotRaconteur::VerifyRRArrayLength(" + varname + ", " + boost::lexical_cast<std::string>(t.ArrayLength.at(0)) + ", " + (t.ArrayVarLength ? "true" : "false") + ")";
		}
		if (t.ArrayType == DataTypes_ArrayTypes_multidimarray && t.ArrayLength.size() != 0 && !t.ArrayVarLength)
		{
			int32_t n_elems = boost::accumulate(t.ArrayLength, 1, std::multiplies<int32_t>());
			return "RobotRaconteur::VerifyRRMultiDimArrayLength<" + boost::lexical_cast<std::string>(t.ArrayLength.size()) +  ">(" + varname
				+ "," + boost::lexical_cast<std::string>(n_elems) + ",boost::assign::list_of(" + boost::join(t.ArrayLength | boost::adaptors::transformed(boost::lexical_cast<std::string, int32_t>), ")(")
				+ "))";
		}
		return varname;		
	}
	
	static bool CPPServiceLangGen_UseVerifyArrayLength(TypeDefinition& t)
	{
		if (t.ArrayType == DataTypes_ArrayTypes_array && t.ArrayLength.at(0) != 0)
		{
			return true;
		}
		if (t.ArrayType == DataTypes_ArrayTypes_multidimarray && t.ArrayLength.size() != 0 && !t.ArrayVarLength)
		{
			return true;
		}
		return false;
	}

	std::string CPPServiceLangGen::str_pack_message_element(const std::string &elementname, const std::string &varname, const RR_SHARED_PTR<TypeDefinition> &t, const std::string &packer)
	{		
		TypeDefinition t1;
		t->CopyTo(t1);
		t1.RemoveContainers();
		get_variable_type_result tt = get_variable_type(t1, false);

		switch (t->ContainerType)
		{
		case DataTypes_ContainerTypes_none:
		{
			if (IsTypeNumeric(t->Type))
			{
				switch (t->ArrayType)
				{
				case DataTypes_ArrayTypes_none:
				{
					convert_type_result ts = convert_type(*t);
					return "RobotRaconteur::MessageElement_PackScalarElement<" + ts.cpp_type + " >(\"" + elementname + "\"," + varname + ")";
					break;
				}
				case DataTypes_ArrayTypes_array:
				{
					convert_type_result ts = convert_type(*t);					
					return "RobotRaconteur::MessageElement_PackArrayElement<" + ts.cpp_type + " >(\"" + elementname + "\"," + CPPServiceLangGen_VerifyArrayLength(*t,varname) + ")";
					
					break;
				}
				case DataTypes_ArrayTypes_multidimarray:
				{
					convert_type_result ts = convert_type(*t);
					return "RobotRaconteur::MessageElement_PackMultiDimArrayElement<" + ts.cpp_type + " >(RRGetNodeWeak(),\"" + elementname + "\"," + CPPServiceLangGen_VerifyArrayLength(*t,varname) + ")";
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
				return "RobotRaconteur::MessageElement_PackVarTypeElement(RRGetNodeWeak(),\"" + elementname + "\"," + varname + ")";
			}
			else if (t->Type == DataTypes_namedtype_t)
			{
				RR_SHARED_PTR<NamedTypeDefinition> nt = t->ResolveNamedType();
				switch (nt->RRDataType())
				{
				case DataTypes_structure_t:
					return "RobotRaconteur::MessageElement_PackStructElement(RRGetNodeWeak(),\"" + elementname + "\"," + varname + ")";
					break;
				case DataTypes_enum_t:
					return "RobotRaconteur::MessageElement_PackEnumElement(\"" + elementname + "\"," + varname + ")";
					break;
				case DataTypes_pod_t:
				case DataTypes_namedarray_t:
				{
					std::string a = nt->RRDataType() == DataTypes_pod_t ? "Pod" : "Named";
					switch (t->ArrayType)
					{
					case DataTypes_ArrayTypes_none:
						if (a == "Named")
						{
							return "RobotRaconteur::MessageElement_Pack" + a + "ArrayToArrayElement(\"" + elementname + "\"," + varname + ")";
						}
						else
						{
							return "RobotRaconteur::MessageElement_Pack" + a + "ToArrayElement(\"" + elementname + "\"," + varname + ")";
						}
						break;
					case DataTypes_ArrayTypes_array:
						return "RobotRaconteur::MessageElement_Pack" + a + "ArrayElement(\"" + elementname + "\"," + CPPServiceLangGen_VerifyArrayLength(*t, varname) + ")";
						break;
					case DataTypes_ArrayTypes_multidimarray:
						return "RobotRaconteur::MessageElement_Pack" + a + "MultiDimArrayElement(\"" + elementname + "\"," + CPPServiceLangGen_VerifyArrayLength(*t, varname) + ")";
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
			return "RobotRaconteur::MessageElement_PackListElement<" + remove_RR_INTRUSIVE_PTR(tt.cpp_type) + " >(RRGetNodeWeak(),\"" + elementname + "\"," + CPPServiceLangGen_VerifyArrayLength(*t,varname) + ")";
		case DataTypes_ContainerTypes_map_int32:
			return "RobotRaconteur::MessageElement_PackMapElement<int32_t," + remove_RR_INTRUSIVE_PTR(tt.cpp_type) + " >(RRGetNodeWeak(),\"" + elementname + "\"," + CPPServiceLangGen_VerifyArrayLength(*t,varname) + ")";
		case DataTypes_ContainerTypes_map_string:
			return "RobotRaconteur::MessageElement_PackMapElement<std::string," + remove_RR_INTRUSIVE_PTR(tt.cpp_type) + " >(RRGetNodeWeak(),\"" + elementname + "\"," + CPPServiceLangGen_VerifyArrayLength(*t,varname) + ")";
		default:
			throw DataTypeException("Invalid container type");
		}

	}

	std::string CPPServiceLangGen::str_unpack_message_element(const std::string &varname, const RR_SHARED_PTR<TypeDefinition> &t, const std::string &packer)
	{
		TypeDefinition t1;
		t->CopyTo(t1);
		t1.RemoveContainers();
		
		convert_type_result tt = convert_type(t1);		
		std::string structunpackstring = "";

		get_variable_type_result tt1 = get_variable_type(t1,false);

		if (IsTypeNumeric(t->Type))
		{
			switch (t->ArrayType)
			{
			case DataTypes_ArrayTypes_none:
			{
				structunpackstring = "RobotRaconteur::MessageElement_UnpackScalar<" + tt.cpp_type + " >(" + varname + ")";
				break;
			}
			case DataTypes_ArrayTypes_array:
			{				
				structunpackstring = CPPServiceLangGen_VerifyArrayLength(*t,"RobotRaconteur::MessageElement_UnpackArray<" + tt.cpp_type + " >(" + varname + ")");
				break;
			}
			case DataTypes_ArrayTypes_multidimarray:
			{
				structunpackstring = CPPServiceLangGen_VerifyArrayLength(*t,"RobotRaconteur::MessageElement_UnpackMultiDimArray<" + tt.cpp_type + " >(RRGetNodeWeak()," + varname + ")");
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
				structunpackstring = "RobotRaconteur::MessageElement_UnpackStructure<" + fix_qualified_name(tt.cpp_type) + " >(RRGetNodeWeak(), " + varname + ")";
				break;
			case DataTypes_enum_t:
				structunpackstring = "RobotRaconteur::MessageElement_UnpackEnum<" + fix_qualified_name(tt1.cpp_type) + ">(" + varname + ")";
				break;
			case DataTypes_pod_t:
			case DataTypes_namedarray_t:
			{
				std::string a = nt->RRDataType() == DataTypes_pod_t ? "Pod" : "Named";
				switch (t->ArrayType)
				{
				case DataTypes_ArrayTypes_none:
					if (a == "Named")
					{
						structunpackstring = "RobotRaconteur::MessageElement_Unpack" + a + "ArrayFromArray<" + fix_qualified_name(tt.cpp_type) + ">(" + varname + ")";
					}
					else
					{
						structunpackstring = "RobotRaconteur::MessageElement_Unpack" + a + "FromArray<" + fix_qualified_name(tt.cpp_type) + ">(" + varname + ")";
					}
					break;
				case DataTypes_ArrayTypes_array:
					structunpackstring = CPPServiceLangGen_VerifyArrayLength(*t, "RobotRaconteur::MessageElement_Unpack" + a + "Array<" + fix_qualified_name(tt.cpp_type) + ">(" + varname + ")");
					break;
				case DataTypes_ArrayTypes_multidimarray:
					structunpackstring = CPPServiceLangGen_VerifyArrayLength(*t, "RobotRaconteur::MessageElement_Unpack" + a + "MultiDimArray<" + fix_qualified_name(tt.cpp_type) + ">(" + varname + ")");
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
			return CPPServiceLangGen_VerifyArrayLength(*t,"RobotRaconteur::MessageElement_UnpackList<" + remove_RR_INTRUSIVE_PTR(tt1.cpp_type) + " >(RRGetNodeWeak()," + varname + ")");
		case DataTypes_ContainerTypes_map_int32:
			return CPPServiceLangGen_VerifyArrayLength(*t,"RobotRaconteur::MessageElement_UnpackMap<int32_t," + remove_RR_INTRUSIVE_PTR(tt1.cpp_type) + " >(RRGetNodeWeak()," + varname + ")");
		case DataTypes_ContainerTypes_map_string:
			return CPPServiceLangGen_VerifyArrayLength(*t,"RobotRaconteur::MessageElement_UnpackMap<std::string," + remove_RR_INTRUSIVE_PTR(tt1.cpp_type) + " >(RRGetNodeWeak()," + varname + ")");
		default:
			throw DataTypeException("Invalid container type");
		}
	}

	//Code to generate the various types of generators


	string CPPServiceLangGen::GetPropertyDeclaration(PropertyDefinition* d, bool paramtype)
	{
		get_variable_type_result t=get_variable_type(*d->Type);
		t.name=fix_name(d->Name);
		if (paramtype)
		{
			return "virtual " + t.cpp_type + " get_" + t.name + "()";
		}
		else
		{
			return "get_" + t.name + "()";
		}

	}

	string CPPServiceLangGen::SetPropertyDeclaration(PropertyDefinition* d, bool paramtype)
	{
		get_variable_type_result t=get_variable_type(*d->Type);
		t.name=fix_name(d->Name);
		if (paramtype)
		{
			return "virtual void set_" + t.name + "(" + t.cpp_param_type +" value)";
		}
		else
		{
			return "set_" + t.name + "(value)";
		}
	}

	string CPPServiceLangGen::FunctionDeclaration(FunctionDefinition* d, bool inclass)
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
			return "virtual " + ret_type + " " + name + "(" + param_str + ")";
		else
			return name + "(" + param_str + ")";		
	}

	//Async properties and functions

	string CPPServiceLangGen::GetPropertyDeclaration_async(PropertyDefinition* d, bool paramtype)
	{
		get_variable_type_result t=get_variable_type(*d->Type);
		t.name=fix_name(d->Name);
		if (paramtype)
		{
			return "virtual void async_get_" + t.name + "(boost::function<void ("+ t.cpp_param_type +",RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE)";
		}
		else
		{
			return "async_get_" + t.name + "(handler)";
		}
	}

	string CPPServiceLangGen::SetPropertyDeclaration_async(PropertyDefinition* d, bool paramtype)
	{
		get_variable_type_result t=get_variable_type(*d->Type);
		t.name=fix_name(d->Name);
		if (paramtype)
		{
			return "virtual void async_set_" + t.name + "(" + t.cpp_param_type +" value,boost::function<void (RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE)";
		}
		else
		{
			return "async_set_" + t.name + "(value,rr_handler)";
		}
	}

	string CPPServiceLangGen::FunctionDeclaration_async(FunctionDefinition* d, bool inclass)
	{
		if (!d->IsGenerator())
		{
			get_variable_type_result t = get_variable_type(*d->ReturnType);

			t.name = fix_name(d->Name);

			std::string param_str = str_pack_parameters(d->Parameters, inclass);

			std::vector<std::string> s1;

			if (d->Parameters.size() > 0)
			{
				s1.push_back(param_str);
			}

			if (d->ReturnType->Type == DataTypes_void_t)
			{
				s1.push_back("boost::function<void (RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE");
			}
			else
			{
				s1.push_back("boost::function<void (" + t.cpp_param_type + ", RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE");
			}

			std::string s2 = boost::join(s1, ",");

			if (inclass)
			{
				return "virtual void async_" + t.name + "(" + s2 + ")";
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

			s1.push_back("boost::function<void (" + t.generator_cpp_type + ", RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > rr_handler, int32_t rr_timeout=RR_TIMEOUT_INFINITE");

			std::string s2 = boost::join(s1, ",");

			std::string name = fix_name(d->Name);

			if (inclass)
			{
				return "virtual void async_" + name + "(" + s2 + ")";
			}
			else
				return "async_" + name + "(" + param_str + ", rr_handler)";
		}		
	}

	//Rest of definitions


	string CPPServiceLangGen::EventDeclaration(EventDefinition* d, bool inclass, bool var)
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
				return ("virtual boost::signals2::signal<void (" + plist + ")>& get_" + fix_name(d->Name) + "()");
			else
				return ("boost::signals2::signal<void (" + plist + ")> rrvar_" + d->Name);
		else 
			return "get_" + fix_name(d->Name) + "()(" + str_pack_parameters(d->Parameters,false) +")";	

	}

	string CPPServiceLangGen::ObjRefDeclaration(ObjRefDefinition* d, bool inclass)
	{
		std::string member_name = fix_name(d->Name);
		std::string object_type = fix_qualified_name(d->ObjectType);

		if (d->ObjectType == "varobject") object_type = "RobotRaconteur::RRObject";

		std::string indexer = "";

		OBJREF_ARRAY_CONTAINER_CMD( d, ,
			indexer = inclass ? "int32_t ind" : "int32_t",
			indexer = inclass ? "int32_t ind" : "int32_t",
			indexer = inclass ? "const std::string& ind" : "std::string"
			)			
		
		if (inclass)
		{
			return "virtual RR_SHARED_PTR<" + object_type + " > get_" + member_name + "(" + indexer + ")";
		}
		else
		{
			return "get_" + member_name + "(" + indexer + ")";
		}
	}

	string CPPServiceLangGen::ObjRefDeclaration_async(ObjRefDefinition* d, bool inclass)
	{
		std::string member_name = fix_name(d->Name);
		std::string object_type = fix_qualified_name(d->ObjectType);
		
		if (d->ObjectType=="varobject") object_type="RobotRaconteur::RRObject";

		std::string indexer = "";

		OBJREF_ARRAY_CONTAINER_CMD(d , ,
			indexer = inclass ? "int32_t ind" : "int32_t",
			indexer = inclass ? "int32_t ind" : "int32_t",
			indexer = inclass ? "const std::string& ind" : "std::string"
		)
				
		std::string param;
		if (indexer.empty())
		{
			param="boost::function<void(RR_SHARED_PTR<" + object_type + ">,RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE";
		}
		else
		{
			param=indexer + ", boost::function<void(RR_SHARED_PTR<" + object_type + ">,RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>)> handler, int32_t timeout=RR_TIMEOUT_INFINITE";
		}
		if (inclass)
		{
			return "virtual void async_get_" + member_name + "(" + param + ")";
		}
		else
		{
			return "async_get_" + member_name + "(" + param + ")";
		}
	}

	string CPPServiceLangGen::GetPipeDeclaration(PipeDefinition*d, bool inclass)
	{
		get_variable_type_result t=get_variable_type(*d->Type);
		t.name=fix_name(d->Name);
		if (inclass)
		{
			return "virtual RR_SHARED_PTR<RobotRaconteur::Pipe<" + t.cpp_type + " > > get_" + t.name + "()";
		}
		else
		{
			return "get_" + t.name + "()";
		}
	}

	string CPPServiceLangGen::SetPipeDeclaration(PipeDefinition*d, bool inclass)
	{
		get_variable_type_result t=get_variable_type(*d->Type);
		t.name=fix_name(d->Name);
		if (inclass)
		{
			return "virtual void set_" + t.name + "(RR_SHARED_PTR<RobotRaconteur::Pipe<" + t.cpp_type +" > > value)";
		}
		else
		{
			return "set_" + t.name + "(value)";
		}
	}

	string CPPServiceLangGen::GetCallbackDeclaration(CallbackDefinition *d, bool inclass, bool var)
	{
		std::vector<std::string> pm(d->Parameters.size());
		for (size_t i = 0; i < d->Parameters.size(); i++)
		{
			get_variable_type_result t = get_variable_type(*d->Parameters[i]);
			pm[i] = t.cpp_type;
		}

		get_variable_type_result ret=get_variable_type(*d->ReturnType);

		std::string plist = boost::join(pm, ", ");
		if (inclass)
		{
			if (!var)
				return "virtual RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<" + ret.cpp_type + "(" + plist + ")"  + " > > > get_" + fix_name(d->Name) + "()";
			else
				return "boost::function<" + ret.cpp_type + "(" + plist + ")"  + " >";
		}
		else
		{
			return "get_" + fix_name(d->Name) + "()";
		}
	}

	string CPPServiceLangGen::SetCallbackDeclaration(CallbackDefinition*d, bool inclass)
	{
		std::vector<std::string> pm(d->Parameters.size());
		for (size_t i = 0; i < d->Parameters.size(); i++)
		{
			get_variable_type_result t = get_variable_type(*d->Parameters[i]);
			pm[i] = t.cpp_type;
		}

		get_variable_type_result ret=get_variable_type(*d->ReturnType);

		std::string plist = boost::join(pm, ", ");
		if (inclass)
		{
			return "virtual void set_" +fix_name(d->Name) + "(RR_SHARED_PTR<RobotRaconteur::Callback<boost::function<" + ret.cpp_type + "(" + plist + ")> > > value)";
		}
		else
		{
			return "set_" + fix_name(d->Name) + "(value)";
		}
	}

	string CPPServiceLangGen::GetWireDeclaration(WireDefinition*d, bool inclass)
	{
		get_variable_type_result t=get_variable_type(*d->Type);
		t.name=fix_name(d->Name);
		if (inclass)
		{
			return "virtual RR_SHARED_PTR<RobotRaconteur::Wire<" + t.cpp_type + " > > get_" + t.name + "()";
		}
		else
		{
			return "get_" + t.name + "()";
		}
	}

	string CPPServiceLangGen::SetWireDeclaration(WireDefinition*d, bool inclass)
	{
		get_variable_type_result t=get_variable_type(*d->Type);
		t.name=fix_name(d->Name);
		if (inclass)
		{
			return "virtual void set_" + t.name + "(RR_SHARED_PTR<RobotRaconteur::Wire<" + t.cpp_type +" > > value)";
		}
		else
		{
			return "set_" + t.name + "(value)";
		}
	}

	string CPPServiceLangGen::MemoryDeclaration(MemoryDefinition*d, bool inclass)
	{
		convert_type_result t=convert_type(*d->Type);
		t.name=fix_name(d->Name);
		if (inclass)
		{
			if (IsTypeNumeric(d->Type->Type))
			{
				switch (d->Type->ArrayType)
				{
				case DataTypes_ArrayTypes_array:
					return "virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemory<" + t.cpp_type + " > > get_" + t.name + "()";
				case DataTypes_ArrayTypes_multidimarray:
					return "virtual RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<" + t.cpp_type + " > > get_" + t.name + "()";
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
					return "virtual RR_SHARED_PTR<RobotRaconteur::" + c +  "ArrayMemory<" + t.cpp_type + " > > get_" + t.name + "()";
				case DataTypes_ArrayTypes_multidimarray:
					return "virtual RR_SHARED_PTR<RobotRaconteur::" + c + "MultiDimArrayMemory<" + t.cpp_type + " > > get_" + t.name + "()";
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

	void CPPServiceLangGen::GenerateInterfaceHeaderFile(ServiceDefinition* d, ostream* w)
	{
		ostream& w2=*w;

		w2 << "//This file is automatically generated. DO NOT EDIT!" << endl << endl;
		w2 << "#include <RobotRaconteur.h>" << endl;
		//w2 << "#include <memory>" << endl;
		//w2 << "#include <boost/shared_ptr.hpp>" << endl;
		//w2 << "#include <boost/make_shared.hpp>" << endl;
		//w2 << "#include <string>" << endl;
		w2 << "#include <boost/signals2.hpp>" << endl;
		w2 << "#include <boost/array.hpp>" << endl;
		w2 << "#include <boost/container/static_vector.hpp>" << endl;

		std::set<string> importedheaders;

		

		for (std::vector<string>::const_iterator e = d->Imports.begin(); e != d->Imports.end(); ++e)
		{
			importedheaders.insert(*e);
		}

		for (std::set<string>::const_iterator e=importedheaders.begin(); e!=importedheaders.end(); ++e)
		{
			w2 << "#include \"" << boost::replace_all_copy(fix_name(*e),".","__") << ".h\"" << endl;
		}

		w2 << "#pragma once" << endl << endl;

		vector<string> namespace_vec;
		split(namespace_vec,d->Name,boost::is_from_range('.','.'));

		for (vector<string>::iterator ns_e=namespace_vec.begin(); ns_e!=namespace_vec.end(); ns_e++)
		{
			w2 << "namespace " << fix_name(*ns_e) << endl << "{" << endl;
		}

		w2 << endl;

		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin(); e != d->Structures.end(); ++e)
		{
			w2 << "class " << fix_name((*e)->Name) << ";" << endl;
		}
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin(); e != d->Objects.end(); ++e)
		{
			w2 << "class " << fix_name((*e)->Name) << ";" << endl;
		}

		w2 << endl;

		GenerateConstants(d, w);

		w2 << endl;

		std::map<std::string, RR_SHARED_PTR<ServiceEntryDefinition> > pods;
		BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition> e, d->NamedArrays)
		{
			pods.insert(std::make_pair(e->Name, e));
		}

		BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition> e, d->Pods)
		{
			pods.insert(std::make_pair(e->Name, e));
		}

		while (!pods.empty())
		{
			RR_SHARED_PTR<ServiceEntryDefinition> e2;
			BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition> e, pods | boost::adaptors::map_values)
			{
				bool local_found = false;
				BOOST_FOREACH(RR_SHARED_PTR<MemberDefinition> m, e->Members)
				{
					RR_SHARED_PTR<PropertyDefinition> p = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(m);
					if (!p) InternalErrorException("");

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

			if (!e2) throw ServiceDefinitionException("Recursive pod detected");

			pods.erase(e2->Name);

			if (e2->EntryType == DataTypes_pod_t)
			{
				w2 << "class " << fix_name(e2->Name) << " : public RobotRaconteur::RRPod {" << endl;
				w2 << "public:" << endl;
			}
			else
			{
				w2 << "union " << fix_name(e2->Name) << "{" << endl;
				boost::tuple<DataTypes, size_t> namedarray_t = GetNamedArrayElementTypeAndCount(e2);
				TypeDefinition tdef;
				tdef.Type = namedarray_t.get<0>();
				convert_type_result t = convert_type(tdef);
				w2 << t.cpp_type << " a" << "[" << namedarray_t.get<1>() << "];" << endl;
				w2 << "struct s_type {" << endl;
			}
			BOOST_FOREACH(RR_SHARED_PTR<MemberDefinition> m, e2->Members)
			{
				RR_SHARED_PTR<PropertyDefinition> p = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(m);
				if (!p) InternalErrorException("");
				TypeDefinition t2;
				p->Type->CopyTo(t2);
				t2.RemoveArray();
				get_variable_type_result t = get_variable_type(t2);
				t.name = fix_name((m)->Name);
				if (p->Type->ArrayType == DataTypes_ArrayTypes_none)
				{
					w2 << t.cpp_type << " " << t.name << ";" << endl;
				}
				else
				{
					int32_t array_count = boost::accumulate(p->Type->ArrayLength, 1, std::multiplies<int32_t>());
					if (e2->EntryType == DataTypes_pod_t)
					{
						if (!p->Type->ArrayVarLength)
						{
							w2 << "RobotRaconteur::pod_field_array<" << t.cpp_type << "," << array_count << ",false> " << t.name << ";" << endl;
						}
						else
						{
							w2 << "RobotRaconteur::pod_field_array<" << t.cpp_type << "," << array_count << ",true> " << t.name << ";" << endl;
						}
					}
					else
					{
						w2 << t.cpp_type << " " << t.name << "[" << array_count << "];" << endl;
					}
				}


			}

			if (e2->EntryType == DataTypes_pod_t)
			{
				w2 << endl << "virtual std::string RRType() {return \"" << d->Name + "." << e2->Name << "\";  }" << endl;
			}
			else
			{
				w2 << "} s;" << endl;
			}

			w2 << "};" << endl;

			if (e2->EntryType == DataTypes_namedarray_t)
			{
				boost::tuple<DataTypes, size_t> namedarray_t = GetNamedArrayElementTypeAndCount(e2);
				w2 << "BOOST_STATIC_ASSERT(sizeof(" << fix_name(e2->Name) << ") == " << namedarray_t.get<1>() * RRArrayElementSize(namedarray_t.get<0>()) << ");" << endl;
			}

			w2 << endl;
		}

		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin(); e != d->Structures.end(); ++e)
		{
			w2 << "class " << fix_name((*e)->Name) << " : public RobotRaconteur::RRStructure {" << endl;
			w2 << "public:" << endl;
			MEMBER_ITER(PropertyDefinition)
				get_variable_type_result t=get_variable_type(*m->Type);
				t.name=fix_name((m)->Name);
				w2 << t.cpp_type << " " << t.name << ";" << endl;

				
			MEMBER_ITER_END()

			w2 << endl << "virtual std::string RRType() {return \"" << d->Name + "." << (*e)->Name <<"\";  }"<< endl;

			w2 << "};" << endl << endl;
		}

		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin(); e != d->Objects.end(); ++e)
		{
			
			vector<string> implements;

			for (vector<string>::const_iterator ee=(*e)->Implements.begin(); ee!=(*e)->Implements.end(); ++ee)
			{
				implements.push_back("public virtual " + fix_qualified_name(*ee));
			}

			if (implements.size() ==0) implements.push_back("public virtual RobotRaconteur::RRObject");


			w2 << "class " << fix_name((*e)->Name) << " : " << boost::join(implements,", ") << endl;
			w2 << "{" << endl;
			w2 << "public:" << endl;
			MEMBER_ITER(PropertyDefinition)
				if (m->Direction() != MemberDefinition_Direction_writeonly)
				{
					w2 << GetPropertyDeclaration(m.get(), true) << "=0;" << endl;
				}
				if (m->Direction() != MemberDefinition_Direction_readonly)
				{
					w2 << SetPropertyDeclaration(m.get(), true) << "=0;" << endl;
				}
				w2 << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(FunctionDefinition)
				w2 << FunctionDeclaration(m.get(),true) << "=0;" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(EventDefinition)
				w2 << EventDeclaration(m.get(),true) << "=0;" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(ObjRefDefinition)
				w2 << ObjRefDeclaration(m.get(),true) << "=0;" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(PipeDefinition)
				w2 << GetPipeDeclaration(m.get(),true) << "=0;" << endl;
				w2 << SetPipeDeclaration(m.get(),true)  << "=0;" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(CallbackDefinition)
				w2 << GetCallbackDeclaration(m.get(),true) << "=0;" << endl;
				w2 << SetCallbackDeclaration(m.get(),true)  << "=0;" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(WireDefinition)
				w2 << GetWireDeclaration(m.get(),true) << "=0;" << endl;
				w2 << SetWireDeclaration(m.get(),true)  << "=0;" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(MemoryDefinition)
				w2 << MemoryDeclaration(m.get(),true) << "=0;" << endl << endl;
			MEMBER_ITER_END()

			w2 << "virtual std::string RRType() {return \"" << d->Name + "." << (*e)->Name <<"\";  }" << endl;

			w2 << "};" << endl << endl;
		}
		
		for (vector<string>::iterator e=d->Exceptions.begin(); e!=d->Exceptions.end(); e++)
		{
			w2 << "class " << fix_name(*e) << " : public RobotRaconteur::RobotRaconteurRemoteException" << endl << "{" << endl;
			w2 << "    public:" << endl;
			w2 << "    " << fix_name(*e) << "(std::string message) : RobotRaconteur::RobotRaconteurRemoteException(\"" << d->Name << "." << *e << "\",message) {}" << endl;
			w2 << "};" << endl;
		}

		for (vector<string>::iterator ns_e=namespace_vec.begin(); ns_e!=namespace_vec.end(); ns_e++)
		{
			w2 << "}" << endl;
		}
		w2 << endl;

		if (!d->Pods.empty())
		{
			w2 << "namespace RobotRaconteur" << endl << "{" << endl;
			BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition> e, d->Pods)
			{
				w2 << "RRPrimUtilPod(" << fix_qualified_name(d->Name) << "::" << fix_name(e->Name) <<  ", \"" << d->Name << "." << e->Name <<  "\");" << endl;
			}
			BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition> e, d->NamedArrays)
			{
				boost::tuple<DataTypes, size_t> namedarray_t = GetNamedArrayElementTypeAndCount(e);
				TypeDefinition tdef;
				tdef.Type = namedarray_t.get<0>();
				convert_type_result t = convert_type(tdef);
				w2 << "RRPrimUtilNamedArray(" << fix_qualified_name(d->Name) << "::" << fix_name(e->Name) << ", \"" << d->Name << "." << e->Name << "\"," << t.cpp_type << ");" << endl;
				w2 << "RRPodStubNamedArrayType(" << fix_qualified_name(d->Name) << "::" << fix_name(e->Name) << ");" << endl;
				
			}
			w2 << "}" << endl;
		}


	}

	void CPPServiceLangGen::GenerateStubSkelHeaderFile(ServiceDefinition* d, ostream* w)
	{
		ostream& w2=*w;

		w2 << "//This file is automatically generated. DO NOT EDIT!" << endl << endl;
		w2 << "#include \""<< boost::replace_all_copy(fix_name(d->Name),".","__") << ".h\"" << endl;
		
		std::set<string> importedheaders;

		

		for (std::vector<string>::const_iterator e = d->Imports.begin(); e != d->Imports.end(); ++e)
		{
			importedheaders.insert(*e);
		}

		for (std::set<string>::const_iterator e=importedheaders.begin(); e!=importedheaders.end(); ++e)
		{
			w2 << "#include \"" << boost::replace_all_copy(fix_name(*e),".","__") << "_stubskel.h\"" << endl;
		}
		
		w2 << "#pragma once" << endl << endl;

		vector<string> namespace_vec;
		split(namespace_vec,d->Name,boost::is_from_range('.','.'));

		for (vector<string>::iterator ns_e=namespace_vec.begin(); ns_e!=namespace_vec.end(); ns_e++)
		{
			w2 << "namespace " << fix_name(*ns_e) << endl << "{" << endl;
		}

		w2 << endl;
		GenerateServiceFactoryHeader(d,w);
		w2 <<endl;
		GenerateStubHeader(d,w);
		w2 << endl;
		GenerateSkelHeader(d,w);
		w2 << endl;
		GenerateDefaultImplHeader(d, w);
		w2 << endl;
		
		for (vector<string>::iterator ns_e=namespace_vec.begin(); ns_e!=namespace_vec.end(); ns_e++)
		{
			w2 << "}" << endl;
		}
		
		w2 << endl;
		if (!d->Pods.empty())
		{
			w2 << "namespace RobotRaconteur" << endl;
			w2 << "{" << endl;

			BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition> e, d->Pods)
			{
				std::string q_name = fix_qualified_name(d->Name) + "::" + fix_name(e->Name);
				w2 << "template<>" << endl;
				w2 << "class PodStub<" << q_name << ">" << endl;
				w2 << "{" << endl;
				w2 << "public:" << endl;
				w2 << "    template<typename U>" << endl;
				w2 << "    static void PackField(const " << q_name << "& v, const std::string& name, U& out)" << endl;
				w2 << "    {" << endl;
				w2 << "    out.push_back(RobotRaconteur::CreateMessageElement(name, PodStub_PackPodToArray(v)));" << endl;
				w2 << "    }" << endl;
				w2 << "    template<typename U>" << endl;
				w2 << "    static void UnpackField(" << q_name << "& v, const std::string& name, U& in)" << endl;
				w2 << "    {" << endl;
				w2 << "    PodStub_UnpackPodFromArray(v, MessageElement::FindElement(in, name)->template CastData<MessageElementPodArray>());" << endl;
				w2 << "    }" << endl;
				w2 << "    static RR_INTRUSIVE_PTR<MessageElementPod> PackToMessageElementPod(const " << q_name << "& v)" << endl;
				w2 << "    {" << endl;
				w2 << "    std::vector<RR_INTRUSIVE_PTR<MessageElement> > o;" << endl;
				BOOST_FOREACH(RR_SHARED_PTR<MemberDefinition> m, e->Members)
				{
					RR_SHARED_PTR<PropertyDefinition> p = rr_cast<PropertyDefinition>(m);
					w2 << "    PodStub_PackField(v." << fix_name(p->Name) << ", \"" << p->Name << "\", o);" << endl;
				}
				w2 << "    return RobotRaconteur::CreateMessageElementPod(o);" << endl;
				w2 << "    }" << endl;
				w2 << "    static void UnpackFromMessageElementPod(" << q_name << "& v, RR_INTRUSIVE_PTR<MessageElementPod> m)" << endl;
				w2 << "    {" << endl;
				w2 << "    if (!m) throw NullValueException(\"Unexpected null value for pod unpack\");" << endl;
				//w2 << "    if (m->Type != \"" << d->Name << "." << e->Name << "\") throw DataTypeException(\"Pod type mismatch\");" << endl;
				w2 << "    std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> >& i = m->Elements;" << endl;
				BOOST_FOREACH(RR_SHARED_PTR<MemberDefinition> m, e->Members)
				{
					RR_SHARED_PTR<PropertyDefinition> p = rr_cast<PropertyDefinition>(m);
					w2 << "    PodStub_UnpackField(v." << fix_name(p->Name) << ", \"" << p->Name << "\", i);" << endl;
				}
				w2 << "    }" << endl;
				w2 << "};" << endl;
			}
			w2 << "}" << endl;

		}

		w2 << endl;
	}

	void CPPServiceLangGen::GenerateStubSkelFile(ServiceDefinition* d, ostream* w, string servicedef)
	{
		ostream& w2=*w;

		w2 << "//This file is automatically generated. DO NOT EDIT!" << endl << endl;
		w2 << "#ifndef ROBOTRACONTEUR_USE_STDAFX" << endl;
		w2 << "#include \""<< boost::replace_all_copy(fix_name(d->Name),".","__") << "_stubskel.h\"" << endl;
		w2 << "#include <boost/algorithm/string.hpp>" << endl;
		w2 << "#include <boost/lexical_cast.hpp>" << endl;
		w2 << "#include <boost/assign/list_of.hpp>" << endl;
		w2 << "#else" << endl;
		w2 << "#include \"stdafx.h\"" << endl;
		w2 << "#endif" << endl;		

		vector<string> namespace_vec;
		split(namespace_vec,d->Name,boost::is_from_range('.','.'));

		for (vector<string>::iterator ns_e=namespace_vec.begin(); ns_e!=namespace_vec.end(); ns_e++)
		{
			w2 << "namespace " << fix_name(*ns_e) << endl << "{" << endl;
		}

		GenerateServiceFactory(d,w,servicedef);
		w2 << endl;
		GenerateStubDefinition(d,w);
		w2 << endl;
		GenerateSkelDefinition(d,w);
		w2 << endl;
		GenerateDefaultImplDefinition(d, w);
		w2 << endl;
		for (vector<string>::iterator ns_e=namespace_vec.begin(); ns_e!=namespace_vec.end(); ns_e++)
		{
			w2 << "}" << endl;
		}
		
		w2 << endl;
	}

	void CPPServiceLangGen::GenerateServiceFactoryHeader(ServiceDefinition* d, ostream* w)
	{
		
		string factory_name=fix_name(boost::replace_all_copy(d->Name,".","__")) + "Factory";

		ostream& w2=*w;
		w2 << "class " << factory_name << " : public virtual RobotRaconteur::ServiceFactory" << endl << "{" << endl;
		w2 << "public:" << endl;

		w2 << "virtual std::string GetServiceName();" << endl;

		w2 << "virtual std::string DefString();" << endl;

		w2 << "virtual RR_SHARED_PTR<RobotRaconteur::StructureStub> FindStructureStub(const std::string& s);" << endl;
		
		w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> PackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> structin);" << endl;
				
		w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> UnpackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> mstructin);" << endl;

		w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodArray> PackPodArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> structure);" << endl;

		w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> UnpackPodArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodArray> structure);" << endl;

		w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodMultiDimArray> PackPodMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> structure);" << endl;

		w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> UnpackPodMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodMultiDimArray> structure);" << endl;
		
		w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedArray> PackNamedArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> structure);" << endl;

		w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> UnpackNamedArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedArray> structure);" << endl;

		w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedMultiDimArray> PackNamedMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> structure);" << endl;

		w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> UnpackNamedMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedMultiDimArray> structure);" << endl;

		w2 << "virtual RR_SHARED_PTR<RobotRaconteur::ServiceStub> CreateStub(const std::string& objecttype, const std::string& path, RR_SHARED_PTR<RobotRaconteur::ClientContext> context);" << endl;

		w2 << "virtual RR_SHARED_PTR<RobotRaconteur::ServiceSkel> CreateSkel(const std::string& objecttype, const std::string& path, RR_SHARED_PTR<RobotRaconteur::RRObject> obj, RR_SHARED_PTR<RobotRaconteur::ServerContext> context);" << endl;

		//w2 << "virtual RR_SHARED_PTR<RobotRaconteur::ServiceDefinition> ServiceDef();" << endl;
		
		//w2 << "virtual std::string RemovePath(const std::string &path);" << endl;

		w2 << "virtual void DownCastAndThrowException(RobotRaconteur::RobotRaconteurException& exp);" << endl;

		w2 << "virtual RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> DownCastException(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> exp);" << endl;

		w2 << "};" << endl;
	
	}

	void CPPServiceLangGen::GenerateServiceFactory(ServiceDefinition* d, ostream* w,string defstring)
	{
		string factory_name=fix_name(boost::replace_all_copy(d->Name,".","__")) + "Factory";
		ostream& w2=*w;

		w2 << "std::string " << factory_name << "::GetServiceName()" << endl;
		w2 << "{" << endl << "return \"" << d->Name << "\";" << endl << "}" << endl;

		w2 << "std::string " << factory_name << "::DefString()" << endl;
		w2 << "{" << endl << "std::string out(" << endl;
		vector<string> lines;
		boost::split(lines,defstring,boost::is_from_range('\n','\n'));
		for (vector<string>::iterator e=lines.begin(); e!=lines.end(); ++e)
		{			
			std::string l=boost::replace_all_copy(*e, "\\", "\\\\");
			boost::replace_all(l, "\"", "\\\"");
			boost::trim_if(l, boost::is_any_of("\r\n"));	
			w2 << "\"" << l << "\\n\"" << endl;
		}
		w2 << ");" << endl;
		w2 << "return out;" << endl;
		w2 << "}" << endl;

		w2 << "RR_SHARED_PTR<RobotRaconteur::StructureStub> " << factory_name << "::FindStructureStub(const std::string& s)" << endl << "{" << endl;
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(s);" << endl;
		
		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin(); e != d->Structures.end(); ++e)
		{
			w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::rr_cast<RobotRaconteur::StructureStub>(RR_MAKE_SHARED<" << fix_name((*e)->Name) << "_stub>(GetNode()));" << endl;
		}
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid structure stub type.\");" << endl;
		w2 << "}" << endl;
		
		w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> " << factory_name << "::PackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> structin)" << endl << "{" << endl;
		w2 << "std::string type=structin->RRType();";
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;
		
		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->PackStructure(structin);" << endl;
		w2 << "RR_SHARED_PTR<RobotRaconteur::StructureStub> stub=FindStructureStub(type);" << endl;
		w2 << "return stub->PackStructure(structin);" << endl;
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid structure stub type.\");" << endl;
		w2 << "}" << endl;

		w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> " << factory_name << "::UnpackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> mstructin)" << endl << "{" << endl;
		w2 << "std::string type=mstructin->GetTypeString();";
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;
		
		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->UnpackStructure(mstructin);" << endl;
		w2 << "RR_SHARED_PTR<RobotRaconteur::StructureStub> stub=FindStructureStub(type);" << endl;
		w2 << "return stub->UnpackStructure(mstructin);" << endl;
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid structure stub type.\");" << endl;
		w2 << "}" << endl;

		w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodArray> " << factory_name << "::PackPodArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> structin)" << endl << "{" << endl;
		w2 << "std::string type=structin->RRElementTypeString();" << endl;
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;

		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->PackPodArray(structin);" << endl;
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end(); ++e)
		{
			w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::PodStub_PackPodArray(RobotRaconteur::rr_cast<RobotRaconteur::RRPodArray<" << fix_name((*e)->Name) << "> >(structin));" << endl;
		}
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid pod type.\");" << endl;
		w2 << "}" << endl;

		w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseArray> " << factory_name << "::UnpackPodArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodArray> mstructin)" << endl << "{" << endl;
		w2 << "std::string type=mstructin->GetTypeString();" << endl;
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;
		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->UnpackPodArray(mstructin);" << endl;
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end(); ++e)
		{
			w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::PodStub_UnpackPodArray<" << fix_name((*e)->Name) << ">(mstructin);" << endl;
		}
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid pod type.\");" << endl;
		w2 << "}" << endl;

		w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodMultiDimArray> " << factory_name << "::PackPodMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> structin)" << endl << "{" << endl;
		w2 << "std::string type=structin->RRElementTypeString();" << endl;
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;
		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->PackPodMultiDimArray(structin);" << endl;
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end(); ++e)
		{
			w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::PodStub_PackPodMultiDimArray(RobotRaconteur::rr_cast<RobotRaconteur::RRPodMultiDimArray<" << fix_name((*e)->Name) << "> >(structin));" << endl;
		}
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid pod type.\");" << endl;
		w2 << "}" << endl;
		
		w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRPodBaseMultiDimArray> " << factory_name << "::UnpackPodMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementPodMultiDimArray> mstructin)" << endl << "{" << endl;
		w2 << "std::string type=mstructin->GetTypeString();" << endl;
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;
		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->UnpackPodMultiDimArray(mstructin);" << endl;
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Pods.begin(); e != d->Pods.end(); ++e)
		{
			w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::PodStub_UnpackPodMultiDimArray<" << fix_name((*e)->Name) << ">(mstructin);" << endl;
		}
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid pod type.\");" << endl;
		w2 << "}" << endl;

		w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedArray> " << factory_name << "::PackNamedArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> structin)" << endl << "{" << endl;
		w2 << "std::string type=structin->RRElementTypeString();" << endl;
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;

		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->PackNamedArray(structin);" << endl;
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin(); e != d->NamedArrays.end(); ++e)
		{
			w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::NamedArrayStub_PackNamedArray(RobotRaconteur::rr_cast<RobotRaconteur::RRNamedArray<" << fix_name((*e)->Name) << "> >(structin));" << endl;
		}
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid namedarray type.\");" << endl;
		w2 << "}" << endl;

		w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseArray> " << factory_name << "::UnpackNamedArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedArray> mstructin)" << endl << "{" << endl;
		w2 << "std::string type=mstructin->GetTypeString();" << endl;
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;
		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->UnpackNamedArray(mstructin);" << endl;
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin(); e != d->NamedArrays.end(); ++e)
		{
			w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::NamedArrayStub_UnpackNamedArray<" << fix_name((*e)->Name) << ">(mstructin);" << endl;
		}
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid namedarray type.\");" << endl;
		w2 << "}" << endl;

		w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedMultiDimArray> " << factory_name << "::PackNamedMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> structin)" << endl << "{" << endl;
		w2 << "std::string type=structin->RRElementTypeString();" << endl;
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;
		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->PackNamedMultiDimArray(structin);" << endl;
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin(); e != d->NamedArrays.end(); ++e)
		{
			w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::NamedArrayStub_PackNamedMultiDimArray(RobotRaconteur::rr_cast<RobotRaconteur::RRNamedMultiDimArray<" << fix_name((*e)->Name) << "> >(structin));" << endl;
		}
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid namedarray type.\");" << endl;
		w2 << "}" << endl;

		w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedBaseMultiDimArray> " << factory_name << "::UnpackNamedMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementNamedMultiDimArray> mstructin)" << endl << "{" << endl;
		w2 << "std::string type=mstructin->GetTypeString();" << endl;
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;
		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->UnpackNamedMultiDimArray(mstructin);" << endl;
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->NamedArrays.begin(); e != d->NamedArrays.end(); ++e)
		{
			w2 << "if (objecttype==\"" << (*e)->Name << "\") return RobotRaconteur::NamedArrayStub_UnpackNamedMultiDimArray<" << fix_name((*e)->Name) << ">(mstructin);" << endl;
		}
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid namedarray type.\");" << endl;
		w2 << "}" << endl;

		w2 << "RR_SHARED_PTR<RobotRaconteur::ServiceStub> " << factory_name << "::CreateStub(const std::string& type, const std::string& path, RR_SHARED_PTR<RobotRaconteur::ClientContext> context)" << endl << "{" << endl;
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;
		
		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->GetServiceType(servicetype)->CreateStub(type,path,context);" << endl;
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin(); e != d->Objects.end(); ++e)
		{
			w2 << "if (objecttype==\"" << (*e)->Name << "\") { RR_SHARED_PTR<" << fix_name((*e)-> Name) << "_stub> o=(RR_MAKE_SHARED<" << fix_name((*e)->Name) << "_stub>(path,context)); o->RRInitStub(); return o; }" << endl;
		}
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid structure stub type.\");" << endl;
		w2 << "}" << endl;


		w2 << "RR_SHARED_PTR<RobotRaconteur::ServiceSkel> " << factory_name << "::CreateSkel(const std::string& type, const std::string& path, RR_SHARED_PTR<RobotRaconteur::RRObject> obj, RR_SHARED_PTR<RobotRaconteur::ServerContext> context)" << endl << "{" << endl;
	
		w2 << "boost::tuple<std::string,std::string> res=RobotRaconteur::SplitQualifiedName(type);" << endl;
		
		w2 << "std::string servicetype=res.get<0>();" << endl;
		w2 << "std::string objecttype=res.get<1>();" << endl;
		w2 << "if (servicetype != \"" << d->Name << "\") return GetNode()->GetServiceType(servicetype)->CreateSkel(type,path,obj,context);" << endl;
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin(); e != d->Objects.end(); ++e)
		{
			w2 << "if (objecttype==\"" << (*e)->Name << "\") {RR_SHARED_PTR<"<< fix_name((*e)->Name) <<  "_skel> o=RR_MAKE_SHARED<" << fix_name((*e)->Name) << "_skel>(); o->Init(path,obj,context); return o; }" << endl;
		}
		w2 << "throw RobotRaconteur::ServiceException(\"Invalid structure skel type.\");" << endl;
		
		w2 << "return RR_SHARED_PTR<RobotRaconteur::ServiceSkel>();" << endl;
		w2 << "}" << endl;	

		w2 << "void " << factory_name << "::DownCastAndThrowException(RobotRaconteur::RobotRaconteurException& rr_exp)" << endl << "{" << endl;
		w2 << "std::string rr_type=rr_exp.Error;" << endl;
		w2 << "if (rr_type.find('.')==std::string::npos)" << endl;
		w2 << "{" << endl;
		w2 << "	return;" << endl;
		w2 << "}" << endl;
		w2 << "boost::tuple<std::string,std::string> rr_res=RobotRaconteur::SplitQualifiedName(rr_type);" << endl;
		w2 << "if (rr_res.get<0>() != \"" << d->Name << "\") GetNode()->DownCastAndThrowException(rr_exp);" << endl;
		for (vector<string>::iterator e=d->Exceptions.begin(); e!=d->Exceptions.end(); e++)
		{
			w2 << "if (rr_res.get<1>()==\"" << *e << "\") throw " << fix_name(*e) << "(rr_exp.Message);" << endl;
		}
		w2 << "return;" << endl;
		w2 << "}" << endl;

		w2 << "RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> " << factory_name << "::DownCastException(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> rr_exp)" << "{" << endl;
		w2 << "if (!rr_exp) return rr_exp;" << endl;
		w2 << "std::string rr_type=rr_exp->Error;" << endl;
		w2 << "if (rr_type.find('.')==std::string::npos)" << endl;
		w2 << "{" << endl;
		w2 << "	return rr_exp;" << endl;
		w2 << "}" << endl;
		w2 << "boost::tuple<std::string,std::string> rr_res=RobotRaconteur::SplitQualifiedName(rr_type);" << endl;
		w2 << "if (rr_res.get<0>() != \"" << d->Name << "\") return GetNode()->DownCastException(rr_exp);" << endl;
		for (vector<string>::iterator e=d->Exceptions.begin(); e!=d->Exceptions.end(); e++)
		{
			w2 << "if (rr_res.get<1>()==\"" << *e << "\") return RR_MAKE_SHARED<" << fix_name(*e) << ">(rr_exp->Message);" << endl;
		}
		w2 << "return rr_exp;" << endl;
		w2 << "}" << endl;

	}

	void CPPServiceLangGen::GenerateStubHeader(ServiceDefinition* d, ostream* w)
	{
		ostream& w2=*w;

		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin(); e != d->Structures.end(); ++e)
		{
			w2 << "class " << fix_name((*e)->Name) << "_stub : public virtual RobotRaconteur::StructureStub" << endl << "{" << endl;
			w2 << "public:" << endl;
			w2 << "" << fix_name((*e)->Name) << "_stub(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurNode> node) : RobotRaconteur::StructureStub(node) {}" << endl;
			w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> PackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> s);" << endl;
			w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> UnpackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> m);" << endl;
			w2 << "};" << endl << endl;
		}

		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin(); e != d->Objects.end(); ++e)
		{
			vector<string> implements;

			for (vector<string>::const_iterator ee=(*e)->Implements.begin(); ee!=(*e)->Implements.end(); ++ee)
			{
				std::string iname=fix_qualified_name(*ee);
				if (iname.find("::")!=std::string::npos)
				{
					size_t pos=iname.rfind("::");
					
					iname=iname.substr(0,pos) + "::async_" + iname.substr(pos+2,iname.size()-pos-2);
				}
				implements.push_back("public virtual " + iname);
				
			}

			w2 << "class async_" << fix_name((*e)->Name) ;
			if (implements.size() >0)
			{
				w2 << " : " << boost::join(implements,", ");
			}

			w2	<<  endl;
			w2 << "{" << endl;
			w2 << "public:" << endl;
						
			MEMBER_ITER(PropertyDefinition)
				if (m->Direction() != MemberDefinition_Direction_writeonly)
				{
					w2 << GetPropertyDeclaration_async(m.get(), true) << " = 0;" << endl;
				}
				if (m->Direction() != MemberDefinition_Direction_readonly)
				{
					w2 << SetPropertyDeclaration_async(m.get(), true) << " = 0;" << endl << endl;
				}
				
				w2 << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(FunctionDefinition)
				w2 << FunctionDeclaration_async(m.get(),true) << " = 0;" << endl << endl;
				
				w2 << endl;
			MEMBER_ITER_END()

				
			MEMBER_ITER(ObjRefDefinition)
				w2 << ObjRefDeclaration_async(m.get(),true) << "=0;" << endl << endl;
				
			MEMBER_ITER_END()

			w2 << "};" << endl;

		}

		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin(); e != d->Objects.end(); ++e)
		{
			w2 << "class " << fix_name((*e)->Name) << "_stub : public virtual " << fix_name((*e)->Name)  << ", public virtual async_" << fix_name((*e)->Name)  << ", public virtual RobotRaconteur::ServiceStub" << endl;
			w2 << "{" << endl;
			w2 << "public:" << endl;

			w2 << fix_name((*e)->Name) << "_stub(const std::string& path, RR_SHARED_PTR<RobotRaconteur::ClientContext> c);" << endl << endl;
			w2 << "virtual void RRInitStub();" << endl;

			MEMBER_ITER(PropertyDefinition)
				if (m->Direction() != MemberDefinition_Direction_writeonly)
				{
					w2 << GetPropertyDeclaration(m.get(), true) << ";" << endl;
				}
				if (m->Direction() != MemberDefinition_Direction_readonly)
				{
					w2 << SetPropertyDeclaration(m.get(), true) << ";" << endl << endl;
				}
			MEMBER_ITER_END()

			MEMBER_ITER(FunctionDefinition)
				w2 << FunctionDeclaration(m.get(),true) << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(EventDefinition)
				w2 << EventDeclaration(m.get(),true) << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(ObjRefDefinition)
				w2 << ObjRefDeclaration(m.get(),true) << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(PipeDefinition)
				w2 << GetPipeDeclaration(m.get(),true) << ";" << endl;
				w2 << SetPipeDeclaration(m.get(),true)  << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(CallbackDefinition)
				w2 << GetCallbackDeclaration(m.get(),true) << ";" << endl;
				w2 << SetCallbackDeclaration(m.get(),true)  << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(WireDefinition)
				w2 << GetWireDeclaration(m.get(),true) << ";" << endl;
				w2 << SetWireDeclaration(m.get(),true)  << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(MemoryDefinition)
				w2 << MemoryDeclaration(m.get(),true) << ";" << endl << endl;
			MEMBER_ITER_END()

			w2 <<endl;
			w2 << "virtual void DispatchEvent(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m);" << endl;
			w2 << "virtual void DispatchPipeMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m);" << endl;
			w2 << "virtual void DispatchWireMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m);" << endl;
			w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallbackCall(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m);" << endl;
			w2 << "virtual void RRClose();" << endl;
			w2 << "virtual RR_SHARED_PTR<RobotRaconteur::PipeClientBase> RRGetPipeClient(const std::string& membername);" << endl;
			w2 << "virtual RR_SHARED_PTR<RobotRaconteur::WireClientBase> RRGetWireClient(const std::string& membername);" << endl;

			w2 << "private:" << endl;

			MEMBER_ITER(EventDefinition)
				w2 << EventDeclaration(m.get(),true,true) << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(PipeDefinition)
				w2 << "RR_SHARED_PTR<RobotRaconteur::PipeClient<" << get_variable_type(*m->Type).cpp_type << " > > rrvar_" << m->Name << ";" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(CallbackDefinition)
				w2 << "RR_SHARED_PTR<RobotRaconteur::CallbackClient<" << GetCallbackDeclaration(m.get(),true,true) << " > > rrvar_" << m->Name << ";" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(WireDefinition)
				w2 << "RR_SHARED_PTR<RobotRaconteur::WireClient<" << get_variable_type(*m->Type).cpp_type << " > > rrvar_" << m->Name << ";" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(MemoryDefinition)
				if (IsTypeNumeric(m->Type->Type))
				{
					if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
						w2 << "RR_SHARED_PTR<RobotRaconteur::ArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > > rrvar_" << m->Name << ";" << endl;
					else
						w2 << "RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > > rrvar_" << m->Name << ";" << endl;
				}
				else
				{
					std::string c = "Pod";
					if (m->Type->ResolveNamedType()->RRDataType() == DataTypes_namedarray_t)
					{
						c = "Named";
					}
					if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
						w2 << "RR_SHARED_PTR<RobotRaconteur::" + c + "ArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > > rrvar_" << m->Name << ";" << endl;
					else
						w2 << "RR_SHARED_PTR<RobotRaconteur::" + c + "MultiDimArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > > rrvar_" << m->Name << ";" << endl;

				}
			MEMBER_ITER_END()

			MEMBER_ITER(PropertyDefinition)
				get_variable_type_result t=get_variable_type(*m->Type);
				if (m->Direction() != MemberDefinition_Direction_writeonly)
				{
					w2 << GetPropertyDeclaration_async(m.get(), true) << ";" << endl;
				}
				if (m->Direction() != MemberDefinition_Direction_readonly)
				{
					w2 << SetPropertyDeclaration_async(m.get(), true) << ";" << endl << endl;
				}
				w2 << "protected:" << endl;
				if (m->Direction() != MemberDefinition_Direction_writeonly)
				{
					w2 << "virtual void rrend_get_" << fix_name(m->Name) << "(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, boost::function< void (" << t.cpp_param_type << " ,RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > handler);" << endl;
				}
				if (m->Direction() != MemberDefinition_Direction_readonly)
				{
					w2 << "virtual void rrend_set_" << fix_name(m->Name) << "(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, boost::function< void (RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > handler);" << endl;
				}
				
				w2 << "public:" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(FunctionDefinition)
				w2 << FunctionDeclaration_async(m.get(),true) << ";" << endl << endl;
				if (!m->IsGenerator())
				{
					w2 << "protected:" << endl;
					if (m->ReturnType->Type == DataTypes_void_t)
					{

						w2 << "virtual void rrend_" << fix_name(m->Name) << "(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, boost::function< void (RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > handler);" << endl;
					}
					else
					{
						get_variable_type_result t = get_variable_type(*m->ReturnType);
						w2 << "virtual void rrend_" << fix_name(m->Name) << "(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, boost::function< void (" << t.cpp_param_type << " ,RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > handler);" << endl;
					}

					w2 << "public:" << endl;
				}
				else
				{
					convert_generator_result t = convert_generator(m.get());
					w2 << "virtual void rrend_" << fix_name(m->Name) << "(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, boost::function< void (" << t.generator_cpp_type << " ,RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > handler);" << endl;
				}
			MEMBER_ITER_END()

			
			MEMBER_ITER(ObjRefDefinition)
				w2 << ObjRefDeclaration_async(m.get(),true) << ";" << endl << endl;
			MEMBER_ITER_END()


			w2 << "virtual std::string RRType();" << endl;
			w2 << "};" << endl << endl;
		}
	}

	void CPPServiceLangGen::GenerateSkelHeader(ServiceDefinition *d, ostream* w)
	{
		ostream& w2=*w;

		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin(); e != d->Objects.end(); ++e)
		{
			w2 << "class " << fix_name((*e)->Name) << "_skel : " <<  "public virtual RobotRaconteur::ServiceSkel" << endl;
			w2 << "{" << endl;
			w2 << "public:" << endl;
			w2 << "virtual void Init(const std::string& path, RR_SHARED_PTR<RobotRaconteur::RRObject> object, RR_SHARED_PTR<RobotRaconteur::ServerContext> context);" << endl;
			w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallGetProperty(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m);" << endl << endl;
			w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallSetProperty(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m);" << endl << endl;
			w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallFunction(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m);" << endl << endl;
			w2 << "virtual void ReleaseCastObject();" << endl << endl;
			w2 << "virtual void RegisterEvents(RR_SHARED_PTR<RobotRaconteur::RRObject> rrobj1);" << endl << endl;
			w2 << "virtual void UnregisterEvents(RR_SHARED_PTR<RobotRaconteur::RRObject> rrobj1);" << endl << endl;
			w2 << "virtual RR_SHARED_PTR<RobotRaconteur::RRObject> GetSubObj(const std::string &name, const std::string &ind);" << endl << endl;
			w2 << "virtual void InitPipeServers(RR_SHARED_PTR<RobotRaconteur::RRObject> rrobj1);" << endl << endl;
			w2 << "virtual void InitWireServers(RR_SHARED_PTR<RobotRaconteur::RRObject> rrobj1);" << endl << endl;
			w2 << "virtual void DispatchPipeMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t e);" << endl << endl;
			w2 << "virtual void DispatchWireMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t e);" << endl << endl;
			w2 << "virtual void InitCallbackServers(RR_SHARED_PTR<RobotRaconteur::RRObject> o);" << endl << endl;
			w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallPipeFunction(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t e);" << endl << endl;
			w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallWireFunction(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t e);" << endl << endl;
			w2 << "virtual RR_SHARED_PTR<void> GetCallbackFunction(uint32_t endpoint, const std::string& membername);" << endl << endl;
			w2 << "virtual RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> CallMemoryFunction(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::Endpoint> e);" << endl << endl;
			w2 << "virtual std::string GetObjectType();" << endl;
			w2 << "virtual RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << fix_name((*e)->Name) << " > get_obj();" << endl << endl;
			w2 << "virtual RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << "async_" << fix_name((*e)->Name) << " > get_asyncobj();" << endl << endl;
			w2 << "protected:" << endl;
			MEMBER_ITER(PropertyDefinition)
			if (m->Direction() != MemberDefinition_Direction_writeonly)
			{
				w2 << "static void rr_get_" << fix_name(m->Name) << "(RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> skel, " << get_variable_type(*m->Type, true).cpp_type << " value, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::ServerEndpoint> ep);" << endl;
			}
			MEMBER_ITER_END()

			MEMBER_ITER(FunctionDefinition)
			
			vector<string> v1;
			if (!m->IsGenerator())
			{
				if (m->ReturnType->Type != DataTypes_void_t)
				{
					v1.push_back(get_variable_type(*m->ReturnType, true).cpp_type + " ret");
				}
				v1.push_back("RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::ServerEndpoint> ep");
				w2 << "static void rr_" << fix_name(m->Name) << "(RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> skel, " << boost::join(v1, ", ") << ");" << endl;
			}
			else
			{
				convert_generator_result t = convert_generator(m.get());
				w2 << "static void rr_" << fix_name(m->Name) << "(RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> skel, "
					<< t.generator_cpp_type << " ret, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::ServerEndpoint> ep" << ");" << endl;
			}
			MEMBER_ITER_END()
			w2 << " public:" << endl;

			MEMBER_ITER(EventDefinition)
			vector<string> params;
			for (vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee=m->Parameters.begin(); ee!=m->Parameters.end(); ++ee) params.push_back(get_variable_type(*(*ee)).cpp_type + " " + fix_name((*ee)->Name) );
			w2 << "void rr_" << fix_name(m->Name) << "_Handler(" << boost::join(params,", ") << ");" << endl;
			MEMBER_ITER_END()

			w2 << "protected:";
			MEMBER_ITER(EventDefinition)
			
			w2 << "boost::signals2::connection " << fix_name(m->Name) << "_rrconnection;" << endl;
			MEMBER_ITER_END()

			w2 << "bool rr_InitPipeServersRun;" << endl;
			w2 << "bool rr_InitWireServersRun;" << endl;
			MEMBER_ITER(PipeDefinition)
			w2 << "RR_SHARED_PTR<RobotRaconteur::PipeServer<" << get_variable_type(*m->Type).cpp_type << " > > rr_" << m->Name <<"_pipe;"<<endl;
			MEMBER_ITER_END()
			MEMBER_ITER(WireDefinition)
			w2 << "RR_SHARED_PTR<RobotRaconteur::WireServer<" << get_variable_type(*m->Type).cpp_type << " > > rr_" << m->Name <<"_wire;"<<endl;
			MEMBER_ITER_END()

			w2 << "public: " << endl;
			MEMBER_ITER(CallbackDefinition)
			vector<string> p;
			p.push_back("uint32_t rrendpoint");
			if (m->Parameters.size() > 0) p.push_back(str_pack_parameters(m->Parameters));
			w2 << get_variable_type(*m->ReturnType).cpp_type << " rr_" << m->Name << "_callback(" << boost::join(p,", ") << ");" << endl;
			MEMBER_ITER_END();
			w2 << "private:" << endl;
			MEMBER_ITER(MemoryDefinition)
			convert_type_result t=convert_type(*m->Type);
			t.name=m->Name;
			if (IsTypeNumeric(m->Type->Type))
			{
				if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
				{
					w2 << "RR_SHARED_PTR<RobotRaconteur::ArrayMemoryServiceSkel<" + t.cpp_type + " > > rr_" + t.name + "_mem;" << endl;
				}
				else
				{
					w2 << "RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemoryServiceSkel<" + t.cpp_type + " > > rr_" + t.name + "_mem;" << endl;
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
					w2 << "RR_SHARED_PTR<RobotRaconteur::" << c <<  "ArrayMemoryServiceSkel<" + t.cpp_type + " > > rr_" + t.name + "_mem;" << endl;
				}
				else
				{
					w2 << "RR_SHARED_PTR<RobotRaconteur::" << c << "MultiDimArrayMemoryServiceSkel<" + t.cpp_type + " > > rr_" + t.name + "_mem;" << endl;
				}
			}
			MEMBER_ITER_END()

			w2 << endl << "};" << endl << endl;
		}
	}

	string dforc(string definition, string clas)
	{
		string res1=trim_copy(replace_first_copy(definition,"virtual",""));

		string rettype;
		string function;

		int ccount=0;
		size_t pos=0;

		for (; pos<res1.size(); pos++)
		{
			if (res1[pos] == '<') ccount++;
			if (res1[pos] == '>') ccount--;

			if (ccount==0 && (res1[pos]==' ' || res1[pos]=='\t'))
			{
				break;
			}
		}

		rettype=trim_copy(res1.substr(0,pos));
		function=trim_copy(res1.substr(pos));

		if (function.length()==0) throw InternalErrorException("Internal error");
				
		return trim_copy(rettype) + " " + clas + "::" + trim_copy(function);
	}

	static std::string CPPServiceLangGen_unreliable_str(bool unreliable)
	{
		return unreliable ? "true" : "false";
	}

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

	void CPPServiceLangGen::GenerateStubDefinition(ServiceDefinition *d, ostream* w)
	{

		ostream& w2=*w;

		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Structures.begin(); e != d->Structures.end(); ++e)
		{
			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> " << fix_name((*e)->Name) << "_stub::PackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue> s)" << endl << "{" << endl;
			w2 << "RR_INTRUSIVE_PTR<" << fix_qualified_name((*e)->Name) << " > s2=RobotRaconteur::rr_cast<" << fix_qualified_name((*e)->Name) << " >(s);" << endl;
			w2 << "std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> > vret;" << endl;
			MEMBER_ITER(PropertyDefinition)
				w2 << "vret.push_back(" << str_pack_message_element(m->Name,"s2->" + fix_name(m->Name),m->Type) << ");" << endl;
			MEMBER_ITER_END()
			w2 << "return RobotRaconteur::CreateMessageElementStructure(\"" << d->Name << "." << (*e)->Name  << "\",vret);" << endl;
			w2 << "}" << endl;

			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::RRStructure> " << fix_name((*e)->Name) << "_stub::UnpackStructure(RR_INTRUSIVE_PTR<RobotRaconteur::MessageElementStructure> m)" << endl << "{" << endl;
			w2 << "std::vector<RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> >& i = m->Elements;" << endl;
			w2 << "RR_INTRUSIVE_PTR<" << fix_qualified_name((*e)->Name) << " > ret(new " << fix_qualified_name((*e)->Name) << "());" << endl;
			MEMBER_ITER(PropertyDefinition)
				w2 << "ret->" << fix_name(m->Name) << "=" << str_unpack_message_element("RobotRaconteur::MessageElement::FindElement(i,\"" + m->Name + "\")",m->Type) << ";" << endl;
			MEMBER_ITER_END()
			w2 << "return ret;" << endl;
			w2 << "}" << endl << endl;

		}




		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin(); e != d->Objects.end(); ++e)
		{
			MEMBER_ITER(PipeDefinition)
				if (CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
				{
					w2 << "static void " << fix_name((*e)->Name) << "_stub_rrverify_" << m->Name << "(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)" << endl << "{" << endl;
					w2 << CPPServiceLangGen_VerifyArrayLength(*m->Type, "RobotRaconteur::rr_cast<" + remove_RR_INTRUSIVE_PTR(get_variable_type(*m->Type).cpp_type) +  " >(value)") << ";" << endl;
					w2 << "}" << endl;
				}
			MEMBER_ITER_END()
			MEMBER_ITER(WireDefinition)
				if (CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
				{
					w2 << "static void " << fix_name((*e)->Name) << "_stub_rrverify_" << m->Name << "(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)" << endl << "{" << endl;
					w2 << CPPServiceLangGen_VerifyArrayLength(*m->Type, "RobotRaconteur::rr_cast<" + remove_RR_INTRUSIVE_PTR(get_variable_type(*m->Type).cpp_type) + " >(value)") << ";" << endl;
					w2 << "}" << endl;
				}
			MEMBER_ITER_END()

			//Constructor
			w2 << fix_name((*e)->Name) << "_stub::" << fix_name((*e)->Name) << "_stub(const std::string &path, RR_SHARED_PTR<RobotRaconteur::ClientContext> c) : RobotRaconteur::ServiceStub(path,c)"  <<"{ }" << endl;

			w2 << "void " << fix_name((*e)->Name) << "_stub::RRInitStub()" << endl << "{" << endl;
			MEMBER_ITER(PipeDefinition)
				std::string unreliable_str = CPPServiceLangGen_unreliable_str(m->IsUnreliable());
				std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
				if (!CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
				{
					w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::PipeClient<" << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(), " << unreliable_str << "," << direction_str << ") ;" << endl;
				}
				else
				{
					w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::PipeClient<" << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this(), " << unreliable_str << "," << direction_str << ",&" << fix_name((*e)->Name) << "_stub_rrverify_" << m->Name <<") ;" << endl;
				}
			MEMBER_ITER_END()

			MEMBER_ITER(CallbackDefinition)
				w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::CallbackClient<" << GetCallbackDeclaration(m.get(),true,true) << " > >(\"" << m->Name << "\") ;" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(WireDefinition)
				std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
				if (!CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
				{
					w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::WireClient<" << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << direction_str << ");" << endl;
				}
				else
				{
					w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::WireClient<" << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << direction_str << ",&" << fix_name((*e)->Name) << "_stub_rrverify_" << m->Name <<"); " << endl;
				}
			MEMBER_ITER_END()
			MEMBER_ITER(MemoryDefinition)
			std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
			if (IsTypeNumeric(m->Type->Type))
			{
				if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
				{
					w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::ArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << direction_str << ");" << endl;

				}
				else
				{
					w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::MultiDimArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << direction_str << ");" << endl;

				}
			}
			else
			{
				size_t elem_size;

				std::string c = "Pod";
				if (m->Type->ResolveNamedType()->RRDataType() == DataTypes_namedarray_t)
				{
					c = "Named";
					boost::tuple<DataTypes, size_t> namedarray_t = GetNamedArrayElementTypeAndCount(rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType()));
					elem_size = namedarray_t.get<1>();
				} 
				else
				{
					elem_size = EstimatePodPackedElementSize(rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType()));
				}

				if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
				{
					w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::" << c << "ArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << elem_size << "," << direction_str << ");" << endl;

				}
				else
				{
					w2 << "rrvar_" << m->Name << "=RR_MAKE_SHARED<RobotRaconteur::" << c << "MultiDimArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << elem_size <<  "," << direction_str << ");" << endl;

				}
			}
			MEMBER_ITER_END()
			w2 << "}" << endl << endl;

			//Access functions
			MEMBER_ITER(PropertyDefinition)
			if (m->Direction() != MemberDefinition_Direction_writeonly)
			{
				w2 << dforc(GetPropertyDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << endl << "{" << endl;
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertyGetReq,\"" << m->Name << "\");" << endl;
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr=ProcessRequest(m);" << endl;
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=mr->FindElement(\"value\");" << endl;
				w2 << "return " << str_unpack_message_element("me", m->Type) << ";" << endl;
				w2 << "}" << endl;
			}
			if (m->Direction() != MemberDefinition_Direction_readonly)
			{
				w2 << dforc(SetPropertyDeclaration(m.get(), true), fix_name((*e)->Name) + "_stub") << endl << "{" << endl;
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertySetReq,\"" << m->Name << "\");" << endl;
				w2 << "req->AddElement(" << str_pack_message_element("value", "value", m->Type) << ");" << endl;
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr=ProcessRequest(req);" << endl;
				w2 << "}" << endl << endl;
			}
			MEMBER_ITER_END()

			MEMBER_ITER(FunctionDefinition)
			w2 << dforc(FunctionDeclaration(m.get(),true),fix_name((*e)->Name) + "_stub") << endl << "{" << endl;
			if (!m->IsGenerator())
			{
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq,\"" << m->Name << "\");" << endl;
				for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator ee = m->Parameters.begin(); ee != m->Parameters.end(); ++ee)
				{
					w2 << "rr_req->AddElement(" << str_pack_message_element((*ee)->Name, fix_name((*ee)->Name), (*ee)) << ");" << endl;
				}
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_ret=ProcessRequest(rr_req);" << endl;
				if (m->ReturnType->Type != DataTypes_void_t)
				{
					w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> rr_me=rr_ret->FindElement(\"return\");" << endl;
					w2 << "return " << str_unpack_message_element("rr_me", m->ReturnType) << ";" << endl;
				}
			}
			else
			{
				convert_generator_result t = convert_generator(m.get());
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq,\"" << m->Name << "\");" << endl;
				for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator ee = t.params.begin(); ee != t.params.end(); ++ee)
				{
					w2 << "rr_req->AddElement(" << str_pack_message_element((*ee)->Name, fix_name((*ee)->Name), (*ee)) << ");" << endl;
				}
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_ret=ProcessRequest(rr_req);" << endl;
				w2 << "return RR_MAKE_SHARED<RobotRaconteur::GeneratorClient<" << t.return_type << "," << t.param_type << " > >(\"" << m->Name << "\", RobotRaconteur::RRArrayToScalar(rr_ret->FindElement(\"index\")->CastData<RobotRaconteur::RRArray<int32_t> >()),shared_from_this());" << endl;
			}
			w2 << "}" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(EventDefinition)
			w2 << dforc(EventDeclaration(m.get(),true),fix_name((*e)->Name) + "_stub") << endl << "{" << endl;
			w2 << "return rrvar_" << m->Name << ";" << endl;
			w2 << "}" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(ObjRefDefinition)
			w2 << dforc(ObjRefDeclaration(m.get(),true),fix_name((*e)->Name) + "_stub") << endl << "{" << endl;
			string objecttype=fix_qualified_name(m->ObjectType);
			
			if (m->ObjectType == "varobject")
			{	
				objecttype = "RobotRaconteur::RRObject";
				OBJREF_ARRAY_CONTAINER_CMD(m,
					w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRef(\"" << m->Name << "\"));" << endl,
					w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRef(\"" << m->Name << "\", boost::lexical_cast<std::string>(ind)));" << endl,
					w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRef(\"" << m->Name << "\", boost::lexical_cast<std::string>(ind)));" << endl,
					w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRef(\"" << m->Name << "\", ind));" << endl
				)
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

				OBJREF_ARRAY_CONTAINER_CMD(m,
					w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRefTyped(\"" << m->Name << "\",\"" << objecttype2 << "\"));" << endl,
					w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRefTyped(\"" << m->Name << "\", boost::lexical_cast<std::string>(ind),\"" << objecttype2 << "\"));" << endl,
					w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRefTyped(\"" << m->Name << "\", boost::lexical_cast<std::string>(ind),\"" << objecttype2 << "\"));" << endl,
					w2 << "return RobotRaconteur::rr_cast<" << objecttype << " >(FindObjRefTyped(\"" << m->Name << "\", ind,\"" << objecttype2 << "\"));" << endl
				)
			}
						
			w2 << "}" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(PipeDefinition)
			w2 << dforc(GetPipeDeclaration(m.get(),true),fix_name((*e)->Name) + "_stub") << endl << "{" << endl;
			get_variable_type_result t=get_variable_type(*m->Type);
			w2 << "RR_SHARED_PTR<RobotRaconteur::PipeClient<" << t.cpp_type << " > > value=rrvar_" << m->Name << ";" << endl;
			w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");" << endl;
			w2 << "return value;" << endl;
			w2 << "}" << endl;
			w2 << dforc(SetPipeDeclaration(m.get(),true),fix_name((*e)->Name) + "_stub") << endl << "{" << endl;
			w2 << "throw RobotRaconteur::InvalidOperationException(\"Not valid for client\");" << endl;
			w2 << "}" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(CallbackDefinition)
			w2 << dforc(GetCallbackDeclaration(m.get(),true),fix_name((*e)->Name)+"_stub") << endl << "{" << endl;
			w2 << "RR_SHARED_PTR<RobotRaconteur::CallbackClient<" << GetCallbackDeclaration(m.get(),true,true) << " > > value=rrvar_" << m->Name << ";" << endl;
			w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");" << endl;
			w2 << "return value;" << endl;
			w2 << "}" << endl;
			w2 << dforc(SetCallbackDeclaration(m.get(),true),fix_name((*e)->Name)+"_stub") << endl << "{" << endl;
			w2 << "throw RobotRaconteur::InvalidOperationException(\"Not valid for client\");" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(WireDefinition)
			w2 << dforc(GetWireDeclaration(m.get(),true),fix_name((*e)->Name) + "_stub") << endl << "{" << endl;
			get_variable_type_result t=get_variable_type(*m->Type);
			w2 << "RR_SHARED_PTR<RobotRaconteur::WireClient<" << t.cpp_type << " > > value=rrvar_" << m->Name << ";" << endl;
			w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");" << endl;
			w2 << "return value;" << endl;
			w2 << "}" << endl;
			w2 << dforc(SetWireDeclaration(m.get(),true),fix_name((*e)->Name) + "_stub") << endl << "{" << endl;
			w2 << "throw RobotRaconteur::InvalidOperationException(\"Not valid for client\");" << endl;
			w2 << "}" << endl << endl;

			MEMBER_ITER_END()

			MEMBER_ITER(MemoryDefinition)
			w2 << dforc(MemoryDeclaration(m.get(),true),fix_name((*e)->Name) + "_stub") << endl << "{" << endl;
			if (IsTypeNumeric(m->Type->Type))
			{
				if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
				{
					w2 << "RR_SHARED_PTR<RobotRaconteur::ArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > > value=rrvar_" << m->Name << ";" << endl;
					w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");" << endl;
					w2 << "return value;" << endl;
				}
				else
				{
					w2 << "RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > > value=rrvar_" << m->Name << ";" << endl;
					w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");" << endl;
					w2 << "return value;" << endl;
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
					w2 << "RR_SHARED_PTR<RobotRaconteur::" << c << "ArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > > value=rrvar_" << m->Name << ";" << endl;
					w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");" << endl;
					w2 << "return value;" << endl;
				}
				else
				{
					w2 << "RR_SHARED_PTR<RobotRaconteur::" << c << "MultiDimArrayMemoryClient<" << convert_type(*m->Type).cpp_type << " > > value=rrvar_" << m->Name << ";" << endl;
					w2 << "if (!value) throw RobotRaconteur::InvalidOperationException(\"Stub has been closed\");" << endl;
					w2 << "return value;" << endl;
				}
			}
			
			w2 << "}" << endl;
			MEMBER_ITER_END()

			w2 << "void " << fix_name((*e)->Name) << "_stub::DispatchEvent(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_m)" << endl << "{" << endl;
			//w2 << "boost::shared_lock<boost::shared_mutex> lock(context_lock);" << endl;
			MEMBER_ITER(EventDefinition)
			w2 << "if (rr_m->MemberName==\"" << m->Name << "\")" << endl << "{" << endl;
			for (std::vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee=m->Parameters.begin(); ee!=m->Parameters.end(); ++ee)
			{
				w2 << get_variable_type(*(*ee)).cpp_type << " " << fix_name((*ee)->Name) << "=" << str_unpack_message_element("rr_m->FindElement(\"" + (*ee)->Name + "\")",(*ee)) << ";" << endl;
				
			}
			w2 <<  EventDeclaration(m.get(),false) << ";" << endl;
			w2 << "return;" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END()
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl;

			w2 << "void " << fix_name((*e)->Name) << "_stub::DispatchPipeMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m)" << endl << "{" << endl;
			//w2 << "boost::shared_lock<boost::shared_mutex> lock(context_lock);" << endl;
			MEMBER_ITER(PipeDefinition)
			w2 << "if (m->MemberName==\"" << m->Name << "\")" << endl << "{" << endl;
			w2 << "rrvar_" << m->Name << "->" << "PipePacketReceived(m);" << endl;
			w2 << "return;" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END()
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl;

			w2 << "void " << fix_name((*e)->Name) << "_stub::DispatchWireMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m)" << endl << "{" << endl; 
			//w2 << "boost::shared_lock<boost::shared_mutex> lock(context_lock);" << endl;
			MEMBER_ITER(WireDefinition)
			w2 << "if (m->MemberName==\"" << m->Name << "\")" << endl << "{" << endl;
			w2 << "rrvar_" << m->Name << "->" << "WirePacketReceived(m);" << endl;
			w2 << "return;" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END()
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl;

			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>" << fix_name((*e)->Name) << "_stub::CallbackCall(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_m)" << endl << "{" << endl;
			//w2 << "boost::shared_lock<boost::shared_mutex> lock(context_lock);" << endl;
			w2 << "std::string ename=rr_m->MemberName;" << endl;
			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_mr=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_CallbackCallRet, ename);" << endl;
			w2 << "rr_mr->ServicePath=rr_m->ServicePath;" << endl;
			w2 << "rr_mr->RequestID=rr_m->RequestID;" << endl;
			MEMBER_ITER(CallbackDefinition)
			w2 << "if (rr_m->MemberName==\"" << m->Name << "\")" << endl << "{" << endl;
			vector<string> cvarnames;
			for (std::vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee=m->Parameters.begin(); ee!=m->Parameters.end(); ++ee)
			{
				w2 << get_variable_type(*(*ee)).cpp_type << " " << fix_name((*ee)->Name) << "=" << str_unpack_message_element("rr_m->FindElement(\"" + (*ee)->Name + "\")",(*ee)) << ";" << endl;
				cvarnames.push_back(fix_name((*ee)->Name));
			}
			if (m->ReturnType->Type==DataTypes_void_t)
			{
				w2 << "rrvar_" << m->Name << "->GetFunction()(" << boost::join(cvarnames,", ") <<")" << ";" << endl;
				w2 << "rr_mr->AddElement(\"return\",RobotRaconteur::ScalarToRRArray<int32_t>(0));" << endl;
			}
			else
			{
				w2 << get_variable_type(*m->ReturnType).cpp_type << " rr_ret=rrvar_" << m->Name << "->GetFunction()(" << boost::join(cvarnames,", ") << ")" << ";" << endl;
				w2 << "rr_mr->AddElement(" << str_pack_message_element("return","rr_ret",m->ReturnType) <<");" << endl;
			}
			w2 << "return rr_mr;" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END()
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl;

			w2 << endl;
			w2 << "std::string " << fix_name((*e)->Name) << "_stub::RRType()" << endl << "{" << endl;
			w2 << "return \"" << d->Name << "." << (*e)->Name << "\";" << endl;
			w2 << "}" << endl;

			w2 << "void " << fix_name((*e)->Name) << "_stub::RRClose()" << endl << "{" << endl;
			MEMBER_ITER(PipeDefinition)
			w2 << "rrvar_" << m->Name << "->" << "Shutdown();" << endl;
			//w2 << "rrvar_" << m->Name << ".reset();" << endl;
			MEMBER_ITER_END()
			MEMBER_ITER(WireDefinition)
			w2 << "rrvar_" << m->Name << "->" << "Shutdown();" << endl;
			//w2 << "rrvar_" << m->Name << ".reset();" << endl;
			MEMBER_ITER_END()
			MEMBER_ITER(MemoryDefinition)
			w2 << "if (rrvar_" << m->Name << ")" << endl << "{" << endl;
			w2 << "rrvar_" << m->Name << "->" << "Shutdown();" << endl;
			//w2 << "rrvar_" << m->Name << ".reset();" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(EventDefinition)
			w2 << "rrvar_" << m->Name << "." << "disconnect_all_slots();" << endl;
			//w2 << "rrvar_" << m->Name << ".reset();" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(CallbackDefinition)
			w2 << "if (rrvar_" << m->Name << ")" << endl << "{" << endl;
			w2 << "rrvar_" << m->Name << "->Shutdown();" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END()

			w2 << "ServiceStub::RRClose();" << endl;
			w2 << "}" << endl << endl;

			w2 << "RR_SHARED_PTR<RobotRaconteur::PipeClientBase> " << fix_name((*e)->Name) << "_stub::RRGetPipeClient(const std::string& membername)" << endl << "{" << endl;
			MEMBER_ITER(PipeDefinition)
			w2 << "if (membername ==\"" << m->Name << "\") return rrvar_" << m->Name << ";" << endl;
			MEMBER_ITER_END()
			w2 << "return ServiceStub::RRGetPipeClient(membername);" << endl;
			w2 << "}" << endl << endl;

			w2 << "RR_SHARED_PTR<RobotRaconteur::WireClientBase> " << fix_name((*e)->Name) << "_stub::RRGetWireClient(const std::string& membername)" << endl << "{" << endl;
			MEMBER_ITER(WireDefinition)
				w2 << "if (membername ==\"" << m->Name << "\") return rrvar_" << m->Name << ";" << endl;
			MEMBER_ITER_END()
				w2 << "return ServiceStub::RRGetWireClient(membername);" << endl;
			w2 << "}" << endl << endl;

 
			//Asynchronous operations

			MEMBER_ITER(PropertyDefinition)
			get_variable_type_result t=get_variable_type(*m->Type);
			if (m->Direction() != MemberDefinition_Direction_writeonly)
			{
				w2 << boost::replace_last_copy(dforc(GetPropertyDeclaration_async(m.get(), true), fix_name((*e)->Name) + "_stub"), "rr_timeout=RR_TIMEOUT_INFINITE", "rr_timeout") << endl << "{" << endl;
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertyGetReq,\"" << fix_name(m->Name) << "\");" << endl;
				w2 << "AsyncProcessRequest(m,boost::bind(&" << fix_name((*e)->Name) << "_stub::rrend_get_" << fix_name(m->Name) << ", RobotRaconteur::rr_cast<" << fix_name((*e)->Name) << "_stub>(shared_from_this()),_1,_2,rr_handler ),rr_timeout);" << endl;
				w2 << "}" << endl;
				w2 << "void " << fix_name((*e)->Name) << "_stub::rrend_get_" << fix_name(m->Name) << "(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, boost::function< void (" << t.cpp_param_type << " ,RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > handler)" << endl;
				w2 << "{" << endl;
				/*w2 << "if (err.value()==boost::system::errc::timed_out)" << endl;
				w2 << "{" << endl;
				w2 << "handler(" << GetDefaultValue(*m->Type) << ",RR_MAKE_SHARED<RobotRaconteur::RequestTimeoutException>(\"Request Timeout\"));" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;*/
				w2 << "if (err)" << endl;
				w2 << "{" << endl;
				w2 << "handler(" << GetDefaultValue(*m->Type) << ",err);" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;
				w2 << "if (m->Error != RobotRaconteur::MessageErrorType_None)" << endl;
				w2 << "{" << endl;
				w2 << "handler(" << GetDefaultValue(*m->Type) << ",RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;
				w2 << get_variable_type(*m->Type).cpp_type << " rr_ret;" << endl;
				w2 << "try" << endl;
				w2 << "{" << endl;
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"value\");" << endl;
				w2 << "rr_ret=" << str_unpack_message_element("me", m->Type) << ";" << endl;
				w2 << "}" << endl;
				w2 << "catch (std::exception& err2)" << endl << "{" << endl;
				w2 << "RobotRaconteur::detail::InvokeHandlerWithException(node, handler, err2, RobotRaconteur::MessageErrorType_DataTypeError);" << endl;
				w2 << "}" << endl;
				/*w2 << "catch (RobotRaconteur::RobotRaconteurException& err)" << endl;
				w2 << "{" << endl;
				w2 << "handler(" << GetDefaultValue(*m->Type) << ",RobotRaconteur::RobotRaconteurExceptionUtil::DownCastException(err));" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;
				w2 << "catch (std::exception& err)" << endl;
				w2 << "{" << endl;
				w2 << "handler(" << GetDefaultValue(*m->Type) << ",RR_MAKE_SHARED<RobotRaconteur::RobotRaconteurRemoteException>(std::string(typeid(err).name()),err.what()));" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;*/
				//w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"value\");" << endl;
				w2 << "handler(rr_ret, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());" << endl;
				w2 << "}" << endl;
			}
			if (m->Direction() != MemberDefinition_Direction_readonly)
			{
				w2 << boost::replace_last_copy(dforc(SetPropertyDeclaration_async(m.get(), true), fix_name((*e)->Name) + "_stub"), "rr_timeout=RR_TIMEOUT_INFINITE", "rr_timeout") << endl << "{" << endl;
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertySetReq,\"" << fix_name(m->Name) << "\");" << endl;
				w2 << "req->AddElement(" << str_pack_message_element("value", "value", m->Type) << ");" << endl;
				w2 << "AsyncProcessRequest(req,boost::bind(&" << fix_name((*e)->Name) << "_stub::rrend_set_" << fix_name(m->Name) << ", RobotRaconteur::rr_cast<" << fix_name((*e)->Name) << "_stub>(shared_from_this()),_1,_2,rr_handler ),rr_timeout);" << endl;
				w2 << "}" << endl;
				w2 << "void " << fix_name((*e)->Name) << "_stub::rrend_set_" << fix_name(m->Name) << "(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, boost::function< void (RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > handler)" << endl;
				w2 << "{" << endl;
				/*w2 << "if (err.value()==boost::system::errc::timed_out)" << endl;
				w2 << "{" << endl;
				w2 << "handler(RR_MAKE_SHARED<RobotRaconteur::RequestTimeoutException>(\"Request Timeout\"));" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;*/
				w2 << "if (err)" << endl;
				w2 << "{" << endl;
				w2 << "handler(err);" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;
				w2 << "if (m->Error != RobotRaconteur::MessageErrorType_None)" << endl;
				w2 << "{" << endl;
				w2 << "handler(RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;
				//w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"value\");" << endl;
				w2 << "handler(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());" << endl;
				w2 << "}" << endl;
			}
			MEMBER_ITER_END()

			MEMBER_ITER(FunctionDefinition)			
			w2 << boost::replace_last_copy(dforc(FunctionDeclaration_async(m.get(),true),fix_name((*e)->Name) + "_stub"),"rr_timeout=RR_TIMEOUT_INFINITE","rr_timeout") << endl << "{" << endl;
			if (!m->IsGenerator())
			{
				get_variable_type_result t = get_variable_type(*m->ReturnType);
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq,\"" << m->Name << "\");" << endl;
				for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator ee = m->Parameters.begin(); ee != m->Parameters.end(); ++ee)
				{
					w2 << "rr_req->AddElement(" << str_pack_message_element((*ee)->Name, fix_name((*ee)->Name), (*ee)) << ");" << endl;
				}
				w2 << "AsyncProcessRequest(rr_req,boost::bind(&" << fix_name((*e)->Name) << "_stub::rrend_" << fix_name(m->Name) << ", RobotRaconteur::rr_cast<" << fix_name((*e)->Name) << "_stub>(shared_from_this()),_1,_2,rr_handler ),rr_timeout);" << endl;

				w2 << "}" << endl << endl;

				if (m->ReturnType->Type == DataTypes_void_t)
				{

					w2 << "void " << fix_name((*e)->Name) << "_stub::rrend_" << fix_name(m->Name) << "(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, boost::function< void (RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > handler)" << endl;
				}
				else
				{
					w2 << "void " << fix_name((*e)->Name) << "_stub::rrend_" << fix_name(m->Name) << "(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, boost::function< void (" << t.cpp_param_type << " ,RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > handler)" << endl;
				}

				w2 << "{" << endl;
				/*w2 << "if (err.value()==boost::system::errc::timed_out)" << endl;
				w2 << "{" << endl;
				if (m->ReturnType->Type==DataTypes_void_t)
				{
					w2 << "handler(RR_MAKE_SHARED<RobotRaconteur::RequestTimeoutException>(\"Request Timeout\"));" << endl;
				}
				else
				{
					w2 << "handler(" << GetDefaultValue(*m->ReturnType) << ",RR_MAKE_SHARED<RobotRaconteur::RequestTimeoutException>(\"Request Timeout\"));" << endl;
				}
				w2 << "return;" << endl;
				w2 << "}" << endl;*/
				w2 << "if (err)" << endl;
				w2 << "{" << endl;
				if (m->ReturnType->Type == DataTypes_void_t)
				{
					w2 << "handler(err);" << endl;
				}
				else
				{
					w2 << "handler(" << GetDefaultValue(*m->ReturnType) << ",err);" << endl;
				}
				w2 << "return;" << endl;
				w2 << "}" << endl;
				w2 << "if (m->Error != RobotRaconteur::MessageErrorType_None)" << endl;
				w2 << "{" << endl;
				if (m->ReturnType->Type == DataTypes_void_t)
				{
					w2 << "handler(RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));" << endl;
				}
				else
				{
					w2 << "handler(" << GetDefaultValue(*m->ReturnType) << ",RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));" << endl;
				}
				w2 << "return;" << endl;
				w2 << "}" << endl;

				if (m->ReturnType->Type == DataTypes_void_t)
				{
					w2 << "handler(RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());" << endl;
				}
				else
				{
					w2 << get_variable_type(*m->ReturnType).cpp_type << " rr_ret;" << endl;
					w2 << "try" << endl;
					w2 << "{" << endl;
					w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"return\");" << endl;
					w2 << "rr_ret=" << str_unpack_message_element("me", m->ReturnType) << ";" << endl;
					w2 << "}" << endl;
					w2 << "catch (std::exception& err2)" << endl << "{" << endl;
					w2 << "RobotRaconteur::detail::InvokeHandlerWithException(node, handler, err2, RobotRaconteur::MessageErrorType_DataTypeError);" << endl;
					w2 << "}" << endl;
					/*w2 << "catch (RobotRaconteur::RobotRaconteurException& err)" << endl;
					w2 << "{" << endl;
					w2 << "handler(" << GetDefaultValue(*m->ReturnType) << ",RobotRaconteur::RobotRaconteurExceptionUtil::DownCastException(err));" << endl;
					w2 << "return;" << endl;
					w2 << "}" << endl;
					w2 << "catch (std::exception& err)" << endl;
					w2 << "{" << endl;
					w2 << "handler(" << GetDefaultValue(*m->ReturnType) << ",RR_MAKE_SHARED<RobotRaconteur::RobotRaconteurRemoteException>(std::string(typeid(err).name()),err.what()));" << endl;
					w2 << "return;" << endl;
					w2 << "}" << endl;*/
					//w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"value\");" << endl;
					w2 << "handler(rr_ret, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());" << endl;

					//w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> me=m->FindElement(\"return\");" << endl;
					//w2 << "handler(" << str_unpack_message_element("me",m->ReturnType) << ", RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());" << endl;
				}				
			}
			else
			{
				convert_generator_result t = convert_generator(m.get());
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallReq,\"" << m->Name << "\");" << endl;
				for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator ee = t.params.begin(); ee != t.params.end(); ++ee)
				{
					w2 << "rr_req->AddElement(" << str_pack_message_element((*ee)->Name, fix_name((*ee)->Name), (*ee)) << ");" << endl;
				}
				w2 << "AsyncProcessRequest(rr_req,boost::bind(&" << fix_name((*e)->Name) << "_stub::rrend_" << fix_name(m->Name) << ", RobotRaconteur::rr_cast<" << fix_name((*e)->Name) << "_stub>(shared_from_this()),_1,_2,rr_handler ),rr_timeout);" << endl;
				w2 << "}" << endl << endl;

				w2 << "void " << fix_name((*e)->Name) << "_stub::rrend_" << fix_name(m->Name) << "(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, boost::function< void (" << t.generator_cpp_type << " ,RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>) > handler)" << endl;
				w2 << "{" << endl;				
				w2 << "if (err)" << endl;
				w2 << "{" << endl;
				w2 << "handler(" << t.generator_cpp_type << "(),err);" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;
				w2 << "if (m->Error != RobotRaconteur::MessageErrorType_None)" << endl;
				w2 << "{" << endl;
				w2 << "handler(" << t.generator_cpp_type << "(),RobotRaconteur::RobotRaconteurExceptionUtil::MessageEntryToException(m));" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;
				
				w2 << "int32_t index;" << endl;
				w2 << "try" << endl;
				w2 << "{" << endl;
				w2 << "index=RobotRaconteur::RRArrayToScalar(m->FindElement(\"index\")->CastData<RobotRaconteur::RRArray<int32_t> >());" << endl;
				w2 << "}" << endl;
				w2 << "catch (std::exception& err2)" << endl << "{" << endl;
				w2 << "RobotRaconteur::detail::InvokeHandlerWithException(node, handler, err2, RobotRaconteur::MessageErrorType_DataTypeError);" << endl;
				w2 << "}" << endl;
				w2 << "handler(RR_MAKE_SHARED<RobotRaconteur::GeneratorClient<" << t.return_type << "," << t.param_type << " > >(\"" << m->Name << "\", index, shared_from_this()), RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException>());" << endl;
									
			}

			w2 << "}" << endl;

			MEMBER_ITER_END()

			MEMBER_ITER(ObjRefDefinition)
			w2 << boost::replace_all_copy(dforc(ObjRefDeclaration_async(m.get(),true),fix_name((*e)->Name) + "_stub"),"int32_t timeout=RR_TIMEOUT_INFINITE", "int32_t timeout") << endl << "{" << endl;
			string objecttype=fix_qualified_name(m->ObjectType);
			if (m->ObjectType=="varobject") 
			{
				objecttype="RobotRaconteur::RRObject";
			
				OBJREF_ARRAY_CONTAINER_CMD(m, 
					w2 << "AsyncFindObjRef(\"" << m->Name << "\", boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype << " >,_1,_2,handler) ,timeout);" << endl,
					w2 << "AsyncFindObjRef(\"" << m->Name << "\", boost::lexical_cast<std::string>(ind), boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype << " >,_1,_2,handler) ,timeout);" << endl,
					w2 << "AsyncFindObjRef(\"" << m->Name << "\", boost::lexical_cast<std::string>(ind), boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype << " >,_1,_2,handler) ,timeout);" << endl,
					w2 << "AsyncFindObjRef(\"" << m->Name << "\", ind, boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype << " >,_1,_2,handler) ,timeout);" << endl
					)				
			}
			else
			{
				string objecttype2="";
				
				if (m->ObjectType.find('.')==std::string::npos)
				{
					objecttype2=d->Name + "." + m->ObjectType;
				}
				else
				{
					objecttype2=m->ObjectType;
				}								

				OBJREF_ARRAY_CONTAINER_CMD(m,
					w2 << "AsyncFindObjRefTyped(\"" << m->Name << "\", \"" << objecttype2 << "\", boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype << " >,_1,_2,handler) ,timeout);" << endl,
					w2 << "AsyncFindObjRefTyped(\"" << m->Name << "\", boost::lexical_cast<std::string>(ind),\"" << objecttype2 << "\", boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype << " >,_1,_2,handler) ,timeout);" << endl,
					w2 << "AsyncFindObjRefTyped(\"" << m->Name << "\", boost::lexical_cast<std::string>(ind),\"" << objecttype2 << "\", boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype << " >,_1,_2,handler) ,timeout);" << endl,
					w2 << "AsyncFindObjRefTyped(\"" << m->Name << "\", ind, \"" << objecttype2 << "\", boost::bind(&RobotRaconteur::ServiceStub::EndAsyncFindObjRef<" << objecttype << " >,_1,_2,handler) ,timeout);" << endl
					)				
			}
			w2 << "}" << endl << endl;
			MEMBER_ITER_END()

		}
	}

	void CPPServiceLangGen::GenerateSkelDefinition(ServiceDefinition *d, ostream* w)
	{
		ostream& w2=*w;

		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin(); e != d->Objects.end(); ++e)
		{
			w2 << "void " << fix_name((*e)->Name) << "_skel::Init(const std::string& path, RR_SHARED_PTR<RobotRaconteur::RRObject> object, RR_SHARED_PTR<RobotRaconteur::ServerContext> context)" << endl << "{" << endl;
			w2 << "uncastobj=object;" << endl;
			w2 << "rr_InitPipeServersRun=false;" << endl;
			w2 << "rr_InitWireServersRun=false;" << endl;
			MEMBER_ITER(MemoryDefinition)
			w2 << "rr_" << m->Name << "_mem.reset();" << endl;
			MEMBER_ITER_END()
			w2 << "ServiceSkel::Init(path,object,context);" << endl;
			w2 << "}" << endl;
			w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << fix_name((*e)->Name) << " > " << fix_name((*e)->Name) << "_skel::" << "get_obj()" << endl << "{" << endl;
			w2 << "return RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << fix_name((*e)->Name) << " >(uncastobj);" << endl;
			w2 << "}" << endl;

			w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::async_" << fix_name((*e)->Name) << " > " << fix_name((*e)->Name) << "_skel::" << "get_asyncobj()" << endl << "{" << endl;
			w2 << "return RR_DYNAMIC_POINTER_CAST<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::async_" << fix_name((*e)->Name) << " >(uncastobj);" << endl;
			w2 << "}" << endl;

			w2 << "void " << fix_name((*e)->Name) << "_skel::" << "ReleaseCastObject() " << endl <<"{" << endl;
			MEMBER_ITER(PipeDefinition)
			w2 << "rr_" << m->Name << "_pipe->" << "Shutdown();" << endl;
			//w2 << "rr_" << m->Name << "_pipe.reset();" << endl;
			MEMBER_ITER_END()
			MEMBER_ITER(WireDefinition)
			w2 << "rr_" << m->Name << "_wire->" << "Shutdown();" << endl;
			//w2 << "rr_" << m->Name << "_wire.reset();" << endl;
			MEMBER_ITER_END()
			MEMBER_ITER(CallbackDefinition)
			w2 << "get_obj()->get_" << fix_name(m->Name) << "()->" << "Shutdown();" << endl;
			//w2 << "rr_" << m->Name << "_callback.reset();" << endl;
			MEMBER_ITER_END()
			w2 << "}" << endl;
			w2 << "std::string " << fix_name((*e)->Name) << "_skel::" << "GetObjectType()" << endl << "{" << endl << "return \"" << d->Name << "." << (*e)->Name << "\";" << endl << "}" << endl;
			
			//properties
			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name) << "_skel::" << "CallGetProperty(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m)" << endl << "{" << endl;
			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertyGetRes,m->MemberName);" << endl;
			w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << "async_" << fix_name((*e)->Name) << " > async_obj=get_asyncobj();" << endl;
			MEMBER_ITER(PropertyDefinition)
			w2 << "if (m->MemberName == \"" << m->Name << "\")" << endl << "{" << endl;
			if (m->Direction() != MemberDefinition_Direction_writeonly)
			{
				w2 << "if (async_obj)" << endl << "{" << endl;
				w2 << "RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> wp=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel>(shared_from_this());" << endl;
				w2 << "async_obj->async_get_" << fix_name(m->Name) << "(boost::bind(&" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel::rr_get_" << fix_name(m->Name) << ",wp,_1,_2,m,RobotRaconteur::ServerEndpoint::GetCurrentEndpoint()));" << endl;
				w2 << "return RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>();" << endl;
				w2 << "}" << endl;
				w2 << "else" << endl;
				w2 << "{" << endl;
				w2 << get_variable_type(*m->Type, true).cpp_type << " value=get_obj()->" << GetPropertyDeclaration(m.get(), false) << ";" << endl;
				w2 << "mr->AddElement(" << str_pack_message_element("value", "value", m->Type) << ");" << endl;
				w2 << "return mr;" << endl;
				w2 << "}" << endl;
			}
			else
			{
				w2 << "throw RobotRaconteur::WriteOnlyMemberException(\"Write only property\");" << endl;
			}
			w2 << "}" << endl;
			MEMBER_ITER_END()
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl << endl;

			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name) << "_skel::" <<  "CallSetProperty(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m)" << endl << "{" << endl;
			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> mr=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_PropertySetRes,m->MemberName);" << endl;
			w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << "async_" << fix_name((*e)->Name) << " > async_obj=get_asyncobj();" << endl;
			MEMBER_ITER(PropertyDefinition)
			w2 << "if (m->MemberName == \"" << m->Name << "\")" << endl << "{" << endl;
			if (m->Direction() != MemberDefinition_Direction_readonly)
			{
				w2 << get_variable_type(*m->Type, true).cpp_type << " value=" << str_unpack_message_element("m->FindElement(\"value\")", m->Type) << ";" << endl;
				w2 << "if (async_obj)" << endl << "{" << endl;
				w2 << "RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> wp=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel>(shared_from_this());" << endl;
				w2 << "async_obj->async_set_" << fix_name(m->Name) << "(value,boost::bind(&RobotRaconteur::ServiceSkel::EndAsyncCallSetProperty,wp,_1,m,RobotRaconteur::ServerEndpoint::GetCurrentEndpoint()));" << endl;
				w2 << "return RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>();" << endl;
				w2 << "}" << endl;
				w2 << "else" << endl;
				w2 << "{" << endl;
				w2 << "get_obj()->" << SetPropertyDeclaration(m.get(), false) << ";" << endl;
				w2 << "return mr;" << endl;
				w2 << "}" << endl;
			}
			else
			{
				w2 << "throw RobotRaconteur::ReadOnlyMemberException(\"Read only property\");" << endl;
			}
			w2 << "}" << endl;
			MEMBER_ITER_END()
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl << endl;

			MEMBER_ITER(PropertyDefinition)
			if (m->Direction() != MemberDefinition_Direction_writeonly)
			{
				w2 << "void " << fix_name((*e)->Name) << "_skel::rr_get_" << fix_name(m->Name) << "(RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> skel," << get_variable_type(*m->Type, true).cpp_type << " value, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::ServerEndpoint> ep)" << endl << "{" << endl;
				w2 << "if(err)" << endl << "{" << endl;
				w2 << "EndAsyncCallGetProperty(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),err,m, ep);" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> mr;" << endl;
				w2 << "try" << endl << "{" << endl;
				w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> skel1=skel.lock();" << endl;
				w2 << "if (!skel1) throw RobotRaconteur::InvalidOperationException(\"skel release\");" << endl;
				w2 << "mr=" << replace_all_copy(str_pack_message_element("value", "value", m->Type), "RRGetNodeWeak()", "skel1->RRGetNodeWeak()") << ";" << endl;
				w2 << "}" << endl;
				w2 << "catch (std::exception& err2)" << endl << "{" << endl;
				w2 << "EndAsyncCallGetProperty(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RobotRaconteur::RobotRaconteurExceptionUtil::ExceptionToSharedPtr(err2, RobotRaconteur::MessageErrorType_DataTypeError),m, ep);" << endl;
				w2 << "return;" << endl;
				w2 << "}" << endl;
				w2 << "EndAsyncCallGetProperty(skel, mr, err, m,ep);" << endl;
				/*w2 << "catch (RobotRaconteur::RobotRaconteurException& err2)" << endl << "{" << endl;
				w2 << "EndAsyncCallGetProperty(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RobotRaconteur::RobotRaconteurExceptionUtil::DownCastException(err2),m, ep);" << endl;
				w2 << "}" << endl;
				w2 << "catch (std::exception& err2)" << endl << "{" << endl;
				w2 << "EndAsyncCallGetProperty(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RR_MAKE_SHARED<RobotRaconteur::DataTypeException>(err2.what()),m, ep);" << endl;
				w2 << "}" << endl;*/
				w2 << "}" << endl;
			}
			MEMBER_ITER_END()

			//functions
			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name) << "_skel::" << "CallFunction(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_m)" << endl << "{" << endl;
			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_mr=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_FunctionCallRes,rr_m->MemberName);" << endl;
			w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << "async_" << fix_name((*e)->Name) << " > async_obj=get_asyncobj();" << endl;
			MEMBER_ITER(FunctionDefinition)
			w2 << "if (rr_m->MemberName == \"" << m->Name << "\")" << endl << "{" << endl;
			if (!m->IsGenerator())
			{
				vector<string> v1;

				for (vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee = m->Parameters.begin(); ee != m->Parameters.end(); ++ee)
				{
					w2 << get_variable_type(*(*ee), true).cpp_type << " " << fix_name((*ee)->Name) << " =" << str_unpack_message_element("rr_m->FindElement(\"" + (*ee)->Name + "\")", (*ee)) << ";" << endl;
					v1.push_back(fix_name((*ee)->Name));
				}

				if (m->ReturnType->Type == DataTypes_void_t)
				{
					v1.push_back("boost::bind(&" + boost::replace_all_copy(fix_name(d->Name), ".", "::") + "::" + fix_name((*e)->Name) + "_skel::rr_" + fix_name(m->Name) + ",rr_wp, _1, rr_m, RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())");

				}
				else
				{
					v1.push_back("boost::bind(&" + boost::replace_all_copy(fix_name(d->Name), ".", "::") + "::" + fix_name((*e)->Name) + "_skel::rr_" + fix_name(m->Name) + ", rr_wp, _1, _2, rr_m, RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())");
				}

				w2 << "if (async_obj)" << endl << "{" << endl;
				w2 << "RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> rr_wp=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel>(shared_from_this());" << endl;
				w2 << "async_obj->async_" << fix_name(m->Name) << "(" << boost::join(v1, ", ") << ");" << endl;
				w2 << "return RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>();" << endl;
				w2 << "}" << endl;
				w2 << "else" << endl;
				w2 << "{" << endl;
				if (m->ReturnType->Type != DataTypes_void_t)
				{
					w2 << get_variable_type(*m->ReturnType).cpp_type << " rr_return=get_obj()->" << FunctionDeclaration(m.get(), false) << ";" << endl;
					w2 << "rr_mr->AddElement(" << str_pack_message_element("return", "rr_return", m->ReturnType) << ");" << endl;
				}
				else
				{
					w2 << "get_obj()->" << FunctionDeclaration(m.get(), false) << ";" << endl;
					w2 << "rr_mr->AddElement(\"return\",RobotRaconteur::ScalarToRRArray<int32_t>(0));" << endl;
				}
				w2 << "return rr_mr;" << endl;
				w2 << "}" << endl;
				
			}
			else
			{
				vector<string> v1;

				convert_generator_result t = convert_generator(m.get());

				for (vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee = t.params.begin(); ee != t.params.end(); ++ee)
				{
					w2 << get_variable_type(*(*ee), true).cpp_type << " " << fix_name((*ee)->Name) << " =" << str_unpack_message_element("rr_m->FindElement(\"" + (*ee)->Name + "\")", (*ee)) << ";" << endl;
					v1.push_back(fix_name((*ee)->Name));
				}

				/*if (m->ReturnType->Type == DataTypes_void_t)
				{
					v1.push_back("boost::bind(&" + boost::replace_all_copy(fix_name(d->Name), ".", "::") + "::" + fix_name((*e)->Name) + "_skel::rr_" + fix_name(m->Name) + ",rr_wp, _1, rr_m, RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())");

				}
				else
				{
					v1.push_back("boost::bind(&" + boost::replace_all_copy(fix_name(d->Name), ".", "::") + "::" + fix_name((*e)->Name) + "_skel::rr_" + fix_name(m->Name) + ", rr_wp, _1, _2, rr_m, RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())");
				}

				w2 << "if (async_obj)" << endl << "{" << endl;
				w2 << "RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> rr_wp=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel>(shared_from_this());" << endl;
				w2 << "async_obj->async_" << fix_name(m->Name) << "(" << boost::join(v1, ", ") << ");" << endl;
				w2 << "return RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry>();" << endl;
				w2 << "}" << endl;
				w2 << "else" << endl;*/
				w2 << "{" << endl;
				
				w2 << t.generator_cpp_type << " rr_return=get_obj()->" << FunctionDeclaration(m.get(), false) << ";" << endl;
				w2 << "int32_t rr_index;" << endl;
				w2 << "{" << endl;
				w2 << "boost::mutex::scoped_lock lock(generators_lock);" << endl;
				w2 << "rr_index = get_new_generator_index();" << endl;
				w2 << "generators.insert(std::make_pair(rr_index,RR_MAKE_SHARED<RobotRaconteur::GeneratorServer<" << t.return_type << "," << t.param_type << " > >(rr_return, \"" << m->Name << "\",rr_index, shared_from_this(), RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())));" << endl;
				w2 << "}" << endl;
				w2 << "rr_mr->AddElement(\"index\", RobotRaconteur::ScalarToRRArray(rr_index));" << endl;
					
				
				w2 << "return rr_mr;" << endl;
				w2 << "}" << endl;
			}
			w2 << "}" << endl;
			MEMBER_ITER_END()
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl << endl;

			MEMBER_ITER(FunctionDefinition)
				if (!m->IsGenerator())
				{
					vector<string> v1;
					if (m->ReturnType->Type != DataTypes_void_t)
					{
						v1.push_back(get_variable_type(*m->ReturnType, true).cpp_type + " ret");
					}
					v1.push_back("RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::ServerEndpoint> ep");
					w2 << "void " << fix_name((*e)->Name) << "_skel::rr_" << fix_name(m->Name) << "(RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> skel, " << boost::join(v1, ", ") << ")" << endl << "{" << endl;
					w2 << "if(err)" << endl << "{" << endl;
					w2 << "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),err,m, ep);" << endl;
					w2 << "return;" << endl;
					w2 << "}" << endl;
					w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> mr;" << endl;
					w2 << "try" << endl << "{" << endl;
					if (m->ReturnType->Type != DataTypes_void_t)
					{
						w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> skel1=skel.lock();" << endl;
						w2 << "if (!skel1) throw RobotRaconteur::InvalidOperationException(\"skel release\");" << endl;

						w2 << "mr=" << boost::replace_all_copy(str_pack_message_element("return", "ret", m->ReturnType), "RRGetNodeWeak()", "skel1->RRGetNodeWeak()") << ";" << endl;

					}
					else
					{
						w2 << "mr=RobotRaconteur::CreateMessageElement(\"return\",RobotRaconteur::ScalarToRRArray<int32_t>(0));" << endl;

					}
					w2 << "}" << endl;
					w2 << "catch (std::exception& err2)" << endl << "{" << endl;
					w2 << "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RobotRaconteur::RobotRaconteurExceptionUtil::ExceptionToSharedPtr(err2, RobotRaconteur::MessageErrorType_DataTypeError),m, ep);" << endl;
					w2 << "return;" << endl;
					w2 << "}" << endl;
					w2 << "EndAsyncCallFunction(skel, mr, err, m,ep);" << endl; //TODO: move this
					/*w2 << "catch (RobotRaconteur::RobotRaconteurException& err2)" << endl << "{" << endl;
					w2 << "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RobotRaconteur::RobotRaconteurExceptionUtil::DownCastException(err2),m, ep);" << endl;
					w2 << "}" << endl;
					w2 << "catch (std::exception& err2)" << endl << "{" << endl;
					w2 << "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RR_MAKE_SHARED<RobotRaconteur::DataTypeException>(err2.what()),m, ep);" << endl;
					w2 << "}" << endl;*/
					w2 << "}" << endl;
				}
				else
				{
					convert_generator_result t = convert_generator(m.get());					
					w2 << "void " << fix_name((*e)->Name) << "_skel::rr_" << fix_name(m->Name) << "(RR_WEAK_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> skel, " 
						<< t.generator_cpp_type << " ret, RR_SHARED_PTR<RobotRaconteur::RobotRaconteurException> err, RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::ServerEndpoint> ep" << ")" << endl << "{" << endl;
					w2 << "if(err)" << endl << "{" << endl;
					w2 << "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),err,m, ep);" << endl;
					w2 << "return;" << endl;
					w2 << "}" << endl;
					w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> mr;" << endl;
					w2 << "try" << endl << "{" << endl;
					
					w2 << "RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name), ".", "::") << "::" << fix_name((*e)->Name) << "_skel> skel1=skel.lock();" << endl;
					w2 << "if (!skel1) throw RobotRaconteur::InvalidOperationException(\"skel release\");" << endl;
					w2 << "int32_t rr_index;" << endl;
					w2 << "{" << endl;
					w2 << "boost::mutex::scoped_lock lock(skel1->generators_lock);" << endl;
					w2 << "rr_index = skel1->get_new_generator_index();" << endl;
					w2 << "skel1->generators.insert(std::make_pair(rr_index,RR_MAKE_SHARED<RobotRaconteur::GeneratorServer<" << t.return_type << "," << t.param_type << " > >(ret, \"" << m->Name << "\",rr_index, skel1, RobotRaconteur::ServerEndpoint::GetCurrentEndpoint())));" << endl;
					w2 << "}" << endl;
					w2 << "mr = RobotRaconteur::CreateMessageElement(\"index\",RobotRaconteur::ScalarToRRArray(rr_index));" << endl;
					w2 << "}" << endl;
					w2 << "catch (std::exception& err2)" << endl << "{" << endl;
					w2 << "EndAsyncCallFunction(skel,RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement>(),RobotRaconteur::RobotRaconteurExceptionUtil::ExceptionToSharedPtr(err2, RobotRaconteur::MessageErrorType_DataTypeError),m, ep);" << endl;
					w2 << "return;" << endl;
					w2 << "}" << endl;
					w2 << "EndAsyncCallFunction(skel, mr, err, m,ep);" << endl;
					w2 << "}" << endl;

				}
			MEMBER_ITER_END()

			//events
			MEMBER_ITER(EventDefinition)
			vector<string> params;
			for (vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee=m->Parameters.begin(); ee!=m->Parameters.end(); ++ee) params.push_back(get_variable_type(*(*ee)).cpp_type + " " + fix_name((*ee)->Name) );
			w2 << "void " << fix_name((*e)->Name) << "_skel::rr_" <<  fix_name(m->Name) << "_Handler(" << boost::join(params,", ") << ")" << endl << "{" << endl;
			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_mm=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_EventReq,\"" << m->Name << "\");" << endl;
			for (vector<RR_SHARED_PTR<TypeDefinition> >::iterator ee=m->Parameters.begin(); ee!=m->Parameters.end(); ++ee)
			{
			w2 << "rr_mm->AddElement(" << str_pack_message_element((*ee)->Name,fix_name((*ee)->Name),*ee) << ");" << endl;
			}
			w2 << "SendEvent(rr_mm);" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END()

			w2 << "void " << fix_name((*e)->Name) << "_skel::RegisterEvents(RR_SHARED_PTR<RobotRaconteur::RRObject> rrobj1)" << endl << "{" << endl;
			w2 <<"RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << fix_name((*e)->Name) << " > obj=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::"<< fix_name((*e)->Name) << " >(rrobj1);" << endl;
			MEMBER_ITER(EventDefinition)
			vector<string> p;
			p.push_back("&" + fix_name((*e)->Name) + "_skel::rr_" + m->Name + "_Handler");
			p.push_back("RobotRaconteur::rr_cast<" + fix_name((*e)->Name) + "_skel>(shared_from_this())");
			for (size_t i=0; i<m->Parameters .size(); i++)
			{
				p.push_back("_" + boost::lexical_cast<string>(i+1));
			}

			w2 << m->Name << "_rrconnection=obj->get_" << fix_name(m->Name) << "().connect(boost::bind("<< boost::join(p,", ") <<"));" << endl;
			MEMBER_ITER_END()
			w2 << "RobotRaconteur::ServiceSkel::RegisterEvents(rrobj1);" << endl;
			w2 << "}" << endl << endl;

			w2 << "void " << fix_name((*e)->Name) << "_skel::UnregisterEvents(RR_SHARED_PTR<RobotRaconteur::RRObject> rrobj1)" << endl << "{" << endl;
			MEMBER_ITER(EventDefinition)
			w2 << m->Name << "_rrconnection.disconnect();" << endl;
			MEMBER_ITER_END()
			w2 << "RobotRaconteur::ServiceSkel::UnregisterEvents(rrobj1);" << endl;
			w2 << "}" << endl << endl;

			//objrefs
			w2 << "RR_SHARED_PTR<RobotRaconteur::RRObject> " << fix_name((*e)->Name) << "_skel::GetSubObj(const std::string &name, const std::string &ind)" << endl << "{" << endl;
			MEMBER_ITER(ObjRefDefinition)
			w2 << "if (name==\"" << m->Name << "\")" << endl << "{" << endl;
			OBJREF_ARRAY_CONTAINER_CMD(m,
				w2 << "return RobotRaconteur::rr_cast<RobotRaconteur::RRObject>(get_obj()->get_" << fix_name(m->Name) << "());" << endl,
				w2 << "return RobotRaconteur::rr_cast<RobotRaconteur::RRObject>(get_obj()->get_" << fix_name(m->Name) << "(boost::lexical_cast<int32_t>(ind)));" << endl,
				w2 << "return RobotRaconteur::rr_cast<RobotRaconteur::RRObject>(get_obj()->get_" << fix_name(m->Name) << "(boost::lexical_cast<int32_t>(ind)));" << endl,
				w2 << "return RobotRaconteur::rr_cast<RobotRaconteur::RRObject>(get_obj()->get_" << fix_name(m->Name) << "(ind));" << endl
				)
		
			w2 << "}" << endl;
			MEMBER_ITER_END()
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl << endl;

			//Pipes

			MEMBER_ITER(PipeDefinition)
				if (CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
				{
					w2 << "static void " << fix_name((*e)->Name) << "_skel_rrverify_" << m->Name << "(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)" << endl << "{" << endl;
					w2 << CPPServiceLangGen_VerifyArrayLength(*m->Type, "RobotRaconteur::rr_cast<" + remove_RR_INTRUSIVE_PTR(get_variable_type(*m->Type).cpp_type) + " >(value)") << ";" << endl;
					w2 << "}" << endl;
				}
			MEMBER_ITER_END()

			w2 << "void " << fix_name((*e)->Name) << "_skel::InitPipeServers(RR_SHARED_PTR<RobotRaconteur::RRObject> rrobj1)" << endl << "{" << endl;
			w2 << "if (rr_InitPipeServersRun) return;" << endl;
			w2 << "rr_InitPipeServersRun=true;" << endl;
			w2 <<"RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << fix_name((*e)->Name) << " > obj=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << fix_name((*e)->Name) << " >(rrobj1);" << endl;
			MEMBER_ITER(PipeDefinition)
				std::string unreliable_str = CPPServiceLangGen_unreliable_str(m->IsUnreliable());
				std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
				if (!CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
				{
					w2 << "rr_" << m->Name << "_pipe=RR_MAKE_SHARED<RobotRaconteur::PipeServer<" << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << unreliable_str << "," << direction_str << ");" << endl;
				}
				else
				{
					w2 << "rr_" << m->Name << "_pipe=RR_MAKE_SHARED<RobotRaconteur::PipeServer<" << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << unreliable_str << "," << direction_str << ",&" << fix_name((*e)->Name) << "_skel_rrverify_" << m->Name << ");" << endl;
				}
			MEMBER_ITER_END()
			MEMBER_ITER(PipeDefinition)
			w2 << "obj->set_" << fix_name(m->Name) << "(rr_" << m->Name << "_pipe);" << endl;
			MEMBER_ITER_END()
			w2 << "}" << endl << endl;

			w2 << "void " << fix_name((*e)->Name) << "_skel::DispatchPipeMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t e)" << endl <<"{" << endl;
			MEMBER_ITER(PipeDefinition)
			w2 << "if (m->MemberName==\"" << m->Name << "\")" << endl << "{" << endl;
			w2 << "rr_" << m->Name << "_pipe->PipePacketReceived(m,e);" <<endl;
			w2 << "return;" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END();
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl << endl;

			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name) << "_skel::CallPipeFunction(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t e)" << endl <<"{" << endl;
			MEMBER_ITER(PipeDefinition)
			w2 << "if (m->MemberName==\"" << m->Name << "\")" << endl << "{" << endl;
			w2 << "return rr_" << m->Name << "_pipe->PipeCommand(m,e);" <<endl;
			w2 << "}" << endl;
			MEMBER_ITER_END();
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl << endl;

			//wires

			MEMBER_ITER(WireDefinition)
				if (CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
				{
					w2 << "static void " << fix_name((*e)->Name) << "_skel_rrverify_" << m->Name << "(RR_INTRUSIVE_PTR<RobotRaconteur::RRValue>& value)" << endl << "{" << endl;
					w2 << CPPServiceLangGen_VerifyArrayLength(*m->Type, "RobotRaconteur::rr_cast<" + remove_RR_INTRUSIVE_PTR(get_variable_type(*m->Type).cpp_type) + " >(value)") << ";" << endl;
					w2 << "}" << endl;
				}
			MEMBER_ITER_END()

			w2 << "void " << fix_name((*e)->Name) << "_skel::InitWireServers(RR_SHARED_PTR<RobotRaconteur::RRObject> rrobj1)" << endl << "{" << endl;
			w2 << "if (rr_InitWireServersRun) return;" << endl;
			w2 << "rr_InitWireServersRun=true;" << endl;
			w2 <<"RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << fix_name((*e)->Name) << " > obj=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << fix_name((*e)->Name) << " >(rrobj1);" << endl;
			MEMBER_ITER(WireDefinition)
			std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
			if (!CPPServiceLangGen_UseVerifyArrayLength(*m->Type))
			{
				w2 << "rr_" << m->Name << "_wire=RR_MAKE_SHARED<RobotRaconteur::WireServer<" << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << direction_str << ");" << endl;
			}
			else
			{
				w2 << "rr_" << m->Name << "_wire=RR_MAKE_SHARED<RobotRaconteur::WireServer<" << get_variable_type(*m->Type).cpp_type << " > >(\"" << m->Name << "\",shared_from_this()," << direction_str << ",&" << fix_name((*e)->Name) << "_skel_rrverify_" << m->Name << ");" << endl;
			}
			MEMBER_ITER_END()
			MEMBER_ITER(WireDefinition)
			w2 << "obj->set_" << fix_name(m->Name) << "(rr_" << m->Name << "_wire);" << endl;
			MEMBER_ITER_END()
			w2 << "}" << endl << endl;

			w2 << "void " << fix_name((*e)->Name) << "_skel::DispatchWireMessage(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t e)" << endl <<"{" << endl;
			MEMBER_ITER(WireDefinition)
			w2 << "if (m->MemberName==\"" << m->Name << "\")" << endl << "{" << endl;
			w2 << "rr_" << m->Name << "_wire->WirePacketReceived(m,e);" <<endl;
			w2 << "return;" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END();
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl << endl;

			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name) << "_skel::CallWireFunction(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, uint32_t e)" << endl <<"{" << endl;
			MEMBER_ITER(WireDefinition)
			w2 << "if (m->MemberName==\"" << m->Name << "\")" << endl << "{" << endl;
			w2 << "return rr_" << m->Name << "_wire->WireCommand(m,e);" <<endl;
			w2 << "}" << endl;
			MEMBER_ITER_END();
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl << endl;


			w2 << "void " << fix_name((*e)->Name) << "_skel::InitCallbackServers(RR_SHARED_PTR<RobotRaconteur::RRObject> o)" << endl << "{" << endl;
			w2 <<"RR_SHARED_PTR<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << fix_name((*e)->Name) << " > obj=RobotRaconteur::rr_cast<" << boost::replace_all_copy(fix_name(d->Name),".","::") << "::" << fix_name((*e)->Name) << " >(o);" << endl;
			MEMBER_ITER(CallbackDefinition)
			w2 << "obj->set_" << fix_name(m->Name) << "(RR_MAKE_SHARED<RobotRaconteur::CallbackServer<" << GetCallbackDeclaration(m.get(),true,true) << " > >(\"" << m->Name << "\",shared_from_this()));" << endl;
			MEMBER_ITER_END()
			w2 << "}" << endl;
			MEMBER_ITER(CallbackDefinition)
			vector<string> p;
			p.push_back("uint32_t rrendpoint");
			if (m->Parameters.size() > 0) p.push_back(str_pack_parameters(m->Parameters));
			w2 << get_variable_type(*m->ReturnType).cpp_type << " " << fix_name((*e)->Name) << "_skel::rr_" << m->Name << "_callback(" << boost::join(p,", ") << ")" << "{" << endl;
			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_req=RobotRaconteur::CreateMessageEntry(RobotRaconteur::MessageEntryType_CallbackCallReq,\"" << m->Name << "\");" << endl;
			w2 << "rr_req->ServicePath=GetServicePath();" << endl;
			for (std::vector<RR_SHARED_PTR<TypeDefinition> >::const_iterator ee=m->Parameters.begin(); ee!=m->Parameters.end(); ++ee)
			{
				w2 << "rr_req->AddElement(" << str_pack_message_element((*ee)->Name,fix_name((*ee)->Name), (*ee)) << ");" << endl;
			}
			w2 << "RR_SHARED_PTR<RobotRaconteur::ServerContext> rr_s=GetContext();" << endl;
			w2 << "if (rr_s==0) throw RobotRaconteur::InvalidOperationException(\"Service has been closed\");" << endl;
			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> rr_ret=rr_s->ProcessCallbackRequest(rr_req,rrendpoint);" << endl;
			if (m->ReturnType->Type != DataTypes_void_t)
			{
				w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageElement> rr_me=rr_ret->FindElement(\"return\");" << endl;
				w2 << "return " << str_unpack_message_element("rr_me",m->ReturnType) << ";" << endl;
			}
			w2 << "}" << endl << endl;
			MEMBER_ITER_END()

			
			w2 << "RR_SHARED_PTR<void> " << fix_name((*e)->Name) << "_skel::GetCallbackFunction(uint32_t endpoint, const std::string& membername)" << endl << "{" << endl;
			MEMBER_ITER(CallbackDefinition)
			w2 << "if (membername==\"" << m->Name << "\")" << endl << "{" << endl;
			vector<string> p;
			p.push_back("&" + fix_name((*e)->Name) + "_skel::rr_" + m->Name + "_callback");
			p.push_back("RobotRaconteur::rr_cast<" + fix_name((*e)->Name) + "_skel>(shared_from_this())");
			p.push_back("endpoint");
			for (size_t i=0; i<m->Parameters .size(); i++)
			{
				p.push_back("_" + boost::lexical_cast<string>(i+1));
			}
			w2 << "return RR_MAKE_SHARED<" << GetCallbackDeclaration(m.get(),true,true) << " >(boost::bind(" << boost::join(p,", ") << "));" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END()
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl << endl;

			w2 << "RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> " << fix_name((*e)->Name) << "_skel::CallMemoryFunction(RR_INTRUSIVE_PTR<RobotRaconteur::MessageEntry> m, RR_SHARED_PTR<RobotRaconteur::Endpoint> e)" << endl << "{" << endl;
			
			MEMBER_ITER(MemoryDefinition)
			w2 << "if (m->MemberName==\"" << m->Name << "\")" << endl << "{" << endl;
			w2 << "if (rr_" << m->Name << "_mem==0) ";
				
			convert_type_result t=convert_type(*m->Type);
			t.name=m->Name;
			std::string direction_str = CPPServiceLangGen_direction_str(m->Direction());
			if (IsTypeNumeric(m->Type->Type))
			{
				if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
				{
					w2 << "rr_" + t.name + "_mem=RR_MAKE_SHARED<RobotRaconteur::ArrayMemoryServiceSkel<" + t.cpp_type + " > >(\"" << m->Name << "\",shared_from_this()," << direction_str << ");" << endl;
				}
				else
				{
					w2 << "rr_" + t.name + "_mem=RR_MAKE_SHARED<RobotRaconteur::MultiDimArrayMemoryServiceSkel<" + t.cpp_type + " > >(\"" << m->Name << "\",shared_from_this()," << direction_str << ");" << endl;
				}
			}
			else
			{
				std::string c = "Pod";
				size_t elem_size;
				if (m->Type->ResolveNamedType()->RRDataType() == DataTypes_namedarray_t)
				{
					c = "Named";

					boost::tuple<DataTypes, size_t> namedarray_t = GetNamedArrayElementTypeAndCount(rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType()));
					elem_size = namedarray_t.get<1>();
				}
				else
				{
					elem_size = EstimatePodPackedElementSize(rr_cast<ServiceEntryDefinition>(m->Type->ResolveNamedType()));
				}
				
				if (m->Type->ArrayType == DataTypes_ArrayTypes_array)
				{
					w2 << "rr_" + t.name + "_mem=RR_MAKE_SHARED<RobotRaconteur::" + c + "ArrayMemoryServiceSkel<" + t.cpp_type + " > >(\"" << m->Name << "\",shared_from_this()," << elem_size << "," << direction_str << ");" << endl;
				}
				else
				{
					w2 << "rr_" + t.name + "_mem=RR_MAKE_SHARED<RobotRaconteur::" + c + "MultiDimArrayMemoryServiceSkel<" + t.cpp_type + " > >(\"" << m->Name << "\",shared_from_this()," << elem_size << "," << direction_str << ");" << endl;
				}
			}
			w2 << "return rr_" << m->Name << "_mem->CallMemoryFunction(m,e,get_obj()->get_" << fix_name(m->Name) << "());" << endl;
			w2 << "}" << endl;
			MEMBER_ITER_END()
			
			w2 << "throw RobotRaconteur::MemberNotFoundException(\"Member not found\");" << endl;
			w2 << "}" << endl;

		}

	}

	
	template <typename T>
	static void null_deleter(T*) {}

	static std::string CPPServiceLang_convert_constant_EscapeString_Formatter(const boost::smatch& match)
	{
		std::string i = match[0].str();

		if (i == "\"") return "\\\"";
		if (i == "\\") return "\\\\";		
		if (i == "\b") return "\\b";
		if (i == "\f") return "\\f";
		if (i == "\n") return "\\n";
		if (i == "\r") return "\\r";
		if (i == "\t") return "\\t";

		std::string& v = i;

		std::stringstream v2;
		v2 << std::hex << std::setfill('0');
		BOOST_FOREACH(const uint8_t& v3, v)
		{
			uint32_t v4 = v3;
			v2 << std::setw(0) << "\\x" << std::setw(2) << v4;
		}

		return v2.str();
	}

	std::string CPPServiceLangGen::convert_constant(ConstantDefinition* c, std::vector<RR_SHARED_PTR<ConstantDefinition> >& c2, ServiceDefinition* def)
	{				
		boost::shared_ptr<TypeDefinition> t=c->Type;		
		if(t->ContainerType != DataTypes_ContainerTypes_none) throw DataTypeException("Only numbers, primitive number arrays, and strings can be constants");
		switch (t->ArrayType)
		{
		case DataTypes_ArrayTypes_none:
			break;
		case DataTypes_ArrayTypes_array:
			if (t->ArrayVarLength) break;
		default:
			throw DataTypeException("Only numbers, primitive number arrays, and strings can be constants");
		}
		
		convert_type_result c1=convert_type(*t);
		if (t->Type==DataTypes_string_t)
		{
			const std::string v1 = c->ValueToString();
			boost::regex r_replace("(\"|\\\\|[\\x00-\\x1F]|\\x7F|[\\x80-\\xFF]+)");

			std::ostringstream t2(std::ios::out | std::ios::binary);
			std::ostream_iterator<char, char> oi(t2);

			boost::regex_replace(oi, v1.begin(), v1.end(), r_replace, CPPServiceLang_convert_constant_EscapeString_Formatter,
				boost::match_default | boost::format_all);

			return "static const char* " + fix_name(c->Name) + "=\"" + t2.str() + "\";";
		}
		
		if (t->Type == DataTypes_namedtype_t)
		{
			std::vector<ConstantDefinition_StructField> f = c->ValueToStructFields();

			std::string o = "namespace " + fix_name(c->Name) + " { ";

			BOOST_FOREACH(ConstantDefinition_StructField f2, f)
			{
				RR_SHARED_PTR<ConstantDefinition> c3 = TryFindByName(c2, f2.ConstantRefName);
				if (!c3) throw ServiceException("Invalid structure cosntant " + c->Name);
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
		if (!f->IsGenerator()) throw InternalErrorException("");
		
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

	void CPPServiceLangGen::GenerateConstants(ServiceDefinition* d, ostream* w)
	{
		ostream& w2=*w;

		bool hasconstants=false;

		for (vector<string>::iterator e=d->Options.begin(); e!=d->Options.end(); ++e)
		{
			if (boost::starts_with(*e,"constant")) hasconstants=true;
		}

		if (!d->Enums.empty() || !d->Constants.empty()) hasconstants = true;

		for (vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator ee=d->Objects.begin(); ee!=d->Objects.end(); ++ee)
		{
			for (vector<string>::iterator e=(*ee)->Options.begin(); e!=(*ee)->Options.end(); ++e)
			{
				if (boost::starts_with(*e,"constant")) hasconstants=true;
			}

			if (!(*ee)->Constants.empty()) hasconstants = true;
		}

		if (!hasconstants) return;

		
		w2 << "namespace " << boost::replace_all_copy(fix_name(d->Name),".","__") << "Constants " << endl << "{" << endl;
			

		for (vector<string>::iterator e=d->Options.begin(); e!=d->Options.end(); ++e)
		{
			if (boost::starts_with(*e,"constant"))
			{
				RR_SHARED_PTR<ServiceDefinition> def2(d, null_deleter<ServiceDefinition>);
				RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(def2);
				c->FromString(*e);

				std::vector<RR_SHARED_PTR<ConstantDefinition> > c2;
				w2 << "    " << convert_constant(c.get(), c2, d) << endl;
			}
		}

		BOOST_FOREACH(RR_SHARED_PTR<ConstantDefinition>& c, d->Constants)
		{
			w2 << "    " << convert_constant(c.get(), d->Constants, d) << endl;
		}
				
		for (vector<boost::shared_ptr<ServiceEntryDefinition> >::iterator ee=d->Objects.begin(); ee!=d->Objects.end(); ++ee)
		{
			bool objhasconstants=false;

			for (vector<string>::iterator e=(*ee)->Options.begin(); e!=(*ee)->Options.end(); ++e)
			{
				if (boost::starts_with(*e,"constant")) objhasconstants=true;
			}

			if (objhasconstants)
			{
				w2 << "    namespace " << fix_name((*ee)->Name) <<  endl << "    {" << endl;
				for (vector<string>::iterator e=(*ee)->Options.begin(); e!=(*ee)->Options.end(); ++e)
				{
					if (boost::starts_with(*e,"constant"))
					{
						RR_SHARED_PTR<ServiceDefinition> def2(d, null_deleter<ServiceDefinition>);
						RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(def2);
						c->FromString(*e);
						std::vector<RR_SHARED_PTR<ConstantDefinition> > c2;
						w2 << "    " << convert_constant(c.get(), c2, d) << endl;
					}
				}

				BOOST_FOREACH(RR_SHARED_PTR<ConstantDefinition>& c, (*ee)->Constants)
				{
					w2 << "    " << convert_constant(c.get(), (*ee)->Constants, d) << endl;
				}

				w2 << "    }" << endl;
			}
		}

		w2 << "}" << endl;

		BOOST_FOREACH(RR_SHARED_PTR<EnumDefinition>& e, d->Enums)
		{
			w2 << "    namespace " << fix_name(e->Name) << std::endl;
			w2 << "    {" << std::endl;
			w2 << "    enum " << fix_name(e->Name) << std::endl;
			w2 << "    {" << std::endl;
			for (size_t i = 0; i<e->Values.size(); i++)
			{
				EnumDefinitionValue &v = e->Values[i];
				if (!v.HexValue)
				{
					w2 << "    " << fix_name(v.Name) << " = " << v.Value;
				}
				else
				{
					w2 << "    " << fix_name(v.Name) << " = 0x" << std::hex << v.Value << std::dec;
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
	}

	void CPPServiceLangGen::GenerateFiles(RR_SHARED_PTR<ServiceDefinition> d, std::string servicedef,std::string path)
	{
#ifdef _WIN32
		const std::string os_pathsep("\\");
#else
		const std::string os_pathsep("/");
#endif

		ofstream f1((path+os_pathsep+boost::replace_all_copy(fix_name(d->Name),".","__") + ".h").c_str());
		GenerateInterfaceHeaderFile(d.get(),&f1);
		f1.close();

		ofstream f2((path+os_pathsep+boost::replace_all_copy(fix_name(d->Name),".","__") + "_stubskel.h").c_str());
		GenerateStubSkelHeaderFile(d.get(),&f2);
		f2.close();

		ofstream f3((path+os_pathsep+boost::replace_all_copy(fix_name(d->Name),".","__") + "_stubskel.cpp").c_str());
		GenerateStubSkelFile(d.get(),&f3,servicedef);
		f3.close();


	}

	std::string CPPServiceLangGen::GetDefaultValue(const TypeDefinition& tdef)
	{
		if (tdef.Type == DataTypes_void_t) throw InternalErrorException("Internal error");
		if (tdef.ArrayType == DataTypes_ArrayTypes_none && tdef.ContainerType == DataTypes_ContainerTypes_none)
		{
			if (tdef.Type == DataTypes_double_t || tdef.Type == DataTypes_single_t) return "0.0";
			if (tdef.Type >= DataTypes_int8_t && tdef.Type <= DataTypes_uint64_t) return "0";
			if (tdef.Type == DataTypes_cdouble_t) return "RobotRaconteur::cdouble(0.0,0.0)";
			if (tdef.Type == DataTypes_csingle_t) return "RobotRaconteur::cfloat(0.0,0.0)";
			if (tdef.Type == DataTypes_bool_t) return "RobotRaconteur::rr_bool(0)";
			if (tdef.Type == DataTypes_string_t) return "\"\"";
		}

		return get_variable_type(tdef).cpp_type + "()";
	}

	std::string CPPServiceLangGen::GetDefaultInitializedValue(const TypeDefinition& tdef)
	{
		if (tdef.Type==DataTypes_void_t) throw InternalErrorException("Internal error");

		if (tdef.ContainerType == DataTypes_ContainerTypes_none)
		{
			if (IsTypeNumeric(tdef.Type))
			{
				switch (tdef.ArrayType)
				{
				case DataTypes_ArrayTypes_none:
				{
					if (tdef.Type == DataTypes_double_t || tdef.Type == DataTypes_single_t) return "0.0";
					if (tdef.Type >= DataTypes_int8_t && tdef.Type <= DataTypes_uint64_t) return "0";
					if (tdef.Type == DataTypes_cdouble_t) return "RobotRaconteur::cdouble(0.0,0.0)";
					if (tdef.Type == DataTypes_csingle_t) return "RobotRaconteur::cfloat(0.0,0.0)";
					if (tdef.Type == DataTypes_bool_t) return "RobotRaconteur::rr_bool(0)";
					throw InvalidArgumentException("Invalid numeric type");
				}
				case DataTypes_ArrayTypes_array:
				{
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
						return "RobotRaconteur::AllocateEmptyRRArray<" + t.cpp_type + ">(" + boost::lexical_cast<std::string>(tdef.ArrayLength.at(0)) + ")";
					}
				}
				case DataTypes_ArrayTypes_multidimarray:
				{
					RR_SHARED_PTR<TypeDefinition> tdef2 = tdef.Clone();
					tdef2->RemoveContainers();
					tdef2->RemoveArray();
					convert_type_result t = convert_type(*tdef2);
					if (tdef.ArrayVarLength)
					{
						return "RobotRaconteur::AllocateEmptyRRMultiDimArray<" + t.cpp_type + ">(boost::assign::list_of(1)(0))";
					}
					else
					{
						return "RobotRaconteur::AllocateEmptyRRMultiDimArray<" + t.cpp_type + ">(boost::assign::list_of(" + boost::join(tdef.ArrayLength | boost::adaptors::transformed(boost::lexical_cast<std::string, int32_t>), ")(") + "))";
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
					case DataTypes_ArrayTypes_none:
					{
						convert_type_result t = convert_type(*tdef2);
						return t.cpp_type + "()";
					}
					case DataTypes_ArrayTypes_array:
					{
						convert_type_result t = convert_type(*tdef2);
						if (tdef.ArrayVarLength)
						{
							return "RobotRaconteur::AllocateEmptyRRPodArray<" + t.cpp_type + ">(0)";
						}
						else
						{
							return "RobotRaconteur::AllocateEmptyRRPodArray<" + t.cpp_type + ">(" + boost::lexical_cast<std::string>(tdef.ArrayLength.at(0)) + ")";
						}
					}
					case DataTypes_ArrayTypes_multidimarray:
					{						
						convert_type_result t = convert_type(*tdef2);
						if (tdef.ArrayVarLength)
						{
							return "RobotRaconteur::AllocateEmptyRRPodMultiDimArray<" + t.cpp_type + ">(boost::assign::list_of(1)(0))";
						}
						else
						{
							return "RobotRaconteur::AllocateEmptyRRPodMultiDimArray<" + t.cpp_type + ">(boost::assign::list_of(" + boost::join(tdef.ArrayLength | boost::adaptors::transformed(boost::lexical_cast<std::string, int32_t>), ")(") + "))";
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


	void CPPServiceLangGen::GenerateMasterHeaderFile(std::vector<RR_SHARED_PTR<ServiceDefinition> > d, ostream* w)
	{
		ostream& w2 = *w;
		w2 << "//This file is automatically generated. DO NOT EDIT!" << endl << endl;
		BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition> e, d)
		{
			w2 << "#include \"" << boost::replace_all_copy(fix_name(e->Name), ".", "__") << "_stubskel.h\"" << endl;
		}
		w2 << endl;

		std::string type_list = "boost::assign::list_of<RR_SHARED_PTR<RobotRaconteur::ServiceFactory> >";
		BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition> e, d)
		{
			w2 << "#include \"" << boost::replace_all_copy(fix_name(e->Name), ".", "__") << "_stubskel.h\"" << endl;
		}

		BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition> e, d)
		{
			std::string n1 = boost::replace_all_copy(fix_name(e->Name), ".", "::");
			std::string n2 = boost::replace_all_copy(fix_name(e->Name), ".", "__");
			type_list += "(RR_MAKE_SHARED< ::" + n1 + "::" + n2 + "Factory>())";
		}
		
		w2 << "#define ROBOTRACONTEUR_SERVICE_TYPES " << type_list << endl;
		w2 << "#pragma once" << endl << endl;
	}

	void CPPServiceLangGen::GenerateDefaultImplHeader(ServiceDefinition* d, ostream* w)
	{
		ostream& w2 = *w;
	
		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin(); e != d->Objects.end(); ++e)
		{
			w2 << "class " << fix_name((*e)->Name) << "_default_impl : public virtual " << fix_name((*e)->Name) << endl;
			w2 << "{" << endl;
			w2 << "protected:";
			w2 << "boost::mutex this_lock;" << endl;
			MEMBER_ITER(PropertyDefinition)
				get_variable_type_result t = get_variable_type(*m->Type);
				w2 << t.cpp_type << " rrvar_" << fix_name(m->Name) << ";" << endl;
			MEMBER_ITER_END()
			MEMBER_ITER(EventDefinition)
				w2 << EventDeclaration(m.get(), true, true) << ";" << endl;
			MEMBER_ITER_END()
			MEMBER_ITER(PipeDefinition)
				if (m->Direction() == MemberDefinition_Direction_readonly)
				{
					get_variable_type_result t = get_variable_type(*m->Type);
					w2 << "RR_SHARED_PTR<RobotRaconteur::PipeBroadcaster<" << t.cpp_type << " > > rrvar_" << fix_name(m->Name) << ";" << endl;
				}
			MEMBER_ITER_END()
				MEMBER_ITER(CallbackDefinition)
				w2 << "RR_SHARED_PTR<RobotRaconteur::Callback<" << GetCallbackDeclaration(m.get(), true, true) << " > > rrvar_" << fix_name(m->Name) << ";" << endl;
			MEMBER_ITER_END()
			MEMBER_ITER(WireDefinition)
				if (m->Direction() == MemberDefinition_Direction_readonly)
				{
					get_variable_type_result t = get_variable_type(*m->Type);
					w2 << "RR_SHARED_PTR<RobotRaconteur::WireBroadcaster<" << t.cpp_type << " > > rrvar_" << m->Name << ";" << endl;
				}
				if (m->Direction() == MemberDefinition_Direction_writeonly)
				{
					get_variable_type_result t = get_variable_type(*m->Type);
					w2 << "RR_SHARED_PTR<RobotRaconteur::WireUnicastReceiver<" << t.cpp_type << " > > rrvar_" << m->Name << ";" << endl;
				}
			MEMBER_ITER_END()

			w2 << endl;
			w2 << "public:" << endl;

			w2 << fix_name((*e)->Name) << "_default_impl();" << endl;
			
			MEMBER_ITER(PropertyDefinition)
				if (m->Direction() != MemberDefinition_Direction_writeonly)
				{
					w2 << GetPropertyDeclaration(m.get(), true) << ";" << endl;
				}
				if (m->Direction() != MemberDefinition_Direction_readonly)
				{
					w2 << SetPropertyDeclaration(m.get(), true) << ";" << endl << endl;
				}
			MEMBER_ITER_END()

			MEMBER_ITER(FunctionDefinition)
				w2 << FunctionDeclaration(m.get(), true) << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(EventDefinition)
				w2 << EventDeclaration(m.get(), true) << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(ObjRefDefinition)
				w2 << ObjRefDeclaration(m.get(), true) << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(PipeDefinition)
				w2 << GetPipeDeclaration(m.get(), true) << ";" << endl;
				w2 << SetPipeDeclaration(m.get(), true) << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(CallbackDefinition)
				w2 << GetCallbackDeclaration(m.get(), true) << ";" << endl;
				w2 << SetCallbackDeclaration(m.get(), true) << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(WireDefinition)
				w2 << GetWireDeclaration(m.get(), true) << ";" << endl;
				w2 << SetWireDeclaration(m.get(), true) << ";" << endl << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(MemoryDefinition)
				w2 << MemoryDeclaration(m.get(), true) << ";" << endl << endl;
			MEMBER_ITER_END()

			w2 << "};" << endl << endl;
		}
	}

	void CPPServiceLangGen::GenerateDefaultImplDefinition(ServiceDefinition* d, ostream* w)
	{
		ostream& w2 = *w;

		for (std::vector<RR_SHARED_PTR<ServiceEntryDefinition> >::const_iterator e = d->Objects.begin(); e != d->Objects.end(); ++e)
		{
			w2 << fix_name((*e)->Name) << "_default_impl::" <<  fix_name((*e)->Name)  << "_default_impl()" << endl;
			w2 << "{" << endl;
			MEMBER_ITER(PropertyDefinition)
				w2 << "rrvar_" << fix_name(m->Name) << "=" << GetDefaultInitializedValue(*m->Type) << ";" << endl;
			MEMBER_ITER_END()
			w2 << "}" << endl;

			MEMBER_ITER(PropertyDefinition)
				if (m->Direction() != MemberDefinition_Direction_writeonly)
				{
					w2 << dforc(GetPropertyDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
					w2 << "{" << endl;
					w2 << "boost::mutex::scoped_lock lock(this_lock);" << endl;
					w2 << "return rrvar_" << fix_name(m->Name) << ";" << endl;
					w2 << "}" << endl;				
				}
				if (m->Direction() != MemberDefinition_Direction_readonly)
				{
					w2 << dforc(SetPropertyDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
					w2 << "{" << endl;
					w2 << "boost::mutex::scoped_lock lock(this_lock);" << endl;
					w2 << "rrvar_" << fix_name(m->Name) << " = value;" << endl;
					w2 << "}" << endl;
				}
			MEMBER_ITER_END()

			MEMBER_ITER(FunctionDefinition)
				w2 << dforc(FunctionDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
				w2 << "{" << endl;
				w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << endl;
				w2 << "}" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(EventDefinition)
				w2 << dforc(EventDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
				w2 << "{" << endl;
				w2 << "return rrvar_" << fix_name(m->Name) << ";" << endl;
				w2 << "}" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(ObjRefDefinition)
				w2 << dforc(ObjRefDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
				w2 << "{" << endl;
				w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << endl;
				w2 << "}" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(PipeDefinition)
				w2 << dforc(GetPipeDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
				w2 << "{" << endl;
				if (m->Direction() == MemberDefinition_Direction_readonly)
				{
					w2 << "boost::mutex::scoped_lock lock(this_lock);" << endl;
					w2 << "if (!rrvar_" << fix_name(m->Name) << ") throw RobotRaconteur::InvalidOperationException(\"Pipe not set\");" << endl;
					w2 << "return rrvar_" << fix_name(m->Name) << "->GetPipe();" << endl;
				}
				else
				{
					w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << endl;
				}
				w2 << "}" << endl;
				w2 << dforc(SetPipeDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
				w2 << "{" << endl;
				if (m->Direction() == MemberDefinition_Direction_readonly)
				{
					w2 << "boost::mutex::scoped_lock lock(this_lock);" << endl;
					get_variable_type_result t = get_variable_type(*m->Type);
					w2 << "if (rrvar_" << fix_name(m->Name) << ") throw RobotRaconteur::InvalidOperationException(\"Pipe already set\");" << endl;
					w2 << "rrvar_" << fix_name(m->Name) << " = RR_MAKE_SHARED<RobotRaconteur::PipeBroadcaster<" << t.cpp_type << "> >();" << endl;
					w2 << "rrvar_" << fix_name(m->Name) << "->Init(value);";
				}
				w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << endl;
				w2 << "}" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(CallbackDefinition)
				w2 << dforc(GetCallbackDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
				w2 << "{" << endl;
				w2 << "boost::mutex::scoped_lock lock(this_lock);" << endl;
				w2 << "return rrvar_" << fix_name(m->Name) << ";" << endl;
				w2 << "}" << endl;
				w2 << dforc(SetCallbackDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
				w2 << "{" << endl;
				w2 << "boost::mutex::scoped_lock lock(this_lock);" << endl;
				w2 << "if (rrvar_" << fix_name(m->Name) << ") throw RobotRaconteur::InvalidOperationException(\"Callback already set\");" << endl;
				w2 << "rrvar_" << fix_name(m->Name) << " = value;" << endl;
				w2 << "}" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(WireDefinition)
				w2 << dforc(GetWireDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
				w2 << "{" << endl;
				if (m->Direction() == MemberDefinition_Direction_readonly || m->Direction() == MemberDefinition_Direction_writeonly)
				{
					w2 << "boost::mutex::scoped_lock lock(this_lock);" << endl;
					w2 << "if (!rrvar_" << fix_name(m->Name) << ") throw RobotRaconteur::InvalidOperationException(\"Wire not set\");" << endl;
					w2 << "return rrvar_" << fix_name(m->Name) << "->GetWire();" << endl;
				}
				else
				{
					w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << endl;
				}
				w2 << "}" << endl;
				w2 << dforc(SetWireDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
				w2 << "{" << endl;
				if (m->Direction() == MemberDefinition_Direction_readonly)
				{
					get_variable_type_result t = get_variable_type(*m->Type);
					w2 << "if (rrvar_" << fix_name(m->Name) << ") throw RobotRaconteur::InvalidOperationException(\"Wire already set\");" << endl;
					w2 << "rrvar_" << fix_name(m->Name) << " = RR_MAKE_SHARED<RobotRaconteur::WireBroadcaster<" << t.cpp_type << "> >();" << endl;
					w2 << "rrvar_" << fix_name(m->Name) << "->Init(value);";
				}
				else if (m->Direction() == MemberDefinition_Direction_writeonly)
				{
					get_variable_type_result t = get_variable_type(*m->Type);
					w2 << "if (rrvar_" << fix_name(m->Name) << ") throw RobotRaconteur::InvalidOperationException(\"Wire already set\");" << endl;
					w2 << "rrvar_" << fix_name(m->Name) << " = RR_MAKE_SHARED<RobotRaconteur::WireUnicastReceiver<" << t.cpp_type << "> >();" << endl;
					w2 << "rrvar_" << fix_name(m->Name) << "->Init(value);";
				}
				else
				{
					w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << endl;
				}
				w2 << "}" << endl;
			MEMBER_ITER_END()

			MEMBER_ITER(MemoryDefinition)
				w2 << dforc(MemoryDeclaration(m.get(), true), fix_name((*e)->Name) + "_default_impl") << endl;
				w2 << "{" << endl;				
				w2 << "throw RobotRaconteur::NotImplementedException(\"\");" << endl;
				w2 << "}" << endl;
			MEMBER_ITER_END()

				
		}
	}

}
