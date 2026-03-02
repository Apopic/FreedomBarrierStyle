#include "GameSystem.h"

constexpr discord::ClientId DiscordClientID = 1162615073597038592;

#define SCENEBRANCH(name, type) \
case Scene::name:\
name##type();\
break;


#define INSWITCH(type)\
SCENEBRANCH(Title, type)\
SCENEBRANCH(SongSelect, type)\
SCENEBRANCH(Loading, type)\
SCENEBRANCH(Playing, type)\
SCENEBRANCH(Result, type)\
\
SCENEBRANCH(MultiSelect, type)\
SCENEBRANCH(MultiRoom, type)\
\
SCENEBRANCH(Config, type)\
SCENEBRANCH(Skin, type)\

void GameSystem::Draw() {
	BackFPS = (BackFPS + (1 / FPSTimer.GetRecordingTime())) / 2;
	FPSTimer.Start();
	
	if (MemScene != NowScene) {
		switch (MemScene) {
			INSWITCH(End);
		}

		MemScene = NowScene;

		switch (NowScene) {
			INSWITCH(Init);
		}
	}

	DrawTimer.Start();
	switch (NowScene) {
		INSWITCH(Draw);
	}
	BackDrawTime = (BackDrawTime + DrawTimer.GetRecordingTime()) / 2;

	if (Config.ViewDebugData) {
		if (AverageTimer.GetRecordingTime() > 0.5) {
			AverageTimer.Start();
			NowFPS = BackFPS;
			NowDrawTime = BackDrawTime;
		}
		
		DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS:%d\nTPS:%d\nDrawTime:%.12lf", NowFPS, NowTPS, NowDrawTime);
	}
}

void GameSystem::Proc() {
	NowTPS = (NowTPS + (1 / TPSTimer.GetRecordingTime())) / 2;
	TPSTimer.Start();

	switch (NowScene) {
		INSWITCH(Proc);
	}

	Input.HitKeyProcess(VK_CONTROL, KeyState::Hold, [&] {
		Input.HitKeyProcess(VK_F1, KeyState::Down, [&] {
			SongSelect.EnumChartFile(Config.SongDirectories);
		});
		Input.HitKeyProcess(VK_F2, KeyState::Down, [&] {
			Config.ConfigLoad();
		});
		Input.HitKeyProcess(VK_F3, KeyState::Down, [&] {
			Skin.SkinLoad(Config.SkinName);
		});
	});

}

void GameSystem::DiscordActivityChange(const char *state_str, const char *smallimagename, const char *hovertext, bool timeupdate) {
	if (discord_Core == nullptr) {
		return;
	}
	time(&UnixTime);

	discord_Activity.SetType(discord::ActivityType::Playing);
	if (state_str != nullptr) { discord_Activity.SetState(state_str); }
	if (smallimagename != nullptr) { discord_Activity.GetAssets().SetSmallImage(smallimagename); }
	if (hovertext != nullptr) { discord_Activity.GetAssets().SetLargeText(hovertext); discord_Activity.GetAssets().SetSmallText(hovertext); }
	if (timeupdate) { discord_Activity.GetTimestamps().SetStart(UnixTime); }
	
	discord_Core->ActivityManager().UpdateActivity(discord_Activity, nullptr);
	discord_Core->RunCallbacks();
}

GameSystem::GameSystem() {
	discord::Core *core_raw{};
	if (Config.DiscordRPC) {
		discord::Core::Create(DiscordClientID, DiscordCreateFlags_Default, &core_raw);
	}

	discord_Core.reset(core_raw);

	if (discord_Core == nullptr) {
		return;
	}

	discord_Activity.GetAssets().SetLargeImage("appicon");
	DiscordActivityChange("Initializing...", nullptr, nullptr, true);
}

GameSystem::~GameSystem() {

}