/*	File Name: Filter.cpp
Author : Zachary Wells
Date : 10 / 18 / 2016
Tested With : Chai version 3.1.1
*/

#include "Filter.hpp"

//helper
std::string SendStringFormat(int num2Convert)
{
	std::stringstream ss;
	ss << num2Convert;
	std::string target = ss.str();

	return target;
}



//constructor
ZFilter::ZFilter()
{
	//set filter to have no effect in the beginning
	Filter.Amplitude = 0;
	Filter.PhaseShift = 0;

	Filter.LowerFreqCutOffValue = 100;
	Filter.UpperFreqCutOffValue = 500;
}

//destructor
ZFilter::~ZFilter()
{
}


void ZFilter::SendData()
{

}

std::string ZFilter::GetFilterAmplitude()
{
	std::string AmpString = SendStringFormat(Filter.Amplitude);

	return AmpString;
}

int ZFilter::GetLowerFreqCutOff()
{
	return Filter.LowerFreqCutOffValue;
}

void ZFilter::SetLowerFreqCutOff(int NewVal)
{
	Filter.LowerFreqCutOffValue = NewVal;
}

void ZFilter::AdjustLowerFreqCutOff(int Adjust)
{
	Filter.LowerFreqCutOffValue += Adjust;
}

int ZFilter::GetUpperFreqCutOff()
{
	return Filter.UpperFreqCutOffValue;
}

void ZFilter::SetUpperFreqCutOff(int NewVal)
{
	Filter.UpperFreqCutOffValue = NewVal;
}

void ZFilter::AdjustUpperFreqCutOff(int Adjust)
{
	Filter.UpperFreqCutOffValue += Adjust;
}

