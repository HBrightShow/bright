#include<iostream>
#include<boost/asio.hpp>
#include<boost/thread.hpp>


void handler1(const boost::system::error_code &ec)
{
	std::cout << "5 s. first" << std::endl;
	boost::this_thread::sleep(boost::posix_time::seconds(3));
}

void handle2(const boost::system::error_code& ec)
{
	std::cout << "5 s second" << std::endl;
}

void handle3(const boost::system::error_code& ec)
{
	std::cout << "5 s third" << std::endl;
}

boost::asio::io_service io_service;

void run()
{
	io_service.run();
}


int main() {

	boost::asio::deadline_timer timer1(io_service, boost::posix_time::seconds(5));
	timer1.async_wait(handler1);

	boost::asio::deadline_timer timer2(io_service, boost::posix_time::seconds(5));
	timer2.async_wait(handle2);

	boost::asio::deadline_timer timer3(io_service, boost::posix_time::seconds(5));
	timer3.async_wait(handle3);

	boost::thread thread1(run);
	boost::thread thread2(run);

	std::cout << "waiting ..." << std::endl;

	thread2.join();
	thread2.join();







	return 0;
}