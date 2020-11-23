//Timespec


namespace RobotRaconteur
{
class TimeSpec
{

%pythoncode
%{
	def __str__(self):
		return str(self.seconds + (self.nanoseconds * 1.0e-9))
%}

public:
	int64_t seconds;
	int32_t nanoseconds;

	TimeSpec();

	TimeSpec(int64_t seconds, int32_t nanoseconds);

public:
	bool operator == (const TimeSpec &t2);

	bool operator != (const TimeSpec &t2);

	TimeSpec operator - (const TimeSpec &t2);

	TimeSpec operator + (const TimeSpec &t2);

	bool operator > (const TimeSpec &t2);

	bool operator >= (const TimeSpec &t2);

	bool operator < (const TimeSpec &t2);

	bool operator <= (const TimeSpec &t2);

public:

	void cleanup_nanosecs();

};

}