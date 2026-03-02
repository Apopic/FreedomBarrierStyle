#pragma once
#include <cmath>
#include <string>
#include <ctype.h>
#include <vector>

class MathParser {

	std::vector<std::string> split(std::string str, char del) {

		size_t first = 0;
		size_t last = str.find_first_of(del);

		std::vector<std::string> result;

		while (first < str.size()) {
			std::string subStr(str, first, last - first);

			result.push_back(subStr);

			first = last + 1;
			last = str.find_first_of(del, first);

			if (last == std::string::npos) {
				last = str.size();
			}
		}

		return result;

	}

	using uint = unsigned int;
	using ushort = unsigned short;
	using ulong = unsigned long;
	using ulonglong = unsigned long long;
	
	MathParser(std::string message) { std::cerr << "MathParser Error / Message: " << message << "\n"; }

public:

	MathParser(){}

	enum class Operator {
		Add,
		Sub,
		Mul,
		Div,
		Sur
	};

	class OPData {
	public:
		double val = 0;
		Operator op = Operator::Add;
	};


	inline double Parse(const std::string& str) {
		std::string line = this->__EraseSpace(str);
		OPData _tempOPData;
		std::vector<OPData> OPDatas;
		bool readop = true;
		bool signedflag = false;
		for (uint i = 0; i < line.size(); ++i) {
			if (readop) {
				switch (line[i]) {
				case '-':
					signedflag = true;
				}
			}
			switch (line[i])
			{
			case '+':
				_tempOPData.op = Operator::Add;
				readop = true;
				break;
			case '-':
				_tempOPData.op = Operator::Sub;
				readop = true;
				break;
			case '*':
				_tempOPData.op = Operator::Mul;
				readop = true;
				break;
			case '/':
				_tempOPData.op = Operator::Div;
				readop = true;
				break;
			case '%':
				_tempOPData.op = Operator::Sur;
				readop = true;
				break;
			}

			if (line[i] == '(') {
				if (!readop) {
					throw MathParser("Not Operator");
					return 0;
				}

				std::string _line = line.substr(i);
				_tempOPData.val = this->Parse(this->__BranketAnalysis(_line, &i));

				if (signedflag) {
					_tempOPData.val *= -1;
					signedflag = false;
				}
				OPDatas.push_back(_tempOPData);
				readop = false;
			}
			else if (isdigit(line[i])) {
				bool notbreak = true;
				uint notbreakindex = 0;
				for (uint j = i; j < line.size(); ++j) {
					if (!(isdigit(line[j]) || line[j] == '.')) {
						_tempOPData.val = std::stod(line.substr(i, j - i));
						i = j - 1;
						notbreak = false;
						break;
					}
					notbreakindex = j;
				}
				if (notbreak) {
					_tempOPData.val = std::stod(line.substr(i, line.size() - i));
					i = notbreakindex;
				}
				if (signedflag) {
					_tempOPData.val *= -1;
					signedflag = false;
				}
				OPDatas.push_back(_tempOPData);
				readop = false;
			}
			else if (isalpha(line[i])) {
				if (!readop) {
					throw MathParser("Not Operator");
					return 0;
				}
				int addindex = i;
				std::string _line = line.substr(i, line.size() - i);
				std::vector<std::string> _datas = split(this->__BranketAnalysis(_line, &i), ',');
				
#define MATHFUNC(name) \
if (_line.find(#name + std::string("(")) == 0) {\
_tempOPData.val = std::##name(this->Parse(_datas[0]));\
break;\
}
				do {
					MATHFUNC(sqrt);
					MATHFUNC(cbrt);
					MATHFUNC(sin);
					MATHFUNC(cos);
					MATHFUNC(tan);
					MATHFUNC(asin);
					MATHFUNC(acos);
					MATHFUNC(atan);
					MATHFUNC(sinh);
					MATHFUNC(cosh);
					MATHFUNC(tanh);
					MATHFUNC(asinh);
					MATHFUNC(acosh);
					MATHFUNC(atanh);

				} while (false);

#undef MATHFUNC

				i += addindex;

				if (signedflag) {
					_tempOPData.val *= -1;
					signedflag = false;
				}
				OPDatas.push_back(_tempOPData);
				readop = false;
			}
		}

		std::vector<OPData> _tempOPDatas;

		// Operator::Sur & Operator::Div & Operator::Mul
		for (uint i = 0, e = 0; i < OPDatas.size(); ++i) {
			if (i != 0) {
				switch (OPDatas[i].op) {
				case Operator::Sur:
					_tempOPDatas[e].val = std::fmod(_tempOPDatas[e].val, OPDatas[i].val);
					continue;
					break;
				case Operator::Div:
					_tempOPDatas[e].val = _tempOPDatas[e].val / OPDatas[i].val;
					continue;
					break;
				case Operator::Mul:
					_tempOPDatas[e].val = _tempOPDatas[e].val * OPDatas[i].val;
					continue;
					break;
				}
			}
			_tempOPDatas.push_back(OPDatas[i]); e = i;
		}
		OPDatas = _tempOPDatas;
		_tempOPDatas.clear();

		// Operator::Add & Operator::Sub
		for (uint i = 0, e = 0; i < OPDatas.size(); ++i) {
			if (i != 0) {
				switch (OPDatas[i].op) {
				case Operator::Add:
					_tempOPDatas[e].val = _tempOPDatas[e].val + OPDatas[i].val;
					continue;
					break;
				case Operator::Sub:
					_tempOPDatas[e].val = _tempOPDatas[e].val - OPDatas[i].val;
					continue;
					break;
				}
			}
			_tempOPDatas.push_back(OPDatas[i]); e = i;
		}
		OPDatas = _tempOPDatas;
		_tempOPDatas.clear();

		return OPDatas[0].val;
	}

	inline std::string __BranketAnalysis(const std::string& str, uint *index) {
		std::string res;
		for (uint i = 0; i < str.size(); ++i) {
			if (str[i] == '(') {
				int branketval = 0;
				for (uint j = i; j < str.size(); ++j) {
					if (str[j] == '(') {
						++branketval;
					}
					else if (str[j] == ')') {
						if (branketval == 1) {
							if (index != nullptr) { *index = j; }
							res = str.substr(i + 1, j - i - 1);
							return res;
						}
						--branketval;
					}
				}
				if (branketval != 1) {
					throw MathParser("Not branketval 0");
					return "";
				}
			}
		}		
		return res;
	}
	inline std::string __EraseSpace(const std::string& str) {
		std::string res;
		
		for (ulonglong i = 0; i < str.size(); ++i) {
			if (std::isspace(str[i])) {
				continue;
			}
			res += str[i];
		}

		return res;
	}
};
