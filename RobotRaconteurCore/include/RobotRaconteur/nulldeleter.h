#pragma once

namespace RobotRaconteur
{
	struct null_deleter
	{
		void operator()(void const *) const
		{
		}
	};
}