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
#include "RobotRaconteur/RobotRaconteurNode.h"
#include "RobotRaconteur/IOUtils.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/foreach.hpp>
#include <boost/range/combine.hpp>
#include <boost/range/numeric.hpp>
#include <boost/tuple/tuple_comparison.hpp>

#ifdef ROBOTRACONTEUR_EMSCRIPTEN
#include <uchar.h>
#endif

using namespace boost::algorithm;

namespace RobotRaconteur
{
typedef boost::match_results<boost::string_ref::const_iterator> ref_match;

static boost::string_ref to_ref(const ref_match::value_type& s) { return boost::string_ref(s.first, s.length()); }

/*static boost::string_ref to_ref(const boost::smatch::value_type& s)
{
    return boost::string_ref((const char*)s.first,s.length());
}*/

RobotRaconteurVersion::RobotRaconteurVersion()
{
    major = 0;
    minor = 0;
    patch = 0;
    tweak = 0;
}

RobotRaconteurVersion::RobotRaconteurVersion(uint32_t major, uint32_t minor, uint32_t patch, uint32_t tweak)
{
    this->major = major;
    this->minor = minor;
    this->patch = patch;
    this->tweak = tweak;
}

RobotRaconteurVersion::RobotRaconteurVersion(boost::string_ref v)
{
    major = 0;
    minor = 0;
    patch = 0;
    tweak = 0;
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
void RobotRaconteurVersion::FromString(boost::string_ref v, const ServiceDefinitionParseInfo* parse_info)
{
    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    static boost::regex r("^(\\d+)\\.(\\d+)(?:\\.(\\d+)(?:\\.(\\d+))?)?$");

    ref_match r_match;
    if (!boost::regex_match(v.begin(), v.end(), r_match, r))
    {
        throw ServiceDefinitionParseException(
            "Format error for version definition \"" + boost::trim_copy(v.to_string()) + "\"", ParseInfo);
    }

    major = boost::lexical_cast<uint32_t>(r_match[1].str());
    minor = boost::lexical_cast<uint32_t>(r_match[2].str());
    patch = r_match[3].matched ? boost::lexical_cast<uint32_t>(r_match[3].str()) : 0;
    tweak = r_match[4].matched ? boost::lexical_cast<uint32_t>(r_match[4].str()) : 0;
}

bool operator==(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2)
{
    return boost::tie(v1.major, v1.minor, v1.patch, v1.tweak) == boost::tie(v2.major, v2.minor, v2.patch, v2.tweak);
}
bool operator!=(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2)
{
    return boost::tie(v1.major, v1.minor, v1.patch, v1.tweak) != boost::tie(v2.major, v2.minor, v2.patch, v2.tweak);
}
bool operator>(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2)
{
    if (v1.major > v2.major)
        return true;
    if (v1.major < v2.major)
        return false;
    if (v1.minor > v2.minor)
        return true;
    if (v1.minor < v2.minor)
        return false;
    if (v1.patch > v2.patch)
        return true;
    if (v1.patch < v2.patch)
        return false;
    if (v1.tweak > v2.tweak)
        return true;
    return false;
}
bool operator>=(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2)
{
    if (boost::tie(v1.major, v1.minor, v1.patch, v1.tweak) == boost::tie(v2.major, v2.minor, v2.patch, v2.tweak))
        return true;
    return v1 > v2;
}
bool operator<(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2)
{
    if (v1.major < v2.major)
        return true;
    if (v1.major > v2.major)
        return false;
    if (v1.minor < v2.minor)
        return true;
    if (v1.minor > v2.minor)
        return false;
    if (v1.patch < v2.patch)
        return true;
    if (v1.patch > v2.patch)
        return false;
    if (v1.tweak < v2.tweak)
        return true;
    return false;
}
bool operator<=(const RobotRaconteurVersion& v1, const RobotRaconteurVersion& v2)
{
    if (boost::tie(v1.major, v1.minor, v1.patch, v1.tweak) == boost::tie(v2.major, v2.minor, v2.patch, v2.tweak))
        return true;
    return v1 < v2;
}

RobotRaconteurVersion::operator bool() const { return *this != RobotRaconteurVersion(); }

#define RR_NAME_REGEX "[a-zA-Z](?:\\w*[a-zA-Z0-9])?"
#define RR_TYPE_REGEX "(?:[a-zA-Z](?:\\w*[a-zA-Z0-9])?)(?:\\.[a-zA-Z](?:\\w*[a-zA-Z0-9])?)*"
#define RR_QUAIFIED_TYPE_REGEX "(?:[a-zA-Z](?:\\w*[a-zA-Z0-9])?)(?:\\.[a-zA-Z](?:\\w*[a-zA-Z0-9])?)+"
#define RR_TYPE2_REGEX                                                                                                 \
    "(?:[a-zA-Z](?:\\w*[a-zA-Z0-9])?)(?:\\.[a-zA-Z](?:\\w*[a-zA-Z0-9])?)*(?:\\[[0-9\\,\\*\\-]*\\])?(?:\\{\\w{1,16}\\}" \
    ")?"
#define RR_INT_REGEX "[+\\-]?(?:0|[1-9]\\d*)"
#define RR_HEX_REGEX "[+\\-]?0x[\\da-fA-F]+"
#define RR_FLOAT_REGEX "[+\\-]?(?:(?:0|[1-9]\\d*)(?:\\.\\d*)?|(?:\\.\\d+))(?:[eE][+\\-]?\\d+)?"
#define RR_NUMBER_REGEX "(?:" RR_INT_REGEX "|" RR_HEX_REGEX "|" RR_FLOAT_REGEX ")"

static void ServiceDefinition_FromStringFormat_common(const boost::regex& r, boost::string_ref l,
                                                      boost::string_ref keyword, std::vector<std::string>& vec,
                                                      ServiceDefinitionParseInfo& parse_info)
{
    ref_match r_match;
    if (!boost::regex_match(l.begin(), l.end(), r_match, r))
    {
        throw ServiceDefinitionParseException(
            "Format error for " + keyword + " definition \"" + boost::trim_copy(l.to_string()) + "\"", parse_info);
    }

    if (r_match[1] != keyword.begin())
    {
        throw ServiceDefinitionParseException(
            "Format error for " + keyword + " definition \"" + boost::trim_copy(l.to_string()) + "\"", parse_info);
    }
    vec.push_back(r_match[2]);
}

static void ServiceDefinition_FromStringImportFormat(boost::string_ref l, boost::string_ref keyword,
                                                     std::vector<std::string>& vec,
                                                     ServiceDefinitionParseInfo& parse_info)
{
    static boost::regex r("^[ \\t]*(\\w{1,16})[ \\t]+(" RR_TYPE_REGEX ")[ \\t]*$");
    ServiceDefinition_FromStringFormat_common(r, l, keyword, vec, parse_info);
}

static void ServiceDefinition_FromStringTypeFormat(boost::string_ref l, boost::string_ref keyword,
                                                   std::vector<std::string>& vec,
                                                   ServiceDefinitionParseInfo& parse_info)
{
    static boost::regex r("^[ \\t]*(\\w{1,16})[ \\t]+(" RR_TYPE_REGEX ")[ \\t]*$");
    ServiceDefinition_FromStringFormat_common(r, l, keyword, vec, parse_info);
}

template <typename T>
static bool ServiceDefinition_GetLine(T& is, std::string& l, std::string& docstring,
                                      ServiceDefinitionParseInfo& parse_info, bool return_docstring = false)
{
    static boost::regex r_docstring("^[ \\t]*##([ -~\\t]*)$");
    static boost::regex r_comment("^[ \\t]*#(?!#)[ -~\\t]*$");
    static boost::regex r_empty("^[ \\t]*$");
    static boost::regex r_valid("^[ -~\\t]*$");

    std::string l2;
    std::vector<std::string> docstring_v;

    bool empty_encountered_docstring = false;

    while (true)
    {
        if (!std::getline(is, l2))
            return false;
        parse_info.LineNumber++;
        parse_info.Line.clear();

        boost::trim_right_if(l2, boost::is_any_of("\r"));

        if (l2.find('\0') != std::string::npos)
            throw ServiceDefinitionParseException("Service definition must not contain null characters", parse_info);

        boost::smatch r_docstring_match;
        if (boost::regex_match(l2, r_docstring_match, r_docstring))
        {
            if (!return_docstring)
            {
                if (empty_encountered_docstring)
                {
                    empty_encountered_docstring = false;
                    docstring_v.clear();
                }
                docstring_v.push_back(r_docstring_match[1].str());
                continue;
            }
        }

        if (boost::regex_match(l2, r_comment))
        {
            docstring_v.clear();
            continue;
        }

        if (boost::regex_match(l2, r_empty))
        {
            empty_encountered_docstring = true;
            continue;
        }

        while (boost::ends_with(l2, "\\"))
        {
            if (l2.empty())
                throw InternalErrorException("Internal parsing error");
            *l2.rbegin() = ' ';
            std::string l3;
            if (!std::getline(is, l3))
                throw ServiceDefinitionParseException("Service definition line continuation must not be on last line",
                                                      parse_info);
            boost::trim_right_if(l3, boost::is_any_of("\r"));

            if (l3.find('\0') != std::string::npos)
                throw ServiceDefinitionParseException("Service definition must not contain null characters",
                                                      parse_info);
            l2 += l3;
        }

        if (!boost::regex_match(l2, r_valid))
        {
            throw ServiceDefinitionParseException("Service definition must contain only ASCII characters", parse_info);
        }

        l.swap(l2);

        parse_info.Line = l;

        if (docstring_v.empty())
        {
            docstring.clear();
        }
        else
        {
            docstring = boost::join(docstring_v, "\n");
        }
        return true;
    }
}

static void ServiceDefinition_FindBlock(boost::string_ref current_line, std::istream& is, std::ostream& os,
                                        ServiceDefinitionParseInfo& parse_info,
                                        ServiceDefinitionParseInfo& init_parse_info,
                                        const RobotRaconteurVersion& stdver)
{
    static boost::regex r_start("^[ \\t]*(\\w{1,16})[ \\t]+(" RR_NAME_REGEX ")[ \\t]*$");
    static boost::regex r_end("^[ \\t]*end(?:[ \\t]+(\\w{1,16}))?[ \\t]*$");

    init_parse_info = parse_info;

    ref_match r_start_match;
    if (!boost::regex_match(current_line.begin(), current_line.end(), r_start_match, r_start))
    {
        throw ServiceDefinitionParseException("Parse error", parse_info);
    }

    os << current_line << "\n";

    std::string block_type = r_start_match[1];
    std::string l;

    size_t last_pos = parse_info.LineNumber;

    std::string docstring;
    while (ServiceDefinition_GetLine(is, l, docstring, parse_info, true))
    {
        last_pos++;
        for (; last_pos < parse_info.LineNumber; last_pos++)
        {
            os << "\n";
        }

        os << l << "\n";

        boost::smatch r_end_match;
        if (boost::regex_match(l, r_end_match, r_end))
        {
            if (r_end_match[1].matched)
            {
                if (stdver >= RobotRaconteurVersion(0, 9, 2))
                {
                    throw ServiceDefinitionParseException(
                        "end keyword must be only keyword on line in stdver 0.9.2 and greater", parse_info);
                }

                if (r_end_match[1] != block_type)
                {
                    throw ServiceDefinitionParseException("Block end does not match start", parse_info);
                }
            }

            return;
        }
    }

    throw ServiceDefinitionParseException("Block end not found", init_parse_info);
}

std::string ServiceDefinition::ToString()
{
    std::ostringstream o;
    ToStream(o);
    return o.str();
}

void ServiceDefinition::ToStream(std::ostream& o) const
{
    o << "service " << Name << "\n\n";

    if (StdVer)
    {
        bool version_found = false;
        BOOST_FOREACH (const std::string& so, Options)
        {
            static boost::regex r_version("^[ \\t]*version[ \\t]+(?:(\\d+(?:\\.\\d+)*)|[ -~\\t]*)$");
            boost::smatch r_version_match;
            if (boost::regex_match(so, r_version_match, r_version))
            {
                if (version_found)
                    throw ServiceDefinitionParseException("Robot Raconteur version already specified");
                if (r_version_match[1].matched)
                {
                    version_found = true;
                    break;
                }
                else
                {
                    throw ServiceDefinitionParseException("Invalid Robot Raconteur version specified");
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

    BOOST_FOREACH (const std::string& import, Imports)
    {
        o << "import " << import << "\n";
    }

    if (!Imports.empty())
        o << "\n";

    BOOST_FOREACH (const RR_SHARED_PTR<UsingDefinition>& u, Using)
    {
        o << u->ToString();
    }

    if (!Using.empty())
        o << "\n";

    BOOST_FOREACH (const std::string& option, Options)
    {
        o << "option " << option << "\n"; // replace_all_copy((*option),"\"", "\"\"") + "\n";
    }

    if (!Options.empty())
        o << "\n";

    BOOST_FOREACH (const RR_SHARED_PTR<ConstantDefinition>& c, Constants)
    {
        o << c->ToString() << "\n";
    }

    if (!Constants.empty())
        o << "\n";

    BOOST_FOREACH (const RR_SHARED_PTR<EnumDefinition>& e, Enums)
    {
        o << e->ToString() << "\n";
    }

    if (!Enums.empty())
        o << "\n";

    BOOST_FOREACH (const RR_SHARED_PTR<ExceptionDefinition>& exception, Exceptions)
    {
        o << exception->ToString() << "\n"; // replace_all_copy((*option),"\"", "\"\"") + "\n";
    }

    if (!Exceptions.empty())
        o << "\n";

    BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& d, Structures)
    {
        o << d->ToString() << "\n";
    }

    BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& d, Pods)
    {
        o << d->ToString() << "\n";
    }

    BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& d, NamedArrays)
    {
        o << d->ToString() << "\n";
    }

    BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& d, Objects)
    {
        o << d->ToString() << "\n";
    }
}

void ServiceDefinition::CheckVersion(const RobotRaconteurVersion& ver) const
{
    if (!StdVer)
        return;
    RobotRaconteurVersion ver2 = ver;
    if (ver2 == RobotRaconteurVersion(0, 0))
    {
        ver2 = RobotRaconteurVersion(ROBOTRACONTEUR_VERSION_TEXT);
    }

    if (ver2 < StdVer)
    {
        throw ServiceException("Service " + Name + " requires newer version of Robot Raconteur");
    }
}

void ServiceDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    std::vector<ServiceDefinitionParseException> w;
    FromString(s, w, parse_info);
}

void ServiceDefinition::FromStream(std::istream& is, const ServiceDefinitionParseInfo* parse_info)
{
    std::vector<ServiceDefinitionParseException> w;
    FromStream(is, w, parse_info);
}

void ServiceDefinition::FromString(boost::string_ref s1, std::vector<ServiceDefinitionParseException>& warnings,
                                   const ServiceDefinitionParseInfo* parse_info)
{
    std::istringstream s(s1.to_string());
    FromStream(s, warnings, parse_info);
}

// TODO: Test ignore_invalid_member
void ServiceDefinition::FromStream(std::istream& s, std::vector<ServiceDefinitionParseException>& warnings,
                                   const ServiceDefinitionParseInfo* parse_info)
{
    Reset();

    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    static boost::regex r_comment("^[ \\t]*#[ -~\\t]*$");
    static boost::regex r_empty("^[ \\t]*$");
    static boost::regex r_entry(
        "(?:^[ "
        "\\t]*(?:(service)|(stdver)|(option)|(import)|(using)|(exception)|(constant)|(enum)|(struct)|(object)|(pod)|("
        "namedarray))[ \\t]+(\\w[^\\s]*(?:[ \\t]+[^\\s]+)*)[ \\t]*$)|(^[ \\t]*$)");

    bool service_name_found = false;

    ServiceDefinitionParseInfo working_info = ParseInfo;
    std::string l;

    RobotRaconteurVersion stdver_version;
    bool stdver_found = false;

    int32_t entry_key_max = 0;

    bool first_line = false;

    try
    {
        while (true)
        {
            std::string docstring;
            if (!ServiceDefinition_GetLine(s, l, docstring, working_info))
            {
                break;
            }

            if (first_line)
            {
                first_line = true;
                ParseInfo.Line = l;
            }

            ref_match r_entry_match;
            boost::string_ref l2(l);
            if (!boost::regex_match(l2.begin(), l2.end(), r_entry_match, r_entry))
            {
                throw ServiceDefinitionParseException("Parse error", working_info);
            }

            const ref_match::value_type& r_entry_match_blank = r_entry_match[14];
            if (r_entry_match_blank.matched)
                continue;

            int32_t entry_key = 1;
            for (; entry_key < 12; entry_key++)
            {
                if (r_entry_match[entry_key].matched)
                    break;
            }

            const ref_match::value_type& r_entry_match_remaining = r_entry_match[13];

            if (entry_key != 1 && !service_name_found)
                throw ServiceDefinitionParseException("service name must be first entry in service definition",
                                                      working_info);

            switch (entry_key)
            {
                // service name
            case 1: {
                if (entry_key_max >= 1)
                    throw ServiceDefinitionParseException("service name must be first entry in service definition",
                                                          working_info);
                if (service_name_found)
                    throw ServiceDefinitionParseException("service name already specified", working_info);
                std::vector<std::string> tmp_name;
                ServiceDefinition_FromStringTypeFormat(l, "service", tmp_name, working_info);
                Name = tmp_name.at(0);
                DocString = docstring;
                entry_key_max = 1;
                service_name_found = true;
                ParseInfo.ServiceName = Name;
                working_info.ServiceName = Name;
                continue;
            }
            // stdver
            case 2: {
                if (entry_key_max >= 2)
                    throw ServiceDefinitionParseException("stdver must be first after service name", working_info);
                stdver_version.FromString(to_ref(r_entry_match_remaining), &working_info);
                stdver_found = true;
                if (stdver_version < RobotRaconteurVersion(0, 9))
                {
                    throw ServiceDefinitionParseException(
                        "Service definition standard version 0.9 or greater required for \"stdver\" keyword",
                        working_info);
                }
                continue;
            }
            // option
            case 3: {
                Options.push_back(r_entry_match_remaining);
                if (stdver_version < RobotRaconteurVersion(0, 9, 2))
                {
                    warnings.push_back(ServiceDefinitionParseException("option keyword is deprecated", working_info));
                }
                else
                {
                    throw ServiceDefinitionParseException("option keyword is not support in stdver 0.9.2 or greater",
                                                          working_info);
                }
                continue;
            }
            // import
            case 4: {
                if (entry_key_max > 4)
                    throw ServiceDefinitionParseException("import must be before all but options", working_info);
                ServiceDefinition_FromStringImportFormat(l, "import", Imports, working_info);
                entry_key_max = 4;
                continue;
            }
            // using
            case 5: {
                if (entry_key_max > 5)
                    throw ServiceDefinitionParseException(
                        "using must be after imports and before all others except options", working_info);
                RR_SHARED_PTR<UsingDefinition> using_def = RR_MAKE_SHARED<UsingDefinition>(shared_from_this());
                using_def->FromString(l, &working_info);
                Using.push_back(using_def);
                entry_key_max = 5;
                continue;
            }
            // exception
            case 6: {
                if (entry_key_max >= 9)
                    throw ServiceDefinitionParseException("exception must be before struct and object", working_info);
                RR_SHARED_PTR<ExceptionDefinition> exception_def =
                    RR_MAKE_SHARED<ExceptionDefinition>(shared_from_this());
                exception_def->FromString(l, &working_info);
                exception_def->DocString = docstring;
                Exceptions.push_back(exception_def);
                entry_key_max = 6;
                continue;
            }
            // constant
            case 7: {
                if (entry_key_max >= 9)
                    throw ServiceDefinitionParseException("constant must be before struct and object", working_info);
                RR_SHARED_PTR<ConstantDefinition> constant_def = RR_MAKE_SHARED<ConstantDefinition>(shared_from_this());
                constant_def->FromString(l, &working_info);
                constant_def->DocString = docstring;
                Constants.push_back(constant_def);
                entry_key_max = 7;
                continue;
            }
            // enum
            case 8: {
                if (entry_key_max >= 9)
                    throw ServiceDefinitionParseException("enum must be before struct and object", working_info);
                ServiceDefinitionParseInfo init_info;
                std::stringstream block;
                ServiceDefinition_FindBlock(l, s, block, working_info, init_info, stdver_version);
                RR_SHARED_PTR<EnumDefinition> enum_def = RR_MAKE_SHARED<EnumDefinition>(shared_from_this());
                enum_def->FromString(block.str(), &init_info);
                enum_def->DocString = docstring;
                Enums.push_back(enum_def);
                entry_key_max = 8;
                continue;
            }
            // struct
            case 9: {
                ServiceDefinitionParseInfo init_info;
                std::stringstream block;
                ServiceDefinition_FindBlock(l, s, block, working_info, init_info, stdver_version);
                RR_SHARED_PTR<ServiceEntryDefinition> struct_def =
                    RR_MAKE_SHARED<ServiceEntryDefinition>(shared_from_this());
                struct_def->FromString(block.str(), warnings, &init_info);
                if (stdver_version >= RobotRaconteurVersion(0, 9, 2) && !struct_def->Options.empty())
                    throw ServiceDefinitionParseException("option keyword is not support in stdver 0.9.2 or greater",
                                                          working_info);
                struct_def->DocString = docstring;
                Structures.push_back(struct_def);
                entry_key_max = 9;
                continue;
            }
            // object
            case 10: {
                ServiceDefinitionParseInfo init_info;
                std::stringstream block;
                ServiceDefinition_FindBlock(l, s, block, working_info, init_info, stdver_version);
                RR_SHARED_PTR<ServiceEntryDefinition> object_def =
                    RR_MAKE_SHARED<ServiceEntryDefinition>(shared_from_this());
                object_def->FromString(block.str(), warnings, &init_info);
                if (stdver_version >= RobotRaconteurVersion(0, 9, 2) && !object_def->Options.empty())
                    throw ServiceDefinitionParseException("option keyword is not support in stdver 0.9.2 or greater",
                                                          working_info);
                object_def->DocString = docstring;
                Objects.push_back(object_def);
                entry_key_max = 10;
                continue;
            }
            // pod
            case 11: {
                ServiceDefinitionParseInfo init_info;
                std::stringstream block;
                ServiceDefinition_FindBlock(l, s, block, working_info, init_info, stdver_version);
                RR_SHARED_PTR<ServiceEntryDefinition> struct_def =
                    RR_MAKE_SHARED<ServiceEntryDefinition>(shared_from_this());
                struct_def->FromString(block.str(), warnings, &init_info);
                if (stdver_version >= RobotRaconteurVersion(0, 9, 2) && !struct_def->Options.empty())
                    throw ServiceDefinitionParseException("option keyword is not support in stdver 0.9.2 or greater",
                                                          working_info);
                struct_def->DocString = docstring;
                Pods.push_back(struct_def);
                entry_key_max = 9;
                continue;
            }
            // namedarray
            case 12: {
                ServiceDefinitionParseInfo init_info;
                std::stringstream block;
                ServiceDefinition_FindBlock(l, s, block, working_info, init_info, stdver_version);
                RR_SHARED_PTR<ServiceEntryDefinition> struct_def =
                    RR_MAKE_SHARED<ServiceEntryDefinition>(shared_from_this());
                struct_def->FromString(block.str(), warnings, &init_info);
                if (stdver_version >= RobotRaconteurVersion(0, 9, 2) && !struct_def->Options.empty())
                    throw ServiceDefinitionParseException("option keyword is not support in stdver 0.9.2 or greater",
                                                          working_info);
                struct_def->DocString = docstring;
                NamedArrays.push_back(struct_def);
                entry_key_max = 9;
                continue;
            }
            default:
                throw ServiceDefinitionParseException("Parse error", working_info);
            }
        }

        bool version_found = false;
        if (stdver_found)
        {
            StdVer = stdver_version;
            version_found = true;
        }
        BOOST_FOREACH (std::string& so, Options)
        {
            static boost::regex r_version("^[ \\t]*version[ \\t]+(?:(\\d+(?:\\.\\d+)*)|[ -~\\t]*)$");
            boost::smatch r_version_match;
            if (boost::regex_match(so, r_version_match, r_version))
            {
                if (version_found)
                    throw ServiceDefinitionParseException("Robot Raconteur version already specified", working_info);
                if (r_version_match[1].matched)
                {
                    StdVer = RobotRaconteurVersion(r_version_match[1].str());
                }
                else
                {
                    throw ServiceDefinitionParseException("Invalid Robot Raconteur version specified", working_info);
                }
                version_found = true;
            }
        }
    }
    catch (ServiceDefinitionParseException&)
    {
        throw;
    }
    catch (std::exception& e)
    {
        throw ServiceDefinitionParseException("Parse error", working_info);
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
    ParseInfo.Reset();
}

ServiceDefinition::ServiceDefinition()
{
    Structures.clear();
    Objects.clear();
    Options = std::vector<std::string>();
    Imports = std::vector<std::string>();
}

ServiceEntryDefinition::ServiceEntryDefinition(const RR_SHARED_PTR<ServiceDefinition>& def)
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

static std::string ServiceEntryDefinition_UnqualifyTypeWithUsing(const ServiceEntryDefinition& e, const std::string& s)
{
    if (!boost::contains(s, "."))
    {
        return s;
    }

    RR_SHARED_PTR<ServiceDefinition> d = e.ServiceDefinition_.lock();
    if (!d)
        return s;

    BOOST_FOREACH (const RR_SHARED_PTR<UsingDefinition>& u, d->Using)
    {
        if (u->QualifiedName == s)
        {
            return u->UnqualifiedName;
        }
    }
    return s;
}

void ServiceEntryDefinition::ToStream(std::ostream& o) const
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

    BOOST_FOREACH (const std::string& imp, Implements)
    {
        o << "    implements " << ServiceEntryDefinition_UnqualifyTypeWithUsing(*this, imp) << "\n";
    }

    BOOST_FOREACH (const std::string& option, Options)
    {
        o << "    option " << option << "\n";
    }

    BOOST_FOREACH (const RR_SHARED_PTR<ConstantDefinition>& constant, Constants)
    {
        o << "    " << constant->ToString() << "\n";
    }

    BOOST_FOREACH (const RR_SHARED_PTR<MemberDefinition>& d, Members)
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

void ServiceEntryDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    std::vector<ServiceDefinitionParseException> w;
    FromString(s, w, parse_info);
}

template <typename member_type>
static void ServiceEntryDefinition_FromString_DoMember(boost::string_ref l, const std::string& docstring,
                                                       const RR_SHARED_PTR<ServiceEntryDefinition>& entry,
                                                       const ServiceDefinitionParseInfo& parse_info)
{
    RR_SHARED_PTR<member_type> member = RR_MAKE_SHARED<member_type>(entry);
    member->FromString(l, &parse_info);
    member->DocString = docstring;
    entry->Members.push_back(member);
}

void ServiceEntryDefinition::FromString(boost::string_ref s, std::vector<ServiceDefinitionParseException>& warnings,
                                        const ServiceDefinitionParseInfo* parse_info)
{
    std::istringstream os(s.to_string());
    FromStream(os, warnings, parse_info);
}

void ServiceEntryDefinition::FromStream(std::istream& s, const ServiceDefinitionParseInfo* parse_info)
{
    std::vector<ServiceDefinitionParseException> w;
    FromStream(s, w, parse_info);
}

static std::string ServiceEntryDefinition_QualifyTypeWithUsing(ServiceEntryDefinition& e, const std::string& s)
{
    if (boost::contains(s, "."))
    {
        return s;
    }

    RR_SHARED_PTR<ServiceDefinition> d = e.ServiceDefinition_.lock();
    if (!d)
        return s;

    BOOST_FOREACH (const RR_SHARED_PTR<UsingDefinition>& u, d->Using)
    {
        if (u->UnqualifiedName == s)
        {
            return u->QualifiedName;
        }
    }
    return s;
}

void ServiceEntryDefinition::FromStream(std::istream& s, std::vector<ServiceDefinitionParseException>& warnings,
                                        const ServiceDefinitionParseInfo* parse_info)
{
    Reset();

    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    static boost::regex start_struct_regex("^[ \\t]*struct[ \\t]+(\\w+)[ \\t]*$");
    static boost::regex start_pod_regex("^[ \\t]*pod[ \\t]+(\\w+)[ \\t]*$");
    static boost::regex start_namedarray_regex("^[ \\t]*namedarray[ \\t]+(\\w+)[ \\t]*$");
    static boost::regex start_object_regex("^[ \\t]*object[ \\t]+(\\w+)[ \\t]*$");
    static boost::regex end_struct_regex("^[ \\t]*end[ \\t]+struct[ \\t]*$");
    static boost::regex end_pod_regex("^[ \\t]*end[ \\t]+pod[ \\t]*$");
    static boost::regex end_namedarray_regex("^[ \\t]*end[ \\t]+namedarray[ \\t]*$");
    static boost::regex end_object_regex("^[ \\t]*end[ \\t]+object[ \\t]*$");

    ServiceDefinitionParseInfo working_info = ParseInfo;
    if (working_info.LineNumber > 0)
    {
        working_info.LineNumber -= 1;
    }

    std::string l;
    std::string docstring;
    if (!ServiceDefinition_GetLine(s, l, docstring, working_info))
    {
        throw ServiceDefinitionParseException("Invalid service entry", working_info);
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = l;

    boost::smatch start_struct_cmatch;
    boost::smatch start_pod_cmatch;
    boost::smatch start_namedarray_cmatch;
    boost::smatch start_object_cmatch;
    DocString = docstring;
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
        throw ServiceDefinitionParseException("Parse error", working_info);
    }

    try
    {

        static boost::regex r_member(
            "(?:^[ "
            "\\t]*(?:(option)|(implements)|(constant)|(field)|(property)|(function)|(event)|(objref)|(pipe)|(callback)|"
            "(wire)|(memory)|(end))[ \\t]+(\\w[^\\s]*(?:[ \\t]+[^\\s]+)*)[ \\t]*$)|(^[ \\t]*$)");

        while (ServiceDefinition_GetLine(s, l, docstring, working_info))
        {

            try
            {

                boost::smatch r_member_match;
                if (!boost::regex_match(l, r_member_match, r_member))
                {
                    if (boost::trim_copy(l) == "end")
                    {
                        if (ServiceDefinition_GetLine(s, l, docstring, working_info))
                        {
                            throw ServiceDefinitionParseException("Parse error", working_info);
                        }
                        return;
                    }
                    throw ServiceDefinitionParseException("Parse error", working_info);
                }

                const boost::smatch::value_type& r_member_match_blank = r_member_match[15];
                if (r_member_match_blank.matched)
                    continue;

                int32_t member_key = 1;
                for (; member_key < 13; member_key++)
                {
                    if (r_member_match[member_key].matched)
                        break;
                }

                const boost::smatch::value_type& r_member_match_remaining = r_member_match[14];

                if ((EntryType != DataTypes_object_t) && (member_key >= 5 && member_key != 13))
                {
                    throw ServiceDefinitionParseException("Structures must only contain fields, constants, and options",
                                                          working_info);
                }

                switch (member_key)
                {
                // option
                case 1: {
                    // TODO: look in to this
                    // if (!Members.empty()) throw ServiceDefinitionParseException("Structure option must come before
                    // members", boost::numeric_cast<int32_t>(pos));
                    Options.push_back(r_member_match_remaining);
                    warnings.push_back(ServiceDefinitionParseException("option keyword is deprecated", working_info));
                    continue;
                }
                // implements
                case 2: {
                    if (!Members.empty())
                        throw ServiceDefinitionParseException("Structure implements must come before members",
                                                              working_info);
                    if (EntryType != DataTypes_object_t)
                        throw ServiceDefinitionParseException(
                            "Structures must only contain fields, constants, and options", working_info);
                    std::vector<std::string> implements1;
                    ServiceDefinition_FromStringTypeFormat(l, "implements", implements1, working_info);
                    Implements.push_back(ServiceEntryDefinition_QualifyTypeWithUsing(*this, implements1.at(0)));
                    continue;
                }
                // constant
                case 3: {
                    if (!Members.empty())
                        throw ServiceDefinitionParseException("Structure constants must come before members",
                                                              working_info);
                    RR_SHARED_PTR<ConstantDefinition> constant_def =
                        RR_MAKE_SHARED<ConstantDefinition>(shared_from_this());
                    constant_def->FromString(l, &working_info);
                    constant_def->DocString = docstring;
                    Constants.push_back(constant_def);
                    continue;
                }
                // field
                case 4: {
                    if (EntryType == DataTypes_object_t)
                        throw ServiceDefinitionParseException("Objects cannot contain fields, use properties instead",
                                                              working_info);
                    ServiceEntryDefinition_FromString_DoMember<PropertyDefinition>(l, docstring, shared_from_this(),
                                                                                   working_info);
                    continue;
                }
                // property
                case 5: {
                    ServiceEntryDefinition_FromString_DoMember<PropertyDefinition>(l, docstring, shared_from_this(),
                                                                                   working_info);
                    continue;
                }
                // function
                case 6: {
                    ServiceEntryDefinition_FromString_DoMember<FunctionDefinition>(l, docstring, shared_from_this(),
                                                                                   working_info);
                    continue;
                }
                // event
                case 7: {
                    ServiceEntryDefinition_FromString_DoMember<EventDefinition>(l, docstring, shared_from_this(),
                                                                                working_info);
                    continue;
                }
                // objref
                case 8: {
                    ServiceEntryDefinition_FromString_DoMember<ObjRefDefinition>(l, docstring, shared_from_this(),
                                                                                 working_info);
                    continue;
                }
                // pipe
                case 9: {
                    ServiceEntryDefinition_FromString_DoMember<PipeDefinition>(l, docstring, shared_from_this(),
                                                                               working_info);
                    continue;
                }
                // callback
                case 10: {
                    ServiceEntryDefinition_FromString_DoMember<CallbackDefinition>(l, docstring, shared_from_this(),
                                                                                   working_info);
                    continue;
                }
                // wire
                case 11: {
                    ServiceEntryDefinition_FromString_DoMember<WireDefinition>(l, docstring, shared_from_this(),
                                                                               working_info);
                    continue;
                }
                // memory
                case 12: {
                    ServiceEntryDefinition_FromString_DoMember<MemoryDefinition>(l, docstring, shared_from_this(),
                                                                                 working_info);
                    continue;
                }
                // end
                case 13: {
                    if (EntryType == DataTypes_structure_t)
                    {
                        boost::smatch matches;
                        if (!boost::regex_match(l, matches, end_struct_regex))
                        {
                            throw ServiceDefinitionParseException("Parse error", working_info);
                        }
                    }
                    else if (EntryType == DataTypes_pod_t)
                    {
                        boost::smatch matches;
                        if (!boost::regex_match(l, matches, end_pod_regex))
                        {
                            throw ServiceDefinitionParseException("Parse error", working_info);
                        }
                    }
                    else if (EntryType == DataTypes_namedarray_t)
                    {
                        boost::smatch matches;
                        if (!boost::regex_match(l, matches, end_namedarray_regex))
                        {
                            throw ServiceDefinitionParseException("Parse error", working_info);
                        }
                    }
                    else
                    {
                        boost::smatch matches;
                        if (!boost::regex_match(l, matches, end_object_regex))
                        {
                            throw ServiceDefinitionParseException("Parse error", working_info);
                        }
                    }

                    if (ServiceDefinition_GetLine(s, l, docstring, working_info))
                    {
                        throw ServiceDefinitionParseException("Parse error", working_info);
                    }
                    return;
                }

                default:
                    throw ServiceDefinitionParseException("Parse error", working_info);
                    break;
                }
            }
            catch (ServiceDefinitionParseException&)
            {
                throw;
            }
        }
    }
    catch (ServiceDefinitionParseException&)
    {
        throw;
    }
    catch (std::exception& exp)
    {
        throw ServiceDefinitionParseException("Parse error: " + std::string(exp.what()), working_info);
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
    ParseInfo.Reset();
}

DataTypes ServiceEntryDefinition::RRDataType() const
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
    if (!def)
        throw InvalidOperationException("Could not lock service definition to resolve named type");
    return def->Name + "." + Name;
}

// Parser helper functions
class MemberDefiniton_ParseResults
{
  public:
    std::string MemberType;
    std::string Name;
    boost::optional<std::string> DataType;
    boost::optional<std::vector<std::string> > Parameters;
    boost::optional<std::vector<std::string> > Modifiers;
};

template <typename T>
static bool MemberDefinition_ParseCommaList(const boost::regex& r, boost::string_ref s, std::vector<T>& res)
{
    static boost::regex r_empty("^[ \\t]*$");
    if (boost::regex_match(s.begin(), s.end(), r_empty))
    {
        return true;
    }

    ref_match r_match;
    if (!boost::regex_match(s.begin(), s.end(), r_match, r))
    {
        return false;
    }

    res.push_back(r_match[1]);
    if (r_match[2].matched)
    {
        if (!MemberDefinition_ParseCommaList(r, to_ref(r_match[2]), res))
        {
            return false;
        }
    }

    return true;
}

template <typename T>
static bool MemberDefinition_ParseParameters(boost::string_ref s, std::vector<T>& res)
{
    static boost::regex r_params("^[ \\t]*(" RR_TYPE_REGEX
                                 "(?:\\[[0-9\\,\\*\\-]*\\])?(?:\\{\\w{1,16}\\})?[ \\t]+\\w+)(?:[ \\t]*,[ \\t]*([ "
                                 "-~\\t]*\\w[ -~\\t]*))?[ \\t]*$");

    return MemberDefinition_ParseCommaList(r_params, s, res);
}

template <typename T>
static bool MemberDefinition_ParseModifiers(boost::string_ref s, std::vector<T>& res)
{
    static boost::regex r_modifiers("^[ \\t]*(" RR_NAME_REGEX
                                    "(?:\\([\\w\\-\\+\\., \\t]*\\))?)(?:[ \\t]*,([ -~\\t]*))?$");

    if (!MemberDefinition_ParseCommaList(r_modifiers, s, res))
    {
        return false;
    }

    if (!res.empty())
    {
        static boost::regex r_modifier("^[ \\t]*" RR_NAME_REGEX "(?:\\([ \\t]*(?:" RR_NUMBER_REGEX "|" RR_NAME_REGEX
                                       ")[ \\t]*(?:,[ \\t]*(?:" RR_NUMBER_REGEX "|" RR_NAME_REGEX "))*[ \\t]*\\))?");
        BOOST_FOREACH (const std::string& s2, res)
        {
            if (!boost::regex_match(s2, r_modifier))
            {
                return false;
            }
        }
    }

    return true;
}

static std::string MemberDefinition_ModifiersToString(const std::vector<std::string>& modifiers)
{
    if (modifiers.empty())
        return "";

    return " [" + boost::join(modifiers, ",") + "]";
}

static bool MemberDefinition_ParseFormat_common(boost::string_ref s, MemberDefiniton_ParseResults& res)
{
    static boost::regex r("^[ \\t]*([a-zA-Z]+)[ \\t]+(?:([a-zA-Z][\\w\\{\\}\\[\\]\\*\\,\\-\\.]*)[ \\t]+)?(\\w+)(?:[ "
                          "\\t]*(\\(([^)]*)\\)))?(?:[ \\t]+\\[([^\\]]*)\\])?[ \\t]*$");
    ref_match r_result;
    if (!boost::regex_match(s.begin(), s.end(), r_result, r))
    {
        return false;
    }

    res.Modifiers.reset();
    res.Parameters.reset();

    const ref_match::value_type& member_type_result = r_result[1];
    const ref_match::value_type& data_type_result = r_result[2];
    const ref_match::value_type& name_result = r_result[3];
    const ref_match::value_type& params_present_result = r_result[4];
    const ref_match::value_type& params_result = r_result[5];
    const ref_match::value_type& modifiers_result = r_result[6];

    res.MemberType = member_type_result;
    if (data_type_result.matched)
    {
        res.DataType = data_type_result;
    }
    res.Name = name_result;

    if (params_present_result.matched)
    {
        res.Parameters = std::vector<std::string>();
        if (!MemberDefinition_ParseParameters(to_ref(params_result), *res.Parameters))
        {
            return false;
        }
    }

    if (modifiers_result.matched)
    {
        res.Modifiers = std::vector<std::string>();
        if (!MemberDefinition_ParseModifiers(to_ref(modifiers_result), *res.Modifiers))
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

static void MemberDefinition_FromStringFormat_common(MemberDefiniton_ParseResults& parse_res, boost::string_ref s1,
                                                     const std::vector<std::string>& member_types,
                                                     const RR_SHARED_PTR<MemberDefinition>& def,
                                                     const ServiceDefinitionParseInfo& parse_info)
{

    if (!MemberDefinition_ParseFormat_common(s1, parse_res))
    {
        throw ServiceDefinitionParseException("Could not parse " + member_types.at(0), parse_info);
    }

    if (boost::range::find(member_types, parse_res.MemberType) == member_types.end())
    {
        throw ServiceDefinitionParseException("Format Error", parse_info);
    }

    def->Reset();
    def->ParseInfo = parse_info;
    def->Name = parse_res.Name;
}

static void MemberDefinition_FromStringFormat1(boost::string_ref s1, const std::vector<std::string>& member_types,
                                               const RR_SHARED_PTR<MemberDefinition>& def,
                                               RR_SHARED_PTR<TypeDefinition>& type,
                                               const ServiceDefinitionParseInfo& parse_info)
{
    MemberDefiniton_ParseResults parse_res;
    MemberDefinition_FromStringFormat_common(parse_res, s1, member_types, def, parse_info);

    if (!parse_res.DataType || parse_res.Parameters)
        throw ServiceDefinitionParseException("Format error for " + member_types.at(0), parse_info);
    type = RR_MAKE_SHARED<TypeDefinition>(def);
    type->FromString(*parse_res.DataType, &parse_info);
    type->Rename("value");
    type->QualifyTypeStringWithUsing();

    if (parse_res.Modifiers)
    {
        def->Modifiers = *parse_res.Modifiers;
    }
}
static void MemberDefinition_FromStringFormat1(boost::string_ref s1, boost::string_ref member_type,
                                               const RR_SHARED_PTR<MemberDefinition>& def,
                                               RR_SHARED_PTR<TypeDefinition>& type,
                                               const ServiceDefinitionParseInfo& parse_info)
{
    std::vector<std::string> member_types;
    member_types.push_back(RR_MOVE(member_type.to_string()));
    MemberDefinition_FromStringFormat1(s1, member_types, def, type, parse_info);
}

static std::string MemberDefinition_ToStringFormat1(boost::string_ref member_type, const MemberDefinition& def,
                                                    const TypeDefinition& data_type)
{
    TypeDefinition t;
    data_type.CopyTo(t);
    t.Rename(def.Name);
    t.UnqualifyTypeStringWithUsing();

    return member_type + " " + t.ToString() + MemberDefinition_ModifiersToString(def.Modifiers);
}

static void MemberDefinition_ParamatersFromStrings(const std::vector<std::string>& s,
                                                   std::vector<RR_SHARED_PTR<TypeDefinition> >& params,
                                                   const RR_SHARED_PTR<MemberDefinition>& def,
                                                   const ServiceDefinitionParseInfo& parse_info)
{
    BOOST_FOREACH (const std::string& s1, s)
    {
        RR_SHARED_PTR<TypeDefinition> tdef = RR_MAKE_SHARED<TypeDefinition>(def);
        tdef->FromString(s1, &parse_info);
        tdef->QualifyTypeStringWithUsing();
        params.push_back(tdef);
    }
}

static std::string MemberDefinition_ParametersToString(const std::vector<RR_SHARED_PTR<TypeDefinition> >& params)
{
    std::vector<std::string> params2;
    BOOST_FOREACH (const RR_SHARED_PTR<TypeDefinition>& p, params)
    {
        TypeDefinition p2;
        p->CopyTo(p2);
        p2.UnqualifyTypeStringWithUsing();
        params2.push_back(p2.ToString());
    }

    return boost::join(params2, ", ");
}

static void MemberDefinition_FromStringFormat2(boost::string_ref s1, boost::string_ref member_type,
                                               const RR_SHARED_PTR<MemberDefinition>& def,
                                               RR_SHARED_PTR<TypeDefinition>& return_type,
                                               std::vector<RR_SHARED_PTR<TypeDefinition> >& params,
                                               const ServiceDefinitionParseInfo& parse_info)
{
    std::vector<std::string> member_types;
    member_types.push_back(member_type.to_string());

    MemberDefiniton_ParseResults parse_res;
    MemberDefinition_FromStringFormat_common(parse_res, s1, member_types, def, parse_info);

    if (!parse_res.DataType || !parse_res.Parameters)
        throw ServiceDefinitionParseException("Format error for " + member_types.at(0) + " definition \"" +
                                                  boost::trim_copy(s1.to_string()) + "\"",
                                              parse_info);
    return_type = RR_MAKE_SHARED<TypeDefinition>(def);
    return_type->FromString(*parse_res.DataType, &parse_info);
    return_type->Rename("");
    return_type->QualifyTypeStringWithUsing();

    MemberDefinition_ParamatersFromStrings(*parse_res.Parameters, params, def, parse_info);

    if (parse_res.Modifiers)
    {
        def->Modifiers = *parse_res.Modifiers;
    }
}

static std::string MemberDefinition_ToStringFormat2(boost::string_ref member_type, const MemberDefinition& def,
                                                    const TypeDefinition& return_type,
                                                    const std::vector<RR_SHARED_PTR<TypeDefinition> >& params)
{
    TypeDefinition t;
    return_type.CopyTo(t);
    t.Rename(def.Name);
    t.UnqualifyTypeStringWithUsing();

    return member_type + " " + t.ToString() + "(" + MemberDefinition_ParametersToString(params) + ")" +
           MemberDefinition_ModifiersToString(def.Modifiers);
}

static void MemberDefinition_FromStringFormat3(boost::string_ref s1, boost::string_ref member_type,
                                               const RR_SHARED_PTR<MemberDefinition>& def,
                                               std::vector<RR_SHARED_PTR<TypeDefinition> >& params,
                                               const ServiceDefinitionParseInfo& parse_info)
{
    std::vector<std::string> member_types;
    member_types.push_back(RR_MOVE(member_type.to_string()));

    MemberDefiniton_ParseResults parse_res;
    MemberDefinition_FromStringFormat_common(parse_res, s1, member_types, def, parse_info);

    if (parse_res.DataType || !parse_res.Parameters)
        throw ServiceDefinitionParseException("Format error for " + member_types.at(0) + " definition \"" +
                                              boost::trim_copy(s1.to_string()) + "\"");

    MemberDefinition_ParamatersFromStrings(*parse_res.Parameters, params, def, parse_info);

    if (parse_res.Modifiers)
    {
        def->Modifiers = *parse_res.Modifiers;
    }
}

static std::string MemberDefinition_ToStringFormat3(boost::string_ref member_type, const MemberDefinition& def,
                                                    const std::vector<RR_SHARED_PTR<TypeDefinition> >& params)
{
    return member_type + " " + def.Name + "(" + MemberDefinition_ParametersToString(params) + ")" +
           MemberDefinition_ModifiersToString(def.Modifiers);
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

MemberDefinition::MemberDefinition(const RR_SHARED_PTR<ServiceEntryDefinition>& ServiceEntry)
{
    this->ServiceEntry = ServiceEntry;
}

MemberDefinition_NoLock MemberDefinition::NoLock() const
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
    // ServiceEntry.reset();
    Modifiers.clear();
    ParseInfo.Reset();
}

PropertyDefinition::PropertyDefinition(const RR_SHARED_PTR<ServiceEntryDefinition>& ServiceEntry)
    : MemberDefinition(ServiceEntry)
{}

std::string PropertyDefinition::ToString() { return ToString(false); }

std::string PropertyDefinition::ToString(bool isstruct) const
{
    std::string member_type = isstruct ? "field" : "property";

    return MemberDefinition_ToStringFormat1(member_type, *this, *Type);
}

void PropertyDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = s.to_string();

    std::vector<std::string> member_types;
    member_types.push_back("property");
    member_types.push_back("field");
    MemberDefinition_FromStringFormat1(s, member_types, shared_from_this(), Type, ParseInfo);
}

void PropertyDefinition::Reset()
{
    MemberDefinition::Reset();
    Type.reset();
}

MemberDefinition_Direction PropertyDefinition::Direction() { return MemberDefinition_GetDirection(Modifiers); }

FunctionDefinition::FunctionDefinition(const RR_SHARED_PTR<ServiceEntryDefinition>& ServiceEntry)
    : MemberDefinition(ServiceEntry)
{
    FunctionDefinition::Reset();
}

std::string FunctionDefinition::ToString()
{
    return MemberDefinition_ToStringFormat2("function", *this, *ReturnType, Parameters);
}

void FunctionDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = s.to_string();

    MemberDefinition_FromStringFormat2(s, "function", shared_from_this(), ReturnType, Parameters, ParseInfo);
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

EventDefinition::EventDefinition(const RR_SHARED_PTR<ServiceEntryDefinition>& ServiceEntry)
    : MemberDefinition(ServiceEntry)
{
    EventDefinition::Reset();
}

std::string EventDefinition::ToString() { return MemberDefinition_ToStringFormat3("event", *this, Parameters); }

void EventDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = s.to_string();

    MemberDefinition_FromStringFormat3(s, "event", shared_from_this(), Parameters, ParseInfo);
}

void EventDefinition::Reset()
{
    MemberDefinition::Reset();
    Parameters.clear();
}

ObjRefDefinition::ObjRefDefinition(const RR_SHARED_PTR<ServiceEntryDefinition>& ServiceEntry)
    : MemberDefinition(ServiceEntry)
{
    ArrayType = DataTypes_ArrayTypes_none;
    ContainerType = DataTypes_ContainerTypes_none;
    ObjRefDefinition::Reset();
}

std::string ObjRefDefinition::ToString()
{
    // TODO: make this const friendly
    TypeDefinition t(shared_from_this());
    t.Name = Name;
    t.TypeString = ObjectType;
    t.Type = DataTypes_namedtype_t;

    switch (ArrayType)
    {
    case DataTypes_ArrayTypes_none: {
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
    case DataTypes_ArrayTypes_array: {
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

void ObjRefDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{

    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = s.to_string();

    RR_SHARED_PTR<TypeDefinition> t;
    MemberDefinition_FromStringFormat1(s, "objref", shared_from_this(), t, ParseInfo);
    if (!t)
    {
        throw ServiceDefinitionParseException("Invalid objref definition \"" + boost::trim_copy(s.to_string()) + "\"",
                                              ParseInfo);
    }

    switch (t->ArrayType)
    {
    case DataTypes_ArrayTypes_none: {
        switch (t->ContainerType)
        {
        case DataTypes_ContainerTypes_none:
        case DataTypes_ContainerTypes_map_int32:
        case DataTypes_ContainerTypes_map_string:
            break;
        default:
            throw ServiceDefinitionParseException(
                "Invalid objref definition \"" + boost::trim_copy(s.to_string()) + "\"", ParseInfo);
        }
        break;
    }
    case DataTypes_ArrayTypes_array: {
        if (ContainerType != DataTypes_ContainerTypes_none)
        {
            throw ServiceDefinitionParseException(
                "Invalid objref definition \"" + boost::trim_copy(s.to_string()) + "\"", ParseInfo);
        }
        if (!t->ArrayVarLength)
        {
            throw ServiceDefinitionParseException(
                "Invalid objref definition \"" + boost::trim_copy(s.to_string()) + "\"", ParseInfo);
        }
        if (t->ArrayLength.at(0) != 0)
        {
            throw ServiceDefinitionParseException(
                "Invalid objref definition \"" + boost::trim_copy(s.to_string()) + "\"", ParseInfo);
        }
        break;
    }
    default:
        throw ServiceDefinitionParseException("Invalid objref definition \"" + boost::trim_copy(s.to_string()) + "\"",
                                              ParseInfo);
    }

    if (!((!t->TypeString.empty() && t->Type == DataTypes_namedtype_t) || (t->Type == DataTypes_varobject_t)))
    {
        throw ServiceDefinitionParseException("Invalid objref definition \"" + boost::trim_copy(s.to_string()) + "\"",
                                              ParseInfo);
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

PipeDefinition::PipeDefinition(const RR_SHARED_PTR<ServiceEntryDefinition>& ServiceEntry)
    : MemberDefinition(ServiceEntry)
{
    PipeDefinition::Reset();
}

std::string PipeDefinition::ToString() { return MemberDefinition_ToStringFormat1("pipe", *this, *Type); }

void PipeDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = s.to_string();

    MemberDefinition_FromStringFormat1(s, "pipe", shared_from_this(), Type, ParseInfo);
}

void PipeDefinition::Reset()
{
    MemberDefinition::Reset();
    Type.reset();
}

MemberDefinition_Direction PipeDefinition::Direction() const { return MemberDefinition_GetDirection(Modifiers); }

bool PipeDefinition::IsUnreliable() const
{
    if (boost::range::find(Modifiers, "unreliable") != Modifiers.end())
    {
        return true;
    }

    RR_SHARED_PTR<ServiceEntryDefinition> obj = ServiceEntry.lock();
    if (obj)
    {
        BOOST_FOREACH (const std::string& o, obj->Options)
        {
            static boost::regex r("^[ \\t]*pipe[ \\t]+" + Name + "[ \\t]+unreliable[ \\t]*$");
            if (boost::regex_match(o, r))
            {
                return true;
            }
        }
    }

    return false;
}

CallbackDefinition::CallbackDefinition(const RR_SHARED_PTR<ServiceEntryDefinition>& ServiceEntry)
    : MemberDefinition(ServiceEntry)
{
    CallbackDefinition::Reset();
}

std::string CallbackDefinition::ToString()
{
    return MemberDefinition_ToStringFormat2("callback", *this, *ReturnType, Parameters);
}

void CallbackDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = s.to_string();

    MemberDefinition_FromStringFormat2(s, "callback", shared_from_this(), ReturnType, Parameters, ParseInfo);
}

void CallbackDefinition::Reset()
{
    MemberDefinition::Reset();
    Parameters.clear();
    ReturnType.reset();
}

WireDefinition::WireDefinition(const RR_SHARED_PTR<ServiceEntryDefinition>& ServiceEntry)
    : MemberDefinition(ServiceEntry)
{
    WireDefinition::Reset();
}

std::string WireDefinition::ToString() { return MemberDefinition_ToStringFormat1("wire", *this, *Type); }

void WireDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = s.to_string();

    MemberDefinition_FromStringFormat1(s, "wire", shared_from_this(), Type, ParseInfo);
}

void WireDefinition::Reset()
{
    MemberDefinition::Reset();
    Type.reset();
}

MemberDefinition_Direction WireDefinition::Direction() { return MemberDefinition_GetDirection(Modifiers); }

MemoryDefinition::MemoryDefinition(const RR_SHARED_PTR<ServiceEntryDefinition>& ServiceEntry)
    : MemberDefinition(ServiceEntry)
{
    MemoryDefinition::Reset();
}

std::string MemoryDefinition::ToString() { return MemberDefinition_ToStringFormat1("memory", *this, *Type); }

void MemoryDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = s.to_string();

    MemberDefinition_FromStringFormat1(s, "memory", shared_from_this(), Type, ParseInfo);
}

void MemoryDefinition::Reset()
{
    MemberDefinition::Reset();
    Type.reset();
}

MemberDefinition_Direction MemoryDefinition::Direction() { return MemberDefinition_GetDirection(Modifiers); }

TypeDefinition::TypeDefinition()
{
    ArrayType = DataTypes_ArrayTypes_none;
    ContainerType = DataTypes_ContainerTypes_none;
    Type = DataTypes_void_t;
    ArrayVarLength = false;
    Reset();
    this->member.reset();
}

TypeDefinition::TypeDefinition(const RR_SHARED_PTR<MemberDefinition>& member)
{
    ArrayType = DataTypes_ArrayTypes_none;
    ContainerType = DataTypes_ContainerTypes_none;
    Type = DataTypes_void_t;
    ArrayVarLength = false;
    Reset();
    this->member = member;
}

std::string TypeDefinition::ToString()
{
    std::ostringstream o;
    o << ((Type >= DataTypes_namedtype_t && (!(Type == DataTypes_varvalue_t) && !(Type == DataTypes_varobject_t)))
              ? TypeString
              : StringFromDataType(Type));

    switch (ArrayType)
    {
    case DataTypes_ArrayTypes_none:
        break;
    case DataTypes_ArrayTypes_array: {
        o << "[" + std::string(ArrayLength.at(0) != 0
                                   ? (boost::lexical_cast<std::string>(ArrayLength.at(0)) + (ArrayVarLength ? "-" : ""))
                                   : "")
          << "]";
        break;
    }
    case DataTypes_ArrayTypes_multidimarray: {
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
                BOOST_FOREACH (const int32_t& e, ArrayLength)
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

void TypeDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    Reset();

    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = s.to_string();

    static boost::regex r("^[ "
                          "\\t]*([a-zA-Z][\\w\\.]*)(?:(\\[\\])|\\[(([0-9]+)|([0-9]+)\\-|(\\*)|([0-9]+)\\,|([0-9\\,]+))"
                          "\\])?(?:\\{(\\w{1,16})\\})?(?:[ \\t]+(\\w+))?[ \\t]*$");
    ref_match r_result;
    if (!boost::regex_match(s.begin(), s.end(), r_result, r))
    {
        throw ServiceDefinitionParseException("Could not parse type \"" + trim_copy(s.to_string()) + "\"", ParseInfo);
    }

    const ref_match::value_type& type_result = r_result[1];
    const ref_match::value_type& array_result = r_result[2];
    const ref_match::value_type& array_result2 = r_result[3];
    const ref_match::value_type& array_var_result = r_result[4];
    const ref_match::value_type& array_max_var_result = r_result[5];
    const ref_match::value_type& array_multi_result = r_result[6];
    const ref_match::value_type& array_multi_single_fixed_result = r_result[7];
    const ref_match::value_type& array_multi_fixed_result = r_result[8];
    const ref_match::value_type& container_result = r_result[9];
    const ref_match::value_type& name_result = r_result[10];

    Name = name_result.matched ? name_result.str() : "";

    if (container_result.matched)
    {
        if (container_result == "list")
        {
            ContainerType = DataTypes_ContainerTypes_list;
        }
        else if (container_result == "int32")
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
            throw ServiceDefinitionParseException(
                "Could not parse type \"" + trim_copy(s.to_string()) + "\": invalid container type", ParseInfo);
        }
    }

    if (array_result.matched)
    {
        // variable array
        ArrayType = DataTypes_ArrayTypes_array;
        ArrayVarLength = true;
        ArrayLength.resize(1);
        ArrayLength.at(0) = 0;
    }
    if (array_result2.matched)
    {
        if (array_var_result.matched)
        {
            // Fixed array
            ArrayType = DataTypes_ArrayTypes_array;
            ArrayLength.resize(1);
            ArrayLength.at(0) = boost::lexical_cast<int32_t>(boost::lexical_cast<uint32_t>(array_var_result));
            ArrayVarLength = false;
        }
        else if (array_max_var_result.matched)
        {
            // variable array max sized
            ArrayType = DataTypes_ArrayTypes_array;
            ArrayLength.resize(1);
            ArrayLength.at(0) = boost::lexical_cast<int32_t>(boost::lexical_cast<uint32_t>(array_max_var_result));
            ArrayVarLength = true;
        }
        else if (array_multi_result.matched)
        {
            // multidim array
            ArrayType = DataTypes_ArrayTypes_multidimarray;
            ArrayVarLength = true;
        }
        else if (array_multi_single_fixed_result.matched)
        {
            // multidim single fixed array
            ArrayType = DataTypes_ArrayTypes_multidimarray;
            ArrayVarLength = false;
            ArrayLength.resize(1);
            ArrayLength.at(0) =
                boost::lexical_cast<int32_t>(boost::lexical_cast<uint32_t>(array_multi_single_fixed_result));
        }
        else if (array_multi_fixed_result.matched)
        {
            // multidim fixed array
            ArrayType = DataTypes_ArrayTypes_multidimarray;
            ArrayVarLength = false;
            std::vector<boost::iterator_range<boost::string_ref::const_iterator> > dims;
            boost::iterator_range<boost::string_ref::const_iterator> array_multi_fixed_result_rng(
                array_multi_fixed_result.first, array_multi_fixed_result.second);
            boost::split(dims, array_multi_fixed_result_rng, boost::is_any_of(","));
            ArrayLength.clear();
            BOOST_FOREACH (boost::iterator_range<boost::string_ref::const_iterator>& d, dims)
            {
                ArrayLength.push_back(boost::lexical_cast<int32_t>(boost::lexical_cast<uint32_t>(d)));
            }
        }
        else
        {
            throw ServiceDefinitionParseException(
                "Could not parse type \"" + trim_copy(s.to_string()) + "\": array error", ParseInfo);
        }
    }

    DataTypes t = DataTypeFromString(to_ref(type_result));
    if (t == DataTypes_namedtype_t)
    {
        Type = DataTypes_namedtype_t;
        TypeString = type_result.str();
    }
    else
    {
        Type = t;
        TypeString.clear();
    }
}

DataTypes TypeDefinition::DataTypeFromString(boost::string_ref d)
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

    return "";
}

void TypeDefinition::Reset()
{
    ArrayType = DataTypes_ArrayTypes_none;
    ContainerType = DataTypes_ContainerTypes_none;

    ArrayVarLength = false;
    Type = DataTypes_void_t;
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

void TypeDefinition::Rename(boost::string_ref name) { Name = name.to_string(); }

void TypeDefinition::RemoveContainers() { ContainerType = DataTypes_ContainerTypes_none; }

void TypeDefinition::RemoveArray()
{
    if (ContainerType != DataTypes_ContainerTypes_none)
        throw InvalidOperationException("Remove containers first");

    ArrayType = DataTypes_ArrayTypes_none;
    ArrayLength.clear();
    ArrayVarLength = false;
}

static const std::vector<RR_SHARED_PTR<UsingDefinition> > TypeDefinition_GetServiceUsingDefinition(TypeDefinition& def)
{
    RR_SHARED_PTR<MemberDefinition> member1 = def.member.lock();
    if (!member1)
        throw InvalidOperationException("Member not set for TypeDefinition");
    RR_SHARED_PTR<ServiceEntryDefinition> entry1 = member1->ServiceEntry.lock();
    if (!entry1)
        throw InvalidOperationException("Object or struct not set for MemberDefinition " + member1->Name);
    RR_SHARED_PTR<ServiceDefinition> service1 = entry1->ServiceDefinition_.lock();
    if (!service1)
        throw InvalidOperationException("ServiceDefinition or struct not set for Object or Structure " + entry1->Name);
    return service1->Using;
}

void TypeDefinition::QualifyTypeStringWithUsing()
{
    if (Type != DataTypes_namedtype_t)
        return;

    if (boost::contains(TypeString, "."))
        return;

    const std::vector<RR_SHARED_PTR<UsingDefinition> > using_ = TypeDefinition_GetServiceUsingDefinition(*this);
    BOOST_FOREACH (const RR_SHARED_PTR<UsingDefinition>& u, using_)
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
    BOOST_FOREACH (const RR_SHARED_PTR<UsingDefinition>& u, using_)
    {
        if (u->QualifiedName == TypeString)
        {
            TypeString = u->UnqualifiedName;
            return;
        }
    }
}

RR_SHARED_PTR<NamedTypeDefinition> TypeDefinition::ResolveNamedType(
    const std::vector<RR_SHARED_PTR<ServiceDefinition> >& imported_defs, const RR_SHARED_PTR<RobotRaconteurNode>& node,
    const RR_SHARED_PTR<RRObject>& client)
{
    RR_SHARED_PTR<NamedTypeDefinition> o = ResolveNamedType_cache.lock();
    if (o)
    {
        return o;
    }

    o.reset();

    RR_SHARED_PTR<ServiceDefinition> def;

    boost::string_ref entry_name;

    if (!boost::contains(TypeString, "."))
    {
        entry_name = TypeString;
        // Assume not imported
        RR_SHARED_PTR<MemberDefinition> m = member.lock();
        if (m)
        {
            RR_SHARED_PTR<ServiceEntryDefinition> entry = m->ServiceEntry.lock();
            if (entry)
            {
                def = entry->ServiceDefinition_.lock();
            }
        }
    }
    else
    {
        boost::string_ref def_name;
        boost::tie(def_name, entry_name) = SplitQualifiedName(TypeString);

        def = TryFindByName(imported_defs, def_name);
        if (!def)
        {
            if (!node)
                throw InvalidArgumentException("Node not specified for ResolveType");
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
            catch (std::exception&)
            {}
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

    throw ServiceDefinitionException("Could not resolve named type " + def->Name + "." + entry_name.to_string());
}

ExceptionDefinition::~ExceptionDefinition() {}

ExceptionDefinition::ExceptionDefinition(const RR_SHARED_PTR<ServiceDefinition>& service) { this->service = service; }

// NOLINTNEXTLINE(readability-make-member-function-const)
std::string ExceptionDefinition::ToString() { return "exception " + Name; }

void ExceptionDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    Reset();

    if (parse_info)
    {
        ParseInfo = *parse_info;
    }
    std::vector<std::string> tmp_name;
    ServiceDefinition_FromStringTypeFormat(s, "exception", tmp_name, ParseInfo);
    Name = tmp_name.at(0);
}

void ExceptionDefinition::Reset()
{
    Name.clear();
    DocString.clear();
    service.reset();
    ParseInfo.Reset();
}

UsingDefinition::~UsingDefinition() {}

UsingDefinition::UsingDefinition(const RR_SHARED_PTR<ServiceDefinition>& service) { this->service = service; }

// NOLINTNEXTLINE(readability-make-member-function-const)
std::string UsingDefinition::ToString()
{
    boost::string_ref qualified_name_type;
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
void UsingDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{

    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = s.to_string();

    static boost::regex r("^[ \\t]*using[ \\t]+(" RR_QUAIFIED_TYPE_REGEX ")(?:[ \\t]+as[ \\t](" RR_NAME_REGEX
                          "))?[ \\t]*$");

    ref_match r_match;
    if (!boost::regex_match(s.begin(), s.end(), r_match, r))
    {
        throw ServiceDefinitionParseException("Format error for using  definition", ParseInfo);
    }

    if (!r_match[2].matched)
    {
        this->QualifiedName = r_match[1];
        boost::string_ref unqualified_name;
        boost::tie(boost::tuples::ignore, unqualified_name) = SplitQualifiedName(to_ref(r_match[1]));
        this->UnqualifiedName = unqualified_name.to_string();
    }
    else
    {
        this->QualifiedName = r_match[1];
        this->UnqualifiedName = r_match[2];
    }
}

ConstantDefinition::~ConstantDefinition() {}

ConstantDefinition::ConstantDefinition(const RR_SHARED_PTR<ServiceDefinition>& service) { this->service = service; }
ConstantDefinition::ConstantDefinition(const RR_SHARED_PTR<ServiceEntryDefinition>& service_entry)
{
    this->service_entry = service_entry;
}

// NOLINTNEXTLINE(readability-make-member-function-const)
std::string ConstantDefinition::ToString() { return "constant " + Type->ToString() + " " + Name + " " + Value; }
void ConstantDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    Reset();

    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    if (ParseInfo.Line.empty())
        ParseInfo.Line = s.to_string();

    static boost::regex r("^[ \\t]*constant[ \\t]+(" RR_TYPE2_REGEX ")[ \\t]+(" RR_NAME_REGEX
                          ")[ \\t]+([^\\s](?:[ -~\\t]*[^\\s])?)[ \\t]*$");
    ref_match r_match;
    if (!boost::regex_match(s.begin(), s.end(), r_match, r))
    {
        throw ServiceDefinitionParseException("Invalid constant definition", ParseInfo);
    }

    std::string type_str = r_match[1];
    RR_SHARED_PTR<TypeDefinition> def = RR_MAKE_SHARED<TypeDefinition>();
    def->FromString(type_str, parse_info);
    if (!VerifyTypeAndValue(*def, to_ref(r_match[3])))
    {
        throw ServiceDefinitionParseException("Invalid constant definition", ParseInfo);
    }

    Type = def;
    Name = r_match[2];
    Value = r_match[3];
}

static bool ConstantDefinition_CheckScalar(DataTypes& t, boost::string_ref val)
{
    try
    {
        switch (t)
        {
        case DataTypes_double_t: {
            double v = 0.0;
            return detail::try_convert_string_to_number(val.to_string(), v);
        }
        case DataTypes_single_t: {
            float v = 0.0F;
            return detail::try_convert_string_to_number(std::string(val.begin(), val.end()), v);
        }
        case DataTypes_int8_t: {
            int8_t v = 0;
            return detail::try_convert_string_to_number(std::string(val.begin(), val.end()), v);
        }
        case DataTypes_uint8_t: {
            uint8_t v = 0;
            return detail::try_convert_string_to_number(std::string(val.begin(), val.end()), v);
        }
        case DataTypes_int16_t: {
            int16_t v = 0;
            return detail::try_convert_string_to_number(std::string(val.begin(), val.end()), v);
        }
        case DataTypes_uint16_t: {
            uint16_t v = 0;
            return detail::try_convert_string_to_number(std::string(val.begin(), val.end()), v);
        }
        case DataTypes_int32_t: {
            int32_t v = 0;
            return detail::try_convert_string_to_number(std::string(val.begin(), val.end()), v);
        }
        case DataTypes_uint32_t: {
            uint32_t v = 0;
            return detail::try_convert_string_to_number(std::string(val.begin(), val.end()), v);
        }
        case DataTypes_int64_t: {
            int64_t v = 0;
            return detail::try_convert_string_to_number(std::string(val.begin(), val.end()), v);
        }
        case DataTypes_uint64_t: {
            uint64_t v = 0;
            return detail::try_convert_string_to_number(std::string(val.begin(), val.end()), v);
        }
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

bool ConstantDefinition::VerifyValue() const { return VerifyTypeAndValue(*Type, Value); }

bool ConstantDefinition::VerifyTypeAndValue(TypeDefinition& t, boost::string_ref value)
{
    if (t.ArrayType == DataTypes_ArrayTypes_multidimarray)
        return false;
    if (IsTypeNumeric(t.Type))
    {
        if (t.Type == DataTypes_cdouble_t || t.Type == DataTypes_csingle_t || t.Type == DataTypes_bool_t)
            return false;
        if (t.ContainerType != DataTypes_ContainerTypes_none)
            return false;
        if (t.ArrayType == DataTypes_ArrayTypes_none)
        {
            if (t.Type == DataTypes_double_t || t.Type == DataTypes_single_t)
            {
                static boost::regex r_scalar("^[ \\t]*" RR_FLOAT_REGEX "[ \\t]*$");
                if (!regex_match(value.begin(), value.end(), r_scalar))
                    return false;
            }
            else
            {
                static boost::regex r_scalar("^[ \\t]*(?:(?:" RR_INT_REGEX ")|(?:" RR_HEX_REGEX "))[ \\t]*$");
                if (!regex_match(value.begin(), value.end(), r_scalar))
                    return false;
            }
            return ConstantDefinition_CheckScalar(t.Type, value);
        }
        else
        {
            ref_match r_array_match;
            if (t.Type == DataTypes_double_t || t.Type == DataTypes_single_t)
            {
                static boost::regex r_array("^[ \\t]*\\{[ \\t]*((?:" RR_FLOAT_REGEX
                                            ")(?:[ \\t]*,[ \\t]*(?:" RR_FLOAT_REGEX "))*)?[ \\t]*}[ \\t]*$");
                if (!regex_match(value.begin(), value.end(), r_array_match, r_array))
                    return false;
            }
            else
            {
                static boost::regex r_array("^[ \\t]*\\{[ \\t]*((?:(:?" RR_INT_REGEX ")|(?:" RR_HEX_REGEX
                                            "))(?:[ \\t]*,[ \\t]*(?:" RR_INT_REGEX "|" RR_HEX_REGEX
                                            "))*)?[ \\t]*}[ \\t]*$");
                if (!regex_match(value.begin(), value.end(), r_array_match, r_array))
                    return false;
            }

            if (!r_array_match[1].matched)
                return true;

            typedef boost::split_iterator<boost::string_ref::const_iterator> string_split_iterator;
            boost::iterator_range<boost::string_ref::const_iterator> r_array_match_1_rng(r_array_match[1].first,
                                                                                         r_array_match[1].second);
            for (string_split_iterator e = boost::make_split_iterator(
                     r_array_match_1_rng, boost::token_finder(boost::is_any_of(","), boost::token_compress_on));
                 e != string_split_iterator(); e++)
            {
                // TODO: improve performance
                std::string c(e->begin(), e->end());
                if (!ConstantDefinition_CheckScalar(t.Type, boost::trim_copy(c)))
                {
                    return false;
                }
            }
            return true;
        }
    }
    else
    {
        if (t.ArrayType != DataTypes_ArrayTypes_none)
            return false;
        if (t.Type == DataTypes_string_t)
        {
            static boost::regex r_string("^[ "
                                         "\\t]*\"(?:(?:\\\\\"|\\\\\\\\|\\\\/"
                                         "|\\\\b|\\\\f|\\\\n|\\\\r|\\\\t|\\\\u[\\da-fA-F]{4})|[^\"\\\\])*\"[ \\t]*$");
            return boost::regex_match(value.begin(), value.end(), r_string);
        }
        else if (t.Type == DataTypes_namedtype_t)
        {
            static boost::regex r_struct("^[ \\t]*\\{[ \\t]*(?:" RR_NAME_REGEX "[ \\t]*\\:[ \\t]*" RR_NAME_REGEX
                                         "(?:[ \\t]*,[ \\t]*" RR_NAME_REGEX "[ \\t]*\\:[ \\t]*" RR_NAME_REGEX
                                         ")*[ \\t]*)?\\}[ \\t]*$");
            return boost::regex_match(value.begin(), value.end(), r_struct);
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

// NOLINTNEXTLINE(readability-make-member-function-const)
std::string ConstantDefinition::ValueToString()
{
    if (!Type)
        throw InvalidOperationException("Invalid operation");
    if (Type->Type != DataTypes_string_t)
        throw InvalidOperationException("Invalid operation");

    static boost::regex r_string(
        "^[ \\t]*\"((?:(?:\\\\\"|\\\\\\\\|\\\\/|\\\\b|\\\\f|\\\\n|\\\\r|\\\\t|\\\\u[\\da-fA-F]{4})|(?:(?![\"\\\\])[ "
        "-~]))*)\"[ \\t]*$");
    boost::smatch r_string_match;
    if (!boost::regex_match(Value, r_string_match, r_string))
        throw ServiceDefinitionParseException("Invalid string constant format", ParseInfo);

    std::string value2 = r_string_match[1].str();
    return UnescapeString(value2);
}

static std::string ConstantDefinition_UnescapeString_Formatter(const boost::smatch& match)
{
    std::string i = match[0].str();
    if (i == "\\\"")
        return "\"";
    if (i == "\\\\")
        return "\\";
    if (i == "\\/")
        return "/";
    if (i == "\\b")
        return "\b";
    if (i == "\\f")
        return "\f";
    if (i == "\\n")
        return "\n";
    if (i == "\\r")
        return "\r";
    if (i == "\\t")
        return "\t";

    if (boost::starts_with(i, "\\u"))
    {
        std::stringstream ss;
#ifdef ROBOTRACONTEUR_EMSCRIPTEN
        std::basic_string<char16_t> v3;
#else
        std::basic_string<uint16_t> v3;
#endif
        v3.resize(i.size() / 6);
        for (size_t j = 0; j < v3.size(); j++)
        {
            std::string v = i.substr(j * 6 + 2, 4);
            ss.clear();
            uint16_t v2 = 0;
            ss << std::hex << v;
            ss >> std::hex >> v2;
            if (ss.fail() || !ss.eof())
            {
                throw InvalidArgumentException("Invalid escaped constant string");
            }
            v3[j] = v2;
        }
        return boost::locale::conv::utf_to_utf<char>(v3);
    }

    throw InternalErrorException("Internal error");
}

std::string ConstantDefinition::UnescapeString(boost::string_ref in)
{
    std::ostringstream t(std::ios::out | std::ios::binary);
    std::ostream_iterator<char, char> oi(t);

    static boost::regex r_string_expression(
        "(\\\\\"|\\\\\\\\|\\\\/|\\\\b|\\\\f|\\\\n|\\\\r|\\\\t|(?:\\\\u[\\da-fA-F]{4})+)");
    // const char* format_string = "(?1\\\")(?2\\\\)(?3\\b)(?4\\f)(?5\\n)(?6\\r)(?7\\t)(?8\\"
    // TODO: avoid copy
    const std::string in1 = in.to_string();
    boost::regex_replace(oi, in1.begin(), in1.end(), r_string_expression, ConstantDefinition_UnescapeString_Formatter,
                         boost::match_default | boost::format_all);

    return t.str();
}

static std::string ConstantDefinition_EscapeString_Formatter(const boost::smatch& match)
{
    std::string i = match[0].str();

    if (i == "\"")
        return "\\\"";
    if (i == "\\")
        return "\\\\";
    if (i == "/")
        return "\\/";
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

#ifdef ROBOTRACONTEUR_EMSCRIPTEN
    std::basic_string<char16_t> v = boost::locale::conv::utf_to_utf<char16_t>(i);
#else
    std::basic_string<uint16_t> v = boost::locale::conv::utf_to_utf<uint16_t>(i);
#endif

    std::stringstream v2;
    v2 << std::hex << std::setfill('0');
    BOOST_FOREACH (const uint16_t& v3, v)
    {
        v2 << std::setw(0) << "\\u" << std::setw(4) << v3;
    }

    return v2.str();
}

std::string ConstantDefinition::EscapeString(boost::string_ref in)
{
    static boost::regex r_replace("(\"|\\\\|\\/|[\\x00-\\x1F]|\\x7F|[\\x80-\\xFF]+)");

    std::ostringstream t(std::ios::out | std::ios::binary);
    std::ostream_iterator<char, char> oi(t);

    // TODO: avoid copy
    const std::string in1 = in.to_string();
    boost::regex_replace(oi, in1.begin(), in1.end(), r_replace, ConstantDefinition_EscapeString_Formatter,
                         boost::match_default | boost::format_all);

    return t.str();
}

std::vector<ConstantDefinition_StructField> ConstantDefinition::ValueToStructFields() const
{

    std::vector<ConstantDefinition_StructField> o;
    boost::iterator_range<std::string::const_iterator> value1(Value);
    value1 = boost::trim_copy_if(value1, boost::is_any_of(" \t{}"));
    value1 = boost::trim_copy(value1);

    if (value1.empty())
        return o;

    static boost::regex r("[ \\t]*(" RR_NAME_REGEX ")[ \\t]*\\:[ \\t]*(" RR_NAME_REGEX ")[ \\t]*");

    typedef boost::split_iterator<std::string::const_iterator> string_split_iterator;
    for (string_split_iterator e =
             boost::make_split_iterator(value1, boost::token_finder(boost::is_any_of(","), boost::token_compress_on));
         e != string_split_iterator(); e++)
    {

        boost::smatch r_match;
        if (!boost::regex_match(e->begin(), e->end(), r_match, r))
        {
            throw ServiceDefinitionParseException("Invalid struct constant format", ParseInfo);
        }
        ConstantDefinition_StructField f;
        f.Name = r_match[1];
        f.ConstantRefName = r_match[2];
        o.push_back(f);
    }

    return o;
}

EnumDefinition::~EnumDefinition() {}

EnumDefinition::EnumDefinition(const RR_SHARED_PTR<ServiceDefinition>& service)
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

    BOOST_FOREACH (const EnumDefinitionValue& e, Values)
    {
        if (e.ImplicitValue)
        {
            values.push_back("    " + e.Name);
        }
        else
        {
            if (!e.HexValue)
            {
                values.push_back("    " + e.Name + " = " + boost::lexical_cast<std::string>(e.Value));
            }
            else
            {
                if (e.Value >= 0)
                {
                    std::stringstream ss;
                    ss << std::hex << e.Value;
                    values.push_back("    " + e.Name + " = 0x" + ss.str());
                }
                else
                {
                    std::stringstream ss;
                    ss << std::hex << -e.Value;
                    values.push_back("    " + e.Name + " = -0x" + ss.str());
                }
            }
        }
    }
    s += boost::join(values, ",\n");
    s += "\nend\n";
    return s;
}

void EnumDefinition::FromString(boost::string_ref s, const ServiceDefinitionParseInfo* parse_info)
{
    Reset();

    if (parse_info)
    {
        ParseInfo = *parse_info;
    }

    std::string s2 = boost::trim_copy(s.to_string());
    std::vector<std::string> lines;
    boost::split(lines, s2, boost::is_any_of("\n"));
    if (lines.size() < 2)
        throw ServiceDefinitionParseException("Invalid enum", ParseInfo);

    if (ParseInfo.Line.empty())
        ParseInfo.Line = lines.front();

    static boost::regex r_start("^[ \\t]*enum[ \\t]+([a-zA-Z]\\w*)[ \\t]*$");
    static boost::regex r_end("^[ \\t]*end(?:[ \\t]+enum)?[ \\t]*$");

    ServiceDefinitionParseInfo working_info = ParseInfo;
    working_info.LineNumber += boost::numeric_cast<int32_t>(lines.size()) - 1;
    working_info.Line = lines.back();

    boost::smatch r_start_match;
    if (!boost::regex_match(lines.front(), r_start_match, r_start))
    {
        throw ServiceDefinitionParseException("Parse error", ParseInfo);
    }
    Name = r_start_match[1];

    if (!boost::regex_match(lines.back(), r_end))
    {
        ServiceDefinitionParseInfo working_info2 = ParseInfo;
        working_info2.LineNumber += boost::numeric_cast<int32_t>(lines.size());
        working_info2.Line = lines.back();
        throw ServiceDefinitionParseException("Parse error near: " + lines.front(), working_info2);
    }

    // std::string values1 = boost::join(boost::make_iterator_range(++lines.begin(), --lines.end()), " ");
    static boost::regex r_docstring("^[ \\t]*##([ -~\\t]*)$");
    static boost::regex r_empty("^[ \\t]*$");
    bool empty_encountered_docstring = false;
    bool comma_endl = true;
    std::vector<std::string> docstring_v;
    BOOST_FOREACH (std::string line, boost::make_iterator_range(++lines.begin(), --lines.end()))
    {
        boost::smatch r_docstring_match;
        if (boost::regex_match(line, r_docstring_match, r_docstring))
        {
            if (empty_encountered_docstring)
            {
                empty_encountered_docstring = false;
                docstring_v.clear();
            }
            docstring_v.push_back(r_docstring_match[1].str());
            continue;
        }

        if (boost::regex_match(line, r_empty))
        {
            empty_encountered_docstring = true;
            continue;
        }

        if (!comma_endl)
        {
            throw ServiceDefinitionParseException("Enum parse error", working_info);
        }

        boost::trim(line);
        comma_endl = boost::ends_with(line, ",");
        boost::trim_right_if(line, boost::is_any_of(","));

        std::vector<std::string> values2;
        boost::split(values2, line, boost::is_any_of(","));

        static boost::regex r_value("^[ \\t]*([A-Za-z]\\w*)(?:[ \\t]*=[ \\t]*(?:(" RR_HEX_REGEX ")|(" RR_INT_REGEX
                                    ")))?[ \\t]*$");

        BOOST_FOREACH (const std::string& l, values2)
        {
            boost::smatch r_value_match;
            if (!boost::regex_match(l, r_value_match, r_value))
            {
                throw ServiceDefinitionParseException("Enum parse error", working_info);
            }

            Values.resize(Values.size() + 1);
            EnumDefinitionValue& enum_i = Values.back();
            if (!docstring_v.empty())
            {
                enum_i.DocString = boost::join(docstring_v, "\n");
            }
            empty_encountered_docstring = false;
            docstring_v.clear();
            enum_i.Name = r_value_match[1];

            if (r_value_match[2].matched)
            {
                try
                {
                    std::stringstream ss;
                    ss << std::hex << std::string(r_value_match[2]);
                    int64_t v = 0;
                    ss >> v;
                    if (ss.fail() || !ss.eof())
                    {
                        throw InvalidArgumentException("Bad hex number");
                    }
                    enum_i.ImplicitValue = false;
                    enum_i.Value = boost::numeric_cast<int32_t>(v);
                    enum_i.HexValue = true;
                }
                catch (std::exception&)
                {
                    throw ServiceDefinitionParseException("Enum parse error", working_info);
                }
            }
            else if (r_value_match[3].matched)
            {
                try
                {
                    enum_i.ImplicitValue = false;
                    enum_i.Value = boost::lexical_cast<int32_t>(r_value_match[3]);
                    enum_i.HexValue = false;
                }
                catch (std::exception&)
                {
                    throw ServiceDefinitionParseException("Enum parse error", working_info);
                }
            }
            else
            {
                if (Values.size() == 1)
                {
                    throw ServiceDefinitionParseException("Enum first value must be specified", working_info);
                }

                enum_i.ImplicitValue = true;
                enum_i.Value = (Values.rbegin() + 1)->Value + 1;
                enum_i.HexValue = (Values.rbegin() + 1)->HexValue;
            }
        }
    }

    if (!VerifyValues())
    {
        throw ServiceDefinitionParseException("Enum names or values not unique", working_info);
    }
}
bool EnumDefinition::VerifyValues() const
{
    if (Values.size() == 1)
    {
        return true;
    }

    std::vector<EnumDefinitionValue>::const_iterator e = Values.begin();
    std::vector<EnumDefinitionValue>::const_iterator e_end = Values.end()--;

    std::vector<EnumDefinitionValue>::const_iterator e2_end = Values.end();
    for (; e != e_end; e++)
    {
        std::vector<EnumDefinitionValue>::const_iterator e2 = e + 1;
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

void EnumDefinition::Reset() { Values.clear(); }

DataTypes EnumDefinition::RRDataType() const { return DataTypes_enum_t; }

std::string EnumDefinition::ResolveQualifiedName()
{
    RR_SHARED_PTR<ServiceDefinition> def = service.lock();
    if (!def)
        throw InvalidOperationException("Could not lock service definition to resolve named type");
    return def->Name + "." + Name;
}

EnumDefinitionValue::EnumDefinitionValue()
{
    Value = 0;
    ImplicitValue = false;
    HexValue = false;
    Name.clear();
}

ServiceDefinitionParseInfo::ServiceDefinitionParseInfo() { LineNumber = 0; }

void ServiceDefinitionParseInfo::Reset()
{
    LineNumber = 0;
    Line.clear();
    RobDefFilePath.clear();
    ServiceName.clear();
}

ServiceDefinitionParseException::ServiceDefinitionParseException(const std::string& e) : ServiceDefinitionException(e)
{
    ShortMessage = e;
    ParseInfo.LineNumber = -1;
    Message = ServiceDefinitionParseException::ToString();
    what_store = Message;
}

ServiceDefinitionParseException::ServiceDefinitionParseException(const std::string& e,
                                                                 const ServiceDefinitionParseInfo& info)
    : ServiceDefinitionException(e)
{
    ShortMessage = e;
    ParseInfo = info;
    Message = ServiceDefinitionParseException::ToString();
    what_store = Message;
}

std::string ServiceDefinitionParseException::ToString() const
{
    if (!ParseInfo.ServiceName.empty())
    {
        return "Parse error on line " + boost::lexical_cast<std::string>(ParseInfo.LineNumber) + " in " +
               ParseInfo.ServiceName + ": " + Message;
    }
    else if (!ParseInfo.Line.empty())
    {
        return "Parse error in \"" + ParseInfo.Line + "\": " + ShortMessage;
    }
    else
    {
        return "Parse error: " + ShortMessage;
    }
}

const char* ServiceDefinitionParseException::what() const throw() { return what_store.c_str(); }

ServiceDefinitionVerifyException::ServiceDefinitionVerifyException(const std::string& e) : ServiceDefinitionException(e)
{
    ShortMessage = e;
    Message = ServiceDefinitionVerifyException::ToString();
    ParseInfo.LineNumber = -1;
    what_store = Message;
}

ServiceDefinitionVerifyException::ServiceDefinitionVerifyException(const std::string& e,
                                                                   const ServiceDefinitionParseInfo& info)
    : ServiceDefinitionException(e)
{
    ShortMessage = e;
    Message = ServiceDefinitionVerifyException::ToString();
    ParseInfo = info;
    what_store = Message;
}

std::string ServiceDefinitionVerifyException::ToString() const
{
    if (!ParseInfo.ServiceName.empty())
    {
        return "Verify error on line " + boost::lexical_cast<std::string>(ParseInfo.LineNumber) + " in " +
               ParseInfo.ServiceName + ": " + ShortMessage;
    }
    else if (!ParseInfo.Line.empty())
    {
        return "Verify error in \"" + ParseInfo.Line + "\": " + ShortMessage;
    }
    else
    {
        return "Verify error: " + ShortMessage;
    }
}

const char* ServiceDefinitionVerifyException::what() const throw() { return what_store.c_str(); }

// Code to verify the service definition.  Most of these functions are not made available in the header.

static void VerifyVersionSupport(const RR_SHARED_PTR<ServiceDefinition>& def, int32_t major, int32_t minor,
                                 const char* msg)
{
    RobotRaconteurVersion def_version = def->StdVer;
    if (!def_version)
        return;

    if (def_version < RobotRaconteurVersion(major, minor))
    {
        if (msg)
        {
            throw ServiceDefinitionVerifyException(msg, def->ParseInfo);
        }
        else
        {
            throw ServiceDefinitionVerifyException("Newer service definition standard required for feature",
                                                   def->ParseInfo);
        }
    }
}

void VerifyName(const std::string& name, const RR_SHARED_PTR<ServiceDefinition>& def,
                const ServiceDefinitionParseInfo& parse_info, bool allowdot = false, bool ignorereserved = false)
{
    RR_UNUSED(def);
    if (name.length() == 0)
        throw ServiceDefinitionVerifyException("name must not be empty", parse_info);

    std::string name2 = boost::to_lower_copy(name);

    if (!ignorereserved)
    {
        if (name == "this" || name == "self" || name == "Me")
            throw ServiceDefinitionVerifyException("The names \"this\", \"self\", and \"Me\" are reserved", parse_info);

        // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays)
        const char* res_str[] = {"object",    "end",       "option",   "service",  "struct", "import", "implements",
                                 "field",     "property",  "function", "event",    "objref", "pipe",   "callback",
                                 "wire",      "memory",    "void",     "int8",     "uint8",  "int16",  "uint16",
                                 "int32",     "uint32",    "int64",    "uint64",   "single", "double", "varvalue",
                                 "varobject", "exception", "using",    "constant", "enum",   "pod",    "namedarray",
                                 "cdouble",   "csingle",   "bool",     "stdver",   "string"};
        // NOLINTEND(cppcoreguidelines-avoid-c-arrays)

        std::vector<std::string> reserved(res_str, res_str + sizeof(res_str) / (sizeof(res_str[0])));

        if (boost::range::find(reserved, name) != reserved.end())
        {
            throw ServiceDefinitionVerifyException("Name \"" + name + "\" is reserved", parse_info);
        }

        if (boost::starts_with(name2, "get_") || boost::starts_with(name2, "set_") || boost::starts_with(name2, "rr") ||
            boost::starts_with(name2, "robotraconteur") || boost::starts_with(name2, "async_"))
        {
            throw ServiceDefinitionVerifyException("Name \"" + name + "\" is reserved or invalid", parse_info);
        }
    }

    if (allowdot)
    {
        if (!boost::regex_match(name, boost::regex("^" RR_TYPE_REGEX "$")))
        {
            throw ServiceDefinitionVerifyException("Name \"" + name + "\" is invalid", parse_info);
        }
    }
    else
    {
        if (!boost::regex_match(name, boost::regex("^" RR_NAME_REGEX "$")))
        {
            throw ServiceDefinitionVerifyException("Name \"" + name + "\" is invalid", parse_info);
        }
    }
}

std::string VerifyConstant(boost::string_ref constant, const RR_SHARED_PTR<ServiceDefinition>& def,
                           const ServiceDefinitionParseInfo& parse_info)
{
    RR_SHARED_PTR<ConstantDefinition> c = RR_MAKE_SHARED<ConstantDefinition>(def);
    try
    {
        c->FromString(constant, &parse_info);
    }
    catch (std::exception&)
    {
        throw ServiceDefinitionVerifyException("could not parse constant \"" + constant.to_string() + "\"", parse_info);
    }

    if (!c->VerifyValue())
        throw ServiceDefinitionVerifyException("Error in constant " + c->Name, parse_info);

    if (c->Type->Type == DataTypes_namedtype_t)
        throw ServiceDefinitionVerifyException("Error in constant " + c->Name, parse_info);

    VerifyName(c->Name, def, parse_info);

    return c->Name;
}

void VerifyConstantStruct(const RR_SHARED_PTR<ConstantDefinition>& c, const RR_SHARED_PTR<ServiceDefinition>& def,
                          std::vector<RR_SHARED_PTR<ConstantDefinition> >& constants,
                          std::vector<std::string> parent_types)
{
    std::vector<ConstantDefinition_StructField> fields;
    fields = c->ValueToStructFields();
    parent_types.push_back(c->Name);
    BOOST_FOREACH (ConstantDefinition_StructField& e, fields)
    {
        VerifyName(e.Name, def, c->ParseInfo);
        BOOST_FOREACH (const std::string& name, parent_types)
        {
            if (e.ConstantRefName == name)
                throw ServiceDefinitionVerifyException(
                    "Error in constant " + c->Name + ": recursive struct not allowed", c->ParseInfo);
        }
        bool found = false;
        BOOST_FOREACH (RR_SHARED_PTR<ConstantDefinition>& f, constants)
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

        if (!found)
            throw ServiceDefinitionVerifyException(
                "Error in constant " + c->Name + ": struct field " + e.ConstantRefName + " not found", c->ParseInfo);
    }
}

std::string VerifyConstant(const RR_SHARED_PTR<ConstantDefinition>& c, const RR_SHARED_PTR<ServiceDefinition>& def,
                           std::vector<RR_SHARED_PTR<ConstantDefinition> >& constants)
{
    if (!c->VerifyValue())
        throw ServiceDefinitionVerifyException("Error in constant " + c->Name, c->ParseInfo);
    VerifyName(c->Name, def, c->ParseInfo);

    if (c->Type->Type == DataTypes_namedtype_t)
    {
        VerifyConstantStruct(c, def, constants, std::vector<std::string>());
    }

    return c->Name;
}

void VerifyEnum(EnumDefinition& e, const RR_SHARED_PTR<ServiceDefinition>& def)
{
    if (!e.VerifyValues())
    {
        throw ServiceDefinitionVerifyException("Invalid constant in enum " + e.Name, e.ParseInfo);
    }

    VerifyName(e.Name, def, e.ParseInfo);
    BOOST_FOREACH (const EnumDefinitionValue& e1, e.Values)
    {
        VerifyName(e1.Name, def, e.ParseInfo);
    }
}

std::vector<std::string> GetServiceNames(const RR_SHARED_PTR<ServiceDefinition>& def)
{
    std::vector<std::string> o;
    BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& e, def->Objects)
    {
        o.push_back(e->Name);
    }
    BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& e, def->Structures)
    {
        o.push_back(e->Name);
    }
    BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& e, def->Pods)
    {
        o.push_back(e->Name);
    }
    BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& e, def->NamedArrays)
    {
        o.push_back(e->Name);
    }
    BOOST_FOREACH (RR_SHARED_PTR<ConstantDefinition>& e, def->Constants)
    {
        o.push_back(e->Name);
    }
    BOOST_FOREACH (RR_SHARED_PTR<EnumDefinition>& e, def->Enums)
    {
        o.push_back(e->Name);
    }
    BOOST_FOREACH (RR_SHARED_PTR<ExceptionDefinition>& e, def->Exceptions)
    {
        o.push_back(e->Name);
    }

    return o;
}

void VerifyUsing(UsingDefinition& e, const RR_SHARED_PTR<ServiceDefinition>& def,
                 std::vector<RR_SHARED_PTR<ServiceDefinition> >& importeddefs)
{
    VerifyName(e.UnqualifiedName, def, e.ParseInfo);
    static boost::regex r(RR_QUAIFIED_TYPE_REGEX);
    if (!boost::regex_match(e.QualifiedName, r))
    {
        throw ServiceDefinitionVerifyException("Using \"" + e.QualifiedName + "\" is invalid", e.ParseInfo);
    }

    boost::tuple<boost::string_ref, boost::string_ref> s1 = SplitQualifiedName(e.QualifiedName);

    BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& d1, importeddefs)
    {
        if (s1.get<0>() == d1->Name)
        {
            std::vector<std::string> importeddefs_names = GetServiceNames(d1);
            if (boost::range::find(importeddefs_names, s1.get<1>()) == importeddefs_names.end())
            {
                throw ServiceDefinitionVerifyException("Using \"" + e.QualifiedName + "\" is invalid", e.ParseInfo);
            }
            return;
        }
    }

    throw ServiceDefinitionVerifyException("Using \"" + e.QualifiedName + "\" is invalid", e.ParseInfo);
}

RR_SHARED_PTR<NamedTypeDefinition> VerifyResolveNamedType(
    const RR_SHARED_PTR<TypeDefinition>& tdef, const std::vector<RR_SHARED_PTR<ServiceDefinition> >& imported_defs)
{
    try
    {
        return tdef->ResolveNamedType(imported_defs);
    }
    catch (std::exception&)
    {
        throw ServiceDefinitionVerifyException("could not resolve named type \"" + tdef->TypeString + "\"",
                                               tdef->ParseInfo);
    }
}

void VerifyType(const RR_SHARED_PTR<TypeDefinition>& t, const RR_SHARED_PTR<ServiceDefinition>& def,
                const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs)
{
    RR_UNUSED(def);
    switch (t->ArrayType)
    {
    case DataTypes_ArrayTypes_none:
    case DataTypes_ArrayTypes_array:
    case DataTypes_ArrayTypes_multidimarray:
        break;
    default:
        throw ServiceDefinitionVerifyException("Invalid Robot Raconteur data type \"" + t->ToString() + "\"",
                                               t->ParseInfo);
    }

    switch (t->ContainerType)
    {
    case DataTypes_ContainerTypes_none:
    case DataTypes_ContainerTypes_list:
    case DataTypes_ContainerTypes_map_int32:
    case DataTypes_ContainerTypes_map_string:
        break;
    default:
        throw ServiceDefinitionVerifyException("Invalid Robot Raconteur data type \"" + t->ToString() + "\"",
                                               t->ParseInfo);
    }

    if (IsTypeNumeric(t->Type))
    {
        return;
    }
    if (t->Type == DataTypes_string_t)
    {
        if (t->ArrayType != DataTypes_ArrayTypes_none)
            throw ServiceDefinitionVerifyException("Invalid Robot Raconteur data type \"" + t->ToString() + "\"",
                                                   t->ParseInfo);

        return;
    }
    if (t->Type == DataTypes_vector_t || t->Type == DataTypes_dictionary_t || t->Type == DataTypes_object_t ||
        t->Type == DataTypes_varvalue_t || t->Type == DataTypes_varobject_t || t->Type == DataTypes_multidimarray_t)
        return;
    if (t->Type == DataTypes_namedtype_t)
    {
        RR_SHARED_PTR<NamedTypeDefinition> nt = VerifyResolveNamedType(t, defs);
        DataTypes nt_type = nt->RRDataType();
        if ((nt_type != DataTypes_pod_t && nt_type != DataTypes_namedarray_t) &&
            t->ArrayType != DataTypes_ArrayTypes_none)
            throw ServiceDefinitionVerifyException("Invalid Robot Raconteur data type \"" + t->ToString() + "\"",
                                                   t->ParseInfo);
        if (nt_type != DataTypes_structure_t && nt_type != DataTypes_pod_t && nt_type != DataTypes_namedarray_t &&
            nt_type != DataTypes_enum_t)
            throw ServiceDefinitionVerifyException("Invalid Robot Raconteur data type \"" + t->ToString() + "\"",
                                                   t->ParseInfo);
        if (nt_type == DataTypes_pod_t)
        {}
        return;
    }
    throw ServiceDefinitionVerifyException("Invalid Robot Raconteur data type \"" + t->ToString() + "\"", t->ParseInfo);
}

void VerifyReturnType(const RR_SHARED_PTR<TypeDefinition>& t, const RR_SHARED_PTR<ServiceDefinition>& def,
                      const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs)
{
    if (t->Type == DataTypes_void_t)
    {
        if (t->ArrayType != DataTypes_ArrayTypes_none || t->ContainerType != DataTypes_ContainerTypes_none)
        {
            throw ServiceDefinitionVerifyException("Invalid Robot Raconteur data type \"" + t->ToString() + "\"",
                                                   t->ParseInfo);
        }
        return;
    }
    else
    {
        VerifyType(t, def, defs);
    }
}

void VerifyParameters(const std::vector<RR_SHARED_PTR<TypeDefinition> >& p, const RR_SHARED_PTR<ServiceDefinition>& def,
                      const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs)
{
    std::vector<std::string*> names;
    BOOST_FOREACH (const RR_SHARED_PTR<TypeDefinition>& t, p)
    {
        VerifyType(t, def, defs);
        if (boost::range::find(names | boost::adaptors::indirected, t->Name) != boost::adaptors::indirect(names).end())
            throw ServiceDefinitionVerifyException("Parameters must have unique names", t->ParseInfo);
        names.push_back(&t->Name);
    }
}

void VerifyModifier(boost::string_ref modifier_str, const RR_SHARED_PTR<MemberDefinition>& m,
                    std::vector<ServiceDefinitionParseException>& warnings)
{
    static boost::regex r_modifier("^[ \\t]*" RR_NAME_REGEX "(?:\\([ \\t]*((?:" RR_NUMBER_REGEX "|" RR_NAME_REGEX
                                   ")[ \\t]*(?:,[ \\t]*(?:" RR_NUMBER_REGEX "|" RR_NAME_REGEX "))*)[ \\t]*\\))?");
    ref_match r_match;

    if (!boost::regex_match(modifier_str.begin(), modifier_str.end(), r_match, r_modifier))
    {
        throw ServiceDefinitionVerifyException("Invalid modifier: [" + modifier_str.to_string() + "]", m->ParseInfo);
    }

    if (r_match[1].matched)
    {
        std::vector<std::string> modifier_params;
        std::string r_match1 = r_match[1].str();
        boost::split(modifier_params, r_match1, boost::is_any_of(","));
        if (modifier_params.empty())
        {
            throw ServiceDefinitionVerifyException("Invalid modifier parameters: [" + modifier_str.to_string() + "]",
                                                   m->ParseInfo);
        }

        BOOST_FOREACH (std::string& p, modifier_params)
        {
            boost::trim(p);
            static boost::regex r_name(RR_NAME_REGEX);
            if (boost::regex_match(p, r_name))
            {
                RR_SHARED_PTR<ServiceEntryDefinition> entry_def = m->ServiceEntry.lock();
                if (!entry_def)
                {
                    warnings.push_back(ServiceDefinitionParseException(
                        "Could not verify modifier parameters: [" + modifier_str.to_string() + "]", m->ParseInfo));
                    continue;
                }

                RR_SHARED_PTR<ConstantDefinition> param_const = TryFindByName(entry_def->Constants, p);
                if (!param_const)
                {
                    warnings.push_back(ServiceDefinitionParseException(
                        "Could not verify modifier parameters: [" + modifier_str.to_string() + "]", m->ParseInfo));
                }
            }
        }
    }
}

void VerifyModifiers(const RR_SHARED_PTR<MemberDefinition>& m, bool readwrite, bool unreliable, bool nolock,
                     bool nolockread, bool perclient, bool urgent,
                     std::vector<ServiceDefinitionParseException>& warnings)
{
    bool direction_found = false;
    bool unreliable_found = false;
    bool nolock_found = false;
    bool perclient_found = false;
    bool urgent_found = false;

    BOOST_FOREACH (std::string& s, m->Modifiers)
    {
        if (readwrite)
        {
            if (s == "readonly" || s == "writeonly")
            {
                if (direction_found)
                {
                    warnings.push_back(ServiceDefinitionParseException(
                        "Invalid member modifier combination: [readonly,writeonly]", m->ParseInfo));
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
                    BOOST_FOREACH (const std::string& o, obj->Options)
                    {
                        static boost::regex r("^[ \\t]*pipe[ \\t]+" + m->Name + "[ \\t]+unreliable[ \\t]*$");
                        if (boost::regex_match(o, r))
                        {
                            warnings.push_back(ServiceDefinitionParseException(
                                "Invalid member modifier combination: [unreliable]", m->ParseInfo));
                        }
                    }
                }

                if (unreliable_found)
                {
                    warnings.push_back(ServiceDefinitionParseException(
                        "Invalid member modifier combination: [unreliable]", m->ParseInfo));
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
                    warnings.push_back(
                        ServiceDefinitionParseException("Invalid member modifier combination: [nolock]", m->ParseInfo));
                }
                nolock_found = true;
                continue;
            }
        }

        if (nolockread)
        {
            if (s == "nolockread")
            {
                if (nolock_found)
                {
                    warnings.push_back(ServiceDefinitionParseException(
                        "Invalid member modifier combination: [nolock,nolockread]", m->ParseInfo));
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
                    warnings.push_back(ServiceDefinitionParseException(
                        "Invalid member modifier combination: [perclient]", m->ParseInfo));
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
                    warnings.push_back(
                        ServiceDefinitionParseException("Invalid member modifier combination: [urgent]", m->ParseInfo));
                }
                urgent_found = true;
                continue;
            }
        }

        VerifyModifier(s, m, warnings);

        warnings.push_back(ServiceDefinitionParseException("Unknown member modifier: [" + s + "]", m->ParseInfo));
    }
}

std::string VerifyMember(const RR_SHARED_PTR<MemberDefinition>& m, const RR_SHARED_PTR<ServiceDefinition>& def,
                         const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs,
                         std::vector<ServiceDefinitionParseException>& warnings)
{
    VerifyName(m->Name, def, m->ParseInfo);

    if (!m->Modifiers.empty())
    {
        VerifyVersionSupport(def, 0, 9,
                             "Service definition standard version 0.9 or greater required for Member Modifiers");
    }

    RR_SHARED_PTR<PropertyDefinition> p = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(m);
    if (p)
    {
        VerifyType(p->Type, def, defs);
        VerifyModifiers(m, true, false, true, true, true, true, warnings);
        return p->Name;
    }
    RR_SHARED_PTR<FunctionDefinition> f = RR_DYNAMIC_POINTER_CAST<FunctionDefinition>(m);
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
            bool generator_found = false;
            ;
            if (f->ReturnType->ContainerType == DataTypes_ContainerTypes_generator)
            {
                if (f->ReturnType->Type == DataTypes_void_t)
                {
                    throw ServiceDefinitionVerifyException("Generator return must not be void", f->ParseInfo);
                }

                RR_SHARED_PTR<TypeDefinition> ret2 = f->ReturnType->Clone();
                ret2->RemoveContainers();
                VerifyType(ret2, def, defs);
                if (f->ReturnType->Type == DataTypes_namedtype_t)
                    VerifyResolveNamedType(f->ReturnType, defs);
                generator_found = true;
            }
            else
            {
                if (f->ReturnType->Type != DataTypes_void_t)
                {
                    throw ServiceDefinitionVerifyException("Generator return must use generator container",
                                                           f->ParseInfo);
                }
                // VerifyReturnType(f->ReturnType, def, defs);
            }

            if (!f->Parameters.empty() && f->Parameters.back()->ContainerType == DataTypes_ContainerTypes_generator)
            {
                RR_SHARED_PTR<TypeDefinition> p2 = f->Parameters.back()->Clone();
                p2->RemoveContainers();
                VerifyType(p2, def, defs);
                if (f->Parameters.back()->Type == DataTypes_namedtype_t)
                    VerifyResolveNamedType(f->Parameters.back(), defs);
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
                throw ServiceDefinitionVerifyException("Generator return or parameter not found", f->ParseInfo);
            }

            return f->Name;
        }
    }
    RR_SHARED_PTR<EventDefinition> e = RR_DYNAMIC_POINTER_CAST<EventDefinition>(m);
    if (e)
    {
        VerifyParameters(e->Parameters, def, defs);
        VerifyModifiers(m, false, false, false, false, false, true, warnings);
        return e->Name;
    }
    RR_SHARED_PTR<ObjRefDefinition> o = RR_DYNAMIC_POINTER_CAST<ObjRefDefinition>(m);
    if (o)
    {

        std::vector<std::string> modifiers;
        VerifyModifiers(m, false, false, false, false, false, false, warnings);

        if (o->ObjectType == "varobject")
            return o->Name;

        if (!boost::contains(o->ObjectType, "."))
        {
            BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& ee, def->Objects)
            {
                if (ee->Name == o->ObjectType)
                    return o->Name;
            }
        }
        else
        {

            boost::tuple<boost::string_ref, boost::string_ref> s1 = SplitQualifiedName(o->ObjectType);

            boost::string_ref defname = s1.get<0>();
            RR_SHARED_PTR<ServiceDefinition> def2;

            BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& ee2, defs)
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
        throw ServiceDefinitionVerifyException("Unknown object type \"" + o->ObjectType + "\"", o->ParseInfo);
    }

    RR_SHARED_PTR<PipeDefinition> p2 = RR_DYNAMIC_POINTER_CAST<PipeDefinition>(m);
    if (p2)
    {
        VerifyType(p2->Type, def, defs);
        VerifyModifiers(m, true, true, true, false, false, false, warnings);
        return p2->Name;
    }

    RR_SHARED_PTR<CallbackDefinition> c = RR_DYNAMIC_POINTER_CAST<CallbackDefinition>(m);
    if (c)
    {
        VerifyParameters(c->Parameters, def, defs);
        VerifyReturnType(c->ReturnType, def, defs);
        VerifyModifiers(m, false, false, false, false, false, true, warnings);
        return c->Name;
    }

    RR_SHARED_PTR<WireDefinition> w = RR_DYNAMIC_POINTER_CAST<WireDefinition>(m);
    if (w)
    {
        VerifyType(w->Type, def, defs);
        VerifyModifiers(m, true, false, true, false, false, false, warnings);
        return w->Name;
    }

    RR_SHARED_PTR<MemoryDefinition> m2 = RR_DYNAMIC_POINTER_CAST<MemoryDefinition>(m);
    if (m2)
    {
        VerifyType(m2->Type, def, defs);
        VerifyModifiers(m, true, false, true, true, false, false, warnings);
        if (!IsTypeNumeric(m2->Type->Type))
        {
            if (m2->Type->Type != DataTypes_namedtype_t)
            {
                throw ServiceDefinitionVerifyException("Memory member must be numeric, pod, or namedarray",
                                                       m2->ParseInfo);
            }
            RR_SHARED_PTR<NamedTypeDefinition> nt = VerifyResolveNamedType(m2->Type, defs);
            if (nt->RRDataType() != DataTypes_pod_t && nt->RRDataType() != DataTypes_namedarray_t)
            {
                throw ServiceDefinitionVerifyException("Memory member must be numeric, pod, or namedarray",
                                                       m2->ParseInfo);
            }
        }
        switch (m2->Type->ArrayType)
        {
        case DataTypes_ArrayTypes_array:
        case DataTypes_ArrayTypes_multidimarray:
            break;
        default:
            throw ServiceDefinitionVerifyException("Memory member must be array or multidimarray", m2->ParseInfo);
        }

        if (!m2->Type->ArrayVarLength)
        {
            throw ServiceDefinitionVerifyException("Memory member must not be fixed size", m2->ParseInfo);
        }

        if (!m2->Type->ArrayLength.empty())
        {
            int32_t array_count = boost::accumulate(m2->Type->ArrayLength, 1, std::multiplies<int32_t>());
            if (array_count != 0)
            {
                throw ServiceDefinitionVerifyException("Memory member must not be fixed size", m2->ParseInfo);
            }
        }

        return m2->Name;
    }

    if (m2)
    {
        throw ServiceDefinitionVerifyException("Invalid member \"" + m->Name + "\"", m2->ParseInfo);
    }
    else
    {
        throw ServiceDefinitionVerifyException("Invalid member \"" + m->Name + "\"");
    }
}

struct rrimplements
{
    std::string name;
    RR_SHARED_PTR<ServiceEntryDefinition> obj;
    std::vector<rrimplements> implements;
};

rrimplements get_implements(const RR_SHARED_PTR<ServiceEntryDefinition>& obj,
                            const RR_SHARED_PTR<ServiceDefinition>& def,
                            const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs,
                            ServiceDefinitionParseInfo& parse_info, std::string rootobj = "")
{
    rrimplements out;
    out.obj = obj;
    out.name = def->Name + "." + obj->Name;

    if (rootobj.empty())
        rootobj = out.name;

    BOOST_FOREACH (std::string& e, obj->Implements)
    {
        if (!boost::contains(e, "."))
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

            if (!obj2)
                throw ServiceDefinitionVerifyException(
                    "Object \"" + def->Name + "." + e + " not found to implement in " + out.name, parse_info);

            if (rootobj == (def->Name + "." + obj2->Name))
                throw ServiceDefinitionVerifyException("Recursive implements between \"" + rootobj + "\" and \"" +
                                                           def->Name + "." + obj2->Name + "\"",
                                                       parse_info);

            rrimplements imp2 = get_implements(obj2, def, defs, parse_info, rootobj);
            out.implements.push_back(imp2);
        }
        else
        {
            boost::tuple<boost::string_ref, boost::string_ref> s1 = SplitQualifiedName(e);

            RR_SHARED_PTR<ServiceDefinition> def2;

            BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& ee, defs)
            {
                if (ee->Name == s1.get<0>())
                {
                    def2 = ee;
                    break;
                }
            }

            if (!def2)
                throw ServiceDefinitionVerifyException(
                    "Service definition \"" + s1.get<0>() + "\" not found to implement in " + out.name, parse_info);

            RR_SHARED_PTR<ServiceEntryDefinition> obj2;
            BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& ee2, def2->Objects)
            {
                if (ee2->Name == s1.get<1>())
                {
                    obj2 = ee2;
                    break;
                }
            }

            if (!obj2)
                throw ServiceDefinitionVerifyException("Object \"" + e + "\" not found to implement", parse_info);

            if (rootobj == (def2->Name + "." + obj2->Name))
                throw ServiceDefinitionVerifyException("Recursive implements between \"" + rootobj + "\" and \"" +
                                                           def2->Name + "." + obj2->Name + "\"",
                                                       parse_info);

            rrimplements imp2 = get_implements(obj2, def2, defs, parse_info, rootobj);
            out.implements.push_back(imp2);
        }
    }

    BOOST_FOREACH (rrimplements& r, out.implements)
    {
        BOOST_FOREACH (rrimplements& r2, r.implements)
        {
            bool found = false;
            BOOST_FOREACH (rrimplements& r3, out.implements)
            {
                if (r2.name == r3.name)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                throw ServiceDefinitionVerifyException(
                    "Object \"" + out.name + "\" does not implement inherited type \"" + r2.name + "\"", parse_info);
            }
        }
    }

    return out;
}

bool CompareTypeDefinition(const RR_SHARED_PTR<ServiceDefinition>& d1, const RR_SHARED_PTR<TypeDefinition>& t1,
                           const RR_SHARED_PTR<ServiceDefinition>& d2, const RR_SHARED_PTR<TypeDefinition>& t2)
{
    if (t1->Name != t2->Name)
        return false;
    // if (t1->ImportedType!=t2->ImportedType) return false;
    if (t1->ArrayType != t2->ArrayType)
        return false;
    if (t1->ArrayType != DataTypes_ArrayTypes_none)
    {
        if (t1->ArrayVarLength != t2->ArrayVarLength)
            return false;
        if (t1->ArrayLength.size() != t2->ArrayLength.size())
            return false;
        if (!boost::range::equal(t1->ArrayLength, t2->ArrayLength))
            return false;
    }

    if (t1->ContainerType != t2->ContainerType)
        return false;

    if (t1->Type != t2->Type)
        return false;
    if (t1->Type != DataTypes_namedtype_t && t1->Type != DataTypes_object_t)
        return true;

    if (t1->TypeString == "varvalue" && t2->TypeString == "varvalue")
        return true;

    std::string st1;
    std::string st2;

    if (!boost::contains(t1->TypeString, "."))
    {
        st1 = d1->Name + "." + t1->TypeString;
    }
    else
    {
        st1 = t1->TypeString;
    }

    if (!boost::contains(t2->TypeString, "."))
    {
        st2 = d2->Name + "." + t2->TypeString;
    }
    else
    {
        st2 = t2->TypeString;
    }

    return st1 == st2;
}

bool CompareTypeDefinitions(const RR_SHARED_PTR<ServiceDefinition>& d1, std::vector<RR_SHARED_PTR<TypeDefinition> >& t1,
                            const RR_SHARED_PTR<ServiceDefinition>& d2, std::vector<RR_SHARED_PTR<TypeDefinition> >& t2)
{
    if (t1.size() != t2.size())
        return false;
    for (size_t i = 0; i < t1.size(); i++)
    {
        if (!CompareTypeDefinition(d1, t1[i], d2, t2[i]))
            return false;
    }

    return true;
}

bool CompareMember(const RR_SHARED_PTR<MemberDefinition>& m1, const RR_SHARED_PTR<MemberDefinition>& m2)
{
    if (m1->Name != m2->Name)
        return false;
    if (!boost::range::equal(m1->Modifiers, m2->Modifiers))
        return false;

    RR_SHARED_PTR<ServiceEntryDefinition> e1 = m1->ServiceEntry.lock();
    RR_SHARED_PTR<ServiceEntryDefinition> e2 = m2->ServiceEntry.lock();
    if ((!e1) || (!e2))
        return false;

    RR_SHARED_PTR<ServiceDefinition> d1 = e1->ServiceDefinition_.lock();
    RR_SHARED_PTR<ServiceDefinition> d2 = e2->ServiceDefinition_.lock();
    if ((!d1) || (!d2))
        return false;

    RR_SHARED_PTR<PropertyDefinition> p1 = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(m1);
    RR_SHARED_PTR<PropertyDefinition> p2 = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(m2);
    if (p1)
    {
        if (!p2)
            return false;
        return CompareTypeDefinition(d1, p1->Type, d2, p2->Type);
    }

    RR_SHARED_PTR<FunctionDefinition> f1 = RR_DYNAMIC_POINTER_CAST<FunctionDefinition>(m1);
    RR_SHARED_PTR<FunctionDefinition> f2 = RR_DYNAMIC_POINTER_CAST<FunctionDefinition>(m2);
    if (f1)
    {
        if (!f2)
            return false;
        if (!CompareTypeDefinition(d1, f1->ReturnType, d2, f2->ReturnType))
            return false;
        return CompareTypeDefinitions(d1, f1->Parameters, d2, f2->Parameters);
    }

    RR_SHARED_PTR<EventDefinition> ev1 = RR_DYNAMIC_POINTER_CAST<EventDefinition>(m1);
    RR_SHARED_PTR<EventDefinition> ev2 = RR_DYNAMIC_POINTER_CAST<EventDefinition>(m2);
    if (ev1)
    {
        if (!ev2)
            return false;

        return CompareTypeDefinitions(d1, ev1->Parameters, d2, ev2->Parameters);
    }

    RR_SHARED_PTR<ObjRefDefinition> o1 = RR_DYNAMIC_POINTER_CAST<ObjRefDefinition>(m1);
    RR_SHARED_PTR<ObjRefDefinition> o2 = RR_DYNAMIC_POINTER_CAST<ObjRefDefinition>(m2);
    if (o1)
    {
        if (!o2)
            return false;

        if (o1->ArrayType != o2->ArrayType)
            return false;
        if (o1->ContainerType != o2->ContainerType)
            return false;

        if (o1->ObjectType == "varobject" && o2->ObjectType == "varobject")
            return true;

        std::string st1;
        std::string st2;

        if (!boost::contains(o1->ObjectType, "."))
        {
            st1 = d1->Name + "." + o1->ObjectType;
        }
        else
        {
            st1 = o1->ObjectType;
        }

        if (!boost::contains(o2->ObjectType, "."))
        {
            st2 = d2->Name + "." + o2->ObjectType;
        }
        else
        {
            st2 = o2->ObjectType;
        }

        return st1 == st2;
    }

    RR_SHARED_PTR<PipeDefinition> pp1 = RR_DYNAMIC_POINTER_CAST<PipeDefinition>(m1);
    RR_SHARED_PTR<PipeDefinition> pp2 = RR_DYNAMIC_POINTER_CAST<PipeDefinition>(m2);
    if (pp1)
    {
        if (!pp2)
            return false;

        return CompareTypeDefinition(d1, pp1->Type, d2, pp2->Type);
    }

    RR_SHARED_PTR<CallbackDefinition> c1 = RR_DYNAMIC_POINTER_CAST<CallbackDefinition>(m1);
    RR_SHARED_PTR<CallbackDefinition> c2 = RR_DYNAMIC_POINTER_CAST<CallbackDefinition>(m2);
    if (c1)
    {
        if (!c2)
            return false;
        if (!CompareTypeDefinition(d1, c1->ReturnType, d2, c2->ReturnType))
            return false;
        return CompareTypeDefinitions(d1, c1->Parameters, d2, c2->Parameters);
    }

    RR_SHARED_PTR<WireDefinition> w1 = RR_DYNAMIC_POINTER_CAST<WireDefinition>(m1);
    RR_SHARED_PTR<WireDefinition> w2 = RR_DYNAMIC_POINTER_CAST<WireDefinition>(m2);
    if (w1)
    {
        if (!w2)
            return false;

        return CompareTypeDefinition(d1, w1->Type, d2, w2->Type);
    }

    RR_SHARED_PTR<MemoryDefinition> mem1 = RR_DYNAMIC_POINTER_CAST<MemoryDefinition>(m1);
    RR_SHARED_PTR<MemoryDefinition> mem2 = RR_DYNAMIC_POINTER_CAST<MemoryDefinition>(m2);
    if (mem1)
    {
        if (!mem2)
            return false;

        return CompareTypeDefinition(d1, mem1->Type, d2, mem2->Type);
    }

    return false;
}

void VerifyObject(const RR_SHARED_PTR<ServiceEntryDefinition>& obj, const RR_SHARED_PTR<ServiceDefinition>& def,
                  const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs,
                  std::vector<ServiceDefinitionParseException>& warnings)
{
    if (obj->EntryType != DataTypes_object_t)
        throw ServiceDefinitionVerifyException("Invalid EntryType \"" + obj->Name + "\"", obj->ParseInfo);

    VerifyName(obj->Name, def, obj->ParseInfo);

    std::vector<std::string> membernames;

    BOOST_FOREACH (std::string& e, obj->Options)
    {
        std::vector<std::string> s1;
        boost::split(s1, e, boost::is_space());
        if (s1.at(0) == "constant")
        {
            std::string membername = VerifyConstant(e, def, obj->ParseInfo);
            if (boost::range::find(membernames, membername) != membernames.end())
                throw ServiceDefinitionVerifyException("Object \"" + obj->Name +
                                                           "\" contains multiple members named \"" + membername + "\"",
                                                       obj->ParseInfo);
            membernames.push_back(membername);
        }
    }

    if (!obj->Constants.empty())
    {
        VerifyVersionSupport(def, 0, 9,
                             "Service definition standard version 0.9 or greater required for \"constant\" keyword");
    }

    BOOST_FOREACH (const RR_SHARED_PTR<ConstantDefinition>& e, obj->Constants)
    {
        std::string membername = VerifyConstant(e, def, obj->Constants);
        if (boost::range::find(membernames, membername) != membernames.end())
            throw ServiceDefinitionVerifyException(
                "Object \"" + obj->Name + "\" contains multiple members named \"" + membername + "\"", obj->ParseInfo);
        membernames.push_back(membername);
    }

    BOOST_FOREACH (const RR_SHARED_PTR<MemberDefinition>& e, obj->Members)
    {

        std::string membername = VerifyMember(e, def, defs, warnings);
        if (boost::range::find(membernames, membername) != membernames.end())
            throw ServiceDefinitionVerifyException(
                "Object \"" + obj->Name + "\" contains multiple members named \"" + membername + "\"", obj->ParseInfo);
        membernames.push_back(membername);
    }

    rrimplements r = get_implements(obj, def, defs, obj->ParseInfo);

    BOOST_FOREACH (rrimplements& e, r.implements)
    {
        BOOST_FOREACH (RR_SHARED_PTR<MemberDefinition>& ee, e.obj->Members)
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
            if (!m2)
                throw ServiceDefinitionVerifyException("Object \"" + obj->Name +
                                                           "\" does not implement required member \"" + ee->Name + "\"",
                                                       obj->ParseInfo);

            if (!CompareMember(m2, ee))
                throw ServiceDefinitionVerifyException("Member \"" + ee->Name + "\" in object \"" + obj->Name +
                                                           "\" does not match implemented member",
                                                       m2->ParseInfo);
        }
    }
}

void VerifyStructure_check_recursion(const RR_SHARED_PTR<ServiceEntryDefinition>& strut,
                                     const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs,
                                     std::set<std::string> names, DataTypes entry_type)
{
    if (strut->EntryType != entry_type && strut->EntryType != DataTypes_namedarray_t)
    {
        throw InternalErrorException("");
    }

    names.insert(strut->Name);

    BOOST_FOREACH (RR_SHARED_PTR<MemberDefinition>& e, strut->Members)
    {
        RR_SHARED_PTR<PropertyDefinition> p = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(e);
        if (!p)
            throw InternalErrorException("");

        if (p->Type->Type == DataTypes_namedtype_t)
        {
            RR_SHARED_PTR<NamedTypeDefinition> nt_def = VerifyResolveNamedType(p->Type, defs);
            RR_SHARED_PTR<ServiceEntryDefinition> et_def = RR_DYNAMIC_POINTER_CAST<ServiceEntryDefinition>(nt_def);
            if (!et_def)
                throw InternalErrorException("");
            if (et_def->EntryType != entry_type && et_def->EntryType != DataTypes_namedarray_t)
                throw InternalErrorException("");

            if (names.find(et_def->Name) != names.end())
            {
                throw ServiceDefinitionVerifyException("Recursive namedarray/pod detected in \"" + strut->Name + "\"",
                                                       strut->ParseInfo);
            }

            VerifyStructure_check_recursion(et_def, defs, names, entry_type);
        }
    }
}

void VerifyStructure_common(const RR_SHARED_PTR<ServiceEntryDefinition>& strut,
                            const RR_SHARED_PTR<ServiceDefinition>& def,
                            const std::vector<RR_SHARED_PTR<ServiceDefinition> >& imported_defs,
                            const std::vector<RR_SHARED_PTR<ServiceDefinition> >& all_defs,
                            std::vector<ServiceDefinitionParseException>& warnings, DataTypes entry_type)
{
    if (strut->EntryType != entry_type)
        throw ServiceDefinitionVerifyException("Invalid EntryType in \"" + strut->Name + "\"", strut->ParseInfo);

    VerifyName(strut->Name, def, strut->ParseInfo);
    std::vector<std::string> membernames;

    BOOST_FOREACH (std::string& e, strut->Options)
    {
        std::vector<std::string> s1;
        boost::split(s1, e, boost::is_space());
        if (s1.at(0) == "constant")
        {
            std::string membername = VerifyConstant(e, def, strut->ParseInfo);
            if (boost::range::find(membernames, membername) != membernames.end())
                throw ServiceDefinitionVerifyException("Structure \"" + strut->Name +
                                                           "\" contains multiple members named \"" + membername + "\"",
                                                       strut->ParseInfo);
            membernames.push_back(membername);
        }
    }

    BOOST_FOREACH (const RR_SHARED_PTR<ConstantDefinition>& e, strut->Constants)
    {
        std::string membername = VerifyConstant(e, def, strut->Constants);
        if (boost::range::find(membernames, membername) != membernames.end())
            throw ServiceDefinitionVerifyException("Struct \"" + strut->Name + "\" contains multiple members named \"" +
                                                       membername + "\"",
                                                   strut->ParseInfo);
        membernames.push_back(membername);
    }

    DataTypes namedarray_element_type = DataTypes_void_t;

    BOOST_FOREACH (RR_SHARED_PTR<MemberDefinition>& e, strut->Members)
    {
        RR_SHARED_PTR<PropertyDefinition> p = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(e);
        if (!p)
            throw ServiceDefinitionVerifyException("Structure \"" + strut->Name + "\" must only contain fields",
                                                   e->ParseInfo);

        std::string membername = VerifyMember(p, def, imported_defs, warnings);

        if (entry_type == DataTypes_pod_t)
        {
            RR_SHARED_PTR<TypeDefinition> t = p->Type;
            if (!IsTypeNumeric(t->Type) && t->Type != DataTypes_namedtype_t)
            {
                throw ServiceDefinitionVerifyException("Pods must only contain numeric, pod, and namedarray types",
                                                       e->ParseInfo);
            }

            if (t->Type == DataTypes_namedtype_t)
            {
                RR_SHARED_PTR<NamedTypeDefinition> tt = VerifyResolveNamedType(t, imported_defs);
                if (tt->RRDataType() != DataTypes_pod_t && tt->RRDataType() != DataTypes_namedarray_t)
                {
                    throw ServiceDefinitionVerifyException("Pods must only contain numeric, namedarray, pod types",
                                                           e->ParseInfo);
                }
            }

            if (t->ContainerType != DataTypes_ContainerTypes_none)
            {
                throw ServiceDefinitionVerifyException("Pods must not use containers", e->ParseInfo);
            }

            if ((boost::range::find(t->ArrayLength, 0) != t->ArrayLength.end()) ||
                (t->ArrayType == DataTypes_ArrayTypes_multidimarray && t->ArrayLength.empty()))
            {
                throw ServiceDefinitionVerifyException("Pods must have fixed or finite length arrays", e->ParseInfo);
            }
        }

        if (entry_type == DataTypes_namedarray_t)
        {
            RR_SHARED_PTR<TypeDefinition> t = p->Type;
            if (!IsTypeNumeric(t->Type) && t->Type != DataTypes_namedtype_t)
            {
                throw ServiceDefinitionVerifyException(
                    "NamedArrays must only contain numeric and namedarray types: " + e->Name, e->ParseInfo);
            }

            if (t->Type == DataTypes_namedtype_t)
            {
                RR_SHARED_PTR<NamedTypeDefinition> tt = VerifyResolveNamedType(t, imported_defs);
                if (tt->RRDataType() != DataTypes_namedarray_t)
                {
                    throw ServiceDefinitionVerifyException(
                        "NamedArrays must only contain numeric and namedarray types: " + e->Name, e->ParseInfo);
                }
            }

            if (t->ContainerType != DataTypes_ContainerTypes_none)
            {
                throw ServiceDefinitionVerifyException("NamedArrays may not use containers: " + e->Name, e->ParseInfo);
            }

            switch (t->ArrayType)
            {
            case DataTypes_ArrayTypes_none:
                break;
            case DataTypes_ArrayTypes_array:
                if (t->ArrayVarLength)
                    throw ServiceDefinitionVerifyException("NamedArray fields must be scalars or fixed arrays",
                                                           e->ParseInfo);
                break;
            default:
                throw ServiceDefinitionVerifyException("NamedArray fields must be scalars or fixed arrays",
                                                       e->ParseInfo);
            }

            std::set<std::string> n;
        }

        if (boost::range::find(membernames, membername) != membernames.end())
            throw ServiceDefinitionVerifyException("Structure \"" + strut->Name +
                                                       "\" contains multiple members named \"" + membername + "\"",
                                                   e->ParseInfo);
        membernames.push_back(membername);
    }

    if (entry_type == DataTypes_pod_t)
    {
        std::set<std::string> n;
        VerifyStructure_check_recursion(strut, all_defs, n, DataTypes_pod_t);
    }

    if (entry_type == DataTypes_namedarray_t)
    {
        std::set<std::string> n;
        // VerifyStructure_check_recursion(strut, defs, n, DataTypes_namedarray_t);
        try
        {
            GetNamedArrayElementTypeAndCount(strut, all_defs);
        }
        catch (RobotRaconteurException& e)
        {
            throw ServiceDefinitionVerifyException("Invalid namedarray \"" + strut->Name + "\" " + e.Message,
                                                   strut->ParseInfo);
        }
    }
}

void VerifyStructure(const RR_SHARED_PTR<ServiceEntryDefinition>& strut, const RR_SHARED_PTR<ServiceDefinition>& def,
                     const std::vector<RR_SHARED_PTR<ServiceDefinition> >& imported_defs,
                     const std::vector<RR_SHARED_PTR<ServiceDefinition> >& all_defs,
                     std::vector<ServiceDefinitionParseException>& warnings)
{
    VerifyStructure_common(strut, def, imported_defs, all_defs, warnings, DataTypes_structure_t);
}

void VerifyPod(const RR_SHARED_PTR<ServiceEntryDefinition>& strut, const RR_SHARED_PTR<ServiceDefinition>& def,
               const std::vector<RR_SHARED_PTR<ServiceDefinition> >& imported_defs,
               const std::vector<RR_SHARED_PTR<ServiceDefinition> >& all_defs,
               std::vector<ServiceDefinitionParseException>& warnings)
{
    VerifyStructure_common(strut, def, imported_defs, all_defs, warnings, DataTypes_pod_t);
}

void VerifyNamedArray(const RR_SHARED_PTR<ServiceEntryDefinition>& strut, const RR_SHARED_PTR<ServiceDefinition>& def,
                      const std::vector<RR_SHARED_PTR<ServiceDefinition> >& imported_defs,
                      const std::vector<RR_SHARED_PTR<ServiceDefinition> >& all_defs,
                      std::vector<ServiceDefinitionParseException>& warnings)
{
    VerifyStructure_common(strut, def, imported_defs, all_defs, warnings, DataTypes_namedarray_t);
}

struct rrimports
{
    RR_SHARED_PTR<ServiceDefinition> def;
    std::vector<rrimports> imported;
};

rrimports get_imports(const RR_SHARED_PTR<ServiceDefinition>& def,
                      const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs,
                      std::set<std::string> parent_defs = std::set<std::string>())
{
    rrimports out;
    out.def = def;
    if (def->Imports.empty())
        return out;

    parent_defs.insert(def->Name);

    BOOST_FOREACH (std::string& e, def->Imports)
    {
        RR_SHARED_PTR<ServiceDefinition> def2;

        BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& ee, defs)
        {
            if (ee->Name == e)
            {
                def2 = ee;
                break;
            }
        }

        if (!def2)
            throw ServiceDefinitionVerifyException("Service definition \"" + e + "\" not found", def->ParseInfo);

        std::set<std::string>::iterator e_parent_defs = parent_defs.find(def2->Name);
        if (e_parent_defs != parent_defs.end())
        {
            throw ServiceDefinitionVerifyException(
                "Recursive imports between \"" + def->Name + "\" and \"" + *e_parent_defs + "\"", def->ParseInfo);
        }

        rrimports imp2 = get_imports(def2, defs, parent_defs);
        out.imported.push_back(imp2);
    }

    return out;
}

void VerifyImports(const RR_SHARED_PTR<ServiceDefinition>& def,
                   const std::vector<RR_SHARED_PTR<ServiceDefinition> >& defs)
{
    rrimports c = get_imports(def, defs);
}

ROBOTRACONTEUR_CORE_API void VerifyServiceDefinitions(const std::vector<RR_SHARED_PTR<ServiceDefinition> >& def,
                                                      std::vector<ServiceDefinitionParseException>& warnings)
{
    BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& e, def)
    {
        e->CheckVersion();

        if (!boost::starts_with(e->Name, "RobotRaconteurTestService") && !(e->Name == "RobotRaconteurServiceIndex"))
            VerifyName(e->Name, e, e->ParseInfo, true);

        if (boost::ends_with(e->Name, "_signed"))
            throw ServiceDefinitionVerifyException("Service definition names ending with \"_signed\" are reserved",
                                                   e->ParseInfo);

        VerifyImports(e, def);

        std::vector<std::string> names;
        BOOST_FOREACH (std::string& ee, e->Options)
        {
            std::vector<std::string> s1;
            boost::split(s1, ee, boost::is_space());
            if (s1.at(0) == "constant")
            {
                std::string name = VerifyConstant(ee, e, e->ParseInfo);
                if (boost::range::find(names, name) != names.end())
                    throw ServiceDefinitionVerifyException("Service definition \"" + e->Name +
                                                               "\" contains multiple high level names \"" + name + "\"",
                                                           e->ParseInfo);
                names.push_back(name);
            }
        }

        if (!e->Constants.empty())
        {
            VerifyVersionSupport(
                e, 0, 9, "Service definition standard version 0.9 or greater required for \"constant\" keyword");
        }

        BOOST_FOREACH (const RR_SHARED_PTR<ConstantDefinition>& ee, e->Constants)
        {
            std::string name = VerifyConstant(ee, e, e->Constants);
            if (boost::range::find(names, name) != names.end())
                throw ServiceDefinitionVerifyException("Service definition \"" + e->Name +
                                                           "\" contains multiple high level names \"" + name + "\"",
                                                       ee->ParseInfo);
            names.push_back(name);
        }

        BOOST_FOREACH (const RR_SHARED_PTR<ExceptionDefinition>& ee, e->Exceptions)
        {
            VerifyName(ee->Name, e, e->ParseInfo);
            std::string name = ee->Name;
            if (boost::range::find(names, name) != names.end())
                throw ServiceDefinitionVerifyException("Service definition \"" + e->Name +
                                                           "\" contains multiple high level names \"" + name + "\"",
                                                       e->ParseInfo);
            names.push_back(name);
        }

        std::vector<RR_SHARED_PTR<ServiceDefinition> > importeddefs;
        BOOST_FOREACH (std::string& ee, e->Imports)
        {
            BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& ee2, def)
            {
                if (ee == ee2->Name)
                {
                    importeddefs.push_back(ee2);
                }
            }
        }

        if (e->StdVer)
        {
            BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& ee, importeddefs)
            {
                if (!ee->StdVer || ee->StdVer > e->StdVer)
                {
                    throw ServiceDefinitionVerifyException(
                        "Imported service definition " + ee->Name +
                            " has a higher Service Definition standard version than " + e->Name,
                        e->ParseInfo);
                }
            }
        }

        if (!e->Using.empty())
        {
            VerifyVersionSupport(e, 0, 9,
                                 "Service definition standard version 0.9 or greater required for \"using\" keyword");
        }

        BOOST_FOREACH (RR_SHARED_PTR<UsingDefinition>& ee, e->Using)
        {
            std::string name = ee->UnqualifiedName;
            if (boost::range::find(names, name) != names.end())
                throw ServiceDefinitionVerifyException("Service definition \"" + e->Name +
                                                           "\" contains multiple high level names \"" + name + "\"",
                                                       ee->ParseInfo);
            VerifyUsing(*ee, e, importeddefs);
            names.push_back(name);
        }

        if (!e->Enums.empty())
        {
            VerifyVersionSupport(e, 0, 9,
                                 "Service definition standard version 0.9 or greater required for \"enum\" keyword");
        }

        BOOST_FOREACH (RR_SHARED_PTR<EnumDefinition>& ee, e->Enums)
        {
            VerifyEnum(*ee, e);
            std::string name = ee->Name;
            if (boost::range::find(names, name) != names.end())
                throw ServiceDefinitionVerifyException("Service definition \"" + e->Name +
                                                           "\" contains multiple high level names \"" + name + "\"",
                                                       ee->ParseInfo);
            names.push_back(name);
        }

        BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& ee, e->Structures)
        {

            VerifyStructure(ee, e, importeddefs, def, warnings);

            std::string name = ee->Name;
            if (boost::range::find(names, name) != names.end())
                throw ServiceDefinitionVerifyException("Service definition \"" + e->Name +
                                                           "\" contains multiple high level names \"" + name + "\"",
                                                       ee->ParseInfo);
            names.push_back(name);
        }

        BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& ee, e->Pods)
        {

            VerifyPod(ee, e, importeddefs, def, warnings);

            std::string name = ee->Name;
            if (boost::range::find(names, name) != names.end())
                throw ServiceDefinitionVerifyException("Service definition \"" + e->Name +
                                                           "\" contains multiple high level names \"" + name + "\"",
                                                       ee->ParseInfo);
            names.push_back(name);
        }

        BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& ee, e->NamedArrays)
        {

            VerifyNamedArray(ee, e, importeddefs, def, warnings);

            std::string name = ee->Name;
            if (boost::range::find(names, name) != names.end())
                throw ServiceDefinitionVerifyException("Service definition \"" + e->Name +
                                                           "\" contains multiple high level names \"" + name + "\"",
                                                       ee->ParseInfo);
            names.push_back(name);
        }

        BOOST_FOREACH (RR_SHARED_PTR<ServiceEntryDefinition>& ee, e->Objects)
        {

            VerifyObject(ee, e, importeddefs, warnings);

            std::string name = ee->Name;
            if (boost::range::find(names, name) != names.end())
                throw ServiceDefinitionVerifyException("Service definition \"" + e->Name +
                                                           "\" contains multiple high level names \"" + name + "\"",
                                                       ee->ParseInfo);
            names.push_back(name);
        }
    }
}

ROBOTRACONTEUR_CORE_API void VerifyServiceDefinitions(const std::vector<RR_SHARED_PTR<ServiceDefinition> >& def)
{
    std::vector<ServiceDefinitionParseException> warnings;
    VerifyServiceDefinitions(def, warnings);
}

bool CompareConstantDefinition(const RR_SHARED_PTR<ServiceDefinition>& service1, RR_SHARED_PTR<ConstantDefinition>& d1,
                               const RR_SHARED_PTR<ServiceDefinition>& service2, RR_SHARED_PTR<ConstantDefinition>& d2)
{
    if (d1->Name != d2->Name)
        return false;
    if (!CompareTypeDefinition(service1, d1->Type, service2, d2->Type))
        return false;
    if (boost::trim_copy(d1->Value) != boost::trim_copy(d2->Value))
        return false;
    return true;
}

ROBOTRACONTEUR_CORE_API bool CompareServiceEntryDefinition(const RR_SHARED_PTR<ServiceDefinition>& service1,
                                                           RR_SHARED_PTR<ServiceEntryDefinition>& d1,
                                                           const RR_SHARED_PTR<ServiceDefinition>& service2,
                                                           RR_SHARED_PTR<ServiceEntryDefinition>& d2)
{
    if (d1->Name != d2->Name)
        return false;
    if (d1->EntryType != d2->EntryType)
        return false;
    if (!boost::range::equal(d1->Implements, d2->Implements))
        return false;
    if (!boost::range::equal(d1->Options, d2->Options))
        return false;
    if (d1->Constants.size() != d2->Constants.size())
        return false;
    for (size_t i = 0; i < d1->Constants.size(); i++)
    {
        if (!CompareConstantDefinition(service1, d1->Constants[i], service2, d2->Constants[i]))
            return false;
    }

    if (d1->Members.size() != d2->Members.size())
        return false;
    for (size_t i = 0; i < d1->Members.size(); i++)
    {
        if (!CompareMember(d1->Members[i], d2->Members[i]))
            return false;
    }
    return true;
}

bool CompareUsingDefinition(const RR_SHARED_PTR<UsingDefinition>& u1, const RR_SHARED_PTR<UsingDefinition>& u2)
{
    if (u1->QualifiedName != u2->QualifiedName)
        return false;
    if (u1->UnqualifiedName != u2->UnqualifiedName)
        return false;
    return true;
}

bool CompareEnumDefinition(const RR_SHARED_PTR<EnumDefinition>& enum1, const RR_SHARED_PTR<EnumDefinition>& enum2)
{
    if (enum1->Name != enum2->Name)
        return false;
    if (enum1->Values.size() != enum2->Values.size())
        return false;
    for (size_t i = 0; i < enum1->Values.size(); i++)
    {
        if (enum1->Values[i].Name != enum2->Values[i].Name)
            return false;
        if (enum1->Values[i].Value != enum2->Values[i].Value)
            return false;
        if (enum1->Values[i].ImplicitValue != enum2->Values[i].ImplicitValue)
            return false;
        if (enum1->Values[i].HexValue != enum2->Values[i].HexValue)
            return false;
    }
    return true;
}

ROBOTRACONTEUR_CORE_API bool CompareServiceDefinitions(const RR_SHARED_PTR<ServiceDefinition>& def1,
                                                       const RR_SHARED_PTR<ServiceDefinition>& def2)
{
    const RR_SHARED_PTR<ServiceDefinition>& service1 = def1;
    const RR_SHARED_PTR<ServiceDefinition>& service2 = def2;
    if (service1->Name != service2->Name)
        return false;
    if (!boost::range::equal(service1->Imports, service2->Imports))
        return false;
    if (!boost::range::equal(service1->Options, service2->Options))
        return false;

    if (service1->Using.size() != service2->Using.size())
        return false;
    for (size_t i = 0; i < service1->Using.size(); i++)
    {
        if (!CompareUsingDefinition(service1->Using[i], service2->Using[i]))
            return false;
    }

    if (service1->Constants.size() != service2->Constants.size())
        return false;
    for (size_t i = 0; i < service1->Constants.size(); i++)
    {
        if (!CompareConstantDefinition(service1, service1->Constants[i], service2, service2->Constants[i]))
            return false;
    }

    if (service1->Enums.size() != service2->Enums.size())
        return false;
    for (size_t i = 0; i < service1->Enums.size(); i++)
    {
        if (!CompareEnumDefinition(service1->Enums[i], service2->Enums[i]))
            return false;
    }

    if (service1->StdVer != service2->StdVer)
        return false;

    if (service1->Objects.size() != service2->Objects.size())
        return false;
    for (size_t i = 0; i < service1->Objects.size(); i++)
    {
        if (!CompareServiceEntryDefinition(service1, service1->Objects[i], service2, service2->Objects[i]))
            return false;
    }

    if (service1->Structures.size() != service2->Structures.size())
        return false;
    for (size_t i = 0; i < service1->Structures.size(); i++)
    {
        if (!CompareServiceEntryDefinition(service1, service1->Structures[i], service2, service2->Structures[i]))
            return false;
    }

    if (service1->Pods.size() != service2->Pods.size())
        return false;
    for (size_t i = 0; i < service1->Pods.size(); i++)
    {
        if (!CompareServiceEntryDefinition(service1, service1->Pods[i], service2, service2->Pods[i]))
            return false;
    }

    if (service1->NamedArrays.size() != service2->NamedArrays.size())
        return false;
    for (size_t i = 0; i < service1->NamedArrays.size(); i++)
    {
        if (!CompareServiceEntryDefinition(service1, service1->NamedArrays[i], service2, service2->NamedArrays[i]))
            return false;
    }

    if (service1->Exceptions.size() != service2->Exceptions.size())
        return false;
    for (size_t i = 0; i < service1->Exceptions.size(); i++)
    {
        if (service1->Exceptions[i]->Name != service2->Exceptions[i]->Name)
            return false;
    }

    return true;
}

ROBOTRACONTEUR_CORE_API boost::tuple<boost::string_ref, boost::string_ref> SplitQualifiedName(boost::string_ref name)
{
    size_t pos = name.find_last_of('.');

    if (pos == std::string::npos)
        throw InvalidArgumentException("Name is not qualified");

    return boost::make_tuple(name.substr(0, pos), name.substr(pos + 1, name.size() - pos - 1));
}

ROBOTRACONTEUR_CORE_API size_t EstimatePodPackedElementSize(
    const RR_SHARED_PTR<ServiceEntryDefinition>& def, const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
    const RR_SHARED_PTR<RobotRaconteurNode>& node, const RR_SHARED_PTR<RRObject>& client)
{
    size_t s = 16;
    s += ArrayBinaryWriter::GetStringByteCount8(def->Name);
    BOOST_FOREACH (const RR_SHARED_PTR<MemberDefinition>& m, def->Members)
    {
        RR_SHARED_PTR<PropertyDefinition> p = rr_cast<PropertyDefinition>(m);
        if (IsTypeNumeric(p->Type->Type))
        {
            s += 16;
            s += ArrayBinaryWriter::GetStringByteCount8(p->Name);
            size_t array_count = 0;
            if (p->Type->ArrayType == DataTypes_ArrayTypes_none)
            {
                array_count = 1;
            }
            else
            {
                array_count =
                    boost::numeric_cast<size_t>(boost::accumulate(p->Type->ArrayLength, 1, std::multiplies<int32_t>()));
            }
            s += RRArrayElementSize(p->Type->Type) * array_count;
        }
        else
        {
            RR_SHARED_PTR<ServiceEntryDefinition> nt =
                rr_cast<ServiceEntryDefinition>(p->Type->ResolveNamedType(other_defs, node, client));
            s += 16;
            s += ArrayBinaryWriter::GetStringByteCount8(p->Name);
            s += ArrayBinaryWriter::GetStringByteCount8(nt->ResolveQualifiedName());
            size_t array_count = 0;
            if (p->Type->ArrayType == DataTypes_ArrayTypes_none)
            {
                array_count = 1;
            }
            else
            {
                array_count =
                    boost::numeric_cast<size_t>(boost::accumulate(p->Type->ArrayLength, 1, std::multiplies<int32_t>()));
            }
            s += EstimatePodPackedElementSize(nt, other_defs, node, client) * array_count;
        }
    }
    return s;
}

boost::tuple<DataTypes, size_t> GetNamedArrayElementTypeAndCount(
    const RR_SHARED_PTR<ServiceEntryDefinition>& def, const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
    const RR_SHARED_PTR<RobotRaconteurNode>& node, const RR_SHARED_PTR<RRObject>& client, std::set<std::string> n)
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

    BOOST_FOREACH (RR_SHARED_PTR<MemberDefinition>& e, def->Members)
    {
        size_t field_element_count = 1;

        RR_SHARED_PTR<PropertyDefinition> p = RR_DYNAMIC_POINTER_CAST<PropertyDefinition>(e);
        if (!p)
            throw ServiceDefinitionException("Invalid member type in namedarray: " + def->Name);

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
            field_element_count =
                boost::numeric_cast<size_t>(boost::accumulate(p->Type->ArrayLength, 1, std::multiplies<int32_t>()));
        }

        if (IsTypeNumeric(p->Type->Type))
        {
            if (element_type == DataTypes_void_t)
            {
                element_type = p->Type->Type;
            }
            else
            {
                if (element_type != p->Type->Type)
                    throw ServiceDefinitionException("namedarray must contain same numeric type: " + def->Name);
            }

            element_count += field_element_count;
        }
        else if (p->Type->Type == DataTypes_namedtype_t)
        {
            RR_SHARED_PTR<NamedTypeDefinition> nt_def = p->Type->ResolveNamedType(other_defs, node, client);
            RR_SHARED_PTR<ServiceEntryDefinition> et_def = RR_DYNAMIC_POINTER_CAST<ServiceEntryDefinition>(nt_def);
            if (!et_def)
                throw InternalErrorException("");
            if (et_def->EntryType != DataTypes_namedarray_t)
                throw InternalErrorException("");

            if (n.find(et_def->Name) != n.end())
            {
                throw ServiceDefinitionException("Recursive namedarray detected in " + def->Name);
            }

            boost::tuple<DataTypes, size_t> v = GetNamedArrayElementTypeAndCount(et_def, other_defs, node, client, n);
            if (element_type == DataTypes_void_t)
            {
                element_type = v.get<0>();
            }
            else
            {
                if (element_type != v.get<0>())
                    throw ServiceDefinitionException("namedarray must contain same numeric type: " + def->Name);
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

ROBOTRACONTEUR_CORE_API boost::tuple<DataTypes, size_t> GetNamedArrayElementTypeAndCount(
    const RR_SHARED_PTR<ServiceEntryDefinition>& def, const std::vector<RR_SHARED_PTR<ServiceDefinition> >& other_defs,
    const RR_SHARED_PTR<RobotRaconteurNode>& node, const RR_SHARED_PTR<RRObject>& client)
{
    std::set<std::string> n;
    return GetNamedArrayElementTypeAndCount(def, other_defs, node, client, n);
}
} // namespace RobotRaconteur
