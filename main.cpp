#include <iostream>
#include <optional>
#include <string>

#include "def.h"   // struct Config
#include "cli.hpp" // namespace CLI

#include <Windows.h> // GetPrivateProfileStringW

// success will be returns memory-allocated struct Config*, don't forget to delete it
static std::optional<Config *> init(const char *execPath)
{
	Config *conf = new Config;
	conf->PWD = execPath;
	conf->PWD = conf->PWD.parent_path();
	conf->INI_FILE_PATH = conf->PWD / "settings.ini";

	if (!std::filesystem::exists(conf->INI_FILE_PATH))
	{
		std::cout << "配置文件不存在, 尝试生成...\n";
		std::ofstream file(conf->INI_FILE_PATH);
		if (!file)
		{
			std::cout << "生成配置文件失败, 请尝试以管理员身份运行此程序.\n";
			return std::nullopt;
		}
		file << "[Settings]\n";
		file << "UserName=\n";
		file << "Password=";
		file.close();
		std::cout << "已在当前目录生成配置文件, 请在 settings.ini 内填写账号密码后再次运行.\n";
		return std::nullopt;
	}

	{
		wchar_t wcUserName[50];
		char cUserName[50];
		GetPrivateProfileStringW(_T("Settings"), _T("UserName"), _T(""), wcUserName, 50, conf->INI_FILE_PATH.wstring().c_str());
		WideCharToMultiByte(CP_UTF8, WC_NO_BEST_FIT_CHARS, wcUserName, 50, cUserName, 50, NULL, NULL);
		conf->USER_NAME = std::move(std::string(cUserName));

		wchar_t wcPassword[50];
		char cPassword[50];
		GetPrivateProfileStringW(_T("Settings"), _T("Password"), _T(""), wcPassword, 50, conf->INI_FILE_PATH.wstring().c_str());
		WideCharToMultiByte(CP_UTF8, WC_NO_BEST_FIT_CHARS, wcPassword, 50, cPassword, 50, NULL, NULL);
		conf->PASSWORD = std::move(std::string(cPassword));
		;
	}

	if (conf->USER_NAME.size() == 0 || conf->PASSWORD.size() == 0)
	{
		std::cout << "账号或密码为空\n"
				  << "请打开 settings.ini 文件配置账号密码.\n";
		return std::nullopt;
	}

	std::cout << "账号: " << conf->USER_NAME << "\n";
	std::cout << "密码: " << Utils::GenerateMask(conf->PASSWORD.size()) << "\n";

	return conf;
}

int main(int argc, char **argv)
{
	Config *conf = nullptr;
	{
		std::optional<Config *> initialized = init(argv[0]);
		if (!initialized.has_value())
		{
			system("pause");
			return 1;
		}
		conf = initialized.value();
	}

	if (argc == 2 && strcmp(argv[1], "-service") == 0)
		return ServiceManager::Run(conf);

	std::cout << "快速登录...\n";
	bool res = Auth::Login(conf->USER_NAME, conf->PASSWORD);
	std::cout << (res ? "登陆成功" : "登陆失败") << '\n';
	return CLI::Run(conf);
}
