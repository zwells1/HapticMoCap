/*	File Name: Marker.cpp
Author : Zachary Wells
Date : 10 / 17 / 2016
Tested With : Chai version 3.1.1
*/

#include "WifiConnect.hpp"


using boost::asio::ip::udp;


ZWifi::ZWifi(
	boost::asio::io_service& io_service,
	const std::string& host,
	const std::string& port
) : io_service_(io_service), socket_(io_service, udp::endpoint(udp::v4(), 0)) {
	udp::resolver resolver(io_service_);
	udp::resolver::query query(udp::v4(), host, port);
	udp::resolver::iterator iter = resolver.resolve(query);
	endpoint_ = *iter;
}

ZWifi::~ZWifi()
{
	socket_.close();
}

void ZWifi::Send(const std::string& msg) {
	socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
}



void ZWifi::SendPacket(
	const std::string& AmplitudeOfObject,
	const std::vector<std::string>& FiltersCoefficients)
{
	std::string msg = AmplitudeOfObject;
	
	for (int i = 0; i < FiltersCoefficients.size(); i++)
	{
		msg += ",";
		msg += FiltersCoefficients[i];
	}

	socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
}