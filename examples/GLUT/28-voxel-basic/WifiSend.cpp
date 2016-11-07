#include <iostream>
#include <sstream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class UDPClient
{
public:
	UDPClient(
		boost::asio::io_service& io_service,
		const std::string& host,
		const std::string& port
	) : io_service_(io_service), socket_(io_service, udp::endpoint(udp::v4(), 0)) {
		udp::resolver resolver(io_service_);
		udp::resolver::query query(udp::v4(), host, port);
		udp::resolver::iterator iter = resolver.resolve(query);
		endpoint_ = *iter;
	}

	~UDPClient()
	{
		socket_.close();
	}

	void send(const std::string& msg) {
		socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
	}

private:
	boost::asio::io_service& io_service_;
	udp::socket socket_;
	udp::endpoint endpoint_;
};

int main()
{
	boost::asio::io_service io_service;
	UDPClient client(io_service, "192.168.0.194", "123");
	/*
	int nint = 5;
	
	int LowFreq = 325;
	int HighFreq = 1000;
	char bytes [5];
	
	bytes[0] = nint & 0x000000ff;
	
	bytes[1] = (LowFreq & 0x0000ff00) >> 8;
	
	bytes[2] = LowFreq & 0x000000ff;
	
	bytes[3] = (HighFreq & 0x0000ff00) >> 8;

	bytes[4] = HighFreq & 0x000000ff;

	std::string x;
	std::stringstream ss;
	ss << bytes;
	ss >> x;
	client.send(x);
	*/
	std::string x;
	std::stringstream ss;
	char tests[50];

	int nint = 1;

	for (int i = 0; i < 50; i++)
	{
		tests[i] = nint & 0x000000ff;
		++nint;
	}
	ss << tests;
	ss >> x;
	client.send(x);


}