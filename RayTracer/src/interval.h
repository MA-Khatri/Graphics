#pragma once

namespace rt 
{

class Interval
{
public:
	Interval() : min(+Inf), max(-Inf) {}; /* Default interval is empty */
	Interval(float min, float max) : min(min), max(max) {};

	float Size() const { return max - min; };
	bool Contains(float x) const { return min <= x && x <= max; };
	bool Surrounds(float x) const { return min < x && x < max; };
	
	float Clamp(float x) const 
	{
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

public:
	float min, max;
	static const Interval empty, universe;
};

const Interval Interval::empty = Interval(+Inf, -Inf);
const Interval Interval::universe = Interval(-Inf, +Inf);

} /* namespace rt */