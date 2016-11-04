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

//remove after debugging
#include <iostream>

bool ZFilter::FilterSolve(chai3d::cVector3d& currentLocation)
{
	//take in x,y,z (z = 0) and determine what the filter should produce
	static const chai3d::cVector3d originOfObj = chai3d::cVector3d(1.0, 1.0, 0.0);

	const static double ScaleFloor = 0.5;
	static const chai3d::cVector3d TL = chai3d::cVector3d(
		-ScaleFloor + originOfObj.x(),
		ScaleFloor + originOfObj.y(),
		0.0);
	static const chai3d::cVector3d BR = chai3d::cVector3d(
		ScaleFloor + originOfObj.x(),
		-ScaleFloor + originOfObj.y(),
		0.0);

	//bounds defined by user. will be set more dynmaically later
	static const double scale = 1.0;
	
	//top wall
	if (currentLocation.z() < TL.y() &&
		currentLocation.z() > BR.y() &&
		currentLocation.x() < BR.x() &&
		currentLocation.x() > TL.x())
	{
		//std::cout << "top wall" << std::endl;
	}
	else if(
		currentLocation.x() < 1.0 &&
		currentLocation.x() > TL.x() &&
		currentLocation.z() < 1.14 &&
		currentLocation.z() > -0.15 ) // right wall
	{
		//std::cout << "right wall" << std::endl;
		return true;
	}
	else if(
		currentLocation.x() > TL.x() &&
		currentLocation.x() < BR.x() &&
		currentLocation.z() < TL.y() &&
		currentLocation.z() > BR.y()) //left wall
	{ 
	//	std::cout << "left wall" << std::endl;
	}
	else if (
		currentLocation.x() < TL.x() &&
		currentLocation.x() > BR.x() &&
		currentLocation.z() > BR.y() &&
		currentLocation.z() < TL.y()) //bottom wall
	{
	//	std::cout << "bottom wall" << std::endl;
	}
	else
	{
		Filter.Amplitude = 0;
		return false;
	}

}


