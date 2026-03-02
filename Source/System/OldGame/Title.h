#pragma once
#include "Include.hpp"

extern class GameSystem;

class _Title {
public:

	_Title(GameSystem* ptr);
	~_Title();

	enum class Mode {
		Start = -1,
		Single,
		Multi,
		Dan,
		Config,
		End,
		Count
	} ModeSelector = Mode::Start;

	std::string ModeStrings[5] = {
		"Single",
		"Multi",
		"Dan",
		"Config",
		"End"
	};

	Timer<millisecond> PlateAlphaTimer;
	int PlateAlphaTime() const {
		return 1000;
	}

	Timer<millisecond> CaptionAlphaTimer;
	int CaptionAlphaLoopTime() const {
		return 1000;
	}
	
	Timer<millisecond> SelectorAlphaTimer;
	int SelectorAlphaLoopTime() const {
		return 1000;
	}
};
