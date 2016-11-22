/*	File Name: BiQuadFilter.cpp
Author : Zachary Wells
Date : 11 / 21 / 2016
Tested With : Chai version 3.1.1
*/

#include "BiQuadFilter.hpp"

//constructor
ZBiQuadFilter::ZBiQuadFilter()
{

}

//destructor
ZBiQuadFilter::~ZBiQuadFilter()
{

}

BiQuadFilterVars ZBiQuadFilter::SolveForCoefficient(BiQuadType type, float Fc, float Q, float Fs)
{
	Filter.FreqCutOffValue = Fc;
	Filter.
}

BiQuadFilterVars ZBiQuadFilter::SolveForCoefficient(BiQuadType type, float Fc, float Fs)
{
	//default value for Q
	float Q = 0.7071;
	SolveForCoefficient(type, Fc, Q, Fs);
}

bool ZBiQuadFilter::CheckForCutoffFreqChange(float newCutoffFreq)
{
	if (newCutoffFreq != Filter.FreqCutOffValue)
	{
		return true;
	}
	return false;
}

void ZBiQuadFilter::SetGain(short Gain) 
{
	Filter.Gain = Gain;
}


std::string ZBiQuadFilter::GetFilterType()
{
	return Filter.type;
}

void ZBiQuadFilter::AdjustFilterType(int Adjust)
{
	if (Adjust == -1 && Filter.type == eMinValue)
	{
		Filter.type = eLowPass;
	}
	else if (Adjust == 1 && Filter.type == eMaxValue)
	{
		Filter.type = eHighShelf;
	}
	else
	{
		(int)Filter.type++;
	}
}

void ZBiQuadFilter::AdjustCutoffFreq(int Adjust)
{
	Filter.FreqCutOffValue += Adjust;
}