#pragma once
/*

WorldMarkers is a helper class to MoCapIntegrated that will keep track off the
Cmesh objects, cmesh marker objects, etc.

It will also help set up the markers in the beginning and create a initial frame
to always be in reference to.

keep in mind that once marker positions are grabbed and set there will be a 
constant offset to the Marker of Interest(MOI) that will need to update a constant
offset in the Marker.cpp

File Name : WorldMarker.hpp
Author : Zachary Wells
	Date : 10 / 27 / 2016
	Tested With : Chai version 3.1.1
*/
#ifndef INCLUDE_World_Marker
#define INCLUDE_World_Marker

#include "Marker.hpp"
#include "world\CMesh.h"
#include <math/CVector3d.h>
#include "graphics/CPrimitives.h"
#include <vector>

using namespace chai3d;

struct WorldMarker
{
	cMesh* Marker;
	unsigned int MarkerNumber = 0;
	bool ignore = false;
	bool ReadyForUpdate = false;
	bool AddToWorld = false;
	double X = 0.0;
	double Y = 0.0;
	double Z = 0.0;
};

class ZWorldMarkers
{
	//-------------------------------------------------------------------------
	// PUBLIC METHODS:
	//-------------------------------------------------------------------------
public:
	//constructor
	ZWorldMarkers();

	//destructor
	~ZWorldMarkers();

	//-----------------------------------------------------------------------
	// PUBLIC METHODS:
	//--------------------------------------------------------------------------
public:

	void InitFrame(std::vector<Markers>);

	void CreateMarkers(Markers& obj, WorldMarker& SetMarker);

	std::vector<WorldMarker> GetReferenceMarkers();

	void InitAllMotionMarkers(std::vector<Markers> InitMarkerSet);

	bool ZWorldMarkers::CheckForMovedMarkerPosition(std::vector<Markers>& NewPos);

	size_t NumberOfWorldMarkers();

	cVector3d GetOrigin();

	void SetEraseMarker(bool Value);

	bool GetEraseMarker();

	void EraseWorldMarkers();

	void EraseMarker(int& index);

	void SwapMarker(WorldMarker& Replace);
	
	cVector3d GrabLastElement();

	//-----------------------------------------------------------------------
	// PRIVATE METHODS:
	//--------------------------------------------------------------------------
private:

	int FindBottomLeftCorner(std::vector<Markers>& Marks);
	
	int FindTopLeftCorner(std::vector<Markers>& Marks);
	
	int FindBottomRightCorner(std::vector<Markers>& Marks);

	void ZWorldMarkers::MakeCornersPermanent();

	bool CompareMarkers(Markers& Mark, WorldMarker& Ref);

	bool CornerMarkerTest(unsigned int& check);

	void SetOrigin(cVector3d& Offset);

	
	//-------------------------------------------------------------------------
	// PRIVATE MEMBERS:
	//--------------------------------------------------------------------------
private:

	std::vector<WorldMarker> ReferenceMarkers;

	cVector3d OriginOffset;

	int mBR;
	int mBL;
	int mTL;
	

};
#endif
