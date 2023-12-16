#ifndef __HNPU_STU_AL__CLI_HPP__
#define __HNPU_STU_AL__CLI_HPP__

#include <iostream>				// std::cout | std::cin

#include "def.h"				// struct Config
#include "service_manager.hpp"	// class ServiceManager
#include "auth.hpp"				// namespace Auth

namespace CLI {
	static inline void ShowMenu() noexcept
	{
		std::cout
			<< "================= 工院校园网登录 =================\n"
			<< "1.开启自动登录校园网功能 (安装服务) [管理员权限]\n"
			<< "2.关闭自动登录校园网功能 (卸载服务) [管理员权限]\n"
			<< "3.尝试登录校园网\n"
			<< "4.退出程序\n";
	}

	inline bool Run(const Config* const conf)
	{
		std::cout << '\t';
		ShowMenu();

		int choice = 0;
		while (true)
		{
			std::cout << ">: ";
			while (!(std::cin >> choice))
			{
				std::cin.clear();
				while (std::cin.get() != '\n');
				std::cout << "错误的输入\n>: ";
			}

			switch (choice)
			{
			case 1:
				std::cout << "安装服务...\n";
				if (ServiceManager::Install()) std::cout << "安装成功.\n";
				else std::cout << "安装失败, 请以管理员身份运行此程序.\n";
				break;
			case 2:
				std::cout << "卸载服务...\n";
				if (ServiceManager::Uninstall()) std::cout << "卸载成功.\n";
				else std::cout << "卸载失败, 请以管理员身份运行此程序.\n";
				break;
			case 3:
				std::cout << "尝试登录...\n";
				std::cout << (Auth::Login(conf->USER_NAME, conf->PASSWORD) ? "登录成功" : "登录失败") << '\n';
				break;
			case 4:
				return true;
			default:
				std::cout << "错误的输入.\n";
				ShowMenu();
				break;
			}
		}

		return true;
	}
} // namespace CLI

#endif // !__HNPU_STU_AL__CLI_HPP__
