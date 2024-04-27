#pragma once
#include <string>
#include <filesystem>

struct Config
{
public:
	std::filesystem::path PWD{};
	std::filesystem::path INI_FILE_PATH{};
	std::string USER_NAME{};
	std::string PASSWORD{};
};
