#pragma once
#include "Include.hpp"

extern class GameSystem;

class _MultiSelect {
public:

	_MultiSelect(GameSystem* ptr);
	~_MultiSelect();

	enum class Mode {
		Null = -1,
		ReturnTitle,
		Connecting,
		Count,
		AddressInput,
		Connect
	} ModeSelector = Mode::ReturnTitle;

	int InputHandle = -1;

	std::string ModeStrings[2] = {
		"ReturnTitle",
		"Connecting"
	};
};

