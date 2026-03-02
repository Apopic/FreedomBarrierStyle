#pragma once
#include "../resource.h"
#include "OldGame/GameSystem.h"
#include "Include.hpp"

class Main {
public:
	Main();
	~Main();

	int Process();

	GameSystem Game;

	bool InitSuccessFlag = false;
};