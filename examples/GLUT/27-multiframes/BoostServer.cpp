#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>

using boost::asio::ip::udp;

std::string SendStringFormat(float num2Convert)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << num2Convert;
	std::string mystring = ss.str();
	return mystring;
}


std::string make_DataMarker_string()
{
	using namespace std; // For time_t, time and ctime;
	
	const unsigned MaxAmountMarkers = 6;

	std::string MarkerString = "";

	for(int i = 0; i <=MaxAmountMarkers; i++)
	{
		std::string markerNum = std::to_string(i);
		std::string marker_X = SendStringFormat(1.01 + i);
		std::string marker_Y = SendStringFormat(2.01 + i);
		std::string marker_Z = SendStringFormat(3.01 + i);

		MarkerString += markerNum + ", " +
			marker_X + ", " +
			marker_Y + ", " +
			marker_Z + "; ";

	}	
	return MarkerString;
}

class udp_server
{
public:
	udp_server(boost::asio::io_service& io_service)
		: socket_(io_service, udp::endpoint(udp::v4(), 666))
	{
		start_receive();
	}

private:
	void start_receive()
	{
		socket_.async_receive_from(
			boost::asio::buffer(recv_buffer_), remote_endpoint_,
			boost::bind(&udp_server::handle_receive, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

	void handle_receive(const boost::system::error_code& error,
		std::size_t /*bytes_transferred*/)
	{
		if (!error || error == boost::asio::error::message_size)
		{
			boost::shared_ptr<std::string> message(
				new std::string(make_DataMarker_string()));

			socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
				boost::bind(&udp_server::handle_send, this, message,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));

			start_receive();
		}
	}

	void handle_send(boost::shared_ptr<std::string> /*message*/,
		const boost::system::error_code& /*error*/,
		std::size_t /*bytes_transferred*/)
	{
	}

	udp::socket socket_;
	udp::endpoint remote_endpoint_;
	boost::array<char, 1> recv_buffer_;
};

int main()
{
	try
	{
		boost::asio::io_service io_service;
		udp_server server(io_service);
		std::cout << "start boost server" << std::endl;
		io_service.run();

	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
