#pragma once
#include "GameSystem.hpp"

class Result : public SceneBase {
public:

	Result();
	~Result();

	void Init();
	void Draw();
	void Proc();
	void End();

private:

	class _ScenemainResult* _ptr;

};

