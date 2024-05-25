#pragma once

namespace rt 
{

class Interval
{
public:
	Interval() : min(+Inf), max(-Inf) {}; /* Default interval is empty */
	Interval(double min, double max) : min(min), max(max) {};

	double Size() const { return max - min; };
	bool Contains(double x) const { return min <= x && x <= max; };
	bool Surrounds(double x) const { return min < x && x < max; };
	
	double Clamp(double x) const 
	{
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

public:
	double min, max;
	static const Interval empty, universe;
};

const Interval Interval::empty = Interval(+Inf, -Inf);
const Interval Interval::universe = Interval(-Inf, +Inf);

} /* namespace rt */