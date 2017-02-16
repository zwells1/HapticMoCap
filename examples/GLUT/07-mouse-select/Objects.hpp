#pragma once
/*

These are objects that will populate the world
At the current point they will be stationary objects that will

File Name : WorldMarker.hpp
Author : Zachary Wells
Date : 10 / 30 / 2016
Tested With : Chai version 3.1.1
*/

#ifndef INCLUDE_Objects
#define INCLUDE_Objects

#include <math/CVector3d.h>
#include "world\CMesh.h"
#include "materials\CMaterial.h"
#include <vector>

using namespace chai3d;

//eventually make different types of objects and rename to box
struct Object
{
	cMesh* Mesh;
	cVector3d TopLeft;
	cVector3d TopRight;
	cVector3d BottomLeft;
	cVector3d BottomRight;
	bool Collision = false;
	cMaterial* Color;
	std::string Amplitude;
	int ObjectIndice;

};

enum class ObjectColor
{
	red = 0, yellow = 1, green = 2, purple = 3
};


class ZObjects
{
	//-------------------------------------------------------------------------
	// PUBLIC METHODS:
	//-------------------------------------------------------------------------
public:
	//constructor
	ZObjects();

	//destructor
	~ZObjects();

	//-------------------------------------------------------------------------
	// PUBLIC METHODS:
	//-------------------------------------------------------------------------
public:

	void ZObjects::MakeBox(
		cVector3d& Dimensions,
		cVector3d& Location,
		ObjectColor Color,
		std::string& Power);

	//run through
	bool CollisionDetection(cVector3d& Marker);

	Object GetAllObject(int& Index);

	int GetNumberOfObjects();

	void DeleteObject(int& Index);

	std::vector<Object> GetAllObjects();

	std::string GetAmplitudeOfCollidedObject();

	bool IsCollisionLastObject();

	void SetAmplitudeofLastObject(std::string amplitude);

	//-------------------------------------------------------------------------
	// PRIVATE METHODS:
	//-------------------------------------------------------------------------
private:

	bool ZObjects::CheckBounds(Object& Obj, cVector3d& Marker);

	//-------------------------------------------------------------------------
	// PRIVATE MEMBERS:
	//-------------------------------------------------------------------------
private:

	std::vector<Object> AllObjects;

	int mObjectIndices = 0;

	int mCollidedIndice = -1;
};

#endif