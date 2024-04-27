#pragma once
#include <string>	// std::string

namespace Utils
{
	// Generate a password mask
	static std::string GenerateMask(size_t len) noexcept
	{
		std::string mask;
		for (size_t i = 0; i < len; ++i)
			mask += "*";
		return mask;
	}

	std::string EncodeURIComponent(const std::string &url) noexcept
	{
		std::string res;
		for (char c : url)
		{
			switch (c)
			{
			case ' ':
				res += "%20";
				break;
			case '!':
				res += "%21";
				break;
			case '"':
				res += "%22";
				break;
			case '#':
				res += "%23";
				break;
			case '$':
				res += "%24";
				break;
			case '%':
				res += "%25";
				break;
			case '&':
				res += "%26";
				break;
			case '\'':
				res += "%27";
				break;
			case '(':
				res += "%28";
				break;
			case ')':
				res += "%29";
				break;
			case '*':
				res += "%2A";
				break;
			case '+':
				res += "%2B";
				break;
			case ',':
				res += "%2C";
				break;
			case '/':
				res += "%2F";
				break;
			case ':':
				res += "%3A";
				break;
			case ';':
				res += "%3B";
				break;
			case '<':
				res += "%3C";
				break;
			case '=':
				res += "%3D";
				break;
			case '>':
				res += "%3E";
				break;
			case '?':
				res += "%3F";
				break;
			case '@':
				res += "%40";
				break;
			case '[':
				res += "%5B";
				break;
			case '\\':
				res += "%5C";
				break;
			case ']':
				res += "%5D";
				break;
			case '^':
				res += "%5E";
				break;
			case '_':
				res += "%5F";
				break;
			case '`':
				res += "%60";
				break;
			case '{':
				res += "%7B";
				break;
			case '|':
				res += "%7C";
				break;
			case '}':
				res += "%7D";
				break;
			case '~':
				res += "%7E";
				break;
			default:
				res += c;
			}
		}
		return res;
	}

	void HideConsoleCursor()
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO cci;
		GetConsoleCursorInfo(hConsole, &cci);
		cci.bVisible = false;
		SetConsoleCursorInfo(hConsole, &cci);
	}
}
