/*	File Name: MoCapLink.cpp
Author : Zachary Wells
Date : 10 / 17 / 2016
Tested With : Chai version 3.1.1
*/

#include "MoCapLink.hpp"

#define LOOPBACK
//#define PRINT_PACKETS
//constructor
ZMoCapLink::ZMoCapLink()
{
	char SocketConnect[128] = "127.0.0.1";

	udp::resolver resolver(Io_Serv);

	udp::resolver::query query(udp::v4(), SocketConnect, "666");

	//std::unique_ptr<udp::endpoint>p1Receiver_Endpoint(new udp::endpoint(*resolver.resolve(query)));

	//pReceiver_Endpoint = std::move(p1Receiver_Endpoint);
	p1Receiver_Endpoint  = new udp::endpoint(*resolver.resolve(query));
}

//destructor
ZMoCapLink::~ZMoCapLink()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
std::string ZMoCapLink::grabData()
{
	
	static const int RecBufferSize = 256;

	boost::array<char, 1> send_buf = { 0 };

	udp::socket socket(Io_Serv);

	socket.open(udp::v4());

	socket.send_to(boost::asio::buffer(send_buf), *p1Receiver_Endpoint);

	//if more points would like to be received just make this receive buffer larger
	boost::array<char, RecBufferSize> recv_buf;

	udp::endpoint sender_endpoint;

	size_t len = socket.receive_from(
		boost::asio::buffer(recv_buf), sender_endpoint);

	#ifdef PRINT_PACKETS
	std::cout.write(recv_buf.data(), len);
	std::cout << std::endl;
	#endif
	
	socket.close();

	std::string RecvString(recv_buf.data());
	return RecvString;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
std::string ZMoCapLink::CreateMotionCaptureLink()
{
	try
	{
		std::string Data = grabData();

		return Data;
	}
	catch (std::exception& e)
	{
		std::cout << "turn on the server!" << std::endl;
		std::cerr << e.what() << std::endl;
	}
}