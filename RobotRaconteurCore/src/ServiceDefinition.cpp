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

#ifdef ROBOTRACONTEUR_CORE_USE_STDAFX
#include "stdafx.h"
#endif

#include "RobotRaconteur/ServiceDefinition.h"
//#include "RobotRaconteur.h"

#include "RobotRaconteur/DataTypes.h"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/locale.hpp>
//#include "Error.h"
#include "RobotRaconteur/nulldeleter.h"
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/IOUtils.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>
#include <boost/range/combine.hpp>
#include <boost/range/numeric.hpp>
#include <boost/tuple/tuple_comparison.hpp>

using namespace boost::algorithm;

namespace RobotRaconteur
{

	RobotRaconteurVersion::RobotRaconteurVersion()
	{
		major=0;
		minor=0;
		patch=0;
		tweak=0;
	}

	RobotRaconteurVersion::RobotRaconteurVersion(uint32_t major, uint32_t minor, uint32_t patch, uint32_t tweak)
	{
		this->major = major;
		this->minor = minor;
		this->patch = patch;
		this->tweak = tweak;
	}

	RobotRaconteurVersion::RobotRaconteurVersion(const std::string& v)
	{
		FromString(v);
	}

	std::string RobotRaconteurVersion::ToString() const
	{
		std::stringstream o;
		if (patch == 0 && tweak == 0)
		{
			o << major << "." << minor;
		}
		else if (tweak == 0)
		{
			o << major << "." << minor << "." << patch;
		}
		else
		{
			o << major << "." << minor << "." << patch << "." << tweak;
		}

		return o.str();
	}
	void RobotRaconteurVersion::FromString(const std::string& v)
	{
		boost::regex r("^(\\d+)\\.(\\d+)(?:\\.(\\d+)(?:\\.(\\d+))?)?$");

		boost::smatch r_match;
		if (!boost::regex_match(v, r_match, r))
		{
			throw RobotRaconteurParseException("Format error for version definition \"" + v + "\"");
		}

		major = boost::lexical_cast<uint32_t>(r_match[1].str());
		minor = boost::lexical_cast<uint32_t>(r_match[2].str());		
		patch = r_match[3].matched ? boost::lexical_cast<uint32_t>(r_match[3].str()) : 0;
		tweak = r_match[4].matched ? boost::lexical_cast<uint32_t>(r_match[4].str()) : 0;
		
	}

	bool operator == (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2)
	{
		return boost::tie(v1.major, v1.minor, v1.patch, v1.tweak) == boost::tie(v2.major, v2.minor, v2.patch, v2.tweak);
	}
	bool operator != (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2)
	{
		return boost::tie(v1.major, v1.minor, v1.patch, v1.tweak) != boost::tie(v2.major, v2.minor, v2.patch, v2.tweak);
	}
	bool operator > (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2)
	{
		if (v1.major > v2.major) return true;
		if (v1.minor > v2.minor) return true;
		if (v1.patch > v2.patch) return true;
		if (v1.tweak > v2.tweak) return true;
		return false;
	}
	bool operator >= (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2)
	{
		if (boost::tie(v1.major, v1.minor, v1.patch, v1.tweak) == boost::tie(v2.major, v2.minor, v2.patch, v2.tweak))
			return true;
		return v1 > v2;
	}
	bool operator < (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2)
	{
		if (v1.major < v2.major) return true;
		if (v1.minor < v2.minor) return true;
		if (v1.patch < v2.patch) return true;
		if (v1.tweak < v2.tweak) return true;
		return false;
	}
	bool operator <= (const RobotRaconteurVersion &v1, const RobotRaconteurVersion &v2)
	{
		if (boost::tie(v1.major, v1.minor, v1.patch, v1.tweak) == boost::tie(v2.major, v2.minor, v2.patch, v2.tweak))
			return true;
		return v1 < v2;
	}

	RobotRaconteurVersion::operator bool() const
	{
		return *this != RobotRaconteurVersion();
	}

#define RR_NAME_REGEX "[a-zA-Z](?:\\w*[a-zA-Z0-9])?"
#define RR_TYPE_REGEX "(?:[a-zA-Z](?:\\w*[a-zA-Z0-9])?)(?:\\.[a-zA-Z](?:\\w*[a-zA-Z0-9])?)*"
#define RR_QUAIFIED_TYPE_REGEX "(?:[a-zA-Z](?:\\w*[a-zA-Z0-9])?)(?:\\.[a-zA-Z](?:\\w*[a-zA-Z0-9])?)+"
#define RR_TYPE2_REGEX "(?:[a-zA-Z](?:\\w*[a-zA-Z0-9])?)(?:\\.[a-zA-Z](?:\\w*[a-zA-Z0-9])?)*(?:\\[[0-9\\,\\*\\-]*\\])?(?:\\{\\w{1,16}\\})?"
#define RR_INT_REGEX "[+\\-]?\\d+"
#define RR_FLOAT_REGEX "[+\\-]?(?:(?:0|[1-9]\\d*)(?:\\.\\d*)?|:\\.\\d+)(?:[eE][+\\-]?\\d+)?"

	static void ServiceDefinition_FromStringFormat_common(const boost::regex& r, const std::string& l, const std::string& keyword, std::vector<std::string>& vec)
	{		
		boost::smatch r_match;
		if (!boost::regex_match(l, r_match, r))
		{
			throw RobotRaconteurParseException("Format error for " + keyword + " definition \"" + l + "\"");
		}

		if (r_match[1] != keyword)
		{
			throw RobotRaconteurParseException("Format error for " + keyword + " definition \"" + l + "\"");
		}
		vec.push_back(r_match[2]);
	}

	static void ServiceDefinition_FromStringImportFormat(const std::string& l, const std::string& keyword, std::vector<std::string>& vec)
	{
		boost::regex r("^[ \\t]*(\\w{1,16})[ \\t]+(" RR_TYPE_REGEX ")[ \\t]*$");
		ServiceDefinition_FromStringFormat_common(r, l, keyword, vec);
	}

	static void ServiceDefinition_FromStringTypeFormat(const std::string& l, const std::string& keyword, std::vector<std::string>& vec)
	{
		boost::regex r("^[ \\t]*(\\w{1,16})[ \\t]+(" RR_TYPE_REGEX ")[ \\t]*$");
		ServiceDefinition_FromStringFormat_common(r, l, keyword, vec);
	}
	
	template <typename T>
	static bool ServiceDefinition_GetLine(T& is, std::string& l, size_t& pos)
	{
		boost::regex r_comment("^[ \\t]*#[ -~\\t]*$");
		boost::regex r_empty("^[ \\t]*$");
		boost::regex r_valid("^[ -~\\t]*$");

		std::string l2;

		while (true)
		{
			if (!std::getline(is, l2))
				return false;
			pos++;

			boost::trim_right_if(l2, boost::is_any_of("\r"));

			if (l2.find('\0') != std::string::npos)
				throw RobotRaconteurParseException("Service definition must not contain null characters");

			if (boost::regex_match(l2, r_comment))
			{
				continue;
			}

			if (boost::regex_match(l2, r_empty))
			{
				continue;
			}

			while (boost::ends_with(l2, "\\"))
			{
				if (l2.size() <= 0) throw InternalErrorException("Internal parsing error");
				*l2.rbegin() = ' ';
				std::string l3;
				if (!std::getline(is, l3))
					throw RobotRaconteurParseException("Service definition line continuation must not be on last line");
				boost::trim_right_if(l3, boost::is_any_of("\r"));

				if (l3.find('\0') != std::string::npos)
					throw RobotRaconteurParseException("Service definition must not contain null characters");
				l2 += l3;
			}

			if (!boost::regex_match(l2, r_valid))
			{
				throw RobotRaconteurParseException("Service definition must contain only ASCII characters");
			}

			l.swap(l2);

			return true;
		}
	}

	static void ServiceDefinition_FindBlock(const std::string& current_line, std::istream& is, std::ostream& os, size_t& pos, size_t& init_pos)
	{
		boost::regex r_start("^[ \\t]*(\\w{1,16})[ \\t]+(" RR_NAME_REGEX ")[ \\t]*$");
		boost::regex r_end("^[ \\t]*end(?:[ \\t]+(\\w{1,16}))?[ \\t]*$");

		init_pos = pos;

		boost::smatch r_start_match;
		if (!boost::regex_match(current_line, r_start_match, r_start))
		{
			throw RobotRaconteurParseException("Parse error near: " + current_line, boost::numeric_cast<int32_t>(pos));
		}

		os << current_line << "\n";

		std::string block_type = r_start_match[1];
		std::string l;

		size_t last_pos = pos;

		while (ServiceDefinition_GetLine(is, l,pos))
		{	
			last_pos++;
			for (; last_pos < pos; last_pos++)
			{
				os << "\n";
			}

			os << l << "\n";

			boost::smatch r_end_match;
			if (boost::regex_match(l, r_end_match, r_end))
			{
				if (r_end_match[1].matched)
				{
					if (r_end_match[1] != block_type)
					{
						throw RobotRaconteurParseException("Block end does not match start: " + l, boost::numeric_cast<int32_t>(pos));
					}
				}
				
				return;
			}
		}

		throw RobotRaconteurParseException("Block end not found: " + current_line, boost::numeric_cast<int32_t>(init_pos));

	}

	std::string ServiceDefinition::ToString()
	{
		std::ostringstream o;
		ToStream(o);
		return o.str();
	}

	void ServiceDefinition::ToStream(std::ostream& o)
	{		
		o << "service " << Name << "\n\n";
			
		if (StdVer)
		{			
			bool version_found = false;
			BOOST_FOREACH(std::string& so, Options)
			{
				boost::regex r_version("^[ \\t]*version[ \\t]+(?:(\\d+(?:\\.\\d+)*)|[ -~\\t]*)$");
				boost::smatch r_version_match;
				if (boost::regex_match(so, r_version_match, r_version))
				{
					if (version_found) throw RobotRaconteurParseException("Robot Raconteur version already specified");
					if (r_version_match[1].matched)
					{
						version_found = true;
						break;
					}
					else
					{
						throw RobotRaconteurParseException("Invalid Robot Raconteur version specified");
					}					
				}
			}
			if (!version_found)
			{
				if (StdVer < RobotRaconteurVersion(0, 9))
				{
					o << "option version " << StdVer.ToString() << "\n";
					o << "\n";
				}
				else
				{
					o << "stdver " << StdVer.ToString() << "\n";
					o << "\n";
				}
			}
		}

		BOOST_FOREACH(std::string& import, Imports)
		{
			o << "import " << import << "\n";
		}

		if (!Imports.empty()) o << "\n";

		BOOST_FOREACH(RR_SHARED_PTR<UsingDefinition>& u, Using)
		{
			o << u->ToString();
		}

		if (!Using.empty())	o << "\n";

		BOOST_FOREACH(std::string& option, Options)
		{
			o << "option " << option << "\n"; //replace_all_copy((*option),"\"", "\"\"") + "\n";
		}

		if (!Options.empty()) o << "\n";
				
		BOOST_FOREACH(RR_SHARED_PTR<ConstantDefinition>& c, Constants)
		{
			o << c->ToString() << "\n";
		}

		if (!Constants.empty()) o << "\n";

		BOOST_FOREACH(RR_SHARED_PTR<EnumDefinition>& e, Enums)
		{
			o << e->ToString() << "\n";
		}

		if (!Enums.empty()) o << "\n";
		
		BOOST_FOREACH (std::string& exception, Exceptions)
		{
			o << "exception " << exception << "\n"; //replace_all_copy((*option),"\"", "\"\"") + "\n";
		}

		if (!Exceptions.empty()) o << "\n";

		BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& d, Structures)
		{
			o << d->ToString() << "\n";
		}

		BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition>& d, Pods)
		{
			o << d->ToString() << "\n";
		}

		BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition>& d, NamedArrays)
		{
			o << d->ToString() << "\n";
		}

		BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& d, Objects)
		{
			o << d->ToString() << "\n";
		}
	}
		
	void ServiceDefinition::CheckVersion(RobotRaconteurVersion ver)
	{
		if (!StdVer) return;

		if (ver == RobotRaconteurVersion(0, 0))
		{
			ver = RobotRaconteurVersion(ROBOTRACONTEUR_VERSION_TEXT);
		}

		if (ver < StdVer)
		{
			throw ServiceException("Service " + Name + " requires newer version of Robot Raconteur");
		}
		
	}


	void ServiceDefinition::FromString(const std::string &s)
	{
		std::vector<RobotRaconteurParseException> w;
		FromString(s, w);
	}

	void ServiceDefinition::FromStream(std::istream& is)
	{
		std::vector<RobotRaconteurParseException> w;
		FromStream(is, w);
	}

		
	void ServiceDefinition::FromString(const std::string &s1, std::vector<RobotRaconteurParseException>& warnings)
	{
		std::istringstream s(s1);
		FromStream(s, warnings);
	}

	//TODO: Test ignore_invalid_member
	void ServiceDefinition::FromStream(std::istream& s, std::vector<RobotRaconteurParseException>& warnings)
	{

		Reset();
		
		boost::regex r_comment("^[ \\t]*#[ -~\\t]*$");
		boost::regex r_empty("^[ \\t]*$");
		boost::regex r_entry("(?:^[ \\t]*(?:(service)|(stdver)|(option)|(import)|(using)|(exception)|(constant)|(enum)|(struct)|(object)|(pod)|(namedarray))[ \\t]+(\\w[^\\s]*(?:[ \\t]+[^\\s]+)*)[ \\t]*$)|(^[ \\t]*$)");

		bool service_name_found = false;
		
		size_t pos = 0;
		std::string l;

		RobotRaconteurVersion stdver_version;
		bool stdver_found = false;

		int32_t entry_key_max = 0;

		try
		{			
			while (true)
			{
								
				if (!ServiceDefinition_GetLine(s, l, pos))
				{
					break;
				}
								
				boost::smatch r_entry_match;
				if (!boost::regex_match(l, r_entry_match, r_entry))
				{
					throw RobotRaconteurParseException("Parse error near: " + l, boost::numeric_cast<int32_t>(pos));
				}

				const boost::smatch::value_type& r_entry_match_blank = r_entry_match[14];
				if (r_entry_match_blank.matched) continue;

				int32_t entry_key = 1;
				for (; entry_key < 12; entry_key++)
				{
					if (r_entry_match[entry_key].matched)
						break;
				}

				const boost::smatch::value_type& r_entry_match_remaining = r_entry_match[13];

				if (entry_key != 1 && !service_name_found)
					throw RobotRaconteurParseException("service name must be first entry in service definition");

				switch (entry_key)
				{
					//service name
				case 1:
				{
					if (entry_key_max >= 1)
						throw RobotRaconteurParseException("service name must be first entry in service definition");
					if (service_name_found)
						throw RobotRaconteurParseException("service name already specified");
					std::vector<std::string> tmp_name;
					ServiceDefinition_FromStringTypeFormat(l, "service", tmp_name);
					Name = tmp_name.at(0);
					entry_key_max = 1;
					service_name_found = true;
					continue;
				}
				//stdver
				case 2:
				{
					if (entry_key_max >= 2)
						throw RobotRaconteurParseException("service name must be first after service name");
					stdver_version.FromString(r_entry_match_remaining);
					stdver_found = true;
					if (stdver_version < RobotRaconteurVersion(0, 9))
					{
						throw ServiceDefinitionException("Service definition standard version 0.9 or greater required for \"stdver\" keyword");
					}
					continue;
				}
				//option
				case 3:
				{
					Options.push_back(r_entry_match_remaining);
					continue;
				}
				//import
				case 4:
				{
					if (entry_key_max > 4) throw RobotRaconteurParseException("import must be before all but options");
					ServiceDefinition_FromStringImportFormat(l, "import", Imports);
					entry_key_max = 4;
					continue;
				}
				//using
				case 5:
				{
					if (entry_key_max > 5) throw RobotRaconteurParseException("using must be after imports and before all others except options");
					RR_SHARED_PTR<UsingDefinition> using_def = RR_MAKE_SHARED<UsingDefinition>(shared_from_this());
					using_def->FromString(l);
					Using.push_back(using_def);
					entry_key_max = 5;
					continue;
				}
				//exception
				case 6:
				{
					if (entry_key_max >= 9) throw RobotRaconteurParseException("exception must be before struct and object");
					ServiceDefinition_FromStringTypeFormat(l, "exception", Exceptions);
					entry_key_max = 6;
					continue;
				}
				//constant
				case 7:
				{
					if (entry_key_max >= 9) throw RobotRaconteurParseException("exception must be before struct and object");
					RR_SHARED_PTR<ConstantDefinition> constant_def = RR_MAKE_SHARED<ConstantDefinition>(shared_from_this());
					constant_def->FromString(l);
					Constants.push_back(constant_def);
					entry_key_max = 7;
					continue;
				}
				//enum
				case 8:
				{
					if (entry_key_max >= 9) throw RobotRaconteurParseException("enum must be before struct and object");
					size_t init_pos;
					std::stringstream block;
					ServiceDefinition_FindBlock(l, s, block, pos, init_pos);
					RR_SHARED_PTR<EnumDefinition> enum_def = RR_MAKE_SHARED<EnumDefinition>(shared_from_this());
					enum_def->FromString(block.str(), init_pos);
					Enums.push_back(enum_def);
					entry_key_max = 8;
					continue;
				}
				//struct
				case 9:
				{
					size_t init_pos;
					std::stringstream block;
					ServiceDefinition_FindBlock(l, s, block, pos, init_pos);
					RR_SHARED_PTR<ServiceEntryDefinition> struct_def = RR_MAKE_SHARED<ServiceEntryDefinition>(shared_from_this());
					struct_def->FromString(block.str(), init_pos, warnings);
					Structures.push_back(struct_def);
					entry_key_max = 9;
					continue;
				}
				//object
				case 10:
				{
					size_t init_pos;
					std::stringstream block;
					ServiceDefinition_FindBlock(l, s, block, pos, init_pos);
					RR_SHARED_PTR<ServiceEntryDefinition> object_def = RR_MAKE_SHARED<ServiceEntryDefinition>(shared_from_this());
					object_def->FromString(block.str(), init_pos, warnings);
					Objects.push_back(object_def);
					entry_key_max = 10;
					continue;
				}
				//pod
				case 11:
				{
					size_t init_pos;
					std::stringstream block;
					ServiceDefinition_FindBlock(l, s, block, pos, init_pos);
					RR_SHARED_PTR<ServiceEntryDefinition> struct_def = RR_MAKE_SHARED<ServiceEntryDefinition>(shared_from_this());
					struct_def->FromString(block.str(), init_pos, warnings);
					Pods.push_back(struct_def);
					entry_key_max = 9;
					continue;
				}
				//namedarray
				case 12:
				{
					size_t init_pos;
					std::stringstream block;
					ServiceDefinition_FindBlock(l, s, block, pos, init_pos);
					RR_SHARED_PTR<ServiceEntryDefinition> struct_def = RR_MAKE_SHARED<ServiceEntryDefinition>(shared_from_this());
					struct_def->FromString(block.str(), init_pos, warnings);
					NamedArrays.push_back(struct_def);
					entry_key_max = 9;
					continue;
				}
				default:
					throw RobotRaconteurParseException("Parse error near: " + l, boost::numeric_cast<int32_t>(pos));
				}
			}

			bool version_found = false;
			if (stdver_found)
			{
				StdVer = stdver_version;
				version_found = true;
			}
			BOOST_FOREACH(std::string& so, Options)
			{
				boost::regex r_version("^[ \\t]*version[ \\t]+(?:(\\d+(?:\\.\\d+)*)|[ -~\\t]*)$");
				boost::smatch r_version_match;
				if (boost::regex_match(so, r_version_match, r_version))
				{
					if (version_found) throw RobotRaconteurParseException("Robot Raconteur version already specified");
					if (r_version_match[1].matched)
					{
						StdVer = RobotRaconteurVersion(r_version_match[1]);
					}
					else
					{
						throw RobotRaconteurParseException("Invalid Robot Raconteur version specified");
					}
					version_found = true;
				}
			}

		}
		catch (std::exception &e)
		{
			if (dynamic_cast<RobotRaconteurParseException*>(&e) != 0)
				throw e;
			throw RobotRaconteurParseException("Parse error near: " + l, boost::numeric_cast<int32_t>(pos));
		}
	}

	void ServiceDefinition::Reset()
	{
		Structures.clear();
		Objects.clear();
		Options.clear();
		Imports.clear();
		Using.clear();
		Exceptions.clear();
		Constants.clear();
		Enums.clear();
		Pods.clear();
		NamedArrays.clear();
	}

	ServiceDefinition::ServiceDefinition()
	{
		Structures.clear();
		Objects.clear() ;
		Options = std::vector<std::string>();
		Imports = std::vector<std::string>();	
	}

	ServiceEntryDefinition::ServiceEntryDefinition(RR_SHARED_PTR<ServiceDefinition> def)
	{
		Members.clear();
		EntryType = DataTypes_structure_t;
		Implements = std::vector<std::string>();
		Options = std::vector<std::string>();
		ServiceDefinition_ = def;
	}

	std::string ServiceEntryDefinition::ToString()
	{
		std::stringstream o;
		ToStream(o);
		return o.str();
	}

	static std::string ServiceEntryDefinition_UnqualifyTypeWithUsing(ServiceEntryDefinition& e, const std::string s)
	{
		if (!boost::contains(s, "."))
		{
			return s;
		}

		RR_SHARED_PTR<ServiceDefinition> d = e.ServiceDefinition_.lock();
		if (!d)
			return s;

		BOOST_FOREACH(const RR_SHARED_PTR<UsingDefinition>& u, d->Using)
		{
			if (u->QualifiedName == s)
			{
				return u->UnqualifiedName;
			}
		}
		return s;
	}

	void ServiceEntryDefinition::ToStream(std::ostream& o)
	{		

		switch (EntryType)
		{
		case DataTypes_structure_t:
			o << "struct " << Name << "\n";
			break;
		case DataTypes_pod_t:
			o << "pod " << Name << "\n";
			break;
		case DataTypes_namedarray_t:
			o << "namedarray " << Name << "\n";
			break;
		case DataTypes_object_t:
			o << "object " << Name << "\n";
			break;
		default:
			throw ServiceDefinitionException("Invalid ServiceEntryDefinition type in " + Name);
		}		

		BOOST_FOREACH (std::string& imp, Implements)
		{
			o << "    implements " << ServiceEntryDefinition_UnqualifyTypeWithUsing(*this,imp) << "\n";

		}

		BOOST_FOREACH (std::string& option, Options)
		{
			o << "    option " << option << "\n";
		}

		BOOST_FOREACH(RR_SHARED_PTR<ConstantDefinition>& constant, Constants)
		{
			o << "    " << constant->ToString() << "\n";
		}

		BOOST_FOREACH (RR_SHARED_PTR<MemberDefinition>& d, Members)
		{
			std::string d1 = d->ToString();
			if (EntryType != DataTypes_object_t)
			{
				boost::replace_first(d1, "property", "field");
			}
			o << "    " << d1 << "\n";
		}

		o << "end\n";

		/*switch (EntryType)
		{
		case DataTypes_structure_t:
			o << "end struct\n";
			break;
		case DataTypes_pod_t:
			o << "end pod\n";
			break;
		case DataTypes_namedarray_t:
			o << "end namedarray\n";
			break;
		case DataTypes_object_t:
			o << "end object\n";
			break;
		default:
			throw ServiceDefinitionException("Invalid ServiceEntryDefinition type in " + Name);
		}*/
	}

	void ServiceEntryDefinition::FromString(const std::string &s)
	{
		FromString(s, 0);
	}

	void ServiceEntryDefinition::FromString(const std::string &s, size_t startline)
	{
		std::vector<RobotRaconteurParseException> w;
		FromString(s, startline, w);
	}

	template<typename member_type>
	static void ServiceEntryDefinition_FromString_DoMember(const std::string& l, RR_SHARED_PTR<ServiceEntryDefinition> entry)
	{
		RR_SHARED_PTR<member_type> member = RR_MAKE_SHARED<member_type>(entry);
		member->FromString(l);
		entry->Members.push_back(member);
	}

	void ServiceEntryDefinition::FromString(const std::string &s, size_t startline, std::vector<RobotRaconteurParseException>& warnings)
	{
		std::istringstream os(s);
		FromStream(os, startline, warnings);
	}

	void ServiceEntryDefinition::FromStream(std::istream &s)
	{
		FromStream(s, 0);
	}

	void ServiceEntryDefinition::FromStream(std::istream &s, size_t startline)
	{
		std::vector<RobotRaconteurParseException> w;
		FromStream(s, startline, w);
	}

	static std::string ServiceEntryDefinition_QualifyTypeWithUsing(ServiceEntryDefinition& e, const std::string s)
	{
		if (boost::contains(s, "."))
		{
			return s;
		}

		RR_SHARED_PTR<ServiceDefinition> d = e.ServiceDefinition_.lock();
		if (!d)
			return s;
		
		BOOST_FOREACH(const RR_SHARED_PTR<UsingDefinition>& u, d->Using)
		{
			if (u->UnqualifiedName == s)
			{
				return u->QualifiedName;				
			}
		}
		return s;		
	}

	void ServiceEntryDefinition::FromStream(std::istream &s, size_t startline, std::vector<RobotRaconteurParseException>& warnings)
	{
		Reset();

		boost::regex start_struct_regex("^[ \\t]*struct[ \\t]+(\\w+)[ \\t]*$");
		boost::regex start_pod_regex("^[ \\t]*pod[ \\t]+(\\w+)[ \\t]*$");
		boost::regex start_namedarray_regex("^[ \\t]*namedarray[ \\t]+(\\w+)[ \\t]*$");
		boost::regex start_object_regex("^[ \\t]*object[ \\t]+(\\w+)[ \\t]*$");
		boost::regex end_struct_regex("^[ \\t]*end[ \\t]+struct[ \\t]*$");
		boost::regex end_pod_regex("^[ \\t]*end[ \\t]+pod[ \\t]*$");
		boost::regex end_namedarray_regex("^[ \\t]*end[ \\t]+namedarray[ \\t]*$");
		boost::regex end_object_regex("^[ \\t]*end[ \\t]+object[ \\t]*$");

		size_t pos=startline-1;

		std::string l;
		if (!ServiceDefinition_GetLine(s, l, startline))
		{
			throw RobotRaconteurParseException("Invalid object member", boost::numeric_cast<int32_t>(startline));
		}

		boost::smatch start_struct_cmatch;
		boost::smatch start_pod_cmatch;
		boost::smatch start_namedarray_cmatch;
		boost::smatch start_object_cmatch;
		if (boost::regex_match(l, start_struct_cmatch, start_struct_regex))
		{
			EntryType = DataTypes_structure_t;
			Name = start_struct_cmatch[1];
		}
		else if (boost::regex_match(l, start_pod_cmatch, start_pod_regex))
		{
			EntryType = DataTypes_pod_t;
			Name = start_pod_cmatch[1];
		}
		else if (boost::regex_match(l, start_namedarray_cmatch, start_namedarray_regex))
		{
			EntryType = DataTypes_namedarray_t;
			Name = start_namedarray_cmatch[1];
		}
		else if (boost::regex_match(l, start_object_cmatch, start_object_regex))
		{
			EntryType = DataTypes_object_t;
			Name = start_object_cmatch[1];
		}
		else
		{
			throw RobotRaconteurParseException("Parse error", startline);
		}
			
		

		try
		{
		
			boost::regex r_member("(?:^[ \\t]*(?:(option)|(implements)|(constant)|(field)|(property)|(function)|(event)|(objref)|(pipe)|(callback)|(wire)|(memory)|(end))[ \\t]+(\\w[^\\s]*(?:[ \\t]+[^\\s]+)*)[ \\t]*$)|(^[ \\t]*$)");

			while (ServiceDefinition_GetLine(s,l,pos))
			{	
				
				try
				{					

					boost::smatch r_member_match;
					if (!boost::regex_match(l, r_member_match, r_member))
					{
						if (boost::trim_copy(l) == "end")
						{
							if (ServiceDefinition_GetLine(s, l, pos))
							{
								throw  RobotRaconteurParseException("Parse error", boost::numeric_cast<int32_t>(pos));
							}
							return;
						}
						throw RobotRaconteurParseException("Parse error near: " + l, boost::numeric_cast<int32_t>(pos));
					}

					const boost::smatch::value_type& r_member_match_blank = r_member_match[15];
					if (r_member_match_blank.matched) continue;

					int32_t member_key = 1;
					for (; member_key < 13; member_key++)
					{
						if (r_member_match[member_key].matched)
							break;
					}

					const boost::smatch::value_type& r_member_match_remaining = r_member_match[14];

					if ((EntryType != DataTypes_object_t) && (member_key >= 5 && member_key != 13))
					{
						throw RobotRaconteurParseException("Structures can only contain fields, constants, and options", boost::numeric_cast<int32_t>(pos));
					}

					switch (member_key)
					{
					//option
					case 1:
					{
						//TODO: look in to this
						//if (!Members.empty()) throw RobotRaconteurParseException("Structure option must come before members", boost::numeric_cast<int32_t>(pos));
						Options.push_back(r_member_match_remaining);
						warnings.push_back(RobotRaconteurParseException("option keyword is deprecated", pos));
						continue;
					}
					//implements
					case 2:
					{
						if (!Members.empty()) throw RobotRaconteurParseException("Structure implements must come before members", boost::numeric_cast<int32_t>(pos));
						if (EntryType != DataTypes_object_t) throw RobotRaconteurParseException("Structures can only contain fields, constants, and options", boost::numeric_cast<int32_t>(pos));
						std::vector<std::string> implements1;
						ServiceDefinition_FromStringTypeFormat(l, "implements", implements1);
						Implements.push_back(ServiceEntryDefinition_QualifyTypeWithUsing(*this, implements1.at(0)));
						continue;
					}
					//constant
					case 3:
					{				
						if (!Members.empty()) throw RobotRaconteurParseException("Structure constants must come before members", boost::numeric_cast<int32_t>(pos));
						RR_SHARED_PTR<ConstantDefinition> constant_def = RR_MAKE_SHARED<ConstantDefinition>(shared_from_this());
						constant_def->FromString(l);
						Constants.push_back(constant_def);						
						continue;
					}
					//field
					case 4:
					{
						if (EntryType == DataTypes_object_t) throw RobotRaconteurParseException("Objects cannot contain fields.  Use properties instead.", boost::numeric_cast<int32_t>(pos));
						ServiceEntryDefinition_FromString_DoMember<PropertyDefinition>(l,shared_from_this());						
						continue;
					}
					//property
					case 5:
					{
						ServiceEntryDefinition_FromString_DoMember<PropertyDefinition>(l, shared_from_this());
						continue;
					}
					//function
					case 6:
					{
						ServiceEntryDefinition_FromString_DoMember<FunctionDefinition>(l, shared_from_this());
						continue;
					}
					//event
					case 7:
					{
						ServiceEntryDefinition_FromString_DoMember<EventDefinition>(l, shared_from_this());
						continue;
					}
					//objref
					case 8:
					{
						ServiceEntryDefinition_FromString_DoMember<ObjRefDefinition>(l, shared_from_this());
						continue;
					}
					//pipe
					case 9:
					{
						ServiceEntryDefinition_FromString_DoMember<PipeDefinition>(l, shared_from_this());
						continue;
					}
					//callback
					case 10:
					{
						ServiceEntryDefinition_FromString_DoMember<CallbackDefinition>(l, shared_from_this());
						continue;
					}
					//wire
					case 11:
					{
						ServiceEntryDefinition_FromString_DoMember<WireDefinition>(l, shared_from_this());
						continue;
					}
					//memory
					case 12:
					{
						ServiceEntryDefinition_FromString_DoMember<MemoryDefinition>(l, shared_from_this());
						continue;
					}
					//end
					case 13:
					{
						if (EntryType == DataTypes_structure_t)
						{
							boost::smatch matches;
							if (!boost::regex_match(l, matches, end_struct_regex))
							{
								throw  RobotRaconteurParseException("Parse error", boost::numeric_cast<int32_t>(pos));
							}
						}
						else if (EntryType == DataTypes_pod_t)
						{
							boost::smatch matches;
							if (!boost::regex_match(l, matches, end_pod_regex))
							{
								throw  RobotRaconteurParseException("Parse error", boost::numeric_cast<int32_t>(pos));
							}
						}
						else if (EntryType == DataTypes_namedarray_t)
						{
							boost::smatch matches;
							if (!boost::regex_match(l, matches, end_namedarray_regex))
							{
								throw  RobotRaconteurParseException("Parse error", boost::numeric_cast<int32_t>(pos));
							}
						}
						else
						{
							boost::smatch matches;
							if (!boost::regex_match(l, matches, end_object_regex))
							{
								throw  RobotRaconteurParseException("Parse error", boost::numeric_cast<int32_t>(pos));
							}
						}

						if (ServiceDefinition_GetLine(s, l, pos))
						{
							throw  RobotRaconteurParseException("Parse error", boost::numeric_cast<int32_t>(pos));
						}
						return;
					}

					default:
						throw RobotRaconteurParseException("Parse error", boost::numeric_cast<int32_t>(pos));
						break;

					}				
				}
				catch (RobotRaconteurParseException&)
				{					
					throw;					
				}

			}
		}
		catch (std::exception& exp)
		{
			throw RobotRaconteurParseException("Parse error: " + std::string(exp.what()) + " near: " + l, boost::numeric_cast<int32_t>(pos));
		}

	}
		

	void ServiceEntryDefinition::Reset()
	{
		Name.clear();
		Members.clear();
		EntryType = DataTypes_structure_t;
		Implements.clear();
		Options.clear();
		Constants.clear();
	}

	DataTypes ServiceEntryDefinition::RRDataType()
	{
		switch (EntryType)
		{
		case DataTypes_structure_t:
		case DataTypes_pod_t:
		case DataTypes_namedarray_t:
		case DataTypes_object_t:
			break;
		default:
			throw ServiceDefinitionException("Invalid ServiceEntryDefinition type in " + Name);
		}

		return EntryType;
	}

	std::string ServiceEntryDefinition::ResolveQualifiedName()
	{
		RR_SHARED_PTR<ServiceDefinition> def = ServiceDefinition_.lock();
		if (!def) throw InvalidOperationException("Could not lock service definition to resolve named type");
		return def->Name + "." + Name;
	}

	//Parser helper functions
	class MemberDefiniton_ParseResults
	{
	public:
		std::string MemberType;
		std::string Name;
		boost::optional<std::string> DataType;
		boost::optional<std::vector<std::string> > Parameters;
		boost::optional<std::vector<std::string> > Modifiers;
	};

	template<typename T>
	static bool MemberDefinition_ParseCommaList(const boost::regex& r, const std::string& s, std::vector<T>& res)
	{
		boost::regex r_empty("^[ \\t]*$");
		if (boost::regex_match(s, r_empty))
		{
			return true;
		}
				
		typename boost::match_results<typename T::const_iterator> r_match;
		if (!boost::regex_match(s, r_match, r))
		{
			return false;
		}

		res.push_back(r_match[1]);
		if (r_match[2].matched)
		{
			if (!MemberDefinition_ParseCommaList(r, r_match[2], res))
			{
				return false;
			}
		}

		return true;
	}

	template<typename T>
	static bool MemberDefinition_ParseParameters(const std::string& s, std::vector<T>& res)
	{
		boost::regex r_params("^[ \\t]*(" RR_TYPE_REGEX "(?:\\[[0-9\\,\\*\\-]*\\])?(?:\\{\\w{1,16}\\})?[ \\t]+\\w+)(?:[ \\t]*,[ \\t]*([ -~\\t]*\\w[ -~\\t]*))?[ \\t]*$");
		
		return MemberDefinition_ParseCommaList(r_params, s, res);
	}
	
	template<typename T>
	static bool MemberDefinition_ParseModifiers(const std::string& s, std::vector<T>& res)
	{
		boost::regex r_modifier("^[ \\t]*(" RR_NAME_REGEX "(?:\\([\\w\\-\\., \\t\\\"\\{\\}\\:]*\\))?)(?:[ \\t]*,([ -~\\t]*))?$");
		
		return MemberDefinition_ParseCommaList(r_modifier, s, res);
	}

	static std::string MemberDefinition_ModifiersToString(const std::vector<std::string> modifiers)
	{
		if (modifiers.size() == 0)
			return "";

		return " [" + boost::join(modifiers, ",") + "]";
	}

	static bool MemberDefinition_ParseFormat_common(const std::string& s, MemberDefiniton_ParseResults& res)
	{
		boost::regex r("^[ \\t]*([a-zA-Z]+)[ \\t]+(?:([a-zA-Z][\\w\\{\\}\\[\\]\\*\\,\\-\\.]*)[ \\t]+)?(\\w+)(?:[ \\t]*(\\(([^)]*)\\)))?(?:[ \\t]+\\[([^\\]]*)\\])?[ \\t]*$");
		boost::smatch r_result;
		if (!boost::regex_match(s, r_result, r))
		{
			return false;
		}

		res.Modifiers.reset();
		res.Parameters.reset();

		const boost::smatch::value_type& member_type_result = r_result[1];
		const boost::smatch::value_type& data_type_result = r_result[2];
		const boost::smatch::value_type& name_result = r_result[3];
		const boost::smatch::value_type& params_present_result = r_result[4];
		const boost::smatch::value_type& params_result = r_result[5];
		const boost::smatch::value_type& modifiers_result = r_result[6];

		res.MemberType = member_type_result;
		if (data_type_result.matched)
		{
			res.DataType = data_type_result;
		}
		res.Name = name_result;

		if (params_present_result.matched)
		{
			res.Parameters = std::vector<std::string>();
			if (!MemberDefinition_ParseParameters(params_result, *res.Parameters))
			{
				return false;
			}
		}

		if (modifiers_result.matched)
		{
			res.Modifiers = std::vector<std::string>();
			if (!MemberDefinition_ParseModifiers(modifiers_result, *res.Modifiers))
			{
				return false;
			}

			if (res.Modifiers->empty())
			{
				return false;
			}
		}
		return true;
	}

	static void MemberDefinition_FromStringFormat_common(MemberDefiniton_ParseResults& parse_res, const std::string s1, const std::vector<std::string>& member_types, RR_SHARED_PTR<MemberDefinition> def)
	{
		
		if (!MemberDefinition_ParseFormat_common(s1, parse_res))
		{
			throw RobotRaconteurParseException("Could not parse " + member_types.at(0) + " definition \"" + s1 + "\"");
		}

		if (boost::range::find(member_types, parse_res.MemberType) == member_types.end())
		{
			throw RobotRaconteurParseException("Format Error");
		}

		def->Reset();

		def->Name = parse_res.Name;
	}

	static void MemberDefinition_FromStringFormat1(const std::string s1, const std::vector<std::string>& member_types, RR_SHARED_PTR<MemberDefinition> def, RR_SHARED_PTR<TypeDefinition>& type)
	{
		MemberDefiniton_ParseResults parse_res;
		MemberDefinition_FromStringFormat_common(parse_res, s1, member_types, def);

		if (!parse_res.DataType || parse_res.Parameters) throw RobotRaconteurParseException("Format error for " + member_types.at(0) + " definition \"" + s1 + "\"");
		type = RR_MAKE_SHARED<TypeDefinition>(def);
		type->FromString(*parse_res.DataType);
		type->Rename("value");
		type->QualifyTypeStringWithUsing();

		if (parse_res.Modifiers)
		{
			def->Modifiers = *parse_res.Modifiers;
		}
	}
	static void MemberDefinition_FromStringFormat1(const std::string s1, const std::string& member_type, RR_SHARED_PTR<MemberDefinition> def, RR_SHARED_PTR<TypeDefinition>& type)
	{
		std::vector<std::string> member_types;
		member_types.push_back(member_type);
		MemberDefinition_FromStringFormat1(s1, member_types, def, type);
	}

	static std::string MemberDefinition_ToStringFormat1(const std::string& member_type, const MemberDefinition& def, const TypeDefinition& data_type)
	{
		TypeDefinition t;
		data_type.CopyTo(t);
		t.Rename(def.Name);
		t.UnqualifyTypeStringWithUsing();
		
		return member_type + " " + t.ToString() + MemberDefinition_ModifiersToString(def.Modifiers);
	}

	static void MemberDefinition_ParamatersFromStrings(const std::vector<std::string>& s, std::vector<RR_SHARED_PTR<TypeDefinition> >& params, RR_SHARED_PTR<MemberDefinition> def)
	{
		BOOST_FOREACH(const std::string& s1, s)
		{
			RR_SHARED_PTR<TypeDefinition> tdef=RR_MAKE_SHARED<TypeDefinition>(def);
			tdef->FromString(s1);
			tdef->QualifyTypeStringWithUsing();
			params.push_back(tdef);			
		}
	}

	static std::string MemberDefinition_ParametersToString(const std::vector<RR_SHARED_PTR<TypeDefinition> >& params)
	{
		std::vector<std::string> params2;
		BOOST_FOREACH(RR_SHARED_PTR<TypeDefinition> p, params)
		{
			TypeDefinition p2;
			p->CopyTo(p2);
			p2.UnqualifyTypeStringWithUsing();
			params2.push_back(p2.ToString());
		}

		return boost::join(params2, ", ");
	}

	static void MemberDefinition_FromStringFormat2(const std::string s1, const std::string& member_type, RR_SHARED_PTR<MemberDefinition> def, RR_SHARED_PTR<TypeDefinition>& return_type, std::vector<RR_SHARED_PTR<TypeDefinition> >& params )
	{
		std::vector<std::string> member_types;
		member_types.push_back(member_type);

		MemberDefiniton_ParseResults parse_res;
		MemberDefinition_FromStringFormat_common(parse_res, s1, member_types, def);

		if (!parse_res.DataType || !parse_res.Parameters) throw RobotRaconteurParseException("Format error for " + member_types.at(0) + " definition \"" + s1 + "\"");
		return_type = RR_MAKE_SHARED<TypeDefinition>(def);
		return_type->FromString(*parse_res.DataType);
		return_type->Rename("");
		return_type->QualifyTypeStringWithUsing();

		MemberDefinition_ParamatersFromStrings(*parse_res.Parameters, params, def);

		if (parse_res.Modifiers)
		{
			def->Modifiers = *parse_res.Modifiers;
		}
	}
	
	static std::string MemberDefinition_ToStringFormat2(const std::string& member_type, const MemberDefinition& def, const TypeDefinition& return_type, std::vector<RR_SHARED_PTR<TypeDefinition> > params)
	{
		TypeDefinition t;
		return_type.CopyTo(t);
		t.Rename(def.Name);
		t.UnqualifyTypeStringWithUsing();

		return member_type + " " + t.ToString() + "(" + MemberDefinition_ParametersToString(params) + ")" + MemberDefinition_ModifiersToString(def.Modifiers);
	}

	static void MemberDefinition_FromStringFormat3(const std::string s1, const std::string& member_type, RR_SHARED_PTR<MemberDefinition> def, std::vector<RR_SHARED_PTR<TypeDefinition> >& params)
	{
		std::vector<std::string> member_types;
		member_types.push_back(member_type);

		MemberDefiniton_ParseResults parse_res;
		MemberDefinition_FromStringFormat_common(parse_res, s1, member_types, def);

		if (parse_res.DataType || !parse_res.Parameters) throw RobotRaconteurParseException("Format error for " + member_types.at(0) + " definition \"" + s1 + "\"");
		
		MemberDefinition_ParamatersFromStrings(*parse_res.Parameters, params, def);

		if (parse_res.Modifiers)
		{
			def->Modifiers = *parse_res.Modifiers;
		}
	}

	static std::string MemberDefinition_ToStringFormat3(const std::string& member_type, const MemberDefinition& def, std::vector<RR_SHARED_PTR<TypeDefinition> > params)
	{		
		return member_type + " " + def.Name + "(" + MemberDefinition_ParametersToString(params) + ")" + MemberDefinition_ModifiersToString(def.Modifiers);
	}

	static MemberDefinition_Direction MemberDefinition_GetDirection(const std::vector<std::string>& modifiers)
	{
		if (boost::range::find(modifiers, "readonly") != modifiers.end())
		{
			return MemberDefinition_Direction_readonly;
		}

		if (boost::range::find(modifiers, "writeonly") != modifiers.end())
		{
			return MemberDefinition_Direction_writeonly;
		}

		return MemberDefinition_Direction_both;
	}

	MemberDefinition::MemberDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry)
	{
		this->ServiceEntry = ServiceEntry;
	}

	MemberDefinition_NoLock MemberDefinition::NoLock()
	{
		if (boost::range::find(Modifiers, "nolock") != Modifiers.end())
		{
			return MemberDefinition_NoLock_all;
		}

		if (boost::range::find(Modifiers, "nolockread") != Modifiers.end())
		{
			return MemberDefinition_NoLock_read;
		}

		return MemberDefinition_NoLock_none;
	}

	void MemberDefinition::Reset()
	{
		Name.clear();
		//ServiceEntry.reset();
		Modifiers.clear();
	}

	PropertyDefinition::PropertyDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry) : MemberDefinition(ServiceEntry)
	{
	}

	std::string PropertyDefinition::ToString()
	{
		return ToString(false);
	}

	std::string PropertyDefinition::ToString(bool isstruct)
	{
		std::string member_type = isstruct ? "field" : "property";

		return MemberDefinition_ToStringFormat1(member_type, *this, *Type);
	}

	void PropertyDefinition::FromString(const std::string &s)
	{
		std::vector<std::string> member_types;
		member_types.push_back("property");
		member_types.push_back("field");
		MemberDefinition_FromStringFormat1(s, member_types, shared_from_this(), Type);
	}

	void PropertyDefinition::Reset()
	{
		MemberDefinition::Reset();
		Type.reset();		
	}

	MemberDefinition_Direction PropertyDefinition::Direction()
	{
		return MemberDefinition_GetDirection(Modifiers);
	}

	FunctionDefinition::FunctionDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry) : MemberDefinition(ServiceEntry)
	{
		Reset();
	}

	std::string FunctionDefinition::ToString()
	{
		return MemberDefinition_ToStringFormat2("function", *this, *ReturnType, Parameters);
	}

	void FunctionDefinition::FromString(const std::string &s)
	{
		MemberDefinition_FromStringFormat2(s, "function", shared_from_this(), ReturnType, Parameters);
	}	

	void FunctionDefinition::Reset()
	{
		MemberDefinition::Reset();
		Parameters.clear();
		ReturnType.reset();
	}

	bool FunctionDefinition::IsGenerator()
	{
		if (ReturnType->ContainerType == DataTypes_ContainerTypes_generator)
		{
			return true;
		}

		if (!Parameters.empty() && Parameters.back()->ContainerType == DataTypes_ContainerTypes_generator)
		{
			return true;
		}

		return false;
	}

	EventDefinition::EventDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry) : MemberDefinition(ServiceEntry)
	{
		Reset();
	}

	std::string EventDefinition::ToString()
	{
		return MemberDefinition_ToStringFormat3("event", *this, Parameters);
	}

	void EventDefinition::FromString(const std::string &s)
	{
		MemberDefinition_FromStringFormat3(s, "event", shared_from_this(), Parameters);
	}

	void EventDefinition::Reset()
	{
		MemberDefinition::Reset();
		Parameters.clear();
	}

	ObjRefDefinition::ObjRefDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry) : MemberDefinition(ServiceEntry)
	{
		Reset();
	}

	std::string ObjRefDefinition::ToString()
	{
		TypeDefinition t(shared_from_this());
		t.Name = Name;
		t.TypeString = ObjectType;
		t.Type = DataTypes_namedtype_t;

		switch (ArrayType)
		{
			case DataTypes_ArrayTypes_none:
			{
				switch (ContainerType)
				{
				case DataTypes_ContainerTypes_none:
					break;
				case DataTypes_ContainerTypes_map_int32:
				case DataTypes_ContainerTypes_map_string:
					t.ContainerType = ContainerType;
					break;
				default:
					throw ServiceDefinitionException("Invalid ObjRefDefinition for objref \"" + Name + "\"");
				}
				break;
			}
			case DataTypes_ArrayTypes_array:
			{
				if (ContainerType != DataTypes_ContainerTypes_none)
				{
					throw ServiceDefinitionException("Invalid ObjRefDefinition for objref \"" + Name + "\"");
				}
				t.ArrayType = ArrayType;
				t.ArrayVarLength = true;
				t.ArrayLength.push_back(0);
				break;
			}
			default:
				throw ServiceDefinitionException("Invalid ObjRefDefinition for objref \"" + Name + "\"");
		}
				
		return MemberDefinition_ToStringFormat1("objref", *this, t);
	}

	void ObjRefDefinition::FromString(const std::string &s)
	{
		RR_SHARED_PTR<TypeDefinition> t;
		MemberDefinition_FromStringFormat1(s, "objref", shared_from_this(), t);
		
		switch (t->ArrayType)
		{
		case DataTypes_ArrayTypes_none:
		{
			switch (t->ContainerType)
			{
			case DataTypes_ContainerTypes_none:
			case DataTypes_ContainerTypes_map_int32:
			case DataTypes_ContainerTypes_map_string:
				break;			
			default:				
				throw RobotRaconteurParseException("Invalid objref definition \"" + s + "\"");
			}
			break;
		}
		case DataTypes_ArrayTypes_array:
		{
			if (ContainerType != DataTypes_ContainerTypes_none)
			{
				throw RobotRaconteurParseException("Invalid objref definition \"" + s + "\"");
			}
			if (!t->ArrayVarLength)
			{
				throw RobotRaconteurParseException("Invalid objref definition \"" + s + "\"");
			}
			if (t->ArrayLength.at(0) != 0)
			{
				throw RobotRaconteurParseException("Invalid objref definition \"" + s + "\"");
			}
			break;		
		}
		default:
			throw RobotRaconteurParseException("Invalid objref definition \"" + s + "\"");
		}

		if (!((!t->TypeString.empty() && t->Type == DataTypes_namedtype_t)
			|| (t->Type == DataTypes_varobject_t)))
		{
			throw RobotRaconteurParseException("Invalid objref definition \"" + s + "\"");
		}
		

		if (t->Type == DataTypes_namedtype_t)
		{
			ObjectType = t->TypeString;
		}
		else
		{
			ObjectType = "varobject";
		}

		ArrayType = t->ArrayType;
		ContainerType = t->ContainerType;

	}

	void ObjRefDefinition::Reset()
	{
		MemberDefinition::Reset();
		ObjectType.clear();
		ArrayType = DataTypes_ArrayTypes_none;
		ContainerType = DataTypes_ContainerTypes_none;
	}

	PipeDefinition::PipeDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry) : MemberDefinition(ServiceEntry)
	{
		Reset();
	}

	std::string PipeDefinition::ToString()
	{
		return MemberDefinition_ToStringFormat1("pipe", *this, *Type);
	}

	void PipeDefinition::FromString(const std::string &s)
	{
		MemberDefinition_FromStringFormat1(s, "pipe", shared_from_this(), Type);
	}

	void PipeDefinition::Reset()
	{
		MemberDefinition::Reset();
		Type.reset();
	}

	MemberDefinition_Direction PipeDefinition::Direction()
	{
		return MemberDefinition_GetDirection(Modifiers);
	}

	bool PipeDefinition::IsUnreliable()
	{
		if (boost::range::find(Modifiers, "unreliable") != Modifiers.end())
		{
			return true;
		}

		RR_SHARED_PTR<ServiceEntryDefinition> obj = ServiceEntry.lock();
		if (obj)
		{
			BOOST_FOREACH(const std::string& o, obj->Options)
			{
				boost::regex r("^[ \\t]*pipe[ \\t]+" + Name + "[ \\t]+unreliable[ \\t]*$");
				if (boost::regex_match(o, r))
				{
					return true;
				}
			}
		}

		return false;
	}

	CallbackDefinition::CallbackDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry) : MemberDefinition(ServiceEntry)
	{
		Reset();
	}

	std::string CallbackDefinition::ToString()
	{
		return MemberDefinition_ToStringFormat2("callback", *this, *ReturnType, Parameters);
	}

	void CallbackDefinition::FromString(const std::string &s)
	{
		MemberDefinition_FromStringFormat2(s, "callback", shared_from_this(), ReturnType, Parameters);
	}

	void CallbackDefinition::Reset()
	{
		MemberDefinition::Reset();
		Parameters.clear();
		ReturnType.reset();
	}

	WireDefinition::WireDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry) : MemberDefinition(ServiceEntry)
	{
		Reset();
	}

	std::string WireDefinition::ToString()
	{
		return MemberDefinition_ToStringFormat1("wire", *this, *Type);

	}

	void WireDefinition::FromString(const std::string &s)
	{
		MemberDefinition_FromStringFormat1(s, "wire", shared_from_this(), Type);
	}

	void WireDefinition::Reset()
	{
		MemberDefinition::Reset();
		Type.reset();
	}

	MemberDefinition_Direction WireDefinition::Direction()
	{
		return MemberDefinition_GetDirection(Modifiers);
	}

	MemoryDefinition::MemoryDefinition(RR_SHARED_PTR<ServiceEntryDefinition> ServiceEntry) : MemberDefinition(ServiceEntry)
	{
		Reset();
	}

	std::string MemoryDefinition::ToString()
	{
		return MemberDefinition_ToStringFormat1("memory", *this, *Type);
	}

	void MemoryDefinition::FromString(const std::string &s)
	{
		MemberDefinition_FromStringFormat1(s, "memory", shared_from_this(), Type);
	}

	void MemoryDefinition::Reset()
	{
		MemberDefinition::Reset();
		Type.reset();
	}

	MemberDefinition_Direction MemoryDefinition::Direction()
	{
		return MemberDefinition_GetDirection(Modifiers);
	}

	TypeDefinition::TypeDefinition()
	{
		Reset();
		this->member.reset();
	}

	TypeDefinition::TypeDefinition(RR_SHARED_PTR<MemberDefinition> member)
	{
		Reset();
		this->member = member;
	}

	std::string TypeDefinition::ToString()
	{
		std::ostringstream o;
		o << ((Type >= DataTypes_namedtype_t && (!(Type == DataTypes_varvalue_t) && !(Type == DataTypes_varobject_t))) ? TypeString : StringFromDataType(Type));
		
		switch (ArrayType)
		{
		case DataTypes_ArrayTypes_none:
			break;
		case DataTypes_ArrayTypes_array:
		{
			o << "[" + std::string(ArrayLength.at(0) != 0 ? (boost::lexical_cast<std::string>(ArrayLength.at(0)) + (ArrayVarLength ? "-" : "")) : "") << "]";
			break;
		}
		case DataTypes_ArrayTypes_multidimarray:
		{
			if (ArrayVarLength)
			{
				o << "[*]";
			}
			else
			{
				if (ArrayLength.size() == 1)
				{
					o << "[" << boost::lexical_cast<std::string>(ArrayLength.at(0)) << ",]";
				}
				else
				{
					std::vector<std::string> s3;
					BOOST_FOREACH(int32_t& e, ArrayLength)
					{
						s3.push_back(boost::lexical_cast<std::string>(e));
					}
					o << "[" + boost::join(s3, ",") << "]";
				}

			}
			break;
		}
		default:
			throw ServiceDefinitionException("Invalid type definition \"" + Name + "\"");
		}

		switch (ContainerType)
		{
		case DataTypes_ContainerTypes_none:
			break;
		case DataTypes_ContainerTypes_list:
			o << "{list}";
			break;
		case DataTypes_ContainerTypes_map_int32:
			o << "{int32}";
			break;
		case DataTypes_ContainerTypes_map_string:
			o << "{string}";
			break;
		case DataTypes_ContainerTypes_generator:
			o << "{generator}";
			break;
		default:
			throw ServiceDefinitionException("Invalid type definition \"" + Name + "\"");
		}
				
		o << " " + Name;
		std::string o2 = o.str();
		trim(o2);
		return o2;
	}
		
	void TypeDefinition::FromString(const std::string &s)
	{
		Reset();

		boost::regex r("^[ \\t]*([a-zA-Z][\\w\\.]*)(?:(\\[\\])|\\[(([0-9]+)|([0-9]+)\\-|(\\*)|([0-9]+)\\,|([0-9\\,]+))\\])?(?:\\{(\\w{1,16})\\})?(?:[ \\t]+(\\w+))?[ \\t]*$");
		boost::smatch r_result;
		if (!boost::regex_match(s, r_result, r))
		{
			throw RobotRaconteurParseException("Could not parse type \"" + trim_copy(s) + "\"");
		}

		const boost::smatch::value_type& type_result = r_result[1];
		const boost::smatch::value_type& array_result = r_result[2];
		const boost::smatch::value_type& array_result2 = r_result[3];
		const boost::smatch::value_type& array_var_result = r_result[4];
		const boost::smatch::value_type& array_max_var_result = r_result[5];
		const boost::smatch::value_type& array_multi_result = r_result[6];
		const boost::smatch::value_type& array_multi_single_fixed_result = r_result[7];
		const boost::smatch::value_type& array_multi_fixed_result = r_result[8];
		const boost::smatch::value_type& container_result = r_result[9];
		const boost::smatch::value_type& name_result = r_result[10];
		
		Name = name_result.matched ? name_result.str() : "";
		
		if (container_result.matched)
		{
			if (container_result == "list")
			{
				ContainerType = DataTypes_ContainerTypes_list;
			}
			else if (container_result  == "int32")
			{
				ContainerType = DataTypes_ContainerTypes_map_int32;
			}
			else if (container_result == "string")
			{
				ContainerType = DataTypes_ContainerTypes_map_string;
			}
			else if (container_result == "generator")
			{
				ContainerType = DataTypes_ContainerTypes_generator;
			}
			else
			{
				throw RobotRaconteurParseException("Could not parse type \"" + trim_copy(s) + "\": invalid container type");
			}
		}
		
		if (array_result.matched)
		{
			//variable array
			ArrayType = DataTypes_ArrayTypes_array;
			ArrayVarLength = true;
			ArrayLength.resize(1);
			ArrayLength.at(0) = 0;
		}
		if (array_result2.matched)
		{			
			if (array_var_result.matched)
			{
				//Fixed array
				ArrayType = DataTypes_ArrayTypes_array;
				ArrayLength.resize(1);
				ArrayLength.at(0) = boost::lexical_cast<uint32_t>(array_var_result);
				ArrayVarLength = false;
			}
			else if (array_max_var_result.matched)
			{
				//variable array max sized
				ArrayType = DataTypes_ArrayTypes_array;
				ArrayLength.resize(1);
				ArrayLength.at(0) = boost::lexical_cast<uint32_t>(array_max_var_result);
				ArrayVarLength = true;
			}
			else if (array_multi_result.matched)
			{
				//multidim array
				ArrayType = DataTypes_ArrayTypes_multidimarray;
				ArrayVarLength = true;
			}
			else if (array_multi_single_fixed_result.matched)
			{
				//multidim single fixed array
				ArrayType = DataTypes_ArrayTypes_multidimarray;
				ArrayVarLength = false;
				ArrayLength.resize(1);
				ArrayLength.at(0) = boost::lexical_cast<uint32_t>(array_multi_single_fixed_result);
			}
			else if (array_multi_fixed_result.matched)
			{
				//multidim fixed array
				ArrayType = DataTypes_ArrayTypes_multidimarray;
				ArrayVarLength = false;
				std::vector<boost::iterator_range<std::string::const_iterator> > dims;
				boost::iterator_range<std::string::const_iterator> array_multi_fixed_result_rng(array_multi_fixed_result.first, array_multi_fixed_result.second);
				boost::split(dims, array_multi_fixed_result_rng, boost::is_any_of(","));
				ArrayLength.clear();
				BOOST_FOREACH(boost::iterator_range<std::string::const_iterator>& d, dims)
				{
					ArrayLength.push_back(boost::lexical_cast<uint32_t>(d));
				}
			}
			else
			{
				throw RobotRaconteurParseException("Could not parse type \"" + trim_copy(s) + "\": array error");
			}
		}

				
		DataTypes t = DataTypeFromString(type_result);
		if (t == DataTypes_namedtype_t)
		{
			Type = DataTypes_namedtype_t;
			TypeString = type_result.str();
		}
		else
		{
			Type = t;
			TypeString = "";
		}
	}

	DataTypes TypeDefinition::DataTypeFromString(const std::string &d)
	{
		if (d == "void")
		{
				return DataTypes_void_t;
		}
		else if (d == "double")
		{
				return DataTypes_double_t;
		}
		else if (d == "single")
		{
				return DataTypes_single_t;
		}
		else if (d == "int8")
		{
				return DataTypes_int8_t;
		}
		else if (d == "uint8")
		{
				return DataTypes_uint8_t;
		}
		else if (d == "int16")
		{
				return DataTypes_int16_t;
		}
		else if (d == "uint16")
		{
				return DataTypes_uint16_t;
		}
		else if (d == "int32")
		{
				return DataTypes_int32_t;
		}
		else if (d == "uint32")
		{
				return DataTypes_uint32_t;
		}
		else if (d == "int64")
		{
				return DataTypes_int64_t;
		}
		else if (d == "uint64")
		{
				return DataTypes_uint64_t;
		}
		else if (d == "string")
		{
				return DataTypes_string_t;
		}
		else if (d == "cdouble")
		{
			return DataTypes_cdouble_t;
		}
		else if (d == "csingle")
		{
			return DataTypes_csingle_t;
		}
		else if (d == "bool")
		{
			return DataTypes_bool_t;
		}		
		else if (d == "structure")
		{
				return DataTypes_structure_t;
		}
		else if (d == "object")
		{
				return DataTypes_object_t;
		}
		else if (d == "varvalue")
		{
				return DataTypes_varvalue_t;
		}
		else if (d == "varobject")
		{
				return DataTypes_varobject_t;

		}
		
		return DataTypes_namedtype_t;
	}

	std::string TypeDefinition::StringFromDataType(DataTypes d)
	{
		switch (d)
		{
			case DataTypes_void_t:
				return "void";
			case DataTypes_double_t:
				return "double";
			case DataTypes_single_t:
				return "single";
			case DataTypes_int8_t:
				return "int8";
			case DataTypes_uint8_t:
				return "uint8";
			case DataTypes_int16_t:
				return "int16";
			case DataTypes_uint16_t:
				return "uint16";
			case DataTypes_int32_t:
				return "int32";
			case DataTypes_uint32_t:
				return "uint32";
			case DataTypes_int64_t:
				return "int64";
			case DataTypes_uint64_t:
				return "uint64";
			case DataTypes_string_t:
				return "string";
			case DataTypes_cdouble_t:
				return "cdouble";
			case DataTypes_csingle_t:
				return "csingle";
			case DataTypes_bool_t:
				return "bool";			
			case DataTypes_structure_t:
				return "structure";
			case DataTypes_object_t:
				return "object";
			case DataTypes_varvalue_t:
				return "varvalue";
			case DataTypes_varobject_t:
				return "varobject";
			default:
				throw DataTypeException("Invalid data type");
		}

		return 0;
	}

	void TypeDefinition::Reset()
	{
		ArrayType = DataTypes_ArrayTypes_none;
		ContainerType = DataTypes_ContainerTypes_none;
		
		ArrayVarLength=false;
		Type=DataTypes_void_t;
		ArrayLength.clear();
		TypeString.clear();
	}

	void TypeDefinition::CopyTo(TypeDefinition& def) const
	{
		def.Name = Name;
		def.Type = Type;
		def.TypeString = TypeString;
		def.ArrayType = ArrayType;
		def.ContainerType = ContainerType;		
		def.ArrayLength = ArrayLength;		
		def.ArrayVarLength = ArrayVarLength;		
		def.member = member;
		def.ResolveNamedType_cache = ResolveNamedType_cache;
	}
	RR_SHARED_PTR<TypeDefinition> TypeDefinition::Clone() const
	{
		RR_SHARED_PTR<TypeDefinition> def2 = RR_MAKE_SHARED<TypeDefinition>();
		CopyTo(*def2);
		return def2;
	}

	void TypeDefinition::Rename(const std::string& name)
	{
		Name = name;
	}

	void TypeDefinition::RemoveContainers()
	{
		ContainerType = DataTypes_ContainerTypes_none;
	}

	void TypeDefinition::RemoveArray()
	{
		if (ContainerType != DataTypes_ContainerTypes_none) throw InvalidOperationException("Remove containers first");
		
		ArrayType = DataTypes_ArrayTypes_none;
		ArrayLength.clear();
		ArrayVarLength = false;
	}

	static const std::vector<RR_SHARED_PTR<UsingDefinition> > TypeDefinition_GetServiceUsingDefinition(TypeDefinition& def)
	{
		RR_SHARED_PTR<MemberDefinition> member1 =def.member.lock();
		if (!member1) throw InvalidOperationException("Member not set for TypeDefinition");
		RR_SHARED_PTR<ServiceEntryDefinition> entry1 = member1->ServiceEntry.lock();
		if (!entry1) throw InvalidOperationException("Object or struct not set for MemberDefinition " + member1->Name);
		RR_SHARED_PTR<ServiceDefinition> service1 = entry1->ServiceDefinition_.lock();
		if (!service1) throw InvalidOperationException("ServiceDefinition or struct not set for Object or Structure " + entry1->Name);
		return service1->Using;
	}

	void TypeDefinition::QualifyTypeStringWithUsing()
	{
		if (Type != DataTypes_namedtype_t) 
			return;

		if (boost::contains(TypeString, "."))
			return;

		const std::vector<RR_SHARED_PTR<UsingDefinition> > using_ = TypeDefinition_GetServiceUsingDefinition(*this);
		BOOST_FOREACH(const RR_SHARED_PTR<UsingDefinition>& u, using_)
		{
			if (u->UnqualifiedName == TypeString)
			{
				TypeString = u->QualifiedName;
				return;
			}
		}
	}

	void TypeDefinition::UnqualifyTypeStringWithUsing()
	{
		if (Type != DataTypes_namedtype_t)
			return;

		if (!boost::contains(TypeString, "."))
			return;

		const std::vector<RR_SHARED_PTR<UsingDefinition> > using_ = TypeDefinition_GetServiceUsingDefinition(*this);
		BOOST_FOREACH(const RR_SHARED_PTR<UsingDefinition>& u, using_)
		{
			if (u->QualifiedName == TypeString)
			{
				TypeString = u->UnqualifiedName;
				return;
			}
		}
	}

	RR_SHARED_PTR<NamedTypeDefinition> TypeDefinition::ResolveNamedType(std::vector<RR_SHARED_PTR<ServiceDefinition> > imported_defs, RR_SHARED_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<RRObject> client)
	{
		RR_SHARED_PTR<NamedTypeDefinition> o = ResolveNamedType_cache.lock();
		if (o)
		{
			return o;
		}

		o.reset();

		RR_SHARED_PTR<ServiceDefinition> def;

		std::string entry_name;

		if (!boost::contains(TypeString, "."))
		{
			entry_name = TypeString;
			//Assume not imported
			RR_SHARED_PTR<MemberDefinition> m = member.lock();
			if (m)
			{
				RR_SHARED_PTR<ServiceEntryDefinition> entry= m->ServiceEntry.lock();
				if (entry)
				{
					def = entry->ServiceDefinition_.lock();
				}
			}		
		}
		else
		{	
			std::string def_name;
			boost::tie(def_name, entry_name) = SplitQualifiedName(TypeString);

			def = TryFindByName(imported_defs, def_name);
			if (!def)
			{
				if (!node) throw InvalidArgumentException("Node not specified for ResolveType");
				try
				{
					if (client)
					{
						RR_SHARED_PTR<ServiceFactory> f = node->GetPulledServiceType(client, def_name);
						def = f->ServiceDef();
					}
					else
					{
						RR_SHARED_PTR<ServiceFactory> f = node->GetServiceType(def_name);
						def = f->ServiceDef();
					}
				}
				catch (std::exception&) {}
			}
		}
		
		if (!def)
		{
			throw ServiceDefinitionException("Could not resolve named type " + TypeString);
		}

		RR_SHARED_PTR<ServiceEntryDefinition> found_struct = TryFindByName(def->Structures, entry_name);
		if (found_struct)
		{
			ResolveNamedType_cache = found_struct;
			return found_struct;
		}
		RR_SHARED_PTR<ServiceEntryDefinition> found_pod = TryFindByName(def->Pods, entry_name);
		if (found_pod)
		{
			ResolveNamedType_cache = found_pod;
			return found_pod;
		}
		RR_SHARED_PTR<ServiceEntryDefinition> found_namedarray = TryFindByName(def->NamedArrays, entry_name);
		if (found_namedarray)
		{
			ResolveNamedType_cache = found_namedarray;
			return found_namedarray;
		}
		RR_SHARED_PTR<ServiceEntryDefinition> found_object = TryFindByName(def->Objects, entry_name);
		if (found_object)
		{
			ResolveNamedType_cache = found_object;
			return found_object;
		}
		RR_SHARED_PTR<EnumDefinition> found_enum = TryFindByName(def->Enums, entry_name);
		if (found_enum)
		{
			ResolveNamedType_cache = found_enum;
			return found_enum;
		}

		throw ServiceDefinitionException("Could not resolve named type " + def->Name + "." + entry_name);
	}

	UsingDefinition::~UsingDefinition()
	{

	}

	UsingDefinition::UsingDefinition(RR_SHARED_PTR<ServiceDefinition> service)
	{
		this->service = service;
	}

	std::string UsingDefinition::ToString()
	{
		std::string qualified_name_type;
		boost::tie(boost::tuples::ignore, qualified_name_type) = SplitQualifiedName(QualifiedName);

		if (qualified_name_type == UnqualifiedName)
		{
			return "using " + QualifiedName + "\n";
		}
		else
		{
			return "using " + QualifiedName + " as " + UnqualifiedName + "\n";
		}
	}
	void UsingDefinition::FromString(const std::string& s)
	{
		boost::regex r("^[ \\t]*using[ \\t]+(" RR_QUAIFIED_TYPE_REGEX ")(?:[ \\t]+as[ \\t](" RR_NAME_REGEX  "))?[ \\t]*$");

		boost::smatch r_match;
		if (!boost::regex_match(s, r_match, r))
		{
			throw RobotRaconteurParseException("Format error for using  definition \"" + s + "\"");
		}

		if (!r_match[2].matched)
		{
			this->QualifiedName = r_match[1];
			boost::tie(boost::tuples::ignore, this->UnqualifiedName) = SplitQualifiedName(r_match[1]);
		}
		else
		{
			this->QualifiedName = r_match[1];
			this->UnqualifiedName = r_match[2];
		}
	}

	ConstantDefinition::~ConstantDefinition()
	{

	}

	ConstantDefinition::ConstantDefinition(RR_SHARED_PTR<ServiceDefinition> service)
	{
		this->service = service;
	}
	ConstantDefinition::ConstantDefinition(RR_SHARED_PTR<ServiceEntryDefinition> service_entry)
	{
		this->service_entry = service_entry;
	}

	std::string ConstantDefinition::ToString()
	{
		return "constant " + Type->ToString() + " " + Name + " " + Value;
	}
	void ConstantDefinition::FromString(const std::string& s)
	{
		Reset();

		boost::regex r("^[ \\t]*constant[ \\t]+(" RR_TYPE2_REGEX ")[ \\t]+(" RR_NAME_REGEX ")[ \\t]+([^\\s](?:[ -~\\t]*[^\\s])?)[ \\t]*$");
		boost::smatch r_match;
		if (!boost::regex_match(s, r_match, r))
		{
			throw RobotRaconteurParseException("Invalid constant definition: " + boost::trim_copy(s));
		}

		std::string type_str = r_match[1];
		RR_SHARED_PTR<TypeDefinition> def = RR_MAKE_SHARED<TypeDefinition>();
		def->FromString(type_str);
		if (!VerifyTypeAndValue(*def, boost::make_iterator_range(r_match[3].first, r_match[3].second)))
		{
			throw RobotRaconteurParseException("Invalid constant definition: " + boost::trim_copy(s));
		}
		
		Type = def;
		Name = r_match[2];
		Value = r_match[3];
	}


	static bool ConstantDefinition_CheckScalar(DataTypes& t, boost::iterator_range<std::string::const_iterator> val)
	{
		try
		{
			switch (t)
			{			
			case DataTypes_double_t:
				boost::lexical_cast<double>(val);
				return true;
			case DataTypes_single_t:
				boost::lexical_cast<float>(val);
				return true;
			case DataTypes_int8_t:
				boost::lexical_cast<int8_t>(val);
				return true;
			case DataTypes_uint8_t:
				boost::lexical_cast<uint8_t>(val);
				return true;
			case DataTypes_int16_t:
				boost::lexical_cast<int16_t>(val);
				return true;
			case DataTypes_uint16_t:
				boost::lexical_cast<uint16_t>(val);
				return true;
			case DataTypes_int32_t:
				boost::lexical_cast<int32_t>(val);
				return true;
			case DataTypes_uint32_t:
				boost::lexical_cast<uint32_t>(val);
				return true;
			case DataTypes_int64_t:
				boost::lexical_cast<int64_t>(val);
				return true;
			case DataTypes_uint64_t:
				boost::lexical_cast<uint16_t>(val);
				return true;
			default:
				return false;
			}
		}
		catch (boost::bad_lexical_cast&)
		{
			return false;
		}

		return true;
	}

	bool ConstantDefinition::VerifyValue()
	{
		return VerifyTypeAndValue(*Type, Value);
	}

	bool ConstantDefinition::VerifyTypeAndValue(TypeDefinition& t, boost::iterator_range<std::string::const_iterator> value)
	{
		if (t.ArrayType == DataTypes_ArrayTypes_multidimarray) return false;
		if (IsTypeNumeric(t.Type))
		{
			if (t.Type == DataTypes_cdouble_t || t.Type == DataTypes_csingle_t || t.Type == DataTypes_bool_t)
				return false;
			if (t.ContainerType != DataTypes_ContainerTypes_none) return false;
			if (t.ArrayType == DataTypes_ArrayTypes_none)
			{
				if (t.Type == DataTypes_double_t || t.Type == DataTypes_single_t)
				{
					boost::regex r_scalar("^[ \\t]*" RR_FLOAT_REGEX "[ \\t]*$");
					if (!regex_match(value.begin(), value.end(), r_scalar)) 
						return false;
				}
				else
				{
					boost::regex r_scalar("^[ \\t]*" RR_INT_REGEX "[ \\t]*$");
					if (!regex_match(value.begin(), value.end(), r_scalar)) 
						return false;					
				}
				return ConstantDefinition_CheckScalar(t.Type, value);
			}
			else
			{
				boost::smatch r_array_match;
				if (t.Type == DataTypes_double_t || t.Type == DataTypes_single_t)
				{
					boost::regex r_array("^[ \\t]*\\{[ \\t]*((?:" RR_FLOAT_REGEX ")(?:[ \\t]*,[ \\t]*(?:" RR_FLOAT_REGEX "))*)?[ \\t]*}[ \\t]*$");
					if (!regex_match(value.begin(), value.end(), r_array_match, r_array)) 
						return false;
				}
				else
				{
					boost::regex r_array("^[ \\t]*\\{[ \\t]*((?:" RR_INT_REGEX ")(?:[ \\t]*,[ \\t]*(?:" RR_INT_REGEX "))*)?[ \\t]*}[ \\t]*$");
					if (!regex_match(value.begin(), value.end(), r_array_match, r_array)) 
						return false;
				}

				if (!r_array_match[1].matched)
					return true;

				typedef boost::split_iterator<std::string::const_iterator> string_split_iterator;
				boost::iterator_range<std::string::const_iterator> r_array_match_1_rng(r_array_match[1].first, r_array_match[1].second);
				for (string_split_iterator e=boost::make_split_iterator(r_array_match_1_rng, boost::token_finder(boost::is_any_of(","), boost::token_compress_on));
					e!=string_split_iterator(); e++)				
				{					
					if (!ConstantDefinition_CheckScalar(t.Type, boost::trim_copy(*e)))
					{
						return false;
					}
				}
				return true;
			}
		}
		else
		{
			if (t.ArrayType != DataTypes_ArrayTypes_none) return false;
			if (t.Type == DataTypes_string_t)
			{
				boost::regex r_string("^[ \\t]*\"(?:(?:\\\\\"|\\\\\\\\|\\\\/|\\\\b|\\\\f|\\\\n|\\\\r|\\\\t|\\\\u[\\da-fA-F]{4})|[^\"\\\\])*\"[ \\t]*$");
				if (!boost::regex_match(value.begin(), value.end(), r_string))
					return false;
				return true;
			}
			else if (t.Type == DataTypes_namedtype_t)
			{
				boost::regex r_struct("^[ \\t]*\\{[ \\t]*(?:" RR_NAME_REGEX "[ \\t]*\\:[ \\t]*" RR_NAME_REGEX "(?:[ \\t]*,[ \\t]*" RR_NAME_REGEX "[ \\t]*\\:[ \\t]*" RR_NAME_REGEX ")*[ \\t]*)?\\}[ \\t]*$");
				if (!boost::regex_match(value.begin(), value.end(), r_struct))
					return false;
				return true;
			}
			return false;
		}
	}

	void ConstantDefinition::Reset()
	{
		Name.clear();
		Type.reset();
		Value.clear();
	}

		
	std::string ConstantDefinition::ValueToString()
	{
		if (!Type) throw InvalidOperationException("Invalid operation");
		if (Type->Type != DataTypes_string_t) throw InvalidOperationException("Invalid operation");

		boost::regex r_string("^[ \\t]*\"((?:(?:\\\\\"|\\\\\\\\|\\\\/|\\\\b|\\\\f|\\\\n|\\\\r|\\\\t|\\\\u[\\da-fA-F]{4})|(?:(?![\"\\\\])[ -~]))*)\"[ \\t]*$");
		boost::smatch r_string_match;
		if (!boost::regex_match(Value, r_string_match, r_string))
			throw RobotRaconteurParseException("Invalid string constant format");

		std::string value2 = r_string_match[1].str();
		return UnescapeString(value2);
	}

	static std::string ConstantDefinition_UnescapeString_Formatter(const boost::smatch& match)
	{
		std::string i = match[0].str();
		if (i == "\\\"") return "\"";
		if (i == "\\\\") return "\\";
		if (i == "\\/") return "/";
		if (i == "\\b") return "\b";
		if (i == "\\f") return "\f";
		if (i == "\\n") return "\n";
		if (i == "\\r") return "\r";
		if (i == "\\t") return "\t";

		if (boost::starts_with(i, "\\u"))
		{
			std::stringstream ss;
			std::basic_string<uint16_t> v3;
			v3.resize(i.size() / 6);
			for (size_t j = 0; j<v3.size(); j++)
			{
				std::string v = i.substr(j * 6 + 2, 4);
				ss.clear();
				uint16_t v2;
				ss << std::hex << v;
				ss >> std::hex >> v2;
				v3[j] = v2;
			}
			return boost::locale::conv::utf_to_utf<char>(v3);
		}

		throw InternalErrorException("Internal error");
	}

	std::string ConstantDefinition::UnescapeString(const std::string& in)
	{
		std::ostringstream t(std::ios::out | std::ios::binary);
		std::ostream_iterator<char, char> oi(t);

		boost::regex r_string_expression("(\\\\\"|\\\\\\\\|\\\\/|\\\\b|\\\\f|\\\\n|\\\\r|\\\\t|(?:\\\\u[\\da-fA-F]{4})+)");
		//const char* format_string = "(?1\\\")(?2\\\\)(?3\\b)(?4\\f)(?5\\n)(?6\\r)(?7\\t)(?8\\"

		boost::regex_replace(oi, in.begin(), in.end(), r_string_expression, ConstantDefinition_UnescapeString_Formatter,
			boost::match_default | boost::format_all);

		return t.str();
	}


	static std::string ConstantDefinition_EscapeString_Formatter(const boost::smatch& match)
	{
		std::string i = match[0].str();

		if (i == "\"") return "\\\"";
		if (i == "\\") return "\\\\";
		if (i == "/") return "\\/";
		if (i == "\b") return "\\b";
		if (i == "\f") return "\\f";
		if (i == "\n") return "\\n";
		if (i == "\r") return "\\r";
		if (i == "\t") return "\\t";

		std::basic_string<uint16_t> v = boost::locale::conv::utf_to_utf<uint16_t>(i);

		std::stringstream v2;
		v2 << std::hex << std::setfill('0');
		BOOST_FOREACH(const uint16_t& v3, v)
		{
			v2 << std::setw(0) << "\\u" << std::setw(4) << v3;
		}

		return v2.str();
	}

	std::string ConstantDefinition::EscapeString(const std::string& in)
	{
		boost::regex r_replace("(\"|\\\\|\\/|[\\x00-\\x1F]|\\x7F|[\\x80-\\xFF]+)");

		std::ostringstream t(std::ios::out | std::ios::binary);
		std::ostream_iterator<char, char> oi(t);

		boost::regex_replace(oi, in.begin(), in.end(), r_replace, ConstantDefinition_EscapeString_Formatter,
			boost::match_default | boost::format_all);

		return t.str();
	}

	std::vector<ConstantDefinition_StructField> ConstantDefinition::ValueToStructFields()
	{

		std::vector<ConstantDefinition_StructField> o;		
		boost::iterator_range<std::string::const_iterator> value1(Value);
		value1 = boost::trim_copy_if(value1, boost::is_any_of(" \t{}"));
		value1 = boost::trim_copy(value1);

		if (value1.empty())
			return o;

		boost::regex r("[ \\t]*(" RR_NAME_REGEX ")[ \\t]*\\:[ \\t]*(" RR_NAME_REGEX ")[ \\t]*");

		typedef boost::split_iterator<std::string::const_iterator> string_split_iterator;
		for (string_split_iterator e = boost::make_split_iterator(value1, boost::token_finder(boost::is_any_of(","), boost::token_compress_on));
			e != string_split_iterator(); e++)
		{
			
			boost::smatch r_match;
			if (!boost::regex_match(e->begin(), e->end(), r_match, r))
			{
				throw RobotRaconteurParseException("Invalid struct constant format");
			}
			ConstantDefinition_StructField f;
			f.Name = r_match[1];
			f.ConstantRefName = r_match[2];
			o.push_back(f);
		}

		return o;
	}

	EnumDefinition::~EnumDefinition()
	{

	}

	EnumDefinition::EnumDefinition(RR_SHARED_PTR<ServiceDefinition> service)
	{
		this->service = service;
		Reset();
	}

	std::string EnumDefinition::ToString()
	{
		if (!VerifyValues())
		{
			throw DataTypeException("Invalid enum: " + Name);
		}

		std::string s = "enum " + Name + "\n";

		std::vector<std::string> values;

		BOOST_FOREACH(EnumDefinitionValue& e, Values)
		{
			if (e.ImplicitValue)
			{
				values.push_back( "    " + e.Name);
			}
			else
			{
				if (!e.HexValue)
				{
					values.push_back( "    " + e.Name + " = " + boost::lexical_cast<std::string>(e.Value));
				}
				else
				{
					uint32_t v = static_cast<uint32_t>(e.Value);
					std::stringstream ss;
					ss << std::hex << v;
					values.push_back( "    " + e.Name + " = 0x" + ss.str());
				}
			}
		}
		s += boost::join(values, ",\n");
		s += "\nend enum\n";
		return s;
	}
	void EnumDefinition::FromString(const std::string& s)
	{
		FromString(s, 0);
	}
	void EnumDefinition::FromString(const std::string& s, int32_t startline)
	{
		Reset();

		std::string s2 = boost::trim_copy(s);
		std::vector<std::string> lines;
		boost::split(lines, s2, boost::is_any_of("\n"));
		if (lines.size() < 2) 
			throw RobotRaconteurParseException("Invalid enum", startline);

		boost::regex r_start("^[ \\t]*enum[ \\t]+([a-zA-Z]\\w*)[ \\t]*$");
		boost::regex r_end("^[ \\t]*end(?:[ \\t]+enum)?[ \\t]*$");

		
		boost::smatch r_start_match;			
		if (!boost::regex_match(lines.front(), r_start_match, r_start))
		{
			throw RobotRaconteurParseException("Parse error near: " + lines.front(), boost::numeric_cast<int32_t>(startline));
		}
		Name = r_start_match[1];

		if (!boost::regex_match(lines.back(), r_end))
		{
			throw RobotRaconteurParseException("Parse error near: " + lines.front(), boost::numeric_cast<int32_t>(startline+lines.size()-1));
		}
		
		std::string values1 = boost::join(boost::make_iterator_range(++lines.begin(), --lines.end()), " ");
		std::vector<std::string> values2;
		boost::split(values2, values1, boost::is_any_of(","));

		boost::regex r_value("^[ \\t]*([A-Za-z]\\w*)(?:[ \\t]*=[ \\t]*(?:(0x\\d+)|(-?\\d+)))?[ \\t]*$");

		BOOST_FOREACH(const std::string& l, values2)
		{
			boost::smatch r_value_match;
			if (!boost::regex_match(l, r_value_match, r_value))
			{
				throw RobotRaconteurParseException("Enum value parse error near: " + lines.front(), boost::numeric_cast<int32_t>(startline + lines.size() - 1));
			}

			Values.resize(Values.size() + 1);
			EnumDefinitionValue& enum_i = Values.back();
			enum_i.Name = r_value_match[1];

			if (r_value_match[2].matched)
			{
				std::stringstream ss;
				ss << std::hex << std::string(r_value_match[2]);
				uint32_t v;
				ss >> v;
				enum_i.ImplicitValue = false;
				enum_i.Value = static_cast<uint32_t>(v);
				enum_i.HexValue = true;
			}
			else if (r_value_match[3].matched)
			{ 
				enum_i.ImplicitValue = false;
				enum_i.Value = boost::lexical_cast<int32_t>(r_value_match[3]);
				enum_i.HexValue = false;
			}
			else
			{
				if (Values.size() == 1)
				{
					throw RobotRaconteurParseException("Enum first value must be specified: " + lines.front(), boost::numeric_cast<int32_t>(startline + lines.size() - 1));
				}

				enum_i.ImplicitValue = true;				
				enum_i.Value = (Values.rbegin()+1)->Value + 1;
				enum_i.HexValue = (Values.rbegin() + 1)->HexValue;
			}			
		}

		if (!VerifyValues())
		{
			throw RobotRaconteurParseException("Enum names or values not unique: " + lines.front(), boost::numeric_cast<int32_t>(startline));
		}

	}
	bool EnumDefinition::VerifyValues()
	{
		if (Values.size() == 1)
		{
			return true;
		}


		std::vector<EnumDefinitionValue>::iterator e = Values.begin();
		std::vector<EnumDefinitionValue>::iterator e_end = Values.end()--;

		std::vector<EnumDefinitionValue>::iterator e2_end = Values.end();
		for (; e != e_end; e++)
		{
			std::vector<EnumDefinitionValue>::iterator e2 = e+1;
			for (; e2 != e2_end; e2++)
			{
				if (e->Value == e2->Value) 
					return false;
				if (e->Name == e2->Name) 
					return false;
			}
		}

		return true;
	}

	void EnumDefinition::Reset()
	{
		Values.clear();
	}

	DataTypes EnumDefinition::RRDataType()
	{
		return DataTypes_enum_t;
	}

	std::string EnumDefinition::ResolveQualifiedName()
	{
		RR_SHARED_PTR<ServiceDefinition> def = service.lock();
		if (!def) throw InvalidOperationException("Could not lock service definition to resolve named type");
		return def->Name + "." + Name;
	}

	EnumDefinitionValue::EnumDefinitionValue()
	{
		Value = 0;
		ImplicitValue = false;
		HexValue = false;
		Name.clear();
	}

	RobotRaconteurParseException::RobotRaconteurParseException(const std::string &e) : std::runtime_error(e)
	{
		Message=e;		
		LineNumber = -1;
		what_store = ToString();
	}

	RobotRaconteurParseException::RobotRaconteurParseException(const std::string &e, int32_t line) : std::runtime_error(e)
	{
		Message=e;
		LineNumber = line;
		what_store = ToString();

	}

	std::string RobotRaconteurParseException::ToString()
	{
		return "RobotRaconteur Parse Error On Line " + boost::lexical_cast<std::string>(LineNumber) + ": " + Message;
	}

	const char* RobotRaconteurParseException::what() const throw ()
	{
		
		return what_store.c_str();
	}


	//Code to verify the service definition.  Most of these functions are not made available in the header.

	static void VerifyVersionSupport(RR_SHARED_PTR<ServiceDefinition>& def, int32_t major, int32_t minor, const char* msg)
	{
		RobotRaconteurVersion def_version = def->StdVer;
		if (!def_version) 
			return;

		if (def_version < RobotRaconteurVersion(major, minor))
		{
			if (msg)
			{
				throw ServiceDefinitionException(msg);
			}
			else
			{
				throw ServiceDefinitionException("Newer service definition standard required for feature");
			}
		}
	}

	void VerifyName(std::string name,RR_SHARED_PTR<ServiceDefinition> def, bool allowdot=false, bool ignorereserved=false)
	{
		
		if (name.length()==0) throw ServiceDefinitionException("Empty name in service definition \"" + def->Name + "\"");

		std::string name2=boost::to_lower_copy(name);

		if (!ignorereserved)
		{
			if (name=="this" || name=="self" || name =="Me") throw ServiceDefinitionException("The names \"this\", \"self\", and \"Me\" are reserved, error in service definition \"" + def->Name + "\"");

			const char* res_str[]={"object","end","option","service","object","struct","import","implements","field","property","function","event","objref","pipe","callback","wire","memory","void","int8","uint8","int16","uint16","int32","uint32","int64","uint64","single","double","varvalue","varobject","exception", "using", "constant", "enum", "pod", "namedarray", "cdouble", "csingle", "bool"};
			std::vector<std::string> reserved(res_str,res_str+sizeof(res_str)/(sizeof(res_str[0])));

			if (boost::range::find(reserved,name)!=reserved.end())
			{
				throw ServiceDefinitionException("Name \"" + name + "\" is reserved in service definition\"" + def->Name + "\"");
			}

			if (boost::starts_with(name2,"get_") || boost::starts_with(name2,"set_") || boost::starts_with(name2,"rr") || boost::starts_with(name2,"robotraconteur") || boost::starts_with(name2,"async_"))
			{
				throw ServiceDefinitionException("Name \"" + name + "\" is invalid in service definition \"" + def->Name + "\"");
			}
		}

		if (allowdot)
		{
			if (!boost::regex_match(name, boost::regex("^" RR_TYPE_REGEX "$")))
			{
				throw ServiceDefinitionException("Name \"" + name + "\" is invalid in service definition \"" + def->Name + "\"");
			}
		}
		else
		{
			if (!boost::regex_match(name, boost::regex("^" RR_NAME_REGEX "$")))
			{
				throw ServiceDefinitionException("Name \"" + name + "\" is invalid in service definition \"" + def->Name + "\"");
			}
		}
				
	}

	std::string VerifyConstant(const std::string& constant, RR_SHARED_PTR<ServiceDefinition> def)
	{
		RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(def);
		try
		{
			c->FromString(constant);
		}
		catch (std::exception&) {
			throw ServiceDefinitionException("Error in constant in service definition \"" + def->Name + "\"");
		}
		
		if (!c->VerifyValue()) throw ServiceDefinitionException("Error in constant " + c->Name + " in service definition \"" + def->Name + "\"");

		if (c->Type->Type == DataTypes_namedtype_t) throw ServiceDefinitionException("Error in constant " + c->Name + " in service definition \"" + def->Name + "\"");

		VerifyName(c->Name,def);

		return c->Name;

	}

	void VerifyConstantStruct(const RR_SHARED_PTR<ConstantDefinition>& c, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ConstantDefinition> >& constants, std::vector<std::string> parent_types)
	{
		std::vector<ConstantDefinition_StructField> fields;
		fields = c->ValueToStructFields();
		parent_types.push_back(c->Name);
		BOOST_FOREACH(ConstantDefinition_StructField& e, fields)
		{
			VerifyName(e.Name, def);
			BOOST_FOREACH(const std::string& name, parent_types)
			{
				if (e.ConstantRefName == name) 
					throw ServiceDefinitionException("Error in constant " + c->Name + " in service definition \"" + def->Name + "\": recursive struct not allowed");
			}
			bool found = false;
			BOOST_FOREACH(RR_SHARED_PTR<ConstantDefinition>& f, constants)
			{
				if (f->Name == e.ConstantRefName)
				{
					found = true;
					if (f->Type->Type == DataTypes_namedtype_t)
					{
						VerifyConstantStruct(f, def, constants, parent_types);
					}
					break;
				}
			}

			if (!found) throw ServiceDefinitionException("Error in constant " + c->Name + " in service definition \"" + def->Name + "\": struct field " + e.ConstantRefName + " not found");
		}
	}

	std::string VerifyConstant(const RR_SHARED_PTR<ConstantDefinition>& c, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ConstantDefinition> >& constants)
	{
		if (!c->VerifyValue()) throw ServiceDefinitionException("Error in constant " + c->Name + " in service definition \"" + def->Name + "\"");
		VerifyName(c->Name, def);

		if (c->Type->Type == DataTypes_namedtype_t)
		{
			VerifyConstantStruct(c, def, constants, std::vector<std::string>());

		}

		return c->Name;
	}

	void VerifyEnum(EnumDefinition& e, RR_SHARED_PTR<ServiceDefinition> def)
	{
		if (!e.VerifyValues())
		{
			throw ServiceDefinitionException("Error in constant in enum definition \"" + def->Name + "\"");
		}

		VerifyName(e.Name, def);
		BOOST_FOREACH(const EnumDefinitionValue& e1, e.Values)
		{
			VerifyName(e1.Name, def);
		}
	}

	std::vector<std::string> GetServiceNames(RR_SHARED_PTR<ServiceDefinition>& def)
	{
		std::vector<std::string> o;
		BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition>& e, def->Objects)
		{
			o.push_back(e->Name);
		}
		BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition>& e, def->Structures)
		{
			o.push_back(e->Name);
		}
		BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition>& e, def->Pods)
		{
			o.push_back(e->Name);
		}
		BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition>& e, def->NamedArrays)
		{
			o.push_back(e->Name);
		}
		BOOST_FOREACH(RR_SHARED_PTR<ConstantDefinition>& e, def->Constants)
		{
			o.push_back(e->Name);
		}
		BOOST_FOREACH(RR_SHARED_PTR<EnumDefinition>& e, def->Enums)
		{
			o.push_back(e->Name);
		}
		BOOST_FOREACH(const std::string& e, def->Exceptions)
		{
			o.push_back(e);
		}

		return o;
	}

	void VerifyUsing(UsingDefinition& e, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> >& importeddefs)
	{
		VerifyName(e.UnqualifiedName, def);
		boost::regex r(RR_QUAIFIED_TYPE_REGEX);
		if (!boost::regex_match(e.QualifiedName, r))
		{
			throw ServiceDefinitionException("Using \"" + e.QualifiedName + "\" is invalid in service definition \"" + def->Name + "\"");
		}

		boost::tuple<std::string, std::string> s1=SplitQualifiedName(e.QualifiedName);

		BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition> d1, importeddefs)
		{
			if (s1.get<0>() == d1->Name)
			{
				std::vector<std::string> importeddefs_names = GetServiceNames(d1);
				if (boost::range::find(importeddefs_names, s1.get<1>()) == importeddefs_names.end())
				{
					throw ServiceDefinitionException("Using \"" + e.QualifiedName + "\" is invalid in service definition \"" + def->Name + "\"");
				}
				return;
			}
		}

		throw ServiceDefinitionException("Using \"" + e.QualifiedName + "\" is invalid in service definition \"" + def->Name + "\"");
	}

	void VerifyType(RR_SHARED_PTR<TypeDefinition> t, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > defs)
	{
		switch (t->ArrayType)
		{
		case DataTypes_ArrayTypes_none:
		case DataTypes_ArrayTypes_array:
		case DataTypes_ArrayTypes_multidimarray:
			break;
		default:
			throw ServiceDefinitionException("Invalid Robot Raconteur data type \"" + t->ToString() + "\" type in service \"" + def->Name + "\"");
		}

		switch (t->ContainerType)
		{
		case DataTypes_ContainerTypes_none:
		case DataTypes_ContainerTypes_list:
		case DataTypes_ContainerTypes_map_int32:
		case DataTypes_ContainerTypes_map_string:
			break;
		default:
			throw ServiceDefinitionException("Invalid Robot Raconteur data type \"" + t->ToString() + "\" type in service \"" + def->Name + "\"");
		}
		
		if (IsTypeNumeric(t->Type))
		{
			return;

		}
		if (t->Type == DataTypes_string_t)
		{
			if (t->ArrayType != DataTypes_ArrayTypes_none) throw ServiceDefinitionException("Invalid Robot Raconteur data type \"" + t->ToString() + "\" type in service \"" + def->Name + "\"");

			return;
		}
		if (t->Type==DataTypes_vector_t || t->Type==DataTypes_dictionary_t || t->Type==DataTypes_object_t || t->Type==DataTypes_varvalue_t || t->Type==DataTypes_varobject_t || t->Type==DataTypes_multidimarray_t) return;
		if (t->Type==DataTypes_namedtype_t)
		{
			RR_SHARED_PTR<NamedTypeDefinition> nt = t->ResolveNamedType(defs);
			DataTypes nt_type = nt->RRDataType();
			if ((nt_type != DataTypes_pod_t && nt_type != DataTypes_namedarray_t) && t->ArrayType != DataTypes_ArrayTypes_none) throw ServiceDefinitionException("Invalid Robot Raconteur data type \"" + t->ToString() + "\" type in service \"" + def->Name + "\"");
			if (nt_type != DataTypes_structure_t && nt_type != DataTypes_pod_t && nt_type != DataTypes_namedarray_t && nt_type != DataTypes_enum_t) throw ServiceDefinitionException("Invalid Robot Raconteur data type \"" + t->ToString() + "\" type in service \"" + def->Name + "\"");
			if (nt_type == DataTypes_pod_t)
			{

			}
			return;
		}
		throw ServiceDefinitionException("Invalid Robot Raconteur data type \"" + t->ToString() + "\" type in service \"" + def->Name + "\"");
	}

	void VerifyReturnType(RR_SHARED_PTR<TypeDefinition> t, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > defs)
	{
		if (t->Type == DataTypes_void_t)
		{
			if (t->ArrayType != DataTypes_ArrayTypes_none || t->ContainerType != DataTypes_ContainerTypes_none)
			{
				throw ServiceDefinitionException("Invalid Robot Raconteur data type \"" + t->ToString() + "\" type in service \"" + def->Name + "\"");
			}
			return;
		}
		else
		{
			VerifyType(t, def, defs);
		}
	}

	void VerifyParameters(std::vector<RR_SHARED_PTR<TypeDefinition> > p, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > defs)
	{
		std::vector<std::string*> names;
		BOOST_FOREACH(RR_SHARED_PTR<TypeDefinition>& t, p)
		{
			VerifyType(t, def, defs);
			if (boost::range::find(names | boost::adaptors::indirected, t->Name) != boost::adaptors::indirect(names).end())
				throw ServiceDefinitionException("Parameters must have unique names");
			names.push_back(&t->Name);
		}	
	}

	void VerifyModifiers(RR_SHARED_PTR<MemberDefinition>& m, bool readwrite, bool unreliable, bool nolock, bool nolockread, bool perclient, bool urgent, std::vector<RobotRaconteurParseException>& warnings)
	{
		bool direction_found = false;
		bool unreliable_found = false;
		bool nolock_found = false;
		bool perclient_found = false;
		bool urgent_found = false;

		BOOST_FOREACH(std::string& s, m->Modifiers)
		{
			if (readwrite)
			{
				if (s == "readonly" || s == "writeonly")
				{
					if (direction_found)
					{
						warnings.push_back(RobotRaconteurParseException("Invalid member modifier combination: [readonly,writeonly]"));
					}
					direction_found = true;
					continue;
				}
			}

			if (unreliable)
			{
				if (s == "unreliable")
				{
					RR_SHARED_PTR<ServiceEntryDefinition> obj = m->ServiceEntry.lock();
					if (obj)
					{
						BOOST_FOREACH(const std::string& o, obj->Options)
						{
							boost::regex r("^[ \\t]*pipe[ \\t]+" + m->Name + "[ \\t]+unreliable[ \\t]*$");
							if (boost::regex_match(o, r))
							{
								warnings.push_back(RobotRaconteurParseException("Invalid member modifier combination: [unreliable]"));
							}
						}
					}

					if (unreliable_found)
					{
						warnings.push_back(RobotRaconteurParseException("Invalid member modifier combination: [unreliable]"));
					}
					unreliable_found = true;
					continue;
				}
			}

			if (nolock)
			{
				if (s == "nolock")
				{
					if (nolock_found)
					{
						warnings.push_back(RobotRaconteurParseException("Invalid member modifier combination: [nolock]"));
					}
					nolock_found = true;
					continue;
				}
			}

			if (nolock)
			{
				if (s == "nolockread")
				{
					if (nolock_found)
					{
						warnings.push_back(RobotRaconteurParseException("Invalid member modifier combination: [nolock,nolockread]"));
					}
					nolock_found = true;
					continue;
				}
			}

			if (perclient)
			{
				if (s == "perclient")
				{
					if (perclient_found)
					{
						warnings.push_back(RobotRaconteurParseException("Invalid member modifier combination: [perclient]"));
					}
					perclient_found = true;
					continue;
				}
			}

			if (urgent)
			{
				if (s == "urgent")
				{
					if (urgent_found)
					{
						warnings.push_back(RobotRaconteurParseException("Invalid member modifier combination: [urgent]"));
					}
					urgent_found = true;
					continue;
				}
			}

			warnings.push_back(RobotRaconteurParseException("Unknown member modifier: [" + s + "]"));
		}
	}

	std::string VerifyMember(RR_SHARED_PTR<MemberDefinition> m, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > defs, std::vector<RobotRaconteurParseException>& warnings)
	{
		VerifyName(m->Name,def);

		if (!m->Modifiers.empty())
		{
			VerifyVersionSupport(def, 0, 9, "Service definition standard version 0.9 or greater required for Member Modifiers");
		}

		RR_SHARED_PTR<PropertyDefinition> p=RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(m);
		if (p)
		{
			VerifyType(p->Type,def,defs);				
			VerifyModifiers(m, true, false, true, true, true, true, warnings);
			return p->Name;
		}
		RR_SHARED_PTR<FunctionDefinition> f=RR_DYNAMIC_POINTER_CAST<FunctionDefinition>(m);
		if (f)
		{
			VerifyModifiers(m, false, false, true, false, false, true, warnings);
			if (!f->IsGenerator())
			{
				VerifyParameters(f->Parameters, def, defs);
				VerifyReturnType(f->ReturnType, def, defs);
				
				return f->Name;
			}
			else
			{
				bool generator_found = false;;
				if (f->ReturnType->ContainerType == DataTypes_ContainerTypes_generator)
				{
					if (f->ReturnType->Type == DataTypes_void_t)
					{
						throw ServiceDefinitionException("Generator return must not be void");
					}

					RR_SHARED_PTR<TypeDefinition> ret2 = f->ReturnType->Clone();
					ret2->RemoveContainers();
					VerifyType(ret2, def, defs);
					if (f->ReturnType->Type == DataTypes_namedtype_t) f->ReturnType->ResolveNamedType(defs);
					generator_found = true;
				}
				else
				{ 
					if (f->ReturnType->Type != DataTypes_void_t)
					{
						throw ServiceDefinitionException("Generator return must use generator container");
					}
					//VerifyReturnType(f->ReturnType, def, defs);
				}

				if (!f->Parameters.empty() && f->Parameters.back()->ContainerType == DataTypes_ContainerTypes_generator)
				{
					RR_SHARED_PTR<TypeDefinition> p2 = f->Parameters.back()->Clone();
					p2->RemoveContainers();
					VerifyType(p2, def, defs);
					if (f->Parameters.back()->Type == DataTypes_namedtype_t) f->Parameters.back()->ResolveNamedType(defs);
					std::vector<RR_SHARED_PTR<TypeDefinition> > params2;
					std::copy(f->Parameters.begin(), --f->Parameters.end(), std::back_inserter(params2));
					VerifyParameters(params2, def, defs);
					generator_found = true;
				}
				else
				{
					VerifyParameters(f->Parameters, def, defs);
				}

				if (!generator_found)
				{
					throw ServiceDefinitionException("Generator return or parameter not found");
				}
				
				return f->Name;
			}
		}
		RR_SHARED_PTR<EventDefinition> e=RR_DYNAMIC_POINTER_CAST<EventDefinition>(m);
		if (e)
		{
			VerifyParameters(e->Parameters, def, defs);			
			VerifyModifiers(m, false, false, false, false, false, true, warnings);
			return e->Name;
		}
		RR_SHARED_PTR<ObjRefDefinition> o=RR_DYNAMIC_POINTER_CAST<ObjRefDefinition>(m);
		if (o)
		{

			std::vector<std::string> modifiers;			
			VerifyModifiers(m, false, false, false, false, false, false, warnings);

			if (o->ObjectType=="varobject") return o->Name;

			if(!boost::contains(o->ObjectType,"."))
			{
				BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& ee, def->Objects)
				{
					if (ee->Name==o->ObjectType) return o->Name;
				}
			}
			else
			{

				boost::tuple<std::string,std::string> s1=SplitQualifiedName(o->ObjectType);
				
				std::string defname=s1.get<0>();
				RR_SHARED_PTR<ServiceDefinition> def2;

				BOOST_FOREACH (RR_SHARED_PTR<ServiceDefinition>& ee2, defs)
				{
					if (ee2->Name == defname)
					{						
						def2 = ee2;
						break;
					}
				}
				

				if (def2)
				{
					BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& ee, def2->Objects)
					{
						if (ee->Name == s1.get<1>())
						{
							return o->Name;
							break;
						}
					}
				}
			}
			throw ServiceDefinitionException("Unknown object type \"" + o->ObjectType + "\" in member \"" +m->Name + "\" in  service \"" + def->Name + "\"");
		}

		RR_SHARED_PTR<PipeDefinition> p2=RR_DYNAMIC_POINTER_CAST<PipeDefinition>(m);
		if (p2)
		{
			VerifyType(p2->Type,def,defs);			
			VerifyModifiers(m, true, true, true, false, false, false, warnings);
			return p2->Name;
		}

		RR_SHARED_PTR<CallbackDefinition> c=RR_DYNAMIC_POINTER_CAST<CallbackDefinition>(m);
		if (c)
		{
			VerifyParameters(c->Parameters, def, defs);
			VerifyReturnType(c->ReturnType,def,defs);				
			VerifyModifiers(m, false, false, false, false, false, true, warnings);
			return c->Name;
		}

		RR_SHARED_PTR<WireDefinition> w=RR_DYNAMIC_POINTER_CAST<WireDefinition>(m);
		if (w)
		{
			VerifyType(w->Type,def,defs);
			VerifyModifiers(m, true, false, true, false, false, false, warnings);
			return w->Name;
		}

		RR_SHARED_PTR<MemoryDefinition> m2=RR_DYNAMIC_POINTER_CAST<MemoryDefinition>(m);
		if (m2)
		{
			VerifyType(m2->Type, def, defs);
			VerifyModifiers(m, true, false, true, true, false, false, warnings);
			if (!IsTypeNumeric(m2->Type->Type))
			{
				if (m2->Type->Type != DataTypes_namedtype_t)
				{ 
					throw ServiceDefinitionException("Memory member must be numeric or pod");
				}
				RR_SHARED_PTR<NamedTypeDefinition> nt = m2->Type->ResolveNamedType(defs);
				if (nt->RRDataType() != DataTypes_pod_t && nt->RRDataType() != DataTypes_namedarray_t)
				{
					throw ServiceDefinitionException("Memory member must be numeric or pod");
				}
			}
			switch (m2->Type->ArrayType)
			{
			case DataTypes_ArrayTypes_array:
			case DataTypes_ArrayTypes_multidimarray:
				break;
			default:
				throw ServiceDefinitionException("Memory member must be numeric or pod");
			}
			
			if (!m2->Type->ArrayVarLength)
			{
				throw ServiceDefinitionException("Memory member must not be fixed size");
			}

			if (!m2->Type->ArrayLength.empty())
			{
				int32_t array_count = boost::accumulate(m2->Type->ArrayLength, 1, std::multiplies<int32_t>());
				if (array_count != 0)
				{
					throw ServiceDefinitionException("Memory member must not be fixed size");
				}
			}

			return m2->Name;
		}

		throw ServiceDefinitionException("Invalid member \"" +m->Name + "\" type in service \"" + def->Name + "\"");
	}
		

	struct rrimplements
	{
		std::string name;
		RR_SHARED_PTR<ServiceEntryDefinition> obj;
		std::vector<rrimplements> implements;
	};

	

	rrimplements get_implements(RR_SHARED_PTR<ServiceEntryDefinition> obj, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs, std::string rootobj="")
	{
		rrimplements out;
		out.obj=obj;
		out.name=def->Name + "." + obj->Name;
		
		if (rootobj=="") rootobj=out.name;

		BOOST_FOREACH (std::string& e, obj->Implements)
		{
			if (!boost::contains(e,"."))
			{
				RR_SHARED_PTR<ServiceEntryDefinition> obj2;
				BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& ee2, def->Objects)
				{
					if (ee2->Name == e)
					{
						obj2 = ee2;
						break;
					}
				}

				if (!obj2) throw ServiceDefinitionException("Object \"" + def->Name + "." + e + " not found");

				if (rootobj== (def->Name + "." + obj2->Name)) throw ServiceDefinitionException("Recursive implements between \"" + rootobj + "\" and \"" + def->Name + "." + obj2->Name + "\"");

				rrimplements imp2=get_implements(obj2,def,defs,rootobj);
				out.implements.push_back(imp2);
			}
			else
			{
				boost::tuple<std::string,std::string> s1=SplitQualifiedName(e);
								
				RR_SHARED_PTR<ServiceDefinition> def2;

				BOOST_FOREACH (RR_SHARED_PTR<ServiceDefinition>& ee, defs)
				{
					if (ee->Name == s1.get<0>())
					{
						def2 = ee;
						break;
					}
				}

			
				if (!def2) throw ServiceDefinitionException("Service definition \"" + e + "\" not found.");

				RR_SHARED_PTR<ServiceEntryDefinition> obj2;
				BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& ee2,def2->Objects)
				{
					if (ee2->Name == s1.get<1>())
					{
						obj2 = ee2;
						break;
					}
				}

				if (!obj2) throw ServiceDefinitionException("Object \"" + e + " not found");

				if (rootobj== (def2->Name + "." + obj2->Name)) throw ServiceDefinitionException("Recursive implements between \"" + rootobj + "\" and \"" + def2->Name + "." + obj2->Name + "\"");

				rrimplements imp2=get_implements(obj2,def2,defs,rootobj);
				out.implements.push_back(imp2);
			}
			
			


		}

		BOOST_FOREACH(rrimplements& r, out.implements)
		{
			BOOST_FOREACH(rrimplements& r2, r.implements)
			{
				bool found = false;
				BOOST_FOREACH(rrimplements& r3, out.implements)
				{
					if (r2.name == r3.name)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					throw ServiceDefinitionException("Object \"" + out.name + "\" does not implement inherited type \"" + r2.name + "\"");
				}
			}
		}


		
		return out;

	}

	bool CompareTypeDefinition(RR_SHARED_PTR<ServiceDefinition> d1, RR_SHARED_PTR<TypeDefinition> t1, RR_SHARED_PTR<ServiceDefinition> d2, RR_SHARED_PTR<TypeDefinition> t2)
	{
		if (t1->Name != t2->Name) return false;
		//if (t1->ImportedType!=t2->ImportedType) return false;
		if (t1->ArrayType!=t2->ArrayType) return false;
		if (t1->ArrayType != DataTypes_ArrayTypes_none)
		{
			if (t1->ArrayVarLength!=t2->ArrayVarLength) return false;
			if (t1->ArrayLength.size() != t2->ArrayLength.size()) return false;
			if (!boost::range::equal(t1->ArrayLength,t2->ArrayLength)) return false;
		}

		if (t1->ContainerType != t2->ContainerType) return false;
		
		if(t1->Type!=t2->Type) return false;
		if (t1->Type!=DataTypes_namedtype_t && t1->Type != DataTypes_object_t) return true;

		if (t1->TypeString=="varvalue" && t2->TypeString=="varvalue") return true;

		
		std::string st1="";
		std::string st2="";

		if (!boost::contains(t1->TypeString,"."))
		{
			st1=d1->Name + "." + t1->TypeString;
		}
		else 
		{
			st1=t1->TypeString;
		}
		
		if (!boost::contains(t2->TypeString,"."))
		{
			st2=d2->Name + "." + t2->TypeString;
		}
		else 
		{
			st2=t2->TypeString;
		}
		

		return st1==st2;

	}

	bool CompareTypeDefinitions(RR_SHARED_PTR<ServiceDefinition> d1, std::vector<RR_SHARED_PTR<TypeDefinition> >& t1, RR_SHARED_PTR<ServiceDefinition> d2, std::vector<RR_SHARED_PTR<TypeDefinition> >& t2)
	{
		if (t1.size()!=t2.size()) return false;
		for (size_t i=0; i<t1.size(); i++)
		{
			if (!CompareTypeDefinition(d1,t1[i],d2,t2[i])) return false;
		}

		return true;
	}

	bool CompareMember(RR_SHARED_PTR<MemberDefinition> m1, RR_SHARED_PTR<MemberDefinition> m2)
	{
		if (m1->Name != m2->Name) return false;
		if (!boost::range::equal(m1->Modifiers, m2->Modifiers)) return false;

		RR_SHARED_PTR<ServiceEntryDefinition> e1=m1->ServiceEntry.lock();
		RR_SHARED_PTR<ServiceEntryDefinition> e2=m2->ServiceEntry.lock();
		if ((!e1) || (!e2)) return false;

		RR_SHARED_PTR<ServiceDefinition> d1=e1->ServiceDefinition_.lock();
		RR_SHARED_PTR<ServiceDefinition> d2=e2->ServiceDefinition_.lock();
		if ((!d1) || (!d2)) return false;

		RR_SHARED_PTR<PropertyDefinition> p1=RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(m1);
		RR_SHARED_PTR<PropertyDefinition> p2=RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(m2);
		if (p1)
		{
			if(!p2) return false;
			return CompareTypeDefinition(d1,p1->Type,d2,p2->Type);
		}

		RR_SHARED_PTR<FunctionDefinition> f1=RR_DYNAMIC_POINTER_CAST<FunctionDefinition>(m1);
		RR_SHARED_PTR<FunctionDefinition> f2=RR_DYNAMIC_POINTER_CAST<FunctionDefinition>(m2);
		if (f1)
		{
			if(!f2) return false;
			if(!CompareTypeDefinition(d1,f1->ReturnType,d2,f2->ReturnType)) return false;
			return CompareTypeDefinitions(d1,f1->Parameters,d2,f2->Parameters);
		}

		RR_SHARED_PTR<EventDefinition> ev1=RR_DYNAMIC_POINTER_CAST<EventDefinition>(m1);
		RR_SHARED_PTR<EventDefinition> ev2=RR_DYNAMIC_POINTER_CAST<EventDefinition>(m2);
		if (ev1)
		{
			if(!ev2) return false;
			
			return CompareTypeDefinitions(d1,ev1->Parameters,d2,ev2->Parameters);
		}

		RR_SHARED_PTR<ObjRefDefinition> o1=RR_DYNAMIC_POINTER_CAST<ObjRefDefinition>(m1);
		RR_SHARED_PTR<ObjRefDefinition> o2=RR_DYNAMIC_POINTER_CAST<ObjRefDefinition>(m2);
		if (o1)
		{
			if(!o2) return false;

			if (o1->ArrayType!=o2->ArrayType) return false;
			if (o1->ContainerType!=o2->ContainerType) return false;
			
			if (o1->ObjectType=="varobject" && o2->ObjectType=="varobject") return true;
						

			std::string st1="";
			std::string st2="";

			if (!boost::contains(o1->ObjectType,"."))
			{
				st1=d1->Name + "." + o1->ObjectType;
			}
			else 
			{
				st1=o1->ObjectType;
			}
			

			if (!boost::contains(o2->ObjectType,"."))
			{
				st2=d2->Name + "." + o2->ObjectType;
			}
			else 
			{
				st2=o2->ObjectType;
			}
			
			return st1==st2;
			
		}

		RR_SHARED_PTR<PipeDefinition> pp1=RR_DYNAMIC_POINTER_CAST<PipeDefinition>(m1);
		RR_SHARED_PTR<PipeDefinition> pp2=RR_DYNAMIC_POINTER_CAST<PipeDefinition>(m2);
		if (pp1)
		{
			if(!pp2) return false;
			
			return CompareTypeDefinition(d1,pp1->Type,d2,pp2->Type);
		}

		RR_SHARED_PTR<CallbackDefinition> c1=RR_DYNAMIC_POINTER_CAST<CallbackDefinition>(m1);
		RR_SHARED_PTR<CallbackDefinition> c2=RR_DYNAMIC_POINTER_CAST<CallbackDefinition>(m2);
		if (c1)
		{
			if(!c2) return false;
			if(!CompareTypeDefinition(d1,c1->ReturnType,d2,c2->ReturnType)) return false;
			return CompareTypeDefinitions(d1,c1->Parameters,d2,c2->Parameters);
		}

		RR_SHARED_PTR<WireDefinition> w1=RR_DYNAMIC_POINTER_CAST<WireDefinition>(m1);
		RR_SHARED_PTR<WireDefinition> w2=RR_DYNAMIC_POINTER_CAST<WireDefinition>(m2);
		if (w1)
		{
			if(!w2) return false;
			
			return CompareTypeDefinition(d1,w1->Type,d2,w2->Type);
		}

		RR_SHARED_PTR<MemoryDefinition> mem1=RR_DYNAMIC_POINTER_CAST<MemoryDefinition>(m1);
		RR_SHARED_PTR<MemoryDefinition> mem2=RR_DYNAMIC_POINTER_CAST<MemoryDefinition>(m2);
		if (mem1)
		{
			if(!mem2) return false;
			
			return CompareTypeDefinition(d1,mem1->Type,d2,mem2->Type);
		}

		return false;

	}

	void VerifyObject(RR_SHARED_PTR<ServiceEntryDefinition> obj, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > defs, std::vector<RobotRaconteurParseException>& warnings)
	{
		if (obj->EntryType != DataTypes_object_t) throw ServiceDefinitionException("Invalid EntryType in " + obj->Name);

		VerifyName(obj->Name,def);
	
		std::vector<std::string> membernames;

		BOOST_FOREACH(std::string& e, obj->Options)
		{
			std::vector<std::string> s1;
			boost::split(s1,e,boost::is_space());
			if (s1.at(0)=="constant")
			{
				std::string membername=VerifyConstant(e,def);
				if (boost::range::find(membernames, membername)!=membernames.end()) throw ServiceDefinitionException("Object \"" + obj->Name + "\" in service definition \"" + def->Name + "\" contains multiple members named \"" + membername + "\"");
				membernames.push_back(membername);
			}
		}

		if (!obj->Constants.empty())
		{
			VerifyVersionSupport(def, 0, 9, "Service definition standard version 0.9 or greater required for \"constant\" keyword");
		}

		BOOST_FOREACH(RR_SHARED_PTR<ConstantDefinition> e, obj->Constants)
		{			
			std::string membername = VerifyConstant(e, def, obj->Constants);
			if (boost::range::find(membernames, membername) != membernames.end()) throw ServiceDefinitionException("Object \"" + obj->Name + "\" in service definition \"" + def->Name + "\" contains multiple members named \"" + membername + "\"");
			membernames.push_back(membername);			
		}

		BOOST_FOREACH(RR_SHARED_PTR<MemberDefinition> e, obj->Members)
		{
			
			std::string membername=VerifyMember(e,def,defs, warnings);
			if (boost::range::find(membernames, membername)!=membernames.end()) throw ServiceDefinitionException("Object \"" + obj->Name + "\" in service definition \"" + def->Name + "\" contains multiple members named \"" + membername + "\"");
			membernames.push_back(membername);
		}

		rrimplements r=get_implements(obj,def,defs);

		BOOST_FOREACH (rrimplements& e, r.implements)
		{
			BOOST_FOREACH (RR_SHARED_PTR<MemberDefinition>& ee,e.obj->Members)
			{
				RR_SHARED_PTR<MemberDefinition> m2;
				BOOST_FOREACH (RR_SHARED_PTR<MemberDefinition>& ee2, obj->Members)
				{
					if (ee2->Name == ee->Name)
					{
						m2 = ee2;
						break;
					}
				}
				if (!m2) throw ServiceDefinitionException("Object \"" + obj->Name + "\" in service definition \"" + def->Name + "\" does not implement required member \"" + ee->Name + "\"");

				
				if (!CompareMember(m2,ee)) throw ServiceDefinitionException("Member \"" + ee->Name + "\" in object \"" + obj->Name + "\" in service definition \"" + def->Name + "\" does not match implemented member " + m2->ToString());
			}

		}

	}

	void VerifyStructure_check_recursion(RR_SHARED_PTR<ServiceEntryDefinition> strut, std::vector<RR_SHARED_PTR<ServiceDefinition> > defs, std::set<std::string> names, DataTypes entry_type)
	{
		if (strut->EntryType != entry_type && strut->EntryType != DataTypes_namedarray_t)
		{
			throw InternalErrorException("");
		}

		names.insert(strut->Name);

		BOOST_FOREACH(RR_SHARED_PTR<MemberDefinition>& e, strut->Members)
		{
			RR_SHARED_PTR<PropertyDefinition> p = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(e);
			if (!p) throw InternalErrorException("");

			if (p->Type->Type == DataTypes_namedtype_t)
			{
				RR_SHARED_PTR<NamedTypeDefinition> nt_def = p->Type->ResolveNamedType(defs);
				RR_SHARED_PTR<ServiceEntryDefinition> et_def = RR_DYNAMIC_POINTER_CAST<ServiceEntryDefinition>(nt_def);
				if (!et_def) throw InternalErrorException("");
				if (et_def->EntryType != entry_type && et_def->EntryType != DataTypes_namedarray_t) throw InternalErrorException("");

				if (names.find(et_def->Name) != names.end())
				{
					throw ServiceDefinitionException("Recursive namedarray/pod detected in " + strut->Name);
				}

				VerifyStructure_check_recursion(et_def, defs, names, entry_type);
			}
		}
	}

	void VerifyStructure_common(RR_SHARED_PTR<ServiceEntryDefinition> strut, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > defs, std::vector<RobotRaconteurParseException>& warnings, DataTypes entry_type)
	{
		if (strut->EntryType != entry_type) throw ServiceDefinitionException("Invalid EntryType in " + strut->Name);
		
		VerifyName(strut->Name,def);
		std::vector<std::string> membernames;

		BOOST_FOREACH(std::string& e, strut->Options)
		{
			std::vector<std::string> s1;
			boost::split(s1,e,boost::is_space());
			if (s1.at(0)=="constant")
			{
				std::string membername=VerifyConstant(e,def);
				if (boost::range::find(membernames, membername)!=membernames.end()) throw ServiceDefinitionException("Structure \"" + strut->Name + "\" in service definition \"" + def->Name + "\" contains multiple members named \"" + membername + "\"");
				membernames.push_back(membername);
			}
		}

		BOOST_FOREACH(RR_SHARED_PTR<ConstantDefinition> e, strut->Constants)
		{
			std::string membername = VerifyConstant(e, def, strut->Constants);
			if (boost::range::find(membernames, membername) != membernames.end()) throw ServiceDefinitionException("Struct \"" + strut->Name + "\" in service definition \"" + def->Name + "\" contains multiple members named \"" + membername + "\"");
			membernames.push_back(membername);
		}

		DataTypes namedarray_element_type = DataTypes_void_t;

		BOOST_FOREACH(RR_SHARED_PTR<MemberDefinition>& e, strut->Members)
		{
			RR_SHARED_PTR<PropertyDefinition> p=RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(e);
			if (!p) throw ServiceDefinitionException("Structure \"" + strut->Name + "\" can only contain fields in service definition \"" + def->Name + "\"");

			std::string membername = VerifyMember(p, def, defs, warnings);

			if (entry_type == DataTypes_pod_t)
			{
				RR_SHARED_PTR<TypeDefinition> t = p->Type;
				if (!IsTypeNumeric(t->Type) && t->Type != DataTypes_namedtype_t)
				{
					throw ServiceDefinitionException("Pods must only contain numeric, pod, and namedarray types");
				}

				if (t->Type == DataTypes_namedtype_t)
				{
					RR_SHARED_PTR<NamedTypeDefinition> tt = t->ResolveNamedType(defs);
					if (tt->RRDataType() != DataTypes_pod_t && tt->RRDataType() != DataTypes_namedarray_t)
					{
						throw ServiceDefinitionException("Pods must only contain numeric, custruct, pod types");
					}
				}

				if (t->ContainerType != DataTypes_ContainerTypes_none)
				{
					throw ServiceDefinitionException("Pods may not use containers");
				}

				if ((boost::range::find(t->ArrayLength, 0) != t->ArrayLength.end()) 
					|| (t->ArrayType == DataTypes_ArrayTypes_multidimarray && t->ArrayLength.empty()))
				{
					throw ServiceDefinitionException("Pods must have fixed or finite length arrays");
				}								
			}
			
			if (entry_type == DataTypes_namedarray_t)
			{
				RR_SHARED_PTR<TypeDefinition> t = p->Type;
				if (!IsTypeNumeric(t->Type) && t->Type != DataTypes_namedtype_t)
				{
					throw ServiceDefinitionException("NamedArrays must only contain numeric and namedarray types: " + e->Name);
				}

				if (t->Type == DataTypes_namedtype_t)
				{
					RR_SHARED_PTR<NamedTypeDefinition> tt = t->ResolveNamedType(defs);
					if (tt->RRDataType() != DataTypes_namedarray_t)
					{
						throw ServiceDefinitionException("NamedArrays must only contain numeric and namedarray types: " + e->Name);
					}
				}

				if (t->ContainerType != DataTypes_ContainerTypes_none)
				{
					throw ServiceDefinitionException("NamedArrays may not use containers: " + e->Name);
				}

				switch (t->ArrayType)
				{
				case DataTypes_ArrayTypes_none:
					break;
				case DataTypes_ArrayTypes_array:
					if (t->ArrayVarLength) throw ServiceDefinitionException("NamedArray fields must be scalars or fixed arrays");
					break;
				default:
					throw ServiceDefinitionException("NamedArray fields must be scalars or fixed arrays");
				}
								
				std::set<std::string> n;
			}
					
			if (boost::range::find(membernames, membername)!=membernames.end()) throw ServiceDefinitionException("Structure \"" + strut->Name + "\" in service definition \"" + def->Name + "\" contains multiple members named \"" + membername + "\"");
			membernames.push_back(membername);
		}

		if (entry_type == DataTypes_pod_t)
		{
			std::set<std::string> n;
			VerifyStructure_check_recursion(strut, defs, n, DataTypes_pod_t);
		}

		if (entry_type == DataTypes_namedarray_t)
		{
			GetNamedArrayElementTypeAndCount(strut, defs);
		}

	}

	void VerifyStructure(RR_SHARED_PTR<ServiceEntryDefinition> strut, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > defs, std::vector<RobotRaconteurParseException>& warnings)
	{
		VerifyStructure_common(strut, def, defs, warnings, DataTypes_structure_t);
	}

	void VerifyPod(RR_SHARED_PTR<ServiceEntryDefinition> strut, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > defs, std::vector<RobotRaconteurParseException>& warnings)
	{
		VerifyStructure_common(strut, def, defs, warnings, DataTypes_pod_t);
	}

	void VerifyNamedArray(RR_SHARED_PTR<ServiceEntryDefinition> strut, RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > defs, std::vector<RobotRaconteurParseException>& warnings)
	{
		VerifyStructure_common(strut, def, defs, warnings, DataTypes_namedarray_t);
	}


	struct rrimports
	{
		RR_SHARED_PTR<ServiceDefinition> def;
		std::vector<rrimports> imported;
	};

	
	rrimports get_imports(RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs,RR_SHARED_PTR<ServiceDefinition> rootdef=RR_SHARED_PTR<ServiceDefinition>())
	{
		rrimports out;
		out.def=def;
		if (def->Imports.size()==0) return out;

		if (!rootdef) rootdef=def;

		BOOST_FOREACH (std::string& e, def->Imports)
		{
			RR_SHARED_PTR<ServiceDefinition> def2;

			BOOST_FOREACH (RR_SHARED_PTR<ServiceDefinition>& ee, defs)
			{
				if (ee->Name == e)
				{
					def2 = ee;
					break;
				}
			}

			/*if (!def2)
			{
				std::vector<std::string> registerednames=RobotRaconteurNode::s()->GetRegisteredServiceTypes();
				if (std::count(registerednames.begin(),registerednames.end(),*e)!=0)
				{
					def2=RobotRaconteurNode::s()->GetServiceType(*e)->ServiceDef();
					defs.push_back(def2);

				}
			}*/

			if (!def2) throw ServiceDefinitionException("Service definition \"" + e + "\" not found.");

			if (def2->Name==rootdef->Name) throw ServiceDefinitionException("Recursive imports between \"" + def->Name + "\" and \"" + rootdef->Name + "\"");

			rrimports imp2=get_imports(def2,defs,rootdef);
			out.imported.push_back(imp2);

		}

		
		return out;

	}


	void VerifyImports(RR_SHARED_PTR<ServiceDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs)
	{
		rrimports c=get_imports(def,defs);


	}

	ROBOTRACONTEUR_CORE_API void VerifyServiceDefinitions(std::vector<RR_SHARED_PTR<ServiceDefinition> > def, std::vector<RobotRaconteurParseException>& warnings)
	{
		BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition>& e, def)
		{
			e->CheckVersion();

			if (!boost::starts_with(e->Name,"RobotRaconteurTestService") && !(e->Name=="RobotRaconteurServiceIndex"))
				VerifyName(e->Name,e,true);

			if (boost::ends_with(e->Name,"_signed")) throw ServiceDefinitionException("Service definition names ending with \"_signed\" are reserved");

			VerifyImports(e,def);

			std::vector<std::string> names;
			BOOST_FOREACH(std::string& ee, e->Options)
			{
				std::vector<std::string> s1;
				boost::split(s1,ee,boost::is_space());
				if (s1.at(0)=="constant")
				{
					std::string name=VerifyConstant(ee,e);
					if (boost::range::find(names, name)!=names.end()) throw ServiceDefinitionException("Service definition \"" + e->Name + "\" contains multiple high level names \"" + name + "\"");
					names.push_back(name);
				}
			}

			if (!e->Constants.empty())
			{
				VerifyVersionSupport(e, 0, 9, "Service definition standard version 0.9 or greater required for \"constant\" keyword");
			}
			
			BOOST_FOREACH(RR_SHARED_PTR<ConstantDefinition> ee, e->Constants)
			{
				std::string name = VerifyConstant(ee, e, e->Constants);
				if (boost::range::find(names, name) != names.end()) throw ServiceDefinitionException("Service definition \"" + e->Name + "\" contains multiple high level names \"" + name + "\"");
				names.push_back(name);
			}

			BOOST_FOREACH(std::string& ee, e->Exceptions)
			{
					VerifyName(ee,e);
					std::string name=ee;
					if (boost::range::find(names, name)!=names.end()) throw ServiceDefinitionException("Service definition \"" + e->Name + "\" contains multiple high level names \"" + name + "\"");
					names.push_back(name);				
			}

			std::vector<RR_SHARED_PTR<ServiceDefinition> > importeddefs;
			BOOST_FOREACH(std::string& ee, e->Imports)
			{
				BOOST_FOREACH (RR_SHARED_PTR<ServiceDefinition>& ee2, def)
				{
					if (ee == ee2->Name)
					{
						importeddefs.push_back(ee2);
					}

				}
			}

			if (e->StdVer)
			{
				BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition> ee, importeddefs)
				{
					if (!ee->StdVer || ee->StdVer > e->StdVer)
					{
						throw ServiceDefinitionException("Imported service definition " + ee->Name + " has a higher Service Definition standard version than " + e->Name);
					}
				}
			}

			if (!e->Using.empty())
			{
				VerifyVersionSupport(e, 0, 9, "Service definition standard version 0.9 or greater required for \"using\" keyword");
			}

			BOOST_FOREACH(RR_SHARED_PTR<UsingDefinition>& ee, e->Using)
			{
				std::string name = ee->UnqualifiedName;
				if (boost::range::find(names, name) != names.end()) throw ServiceDefinitionException("Service definition \"" + e->Name + "\" contains multiple high level names \"" + name + "\"");
				VerifyUsing(*ee, e, importeddefs);
				names.push_back(name);

			}

			if (!e->Enums.empty())
			{
				VerifyVersionSupport(e, 0, 9, "Service definition standard version 0.9 or greater required for \"enum\" keyword");
			}

			BOOST_FOREACH(RR_SHARED_PTR<EnumDefinition>& ee, e->Enums)
			{
				VerifyEnum(*ee, e);
				std::string name = ee->Name;
				if (boost::range::find(names, name) != names.end()) throw ServiceDefinitionException("Service definition \"" + e->Name + "\" contains multiple high level names \"" + name + "\"");
				names.push_back(name);
			}

			BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition> ee, e->Structures)
			{
				
				VerifyStructure(ee,e,importeddefs, warnings);

				std::string name=ee->Name;
				if (boost::range::find(names, name)!=names.end()) throw ServiceDefinitionException("Service definition \"" + e->Name + "\" contains multiple high level names \"" + name + "\"");
				names.push_back(name);
				
			}

			BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition> ee, e->Pods)
			{

				VerifyPod(ee, e, importeddefs, warnings);

				std::string name = ee->Name;
				if (boost::range::find(names, name) != names.end()) throw ServiceDefinitionException("Service definition \"" + e->Name + "\" contains multiple high level names \"" + name + "\"");
				names.push_back(name);

			}

			BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition> ee, e->NamedArrays)
			{

				VerifyNamedArray(ee, e, importeddefs, warnings);

				std::string name = ee->Name;
				if (boost::range::find(names, name) != names.end()) throw ServiceDefinitionException("Service definition \"" + e->Name + "\" contains multiple high level names \"" + name + "\"");
				names.push_back(name);

			}

			BOOST_FOREACH(RR_SHARED_PTR<ServiceEntryDefinition>& ee, e->Objects)
			{
				
				VerifyObject(ee,e,importeddefs, warnings);

				std::string name=ee->Name;
				if (boost::range::find(names, name)!=names.end()) throw ServiceDefinitionException("Service definition \"" + e->Name + "\" contains multiple high level names \"" + name + "\"");
				names.push_back(name);
				
			}

		}

	}

	ROBOTRACONTEUR_CORE_API void VerifyServiceDefinitions(std::vector<RR_SHARED_PTR<ServiceDefinition> > def)
	{
		std::vector<RobotRaconteurParseException> warnings;
		VerifyServiceDefinitions(def, warnings);

	}

	bool CompareConstantDefinition(RR_SHARED_PTR<ServiceDefinition> service1, RR_SHARED_PTR<ConstantDefinition>& d1, RR_SHARED_PTR<ServiceDefinition> service2, RR_SHARED_PTR<ConstantDefinition>& d2)
	{
		if (d1->Name != d2->Name) return false;
		if (!CompareTypeDefinition(service1, d1->Type, service2, d2->Type)) return false;
		if (boost::trim_copy(d1->Value) != boost::trim_copy(d2->Value)) return false;
		return true;
	}

	ROBOTRACONTEUR_CORE_API bool CompareServiceEntryDefinition(RR_SHARED_PTR<ServiceDefinition> service1, RR_SHARED_PTR<ServiceEntryDefinition>& d1, RR_SHARED_PTR<ServiceDefinition> service2, RR_SHARED_PTR<ServiceEntryDefinition>& d2)
	{
		if (d1->Name != d2->Name) return false;
		if (d1->EntryType != d2->EntryType) return false;
		if (!boost::range::equal(d1->Implements, d2->Implements)) return false;
		if (!boost::range::equal(d1->Options, d2->Options)) return false;
		if (d1->Constants.size() != d2->Constants.size()) return false;
		for (size_t i = 0; i < d1->Constants.size(); i++)
		{
			if (!CompareConstantDefinition(service1, d1->Constants[i], service2, d2->Constants[i]))
				return false;
		}

		if (d1->Members.size() != d2->Members.size()) return false;
		for (size_t i = 0; i < d1->Members.size(); i++)
		{
			if (!CompareMember(d1->Members[i], d2->Members[i]))
				return false;
		}
		return true;
	}

	bool CompareUsingDefinition(RR_SHARED_PTR<UsingDefinition> u1, RR_SHARED_PTR<UsingDefinition> u2)
	{
		if (u1->QualifiedName != u2->QualifiedName) return false;
		if (u1->UnqualifiedName != u2->UnqualifiedName) return false;
		return true;
	}

	bool CompareEnumDefinition(RR_SHARED_PTR<EnumDefinition> enum1, RR_SHARED_PTR<EnumDefinition> enum2)
	{
		if (enum1->Name != enum2->Name) return false;
		if (enum1->Values.size() != enum2->Values.size()) return false;
		for (size_t i=0; i< enum1->Values.size(); i++)
		{
			if (enum1->Values[i].Name != enum2->Values[i].Name) return false;
			if (enum1->Values[i].Value != enum2->Values[i].Value) return false;
			if (enum1->Values[i].ImplicitValue != enum2->Values[i].ImplicitValue) return false;
			if (enum1->Values[i].HexValue != enum2->Values[i].HexValue) return false;
		}
		return true;
	}

	ROBOTRACONTEUR_CORE_API bool CompareServiceDefinitions(RR_SHARED_PTR<ServiceDefinition> service1, RR_SHARED_PTR<ServiceDefinition> service2)
	{
		if (service1->Name != service2->Name) return false;
		if (!boost::range::equal(service1->Imports, service2->Imports)) return false;
		if (!boost::range::equal(service1->Options, service2->Options)) return false;
		
		if (service1->Using.size() != service2->Using.size()) return false;
		for (size_t i = 0; i < service1->Using.size(); i++)
		{
			if (!CompareUsingDefinition(service1->Using[i], service2->Using[i]))
				return false;
		}

		if (service1->Constants.size() != service2->Constants.size()) return false;
		for (size_t i = 0; i < service1->Constants.size(); i++)
		{
			if (!CompareConstantDefinition(service1, service1->Constants[i], service2, service2->Constants[i]))
				return false;
		}

		if (service1->Enums.size() != service2->Enums.size()) return false;
		for (size_t i = 0; i < service1->Enums.size(); i++)
		{
			if (!CompareEnumDefinition(service1->Enums[i], service2->Enums[i]))
				return false;
		}
		
		if (service1->StdVer != service2->StdVer) return false;

		if (service1->Objects.size() != service2->Objects.size()) return false;
		for (size_t i = 0; i < service1->Objects.size(); i++)
		{
			if (!CompareServiceEntryDefinition(service1, service1->Objects[i], service2, service2->Objects[i]))
				return false;
		}

		if (service1->Structures.size() != service2->Structures.size()) return false;
		for (size_t i = 0; i < service1->Structures.size(); i++)
		{
			if (!CompareServiceEntryDefinition(service1, service1->Structures[i], service2, service2->Structures[i]))
				return false;
		}

		if (service1->Pods.size() != service2->Pods.size()) return false;
		for (size_t i = 0; i < service1->Pods.size(); i++)
		{
			if (!CompareServiceEntryDefinition(service1, service1->Pods[i], service2, service2->Pods[i]))
				return false;
		}

		return true;
	}
	

	ROBOTRACONTEUR_CORE_API boost::tuple<std::string, std::string> SplitQualifiedName(const std::string& name)
	{
		size_t pos=name.find_last_of('.');

		if (pos==std::string::npos) throw InvalidArgumentException("Name is not qualified");

		return boost::make_tuple(name.substr(0,pos),name.substr(pos+1,name.size()-pos-1));

	}

	ROBOTRACONTEUR_CORE_API size_t EstimatePodPackedElementSize(RR_SHARED_PTR<ServiceEntryDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs, RR_SHARED_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<RRObject> client)
	{
		size_t s = 16;
		s += ArrayBinaryWriter::GetStringByteCount8(def->Name);
		BOOST_FOREACH(RR_SHARED_PTR<MemberDefinition> m, def->Members)
		{
			RR_SHARED_PTR<PropertyDefinition> p = rr_cast<PropertyDefinition>(m);
			if (IsTypeNumeric(p->Type->Type))
			{
				s += 16;
				s += ArrayBinaryWriter::GetStringByteCount8(p->Name);
				size_t array_count;
				if (p->Type->ArrayType == DataTypes_ArrayTypes_none)
				{
					array_count = 1;
				}
				else
				{
					array_count = boost::numeric_cast<size_t>(boost::accumulate(p->Type->ArrayLength, 1, std::multiplies<int32_t>()));
				}
				s += RRArrayElementSize(p->Type->Type) * array_count;
			}
			else
			{
				RR_SHARED_PTR<ServiceEntryDefinition> nt = rr_cast<ServiceEntryDefinition>(p->Type->ResolveNamedType(other_defs,node,client));
				s += 16;
				s += ArrayBinaryWriter::GetStringByteCount8(p->Name);
				s += ArrayBinaryWriter::GetStringByteCount8(nt->ResolveQualifiedName());
				size_t array_count;
				if (p->Type->ArrayType == DataTypes_ArrayTypes_none)
				{
					array_count = 1;
				}
				else
				{
					array_count = boost::numeric_cast<size_t>(boost::accumulate(p->Type->ArrayLength, 1, std::multiplies<int32_t>()));
				}
				s += EstimatePodPackedElementSize(nt,other_defs,node,client) * array_count;
			}
		}
		return s;
	}

	boost::tuple<DataTypes, size_t> GetNamedArrayElementTypeAndCount(RR_SHARED_PTR<ServiceEntryDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs, RR_SHARED_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<RRObject> client, std::set<std::string> n)
	{
		if (def->EntryType != DataTypes_namedarray_t)
		{
			throw InvalidOperationException("Argument must be an namedarray");
		}

		n.insert(def->Name);

		DataTypes element_type = DataTypes_void_t;
		size_t element_count = 0;

		if (def->Members.empty())
		{
			throw ServiceDefinitionException("namedarray must not be empty");
		}

		BOOST_FOREACH(RR_SHARED_PTR<MemberDefinition>& e, def->Members)
		{
			size_t field_element_count = 1;

			RR_SHARED_PTR<PropertyDefinition> p = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(e);
			if (!p) throw ServiceDefinitionException("Invalid member type in namedarray: " + def->Name);

			if (p->Type->ContainerType != DataTypes_ContainerTypes_none)
			{
				throw ServiceDefinitionException("namedarray must not contain containers: " + def->Name);
			}

			if (p->Type->ArrayType != DataTypes_ArrayTypes_none && p->Type->ArrayVarLength)
			{
				throw ServiceDefinitionException("namedarray must not contain variable length arrays: " + def->Name);
			}

			if (p->Type->ArrayType != DataTypes_ArrayTypes_none)
			{
				field_element_count = boost::numeric_cast<size_t>(boost::accumulate(p->Type->ArrayLength, 1, std::multiplies<int32_t>()));
			}

			if (IsTypeNumeric(p->Type->Type))
			{
				if (element_type == DataTypes_void_t)
				{
					element_type = p->Type->Type;
				}
				else
				{			
					if (element_type !=p->Type->Type) throw ServiceDefinitionException("namedarray must contain same numeric type: " + def->Name);
				}

				element_count += field_element_count;
			}
			else if (p->Type->Type == DataTypes_namedtype_t)
			{
				RR_SHARED_PTR<NamedTypeDefinition> nt_def = p->Type->ResolveNamedType(other_defs, node, client);
				RR_SHARED_PTR<ServiceEntryDefinition> et_def = RR_DYNAMIC_POINTER_CAST<ServiceEntryDefinition>(nt_def);
				if (!et_def) throw InternalErrorException("");
				if (et_def->EntryType != DataTypes_namedarray_t) throw InternalErrorException("");

				if (n.find(et_def->Name) != n.end())
				{
					throw ServiceDefinitionException("Recursive namedarray detected in " + def->Name);
				}

				boost::tuple<DataTypes, size_t> v=GetNamedArrayElementTypeAndCount(et_def, other_defs, node, client, n);
				if (element_type == DataTypes_void_t)
				{
					element_type = v.get<0>();
				}
				else
				{
					if (element_type != v.get<0>()) throw ServiceDefinitionException("namedarray must contain same numeric type: " + def->Name);
				}

				element_count += field_element_count * v.get<1>();

			}
			else
			{
				throw ServiceDefinitionException("Invalid namedarray field in " + def->Name);
			}
		}

		return boost::make_tuple(element_type, element_count);

	}


	ROBOTRACONTEUR_CORE_API boost::tuple<DataTypes,size_t> GetNamedArrayElementTypeAndCount(RR_SHARED_PTR<ServiceEntryDefinition> def, std::vector<RR_SHARED_PTR<ServiceDefinition> > other_defs, RR_SHARED_PTR<RobotRaconteurNode> node, RR_SHARED_PTR<RRObject> client)
	{
		std::set<std::string> n;
		return GetNamedArrayElementTypeAndCount(def, other_defs, node, client, n);
	}

}
