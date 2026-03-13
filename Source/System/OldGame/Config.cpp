#include "Config.h"
#include "GameSystem.h"

GameSystem* configptr;

_Config::_Config(GameSystem* ptr) {
	ConfigLoad();
	::configptr = ptr;
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

#define JSONDATA(name) name = data.value(#name, name)\

	JSONDATA(PlayerName);
	JSONDATA(AutoPlayFlag);
	JSONDATA(ServerAddress);
	JSONDATA(ServerPort);
	JSONDATA(HiddenLevel);
	JSONDATA(SuddenLevel);
	JSONDATA(RandomRate);
	JSONDATA(JudgeGood);
	JSONDATA(JudgeOk);
	JSONDATA(JudgeBad);
	JSONDATA(SongOffset);
	JSONDATA(ChartSpeed);
	JSONDATA(SongSpeed);
	JSONDATA(TrainingMode);
	JSONDATA(ScreenFade);
	JSONDATA(SkinName);
	JSONDATA(SongDirectories);
	JSONDATA(SongVolume);
	JSONDATA(SEVolume);
	JSONDATA(HitNoteDisp);
	JSONDATA(RollSpeed);
	JSONDATA(WaitVSync);
	JSONDATA(FastInput);
	JSONDATA(FastDrawRate);
	JSONDATA(FrameCountTimer);
	JSONDATA(FrameExtendRate);
	JSONDATA(SoundDeviceType);
	JSONDATA(Exclusive);
	JSONDATA(SampleRate);
	JSONDATA(BufferSize);
	JSONDATA(FullScreen);
	JSONDATA(DiscordSDK);
	JSONDATA(ViewDebugData);
	JSONDATA(MultiBoot);
	JSONDATA(KaInputLeft);
	JSONDATA(DonInputLeft);
	JSONDATA(DonInputRight);
	JSONDATA(KaInputRight);

#undef JSONDATA

	//KeyCodeParser(false);

	ifs.close();
}

void _Config::ConfigWrite() {

#define JSONDATA(name) {#name, name}
	nlohmann::ordered_json data = {
		JSONDATA(PlayerName),
		JSONDATA(AutoPlayFlag),
		JSONDATA(ServerAddress),
		JSONDATA(ServerPort),
		JSONDATA(HiddenLevel),
		JSONDATA(SuddenLevel),
		JSONDATA(RandomRate),
		JSONDATA(JudgeGood),
		JSONDATA(JudgeOk),
		JSONDATA(JudgeBad),
		JSONDATA(SongOffset),
		JSONDATA(ChartSpeed),
		JSONDATA(SongSpeed),
		JSONDATA(TrainingMode),
		JSONDATA(ScreenFade),
		JSONDATA(SkinName),
		JSONDATA(SongDirectories),
		JSONDATA(SongVolume),
		JSONDATA(SEVolume),
		JSONDATA(HitNoteDisp),
		JSONDATA(RollSpeed),
		JSONDATA(WaitVSync),
		JSONDATA(FastInput),
		JSONDATA(FastDrawRate),
		JSONDATA(FrameCountTimer),
		JSONDATA(FrameExtendRate),
		JSONDATA(SoundDeviceType),
		JSONDATA(Exclusive),
		JSONDATA(SampleRate),
		JSONDATA(BufferSize),
		JSONDATA(FullScreen),
		JSONDATA(DiscordSDK),
		JSONDATA(ViewDebugData),
		JSONDATA(MultiBoot),
		JSONDATA(KaInputLeft),
		JSONDATA(DonInputLeft),
		JSONDATA(DonInputRight),
		JSONDATA(KaInputRight),
	};
#undef JSONDATA

	std::ofstream ofs("config.json");

	ofs << data.dump(4) << "\n";

	ofs.close();
}

void GameSystem::ConfigInit() {

	SetState("Config");

}

void GameSystem::ConfigEnd() {

}

void GameSystem::ConfigDraw() {

	Skin.Base->SongSelect.Image.BackGround.Draw({0, 0});

	unsigned int c = GetColor(255, 255, 255);

	Skin.Base->Other.Font.Game.DrawFontString({ 0,360 }, ">");
	for (int i = 0; i < Config.GenreSize; ++i) {
		float y = (32 * (i - Config.Selector)) + 360;
		if (!Config.GenreFlag) { y = 32 * (i - Config.KeyFlag) + 360; }
		if (MultiRoom.MultiFlag && i == 2) { c = GetColor(0, 0, 0); }
		Skin.Base->Other.Font.Game.DrawFontString({ 16,y }, Config.ConfigGenre[i], c);
	}
	if (!Config.GenreFlag) {

		if (Config.GameFlag) {

			Skin.Base->Other.Font.Game.DrawFontString({ 128,360 }, ">");
			for (int i = 0; i < Config.GameSize; ++i) {
				c = GetColor(255, 255, 255);
				float y = (32 * (i - Config.Selector)) + 360;
				if (MultiRoom.MultiFlag && !(i == 1 || i >= 4 && i <= 12)) { c = GetColor(0, 0, 0); }
				Skin.Base->Other.Font.Game.DrawFontString({ 144,y }, Config.GameConfig[i], c);
			}

			if (CheckKeyInput(Config.KeyInputHandle) == 0) {
				DrawKeyInputString(370, 320, Config.KeyInputHandle);
			}

			int i = 0;

			Config.DrawConfigData(configptr, i, Config.PlayerName);
			Config.DrawConfigData(configptr, i, Config.AutoPlayFlag ? "true" : "false");
			Config.DrawConfigData(configptr, i, Config.ServerAddress);
			Config.DrawConfigData(configptr, i, std::to_string(Config.ServerPort));
			Config.DrawConfigData(configptr, i, std::to_string(Config.HiddenLevel));
			Config.DrawConfigData(configptr, i, std::to_string(Config.SuddenLevel));
			Config.DrawConfigData(configptr, i, std::to_string(Config.RandomRate));
			Config.DrawConfigData(configptr, i, std::to_string(Config.JudgeGood));
			Config.DrawConfigData(configptr, i, std::to_string(Config.JudgeOk));
			Config.DrawConfigData(configptr, i, std::to_string(Config.JudgeBad));
			Config.DrawConfigData(configptr, i, std::to_string(Config.SongOffset));
			Config.DrawConfigData(configptr, i, std::to_string(Config.ChartSpeed));
			Config.DrawConfigData(configptr, i, std::to_string(Config.SongSpeed));
			Config.DrawConfigData(configptr, i, Config.TrainingMode ? "true" : "false");
			Config.DrawConfigData(configptr, i, Config.ScreenFade ? "true" : "false");
			Config.DrawConfigData(configptr, i, Config.SkinName);
			if (MultiRoom.MultiFlag) { c = GetColor(0, 0, 0); }
			if (Config.Selector == i) {
				for (int j = 0; j < Config.SongDirectories.size(); j++) {
					Skin.Base->Other.Font.Game.DrawFontString({ 370, 360 + 32 * (float)j }, Config.SongDirectories[j], c);
				}
			}
			i++;
			Config.DrawConfigData(configptr, i, std::to_string(Config.SongVolume));
			Config.DrawConfigData(configptr, i, std::to_string(Config.SEVolume));
			Config.DrawConfigData(configptr, i, Config.HitNoteDisp ? "true" : "false");
			Config.DrawConfigData(configptr, i, std::to_string(Config.RollSpeed));
			Config.DrawConfigData(configptr, i, Config.WaitVSync ? "true" : "false");
			Config.DrawConfigData(configptr, i, Config.FastInput ? "true" : "false");
			Config.DrawConfigData(configptr, i, std::to_string(Config.FastDrawRate));
			Config.DrawConfigData(configptr, i, Config.FrameCountTimer ? "true" : "false");
			Config.DrawConfigData(configptr, i, std::to_string(Config.FrameExtendRate));
			Config.DrawConfigData(configptr, i, std::to_string(Config.SoundDeviceType));
			Config.DrawConfigData(configptr, i, Config.Exclusive ? "true" : "false");
			Config.DrawConfigData(configptr, i, std::to_string(Config.SampleRate));
			Config.DrawConfigData(configptr, i, std::to_string(Config.BufferSize));
			Config.DrawConfigData(configptr, i, Config.FullScreen ? "true" : "false");
			Config.DrawConfigData(configptr, i, Config.DiscordSDK ? "true" : "false");
			Config.DrawConfigData(configptr, i, Config.ViewDebugData ? "true" : "false");
			Config.DrawConfigData(configptr, i, Config.MultiBoot ? "true" : "false");

		}

		if (Config.KeyFlag) {

			Skin.Base->Other.Font.Game.DrawFontString({ 128,360 }, ">");
			for (int i = 0; i < Config.KeySize; ++i) {
				float y = (32 * (i - Config.Selector)) + 360;
				Skin.Base->Other.Font.Game.DrawFontString({ 144, y }, Config.KeyConfig[i]);
			}

			int s = 0;

			Config.AllDrawKeyData(s, Config.Selector, Config.KaInputLeft);
			Config.AllDrawKeyData(s, Config.Selector, Config.DonInputLeft);
			Config.AllDrawKeyData(s, Config.Selector, Config.DonInputRight);
			Config.AllDrawKeyData(s, Config.Selector, Config.KaInputRight);

			int i = 0;

			Config.DrawKeyData(i, Config.Selector, Config.KaInputLeft[Config.KeyIndex]);
			Config.DrawKeyData(i, Config.Selector, Config.DonInputLeft[Config.KeyIndex]);
			Config.DrawKeyData(i, Config.Selector, Config.DonInputRight[Config.KeyIndex]);
			Config.DrawKeyData(i, Config.Selector, Config.KaInputRight[Config.KeyIndex]);

			if (Config.inputflag) {
				Skin.Base->Other.Font.Game.DrawFontString({ 580,360 }, "割り当てるキーを押してください");
			}
		}
	}
}

void GameSystem::ConfigProc() {

	if (Config.inputflag == 0) {

		Input.HitKeyProcess(VK_ESCAPE, KeyState::Down, [&] {
			if (Config.GenreFlag) {
				NowScene = PrevScene;
			}
			else {
				Config.Selector = Config.KeyFlag;
				Config.GenreFlag = true;
				Config.GameFlag = false;
				Config.KeyFlag = false;
			}
			});

		int Max = 0;
		if (Config.GenreFlag) {
			Max = Config.GenreSize;
		}
		if (Config.GameFlag) {
			Max = Config.GameSize;
		}
		if (Config.KeyFlag) {
			Max = Config.KeySize;
		}

		Input.HitKeyProcess(VK_UP, KeyState::Down, [&] {
			Skin.Base->Title.SE.Ka.Play();
			Config.Selector <= 0 ? 0 : Config.Selector--;
			});
		Input.HitKeyProcess(VK_DOWN, KeyState::Down, [&] {
			Skin.Base->Title.SE.Ka.Play();
			Config.Selector >= Max - 1 ? Max : Config.Selector++;
			});

		if (Config.KeyFlag) {
			Input.HitKeyProcess(VK_LEFT, KeyState::Down, [&] {
				Skin.Base->Title.SE.Ka.Play();
				Config.KeyIndex <= 0 ? 0 : Config.KeyIndex--;
				});
			Input.HitKeyProcess(VK_RIGHT, KeyState::Down, [&] {
				Skin.Base->Title.SE.Ka.Play();
				Config.KeyIndex >= 4 - 1 ? 4 : Config.KeyIndex++;
				});
		}

	}

	if (Config.inputflag == 1) {

		if (Config.GameFlag) {
			if (CheckKeyInput(Config.KeyInputHandle) != 0) {
				GetKeyInputString(Config.InputHandle, Config.KeyInputHandle);
				Config.ConvertVal(Config.InputHandle);
				Config.inputflag = 2;
			}
		}

		if (Config.KeyFlag) {

			for (int i = 0; i < 256; i++) {

				Input.HitKeyProcess(i, KeyState::Down, [&] {

					Config.Key = i;

					switch (Config.Key) {
					case 27:
					case 37:
					case 38:
					case 39:
					case 40:
						Config.Key = 0;
						break;
					case 13:
						Config.Key = 0;
						Config.inputflag = 2;
						Skin.Base->Title.SE.Don.Play();
						break;
					default:
						Config.inputflag = 2;
						Skin.Base->Title.SE.Don.Play();
						break;
					}
					});
			}
		}
	}

	Input.HitKeyProcess(VK_RETURN, KeyState::Down, [&] {

		Skin.Base->Title.SE.Don.Play();

		if (Config.GenreFlag) {

			if (!MultiRoom.MultiFlag || Config.Selector != 2) {

				switch (Config.Selector) {
				case 0:
					Config.GenreFlag = false;
					Config.GameFlag = true;
					break;
				case 1:
					Config.GenreFlag = false;
					Config.KeyFlag = true;
					break;
				}

				Config.Selector = 0;

			}
		}
		else if (Config.inputflag == 0) {

			if (Config.GameFlag) {
				if (!MultiRoom.MultiFlag || (Config.Selector == 1 || Config.Selector >= 4 && Config.Selector <= 12)) {
					Config.inputflag = 1;
					Config.KeyInputHandle = MakeKeyInput(255, false, true, false);
					SetActiveKeyInput(Config.KeyInputHandle);
				}
			}

			if (Config.KeyFlag) {
				Config.inputflag = 1;
				Config.Key = 0;
			}
		}
		});

	if (!Config.GenreFlag) {

		if (Config.inputflag >= 1) {

			if (Config.GameFlag) {

				int i = 0;

				Config.ProcConfigData(i, Config.PlayerName, Config.inputstring);
				Config.ProcConfigData(i, Config.AutoPlayFlag, Config.inputbool);
				Config.ProcConfigData(i, Config.ServerAddress, Config.inputstring);
				Config.ProcConfigData(i, Config.ServerPort, Config.inputint);
				Config.ProcConfigData(i, Config.HiddenLevel, Config.inputdouble);
				Config.ProcConfigData(i, Config.SuddenLevel, Config.inputdouble);
				Config.ProcConfigData(i, Config.RandomRate, Config.inputint);
				Config.ProcConfigData(i, Config.JudgeGood, Config.inputdouble);
				Config.ProcConfigData(i, Config.JudgeOk, Config.inputdouble);
				Config.ProcConfigData(i, Config.JudgeBad, Config.inputdouble);
				Config.ProcConfigData(i, Config.SongOffset, Config.inputdouble);
				Config.ProcConfigData(i, Config.ChartSpeed, Config.inputdouble);
				Config.ProcConfigData(i, Config.SongSpeed, Config.inputdouble);
				Config.ProcConfigData(i, Config.TrainingMode, Config.inputbool);
				Config.ProcConfigData(i, Config.ScreenFade, Config.inputbool);
				Config.ProcConfigData(i, Config.SkinName, Config.inputstring);
				if (Config.Selector == i) {
					if (Config.inputflag)  {
						Config.SongDirectories.clear();
						for (auto&& elem : Config.inputvector) {
							Config.SongDirectories.push_back(elem);
						}
					}
					else {
						Config.inputstring = Config.SongDirectories[0];
					}
				}
				i++;
				Config.ProcConfigData(i, Config.SongVolume, Config.inputfloat);
				Config.ProcConfigData(i, Config.SEVolume, Config.inputfloat);
				Config.ProcConfigData(i, Config.HitNoteDisp, Config.inputbool);
				Config.ProcConfigData(i, Config.RollSpeed, Config.inputdouble);
				Config.ProcConfigData(i, Config.WaitVSync, Config.inputbool);
				Config.ProcConfigData(i, Config.FastInput, Config.inputbool);
				Config.ProcConfigData(i, Config.FastDrawRate, Config.inputdouble);
				Config.ProcConfigData(i, Config.FrameCountTimer, Config.inputbool);
				Config.ProcConfigData(i, Config.FrameExtendRate, Config.inputdouble);
				Config.ProcConfigData(i, Config.SoundDeviceType, Config.inputint);
				Config.ProcConfigData(i, Config.Exclusive, Config.inputbool);
				Config.ProcConfigData(i, Config.SampleRate, Config.inputint);
				Config.ProcConfigData(i, Config.BufferSize, Config.inputint);
				Config.ProcConfigData(i, Config.FullScreen, Config.inputbool);
				Config.ProcConfigData(i, Config.DiscordSDK, Config.inputbool);
				Config.ProcConfigData(i, Config.ViewDebugData, Config.inputbool);
				Config.ProcConfigData(i, Config.MultiBoot, Config.inputbool);
			}

			if (Config.KeyFlag) {

				int i = 0;

				Config.ProcKeyData(i, Config.KaInputLeft[Config.KeyIndex], Config.Key);
				Config.ProcKeyData(i, Config.DonInputLeft[Config.KeyIndex], Config.Key);
				Config.ProcKeyData(i, Config.DonInputRight[Config.KeyIndex], Config.Key);
				Config.ProcKeyData(i, Config.KaInputRight[Config.KeyIndex], Config.Key);
			}
		}

		if (Config.inputflag == 2) {

			Config.ConfigWrite();
			Config.Key = 0;
			Config.inputflag = 0;
			DeleteKeyInput(Config.KeyInputHandle);

			if (MultiRoom.MultiFlag) {

				auto&& data = Private.PlayerDatas[Private.MyIndex];

				data.Option.Random = Config.RandomRate;
				data.Option.Hidden = Config.HiddenLevel;
				data.Option.Sudden = Config.SuddenLevel;
				data.Option.Good = Config.JudgeGood;
				data.Option.Ok = Config.JudgeOk;
				data.Option.Bad = Config.JudgeBad;
				data.Option.ChartSpeed = Config.ChartSpeed;
				data.Option.SongSpeed = Config.SongSpeed;

				Send(DataType::List, data);
			}
		}
	}
}

void _Config::DrawKeyData(int& i, int selector, int data) {
	unsigned int c = GetColor(255, 255, 255);
	if (Selector == i) { c = GetColor((1 - inputflag) * 255, (1 - inputflag) * 255, inputflag * 255); }
	configptr->Skin.Base->Other.Font.Game.DrawFontString({ 64 * (float)KeyIndex + 320, (32 * ((float)i - selector)) + 360 }, data ? GetKeyStr(data) : "*", c);
	i++;
}

void _Config::AllDrawKeyData(int& i, int selector, std::vector<int> data) {

	for (int x = 0; x < data.size(); x++) {
		unsigned int c = GetColor(255, 255, 255);
		if (x == KeyIndex && Selector == i) { c = GetColor((1 - inputflag) * 255, (1 - inputflag) * 255, 255); }
		configptr->Skin.Base->Other.Font.Game.DrawFontString({ 64 * (float)x + 320, (32 * ((float)i - selector)) + 360 }, data[x] ? GetKeyStr(data[x]) : "*", c);
	}
	i++;
}