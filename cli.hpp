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
			<< "================= ��ԺУ԰����¼ =================\n"
			<< "1.�����Զ���¼У԰������ (��װ����) [����ԱȨ��]\n"
			<< "2.�ر��Զ���¼У԰������ (ж�ط���) [����ԱȨ��]\n"
			<< "3.���Ե�¼У԰��\n"
			<< "4.�˳�����\n";
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
				std::cout << "���������\n>: ";
			}

			switch (choice)
			{
			case 1:
				std::cout << "��װ����...\n";
				if (ServiceManager::Install()) std::cout << "��װ�ɹ�.\n";
				else std::cout << "��װʧ��, ���Թ���Ա������д˳���.\n";
				break;
			case 2:
				std::cout << "ж�ط���...\n";
				if (ServiceManager::Uninstall()) std::cout << "ж�سɹ�.\n";
				else std::cout << "ж��ʧ��, ���Թ���Ա������д˳���.\n";
				break;
			case 3:
				std::cout << "���Ե�¼...\n";
				std::cout << (Auth::Login(conf->USER_NAME.c_str(), conf->PASSWORD.c_str()) ? "��¼�ɹ�" : "��¼ʧ��") << '\n';
				break;
			case 4:
				return true;
			default:
				std::cout << "���������.\n";
				ShowMenu();
				break;
			}
		}

		return true;
	}
} // namespace CLI

#endif // !__HNPU_STU_AL__CLI_HPP__
