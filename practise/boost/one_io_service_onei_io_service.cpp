#include<iostream>
#include<boost/asio.hpp>


void handler(const boost::system::error_code& ec)
{
	std::cout << "five seconds " << std::endl;
}



int main() {

	boost::asio::io_service io_service;
	boost::asio::deadline_timer timer(io_service, boost::posix_time::seconds(5));
	timer.async_wait(handler);

	io_service.run();





	return 0;
}