#pragma once
#include "GameSystem.hpp"

class ModeSelectBoard : public ContentBase {
public:

	ModeSelectBoard() { Name = __func__; }
	~ModeSelectBoard() { End(); }

	void Create(const std::string& dir, const json& data) {
		BaseCreate(data);
		JSONKEYCOMP(data, "Board") {
			Board.Create(dir, data["Board"]);
		}
		JSONKEYCOMP(data, "Selector") {
			Board.Create(dir, data["Selector"]);
		}

		Content::String strings[ModeStringSize]{};
		for (uint i = 0; i < ModeStringSize; ++i) {
			Content::String string;
			string.Drawstring = ModeString[i];
			string.Create(dir, data["Font"]);
			strings[i] = std::move(string);
		}



	}

	void Init() {

	}

	void Draw(...) {
		va_list vallist;
		va_start(vallist, 0);
		va_arg(vallist, bool);





		va_end(vallist);
	}

	void End() {
		
	}

	_NODISCARD ContentBase* Clone() {
		return new ModeSelectBoard(*this);
	}

	static const size_t ModeStringSize = 5;
	std::string ModeString[ModeStringSize] = {
		"Single",
		"Multi",
		"Dan",
		"Config",
		"End"
	};

	Content::Graph Board{};
	Content::Graph Selector{};
};
