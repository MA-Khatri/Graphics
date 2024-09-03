#pragma once

namespace rt 
{

class Interval
{
public:
	Interval() : min(+Inf), max(-Inf) {}; /* Default interval is empty */

	Interval(double min, double max) : min(min), max(max) {};

	Interval(const Interval& a, const Interval& b)
	{
		/* Create a new interval enclosing the two provided intervals */
		min = a.min <= b.min ? a.min : b.min;
		max = a.max >= b.max ? a.max : b.max;
	}

	double Size() const { return max - min; };
	bool Contains(double x) const { return min <= x && x <= max; };
	bool Surrounds(double x) const { return min < x && x < max; };
	
	double Clamp(double x) const 
	{
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	Interval Expand(double delta)
	{
		double padding = delta / 2.0;
		return Interval(min - padding, max + padding);
	}

public:
	double min, max;
	//static const Interval empty;
	//static const Interval universe;
};

//const Interval Interval::empty = Interval(+Inf, -Inf);
//const Interval Interval::universe = Interval(-Inf, +Inf);

} /* namespace rt */