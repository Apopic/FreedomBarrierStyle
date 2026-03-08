#pragma once
#include "Include.hpp"
#include <curl/curl.h>

#pragma comment(lib, "urlmon.lib")

extern class GameSystem;

class _Config {
public:

	_Config(GameSystem* ptr);
	~_Config();

	void ConfigLoad();
	void ConfigWrite();

	std::string PlayerName = "NoName";

	bool AutoPlayFlag = false;

	std::string ServerAddress = "localhost";
	int ServerPort = 8080;

	double HiddenLevel = 0;
	double SuddenLevel = 0;
	int RandomRate = 0;

	double JudgeGood = 25;
	double JudgeOk = 75;
	double JudgeBad = 100;

	double SongOffset = 0;
	double ChartSpeed = 1;
	double SongSpeed = 1;

	bool TrainingMode = false;
	bool ScreenFade = true;
	std::string SkinName = "Default";
	std::vector<std::string> SongDirectories{ "Songs" };

	float SongVolume = 62;
	float SEVolume = 70;

	bool HitNoteDisp = true;
	double RollSpeed = 20;

	bool WaitVSync = true;
	bool FastInput = true;
	double FastDrawRate = 0.5;

	bool FrameCountTimer = false;
	double FrameExtendRate = 1.0;
	int SoundDeviceType = 0; // None = 0, WASAPI = 1, XAudio = 2, MMEwaveOut = 3, ASIO = 4
	bool Exclusive = false;

	int SampleRate = 192000;
	int BufferSize = 480;

	bool FullScreen = false;
	bool DiscordSDK = true;
	bool ViewDebugData = false;
	bool MultiBoot = false;

	std::vector<int> KaInputLeft{ 'D','S',0,0 };
	std::vector<int> DonInputLeft{ 'F','G',0,0 };
	std::vector<int> DonInputRight{ 'J','H',0,0 };
	std::vector<int> KaInputRight{ 'K','L',0,0 };

	int Selector = 0;
	bool GenreFlag = true;
	bool GameFlag = false;
	bool KeyFlag = false;

	int Key = 0;

	int inputflag = 0;
	int inputint = 0;
	bool inputbool = false;
	double inputdouble = 0;
	float inputfloat = 0;
	std::string inputstring = "";
	std::vector<std::string> inputvector;

	int KeyInputHandle = 0;
	char InputHandle[1024];
	int KeyIndex = 0;

	std::string ConfigGenre[2]{
		"GameConfig",
		"KeyConfig",
	};

	std::string GameConfig[34]{
	"PlayerName",
	"AutoPlayFlag",
	"ServerAddress",
	"ServerPort",
	"HiddenLevel",
	"SuddenLevel",
	"RandomRate",
	"JudgeGood",
	"JudgeOk",
	"JudgeBad",
	"SongOffset",
	"ChartSpeed",
	"SongSpeed",
	"TrainingMode",
	"ScreenFade",
	"SkinName",
	"SongDirectories",
	"SongVolume",
	"SEVolume",
	"HitNoteDisp",
	"RollSpeed",
	"WaitVSync",
	"FastInput",
	"FastDrawRate",
	"FrameCountTimer",
	"FrameExtendRate",
	"SoundDeviceType",
	"Exclusive",
	"SampleRate",
	"BufferSize",
	"FullScreen",
	"DiscordSDK",
	"ViewDebugData",
	"MultiBoot"
	};

	std::string KeyConfig[4]{
		"KaInputLeft",
		"DonInputLeft",
		"DonInputRight",
		"KaInputRight",
	};

	int GenreSize = sizeof(ConfigGenre) / sizeof(ConfigGenre[0]);
	int GameSize = sizeof(GameConfig) / sizeof(GameConfig[0]);
	int KeySize = sizeof(KeyConfig) / sizeof(KeyConfig[0]);

	void ConvertVal(std::string str) {

		inputvector.clear();

		if (str.find(',') != std::string::npos) {
			inputvector = split(str, ',');
		}
		else {
			try {
				inputint = std::stoi(str);
				inputdouble = std::stod(str);
				inputfloat = std::stof(str);
			}
			catch (...) {
				inputbool = str == "true" ? true : str == "false" ? false : inputbool;
				inputstring = str.empty() ? inputstring : str;
				inputvector.push_back(str);
			}
		}
	}

	void DrawConfigData(int& i, std::string data);

	void DrawKeyData(int& i, int selector, int data);
	void AllDrawKeyData(int& i, int selector, std::vector<int> data);

	template<typename T, typename I>
	void ProcConfigData(int& i, T& data, I& input) {
		if (Selector == i) {
			if (inputflag == 1) {
				input = data;
			}
			if (inputflag == 2) {
				data = input;
			}
		}
		i++;
	}

	void ProcKeyData(int& i, int& data, int& input) {
		if (Selector == i) {
			if (inputflag == 1) {
				input = data;
			}
			if (inputflag == 2) {
				data = input;
			}
		}
		i++;
	}

	std::string GetKeyName(DWORD vkCode) {
		UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);

		switch (vkCode) {
		case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
		case VK_PRIOR: case VK_NEXT: case VK_END: case VK_HOME:
		case VK_INSERT: case VK_DELETE: case VK_DIVIDE:
		case VK_NUMLOCK:
			scanCode |= KF_EXTENDED;
			break;
		}

		char keyName[128];
		if (GetKeyNameTextA((scanCode << 16), keyName, sizeof(keyName)) > 0) {
			return std::string(keyName);
		}
		return "*";
	}
};

