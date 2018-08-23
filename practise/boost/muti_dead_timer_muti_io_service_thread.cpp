#include<iostream>
#include<boost/asio.hpp>
#include<boost/thread.hpp>



void handler1(const boost::system::error_code& ec) {
	std::cout << " handler1" << std::endl;
}

void handler2(const boost::system::error_code& ec) {

	std::cout << "handler2" << std::endl;
}


boost::asio::io_service ioService1;
boost::asio::io_service ioService2;

void run1()
{
	ioService1.run();
}

void run2() {
	ioService2.run();
}


int main() {

	boost::asio::deadline_timer timer1(ioService1, boost::posix_time::seconds(5));
	timer1.async_wait(handler1);

	boost::asio::deadline_timer timer2(ioService2, boost::posix_time::seconds(5));
	timer2.async_wait(handler2);

	boost::thread th1(run1);
	boost::thread th2(run2);

	th1.join();
	th2.join();
	





	return 0;
}