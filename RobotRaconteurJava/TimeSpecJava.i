namespace RobotRaconteur
{
class TimeSpec
{
public:
	int64_t seconds;
	int32_t nanoseconds;

	TimeSpec();

	TimeSpec(int64_t seconds, int32_t nanoseconds);

public:

    %rename(equals) operator==;
    %rename(ne) operator!=;
    %rename(sub) operator-;
    %rename(add) operator+;
    %rename(gt) operator>;
    %rename(lt) operator<;
    %rename(ge) operator>=;
    %rename(le) operator<=;

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