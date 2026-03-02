#pragma once
#include "Include.hpp"

extern class GameSystem;

class _Config {
public:

	_Config(GameSystem* ptr);
	~_Config();

	void ConfigLoad();
	void ConfigWrite();

	bool WaitVSync = true;
	bool FastInput = true;
	double FastDrawRate = 0.5;
	bool FullScreen = false;
	bool FrameCountTimer = false;
	bool DiscordRPC = false;
	int ChartChunkSize = 256;
	bool ViewDebugData = false;
	std::string SkinName = "Default";
	std::vector<std::string> SongDirectories{"Songs"};
	int SoundDeviceType = 0; // None = 0, WASAPI = 1, XAudio = 2, MMEwaveOut = 3, ASIO = 4
	bool Exclusive = false;
	int SampleRate = 192000;
	int BufferSize = 480;
	float SongVolume = 62;
	float SEVolume = 70;

	std::string MultiPlayName = "NoName";
	bool AutoPlayFlag = true;
	bool HitNoteDisp = true;
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

	std::vector<int> KaInputLeft{'D', 'S'};
	std::vector<int> DonInputLeft{'F', 'G'};
	std::vector<int> DonInputRight{'J', 'H'};
	std::vector<int> KaInputRight{'K', 'L'};

	// True=Dx->Sys False=Sys->Dx
	void KeyCodeParser(bool flag);

};
