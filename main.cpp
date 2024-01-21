#include <iostream>
#include <optional>
#include <string>

#include "def.h"		// struct Config
#include "logger.hpp"	// class Logger
#include "cli.hpp"		// namespace CLI

#include <Windows.h>	// GetPrivateProfileString

// success will be returns memory-allocated struct Config*, don't forget to delete it
static std::optional<Config*> init(const char* execPath)
{
	Config* conf = new Config;
	conf->PWD = execPath;
	conf->PWD = conf->PWD.substr(0, conf->PWD.rfind('\\'));
	conf->INI_FILE_PATH = conf->PWD + "\\settings.ini";

	if (!Utils::FileExists(conf->INI_FILE_PATH.c_str()))
	{
		std::cout << "配置文件不存在, 尝试生成...\n";
		std::ofstream file(conf->INI_FILE_PATH);
		if (!file) {
			std::cout << "生成配置文件失败, 请尝试以管理员身份运行此程序.\n";
			return std::nullopt;
		}
		file << "[Settings]\n";
		file << "UserName=\n";
		file << "Password=";
		file.close();
		std::cout << "已在当前目录生成配置文件, 请填写账号密码后再次运行.\n";
		return std::nullopt;
	}

	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		std::wstring path = converter.from_bytes(conf->INI_FILE_PATH);

		wchar_t userName[50];
		GetPrivateProfileStringW(_T("Settings"), _T("UserName"), _T(""), userName, 50, path.c_str());
		conf->USER_NAME = converter.to_bytes(userName);

		wchar_t password[50];
		GetPrivateProfileStringW(_T("Settings"), _T("Password"), _T(""), password, 50, path.c_str());
		conf->PASSWORD = converter.to_bytes(password);
	}

	if (conf->USER_NAME.size() == 0 || conf->PASSWORD.size() == 0)
	{
		std::cout << "账号或密码为空\n" << "请打开 settings.ini 文件配置账号密码.\n";
		return std::nullopt;
	}

	std::cout << "账号: " << conf->USER_NAME << "\n";
	std::cout << "密码: " << Utils::GenerateMask(conf->PASSWORD.size()) << "\n";

	return conf;
}

int main(int argc, char** argv) {
	Config* conf = nullptr;
	{
		std::optional<Config*> initialized = init(argv[0]);
		if (!initialized.has_value()) {
			system("pause");
			return 1;
		}
		conf = initialized.value();
	}

	bool ok = true;
	if (argc == 2 && strcmp(argv[1], "-service") == 0)
		ok = ServiceManager::Run(conf);
	else
	{
		std::cout << "快速登录...\n";
		bool res = Auth::Login(conf->USER_NAME.c_str(), conf->PASSWORD.c_str());
		std::cout << (res ? "登陆成功" : "登陆失败") << '\n';
		ok = CLI::Run(conf);
	}

	delete conf;
	system("pause");
	return ok;
}
