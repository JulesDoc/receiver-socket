/*
 * nuevoServer.cpp
 *
 *  Created on: Apr 6, 2016
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
			mi_strand(io), //for Threading
		    datas(0)
	{
	 std::cout << "receiver4 waiting" << std::endl;
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
	asio::strand mi_strand; //for theadring
	boost::array<char, 20> recv_buffer;


void receiving()
{

	//With strand for safety threading
    mi_socket_server.async_receive_from(asio::buffer(recv_buffer),
    	  remote_endpoint,
    	  mi_strand.wrap(bind(&mi_udp_server::handler, this,
	                  	  asio::placeholders::error,
						  asio::placeholders::bytes_transferred)));
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

		  //Multithreading init

		  boost::thread_group grupo;
		  for (unsigned i = 0; i < thread::hardware_concurrency(); ++i)
		     grupo.create_thread(boost::bind(&asio::io_service::run, ref(io)));

		  grupo.join_all();


	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}















