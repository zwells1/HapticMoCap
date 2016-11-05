#pragma once
//-----------------------------------------------------------------------------
#ifndef INCLUDE_Wifi
#define INCLUDE_Wifi


#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>

class ZWifi
{
	//-------------------------------------------------------------------------
	// PUBLIC METHODS:
	//-------------------------------------------------------------------------
public:
	//constructor
	ZWifi(boost::asio::io_service& io_service,
		const std::string& host,
		const std::string& port);

	//destructor
	~ZWifi();

	//-------------------------------------------------------------------------
	// PUBLIC METHODS:
	//-------------------------------------------------------------------------
public:
	
	void send(const std::string& msg);

	//-------------------------------------------------------------------------
	// PRIVATE MEMBERS:
	//-------------------------------------------------------------------------
private:
	boost::asio::io_service& io_service_;
	boost::asio::ip::udp::socket socket_;
	boost::asio::ip::udp::endpoint endpoint_;

};
#endif