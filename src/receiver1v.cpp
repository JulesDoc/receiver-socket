/*
 * boostmenos.cpp
 *
 *  Created on: Apr 1, 2016
 *      Author: jcalvopi
 */


#include <ctime>
#include <iostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;



//void func(uint16_t &);

int main()
{
	 static uint16_t datas = 0;
  try
  {

	boost::asio::io_service io_service;
    udp::socket socket(io_service, udp::endpoint(udp::v4(), 22222));
    std::cout << "receiver1 waiting" << std::endl;

    for (;;)
    {
      boost::array<char, 1> recv_buf;
      udp::endpoint remote_endpoint;
      boost::system::error_code error;
      socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint, 0, error);
      std::cout << "data piece number: " << datas << std::endl;
      datas+=1;

    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

