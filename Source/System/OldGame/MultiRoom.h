#pragma once
#include "Include.hpp"
#include "Cryptgraphy/KeyManager.h"
#include "SongSelect.h"
#include "Config.h"
#include "Skin.h"
#include <curl/curl.h>

extern class GameSystem;

class _MultiRoom {
public:

	_MultiRoom(GameSystem* ptr);
	~_MultiRoom();

	void Init() {
		server.Close();
		GrantIndex = 0;
		IsHost = false;
		IsStandby = false;
		IsSelected = false;
		HostSelectMode = false;
		MultiFlag = false;
		InviteFlag = false;
		ConnectAddress = "";
		ConnectPort = 8080;
	}

	TCPSocket server = TCPSocket();

	std::string ConnectAddress = "";
	int ConnectPort = 8080;

	int GrantIndex = 0;
	int HostVal = 4;

	bool MultiFlag = false;
	bool InviteFlag = false;
	bool IsHost = false;
	bool IsStandby = false;
	bool IsSelected = false;
	bool HostSelectMode = false;

	void OptionDraw(FontData font, Pos2D<float> pos, std::string str, bool flg) {

		if (flg && str.find('.') != std::string::npos) { str = str.substr(0, str.length() - 3); }

		font.Draw(
			pos,
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			GetDrawStringWidthToHandle(str.c_str(), strlenDx(str.c_str()), font.Handle),
			str);

	}

	void KeyInit() {

		KeyManager key;
		std::string message = "I have skill is write low level programing language.";

		auto q = ECDSA::MakePublicKey(key.GetSecretKey());
		auto v = ECDSA::Sign(key.GetSecretKey(), { message.begin(), message.end() });
		bool ret = ECDSA::Verify(q, v, { message.begin(), message.end() });

		AES128::cbytearray<16> sharedkey;
		
		HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_TEXT1), "TEXT");
		if (!hRes) return;

		HGLOBAL hData = LoadResource(NULL, hRes);
		if (!hData) return;

		DWORD size = SizeofResource(NULL, hRes);
		if (size == 0) return;

		char* pData = static_cast<char*>(LockResource(hData));
		if (!pData) return;

		std::string text(pData, size);

		auto sp = split(text, ',');
		for (int i = 0; i < sp.size(); i++) {
			sharedkey[i] = sp[i][0];
		}

		server.CryptEngine.Init(sharedkey);
	}

	void ChartStrDraw(_Skin* Skin, _SongSelect* SongSelect, ChartData* ChartData) {

		Pos2D<float> pos = {
		Skin->Base->SongSelect.Image.TitleBox.Pos.X,
		Skin->Base->SongSelect.Image.TitleBox.Pos.Y
		};

		Skin->Base->SongSelect.Font.BoxTitle.Draw(
			{ pos.X, pos.Y - 40 },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			ChartData->Title
		);
		Skin->Base->SongSelect.Font.BoxSubTitle.Draw(
			{ pos.X, pos.Y + 10 },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			ChartData->SubTitle
		);

		std::string levelstr = SongSelect->CourseList[ChartData->CourseIndex] + 
			"  ★×" + std::to_string(ChartData->CourseDatas[ChartData->CourseIndex].Level);

		Skin->Base->SongSelect.Font.Course.Draw(
			{ pos.X - 100.0f, pos.Y + 50 },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			levelstr
		);
	}

	template<typename T>
	void PlayerDatasDraw(_Skin* Skin, T& Private) {

		for (int i = 0; i < Private.PlayerDatas.size(); i++) {

			auto data = Private.PlayerDatas[i];
			float y = 100.0f * i;
			bool a = 1 - (GrantIndex == i) * HostSelectMode;
			bool b = 1 - data.Standby % HostVal;

			Skin->Base->MultiRoom.Image.PlayersBox.Draw({ 0, y });

			Skin->Base->MultiRoom.Font.PlayerName.Draw({
				Skin->Base->MultiRoom.Config.PlayerNamePos.X,
				Skin->Base->MultiRoom.Config.PlayerNamePos.Y + y },
				GetColor(255 * a, 255 * a, 255 * b),
				GetColor(0, 0, 0),
				GetStrlen(data.Name, Skin->Base->MultiRoom.Font.PlayerName.Handle),
				data.Name
				);

			if (data.Standby >= HostVal) {
				Skin->Base->MultiRoom.Image.Crown.Draw({ 0, y }, 3);
			}

#define DRAWOPTION(Name) \
        OptionDraw(Skin->Base->MultiRoom.Font.OptionData, {Skin->Base->MultiRoom.Config.Name##Pos.X, Skin->Base->MultiRoom.Config.Name##Pos.Y + y - 10.0f}, #Name, FALSE);\
		OptionDraw(Skin->Base->MultiRoom.Font.OptionData, {Skin->Base->MultiRoom.Config.Name##Pos.X, Skin->Base->MultiRoom.Config.Name##Pos.Y + y + 10.0f}, std::to_string(data.Option.Name), TRUE);

			    DRAWOPTION(Hidden)
				DRAWOPTION(Sudden)
				DRAWOPTION(Random)
				DRAWOPTION(Good)
				DRAWOPTION(Ok)
				DRAWOPTION(Bad)
				DRAWOPTION(ChartSpeed)

				if (data.Standby >= HostVal) {
					DRAWOPTION(SongSpeed)
				}
		}
#undef DRAWOPTION
	}

	template<typename T>
	bool ConnectProc(_Config* Config, T& data) {

		MultiFlag = server.Connect(IPAddress::SolveHostName(ConnectAddress)->Port(ConnectPort));

		if (MultiFlag) {

			KeyInit();

			data.Name = Config->PlayerName;
			data.Option.Random = Config->RandomRate;
			data.Option.Hidden = Config->HiddenLevel;
			data.Option.Sudden = Config->SuddenLevel;
			data.Option.Good = Config->JudgeGood;
			data.Option.Ok = Config->JudgeOk;
			data.Option.Bad = Config->JudgeBad;
			data.Option.ChartSpeed = Config->ChartSpeed;
			data.Option.SongSpeed = Config->SongSpeed;

			return true;
		}

		return false;
	}
};