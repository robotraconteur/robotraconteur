#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"
#include "ServiceTest3.h"

#pragma once

using namespace std;
using namespace RobotRaconteur;
using namespace com::robotraconteur::testing::TestService5;

#include "CompareArray.h"

namespace RobotRaconteurTest
{
	class ServiceTestClient3
	{
	public:

		void RunFullTest(const std::string& url);

		void Connect(const std::string& url);
		void Disconnect();

        void TestProperties();

        void TestFunctions();

        RR_SHARED_PTR<asynctestroot> r;
    };
}