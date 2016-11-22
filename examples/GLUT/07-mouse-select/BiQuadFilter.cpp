/*	File Name: BiQuadFilter.cpp
Author : Zachary Wells
Date : 11 / 21 / 2016
Tested With : Chai version 3.1.1
*/

#include "BiQuadFilter.hpp"
#if(_MSC_VER == 1900)
#define _USE_MATH_DEFINES 
#include <cmath> //abs
#include <ATLComTime.h>
#else
#include <math.h>  //pow , tan
#include <cmath> //abs
#endif


//constructor
ZBiQuadFilter::ZBiQuadFilter()
{
	SetupFilterTypes();
}

//destructor
ZBiQuadFilter::~ZBiQuadFilter()
{

}

BiQuadFilterVars ZBiQuadFilter::SolveForCoefficient(int type, float Fc, float Q, float Fs, float PeakGain)
{
	Filter.FreqCutOffValue = Fc;
	if (type != Filter.type ||
		Fc != Filter.FreqCutOffValue ||
		PeakGain != Filter.PeakGain)
	{
		Filter.TypeName = SetBiQuadFilterType(type);
		Filter.type = type;

		SolveBiQuadFilter(type, Fc, Q, Fs, PeakGain);
	}
	
	return Filter;
}

BiQuadFilterVars ZBiQuadFilter::SolveForCoefficient(int type, float Fc, float Fs, float PeakGain)
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
	Filter.PeakGain = Gain;
}


void ZBiQuadFilter::AdjustFilterType(int Adjust)
{
	if (Adjust == -1 && Filter.type == 0)
	{
		Filter.type = BiQuadType[0].first;
		Filter.TypeName = BiQuadType[0].second;
	}
	else if (Adjust == 1 && Filter.type == BiQuadType.size())
	{
		Filter.type = BiQuadType.back.first;
		Filter.TypeName = BiQuadType.back.second;
	}
	else
	{
		Filter.type = BiQuadType[Adjust].first;
		Filter.TypeName = BiQuadType[Adjust].second;
	}
}

void ZBiQuadFilter::AdjustCutoffFreq(int Adjust)
{
	Filter.FreqCutOffValue += Adjust;
}

std::string ZBiQuadFilter::GetBiQuadFilterType()
{
	return Filter.TypeName;
}

//bad name for function !!! ????
std::string ZBiQuadFilter::SetBiQuadFilterType(int index)
{
	Filter.TypeName = BiQuadType[index].second;
	return Filter.TypeName;
}


void ZBiQuadFilter::SetupFilterTypes()
{
	BiQuadType.push_back(std::make_pair(0, "LowPass"));
	BiQuadType.push_back(std::make_pair(1, "HighPass"));
	BiQuadType.push_back(std::make_pair(2, "BandPass"));
	BiQuadType.push_back(std::make_pair(3, "Notch"));
	BiQuadType.push_back(std::make_pair(4, "Peak"));
	BiQuadType.push_back(std::make_pair(5, "LowShelf"));
	BiQuadType.push_back(std::make_pair(6, "HighShelf"));
}

void ZBiQuadFilter::SolveBiQuadFilter(int type, float Fc, float Q, float Fs, float PeakGain)
{
	std::vector<float> vals;
	float a0, a1, a2, b1, b2, norm;
	float V = pow(10, std::abs(PeakGain) / 20);
	float K = tan(M_PI * Fc / Fs);
	switch (type)
	{
	case 0: //case str2int("lowpass"):
		norm = 1 / (1 + K / Q + K * K);
		a0 = K * K * norm;
		a1 = 2 * a0;
		a2 = a0;
		b1 = 2 * (K * K - 1) * norm;
		b2 = (1 - K / Q + K * K) * norm;
		break;

	case 1://case str2int("highpass"):
		norm = 1 / (1 + K / Q + K * K);
		a0 = 1 * norm;
		a1 = -2 * a0;
		a2 = a0;
		b1 = 2 * (K * K - 1) * norm;
		b2 = (1 - K / Q + K * K) * norm;
		break;

	case 2: //case str2int("bandpass"):
		norm = 1 / (1 + K / Q + K * K);
		a0 = K / Q * norm;
		a1 = 0;
		a2 = -a0;
		b1 = 2 * (K * K - 1) * norm;
		b2 = (1 - K / Q + K * K) * norm;
		break;

	case 3: //case str2int("notch"):
		norm = 1 / (1 + K / Q + K * K);
		a0 = (1 + K * K) * norm;
		a1 = 2 * (K * K - 1) * norm;
		a2 = a0;
		b1 = a1;
		b2 = (1 - K / Q + K * K) * norm;
		break;

	case 4: //case str2int("peak"):
		if (PeakGain >= 0) {    // boost
			norm = 1 / (1 + 1 / Q * K + K * K);
			a0 = (1 + V / Q * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - V / Q * K + K * K) * norm;
			b1 = a1;
			b2 = (1 - 1 / Q * K + K * K) * norm;
		}
		else {    // cut
			norm = 1 / (1 + V / Q * K + K * K);
			a0 = (1 + 1 / Q * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - 1 / Q * K + K * K) * norm;
			b1 = a1;
			b2 = (1 - V / Q * K + K * K) * norm;
		}
		break;
	case 5: //case str2int("lowShelf"):
		if (PeakGain >= 0) {    // boost
			norm = 1 / (1 + M_SQRT2 * K + K * K);
			a0 = (1 + sqrt(2 * V) * K + V * K * K) * norm;
			a1 = 2 * (V * K * K - 1) * norm;
			a2 = (1 - sqrt(2 * V) * K + V * K * K) * norm;
			b1 = 2 * (K * K - 1) * norm;
			b2 = (1 - M_SQRT2 * K + K * K) * norm;
		}
		else {    // cut
			norm = 1 / (1 + sqrt(2 * V) * K + V * K * K);
			a0 = (1 + M_SQRT2 * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - M_SQRT2 * K + K * K) * norm;
			b1 = 2 * (V * K * K - 1) * norm;
			b2 = (1 - sqrt(2 * V) * K + V * K * K) * norm;
		}
		break;
	case 6://case str2int("highShelf"):
		if (PeakGain >= 0) {    // boost
			norm = 1 / (1 + M_SQRT2 * K + K * K);
			a0 = (V + sqrt(2 * V) * K + K * K) * norm;
			a1 = 2 * (K * K - V) * norm;
			a2 = (V - sqrt(2 * V) * K + K * K) * norm;
			b1 = 2 * (K * K - 1) * norm;
			b2 = (1 - M_SQRT2 * K + K * K) * norm;
		}
		else {    // cut
			norm = 1 / (V + sqrt(2 * V) * K + K * K);
			a0 = (1 + M_SQRT2 * K + K * K) * norm;
			a1 = 2 * (K * K - 1) * norm;
			a2 = (1 - M_SQRT2 * K + K * K) * norm;
			b1 = 2 * (K * K - V) * norm;
			b2 = (V - sqrt(2 * V) * K + K * K) * norm;
		}
		break;
	}
	
	Filter.a0 = a0;
	Filter.a1 = a1;
	Filter.a2 = a2;
	Filter.b1 = b1;
	Filter.b2 = b2;
}
