/*	File Name: WorldMarker.cpp
Author : Zachary Wells
Date : 10 / 27 / 2016
Tested With : Chai version 3.1.1
*/

#include "WorldMarkers.hpp"

//constructor
ZWorldMarkers::ZWorldMarkers()
{
}

//destructor
ZWorldMarkers::~ZWorldMarkers()
{
}


//add markers to the ReferenceMarkers
void ZWorldMarkers::CreateMarkers(Markers& obj, WorldMarker& SetMarker)
{
	//create a unique mesh name
	cMesh* mesh = new cMesh();
	
	// build mesh using a cylinder primitive
	cCreateSphere(mesh,
		0.05,
		36,
		36,
		cVector3d(obj.XPos, obj.ZPos, obj.YPos),
		cMatrix3d(cDegToRad(0), cDegToRad(0), cDegToRad(0), C_EULER_ORDER_XYZ)
	);

	// set material properties
	mesh->m_material->setGreenLimeGreen();

	const double toolRadius = 0.05;
	mesh->createAABBCollisionDetector(toolRadius);

	SetMarker.Marker = mesh;

	//keep original position, dont include offset too hard to compare later
	SetMarker.X = obj.XPos;
	SetMarker.Y = obj.YPos;
	SetMarker.Z = obj.ZPos;

	mesh->setLocalPos(obj.XPos, obj.ZPos, obj.YPos);
		
	ReferenceMarkers.push_back(SetMarker);
}

std::vector<WorldMarker> ZWorldMarkers::GetReferenceMarkers()
{
	return ReferenceMarkers;
}

//initialize the rest of the markers that are not the frame
void ZWorldMarkers::InitAllMotionMarkers(std::vector<Markers> InitMarkerSet)
{
	for (auto curr : InitMarkerSet)
	{
		WorldMarker temp;
		temp.MarkerNumber = curr.MarkerNumber;
		CreateMarkers(curr, temp);	
	}
}

//if a marker has moved set that marker to update and update the new values
bool ZWorldMarkers::CheckForMovedMarkerPosition(std::vector<Markers>& NewPos)
{
	bool AnythingNew = false;
	
	for (auto& New : NewPos)
	{
		bool found = false;
		for (auto& Ref : ReferenceMarkers)
		{
			if (New.MarkerNumber == Ref.MarkerNumber) 
			{
				found = true;
				if (CompareMarkers(New, Ref))
				{
					AnythingNew = true;
					Ref.ReadyForUpdate = true;

					Ref.X = New.XPos;
					Ref.Y = New.YPos;
					Ref.Z = New.ZPos;
					 
				}
			}	
		}

		//add new marker if it was not found
		if (found == false)
		{
			AnythingNew = true;
			WorldMarker NewWorldMarker;
			NewWorldMarker.MarkerNumber = New.MarkerNumber;
			//special case to add these
			NewWorldMarker.AddToWorld = true; 
			CreateMarkers(New, NewWorldMarker);
		}
	}
	
	return AnythingNew;
}

bool ZWorldMarkers::CompareMarkers(Markers& Mark, WorldMarker& Ref)
{
	if (Mark.XPos != Ref.X) return true;
	if (Mark.YPos != Ref.Y) return true;
	if (Mark.ZPos != Ref.Z) return true;

	return false;

}

size_t ZWorldMarkers::NumberOfWorldMarkers()
{
	return ReferenceMarkers.size();
}


void ZWorldMarkers::EraseMarker(int& index)
{
	delete ReferenceMarkers[index].Marker;
	ReferenceMarkers.erase(ReferenceMarkers.begin() + index);
}



void ZWorldMarkers::SwapMarker(WorldMarker& Replace)
{	
	//create new
	int i = -1;
	for (auto& Ref : ReferenceMarkers)
	{
		i++;
		if (Replace.MarkerNumber == Ref.MarkerNumber)
		{
			Markers temp;
			temp.XPos = Ref.X;
			temp.YPos = Ref.Y;
			temp.ZPos = Ref.Z;
			//remove old marker must happen after pulling data from it
			delete Ref.Marker;
			ReferenceMarkers.erase(ReferenceMarkers.begin() + i);
			CreateMarkers(temp, Replace);
			break;
		}
	}
}

//eventually turn this into an object that only check ones that are not ignored
cVector3d ZWorldMarkers::GrabLastElement()
{
	WorldMarker temp = ReferenceMarkers.back();

	//remove the offset back in
	return temp.Marker->getLocalPos();
}
