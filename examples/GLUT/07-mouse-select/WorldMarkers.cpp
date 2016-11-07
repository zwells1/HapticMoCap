/*	File Name: WorldMarker.cpp
Author : Zachary Wells
Date : 10 / 27 / 2016
Tested With : Chai version 3.1.1
*/

#include "WorldMarkers.hpp"

//find the origin
int ZWorldMarkers::FindBottomLeftCorner(std::vector<Markers>& Marks)
{
	unsigned int tempMarkerNumber;
	unsigned int VectorIndice;
	//set very large so that we can always find a larger smaller num
	float lowestX = std::numeric_limits<double>::max();
	float lowestZ = std::numeric_limits<double>::max();
	
	if (Marks.size() == 0) 
		std::cerr <<
		"World Markers:: findBLCorner error"
		<< std::endl;
	
	int i = -1;
	for (auto curr : Marks)
	{
		i++;
		if (curr.XPos <= lowestX &&
			curr.ZPos <= lowestZ) 
		{
				lowestX = curr.XPos;
				lowestZ = curr.ZPos;
				tempMarkerNumber = curr.MarkerNumber;
				VectorIndice = i;
		}
	}
	
	WorldMarker temp;
	
	temp.MarkerNumber = tempMarkerNumber;

	cVector3d Offset(
		Marks[VectorIndice].XPos,
		Marks[VectorIndice].YPos,
		Marks[VectorIndice].ZPos);

	SetOrigin(Offset);

	CreateMarkers(Marks[VectorIndice], temp);

	return tempMarkerNumber;
}

//constructor
ZWorldMarkers::ZWorldMarkers()
{
	mBL = -1;
}

//destructor
ZWorldMarkers::~ZWorldMarkers()
{
}


void ZWorldMarkers::InitFrame(std::vector<Markers> InitMarkerSet)
{
		
	//find the BL corner (smallest X, Z value)
	mBL = FindBottomLeftCorner(InitMarkerSet);

	//sanity check make sure i dont have to points that are the deemed
	//two seperate corners
	if (mBL == -1)
	{
		std::cout <<
			"Corners claim to be the same corner" <<
			std::endl;
		exit(1);
	}

	MakeCornerPermanent();
}

//add markers to the ReferenceMarkers
void ZWorldMarkers::CreateMarkers(Markers& obj, WorldMarker& SetMarker)
{
	//create a unique mesh name
	cMesh* mesh = new cMesh();

	
	//deal with the offset to pull everything back to the origin
	double X = obj.XPos - OriginOffset.x();
	double Y = obj.YPos - OriginOffset.y();
	double Z = obj.ZPos - OriginOffset.z();
	
	
	// build mesh using a cylinder primitive
	cCreateSphere(mesh,
		0.05,
		36,
		36,
		cVector3d(X, Z, Y),
		cMatrix3d(cDegToRad(0), cDegToRad(0), cDegToRad(0), C_EULER_ORDER_XYZ)
	);

	// set material properties
	mesh->m_material->setGreenLimeGreen();

	const double toolRadius = 0.05;
	mesh->createAABBCollisionDetector(toolRadius);

	SetMarker.Marker = mesh;

	SetMarker.X = obj.XPos;
	SetMarker.Y = obj.YPos;
	SetMarker.Z = obj.ZPos;

	mesh->setLocalPos(X, Z, Y);
		
	ReferenceMarkers.push_back(SetMarker);
}

std::vector<WorldMarker> ZWorldMarkers::GetReferenceMarkers()
{
	return ReferenceMarkers;
}

void ZWorldMarkers::MakeCornerPermanent()
{
	if(ReferenceMarkers.size() != 1)
	{
		std::cout << "ZWorldMarkers::MakeCornersPermanent is being used"
			<< "improperly there are "
			<< ReferenceMarkers.size()
			<< "there should only be a BL, TL, BR corners"
			<< std::endl;
		exit(1);
	}
	
	for (auto &curr : ReferenceMarkers)
	{
		curr.ignore = true;
	}
}

//initialize the rest of the markers that are not the frame
void ZWorldMarkers::InitAllMotionMarkers(std::vector<Markers> InitMarkerSet)
{
	for (auto curr : InitMarkerSet)
	{
		if (CornerMarkerTest(curr.MarkerNumber))
		{
			WorldMarker temp;
			temp.MarkerNumber = curr.MarkerNumber;
			CreateMarkers(curr, temp);
		}
	}
}

bool ZWorldMarkers::CornerMarkerTest(unsigned int& check)
{
	if (check != mBL)
	{
		return true;
	}
	else 
	{
		return false;
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

void ZWorldMarkers::SetOrigin(cVector3d& Offset)
{
	OriginOffset = Offset;
}

cVector3d ZWorldMarkers::GetOrigin()
{
	return OriginOffset;
}

void ZWorldMarkers::EraseMarker(int& index)
{
	
	if (index < ReferenceMarkers.size())
	{
		delete ReferenceMarkers[index].Marker;
		//could have out of bounds issues !!! ???	
		ReferenceMarkers.erase(ReferenceMarkers.begin() + index);
	}
	else
	{
		std::cout << "error out of bounds could not delete obj"<< std::endl;
	}
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
			delete Ref.Marker;
			ReferenceMarkers.erase(ReferenceMarkers.begin() + i);
			Markers temp;
			temp.XPos = Ref.X;
			temp.YPos = Ref.Y;
			temp.ZPos = Ref.Z;
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
