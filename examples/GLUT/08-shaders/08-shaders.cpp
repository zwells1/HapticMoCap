#include <C:\Users\Retouchlab07\Documents\boost_1_59_0\boost_1_59_0\boost\array.hpp>
#include <C:\Users\Retouchlab07\Documents\boost_1_59_0\boost_1_59_0\boost\asio.hpp>

#include <C:\Users\Retouchlab07\Documents\boost_1_59_0\boost_1_59_0\boost\date_time\posix_time\posix_time.hpp>
#include<boost/asio.hpp>

#include <stdio.h>
#include <iostream>


using boost::asio::ip::udp;
using namespace boost;

void grabData(udp::socket& socket, udp::endpoint& receiver_endpoint)
{

	boost::array<char, 1> send_buf = { 0 };

	socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

	boost::array<char, 128> recv_buf;

	udp::endpoint sender_endpoint;

	size_t len = socket.receive_from(
		boost::asio::buffer(recv_buf), sender_endpoint);

	std::cout.write(recv_buf.data(), len);
}


int main(int argc, char* argv[])
{

	try
	{
		while (1) {

		}
		
		if (argc != 2)
		{
			std::cerr << "Usage: client <host>" << std::endl;
			return 1;
		}

		boost::asio::io_service io_service;

		boost::asio::deadline_timer t(io_service);


		udp::resolver resolver(io_service);
		udp::resolver::query query(udp::v4(), argv[1], "daytime");
		udp::endpoint receiver_endpoint = *resolver.resolve(query);

		udp::socket socket(io_service);
		socket.open(udp::v4());

		char ch;

		while(1)
		{

			ch = getchar();
			if (ch == 'w') {
				grabData(socket, receiver_endpoint);
			}
		}

	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
