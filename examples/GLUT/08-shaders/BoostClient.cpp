#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <stdio.h>
#include <iostream>


using boost::asio::ip::udp;

#define LOOPBACK


void grabData(udp::socket& socket,
	udp::endpoint& receiver_endpoint)
{
	static const int RecBufferSize = 256;

	boost::array<char, 1> send_buf = { 0 };

	socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

    //if more points would like to be received just make this receive buffer larger
	boost::array<char, RecBufferSize> recv_buf;

	udp::endpoint sender_endpoint;

	size_t len = socket.receive_from(
		boost::asio::buffer(recv_buf), sender_endpoint);

	std::cout.write(recv_buf.data(), len);

}


int main(int argc, char* argv[])
{
	try
	{
		if (argc != 2)
		{
			#ifdef LOOPBACK
				char SocketConnect[128] = "127.0.0.1";
				argv[1] = SocketConnect;
			#else
				std::cerr << "Usage: client <host>" << std::endl;
				return 1;
			#endif
		}

		if (argc == 1)
		{
			#ifndef LOOPBACK
				std::cerr << "Usage: client <host>" << std::endl;
				return 1;
			#endif

		}

		boost::asio::io_service io_service;


		udp::resolver resolver(io_service);
		udp::resolver::query query(udp::v4(), argv[1], "666");

		udp::endpoint receiver_endpoint = *resolver.resolve(query);

		udp::socket socket(io_service);

		socket.open(udp::v4());

		grabData(socket, receiver_endpoint);
	}
	catch (std::exception& e)
	{
		std::cout << "turn on the server!" << std::endl;
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
