#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/system/error_code.hpp>
#include <boost/bind/bind.hpp>
#include<boost/thread.hpp>

using namespace boost::asio;
using namespace std;

class client
{
	typedef client this_type;
	typedef ip::tcp::acceptor acceptor_type;
	typedef ip::tcp::endpoint endpoint_type;
	typedef ip::tcp::socket socket_type;
	typedef ip::address address_type;
	typedef boost::shared_ptr<socket_type> sock_ptr;
	typedef vector<char> buffer_type;

private:
	io_service m_io;
	buffer_type m_buf;
	endpoint_type m_ep;
public:
	client() : m_buf(1024, 0), m_ep(address_type::from_string("127.0.0.1"), 8000)
	{
		start();
	}

	void run()
	{
		m_io.run();
	}

	void start()
	{
		sock_ptr sock(new socket_type(m_io));
		sock->async_connect(m_ep, boost::bind(&this_type::conn_handler, this, boost::asio::placeholders::error, sock));
	}

	void conn_handler(const boost::system::error_code&ec, sock_ptr sock)
	{
		if (ec)
		{
			std::cout << ec.message() << std::endl;
			return;
		}
		cout << "Receive from " << sock->remote_endpoint().address() << ": ";
		cout << "Port:" << sock->remote_endpoint().port() << endl;
		sock->async_read_some(buffer(m_buf), boost::bind(&client::read_handler, this, boost::asio::placeholders::error, sock));
	}

	void read_handler(const boost::system::error_code&ec, sock_ptr sock)
	{
		if (ec)
		{
			return;
		}
		sock->async_read_some(buffer(m_buf), boost::bind(&client::read_handler, this, boost::asio::placeholders::error, sock));
		cout << &m_buf[0] << endl;


		boost::this_thread::sleep(boost::posix_time::milliseconds(200));

		char pmsg[] = "client 6666";

		sock->async_write_some(buffer(pmsg), boost::bind(&this_type::write_handler, this, boost::asio::placeholders::error));
	}

	void write_handler(const boost::system::error_code& ec)
	{
		if (ec)
		{
			std::cout << ec.message() << std::endl;
			return;
		}

		std::cout << "send success!" << std::endl;
		
	}
};

int main()
{
	try
	{
		cout << "Client start." << endl;
		client cl;
		cl.run();
	}
	catch (std::exception &e)
	{
		cout << e.what() << endl;
	}

	while (true)
	{
		boost::this_thread::sleep(boost::posix_time::seconds(5));
	}

	return 0;
}