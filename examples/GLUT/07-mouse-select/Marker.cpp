/*	File Name: Marker.cpp
Author : Zachary Wells
Date : 10 / 17 / 2016
Tested With : Chai version 3.1.1
*/

#include "Marker.hpp"
#include <sstream>
#include <iostream>

//constructor
ZMarker::ZMarker()
{
	NumOfMarkers = 0;
	CurrentData = "";
	DiffNumberMarkers = false;
}

//destructor
ZMarker::~ZMarker()
{
}

void ZMarker::ConfigureInitialMarkers()
{
	CurrentData = Link.CreateMotionCaptureLink();
	SetMarkers();
}

int ZMarker::GetNumberOfMarkers()
{
	return NumOfMarkers;
}


std::vector<Markers> ZMarker::GetPositionOfMarkers()
{
	return MarkerSet;
}


//this function is basically the same as ConfigureInitialMarkers
//accept it doesn't need to create the link each time 
//need to add more intelligence so it is not always updating 
//all the points
std::vector<Markers> ZMarker::UpdatePositionOfMarkers()
{
	CurrentData = Link.CreateMotionCaptureLink();

	int PrevNumofMarkers = NumOfMarkers;

	//wont need to be clearing it in the future
	MarkerSet.clear();

	SetMarkers();

	if (PrevNumofMarkers != NumOfMarkers)
	{
		DiffNumMarkers(true);
	}
	
	return MarkerSet;
}

void ZMarker::DiffNumMarkers(bool numMarkers)
{
	DiffNumberMarkers = numMarkers;
}


bool ZMarker::DifferentNumberOfMarkers()
{
	return DiffNumberMarkers;
}

//Private Functions

//fill in new data to the new markers
void ZMarker::SetMarkers()
{
	
	//fill in GetPositionOfMarkers
	size_t PrevCommaFound = 0;
	size_t NextCommaFound = 0;
	std::string st1;

	size_t CommaFound = CurrentData.find(",", PrevCommaFound);
	st1 = CurrentData.substr(PrevCommaFound, CommaFound);
	NumOfMarkers = atoi(st1.c_str());
	PrevCommaFound = CommaFound;

	st1 = CurrentData.substr(PrevCommaFound+1);
	
	//chop off gibberish as the end
	if (st1.length() != CurrentData.find(";"))
	{
		st1 = st1.substr(0,CurrentData.find_last_of(";") + 1);
	}
	
	
	//warnings
	if (NumOfMarkers == 0)
	{
		std::cerr <<
			" Marker.cpp:: error: there are no markers to collect" << std::endl;
	}

	std::string token;
	std::stringstream iss;
	iss << st1;

	//make sure the number of markers doesn't get passed in here
	for (unsigned int i = 0; i < NumOfMarkers; i++) {
		
		Markers Temp;

		//find the marker number
		getline(iss, token, ',');
		Temp.MarkerNumber = std::stoi(token);
		Temp.ID = token;

		//find the xpos
		getline(iss, token, ',');
		Temp.XPos = std::stof(token);


		//find the Ypos
		getline(iss, token, ',');
		Temp.YPos = std::stof(token);

		//find the Zpos
		getline(iss, token, ';');
		Temp.ZPos = std::stof(token);

		MarkerSet.push_back(Temp);
	}
	
	//copy to PrevMarkerSet
	PrevMarkerSet = MarkerSet;
}
