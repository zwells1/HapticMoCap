/*
NatNetClient.cpp

This program connects to a NatNet server, receives a data stream, and writes that data stream
to an ascii file.  The purpose is to illustrate using the NatNetClient class.

Usage [optional]:

SampleClient [ServerIP] [LocalIP] [OutputFilename]

[ServerIP]			IP address of the server (e.g. 192.168.0.107) ( defaults to local machine)
[OutputFilename]	Name of points file (pts) to write out.  defaults to Client-output.pts

*/

#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <winsock2.h>

#include "NatNetTypes.h"
#include "NatNetClient.h"

#include "BoostConnect.hpp"

#pragma warning( disable : 4996 )
#define REALCODE
//#define DEBUG

void _WriteHeader(FILE* fp, sDataDescriptions* pBodyDefs);
void _WriteFrame(FILE* fp, sFrameOfMocapData* data);
void _WriteFooter(FILE* fp);
void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData);		// receives data from the server
void __cdecl MessageHandler(int msgType, char* msg);		            // receives NatNet error mesages
void resetClient();
int CreateClient(int iConnectionType);
void ZWriteFrame(FILE* fp, sFrameOfMocapData* data);

unsigned int MyServersDataPort = 3130;
unsigned int MyServersCommandPort = 3131;
int iConnectionType = ConnectionType_Multicast;

NatNetClient* theClient;
FILE* fp;

char szMyIPAddress[128] = "";
char szServerIPAddress[128] = "";

int analogSamplesPerMocapFrame = 0;


int _tmain(int argc, _TCHAR* argv[])
{
	int iResult;

	// parse command line args
	if (argc>1)
	{
		strcpy(szServerIPAddress, argv[1]);	// specified on command line
		printf("Connecting to server at %s...\n", szServerIPAddress);
	}
	else
	{
		strcpy(szServerIPAddress, "");		// not specified - assume server is local machine
		printf("Connecting to server at LocalMachine\n");
	}
	if (argc>2)
	{
		strcpy(szMyIPAddress, argv[2]);	    // specified on command line
		printf("Connecting from %s...\n", szMyIPAddress);
	}
	else
	{
		strcpy(szMyIPAddress, "");          // not specified - assume server is local machine
		printf("Connecting from LocalMachine...\n");
	}

	// Create NatNet Client
	iResult = CreateClient(iConnectionType);
	if (iResult != ErrorCode_OK)
	{
		printf("Error initializing client.  See log for details.  Exiting");
		return 1;
	}
	else
	{
		printf("Client initialized and ready.\n");
	}


	// send/receive test request
	printf("[SampleClient] Sending Test Request\n");
	void* response;
	int nBytes;
	iResult = theClient->SendMessageAndWait("TestRequest", &response, &nBytes);
	if (iResult == ErrorCode_OK)
	{
		printf("[SampleClient] Received: %s", (char*)response);
	}

	// Retrieve Data Descriptions from server
	printf("\n\n[SampleClient] Requesting Data Descriptions...");
	sDataDescriptions* pDataDefs = NULL;
	int nBodies = theClient->GetDataDescriptions(&pDataDefs);
	if (!pDataDefs)
	{
		printf("[SampleClient] Unable to retrieve Data Descriptions.");
	}
	else
	{
		printf("[SampleClient] Received %d Data Descriptions:\n", pDataDefs->nDataDescriptions);
		for (int i = 0; i < pDataDefs->nDataDescriptions; i++)
		{
			printf("Data Description # %d (type=%d)\n", i, pDataDefs->arrDataDescriptions[i].type);
			if (pDataDefs->arrDataDescriptions[i].type == Descriptor_MarkerSet)
			{
				// MarkerSet
				sMarkerSetDescription* pMS = pDataDefs->arrDataDescriptions[i].Data.MarkerSetDescription;
				printf("MarkerSet Name : %s\n", pMS->szName);
				for (int i = 0; i < pMS->nMarkers; i++)
					printf("%s\n", pMS->szMarkerNames[i]);

			}

			else
			{
				if (pDataDefs->arrDataDescriptions[i].type == Descriptor_RigidBody)
				{
					printf("Rigidbody Data Plate Data not accounted for in this program\n");
				}

				else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_Skeleton)
				{
					printf("Skeletons Data Plate Data not accounted for in this program\n");
				}

				else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_ForcePlate)
				{
					printf("Force Plate Data not accounted for in this program\n");
				}

				else
				{
					printf("Unknown data type.");
				}
			}
		}
	}

	// Create data file for writing received stream into
	char szFile[MAX_PATH];
	char szFolder[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szFolder);
	if (argc > 3)
		sprintf(szFile, "%s\\%s", szFolder, argv[3]);
	else
		sprintf(szFile, "%s\\Client-output.pts", szFolder);
	fp = fopen(szFile, "w");
	if (!fp)
	{
		printf("error opening output file %s.  Exiting.", szFile);
		exit(1);
	}
	if (pDataDefs)
		_WriteHeader(fp, pDataDefs);

	// Ready to receive marker stream!
	printf("\nClient is connected to server and listening for data...\n");
	startConnection();

	printf("Server is now ready to start streaming");
	int c;
	bool bExit = false;
	while (c = _getch())
	{
		printf("%d", c);
		switch (c)
		{
		case 'q':
			bExit = true;
			break;
		case 'r':
			resetClient();
			break;
		case 'p':
			sServerDescription ServerDescription;
			memset(&ServerDescription, 0, sizeof(ServerDescription));
			theClient->GetServerDescription(&ServerDescription);
			if (!ServerDescription.HostPresent)
			{
				printf("Unable to connect to server. Host not present. Exiting.");
				return 1;
			}
			break;
		case 'f':
		{
			sFrameOfMocapData* pData = theClient->GetLastFrameOfData();
			printf("Most Recent Frame: %d", pData->iFrame);
		}
		break;
		case 'm':	                        // change to multicast
			iConnectionType = ConnectionType_Multicast;
			iResult = CreateClient(iConnectionType);
			if (iResult == ErrorCode_OK)
				printf("Client connection type changed to Multicast.\n\n");
			else
				printf("Error changing client connection type to Multicast.\n\n");
			break;
		case 'u':	                        // change to unicast
			iConnectionType = ConnectionType_Unicast;
			iResult = CreateClient(iConnectionType);
			if (iResult == ErrorCode_OK)
				printf("Client connection type changed to Unicast.\n\n");
			else
				printf("Error changing client connection type to Unicast.\n\n");
			break;
		case 'c':                          // connect
			iResult = CreateClient(iConnectionType);
			break;
		case 'd':                          // disconnect
										   // note: applies to unicast connections only - indicates to Motive to stop sending packets to that client endpoint
			iResult = theClient->SendMessageAndWait("Disconnect", &response, &nBytes);
			if (iResult == ErrorCode_OK)
				printf("[SampleClient] Disconnected");
			break;
		default:
			break;
		}
		if (bExit)
			break;
	}

	// Done - clean up.
	theClient->Uninitialize();
	_WriteFooter(fp);
	fclose(fp);

	return ErrorCode_OK;
}

// Establish a NatNet Client connection
int CreateClient(int iConnectionType)
{
	// release previous server
	if (theClient)
	{
		theClient->Uninitialize();
		delete theClient;
	}

	// create NatNet client
	theClient = new NatNetClient(iConnectionType);



	// set the callback handlers
	theClient->SetVerbosityLevel(Verbosity_Warning);
	theClient->SetMessageCallback(MessageHandler);
	theClient->SetDataCallback(DataHandler, theClient);	// this function will receive data from the server
														// [optional] use old multicast group
														//theClient->SetMulticastAddress("224.0.0.1");

														// print version info
	unsigned char ver[4];
	theClient->NatNetVersion(ver);
	printf("NatNet Sample Client (NatNet ver. %d.%d.%d.%d)\n", ver[0], ver[1], ver[2], ver[3]);

	// Init Client and connect to NatNet server
	// to use NatNet default port assignments
	char ZIpAddress[128] = "192.168.0.100";
	int retCode = theClient->Initialize(ZIpAddress, szServerIPAddress);
	// to use a different port for commands and/or data:
	//int retCode = theClient->Initialize(szMyIPAddress, szServerIPAddress, MyServersCommandPort, MyServersDataPort);
	if (retCode != ErrorCode_OK)
	{
		printf("Unable to connect to server.  Error code: %d. Exiting", retCode);
		return ErrorCode_Internal;
	}
	else
	{
		// get # of analog samples per mocap frame of data
		void* pResult;
		int ret = 0;
		int nBytes = 0;
		ret = theClient->SendMessageAndWait("AnalogSamplesPerMocapFrame", &pResult, &nBytes);
		if (ret == ErrorCode_OK)
		{
			analogSamplesPerMocapFrame = *((int*)pResult);
			printf("Analog Samples Per Mocap Frame : %d", analogSamplesPerMocapFrame);
		}

		// print server info
		sServerDescription ServerDescription;
		memset(&ServerDescription, 0, sizeof(ServerDescription));
		theClient->GetServerDescription(&ServerDescription);
		if (!ServerDescription.HostPresent)
		{
			printf("Unable to connect to server. Host not present. Exiting.");
			return 1;
		}
		printf("[SampleClient] Server application info:\n");
		printf("Application: %s (ver. %d.%d.%d.%d)\n", ServerDescription.szHostApp, ServerDescription.HostAppVersion[0],
			ServerDescription.HostAppVersion[1], ServerDescription.HostAppVersion[2], ServerDescription.HostAppVersion[3]);
		printf("NatNet Version: %d.%d.%d.%d\n", ServerDescription.NatNetVersion[0], ServerDescription.NatNetVersion[1],
			ServerDescription.NatNetVersion[2], ServerDescription.NatNetVersion[3]);
		printf("Client IP:%s\n", szMyIPAddress);
		printf("Server IP:%s\n", szServerIPAddress);
		printf("Server Name:%s\n\n", ServerDescription.szHostComputerName);
	}

	return ErrorCode_OK;
}

// DataHandler receives data from the server
void __cdecl DataHandler(sFrameOfMocapData* data, void* pUserData)
{
	NatNetClient* pClient = (NatNetClient*)pUserData;


#ifdef REALCODE
	if (fp) ZWriteFrame(fp, data);
#else
	if (fp)
		_WriteFrame(fp, data);

	int i = 0;

	printf("FrameID : %d\n", data->iFrame);
	printf("Timestamp :  %3.2lf\n", data->fTimestamp);
	printf("Latency :  %3.2lf\n", data->fLatency);

	// FrameOfMocapData params
	bool bIsRecording = ((data->params & 0x01) != 0);
	bool bTrackedModelsChanged = ((data->params & 0x02) != 0);
	if (bIsRecording)
		printf("RECORDING\n");
	if (bTrackedModelsChanged)
		printf("Models Changed.\n");


	// timecode - for systems with an eSync and SMPTE timecode generator - decode to values
	int hour, minute, second, frame, subframe;
	bool bValid = pClient->DecodeTimecode(data->Timecode, data->TimecodeSubframe, &hour, &minute, &second, &frame, &subframe);
	// decode to friendly string
	char szTimecode[128] = "";
	pClient->TimecodeStringify(data->Timecode, data->TimecodeSubframe, szTimecode, 128);
	printf("Timecode : %s\n", szTimecode);

	sDataDescriptions* pDataDefs = NULL;
	int nBodies = theClient->GetDataDescriptions(&pDataDefs);
	if (!pDataDefs)
	{
		printf("[SampleClient] Unable to retrieve Data Descriptions.");
	}
	else
	{
		printf("[SampleClient] Received %d Data Descriptions:\n", pDataDefs->nDataDescriptions);
		for (int i = 0; i < pDataDefs->nDataDescriptions; i++)
		{
			printf("Data Description # %d (type=%d)\n", i, pDataDefs->arrDataDescriptions[i].type);
			if (pDataDefs->arrDataDescriptions[i].type == Descriptor_MarkerSet)
			{
				// MarkerSet
				sMarkerSetDescription* pMS = pDataDefs->arrDataDescriptions[i].Data.MarkerSetDescription;
				printf("MarkerSet Name : %s\n", pMS->szName);
				for (int i = 0; i < pMS->nMarkers; i++)
					printf("%s\n", pMS->szMarkerNames[i]);

			}
			else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_RigidBody)
			{
				// RigidBody
				sRigidBodyDescription* pRB = pDataDefs->arrDataDescriptions[i].Data.RigidBodyDescription;
				printf("RigidBody Name : %s\n", pRB->szName);
				printf("RigidBody ID : %d\n", pRB->ID);
				printf("RigidBody Parent ID : %d\n", pRB->parentID);
				printf("Parent Offset : %3.2f,%3.2f,%3.2f\n", pRB->offsetx, pRB->offsety, pRB->offsetz);
			}
			else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_Skeleton)
			{
				// Skeleton
				sSkeletonDescription* pSK = pDataDefs->arrDataDescriptions[i].Data.SkeletonDescription;
				printf("Skeleton Name : %s\n", pSK->szName);
				printf("Skeleton ID : %d\n", pSK->skeletonID);
				printf("RigidBody (Bone) Count : %d\n", pSK->nRigidBodies);
				for (int j = 0; j < pSK->nRigidBodies; j++)
				{
					sRigidBodyDescription* pRB = &pSK->RigidBodies[j];
					printf("  RigidBody Name : %s\n", pRB->szName);
					printf("  RigidBody ID : %d\n", pRB->ID);
					printf("  RigidBody Parent ID : %d\n", pRB->parentID);
					printf("  Parent Offset : %3.2f,%3.2f,%3.2f\n", pRB->offsetx, pRB->offsety, pRB->offsetz);
				}
			}
			else if (pDataDefs->arrDataDescriptions[i].type == Descriptor_ForcePlate)
			{
				// Force Plate
				sForcePlateDescription* pFP = pDataDefs->arrDataDescriptions[i].Data.ForcePlateDescription;
				printf("Force Plate ID : %d\n", pFP->ID);
				printf("Force Plate Serial : %s\n", pFP->strSerialNo);
				printf("Force Plate Width : %3.2f\n", pFP->fWidth);
				printf("Force Plate Length : %3.2f\n", pFP->fLength);
				printf("Force Plate Electrical Center Offset (%3.3f, %3.3f, %3.3f)\n", pFP->fOriginX, pFP->fOriginY, pFP->fOriginZ);
				for (int iCorner = 0; iCorner<4; iCorner++)
					printf("Force Plate Corner %d : (%3.4f, %3.4f, %3.4f)\n", iCorner, pFP->fCorners[iCorner][0], pFP->fCorners[iCorner][1], pFP->fCorners[iCorner][2]);
				printf("Force Plate Type : %d\n", pFP->iPlateType);
				printf("Force Plate Data Type : %d\n", pFP->iChannelDataType);
				printf("Force Plate Channel Count : %d\n", pFP->nChannels);
				for (int iChannel = 0; iChannel<pFP->nChannels; iChannel++)
					printf("\tChannel %d : %s\n", iChannel, pFP->szChannelNames[iChannel]);
			}
			else
			{
				printf("Unknown data type.");
				// Unknown
			}
		}
	}

#endif 
}

// MessageHandler receives NatNet error/debug messages
void __cdecl MessageHandler(int msgType, char* msg)
{
	printf("\n%s\n", msg);
}

/* File writing routines */
void _WriteHeader(FILE* fp, sDataDescriptions* pBodyDefs)
{
	int i = 0;

	if (!pBodyDefs->arrDataDescriptions[0].type == Descriptor_MarkerSet)
		return;

	sMarkerSetDescription* pMS = pBodyDefs->arrDataDescriptions[0].Data.MarkerSetDescription;

	fprintf(fp, "<MarkerSet>\n\n");
	fprintf(fp, "<Name>\n%s\n</Name>\n\n", pMS->szName);

	fprintf(fp, "<Markers>\n");
	for (i = 0; i < pMS->nMarkers; i++)
	{
		fprintf(fp, "%s\n", pMS->szMarkerNames[i]);
	}
	fprintf(fp, "</Markers>\n\n");

	fprintf(fp, "<Data>\n");
	fprintf(fp, "Frame#\t");
	for (i = 0; i < pMS->nMarkers; i++)
	{
		fprintf(fp, "M%dX\tM%dY\tM%dZ\t", i, i, i);
	}
	fprintf(fp, "\n");
}

void _WriteFrame(FILE* fp, sFrameOfMocapData* data)
{

	printf("write Frame called!!\n");
	fprintf(fp, "%d", data->iFrame);
	for (int i = 0; i < data->MocapData->nMarkers; i++)
	{
		fprintf(fp, "\t%.5f\t%.5f\t%.5f", data->MocapData->Markers[i][0], data->MocapData->Markers[i][1], data->MocapData->Markers[i][2]);
	}
	fprintf(fp, "\n");
}

void _WriteFooter(FILE* fp)
{
	fprintf(fp, "</Data>\n\n");
	fprintf(fp, "</MarkerSet>\n");
}

void resetClient()
{
	int iSuccess;

	printf("\n\nre-setting Client\n\n.");

	iSuccess = theClient->Uninitialize();
	if (iSuccess != 0)
		printf("error un-initting Client\n");

	iSuccess = theClient->Initialize(szMyIPAddress, szServerIPAddress);
	if (iSuccess != 0)
		printf("error re-initting Client\n");
}

void ZWriteFrame(FILE* fp, sFrameOfMocapData* data) {

#ifdef DEBUG
	fprintf(fp, "%d", data->iFrame);
#endif	

	for (int i = 0; i < data->nLabeledMarkers; i++)
	{
		#ifdef DEBUG

		sMarker marker = data->LabeledMarkers[i];

		std::cout << "Mocap Labeled Markers" <<
			i << ", " <<
			marker.x << ", " <<
			marker.y << ", " <<
			marker.z << std::endl;

		#endif
	}

	if (data->nLabeledMarkers != 0)
	{
		UpdateMarkerPositions(data->LabeledMarkers, data->nLabeledMarkers);
	}
		
	#ifdef DEBUG
	
	fprintf(fp, "\n");
	// Other Markers
	printf("Markers [Count=%d]\n", data->nOtherMarkers);
	
	#endif
	
	for (int i = 0; i < data->nOtherMarkers; i++)
	{
		
		#ifdef DEBUG
		std::cout << "other Markers" <<
			i << ", " <<
			data->OtherMarkers[i][0] << ", " <<
			data->OtherMarkers[i][1] << ", " <<
			data->OtherMarkers[i][2] << std::endl;
		#endif
	}
}