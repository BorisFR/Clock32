#pragma once
#ifndef EXPONENTIAL_FILTER_HPP
#define EXPONENTIAL_FILTER_HPP

#include "Arduino.h"

// taken here: https://github.com/Megunolink/MLP/blob/master/Filter.h
/*
 * Implements a simple linear recursive exponential filter.
 * See: http://www.statistics.com/glossary&term_id=756 */
class ExponentialFilter
{
public:
	ExponentialFilter(){};
	void Filter(long New) { m_Current = (100 * m_WeightNew * New + (100 - m_WeightNew) * m_Current + 50) / 100; }
	void SetWeight(long NewWeight) { m_WeightNew = NewWeight; }
	long GetWeight() const { return m_WeightNew; }
	long Current() const { return (m_Current + 50) / 100; }
	void SetCurrent(long NewValue) { m_Current = NewValue * 100; }

private:
	// Weight for new values, as a percentage ([0..100])
	long m_WeightNew;
	// Current filtered value.
	long m_Current;
};

#endif