#pragma once
#include "GameSystem.hpp"

class Title : public SceneBase {
public:

	Title();
	~Title();

	void Init();
	void Draw();
	void Proc();
	void End();

private:

	class _ScenemainTitle* _ptr;

};
