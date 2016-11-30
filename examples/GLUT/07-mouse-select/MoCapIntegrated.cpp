//==============================================================================
/*
Software License Agreement (BSD License)
Copyright (c) 2003-2016, CHAI3D.
(www.chai3d.org)

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution.

* Neither the name of CHAI3D nor the names of its contributors may
be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

\author    <http://www.chai3d.org>
\author    Francois Conti
\version   3.1.1 $Rev: 1925 $
*/
//==============================================================================

//------------------------------------------------------------------------------
// asio has to happen before windows.h otherwise errors will occur with
//winsock
#include <boost/asio.hpp> 
#include "BiQuadFilter.hpp"
#include "chai3d.h"
#include "Filter.hpp"
#include "Marker.hpp"
#include "MoCapLink.hpp"
#include "Objects.hpp"
#include "WifiConnect.hpp"
#include "WorldMarkers.hpp"

#include <mutex>
#include <sstream>
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------
#ifndef MACOSX
#include "GL/glut.h"
#else
#include "GLUT/glut.h"
#endif
//------------------------------------------------------------------------------
// TESTING SETTINGS
//------------------------------------------------------------------------------

#define WIFI_TESTING




//------------------------------------------------------------------------------
// GENERAL SETTINGS
//------------------------------------------------------------------------------

// stereo Mode
/*
C_STEREO_DISABLED:            Stereo is disabled
C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
*/
cStereoMode stereoMode = C_STEREO_DISABLED;

// fullscreen mode
bool fullscreen = false;

// mirrored display
bool mirroredDisplay = false;


//------------------------------------------------------------------------------
// DECLARED VARIABLES
//------------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a light source to illuminate the objects in the world
cSpotLight *light;

// a label to display the rate [Hz] at which the simulation is running
cLabel* labelHud;

cLabel* labelNumMarkers;

//object that is moved around
cMesh* sphere = new cMesh();

//All the Marker Spheres in the map
ZWorldMarkers* AllMarkers;

//objects related the coors of the hand
ZMarker MyHand;

std::vector<Markers> OldPos;

std::vector<Markers> NewPos;


double maxStiffness;


// indicates if the haptic simulation currently running
bool simulationRunning = false;

// indicates if the haptic simulation has terminated
bool simulationFinished = true;

// frequency counter to measure the simulation haptic rate
cFrequencyCounter frequencyCounter;

// information about computer screen and GLUT display window
int screenW;
int screenH;
int windowW;
int windowH;
int windowPosX;
int windowPosY;

//Wifi hook up
boost::asio::io_service io_service;
ZWifi WifiHook(io_service, "192.168.0.194", "123");

//Filter globals
ZFilter Filter;

//Biquad Filter
ZBiQuadFilter BiQuadFilt;


//mutex for when the objects change
std::mutex mObjectOnMapChange;

//all other objects that are on the map
ZObjects AllObjects;

std::string HudString;

std::stringstream NumberOfMarkers;

//haptic timer of when to send data
cPrecisionClock* SendDataTimer;
//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window display is resized
void resizeWindow(int w, int h);

// callback when a key is pressed
void keySelect(unsigned char key, int x, int y);

// callback to render graphic scene
void updateGraphics(void);

// callback of GLUT timer
void graphicsTimer(int data);

// function that closes the application
void close(void);

// main haptics simulation loop
void updateHaptics(void);

//scale Object
void scaleObject(cMesh& Obj, const double x, const double y, const double z);

//set the tabletop
void PlaceAndCenterFloor(cMesh* base,
	cVector3d& TopLeft,
	cVector3d& BottomRight,
	cVector3d& Origin);


//initial function to create the marker positions
void InitMarkers();

//update where the markers should go
void CheckMarkers();

void ReadAllLocalPositions();

void UpdateMarkerLocations();

void SwapMarker(WorldMarker& Mark);

//testing features
void addSphere();
void removeSphere();
void ReadValsWTF();

int main(int argc, char* argv[])
{
	//--------------------------------------------------------------------------
	// INITIALIZATION
	//--------------------------------------------------------------------------

	cout << endl;
	cout << "-----------------------------------" << endl;
	cout << "CHAI3D" << endl;
	cout << "Demo: 13-primitives" << endl;
	cout << "Copyright 2003-2016" << endl;
	cout << "-----------------------------------" << endl << endl << endl;
	cout << "Keyboard Options:" << endl << endl;
	cout << "[r] - Save copy of shadowmap to file" << endl;
	cout << "[f] - Enable/Disable full screen mode" << endl;
	cout << "[m] - Enable/Disable vertical mirroring" << endl;
	cout << "[x] - Exit application" << endl;

	cout << "[p] - grab packet from server" << endl;

	cout << "move the sphere around!!" << endl;
	cout << "[w] moves +Y" << endl;
	cout << "[s] moves -Y" << endl;
	cout << "[a] moves -X" << endl;
	cout << "[d] moves +X" << endl;
	cout << endl;
	cout << "[q] moves +Z" << endl;
	cout << "[e] moves -Z" << endl;

	cout << " [p] cutoff freq adjustment inc by 5Hz" << endl;
	cout << " [l] cutoff freq adjustment dec by 5Hz" << endl;
	
	cout << " [o] change the filter type increment" << endl;
	cout << " [k] change the filter type decrement" << endl;

	cout << endl << endl;


	//--------------------------------------------------------------------------
	// OPEN GL - WINDOW DISPLAY
	//--------------------------------------------------------------------------

	// initialize GLUT
	glutInit(&argc, argv);

	// retrieve  resolution of computer display and position window accordingly
	screenW = glutGet(GLUT_SCREEN_WIDTH);
	screenH = glutGet(GLUT_SCREEN_HEIGHT);
	windowW = 0.8 * screenH;
	windowH = 0.5 * screenH;
	windowPosY = (screenH - windowH) / 2;
	windowPosX = windowPosY;

	// initialize the OpenGL GLUT window
	glutInitWindowPosition(windowPosX, windowPosY);
	glutInitWindowSize(windowW, windowH);

	if (stereoMode == C_STEREO_ACTIVE)
		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STEREO);
	else
		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	// create display context and initialize GLEW library
	glutCreateWindow(argv[0]);

#ifdef GLEW_VERSION
	// initialize GLEW
	glewInit();
#endif

	// setup GLUT options
	glutDisplayFunc(updateGraphics);
	glutKeyboardFunc(keySelect);
	glutReshapeFunc(resizeWindow);
	glutSetWindowTitle("CHAI3D");

	// set fullscreen mode
	if (fullscreen)
	{
		glutFullScreen();
	}

	//--------------------------------------------------------------------------
	// WORLD - CAMERA - LIGHTING
	//--------------------------------------------------------------------------

	// create a new world.
	world = new cWorld();

	// set the background color of the environment
	world->m_backgroundColor.setWhite();

	// create a camera and insert it into the virtual world
	camera = new cCamera(world);
	world->addChild(camera);

	float scalar = 2.0;
	// position and orient the camera
	camera->set(cVector3d(1.0, -2.0, scalar*1.2),    // camera position (eye)
		cVector3d(1.0, 1.0, 0.0),    // lookat position (target)
		cVector3d(0.0, 0.0, 1.0));   // direction of the (up) vector

									 // set the near and far clipping planes of the camera
									 // anything in front or behind these clipping planes will not be rendered
	camera->setClippingPlanes(0.01, 10.0);

	// set stereo mode
	camera->setStereoMode(stereoMode);

	// set stereo eye separation and focal length (applies only if stereo is enabled)
	camera->setStereoEyeSeparation(0.03);
	camera->setStereoFocalLength(1.8);

	// set vertical mirrored display mode
	camera->setMirrorVertical(mirroredDisplay);

	// create a light source
	light = new cSpotLight(world);

	// attach light to camera
	world->addChild(light);

	// enable light source
	light->setEnabled(true);

	// position the light source
	light->setLocalPos(0.5, 0.5, 2.5);

	// define the direction of the light beam
	light->setDir(0.0, 0.0, 0.0);

	// enable this light source to generate shadows
	light->setShadowMapEnabled(true);

	// set the resolution of the shadow map
	//light->m_shadowMap->setQualityLow();
	light->m_shadowMap->setQualityMedium();

	// set light cone half angle
	light->setCutOffAngleDeg(45);


	//--------------------------------------------------------------------------
	// CREATE OBJECTS
	//--------------------------------------------------------------------------

	/////////////////////////////////////////////////////////////////////////
	// BASE
	/////////////////////////////////////////////////////////////////////////
	
	//eventualy these points will be read in and deciphered from the NatNet
	double ScaleFloor = 2.0;
	cVector3d TL = cVector3d(-ScaleFloor, ScaleFloor, 0.0);
	cVector3d BR = cVector3d(ScaleFloor, -ScaleFloor, 0.0);
	cVector3d Origin = cVector3d(1.0, 1.0, -0.15);
	//create a box based off points passed in from the corners of the table

	// create a mesh
	cMesh* base = new cMesh();

	// add object to world
	world->addChild(base);


	PlaceAndCenterFloor(base, TL, BR, Origin);

	base->m_material->setStiffness(0.5 * maxStiffness);
	
	//--------------------------------------------------------------------------
	// WIDGETS
	//--------------------------------------------------------------------------

	// create a font
	cFont *font = NEW_CFONTCALIBRI20();

	// create a label to display the haptic rate of the simulation
	labelHud = new cLabel(font);
	labelHud->m_fontColor.setBlack();
	camera->m_frontLayer->addChild(labelHud);

	//number of markers displayed to the user
	labelNumMarkers = new cLabel(font);
	labelNumMarkers->m_fontColor.setBlack();
	camera->m_frontLayer->addChild(labelNumMarkers);

	// create a background
	cBackground* background = new cBackground();
	camera->m_backLayer->addChild(background);

	// set background properties
	background->setCornerColors(cColorf(1.0f, 1.0f, 1.0f),
		cColorf(1.0f, 1.0f, 1.0f),
		cColorf(0.8f, 0.8f, 0.8f),
		cColorf(0.8f, 0.8f, 0.8f));


	//--------------------------------------------------------------------------
	// INITIALIZE -- Frame
	//--------------------------------------------------------------------------
	SendDataTimer = new cPrecisionClock();
	SendDataTimer->setTimeoutPeriodSeconds(0.001);

	AllMarkers = new ZWorldMarkers();

	InitMarkers();


	
	{
		cVector3d Dim1 = cVector3d(0.3, 0.3, 0.1);
		cVector3d Loc1 = cVector3d(0.1, 0.4, 0.0);
		std::string Amplitude = "5";
		ObjectColor color = ObjectColor::red;
		AllObjects.MakeBox(Dim1, Loc1, color, Amplitude);

		Dim1 = cVector3d(0.3, 0.3, 0.1);
		Loc1 = cVector3d(0.4, 0.4, 0.0);
		Amplitude = "3";
		color = ObjectColor::yellow;
		AllObjects.MakeBox(Dim1, Loc1, color, Amplitude);

		Dim1 = cVector3d(0.3, 0.3, 0.1);
		Loc1 = cVector3d(0.7, 0.4, 0.0);
		Amplitude = "1";
		color = ObjectColor::green;
		AllObjects.MakeBox(Dim1, Loc1, color, Amplitude);

		Dim1 = cVector3d(0.3, 0.3, 0.1);
		Loc1 = cVector3d(0.4, 0.1, 0.0);
		Amplitude = "1";
		color = ObjectColor::purple;
		AllObjects.MakeBox(Dim1, Loc1, color, Amplitude);

	}

	//add all objects to the world
	for (auto& curr : AllObjects.GetAllObjects() )
	{
		world->addChild(curr.Mesh);
	}


	//--------------------------------------------------------------------------
	// START SIMULATION
	//--------------------------------------------------------------------------
	//start timer
	SendDataTimer->start();

	// create a thread which starts the main haptics rendering loop
	cThread* hapticsThread = new cThread();
	hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

	// setup callback when application exits
	atexit(close);

	// start the main graphics rendering loop
	glutTimerFunc(50, graphicsTimer, 0);
	glutMainLoop();

	// exit
	return (0);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void resizeWindow(int w, int h)
{
	windowW = w;
	windowH = h;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void keySelect(unsigned char key, int x, int y)
{
	// option ESC: exit
	if ((key == 27) || (key == 'x'))
	{
		// exit application
		exit(0);
	}

	// option f: toggle fullscreen
	if (key == 'f')
	{
		if (fullscreen)
		{
			windowPosX = glutGet(GLUT_INIT_WINDOW_X);
			windowPosY = glutGet(GLUT_INIT_WINDOW_Y);
			windowW = glutGet(GLUT_INIT_WINDOW_WIDTH);
			windowH = glutGet(GLUT_INIT_WINDOW_HEIGHT);
			glutPositionWindow(windowPosX, windowPosY);
			glutReshapeWindow(windowW, windowH);
			fullscreen = false;
		}
		else
		{
			glutFullScreen();
			fullscreen = true;
		}
	}

	// option s: save screeshot to file
	if (key == 'r')
	{
		cImagePtr image = cImage::create();
		light->m_shadowMap->copyDepthBuffer(image);
		image->saveToFile("shadowmapshot.png");
		cout << "> Saved screenshot of shadowmap to file.       \r";
	}

	// option m: toggle vertical mirroring
	if (key == 'm')
	{
		mirroredDisplay = !mirroredDisplay;
		camera->setMirrorVertical(mirroredDisplay);
	}

	//translate the sphere around
	static const float adjustSpacing = 0.05;

	//up
	if (key == 'w')
	{
		//sphere->translate(0.0, adjustSpacing, 0.0);
		sphere->setLocalPos(0.0, 0.3, 0.0);
	}

	//left
	if (key == 'a')
	{
		//sphere->translate(-adjustSpacing, 0.0, 0.0);
		sphere->setLocalPos(0.1, 0.0, 0.0);
	}

	//right
	if (key == 'd')
	{
		//sphere->translate(adjustSpacing, 0.0, 0.0);
		sphere->setLocalPos(0.3, 0.0, 0.0);
	}

	//down
	if (key == 's')
	{
		//sphere->translate(0.0, -adjustSpacing, 0.0);
		sphere->setLocalPos(0.0, 0.1, 0.0);
	}

	//right
	if (key == 'q')
	{
		sphere->translate(0.0, 0.0, 1 * adjustSpacing);
		std::cout << "glob: " << sphere->getGlobalPos() << std::endl;
		std::cout << "local: " << sphere->getLocalPos() << std::endl;
		std::cout << std::endl;
	}

	//down
	if (key == 'e')
	{
		sphere->translate(0.0, 0.0, -1 * adjustSpacing);
	}

	//change lower freq up by 5
	if (key == 'o')
	{
		BiQuadFilt.AdjustFilterType(1);
	}

	//change lower freq down by 5
	if (key == 'k')
	{
		BiQuadFilt.AdjustFilterType(-1);
	}

	//change lower freq up by 5
	if (key == 'p')
	{
		//WifiHook.send("5");
		//addSphere();
		//std::cout << "glob: " << sphere->getGlobalPos() << std::endl;
		//std::cout << "local: " << sphere->getLocalPos() << std::endl;
		BiQuadFilt.AdjustCutoffFreq(5);
	}

	//change upper freq down by 5
	if (key == 'l')
	{
		//WifiHook.send("0");
		//removeSphere();
		//ReadValsWTF();
		BiQuadFilt.AdjustCutoffFreq(-5);
	}

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void close(void)
{
	// stop the simulation
	simulationRunning = false;

	// wait for graphics and haptics loops to terminate
	while (!simulationFinished) { cSleepMs(100); }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void graphicsTimer(int data)
{
	if (simulationRunning)
	{
		glutPostRedisplay();
	}

	glutTimerFunc(50, graphicsTimer, 0);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void updateGraphics(void)
{
	/////////////////////////////////////////////////////////////////////
	// UPDATE WIDGETS
	/////////////////////////////////////////////////////////////////////

	// update haptic rate data

	HudString = cStr(frequencyCounter.getFrequency(), 0) + "Hz "
		+ "BiQuad Filter Type " + BiQuadFilt.GetBiQuadFilterType() +
		+", cutoff freq " + std::to_string(BiQuadFilt.GetBiQuadCutoffFreq()) + "Hz ";



	labelHud->setText(HudString);

	// update position of label
	labelHud->setLocalPos(
		(int)(0.5 * (windowW - labelHud->getWidth())), 15);
	
	NumberOfMarkers.str("");
	NumberOfMarkers << AllMarkers->NumberOfWorldMarkers();
	labelNumMarkers->setText(NumberOfMarkers.str());

	labelNumMarkers->setLocalPos(
		(int)(0.1 * (windowW - labelHud->getWidth())), 15);

	/////////////////////////////////////////////////////////////////////
	// USER DEFINED OBJECTS
	/////////////////////////////////////////////////////////////////////
	{
		std::lock_guard<std::mutex> guard(mObjectOnMapChange);
		CheckMarkers();
	}
	/////////////////////////////////////////////////////////////////////
	// RENDER SCENE
	/////////////////////////////////////////////////////////////////////

	// update shadow maps (if any)
	//world->updateShadowMaps(false, mirroredDisplay);

	// render world

	try
	{
		camera->renderView(windowW, windowH);
	}
	catch (const std::exception& e)
	{
		std::cout << "camera->renderView() failed" << std::endl;
	}

	// swap buffers actually updates the visuals of the object
	glutSwapBuffers();

	// wait until all GL commands are completed
	glFinish();

	// check for any OpenGL errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) cout << "Error: " << gluErrorString(err) << endl;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

enum cMode
{
	IDLE,
	SELECTION
};

void updateHaptics(void)
{
	cMode state = IDLE;
	cGenericObject* object = NULL;

	// simulation in now running
	simulationRunning = true;
	simulationFinished = false;

	// main haptic simulation loop
	while (simulationRunning)
	{
		/////////////////////////////////////////////////////////////////////////
		// HAPTIC RENDERING
		/////////////////////////////////////////////////////////////////////////

		// update frequency counter
		frequencyCounter.signal(1);

		/////////////////////////////////////////////////////////////////////////
		// USER DEFINED MANIPULATION
		/////////////////////////////////////////////////////////////////////////
		
		bool coll;
		{
			std::lock_guard<std::mutex> guard(mObjectOnMapChange);
			coll = AllObjects.CollisionDetection(AllMarkers->GrabLastElement());
			if (BiQuadFilt.CheckForBiQuadFilterChange())
			{
				//think up a good way to fix and get rid of this unneccessary shit
				// ??? !!!
				BiQuadFilterVars tmp = BiQuadFilt.SolveForCoefficient();
				#ifdef WIFI_TESTING
				WifiHook.SendPacket(
					AllObjects.GetAmplitudeOfCollidedObject(),
					BiQuadFilt.GetFilterParameters());
				#endif
			}
		}
		if (SendDataTimer->timeoutOccurred())
		{
			if (coll == true)
			{
				#ifdef WIFI_TESTING
				//WifiHook.send( AllObjects.GetAmplitudeOfCollidedObject() );
				WifiHook.SendPacket(
					AllObjects.GetAmplitudeOfCollidedObject(),
					BiQuadFilt.GetFilterParameters());
				#endif
			}
			else
			{
				#ifdef WIFI_TESTING
				WifiHook.SendPacket(
					"0",
					BiQuadFilt.GetFilterParameters());
				#endif
			}
			SendDataTimer->start();
		}
	}

	// exit haptics thread
	simulationFinished = true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PlaceAndCenterFloor(cMesh* base,
	cVector3d& TopLeft,
	cVector3d& BottomRight,
	cVector3d& Origin)
{
	double width = (BottomRight.x() - Origin.x()) * 2  ;
	double height = (TopLeft.y() - Origin.y()) * 2;

	//build mesh using a box primitive
	cCreateBox(base,
		width,
		height,
		0.01,
		cVector3d(Origin.x(), Origin.y(), Origin.z()),
		cMatrix3d(cDegToRad(0), cDegToRad(0), cDegToRad(0), C_EULER_ORDER_XYZ));

	// set material properties
	base->m_material->setBrownBlanchedAlmond();
}


void InitMarkers()
{

	MyHand.ConfigureInitialMarkers();

	OldPos = MyHand.GetPositionOfMarkers();

	AllMarkers->InitAllMotionMarkers(OldPos);

	//add all markers to the world
	for (auto curr : AllMarkers->GetReferenceMarkers())
	{
		world->addChild(curr.Marker);
	}
}

void CheckMarkers()
{
	std::vector<Markers> NewPos = MyHand.UpdatePositionOfMarkers();
	bool MarkersMoved = AllMarkers->CheckForMovedMarkerPosition(NewPos); 

	//same number of markers did they move??
	if (MyHand.DifferentNumberOfMarkers() == false &&
		AllMarkers->NumberOfWorldMarkers() == NewPos.size())	
	{	
		//update markers positions
		if (MarkersMoved)
		{
			UpdateMarkerLocations();
		}
	}
	//different number of markers, lost or gained a marker
	else 
	{
		//add new markers to the world
		for (auto& Ref : AllMarkers->GetReferenceMarkers())
		{
			if (Ref.AddToWorld == true)
			{
				world->addChild(Ref.Marker);
				Ref.AddToWorld = false;
			}
		}

		//remove old dead markers from the world
		if (NewPos.size() != AllMarkers->NumberOfWorldMarkers())
		{
			int index = -1;
			for (auto& Ref : AllMarkers->GetReferenceMarkers())
			{
				index++;
				bool found = false;
				for (auto& New : NewPos)
				{
					if (New.MarkerNumber == Ref.MarkerNumber) found = true;
				}
			//if not found remove
				if (found != true)
				{
					world->removeChild(Ref.Marker);
					//erase that element from array
					AllMarkers->EraseMarker(index);
					
				}
			}
		}
	//reset the markers difference
		MyHand.DiffNumMarkers(false);
	}

	OldPos = NewPos;
}

void UpdateMarkerLocations()
{
	for (auto& curr : AllMarkers->GetReferenceMarkers())
	{
		if (curr.ReadyForUpdate == true)
		{
			//std::cout <<"before "<< curr.Marker->getLocalPos() << std::endl;
			//cVector3d OriginOffset = AllMarkers->GetOrigin();
			/*curr.Marker->setLocalPos(
				curr.X - OriginOffset.x(),
				curr.Z - OriginOffset.z(),
				curr.Y - OriginOffset.y());
			*/
			SwapMarker(curr);
			//curr.Marker->translate(-curr.XDist, -curr.ZDist, -curr.YDist);
			//std::cout << "after " << curr.Marker->getLocalPos() << std::endl;
			curr.ReadyForUpdate = false;
		}
	}
}

void SwapMarker(WorldMarker& Mark)
{
	world->removeChild(Mark.Marker);
	AllMarkers->SwapMarker(Mark);
	world->addChild(Mark.Marker);
}


/////////////////////////////////////////////////////////////////////////
// Debug Functions
/////////////////////////////////////////////////////////////////////////
void ReadAllLocalPositions()
{
	for (auto& curr : AllMarkers->GetReferenceMarkers())
	{
		std::cout << "local: " << curr.MarkerNumber << " " << curr.Marker->getLocalPos() << std::endl;
		std::cout << "global: " << curr.MarkerNumber << " " << curr.Marker->getGlobalPos() << std::endl;
	}



}

void addSphere()
{
	if (sphere == nullptr)
	{
		sphere = new cMesh();
	}

	// add object to world
	world->addChild(sphere);


	std::vector<WorldMarker> test = AllMarkers->GetReferenceMarkers();
	WorldMarker test1 = test.back();
	double objX = test1.X;
	double objY = test1.Y;
	double objZ = test1.Z;

	double X = objX;
	double Y = objY;
	double Z = objZ;



	// build mesh using a cylinder primitive
	cCreateSphere(sphere,
		0.07,
		36,
		36,
		cVector3d(X, Z, Y),
		cMatrix3d(cDegToRad(0), cDegToRad(0), cDegToRad(0), C_EULER_ORDER_XYZ));

	

	std::cout << X << " "<< Z << std::endl;
	
	sphere->setLocalPos(X, Z, Y);
	// set material properties
	sphere->m_material->setPurpleBlueViolet();
	sphere->m_material->setStiffness(0.5 * maxStiffness);
}

void ReadValsWTF()
{
	ReadAllLocalPositions();
	
	std::vector<WorldMarker> test = AllMarkers->GetReferenceMarkers();
	WorldMarker test1 = test.back();
	double objX = test1.X;
	double objY = test1.Y;
	double objZ = test1.Z;
	
	std::cout << "purple Local " << sphere->getLocalPos() << std::endl;
	std::cout << "purple Global " << sphere->getGlobalPos() << std::endl;

	std::cout <<"marker local " << test1.Marker->getLocalPos() << std::endl;
	std::cout << "marker global " << test1.Marker->getGlobalPos() << std::endl;
}


void removeSphere()
{
	world->removeChild(sphere);
	delete sphere;
	sphere = nullptr;
}
