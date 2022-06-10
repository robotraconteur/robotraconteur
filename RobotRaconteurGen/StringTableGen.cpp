#include "StringTableGen.h"
#include <boost/range/algorithm.hpp>

namespace RobotRaconteurGen
{

static void copy_to(std::set<std::string>& out, const std::set<std::string>& in)
{
    boost::range::copy(in, std::inserter(out, out.begin()));
}

std::set<std::string> GenerateStringTable(const std::vector<RR_SHARED_PTR<ServiceDefinition> >& gen_defs,
                                          const std::vector<RR_SHARED_PTR<ServiceDefinition> >& all_defs)
{
    std::set<std::string> include_strs;
    std::set<std::string> gen_strs;
    BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& def, all_defs)
    {
        if (!TryFindByName(gen_defs, def->Name))
        {
            copy_to(include_strs, GetServiceDefStrings(def));
        }
    }

    BOOST_FOREACH (const RR_SHARED_PTR<ServiceDefinition>& def, gen_defs)
    {
        copy_to(gen_strs, GetServiceDefStrings(def));
    }

    BOOST_FOREACH (const std::string& s, include_strs)
    {
        gen_strs.erase(s);
    }

    return gen_strs;
}

std::set<std::string> GetServiceDefStrings(const RR_SHARED_PTR<ServiceDefinition>& def)
{
    std::set<std::string> str1;

    str1.insert(def->Name);

    copy_to(str1, GetServiceEntriesDefStrings(def->Structures, def->Name));
    copy_to(str1, GetServiceEntriesDefStrings(def->Pods, def->Name));
    copy_to(str1, GetServiceEntriesDefStrings(def->NamedArrays, def->Name));
    copy_to(str1, GetServiceEntriesDefStrings(def->Objects, def->Name));
    BOOST_FOREACH (const RR_SHARED_PTR<ExceptionDefinition>& exp1, def->Exceptions)
    {
        std::string q_name = def->Name + "." + exp1->Name;
        str1.insert(q_name);
    }
    return str1;
}

std::set<std::string> GetServiceEntryDefStrings(const RR_SHARED_PTR<ServiceEntryDefinition>& def,
                                                const std::string& def_name)
{
    std::set<std::string> str1;
    std::string q_name = def_name + "." + def->Name;
    str1.insert(q_name);
    str1.insert(def->Name);
    copy_to(str1, GetMembersStrings(def->Members));
    return str1;
}

std::set<std::string> GetServiceEntriesDefStrings(std::vector<RR_SHARED_PTR<ServiceEntryDefinition> > def,
                                                  const std::string& def_name)
{
    std::set<std::string> str1;
    BOOST_FOREACH (const RR_SHARED_PTR<ServiceEntryDefinition>& s, def)
    {
        copy_to(str1, GetServiceEntryDefStrings(s, def_name));
    }
    return str1;
}

std::set<std::string> GetMemberStrings(const RR_SHARED_PTR<MemberDefinition>& m)
{
    std::set<std::string> str1;
    str1.insert(m->Name);

    RR_SHARED_PTR<FunctionDefinition> fdef = RR_DYNAMIC_POINTER_CAST<FunctionDefinition>(m);
    if (fdef)
    {
        BOOST_FOREACH (const RR_SHARED_PTR<TypeDefinition>& t, fdef->Parameters)
        {
            str1.insert(t->Name);
        }
    }

    RR_SHARED_PTR<EventDefinition> edef = RR_DYNAMIC_POINTER_CAST<EventDefinition>(m);
    if (edef)
    {
        BOOST_FOREACH (const RR_SHARED_PTR<TypeDefinition>& t, edef->Parameters)
        {
            str1.insert(t->Name);
        }
    }

    RR_SHARED_PTR<CallbackDefinition> cdef = RR_DYNAMIC_POINTER_CAST<CallbackDefinition>(m);
    if (cdef)
    {
        BOOST_FOREACH (const RR_SHARED_PTR<TypeDefinition>& t, cdef->Parameters)
        {
            str1.insert(t->Name);
        }
    }

    return str1;
}

std::set<std::string> GetMembersStrings(std::vector<RR_SHARED_PTR<MemberDefinition> > m)
{
    std::set<std::string> str1;
    BOOST_FOREACH (const RR_SHARED_PTR<MemberDefinition>& m1, m)
    {
        copy_to(str1, GetMemberStrings(m1));
    }

    return str1;
}

} // namespace RobotRaconteurGen