#ifndef COEUS_ARG_PARSER_HPP
#define COEUS_ARG_PARSER_HPP

/**
 * ArgParser.hpp
 * A tool for parsing command-line arguments in the style of getopt() and getopt_long()
 * Supports arguments in the following formats:
 * ./prog -p 1234 -f
 * ./prog --port 1234 --flag
 * ./prog --p "1234"
 * Does not support arguments in the following formats
 * ./prog -p1234
 * ./prog --port=1234
 * ./prog --port"1234"
 **/

#include <functional>
#include <string>
#include <vector>

using std::string;
using std::vector;

enum class OptionFlags : int {
	NoArg = 0,
	RequiredArg = 1,
	OptionalArg = 2,
};

struct Option
{
public:

	Option() = delete;

	Option(const Option&) = default;
	Option& operator=(const Option&) = default;
	~Option() = default;

	inline bool HasFlag(const OptionFlags& flag) const { return (((int)Flags & (int)flag) == (int)flag); }

	Option(const char& shrt, const string& lng, const OptionFlags& flags, std::function<void(string)> cb) :
		Short(shrt),
		Long(lng),
		Flags(flags),
		Callback(cb)
	{ }

	char Short;
	string Long;
	OptionFlags Flags = OptionFlags::NoArg;
	std::function<void(string)> Callback;

};

class ArgParser
{
public:

	ArgParser(const ArgParser&) = delete;
	ArgParser& operator=(const ArgParser&) = delete;

	ArgParser() = default;
	~ArgParser() = default;

	ArgParser(const vector<Option>& handlers) :
		m_Options(handlers)
	{ };

	bool Run(int argc, char** argv);

	inline void AddHandler(const Option& handler) { m_Options.push_back(handler); }

private:

	vector<Option> m_Options;
};

#endif // COEUS_ARG_PARSER_HPP