#pragma once
#include "Include.hpp"
#include "Base/SceneBase.hpp"
#include "Composition/Skin.hpp"
#include "Library/DiscordGameSDK/discord.h"

class GameSystem {

	GameSystem();
	~GameSystem();

	SceneBase* Scene = nullptr;

public:

	static GameSystem& GetInstance() {
		static GameSystem instance;
		return instance;
	}

	void Draw();
	void Proc();

	void SceneChange(SceneBase* dest);
	
	struct _Transition {
		void Start(double time, ease::Base base, ease::Line line, SceneBase* nextptr = nullptr) {
			Input.SetInputLock(true);
			MoveTime = 1. / time;
			Base = base;
			Line = line;
			NextScenePtr = nextptr;
			Is_Move = true;
			MoveTimer.Start();
		}
		void Draw() {
			MoveingTime = MoveTimer.GetRecordingTime() * MoveTime;

			switch (Base)
			{
			case ease::In:
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * ease::GetEasingRate(MoveingTime, Base, Line));
				break;
			case ease::Out:
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - (255 * ease::GetEasingRate(MoveingTime, Base, Line)));
				break;
			default:
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				break;
			}

			DrawBox(
				0,
				0,
				Skin.UseSkinInfo.Resolution.Width,
				Skin.UseSkinInfo.Resolution.Height,
				GetColor(0, 0, 0),
				TRUE
			);

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		}
		bool IsMoveout() {
			return MoveingTime > 1;
		}
		void End() {
			Input.SetInputLock(false);
			*this = _Transition();
		}

		ease::Base Base = ease::Out;
		ease::Line Line = ease::Linear;
		SceneBase* NextScenePtr = nullptr;
		Timer<second> MoveTimer;
		double MoveTime = 0;
		bool Is_Move = false;

	private:
		double MoveingTime = 0;

	} Transition;
	Timer<second> TPSTimer = Timer<second>(false);
	double TPSTime = 0;

	bool EndFlag = false;

	void DiscordActivityChange(const char*, const char*, const char*, bool);
	std::unique_ptr<discord::Core> discord_Core;
	discord::Activity discord_Activity;
	time_t UnixTime;
};

#define Game GameSystem::GetInstance()