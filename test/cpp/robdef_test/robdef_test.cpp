
#include <boost/shared_array.hpp>

#include <gtest/gtest.h>
#include <RobotRaconteur/ServiceDefinition.h>
#include <RobotRaconteur/RobotRaconteurNode.h>

using namespace RobotRaconteur;
using namespace std;

static std::string ReadFile(const std::string& fname)
{
	ifstream file(fname.c_str(), ios_base::in);

	if (!file.is_open()) throw std::runtime_error("File not found");

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

TEST(RobDef,RobDefTest)
{
    RobotRaconteurNode::s()->SetLogLevel(RobotRaconteur_LogLevel_Debug);

    std::vector<std::string> robdef_filenames;
    robdef_filenames.push_back("com.robotraconteur.testing.TestService1.robdef");
    robdef_filenames.push_back("com.robotraconteur.testing.TestService2.robdef");
    robdef_filenames.push_back("com.robotraconteur.testing.TestService3.robdef");
    
    std::vector<RR_SHARED_PTR<ServiceDefinition> > defs;
    std::vector<RR_SHARED_PTR<ServiceDefinition> > defs2;
    BOOST_FOREACH(const std::string& fname, robdef_filenames)
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
    
    BOOST_FOREACH(RR_SHARED_PTR<ServiceDefinition> def, defs)
    {
        BOOST_FOREACH(RR_SHARED_PTR<ConstantDefinition> c, def->Constants)
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
                BOOST_FOREACH(const ConstantDefinition_StructField& f, s)
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

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}