#ifndef __HNPI_STU_AL__UTILS_HPP__
#define __HNPI_STU_AL__UTILS_HPP__

#include <string>		// std::string | std::wstring
#include <filesystem>	// std::filesystem::exists

#include <windows.h>	// WideCharToMultiByte() | MultiByteToWideChar()

namespace Utils
{
	// Convert a wide Unicode string to an UTF8 string
	std::string wchar2char(const wchar_t* const wc)
	{
		size_t len = wcslen(wc);
		if (len == 0) return "";

		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wc, -1, NULL, 0, NULL, NULL);
		char* buffer = new char[sizeNeeded];
		WideCharToMultiByte(CP_UTF8, 0, wc, -1, buffer, sizeNeeded, NULL, NULL);

		std::string res(buffer);
		delete[] buffer;

		return res;
	}

	// Convert an UTF8 string to a wide Unicode String
	std::wstring char2wchar(const char* c)
	{
		size_t len = strlen(c);
		if (!len) return L"";

		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, c, len, NULL, 0);
		wchar_t* buffer = new wchar_t[sizeNeeded];
		MultiByteToWideChar(CP_UTF8, 0, c, len, buffer, sizeNeeded);

		std::wstring res(buffer);
		delete[] buffer;

		return res;
	}

	// Generate a password mask
	static std::string GenerateMask(size_t len) noexcept
	{
		std::string mask;
		for (size_t i = 0; i < len; ++i)
			mask += "*";
		return mask;
	}

	// Check file exist
	static bool FileExists(const char* path) {
		return std::filesystem::exists(path);
	}

	const char* EncodeURIComponent(const char* url) noexcept {
		std::string res;
		for (size_t i = 0; i < strlen(url); ++i) {
			switch (url[i])
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
				res += i;
			}
		}
		return res.c_str();
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

#endif // !__HNPI_STU_AL__UTILS_HPP__
