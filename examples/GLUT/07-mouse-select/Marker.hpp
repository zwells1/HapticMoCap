#pragma once
/*

Marker.hpp this class is designed to keep track of a set of markers that will
be used in the MotionCapture program.

It will handle the configuration settings of where the markers will be placed
and how they are updated.

When one is occluded generally the marker will disappear and will come back
with a different value this class will attempt to keep them numbered in a
convential way so that they will be easier to use in future programs
starting with numbering starting at 0 and asscending

File Name: Marker.hpp
Author: Zachary Wells
Date: 10/17/2016
Tested With: Chai version 3.1.1
*/

//-----------------------------------------------------------------------------
#ifndef INCLUDE_Marker
#define INCLUDE_Marker


#include "MoCapLink.hpp"
#include <string>
#include <vector>


//#include "MoCapLink.hpp"

//-----------------------------------------------------------------------------
//Public Marker struct may make private later
struct Markers
{
	unsigned int MarkerNumber = 0;
	std::string ID = "Obj";
	double XPos = 0;
	double YPos = 0;
	double ZPos = 0;


	friend bool operator != (const Markers& lhs, const Markers& rhs)
	{
		if (lhs.XPos != rhs.XPos) return true;
		if (lhs.YPos != rhs.YPos) return true;
		if (lhs.ZPos != rhs.ZPos) return true;

		return false;
	}

};


class ZMarker
{
	//-------------------------------------------------------------------------
	// PUBLIC METHODS:
	//-------------------------------------------------------------------------
public:
	//constructor
	ZMarker();

	//destructor
	~ZMarker();

	//-----------------------------------------------------------------------
	// PUBLIC METHODS:
	//--------------------------------------------------------------------------
public:

	void ConfigureInitialMarkers();
	
	std::vector<Markers> ZMarker::GetPositionOfMarkers();
	
		//return the number of markers
	int ZMarker::GetNumberOfMarkers();

	std::vector<Markers> UpdatePositionOfMarkers();

	void DiffNumMarkers(bool numMarkers);

	bool DifferentNumberOfMarkers();



	//-------------------------------------------------------------------------
	// PRIVATE METHODS:
	//--------------------------------------------------------------------------
private:
	void SetMarkers();

	
	//-------------------------------------------------------------------------
	// PUBLIC MEMBERS:
	//--------------------------------------------------------------------------
public:

	unsigned int NumOfMarkers;
	
	std::vector<Markers> MarkerSet;

	//-------------------------------------------------------------------------
	// PRIVATE MEMBERS:
	//--------------------------------------------------------------------------
private:
	
	std::vector<Markers> PrevMarkerSet;

	std::string CurrentData;

	ZMoCapLink Link;

	bool DiffNumberMarkers;

};
#endif
