#include "Include.hpp"

#include "Config.h"
#include "Skin.h"

#include "Loading.h"
#include "Playing.h"
#include "Result.h"
#include "SongSelect.h"
#include "Title.h"

#include "MultiRoom.h"
#include "MultiSelect.h"

#include "Library/DiscordGameSDK/discord.h"

#define STRUCTDEF(name) \
void name##Init();\
void name##End();\
void name##Draw();\
void name##Proc();\
_##name name = _##name(this);\

enum class Scene {
	Title,
	SongSelect,
	Loading,
	Playing,
	Result,

	MultiSelect,
	MultiRoom,

	Config,
	Skin
};

class GameSystem {
public:

	GameSystem();
	~GameSystem();

	STRUCTDEF(Config);
	STRUCTDEF(Skin);

	STRUCTDEF(Title);
	STRUCTDEF(SongSelect);
	STRUCTDEF(Loading);
	STRUCTDEF(Playing);
	STRUCTDEF(Result);
	
	STRUCTDEF(MultiSelect);
	STRUCTDEF(MultiRoom);

	void Draw();
	void Proc();

	Scene NowScene = Scene::Title;
	Scene MemScene = Scene::Config;

	bool EndFlag = false;

	Timer<second> AverageTimer = Timer<second>(true);

	Timer<second> FPSTimer = Timer<second>(true);
	double BackFPS = GetRefreshRate();
	int NowFPS = 0;

	Timer<second> DrawTimer = Timer<second>(true);
	double BackDrawTime = 1;
	double NowDrawTime = 0;

	Timer<second> TPSTimer = Timer<second>(false);
	int NowTPS = 10000;
	void DiscordActivityChange(const char *, const char *, const char *, bool);
	std::unique_ptr<discord::Core> discord_Core;
	discord::Activity discord_Activity;
	time_t UnixTime;
};