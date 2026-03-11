#pragma once
#include "Include.hpp"
#include "Config.h"
#include "Skin.h"

extern class GameSystem;

class _Loading {
public:

	_Loading(GameSystem* ptr);
	~_Loading();

	double ScoreRateCalc(double judge, double basis) {
		const double c = 0.9;
		const double b = basis;
		const double m = 10;
		const double d = std::pow(b, std::pow(m, -1 / c));
		return 1 / std::pow(std::log(judge * (b - d) / b + d) / std::log(b), c);
	}

	double SongSpeedRateCalc(double speed) {
		return (speed - 1.0) * 0.2 + 1.0;
	}
};