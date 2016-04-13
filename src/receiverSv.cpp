/*
 * prueba.cpp
 *
 *  Created on: Apr 6, 2016
 *      Author: jcalvopi
 */



#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

using namespace boost;
using asio::ip::udp;
using system::error_code;

std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

class udp_server; // forward declaration

struct udp_session : enable_shared_from_this<udp_session> {

    udp_session(udp_server* server) : server_(server) {}



    udp::endpoint remote_endpoint_;
    array<char, 10> recv_buffer_;
    //std::string message;
    udp_server* server_;

};

class udp_server
{
    typedef shared_ptr<udp_session> shared_session;
  public:
    udp_server(asio::io_service& io_service)
        : socket_(io_service, udp::endpoint(udp::v4(), 22222)),
          strand_(io_service),
		  datas(0)
    {
    	std::cout << "receiverSes waiting" << std::endl;
        receive_session();
    }

  private:
    void receive_session()
    {
        // our session to hold the buffer + endpoint
        auto session = make_shared<udp_session>(this);

        socket_.async_receive_from(
                asio::buffer(session->recv_buffer_),
                session->remote_endpoint_,
                strand_.wrap(
                    bind(&udp_server::handle_receive, this,
                        session, // keep-alive of buffer/endpoint
                        asio::placeholders::error,
                        asio::placeholders::bytes_transferred)));
    }

    void handle_receive(shared_session session, const error_code& ec, std::size_t bytes_transferred) {

        // immediately accept new datagrams
    	//std::cout << "data size: " << bytes_transferred << std::endl;
    	std::cout << "data piece number: " << datas << std::endl;
    	datas += 1;
        receive_session();
    }



    udp::socket  socket_;
    asio::strand strand_;
    uint32_t datas;

    friend struct udp_session;
};



int main()
{
    try {

        asio::io_service io_service;
        udp_server server(io_service);
        boost::thread_group grupo;

        for (unsigned i = 0; i < thread::hardware_concurrency(); ++i)
            grupo.create_thread(boost::bind(&asio::io_service::run, ref(io_service)));

        grupo.join_all();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
