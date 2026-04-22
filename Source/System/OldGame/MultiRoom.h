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

	_Skin* __SkinPtr = nullptr;
	_Config* __ConfigPtr = nullptr;

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
		Once = false;
	}

	bool Once = false;

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

	void ChartStrDraw(_SongSelect* SongSelect, ChartData* ChartData) {

		Pos2D<float> pos = {
		__SkinPtr->Base->SongSelect.Image.TitleBox.Pos.X,
		__SkinPtr->Base->SongSelect.Image.TitleBox.Pos.Y
		};

		__SkinPtr->Base->SongSelect.Font.BoxTitle.Draw(
			{ pos.X, pos.Y - 40 },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			ChartData->Title
		);
		__SkinPtr->Base->SongSelect.Font.BoxSubTitle.Draw(
			{ pos.X, pos.Y + 10 },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			ChartData->SubTitle
		);

		std::string levelstr = SongSelect->CourseList[ChartData->CourseIndex] + 
			"  ★×" + std::to_string(ChartData->CourseDatas[ChartData->CourseIndex].Level);

		__SkinPtr->Base->SongSelect.Font.Course.Draw(
			{ pos.X - 100.0f, pos.Y + 50 },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			levelstr
		);
	}

	template<typename T>
	void PlayerDatasDraw(T& Private) {

		static auto OptionDraw = [&](Pos2D<float> pos, std::string str, bool flg) {

			if (flg && str.find('.') != std::string::npos) { str = str.substr(0, str.length() - 3); }

			__SkinPtr->Base->MultiRoom.Font.OptionData.Draw(
				pos,
				GetColor(255, 255, 255),
				GetColor(0, 0, 0),
				GetStrlen(str.c_str(), __SkinPtr->Base->MultiRoom.Font.OptionData.Handle),
				str);
			};

		for (int i = 0; i < Private.PlayerDatas.size(); i++) {

			auto data = Private.PlayerDatas[i];
			float y = 100.0f * i;
			bool a = 1 - (GrantIndex == i) * HostSelectMode;
			bool b = 1 - data.Standby % HostVal;

			__SkinPtr->Base->MultiRoom.Image.PlayersBox.Draw({ 0, y });

			__SkinPtr->Base->MultiRoom.Font.PlayerName.Draw({
				__SkinPtr->Base->MultiRoom.Config.PlayerNamePos.X,
				__SkinPtr->Base->MultiRoom.Config.PlayerNamePos.Y + y },
				GetColor(255 * a, 255 * a, 255 * b),
				GetColor(0, 0, 0),
				GetStrlen(data.Name, __SkinPtr->Base->MultiRoom.Font.PlayerName.Handle),
				data.Name
				);

			if (data.Standby >= HostVal) {
				__SkinPtr->Base->MultiRoom.Image.Crown.Draw({ 0, y }, 3);
			}

#define DRAWOPTION(Name, index) \
        OptionDraw({__SkinPtr->Base->MultiRoom.Config.OptionDataPos.X[index], __SkinPtr->Base->MultiRoom.Config.OptionDataPos.Y[index] + y - 10.0f}, #Name, FALSE);\
		OptionDraw({__SkinPtr->Base->MultiRoom.Config.OptionDataPos.X[index], __SkinPtr->Base->MultiRoom.Config.OptionDataPos.Y[index] + y + 10.0f}, std::to_string(data.Option.Name), TRUE);\

			DRAWOPTION(Hidden,     0)
			DRAWOPTION(Sudden,     1)
			DRAWOPTION(Random,     2)
			DRAWOPTION(Good,       3)
			DRAWOPTION(Ok,         4)
			DRAWOPTION(Bad,        5)
			DRAWOPTION(ChartSpeed, 6)

				if (data.Standby >= HostVal) {
					DRAWOPTION(SongSpeed, 7)
				}
		}
#undef DRAWOPTION
	}

	template<typename T>
	bool ConnectTry(T&& data) {

		MultiFlag = server.Connect(IPAddress::SolveHostName(ConnectAddress)->Port(ConnectPort));

		if (MultiFlag) {

			KeyInit();

			data.Name = __ConfigPtr->PlayerName;
			data.Option.Random = __ConfigPtr->RandomRate;
			data.Option.Hidden = __ConfigPtr->HiddenLevel;
			data.Option.Sudden = __ConfigPtr->SuddenLevel;
			data.Option.Good = __ConfigPtr->JudgeGood;
			data.Option.Ok = __ConfigPtr->JudgeOk;
			data.Option.Bad = __ConfigPtr->JudgeBad;
			data.Option.ChartSpeed = __ConfigPtr->ChartSpeed;
			data.Option.SongSpeed = __ConfigPtr->SongSpeed;

			return true;
		}

		return false;
	}
};