#include "ArgParser.hpp"

bool ArgParser::Run(int argc, char ** argv)
{
	bool expectingVal = false;
	Option* tmpArgHandler = nullptr;
	for (int i = 1; i < argc; ++i)
	{
		string arg = argv[i];
		if (arg.length() >= 2 && arg[0] == '-')
		{
			if (expectingVal && tmpArgHandler)
			{
				if (tmpArgHandler->HasFlag(OptionFlags::RequiredArg))
				{
					printf("%s: option '%s' requires an argument\n", argv[0], argv[i - 1]);
					return false;
				}
				else
				{
					tmpArgHandler->Callback(arg);
					continue;
				}
			}

			expectingVal = false;
			tmpArgHandler = nullptr;

			if (arg[1] == '-') // Long Option --
			{
				string longArg = arg.substr(2);
				if (longArg.empty())
					continue;

				for (Option& handler : m_Options)
				{
					if (handler.Long == longArg)
					{
						if ( handler.HasFlag(OptionFlags::RequiredArg) ||
							 handler.HasFlag(OptionFlags::OptionalArg) )
						{
							expectingVal = true;
							tmpArgHandler = &handler;
						}
						else
						{
							handler.Callback(string());
						}
					}
				}
			}
			else // Short Option(s) -
			{
				string shortArgs = arg.substr(1);
				for (const char& c : shortArgs)
				{
					for (Option& handler : m_Options)
					{
						if (handler.Short == c)
						{
							if (handler.HasFlag(OptionFlags::RequiredArg) ||
								handler.HasFlag(OptionFlags::OptionalArg))
							{
								expectingVal = true;
								tmpArgHandler = &handler;
							}
							else
							{
								handler.Callback(string());
							}
						}
					}
				}
			}
		}
		else if (expectingVal && tmpArgHandler)
		{
			tmpArgHandler->Callback(arg);

			expectingVal = false;
			tmpArgHandler = nullptr;
		}
	}

	return true;
}
