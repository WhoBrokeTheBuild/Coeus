#ifdef COEUS_TESTING

#include <Functions.hpp>
#include <iostream>
#include <sstream>
#include <thread>
#include <gtest/gtest.h>
#include <asio.hpp>

#include "FTPConfig.hpp"
#include "FTPServer.hpp"

using std::thread;

class FTPTest :
	public ::testing::Test
{
public:

	inline virtual void SetUp()
	{
		m_Config.SetPort(92121);
		m_Config.SetRootDir("public");

		mp_Server = new FTPServer(m_Config);
	    m_Thread = std::thread([&](){
	        mp_Server->Run();
	    });
		m_Thread.detach();
	}

	inline virtual void TearDown()
	{
		mp_Server->Stop();
		delete mp_Server;
	}

protected:

	asio::io_service m_IoSvc;
    FTPConfig m_Config;
	FTPServer* mp_Server;
	std::thread m_Thread;

};

TEST_F(FTPTest, Connection) {

	std::stringstream ss;
	ss << m_Config.GetPort();

	tcp::resolver resolver(m_IoSvc);
	tcp::resolver::query query(tcp::v4(), "localhost", ss.str());
	auto it = resolver.resolve(query);

	tcp::socket sock(m_IoSvc);
	asio::connect(sock, it);
	sock.close();
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();

#if defined(COEUS_OS_WINDOWS)
	std::cin.get();
#endif

	return result;
}

#endif // COEUS_TESTING
