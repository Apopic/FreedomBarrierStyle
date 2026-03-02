#pragma once
#include <Windows.h>
#include <exception>

#include <string>
#include <fstream>
#include <filesystem>

#include <boost/stacktrace.hpp>

class Exception {
	void LogOut() const {
		std::ofstream ofs("Exception_dump.txt");

		ofs <<
			Message <<
			" in " <<
			FuncName << 
			" in " <<
			FileName <<
			" in " <<
			Line <<
			"\nstack trace:\n" <<
			boost::stacktrace::stacktrace();

		ofs.close();
	}

	std::string Message = "Exception";
	std::string FuncName;
	std::string FileName;
	size_t Line = 0;

public:

	Exception(const std::string& message, const std::string& funcname = "unnamed", const std::string& filename = "Exception.hpp", size_t line = __LINE__) {
		Message = message;
		FuncName = funcname;
		FileName = filename;
		Line = line;

		int id = MessageBox(NULL, (Message + "\n" + "An exception has occurred. Would you like to display information about the error?").c_str(), TEXT("Exception!"), MB_ICONERROR | MB_YESNO);
		if (id == IDYES) {
			LogOut();
			MessageBox(NULL, TEXT("If you would like to improve the simulator itself, please send the Exception_log.txt file to the developers."), TEXT("Log Dumped"), MB_ICONINFORMATION | MB_OK);
		}
	}
};

#define SendException(message) throw Exception(message, __FUNCTION__, std::filesystem::path(__FILE__).filename().u8string().c_str(), __LINE__)