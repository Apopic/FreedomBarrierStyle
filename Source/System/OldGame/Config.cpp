#include "Config.h"
#include "GameSystem.h"

_Config::_Config(GameSystem* ptr) {
	ConfigLoad();
}

_Config::~_Config() {

}

void _Config::ConfigLoad() {
	std::ifstream ifs("config.json");

	if (!ifs.is_open()) {
		ifs.close();
		ConfigWrite();
		ifs = std::ifstream("config.json");
	}

	json data = json::parse(ifs);

#define JSONDATA(name) name = data[#name]

	JSONDATA(WaitVSync);
	JSONDATA(FastInput);
	JSONDATA(FastDrawRate);
	JSONDATA(FullScreen);
	JSONDATA(FrameCountTimer);
	JSONDATA(DiscordRPC);
	JSONDATA(ChartChunkSize);
	JSONDATA(ViewDebugData);
	JSONDATA(SkinName);
	JSONDATA(SongDirectories);
	JSONDATA(SoundDeviceType);
	JSONDATA(Exclusive);
	JSONDATA(SampleRate);
	JSONDATA(BufferSize);
	JSONDATA(SongVolume);
	JSONDATA(SEVolume);
	JSONDATA(MultiPlayName);
	JSONDATA(AutoPlayFlag);
	JSONDATA(HitNoteDisp);
	JSONDATA(RollSpeed);
	JSONDATA(JudgeGood);
	JSONDATA(JudgeOk);
	JSONDATA(JudgeBad);
	JSONDATA(SongOffset);
	JSONDATA(ChartSpeed);
	JSONDATA(SongSpeed);
	JSONDATA(HiddenLevel);
	JSONDATA(SuddenLevel);
	JSONDATA(RandomRate);
	JSONDATA(DonInputLeft).get<std::vector<int>>();
	JSONDATA(DonInputRight).get<std::vector<int>>();
	JSONDATA(KaInputLeft).get<std::vector<int>>();
	JSONDATA(KaInputRight).get<std::vector<int>>();

#undef JSONDATA

	//KeyCodeParser(false);

	ifs.close();
}

void _Config::ConfigWrite() {

#define JSONDATA(name) {#name, name}
	nlohmann::ordered_json data = {
		JSONDATA(WaitVSync),
		JSONDATA(FastInput),
		JSONDATA(FastDrawRate),
		JSONDATA(FullScreen),
		JSONDATA(FrameCountTimer),
		JSONDATA(DiscordRPC),
		JSONDATA(ChartChunkSize),
		JSONDATA(ViewDebugData),
		JSONDATA(SkinName),
		JSONDATA(SongDirectories),
		JSONDATA(SoundDeviceType),
		JSONDATA(Exclusive),
		JSONDATA(SampleRate),
		JSONDATA(BufferSize),
		JSONDATA(SongVolume),
		JSONDATA(SEVolume),
		JSONDATA(MultiPlayName),
		JSONDATA(AutoPlayFlag),
		JSONDATA(HitNoteDisp),
		JSONDATA(RollSpeed),
		JSONDATA(JudgeGood),
		JSONDATA(JudgeOk),
		JSONDATA(JudgeBad),
		JSONDATA(SongOffset),
		JSONDATA(ChartSpeed),
		JSONDATA(SongSpeed),
		JSONDATA(HiddenLevel),
		JSONDATA(SuddenLevel),
		JSONDATA(RandomRate),
		JSONDATA(DonInputLeft),
		JSONDATA(DonInputRight),
		JSONDATA(KaInputLeft),
		JSONDATA(KaInputRight),
	};
#undef JSONDATA

	std::ofstream ofs("config.json");

	ofs << data.dump(4) << "\n";

	ofs.close();
}

void _Config::KeyCodeParser(bool flag) {
	for (size_t i = 0, size = DonInputLeft.size(); i < size; ++i) {
		DonInputLeft[i] = flag ? ConvertKeyCodeToVirtualKey(DonInputLeft[i]) : ConvertVirtualKeyToKeyCode(DonInputLeft[i]);
	}
	for (size_t i = 0, size = DonInputRight.size(); i < size; ++i) {
		DonInputRight[i] = flag ? ConvertKeyCodeToVirtualKey(DonInputRight[i]) : ConvertVirtualKeyToKeyCode(DonInputRight[i]);
	}
	for (size_t i = 0, size = KaInputLeft.size(); i < size; ++i) {
		KaInputLeft[i] = flag ? ConvertKeyCodeToVirtualKey(KaInputLeft[i]) : ConvertVirtualKeyToKeyCode(KaInputLeft[i]);
	}
	for (size_t i = 0, size = KaInputRight.size(); i < size; ++i) {
		KaInputRight[i] = flag ? ConvertKeyCodeToVirtualKey(KaInputRight[i]) : ConvertVirtualKeyToKeyCode(KaInputRight[i]);
	}

}

void GameSystem::ConfigInit() {

}

void GameSystem::ConfigEnd() {

}

void GameSystem::ConfigDraw() {

}

void GameSystem::ConfigProc() {

}
