#pragma once
/*
BiQuadFilter.hpp this class is designed to interpret where the Marker is
and to provide feedback that with a parameterisized object. I imagine that each object
will be interpretted different.


File Name: Filter.hpp
Author: Zachary Wells
Date: 10/18/2016
Tested With: Chai version 3.1.1
*/

//-----------------------------------------------------------------------------
#ifndef INCLUDE_BiQuadFilter
#define INCLUDE_BiQuadFilter

#include <vector>
struct BiQuadFilterVars
{
	short PeakGain = 1;
	int FreqCutOffValue = 100;
	//default value for Q
	float Q = 0.7071;
	int Fs = 2000;
	bool NewParameters = false;
	int type = 0;
	std::string TypeName = "LowPass";
	float a0;
	float a1;
	float a2;
	float b1;
	float b2;
	int NumberOfParameters = 5;

};

enum BiQuadType
{
	eMinValue  = 0, 
	eLowPass   = 0,
	eHighPass  = 1,
	eBandPass  = 2,
	eNotch     = 3,
	eLowShelf  = 4,
	eHighShelf = 5,
	eMaxValue  = 5
};

class ZBiQuadFilter
{
	//-------------------------------------------------------------------------
	// PUBLIC METHODS:
	//-------------------------------------------------------------------------
public:

	//constructor
	ZBiQuadFilter();

	//destructor
	~ZBiQuadFilter();


	BiQuadFilterVars SolveForCoefficient(int type, float Fc, float Q, float Fs, float Gain);

	//default Q = 0.7071
	BiQuadFilterVars SolveForCoefficient(int type, float Fc, float Fs, float Gain);
	
	//all filter parameters already set
	BiQuadFilterVars ZBiQuadFilter::SolveForCoefficient();

	bool CheckForBiQuadFilterChange();

	void SetGain(short Gain);

	std::string GetGainString();

	void AdjustFilterType(int Adjust);

	void AdjustCutoffFreq(int Adjust);

	std::string GetBiQuadFilterType();

	std::string SetBiQuadFilterType(int index);

	void AdjustPeakGain(short PeakGain);

	int GetBiQuadCutoffFreq();

	void ZBiQuadFilter::SetFilterParameters();

	std::vector<std::string> GetFilterParameters();

	int GetFilterSize();

	//-------------------------------------------------------------------------
	// PRIVATE METHODS:
	//--------------------------------------------------------------------------
private:

	void SetupFilterTypes();

	void SolveBiQuadFilter(int type, float Fc, float Q, float Fs, float PeakGain);

	//-------------------------------------------------------------------------
	// PUBLIC MEMBERS:
	//--------------------------------------------------------------------------
public:
	
	BiQuadFilterVars Filter;
	
	std::vector<std::pair<int, std::string>> BiQuadType;

	std::vector<std::string> Coefficients;

};

#endif