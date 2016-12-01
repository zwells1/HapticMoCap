/*	File Name: Objects.cpp
Author : Zachary Wells
Date : 10 / 30 / 2016
Tested With : Chai version 3.1.1
*/
#include "graphics/CPrimitives.h"
#include "Objects.hpp"

//debug
#include <iostream>

//constructor
ZObjects::ZObjects()
{
	mCollidedIndice = -1;
}

//destructor
ZObjects::~ZObjects()
{
}

void ZObjects::MakeBox(
	cVector3d& Dimensions,
	cVector3d& Location,
	ObjectColor Color,
	std::string& Amplitude)
{
	//spawn object
	cMesh* mesh = new cMesh();
	Object Box;

	//create object
	cCreateBox(mesh,
		Dimensions.x(),
		Dimensions.y(),
		Dimensions.z(),
		cVector3d(Location.x(), Location.y(), Location.z()),
		cMatrix3d(cDegToRad(0), cDegToRad(0), cDegToRad(0), C_EULER_ORDER_XYZ));

	// set material properties
	if (Color == ObjectColor::red)
	{
		mesh->m_material->setRedCrimson();
	}
	else if (Color == ObjectColor::yellow)
	{
		mesh->m_material->setYellow();
	}
	else if (Color == ObjectColor::green)
	{
		mesh->m_material->setGreenLight();
	}
	else
	{
		mesh->m_material->setPurpleLavender();
	}

	mesh->setLocalPos(Location.x(), Location.y(), Location.z());

	Box.Mesh = mesh;

	Box.Amplitude = Amplitude;

	//record all important edges for collision purposes
	double x;
	double y;

	//visuals have a weird offset that cannot realize with contact
	double buffer = 0.05;

	x = Location.x() + (Dimensions.x() / 2) - buffer;
	y = Location.y() + (Dimensions.y() / 2) - buffer;
	Box.TopRight = cVector3d(x, y, 0.0);

	x = Location.x() - (Dimensions.x() / 2) + buffer;
	y = Location.y() + (Dimensions.y() / 2) - buffer;
	Box.TopLeft = cVector3d(x, y, 0.0);

	x = Location.x() - (Dimensions.x() / 2) + buffer;
	y = Location.y() - (Dimensions.y() / 2) + buffer;
	Box.BottomLeft = cVector3d(x, y, 0.0);

	x = Location.x() + (Dimensions.x() / 2) - buffer;
	y = Location.y() - (Dimensions.y() / 2) + buffer;
	Box.BottomRight = cVector3d(x, y, 0.0);

	Box.ObjectIndice = mObjectIndices;

	AllObjects.push_back(Box);

	mObjectIndices++;
}

bool ZObjects::CollisionDetection(cVector3d& Marker)
{
	bool collision = false;
	for (auto& curr : AllObjects)
	{
		if (CheckBounds(curr, Marker))
		{
			collision = true;
		}

		if (collision == true)
		{
			curr.Collision = true;
			mCollidedIndice = curr.ObjectIndice;
			break;
		}
	}
	return collision;
}

Object ZObjects::GetAllObject(int& Index)
{
	return AllObjects[Index];
}

int ZObjects::GetNumberOfObjects()
{
	return AllObjects.size();
}

void ZObjects::DeleteObject(int& Index)
{
	delete AllObjects[Index].Mesh;
	AllObjects.erase(AllObjects.begin() + Index);
}

std::vector<Object> ZObjects::GetAllObjects()
{
	return AllObjects;
}

std::string ZObjects::GetAmplitudeOfCollidedObject()
{
	std::string temp;
	for (auto& curr : AllObjects)
	{
		if (curr.Collision == true)
		{
			temp = curr.Amplitude;
			curr.Collision = false;
			break;
		}
	}
	//std::cout << temp << std::endl;
	return temp;
}

bool ZObjects::IsCollisionLastObject()
{
	if (mCollidedIndice == (AllObjects.size()-1) )
	{
		//reset
		mCollidedIndice = -1;
		return true;
	}
	return false;
}

void ZObjects::SetAmplitudeofLastObject(std::string amplitude)
{
	AllObjects.back().Amplitude = amplitude;
}


//private functions
bool ZObjects::CheckBounds(Object& Obj, cVector3d& Marker)
{
	bool MarkerCollided = false;

	if (
		Obj.TopRight.x() > Marker.x() &&
		Obj.TopRight.y() > Marker.y() &&   //double check believe it is still z
		Obj.BottomLeft.x() < Marker.x() &&
		Obj.BottomLeft.y() < Marker.y())  //double check believe it is still z !!! ???
	{
		MarkerCollided = true;
	}

	/*
	bool result = Obj.TopRight.x() > Marker.x();
	std::cout << "Obj.TopRight.x() > Marker.x() " << result << std::endl;

	result = Obj.TopRight.y() > Marker.y();
	std::cout << "Obj.TopRight.y() > Marker.y() " << result << std::endl;

	result = Obj.BottomLeft.x() < Marker.x();
	std::cout << "Obj.BottomLeft.x() < Marker.x() " << result << std::endl;

	result = Obj.BottomLeft.y() < Marker.y();
	std::cout << "Obj.BottomLeft.y() < Marker.y() " << result << std::endl;

	std::cout << "marker " << Marker.x() << " " << Marker.y() << " " << Marker.z() << " " << std::endl;
	*/
	return MarkerCollided;
}