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
		std::cout << "�����ļ�������, ��������...\n";
		std::ofstream file(conf->INI_FILE_PATH);
		if (!file) {
			std::cout << "���������ļ�ʧ��, �볢���Թ���Ա������д˳���.\n";
			return std::nullopt;
		}
		file << "[Settings]\n";
		file << "UserName=\n";
		file << "Password=";
		file.close();
		std::cout << "���ڵ�ǰĿ¼���������ļ�, ����д�˺�������ٴ�����.\n";
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
		std::cout << "�˺Ż�����Ϊ��\n" << "��� settings.ini �ļ������˺�����.\n";
		return std::nullopt;
	}

	std::cout << "�˺�: " << conf->USER_NAME << "\n";
	std::cout << "����: " << Utils::GenerateMask(conf->PASSWORD.size()) << "\n";

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
		std::cout << "���ٵ�¼...\n";
		bool res = Auth::Login(conf->USER_NAME.c_str(), conf->PASSWORD.c_str());
		std::cout << (res ? "��½�ɹ�" : "��½ʧ��") << '\n';
		ok = CLI::Run(conf);
	}

	delete conf;
	system("pause");
	return ok;
}
