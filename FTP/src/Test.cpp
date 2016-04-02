#ifdef COEUS_TESTING

#include <iostream>
#include <gtest/gtest.h>
#include <asio.hpp>
#include <Functions.hpp>

TEST(FTP, StringTrim) {
	FTPConfig conf;
	conf.SetRootDir("public");
	conf.SetPort(2121);

	asio::io_service m_IoSvc;
	FTPWorker w(&conf, m_IoSvc, )
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
