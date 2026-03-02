#pragma once
#include "Include.hpp"

extern class GameSystem;

class _Result {
public:

	_Result(GameSystem* ptr);
	~_Result();

	void ScoreDraw(const Pos2D<float> &Pos, ulonglong Num);
	void AccurecyDraw(const Pos2D<float> &Pos, double Rate);
	void JudgesDraw(const Pos2D<float> &Pos, ulonglong Num);

};
