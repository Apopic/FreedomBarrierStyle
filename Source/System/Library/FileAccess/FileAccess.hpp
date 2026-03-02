#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include "Library/ExLib/ExString/strconv.h"

enum class FAO {
	null = -1,
	rc_slash,
	rc_sharp,
	rc_slas,
	rc_all
};

class FileAccess {
public:

	FileAccess(const std::string& path, FAO option = FAO::null) {
		this->Read(path, option);
	}
	
	~FileAccess() {

	}

	std::string& operator[](size_t index) {
		return m_List[index];
	}

	bool Write() const {
		std::ofstream ofs(this->FilePath);

		if (!ofs.is_open()) {
			return false;
		}

		for (uint i = 0; i < m_List.size(); ++i) {
			ofs << m_List[i] << "\n";
		}

		ofs.close();

		return true;
	}

	void Read(const std::string& path, FAO option = FAO::null) {
		m_List.clear();
		std::ifstream ifs(path);
		if (!ifs.is_open()) {
			m_IsOpen = false;
			return;
		}
		m_IsOpen = true;
		FilePath = std::filesystem::path(path);

		std::string line;
		while (std::getline(ifs, line)) {
			m_List.push_back(line);
		}

		RemoveComment(option);

		ifs.close();
	}

	void RemoveComment(FAO option = FAO::null) {
		if (option == FAO::null) {
			return;
		}
		bool slas = false;
		for (size_t i = 0, size = m_List.size(); i < size; ++i) {
			std::string& line = m_List[i];

			if (option != FAO::rc_all) {
				if (option == FAO::rc_slash) {
					line = line.find("//") != std::string::npos ? line.substr(0, line.find("//")) : line;
				}
				else if (option == FAO::rc_sharp) {
					line = line.find("#") != std::string::npos ? line.substr(0, line.find("#")) : line;
				}
				else if (option == FAO::rc_slas) {
					if (line.find("/*") != std::string::npos) {
						line = line.substr(0, line.find("#"));
						slas = true;
					}
					else if (line.find("*/") != std::string::npos) {
						line = line.substr(line.find("*/") + 2, line.size() - (line.find("*/") + 2) - 1);
						slas = false;
					}
					else if (slas) {
						line = "";
					}
				}
			}
			else {
				line = line.find("//") != std::string::npos ? line.substr(0, line.find("//")) : line;
				line = line.find("#") != std::string::npos ? line.substr(0, line.find("#")) : line;
				if (line.find("/*") != std::string::npos) {
					line = line.substr(0, line.find("#"));
					slas = true;
				}
				else if (line.find("*/") != std::string::npos) {
					line = line.substr(line.find("*/") + 2, line.size() - (line.find("*/") + 2) - 1);
					slas = false;
				}
				else if (slas) {
					line = "";
				}
			}
		}
	}

	void CodePageToUTF8(UINT codepage) {
		std::ifstream ifs(FilePath.u8string());

		if (!ifs.is_open()) {
			SendException("It's better to force it to close before you lose your data.");
			return;
		}

		std::ostringstream oss;
		oss.str("");
		oss.clear(std::stringstream::goodbit);
		oss << ifs.rdbuf();

		ifs.close();

		std::string str = oss.str();
		while (str.find('\0') == 0 || str.find('\n') == 0 || str.find('\r') == 0) {
			str = str.substr(1);
		}

		std::string ret = cp_to_utf8(str, codepage);

		std::ofstream ofs(FilePath.u8string());
		
		if (!ofs.is_open()) {
			SendException("It's better to force it to close before you lose your data.");
			return;
		}

		ofs.write(ret.c_str(), ret.size());

		ofs.close();

		this->Read(FilePath.u8string(), m_Option);
	}

	inline size_t LineCount() const { return m_List.size(); }
	inline bool GetIsOpen() const { return m_IsOpen; }

	std::filesystem::path FilePath;

private:

	using uchar = unsigned char;
	using ushort = unsigned short;
	using uint = unsigned int;
	using ulong = unsigned long;
	using ulonglong = unsigned long long;

	std::vector<std::string> m_List;
	bool m_IsOpen;
	FAO m_Option;
};

inline bool FilePathExists(const std::string& path) {
	std::ifstream ifs(path);
	bool ret = ifs.is_open();
	if (ret) {
		ifs.close();
	}
	return ret;
}