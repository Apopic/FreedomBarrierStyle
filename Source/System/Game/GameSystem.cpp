#include "GameSystem.hpp"

constexpr discord::ClientId DiscordClientID = 1162615073597038592;

GameSystem::GameSystem() {

	discord::Core* core_raw{};
	auto discord_result = discord::Core::Create(DiscordClientID, DiscordCreateFlags_Default, &core_raw);

	if (core_raw != nullptr) {
		discord_Core.reset(core_raw);
	}
	else {
		return;
	}

	discord_Activity.GetAssets().SetLargeImage("appicon");
	DiscordActivityChange("Initializing...", nullptr, nullptr, true);
}

GameSystem::~GameSystem() {

}

void GameSystem::Draw() {
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	Scene->Draw();

	if (Transition.Is_Move) {
		Transition.Draw();

		if (Transition.IsMoveout()) {
			_Transition temptrans = Transition;
			Transition.End();
			if (temptrans.Base == ease::In) {
				SceneChange(temptrans.NextScenePtr);
			}
		}
	}
	
	DrawFormatString(0, 0, GetColor(255, 255, 255),"FPS:%d\nTPS:%d",(int)GetFPS(), (int)(1 / TPSTime));
}

void GameSystem::Proc() {
	TPSTimer.Start();
	Input.Update();
	Input.HitKeyProcess(KEY_INPUT_LCONTROL, KeyState::Hold, [&] {
		Input.HitKeyProcess(KEY_INPUT_F1, KeyState::Down, [&] {
			SongSelect.EnumChartFile(Config.SongDirectories);
			});
		Input.HitKeyProcess(KEY_INPUT_F2, KeyState::Down, [&] {
			Config.Read();
			});
		Input.HitKeyProcess(KEY_INPUT_F3, KeyState::Down, [&] {
			Skin.Read();
			Skin.SceneLoad(Scene->GetName());
			});
		});

	Scene->Proc();
	TPSTime = TPSTimer.GetRecordingTime();
}

void GameSystem::DiscordActivityChange(const char* state_str, const char* hovertext, const char* smallimagename, bool timeupdate) {
	if (discord_Core == nullptr) {
		return;
	}
	time(&UnixTime);

	if (state_str != nullptr) { discord_Activity.SetState(state_str); }
	discord_Activity.SetType(discord::ActivityType::Playing);
	if (smallimagename != nullptr) { discord_Activity.GetAssets().SetSmallImage(smallimagename); }
	if (hovertext != nullptr) { discord_Activity.GetAssets().SetLargeText(hovertext); discord_Activity.GetAssets().SetSmallText(hovertext); }
	if (timeupdate) { discord_Activity.GetTimestamps().SetStart(UnixTime); }
	
	discord_Core->ActivityManager().UpdateActivity(discord_Activity, nullptr);
	discord_Core->RunCallbacks();
}

void GameSystem::SceneChange(SceneBase* dest) {
	if (dest == nullptr) {
		SendException("SceneObject NullPointer");
		EndFlag = true;
		return;
	}
	if (Scene != nullptr) {
		Scene->End();
	}
	delete Scene;
	Scene = dest;
	Skin.SceneLoad(Scene->GetName());
	Scene->Init();
}
