#pragma once
/*	File Name: MoCapLink.hpp
Author : Zachary Wells
Date : 10 / 17 / 2016
Tested With : Chai version 3.1.1
*/
#ifndef INCLUDE_MoCapLink
#define INCLUDE_MoCapLink

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <stdio.h>
#include <iostream>
#include <memory>

using boost::asio::ip::udp;

class ZMoCapLink
{


	//-------------------------------------------------------------------------
	// PUBLIC METHODS:
	//-------------------------------------------------------------------------
public:
	
	//constructor
	ZMoCapLink();

	//destructor
	~ZMoCapLink();


	std::string grabData();

	std::string ZMoCapLink::CreateMotionCaptureLink();

	//-------------------------------------------------------------------------
	// PUBLIC MEMBERS:
	//--------------------------------------------------------------------------
public:
	boost::asio::io_service Io_Serv;
	
	//std::unique_ptr<udp::endpoint> pReceiver_Endpoint;
	
	udp::endpoint* p1Receiver_Endpoint;

// ??? !!! change to a string
	char SocketConnect[128];


};
#endif