/*
 * receiver3v.cpp
 *
 *  Created on: Apr 7, 2016
 *      Author: jcalvopi
 */


#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

//using boost::asio::ip::udp;

using namespace boost;
using asio::ip::udp;
using system::error_code;

class mi_udp_server
{
public:


	mi_udp_server(boost::asio::io_service& io)
		:   mi_socket_server(io, udp::endpoint(udp::v4(), 22222)),
		    datas(0)
	{
	 std::cout << "receiver3 waiting" << std::endl;
	 receiving();

	}
	~mi_udp_server()
	{
	 std::cout << "destructor called" << std::endl;
	}
private:

	udp::socket mi_socket_server;
	udp::endpoint remote_endpoint;
	uint32_t datas;
	boost::array<char, 20> recv_buffer;

void receiving()
{

	mi_socket_server.async_receive_from(
	        boost::asio::buffer(recv_buffer), remote_endpoint,
	        boost::bind(&mi_udp_server::handler, this,
	        boost::asio::placeholders::error,
	        boost::asio::placeholders::bytes_transferred));

}

void handler(
  const boost::system::error_code& error, // Result of operation.
  std::size_t bytes_transferred1)          // Number of bytes received.
   {

	std::cout << "data piece number: " << datas << std::endl;
	datas += 1;
	receiving();
  }

};

int main()
{
	try
	{
		  boost::asio::io_service io;
		  mi_udp_server p(io);
		  //for one thread
		  boost::thread t(boost::bind(&boost::asio::io_service::run, &io));
		  io.run();
		  t.join();

	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}



