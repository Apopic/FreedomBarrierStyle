#pragma once
#include "Include.hpp"

class Config {

	Config() { Read(); }
	~Config() {}

public:

	static Config& GetInstance() {
		static Config Instance;
		return Instance;
	}

	// System
	int RefreshRate = 60; // Valid outside Windows
	bool WaitVSync = true;
	bool FastInput = true;
	bool FrameCountTimer = false;
	std::string SkinName = "Default";
	std::vector<std::string> SongDirectories{ "Songs" };
	float SongVolume = 62;
	float SEVolume = 80;

	// GamePlay
	std::string MultiPlayName = "NoName";
	bool AutoPlayFlag = true;
	double RollSpeed = 20;
	double JudgeGood = 25;
	double JudgeOk = 75;
	double JudgeBad = 100;
	double SongOffset = 0;
	double ChartSpeed = 1;
	double SongSpeed = 1;
	double HiddenLevel = 0;
	double SuddenLevel = 0;
	int RandomRate = 0;

	// KeyConfig
	std::vector<int> KaInputLeft{ 'D', 'S' };
	std::vector<int> DonInputLeft{ 'F', 'G' };
	std::vector<int> DonInputRight{ 'J', 'H' };
	std::vector<int> KaInputRight{ 'K', 'L' };

	void Read() {
		std::ifstream ifs("config.json");

		if (!ifs.is_open()) {
			ifs.close();
			Write();
			ifs = std::ifstream("config.json");
		}

		json data = json::parse(ifs);

		try {

#define JSONDATA(name) name = data[#name]

			JSONDATA(RefreshRate);
			JSONDATA(WaitVSync);
			JSONDATA(FastInput);
			JSONDATA(FrameCountTimer);
			JSONDATA(SkinName);
			JSONDATA(SongDirectories);
			JSONDATA(SongVolume);
			JSONDATA(SEVolume);
			JSONDATA(MultiPlayName);
			JSONDATA(AutoPlayFlag);
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

		}
		catch (std::exception& e) {
			SendException(e.what());
		}

		KeyCodeParser(false);

		ifs.close();
	}

	void Write() {

#define JSONDATA(name) {#name, name}
		nlohmann::ordered_json data = {
			JSONDATA(RefreshRate),
			JSONDATA(WaitVSync),
			JSONDATA(FastInput),
			JSONDATA(FrameCountTimer),
			JSONDATA(SkinName),
			JSONDATA(SongDirectories),
			JSONDATA(SongVolume),
			JSONDATA(SEVolume),
			JSONDATA(MultiPlayName),
			JSONDATA(AutoPlayFlag),
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

	void KeyCodeParser(bool flag) {
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
};

#define Config Config::GetInstance()