#pragma once
#include "GameSystem.hpp"

class Playing : public SceneBase {
public:

	Playing();
	~Playing();

	void Init();
	void Draw();
	void Proc();
	void End();

private:

	class _ScenemainPlaying* _ptr;

};

