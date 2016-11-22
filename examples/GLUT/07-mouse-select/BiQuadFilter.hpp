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

#include <math/CVector3d.h>

struct BiQuadFilterVars
{
	short Gain;
	int FreqCutOffValue;
	BiQuadType type;
	float a0;
	float a1;
	float a2;
	float b1;
	float b2;

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


	BiQuadFilterVars SolveForCoefficient(BiQuadType type, float Fc, float Q, float Fs);

	//default Q = 0.7071
	BiQuadFilterVars SolveForCoefficient(BiQuadType type, float Fc, float Fs);

	bool CheckForCutoffFreqChange(float newCutoffFreq);

	void SetGain(short Gain);

	std::string GetFilterType();

	void AdjustFilterType(int Adjust);

	void AdjustCutoffFreq(int Adjust);

	//-------------------------------------------------------------------------
	// PRIVATE METHODS:
	//--------------------------------------------------------------------------
private:



	//-------------------------------------------------------------------------
	// PUBLIC MEMBERS:
	//--------------------------------------------------------------------------
public:
	
	BiQuadFilterVars Filter;

};

#endif