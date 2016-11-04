#pragma once
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <ctime>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <string>
#include <vector>

//mutex for when the objects change
std::mutex mMarkerLock;

using boost::asio::ip::udp;

//eventually make this into a seperate class Marker.cpp etc.
struct Markers
{
	char MarkerNumber;
	float XPos;
	float YPos;
	float ZPos;
};


//GLOBALS that will eventually get removed by a class header
std::vector<sMarker> AllMarkers;

unsigned int CurrentNumofMarkers = 0;



void PopulateMarkers(sMarker* MoCapMarkers, int& NumOfMarkers)
{
	AllMarkers.clear();
	

	for (int i = 0; i < NumOfMarkers; i++)
	{
	
		// !!! ??? debating about reducing size of struct and making my own as
		//opposed to grabbing the whole sMarker obj
		/*
		Markers Current;
		Current.MarkerNumber = i;
		Current.XPos = MoCapMarkers->Markers[i][0];
		Current.YPos = MoCapMarkers->Markers[i][1];
		Current.ZPos = MoCapMarkers->Markers[i][2];
		*/
		sMarker marker = MoCapMarkers[i];
		AllMarkers.push_back(marker);
	}
}

std::string SendStringFormat(float num2Convert)
{
	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << num2Convert;
	std::string target = ss.str();
	
	return target;
}

std::string SendStringFormat(int num2Convert)
{
	std::stringstream ss;
	ss << num2Convert;
	std::string target = ss.str();

	return target;
}

std::string SendStringFormat(unsigned int num2Convert)
{
	std::stringstream ss;
	ss << num2Convert;
	std::string target = ss.str();

	return target;
}

//fix this whole thing later make it more intelligent so it is
//not constantly updating the data if it doesnt have to
void UpdateMarkerPositions(sMarker* MoCapMarkers, int& NumOfMarkers)
{
	{
		std::lock_guard<std::mutex> guard(mMarkerLock);
		//first time
		//if (AllMarkers.size() == 0)
		//{
		PopulateMarkers(MoCapMarkers, NumOfMarkers);
		//}

		//check number of data points if same just update
		//else if

		//if different flush vector and create a new one
		//since I dont think we can tell which are the new ones.
		CurrentNumofMarkers = AllMarkers.size();
	}
}



std::string make_DataMarker_string()
{
	using namespace std; // For time_t, time and ctime;

	std::string MarkerString = "";

	
	//check to see if the next step is even necessary for instance
	//if it is the same data no point int changing the MarkerString around
	//fix later!!
	
	//determine the number of markers that will be passed

	std::string numMarks= SendStringFormat(CurrentNumofMarkers);
	MarkerString += numMarks + ",";

	
	{
		std::lock_guard<std::mutex> guard(mMarkerLock);
	
		for (int i = 0; i < CurrentNumofMarkers; i++)
		{
			sMarker curr = AllMarkers[i];
			std::string markerNum = SendStringFormat(curr.ID);
			//note flipping x bc optitrack gives a reversed x axis
			// see if there is a way to fix !!! ???
			std::string marker_X = SendStringFormat(-curr.x);
			std::string marker_Y = SendStringFormat(curr.y);
			std::string marker_Z = SendStringFormat(curr.z);

			MarkerString += markerNum + "," +
				marker_X + "," +
				marker_Y + "," +
				marker_Z + ";";
		}
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

int startConnection()
{
	try
	{
		boost::asio::io_service io_service;
		udp_server server(io_service);
		std::cout << "boost connect started" << std::endl;
		io_service.run();

	}
	catch (std::exception& e)
	{
		std::cout << "BoostConnect.cpp :: issue starting boost server" << std::endl;
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
