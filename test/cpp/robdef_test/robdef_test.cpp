
#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>

using namespace RobotRaconteur;
using namespace std;

static std::string ReadFile(const std::string& fname)
{
    ifstream file(fname.c_str(), ios_base::in);

    if (!file.is_open())
        throw std::runtime_error("File not found");

    int8_t bom1, bom2, bom3;
    file >> bom1 >> bom2 >> bom3;
    if (!(bom1 == -17 && bom2 == -69 && bom3 == -65))
    {
        file.seekg(0, ifstream::beg);
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

TEST(RobDef, RobDefTest)
{
    RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Debug);

    std::vector<std::string> robdef_filenames;
    robdef_filenames.push_back("com.robotraconteur.testing.TestService1.robdef");
    robdef_filenames.push_back("com.robotraconteur.testing.TestService2.robdef");
    robdef_filenames.push_back("com.robotraconteur.testing.TestService3.robdef");

    std::vector<RR_SHARED_PTR<ServiceDefinition> > defs;
    std::vector<RR_SHARED_PTR<ServiceDefinition> > defs2;
    BOOST_FOREACH (const std::string& fname, robdef_filenames)
    {
        std::string robdef_text;
        ASSERT_NO_THROW(robdef_text = ReadFile(ROBOTRACONTEUR_TEST_ROBDEF_DIR "/" + fname));
        RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
        ASSERT_NO_THROW(def->FromString(robdef_text));
        defs.push_back(def);
        std::string robdef_text2 = def->ToString();
        RR_SHARED_PTR<ServiceDefinition> def2 = RR_MAKE_SHARED<ServiceDefinition>();
        ASSERT_NO_THROW(def2->FromString(robdef_text2));
        defs2.push_back(def2);
    }

    ASSERT_NO_THROW(VerifyServiceDefinitions(defs));

    for (size_t i = 0; i < defs.size(); i++)
    {
        EXPECT_TRUE(CompareServiceDefinitions(defs[i], defs2[i]));
    }

    BOOST_FOREACH (RR_SHARED_PTR<ServiceDefinition> def, defs)
    {
        BOOST_FOREACH (RR_SHARED_PTR<ConstantDefinition> c, def->Constants)
        {
            if (c->Name == "strconst")
            {
                std::string strconst = c->ValueToString();
                std::cout << "strconst " << strconst << std::endl;

                std::string strconst2 = ConstantDefinition::EscapeString(strconst);
                std::string strconst3 = ConstantDefinition::UnescapeString(strconst2);

                EXPECT_EQ(strconst3, strconst);
            }

            if (c->Name == "int32const")
            {
                std::cout << "int32const: " << c->ValueToScalar<int32_t>() << std::endl;
            }

            if (c->Name == "int32const_array")
            {
                RR_INTRUSIVE_PTR<RRArray<int32_t> > a = c->ValueToArray<int32_t>();
                std::cout << "int32const_array: " << a->size() << std::endl;
            }

            if (c->Name == "doubleconst_array")
            {
                RR_INTRUSIVE_PTR<RRArray<double> > a = c->ValueToArray<double>();
                std::cout << "doubleconst_array: " << a->size() << std::endl;
            }

            if (c->Name == "structconst")
            {
                std::vector<ConstantDefinition_StructField> s = c->ValueToStructFields();
                BOOST_FOREACH (const ConstantDefinition_StructField& f, s)
                {
                    std::cout << f.Name << ": " << f.ConstantRefName << " ";
                }

                std::cout << std::endl;
            }
        }
    }

    RR_SHARED_PTR<ServiceDefinition> def1 = TryFindByName(defs, "com.robotraconteur.testing.TestService1");
    if (def1)
    {
        RR_SHARED_PTR<ServiceEntryDefinition> entry = TryFindByName(def1->Objects, "testroot");
        ASSERT_TRUE(entry);

        RR_SHARED_PTR<PropertyDefinition> p1 = rr_cast<PropertyDefinition>(TryFindByName(entry->Members, "d1"));
        EXPECT_EQ(p1->Direction(), MemberDefinition_Direction_both);

        RR_SHARED_PTR<PipeDefinition> p2 = rr_cast<PipeDefinition>(TryFindByName(entry->Members, "p1"));
        EXPECT_EQ(p2->Direction(), MemberDefinition_Direction_both);
        EXPECT_FALSE(p2->IsUnreliable());

        RR_SHARED_PTR<WireDefinition> w1 = rr_cast<WireDefinition>(TryFindByName(entry->Members, "w1"));
        EXPECT_EQ(w1->Direction(), MemberDefinition_Direction_both);

        RR_SHARED_PTR<MemoryDefinition> m1 = rr_cast<MemoryDefinition>(TryFindByName(entry->Members, "m1"));
        EXPECT_EQ(m1->Direction(), MemberDefinition_Direction_both);
    }

    RR_SHARED_PTR<ServiceDefinition> def2 = TryFindByName(defs, "com.robotraconteur.testing.TestService3");
    if (def2)
    {
        RR_SHARED_PTR<ServiceEntryDefinition> entry = TryFindByName(def2->Objects, "testroot3");
        ASSERT_TRUE(entry);

        RR_SHARED_PTR<PropertyDefinition> p1 = rr_cast<PropertyDefinition>(TryFindByName(entry->Members, "readme"));
        EXPECT_EQ(p1->Direction(), MemberDefinition_Direction_readonly);

        RR_SHARED_PTR<PropertyDefinition> p2 = rr_cast<PropertyDefinition>(TryFindByName(entry->Members, "writeme"));
        EXPECT_EQ(p2->Direction(), MemberDefinition_Direction_writeonly);

        RR_SHARED_PTR<PipeDefinition> p3 = rr_cast<PipeDefinition>(TryFindByName(entry->Members, "unreliable1"));
        EXPECT_EQ(p3->Direction(), MemberDefinition_Direction_readonly);
        EXPECT_TRUE(p3->IsUnreliable());

        RR_SHARED_PTR<WireDefinition> w1 = rr_cast<WireDefinition>(TryFindByName(entry->Members, "peekwire"));
        EXPECT_EQ(w1->Direction(), MemberDefinition_Direction_readonly);

        RR_SHARED_PTR<WireDefinition> w2 = rr_cast<WireDefinition>(TryFindByName(entry->Members, "pokewire"));
        EXPECT_EQ(w2->Direction(), MemberDefinition_Direction_writeonly);

        RR_SHARED_PTR<MemoryDefinition> m1 = rr_cast<MemoryDefinition>(TryFindByName(entry->Members, "readmem"));
        EXPECT_EQ(m1->Direction(), MemberDefinition_Direction_readonly);
    }
}

TEST(RobDef, IntegerArrayConstantValue)
{
    TypeDefinition t;
    t.FromString("int32[]");
    EXPECT_TRUE(ConstantDefinition::VerifyTypeAndValue(t, "{1, -2, 0x3}"));
    EXPECT_TRUE(ConstantDefinition::VerifyTypeAndValue(t, "{}"));
    EXPECT_FALSE(ConstantDefinition::VerifyTypeAndValue(t, "{:1, 2}"));
    EXPECT_FALSE(ConstantDefinition::VerifyTypeAndValue(t, "{1, :2}"));
}

TEST(RobDef, FromStringRecordsFirstLine)
{
    RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
    def->FromString("# comment\nservice example.firstline\n\nstdver 0.10\n");
    EXPECT_EQ(def->ParseInfo.Line, "service example.firstline");
}

TEST(RobDef, StdVerOnlyOnce)
{
    RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
    EXPECT_NO_THROW(def->FromString("service example.stdver\n\nstdver 0.10\n"));
    RR_SHARED_PTR<ServiceDefinition> def2 = RR_MAKE_SHARED<ServiceDefinition>();
    EXPECT_THROW(def2->FromString("service example.stdver\n\nstdver 0.10\nstdver 0.9\n"),
                 ServiceDefinitionParseException);
}

TEST(RobDef, NumberOverflowThrowsParseException)
{
    RobotRaconteurVersion v;
    EXPECT_THROW(v.FromString("99999999999.0"), ServiceDefinitionParseException);
    RR_SHARED_PTR<TypeDefinition> t = RR_MAKE_SHARED<TypeDefinition>();
    EXPECT_THROW(t->FromString("double[99999999999] a"), ServiceDefinitionParseException);
    EXPECT_THROW(t->FromString("double[3000000000] a"), ServiceDefinitionParseException);
    EXPECT_THROW(t->FromString("double[99999999999-] a"), ServiceDefinitionParseException);
    EXPECT_THROW(t->FromString("double[2,99999999999] a"), ServiceDefinitionParseException);
    EXPECT_NO_THROW(t->FromString("double[2,3] a"));
}

TEST(RobDef, Int8Constants)
{
    std::string def_text = "service example.int8const\n\nstdver 0.10\n\n"
                           "constant uint8 u1 200\n"
                           "constant uint8 u2 0xC8\n"
                           "constant uint8 u3 5\n"
                           "constant int8 i1 -1\n"
                           "constant int8 i2 -128\n"
                           "constant uint8[] ua {10, 20, 255}\n"
                           "constant int8[] ia {-10, 127}\n";
    RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
    ASSERT_NO_THROW(def->FromString(def_text));
    std::vector<RR_SHARED_PTR<ServiceDefinition> > defs;
    defs.push_back(def);
    EXPECT_NO_THROW(VerifyServiceDefinitions(defs));

    RR_SHARED_PTR<ConstantDefinition> c;
    c = TryFindByName(def->Constants, "u1");
    ASSERT_TRUE(c);
    EXPECT_EQ(c->ValueToScalar<uint8_t>(), 200);
    c = TryFindByName(def->Constants, "u2");
    ASSERT_TRUE(c);
    EXPECT_EQ(c->ValueToScalar<uint8_t>(), 200);
    c = TryFindByName(def->Constants, "u3");
    ASSERT_TRUE(c);
    EXPECT_EQ(c->ValueToScalar<uint8_t>(), 5);
    c = TryFindByName(def->Constants, "i1");
    ASSERT_TRUE(c);
    EXPECT_EQ(c->ValueToScalar<int8_t>(), -1);
    c = TryFindByName(def->Constants, "i2");
    ASSERT_TRUE(c);
    EXPECT_EQ(c->ValueToScalar<int8_t>(), -128);
    c = TryFindByName(def->Constants, "ua");
    ASSERT_TRUE(c);
    RR_INTRUSIVE_PTR<RRArray<uint8_t> > ua = c->ValueToArray<uint8_t>();
    ASSERT_EQ(ua->size(), 3u);
    EXPECT_EQ((*ua)[0], 10);
    EXPECT_EQ((*ua)[1], 20);
    EXPECT_EQ((*ua)[2], 255);
    c = TryFindByName(def->Constants, "ia");
    ASSERT_TRUE(c);
    RR_INTRUSIVE_PTR<RRArray<int8_t> > ia = c->ValueToArray<int8_t>();
    ASSERT_EQ(ia->size(), 2u);
    EXPECT_EQ((*ia)[0], -10);
    EXPECT_EQ((*ia)[1], 127);

    RR_SHARED_PTR<ServiceDefinition> def2 = RR_MAKE_SHARED<ServiceDefinition>();
    EXPECT_THROW(def2->FromString("service example.int8const2\n\nstdver 0.10\n\nconstant uint8 u1 256\n"),
                 ServiceDefinitionParseException);
    RR_SHARED_PTR<ServiceDefinition> def3 = RR_MAKE_SHARED<ServiceDefinition>();
    EXPECT_THROW(def3->FromString("service example.int8const3\n\nstdver 0.10\n\nconstant int8 i1 128\n"),
                 ServiceDefinitionParseException);
}

TEST(RobDef, ObjRefArrayWithContainerRejected)
{
    RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
    RR_SHARED_PTR<ServiceEntryDefinition> entry = RR_MAKE_SHARED<ServiceEntryDefinition>(def);
    RR_SHARED_PTR<ObjRefDefinition> o = RR_MAKE_SHARED<ObjRefDefinition>(entry);
    EXPECT_NO_THROW(o->FromString("objref varobject[] x"));
    RR_SHARED_PTR<ObjRefDefinition> o2 = RR_MAKE_SHARED<ObjRefDefinition>(entry);
    EXPECT_THROW(o2->FromString("objref varobject[]{string} x"), ServiceDefinitionParseException);
}

TEST(RobDef, PipeUnreliableOptionPerPipe)
{
    std::string def_text = "service example.unreliableoption\n\n"
                           "object obj\n"
                           "option pipe p1 unreliable\n"
                           "option pipe p2 unreliable\n"
                           "pipe double p1\n"
                           "pipe double p2\n"
                           "pipe double p3\n"
                           "end\n";
    RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
    ASSERT_NO_THROW(def->FromString(def_text));
    RR_SHARED_PTR<ServiceEntryDefinition> obj = TryFindByName(def->Objects, "obj");
    ASSERT_TRUE(obj);
    RR_SHARED_PTR<PipeDefinition> p1 = rr_cast<PipeDefinition>(TryFindByName(obj->Members, "p1"));
    RR_SHARED_PTR<PipeDefinition> p2 = rr_cast<PipeDefinition>(TryFindByName(obj->Members, "p2"));
    RR_SHARED_PTR<PipeDefinition> p3 = rr_cast<PipeDefinition>(TryFindByName(obj->Members, "p3"));
    EXPECT_TRUE(p1->IsUnreliable());
    EXPECT_TRUE(p2->IsUnreliable());
    EXPECT_FALSE(p3->IsUnreliable());
}

TEST(RobDef, PipeUnreliableModifierAndOptionWarning)
{
    std::string def_text = "service example.unreliablemodifier\n\n"
                           "object obj\n"
                           "option pipe q2 unreliable\n"
                           "pipe double q1 [unreliable]\n"
                           "pipe double q2 [unreliable]\n"
                           "end\n";
    RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
    ASSERT_NO_THROW(def->FromString(def_text));
    std::vector<RR_SHARED_PTR<ServiceDefinition> > defs;
    defs.push_back(def);
    std::vector<ServiceDefinitionParseException> warnings;
    ASSERT_NO_THROW(VerifyServiceDefinitions(defs, warnings));
    size_t combination_warnings = 0;
    for (size_t i = 0; i < warnings.size(); i++)
    {
        if (warnings[i].ShortMessage == "Invalid member modifier combination: [unreliable]")
            combination_warnings++;
    }
    // Only q2 has both the modifier and the legacy option
    EXPECT_EQ(combination_warnings, 1u);
}

TEST(RobDef, ExceptionMessagesIncludeLocation)
{
    ServiceDefinitionParseInfo info;
    info.ServiceName = "example.messages";
    info.LineNumber = 12;
    info.Line = "object foo";

    ServiceDefinitionParseException e1("parse problem", info);
    EXPECT_EQ(std::string(e1.what()), "Parse error on line 12 in example.messages: parse problem");
    EXPECT_EQ(e1.ToString(), "Parse error on line 12 in example.messages: parse problem");

    ServiceDefinitionVerifyException e2("verify problem", info);
    EXPECT_EQ(std::string(e2.what()), "Verify error on line 12 in example.messages: verify problem");
    EXPECT_EQ(e2.ToString(), "Verify error on line 12 in example.messages: verify problem");
}

TEST(RobDef, ExceptionDefinitionKeepsService)
{
    RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
    ASSERT_NO_THROW(def->FromString("service example.exceptionservice\n\nstdver 0.10\n\nexception MyError\n"));
    ASSERT_EQ(def->Exceptions.size(), 1u);
    EXPECT_EQ(def->Exceptions[0]->Name, "MyError");
    EXPECT_EQ(def->Exceptions[0]->service.lock(), def);
}

TEST(RobDef, UsingAsMultipleBlanks)
{
    RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
    RR_SHARED_PTR<UsingDefinition> u1 = RR_MAKE_SHARED<UsingDefinition>(def);
    ASSERT_NO_THROW(u1->FromString("using c.d  as e"));
    EXPECT_EQ(u1->QualifiedName, "c.d");
    EXPECT_EQ(u1->UnqualifiedName, "e");
    RR_SHARED_PTR<UsingDefinition> u2 = RR_MAKE_SHARED<UsingDefinition>(def);
    ASSERT_NO_THROW(u2->FromString("using c.d as  e"));
    EXPECT_EQ(u2->QualifiedName, "c.d");
    EXPECT_EQ(u2->UnqualifiedName, "e");
    RR_SHARED_PTR<UsingDefinition> u3 = RR_MAKE_SHARED<UsingDefinition>(def);
    ASSERT_NO_THROW(u3->FromString("using c.d as\te"));
    EXPECT_EQ(u3->UnqualifiedName, "e");
}

TEST(RobDef, RecursiveImplementsNotThroughRoot)
{
    std::string def_text = "service example.implementscycle\n\nstdver 0.10\n\n"
                           "object A\n"
                           "implements B\n"
                           "end\n\n"
                           "object B\n"
                           "implements C\n"
                           "end\n\n"
                           "object C\n"
                           "implements B\n"
                           "end\n";
    RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
    ASSERT_NO_THROW(def->FromString(def_text));
    std::vector<RR_SHARED_PTR<ServiceDefinition> > defs;
    defs.push_back(def);
    EXPECT_THROW(VerifyServiceDefinitions(defs), ServiceDefinitionVerifyException);
}

TEST(RobDef, VerifyImportsChainNotExponential)
{
    // Each definition imports all earlier definitions. Without memoization the import tree has 2^N nodes.
    std::vector<RR_SHARED_PTR<ServiceDefinition> > defs;
    for (int i = 0; i < 40; i++)
    {
        std::stringstream ss;
        ss << "service example.importchain" << i << "\n\nstdver 0.10\n\n";
        for (int j = 0; j < i; j++)
        {
            ss << "import example.importchain" << j << "\n";
        }
        RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
        ASSERT_NO_THROW(def->FromString(ss.str()));
        defs.push_back(def);
    }
    EXPECT_NO_THROW(VerifyServiceDefinitions(defs));
}

TEST(RobDef, VerifyImportsCycleStillDetected)
{
    std::vector<RR_SHARED_PTR<ServiceDefinition> > defs;
    RR_SHARED_PTR<ServiceDefinition> d1 = RR_MAKE_SHARED<ServiceDefinition>();
    ASSERT_NO_THROW(d1->FromString("service example.importcycle1\n\nstdver 0.10\nimport example.importcycle2\n"));
    RR_SHARED_PTR<ServiceDefinition> d2 = RR_MAKE_SHARED<ServiceDefinition>();
    ASSERT_NO_THROW(d2->FromString("service example.importcycle2\n\nstdver 0.10\nimport example.importcycle3\n"));
    RR_SHARED_PTR<ServiceDefinition> d3 = RR_MAKE_SHARED<ServiceDefinition>();
    ASSERT_NO_THROW(d3->FromString("service example.importcycle3\n\nstdver 0.10\nimport example.importcycle2\n"));
    defs.push_back(d1);
    defs.push_back(d2);
    defs.push_back(d3);
    EXPECT_THROW(VerifyServiceDefinitions(defs), ServiceDefinitionVerifyException);
}

TEST(RobDef, EstimatePodPackedElementSizeRecursivePod)
{
    // Definitions pulled from a service are not verified, so EstimatePodPackedElementSize must not
    // recurse forever on a recursive pod
    std::string def_text = "service example.recursivepod\n\nstdver 0.10\n\n"
                           "pod A\nfield A a\nend\n\n"
                           "pod B\nfield C c\nend\n\n"
                           "pod C\nfield B[2] b\nend\n\n"
                           "pod D\nfield double[2,3] x\nfield int32 y\nend\n\n"
                           "pod E\nfield D d1\nfield D[3] d2\nend\n";
    RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
    ASSERT_NO_THROW(def->FromString(def_text));
    std::vector<RR_SHARED_PTR<ServiceDefinition> > defs;
    defs.push_back(def);

    RR_SHARED_PTR<ServiceEntryDefinition> a = TryFindByName(def->Pods, "A");
    RR_SHARED_PTR<ServiceEntryDefinition> b = TryFindByName(def->Pods, "B");
    RR_SHARED_PTR<ServiceEntryDefinition> d = TryFindByName(def->Pods, "D");
    RR_SHARED_PTR<ServiceEntryDefinition> e = TryFindByName(def->Pods, "E");
    ASSERT_TRUE(a);
    ASSERT_TRUE(b);
    ASSERT_TRUE(d);
    ASSERT_TRUE(e);
    EXPECT_ANY_THROW(EstimatePodPackedElementSize(a, defs));
    EXPECT_ANY_THROW(EstimatePodPackedElementSize(b, defs));
    size_t d_size = 0;
    EXPECT_NO_THROW(d_size = EstimatePodPackedElementSize(d, defs));
    EXPECT_GT(d_size, 6 * sizeof(double) + sizeof(int32_t));
    size_t e_size = 0;
    EXPECT_NO_THROW(e_size = EstimatePodPackedElementSize(e, defs));
    EXPECT_GT(e_size, 4 * d_size);
}

static EnumDefinitionValue MakeEnumDefinitionValue(const std::string& name, int32_t value)
{
    EnumDefinitionValue v;
    v.Name = name;
    v.Value = value;
    return v;
}

// VerifyValues must handle empty and single-value enums, and still find duplicate names and values. It used
// Values.end()-- (a no-op on a temporary that decremented the end iterator of an empty vector)
TEST(RobDef, EnumVerifyValues)
{
    RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
    EnumDefinition e(def);
    EXPECT_TRUE(e.VerifyValues());

    e.Values.push_back(MakeEnumDefinitionValue("a", 1));
    EXPECT_TRUE(e.VerifyValues());

    e.Values.push_back(MakeEnumDefinitionValue("b", 2));
    e.Values.push_back(MakeEnumDefinitionValue("c", 3));
    EXPECT_TRUE(e.VerifyValues());

    e.Values.push_back(MakeEnumDefinitionValue("d", 3));
    EXPECT_FALSE(e.VerifyValues());

    e.Values.back() = MakeEnumDefinitionValue("a", 4);
    EXPECT_FALSE(e.VerifyValues());
}

TEST(RobDef, OptionBeforeStdver)
{
    // stdver must come right after the service line, also when an option precedes it
    {
        RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
        EXPECT_THROW(def->FromString("service a.b\noption foo\nstdver 0.9.2\n"), ServiceDefinitionParseException);
    }
    {
        RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
        EXPECT_THROW(def->FromString("service a.b\noption foo\nstdver 0.9\n"), ServiceDefinitionParseException);
    }

    // Options without stdver, or after a stdver < 0.9.2, are still accepted (deprecated)
    {
        RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
        EXPECT_NO_THROW(def->FromString("service a.b\noption foo\n"));
        EXPECT_EQ(def->Options.size(), 1u);
    }
    {
        RR_SHARED_PTR<ServiceDefinition> def = RR_MAKE_SHARED<ServiceDefinition>();
        EXPECT_NO_THROW(def->FromString("service a.b\nstdver 0.9\noption foo\n"));
        EXPECT_EQ(def->Options.size(), 1u);
        RR_SHARED_PTR<ServiceDefinition> def2 = RR_MAKE_SHARED<ServiceDefinition>();
        EXPECT_NO_THROW(def2->FromString(def->ToString()));
        EXPECT_EQ(def2->Options.size(), 1u);
    }
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
