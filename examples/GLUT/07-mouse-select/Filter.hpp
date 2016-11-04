#pragma once
/*
Filter.hpp this class is designed to interpret where the Marker is 
and to provide feedback via wifi signal of where the hand and the 
appropriate signals to provide for the differing features of the filter.


File Name: Filter.hpp
Author: Zachary Wells
Date: 10/18/2016
Tested With: Chai version 3.1.1
*/

//-----------------------------------------------------------------------------
#ifndef INCLUDE_Filter
#define INCLUDE_Filter

//parrallelism consider using later on
//#include <amp.h>
#include <math/CVector3d.h>

struct FilterVars
{
	short Amplitude;
	short PhaseShift;

};


class ZFilter
{
	//-------------------------------------------------------------------------
	// PUBLIC METHODS:
	//-------------------------------------------------------------------------
public:
	
	//constructor
	ZFilter();

	//destructor
	~ZFilter();


	//-------------------------------------------------------------------------
	// PUBLIC METHODS:
	//--------------------------------------------------------------------------
public:

	//get boost asio or something like it working to send data over wifi
	void SendData();

	//take the current coordinates and run a filter max with the input
	//being the x,y location on the map.
	bool FilterSolve(chai3d::cVector3d& currentLocation);

	std::string GetFilterAmplitude();



	//-------------------------------------------------------------------------
	// PRIVATE METHODS:
	//--------------------------------------------------------------------------
private:



	//-------------------------------------------------------------------------
	// PUBLIC MEMBERS:
	//--------------------------------------------------------------------------
public:
	FilterVars Filter;


};
#endif