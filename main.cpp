#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <utility>
#include <string>

#include <boost/format.hpp>
#include <boost/regex.hpp>

std::list<boost::regex> stRegexList;

bool bReplaceMode = false;
boost::regex stReplaceRegexExpression;
const char* szReplaceExpression;

std::string strContent;
bool bQuiet = false;

void print_usage()
{
	printf("usage: regex [-help] [-match|-icmatch \"regex\"]... [-replace|-icreplace \"regex\" \"replace\"] [content]\n");
}

int main(int argc, char* argv[])
{
	try
	{
		const char* szContent = NULL;
		for(int i=1; i<argc; ++i)
		{
			if(strcmp(argv[i], "-help") == 0)
			{
				print_usage();
				return -1;
			}
			else if(strcmp(argv[i], "-quiet") == 0)
			{
				bQuiet = true;
			}
			else if(strcmp(argv[i], "-icmatch") == 0)
			{
				++i;
				if(i < argc)
				{
					bReplaceMode = false;
					boost::regex r(argv[i], boost::regex::normal|boost::regex::icase);
					stRegexList.push_back(r);
				}
				else
				{
					print_usage();
					return -1;
				}
			}
			else if(strcmp(argv[i], "-match") == 0)
			{
				++i;
				if(i < argc)
				{
					bReplaceMode = false;
					boost::regex r(argv[i]);
					stRegexList.push_back(r);
				}
				else
				{
					print_usage();
					return -1;
				}
			}
			else if(strcmp(argv[i], "-icreplace") == 0)
			{
				if(i + 2 < argc)
				{
					bReplaceMode = true;
					stReplaceRegexExpression.assign(argv[i + 1], boost::regex::normal|boost::regex::icase);
					szReplaceExpression = argv[i + 2];
					i += 2;
				}
				else
				{
					print_usage();
					return -1;
				}
			}
			else if(strcmp(argv[i], "-replace") == 0)
			{
				if(i + 2 < argc)
				{
					bReplaceMode = true;
					stReplaceRegexExpression.assign(argv[i + 1]);
					szReplaceExpression = argv[i + 2];
					i += 2;
				}
				else
				{
					print_usage();
					return -1;
				}
			}
			else
			{
				szContent = argv[i];
			}
		}
	
		if(szContent == NULL)
		{
			char buffer[4097];
			memset(buffer, 0, 4097);
			int size = 0;
			while((size = read(STDIN_FILENO, buffer, 4096)) > 0)
			{
				strContent.append(buffer);
				memset(buffer, 0, size);
			}
		}
		else
			strContent = std::string(szContent);
	
		if(!bReplaceMode)
		{
			bool bHasAllMatch = true;
			for(std::list<boost::regex>::iterator list_iter = stRegexList.begin();
				list_iter != stRegexList.end();
				++list_iter)
			{
				bool bMatch = false;
				boost::sregex_iterator end;
				for(boost::sregex_iterator iter = boost::make_regex_iterator(strContent, *list_iter);
					iter != end;
					++iter)
				{
					bMatch = true;
					if(!bQuiet)
					{
						for(int i=1; i<iter->size(); ++i)
							printf("%s\n", iter->str(i).c_str());
					}
				}
				if(!bMatch)
					bHasAllMatch = false;
			}
			return bHasAllMatch?0:-1;
		}
		else
		{
			printf("%s", boost::regex_replace(strContent, stReplaceRegexExpression, szReplaceExpression).c_str());
			return 0;
		}
	}
	catch(std::exception& error)
	{
		printf("error: %s\n", error.what());
		return -1;
	}
	return -1;
}

