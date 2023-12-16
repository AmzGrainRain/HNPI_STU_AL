#ifndef __HNPI_STU_AL__LOGGER_HPP__
#define __HNPI_STU_AL__LOGGER_HPP__

#include <chrono>		// struct std::chrono::system_clock
#include <string>		// std::string | std::stringstream
#include <format>		// std::format
#include <fstream>		// std::ofstream
#include <filesystem>	// std::filesystem

class Logger
{
public:
	Logger(std::string logDir) : directory_(logDir)
	{
		namespace fs = std::filesystem;

		if (!fs::exists(directory_))
		{
			if (!fs::create_directory(directory_))
				throw new std::exception("Failed to create log directory.");
		}

		if (fs::status(directory_).type() != fs::file_type::directory)
		{
			if (!fs::remove(directory_) || !fs::create_directory(directory_))
				throw new std::exception("A file with the same name already exists, unable to create log directory.");
		}

		clock_ = new std::chrono::system_clock();
		file_ = new std::ofstream(
			std::format("{}\\{}.log", directory_.string(), getCurrentDate()),
			std::ios::app
		);
	}

	~Logger()
	{
		if (clock_ != nullptr)
		{
			delete clock_;
			clock_ = nullptr;
		}

		if (file_ != nullptr)
		{
			if (file_->is_open()) file_->close();
			delete file_;
			file_ = nullptr;
		}
	}

	bool readyToUse() const noexcept
	{
		return file_->is_open();
	}

	bool write(const char* msg) const
	{
		if (!readyToUse()) return false;
		*file_ << msg;
		file_->flush();
		return true;
	}

	bool writeInfo(const char* form, const char* msg) const
	{
		return write("INFO", form, msg);
	}

	bool writeWarning(const char* form, const char* msg) const
	{
		return write("WARNING", form, msg);
	}

	bool writeError(const char* form, const char* msg) const
	{
		return write("ERROR", form, msg);
	}

	std::filesystem::path getLogDirectory() const noexcept
	{
		return directory_;
	}

private:
	bool write(const char* type, const char* form, const char* msg) const
	{
		if (!readyToUse()) return false;
		*file_ << std::format("[{}] [{}] [{}] {}", getCurrentDateTime(), type, form, msg);
		file_->flush();
		return true;
	}

	std::string getCurrentDate() const
	{
		time_t time = std::chrono::system_clock::to_time_t(clock_->now());
		struct tm timeinfo;
		localtime_s(&timeinfo, &time);
		std::stringstream ss;
		ss << std::put_time(&timeinfo, "%Y-%m-%d");
		return ss.str();
	}

	std::string getCurrentDateTime() const
	{
		time_t time = std::chrono::system_clock::to_time_t(clock_->now());
		struct tm timeinfo;
		localtime_s(&timeinfo, &time);
		std::stringstream ss;
		ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
		return ss.str();
	}

	std::chrono::system_clock* clock_;
	std::filesystem::path directory_;
	std::ofstream* file_;
};

#endif // !__HNPI_STU_AL__LOGGER_HPP__
