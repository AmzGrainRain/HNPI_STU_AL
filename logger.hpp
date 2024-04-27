#pragma once
#include <chrono>	  // struct std::chrono::system_clock
#include <string>	  // std::string | std::stringstream
#include <format>	  // std::format
#include <fstream>	  // std::ofstream
#include <filesystem> // std::filesystem

class Logger
{
public:
	static Logger *GetInstance(const std::filesystem::path &path)
	{
		if (instance_ == nullptr)
			instance_ = new Logger(path);

		return instance_;
	}

	static void DestoryInstance()
	{
		if (instance_ != nullptr)
		{
			delete instance_;
			instance_ = nullptr;
		}
	}

	bool write(const char *type, const char *form, const char *msg)
	{
		return write(std::format("[{}] [{}] [{}] {}", getTime("%Y-%m-%d %H:%M:%S"), type, form, msg));
	}

	inline bool writeInfo(const char *form, const char *msg)
	{
		return write("INFO", form, msg);
	}

	inline bool writeWarning(const char *form, const char *msg)
	{
		return write("WARN", form, msg);
	}

	inline bool writeError(const char *form, const char *msg)
	{
		return write("ERROR", form, msg);
	}

	const std::filesystem::path &getLogDirectory() const noexcept
	{
		return directory_;
	}

private:
	Logger(const std::filesystem::path &logDir) : directory_(logDir)
	{
		namespace fs = std::filesystem;

		if (!fs::exists(directory_))
		{
			if (!fs::create_directory(directory_))
				throw new std::runtime_error("Failed to create log directory.");
		}

		if (fs::status(directory_).type() != fs::file_type::directory)
		{
			if (!fs::remove(directory_) || !fs::create_directory(directory_))
				throw new std::runtime_error("A file with the same name already exists, unable to create log directory.");
		}

		clock_ = std::make_unique<std::chrono::system_clock>();
		file_ = std::make_unique<std::ofstream>(getLogFilePath(), std::ios::app);
	}

	// "%Y-%m-%d %H:%M:%S"
	std::string getTime(const char *format) const
	{
		time_t time = std::chrono::system_clock::to_time_t(clock_->now());
		struct tm timeinfo;
		localtime_s(&timeinfo, &time);
		std::stringstream ss;
		ss << std::put_time(&timeinfo, format);
		return ss.str();
	}

	std::filesystem::path getLogFilePath()
	{
		return directory_ / (getTime("%Y-%m-%d") + ".log");
	}

	bool write(const std::string& message)
	{
		if (!file_->is_open())
			return false;

		*file_ << message;
		file_->flush();
		return true;
	}

	static Logger *instance_;
	std::unique_ptr<std::chrono::system_clock> clock_;
	std::unique_ptr<std::ofstream> file_;
	std::filesystem::path directory_;
};

Logger *Logger::instance_ = nullptr;
