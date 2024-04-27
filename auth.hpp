#pragma once

#include <iostream> // std::cout
#include <string>	// std::string
#include <optional> // std::optional
#include <sstream>	// std::stringstream

// ---------------------- libhv ---------------------- //
#include "HttpClient.h"
#include "requests.h"
// --------------------------------------------------- //

#include "utils.hpp"		   // namespace Utils
#include "service_manager.hpp" // namespace ServiceManager

namespace Auth
{
	static std::optional<std::string> GetQueryString()
	{
		try
		{
			// HttpRequest req;
			// req.method = HTTP_GET;
			// req.url = "http://123.123.123.123";
			// req.timeout = 2;
			// req.body = NoBody;
			// req.headers = DefaultHeaders;

			// HttpResponse res;

			// hv::HttpClient client;
			// client.send(&req, &res);

			// std::cout << res.body.c_str();

			// if (res.status_code != 200)
			// 	return std::nullopt;


			// std::string body = res.body.c_str();
			// size_t start = body.find('?') + 1;
			// size_t end = body.rfind('\'');
			// return body.substr(start, end - start);

			auto res = requests::get("http://123.123.123.123");
			if (res == NULL)
				return std::nullopt;

			std::string body = res->body.c_str();
			size_t start = body.find('?') + 1;
			size_t end = body.rfind('\'');
			return body.substr(start, end - start);
		}
		catch (const std::exception &err)
		{
			std::cout << err.what() << "\n";
			return std::nullopt;
		}
	}

	static std::optional<std::string> GetSessionId()
	{
		try
		{
			HttpRequest req;
			req.method = HTTP_GET;
			req.url = "http://192.168.0.123";
			req.redirect = 0;
			req.headers["DNT"] = 1;
			req.headers["Host"] = "192.168.0.123";
			req.headers["Upgrade-Insecure-Requests"] = 1;
			req.headers["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/116.0.0.0 Safari/537.36 Edg/116.0.1938.76";

			hv::HttpClient client;
			HttpResponse res;
			client.send(&req, &res);

			auto it = res.headers.find("Set-Cookie");
			if (it == res.headers.end())
				return std::nullopt;
			std::string cookie = it->second;

			return cookie.substr(0, cookie.find(';'));
		}
		catch (const std::exception &err)
		{
			std::cout << err.what() << "\n";
			return std::nullopt;
		}
	}

	static std::optional<std::string> Auth(const std::string &userName,
										   const std::string &password,
										   const std::string &sessionId,
										   const std::string &queryString)
	{
		std::string cookie = "EPORTAL_COOKIE_PASSWORD=; ";
		cookie += "EPORTAL_COOKIE_USERNAME=; ";
		cookie += "EPORTAL_COOKIE_OPERATORPWD=; ";
		cookie += sessionId;

		std::stringstream body;
		body << "userId=" << userName;
		body << "&password=" << password;
		body << "&service=";
		body << "&operatorPwd=";
		body << "&operatorUserId=";
		body << "&validcode=";
		body << "&passwordEncrypt=false";
		body << "&queryString=" << Utils::EncodeURIComponent(queryString);

		HttpRequest req;
		req.method = HTTP_POST;
		req.url = "http://192.168.0.123/eportal/InterFace.do?method=login";
		req.timeout = 3;
		req.headers["Content-Type"] = "application/x-www-form-urlencoded";
		req.headers["Cookie"] = cookie;
		req.headers["Dnt"] = "1";
		req.headers["Host"] = "192.168.0.123";
		req.headers["Origin"] = "http://192.168.0.123";
		req.headers["Referer"] = "http://192.168.0.123/eportal/index.jsp?" + queryString;
		req.headers["User-Agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/116.0.0.0 Safari/537.36 Edg/116.0.1938.76";
		req.body = body.str();

		HttpResponse res;
		res.SetContentType("text/plain; charset=utf-8");

		hv::HttpClient client;
		client.send(&req, &res);

		return std::string(res.body);
	}

	bool Login(const std::string &userName, const std::string &password)
	{
		auto queryString = GetQueryString();
		if (!queryString.has_value())
		{
			std::cout << "获取 WiFi 信息失败. (已成功登录的无法再次登录)\n";
			return false;
		}

		auto sessionId = GetSessionId();
		if (!sessionId.has_value())
		{
			std::cout << "网络异常.\n";
			return false;
		}

		auto authResult = Auth(userName, password, sessionId.value(), queryString.value());
		if (!authResult.has_value())
		{
			std::cout << "登录失败.\n";
			return false;
		}

		return true;
	}
}
