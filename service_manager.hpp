#pragma once
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef UNICODE
#define UNICODE
#endif

#include <iostream>
#include <string>
#include <fstream> // std::ofstream
#include <format>  // std::format

#include <tchar.h>
#include <winsock2.h> // Windows Network API
#include <netioapi.h> // NotifyIpInterfaceChange
#include <iphlpapi.h> // IP Helper API
#include <ws2tcpip.h> // Windows Socket API
#include <windows.h>  // Windows API
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#include <cstdlib>

#include "def.h"
#include "logger.hpp"
#include "auth.hpp"

namespace ServiceManager
{
	using namespace std::chrono_literals;

	std::wstring ServiceName = L"HNPI_STU_AL";
	std::wstring ServiceDisplayName = L"河南工业职业技术学院校园网自动认证服务";

	SERVICE_STATUS_HANDLE ServiceStatusHandle;
	SERVICE_STATUS ServiceStatus;

	Config *conf;
	static Logger *logger;

	// internal function, don't call this.
	void ReportServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
	{
		ServiceStatus.dwCurrentState = dwCurrentState;
		ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
		ServiceStatus.dwWaitHint = dwWaitHint;

		if (dwCurrentState == SERVICE_START_PENDING)
			ServiceStatus.dwControlsAccepted = 0;
		else
		{
#ifdef _DEBUG
			logger->writeInfo("ReportServiceStatus", "Service started.\n");
#endif
			ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		}

		if (dwCurrentState == SERVICE_RUNNING || dwCurrentState == SERVICE_STOPPED)
			ServiceStatus.dwCheckPoint = 0;
		else
			ServiceStatus.dwCheckPoint++;

		SetServiceStatus(ServiceStatusHandle, &ServiceStatus);
	}

	// internal function, don't call this.
	DWORD WINAPI HandlerEx(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
	{
		logger->writeInfo("HandlerEx", std::format("Operation instructions: {}\n", dwControl).c_str());

		switch (dwControl)
		{
		case SERVICE_CONTROL_STOP:
			ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
#ifdef _DEBUG
			logger->writeInfo("HandlerEx", "Service shutting down...\n");
#endif
			ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
#ifdef _DEBUG
			logger->writeInfo("HandlerEx", "Service stopped.\n");
#endif
			return NO_ERROR;
		case SERVICE_CONTROL_INTERROGATE:
			return NO_ERROR;
		default:
			logger->writeWarning("HandlerEx", "Call not implemented.\n");
			return ERROR_CALL_NOT_IMPLEMENTED;
		}
	}

	// internal function, don't call this.
	void OnNetworkAddressChanged(PVOID callerContext, PMIB_IPINTERFACE_ROW row, MIB_NOTIFICATION_TYPE notificationType)
	{
		logger->writeInfo("OnNetworkAddressChanged", "Network address changed.\n");
		logger->writeInfo("Auth", "Login...\n");
		bool loginResult = Auth::Login(conf->USER_NAME, conf->PASSWORD);
		logger->writeInfo("Auth", loginResult ? "Login successful.\n" : "Login failed.\n");
	}

	// internal function, don't call this.
	void WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
	{
#ifdef _DEBUG
		logger->writeInfo("ServiceMain", "Initializing service...\n");
#endif
		ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
		ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		ServiceStatus.dwWin32ExitCode = NO_ERROR;
		ServiceStatus.dwServiceSpecificExitCode = 0;
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwWaitHint = 0;

		ServiceStatusHandle = ::RegisterServiceCtrlHandlerExW(ServiceName.c_str(), HandlerEx, NULL);
		if (ServiceStatusHandle == NULL)
		{
			logger->writeError("ServiceMain", "Failed to open service control handler.\n");
			return;
		}
		ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			logger->writeError("ServiceMain", "WSAStartup failed.\n");
			ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
			return;
		}

		HANDLE h = NULL;
		if (NotifyIpInterfaceChange(AF_INET, OnNetworkAddressChanged, NULL, FALSE, &h) != NO_ERROR || h == NULL)
		{
			logger->writeError("ServiceMain", "Failed to regist notify for ip interface changed.\n");
			WSACleanup();
			ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
			return;
		}

		logger->writeInfo("ServiceMain", "\n\n\n\t\tService started\n\n\n");
		ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);
		while (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
		{
			Sleep(120000);
		}

		ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
	}

	// install the service
	bool Install()
	{
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		if (hSCM == NULL)
		{
			std::cout << "打开服务管理器失败\n";
			return false;
		}

		wchar_t execPathWithParams[MAX_PATH];
		GetModuleFileNameW(NULL, execPathWithParams, MAX_PATH);
		wcscat_s(execPathWithParams, MAX_PATH, L" -service");

		SC_HANDLE hService = CreateServiceW(
			hSCM, ServiceName.c_str(), ServiceDisplayName.c_str(),
			SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
			SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
			execPathWithParams, NULL, NULL, NULL, NULL, NULL);
		if (hService == NULL)
		{
			if (GetLastError() == 1073)
				std::cout << "服务已存在.\n";
			else
				std::cout << "创建服务失败\n";
			CloseServiceHandle(hSCM);
			return false;
		}

		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);

		return true;
	}

	// uninstall the service
	bool Uninstall()
	{
		SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
		if (hSCM == NULL)
		{
			std::cout << "打开服务管理器失败\n";
			return false;
		}

		SC_HANDLE hService = OpenService(hSCM, ServiceName.c_str(), SERVICE_QUERY_STATUS | DELETE);
		if (hService == NULL)
		{
			std::cout << "打开服务失败\n";
			CloseServiceHandle(hSCM);
			return false;
		}

		SERVICE_STATUS status;
		if (QueryServiceStatus(hService, &status) == 0)
		{
			std::cout << "无法查询服务状态\n";
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return false;
		}

		if (status.dwCurrentState != SERVICE_STOPPED)
		{
			std::cout << "服务正在运行中, 请以管理员身份执行 \"net stop HNPI_STUAL\" 停止服务后再次尝试.\n";
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return false;
		}

		if (DeleteService(hService) == 0)
		{
			std::cout << "卸载服务失败\n";
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCM);
			return false;
		}

		CloseServiceHandle(hService);
		CloseServiceHandle(hSCM);
		return true;
	}

	inline void DestoryLogger()
	{
		logger = nullptr;
		Logger::DestoryInstance();
	}

	// run service
	int Run(Config *config)
	{
		conf = config;

		SERVICE_TABLE_ENTRY ServiceTable[] = {
			{(LPWSTR)ServiceName.c_str(), (LPSERVICE_MAIN_FUNCTIONW)ServiceMain},
			{NULL, NULL}};

		try
		{
			logger = Logger::GetInstance(conf->PWD / "logs");
			logger->writeInfo("SERVICE_TABLE_ENTRY", "OK\n");
			Auth::Login(conf->USER_NAME, conf->PASSWORD);
		}
		catch (const std::exception &err)
		{
			DestoryLogger();
			std::cout << err.what();
			return 1;
		}

		if (::StartServiceCtrlDispatcherW(ServiceTable) == FALSE)
		{
			logger->writeError("Main", "Failed to start service control dispatcher.\n");
			DestoryLogger();
			return 1;
		}

		logger->writeInfo("ServiceCtrlDispatcherW", "OK\n");
		DestoryLogger();
		return 0;
	}
}
