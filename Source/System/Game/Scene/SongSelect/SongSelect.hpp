#pragma once
#include "GameSystem.hpp"

class SongSelect : public SceneBase {
public:

	SongSelect();
	~SongSelect();

	void Init();
	void Draw();
	void Proc();
	void End();

private:

	class _ScenemainSongSelect* _ptr;

};

