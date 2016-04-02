#ifdef COEUS_TESTING

#include <iostream>
#include <gtest/gtest.h>
#include "ConfigParser.hpp"
#include "ArgParser.hpp"
#include "Platform.hpp"
#include "Functions.hpp"
#include "Event.hpp"

TEST(Common, StringTrim) {
	EXPECT_EQ("Hello, World", StringTrim("  \t Hello, World\t\t "));
	EXPECT_EQ("Hello, World", StringTrim("Hello, World \t "));
	EXPECT_EQ("Hello, World", StringTrim(" \t Hello, World"));
	EXPECT_EQ("", StringTrim(""));
}

TEST(Common, CleanWhitespace) {
	EXPECT_EQ("Hello, World", CleanWhitespace("Hello,     World"));
	EXPECT_EQ("Hello, World", CleanWhitespace("Hello, \t World"));
	EXPECT_EQ("Hello, World", CleanWhitespace("Hello,  \t  \t  World"));
	EXPECT_EQ("", CleanWhitespace(""));
}

TEST(Common, Basename) {
	EXPECT_EQ(Basename("/path/to/filename"), "filename");
	EXPECT_EQ(Basename("path/to/filename"), "filename");
	EXPECT_EQ(Basename("/path/to/filename.txt"), "filename.txt");
	EXPECT_EQ(Basename("path/to/filename.txt"), "filename.txt");
}

TEST(Common, Dirname) {
	EXPECT_EQ(Dirname("/path/to/filename"), "/path/to");
	EXPECT_EQ(Dirname("path/to/filename"), "path/to");
	EXPECT_EQ(Dirname("/path/to/filename.txt"), "/path/to");
	EXPECT_EQ(Dirname("path/to/filename.txt"), "path/to");
}

TEST(Common, ConfigParser) {
	std::map<string, string> config;
	EXPECT_TRUE(ParseConfig("conf/test.cfg", config));
	EXPECT_EQ(config.size(), 3);
	EXPECT_EQ(config["testnum"], "1234");
	EXPECT_EQ(config["teststring"], "Hello, World");
	EXPECT_EQ(config["testinc"], "test");
}

TEST(Common, ArgParser) {
	vector<string> rawArgv = {
		"",
		"-f",
		"conf/main.cfg",
		"--port",
		"1234",
		"--flag"
	};

	int argc = rawArgv.size();
	char** argv = new char*[argc];
	for (int i = 0; i < argc; ++i)
	{
		argv[i] = new char[rawArgv[i].size() + 1];
		memcpy(argv[i], rawArgv[i].c_str(), rawArgv[i].size() + 1);
	}

	int callbacks = 0;
	string confVal, portVal, flagVal;

	bool res = ArgParser({
		Option('f', "config", OptionFlags::RequiredArg,
			[&](string val)
			{
				confVal = val;
				++callbacks;
			}),
		Option('p', "port", OptionFlags::OptionalArg,
			[&](string val)
			{
				portVal = val;
				++callbacks;
			}),
		Option('\0', "flag", OptionFlags::NoArg,
			[&](string val)
			{
				flagVal = val;
				++callbacks;
			})
	}).Run(argc, (char**)argv);

	EXPECT_TRUE(res);
	EXPECT_EQ(confVal, string(argv[2]));
	EXPECT_EQ(portVal, string(argv[4]));
	EXPECT_EQ(flagVal, "");
	EXPECT_EQ(callbacks, 3);

	for (int i = 0; i < argc; ++i)
	{
		delete[] argv[i];
	}
	delete[] argv;
}

TEST(Common, Event) {
	Event<int> test;

	bool eventFired = false;
	int eventVal = -1;
	test.AddListener(
		[&](int test) {
			eventFired = true;
			eventVal = test;
		}
	);

	test.Dispatch(1234);

	ASSERT_TRUE(eventFired);
	ASSERT_EQ(eventVal, 1234);
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
