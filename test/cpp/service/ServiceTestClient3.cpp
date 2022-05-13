#include "ServiceTestClient3.h"

#include "array_compare.h"

namespace RobotRaconteurTest
{

	void ServiceTestClient3::RunFullTest(const std::string& url)
	{
		Connect(url);
		
		TestProperties();

        TestFunctions();

		Disconnect();
	}

	void ServiceTestClient3::Connect(const std::string& url)
	{
		r = rr_cast<asynctestroot>(RobotRaconteurNode::s()->ConnectService(url));
	}

	void ServiceTestClient3::Disconnect()
	{
		RobotRaconteurNode::s()->DisconnectService(r);
	}

    void ServiceTestClient3::TestProperties()
    {
        EXPECT_NO_THROW(EXPECT_DOUBLE_EQ(r->get_d1(), 8.5515));
        EXPECT_NO_THROW(r->set_d1(3.0819));

        EXPECT_THROW(r->get_err(),InvalidArgumentException);
        EXPECT_THROW(r->set_err(0.0),InvalidOperationException);      
	}

    void ServiceTestClient3::TestFunctions()
    {
        EXPECT_NO_THROW(r->f1());
        EXPECT_NO_THROW(r->f2(247));
        EXPECT_NO_THROW(EXPECT_EQ(r->f3(10,20), 30));

        EXPECT_THROW(r->err_func(),InvalidOperationException);
        EXPECT_THROW(r->err_func2(),com::robotraconteur::testing::TestService5::asynctestexp);
    }
}