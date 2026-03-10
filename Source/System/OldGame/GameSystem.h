#pragma once
#include "Include.hpp"

#include "Config.h"
#include "Skin.h"

#include "Loading.h"
#include "Playing.h"
#include "Result.h"
#include "DanSelect.h"
#include "SongSelect.h"
#include "Title.h"

#include "MultiRoom.h"

#define STRUCTDEF(name) \
void name##Init();\
void name##End();\
void name##Draw();\
void name##Proc();\
_##name name = _##name(this);\

enum class Scene {
	Title,
	SongSelect,
	DanSelect,
	Loading,
	Playing,
	Result,
	MultiRoom,
	Config,
	Skin
};

enum class DataType : int {
	Private,
	Public,
	Chart,
	List,
	Host,
	Reset
};

struct GameOption {
	int Random = 0;
	double Hidden = 0;
	double Sudden = 0;
	double Good = 0;
	double Ok = 0;
	double Bad = 0;
	double ChartSpeed = 0;
	double SongSpeed = 0;
};

struct PlayerData {
	int Standby = 0;
	std::vector<NoteData> RawNoteDatas = std::vector<NoteData>();
	std::string Name = "\0";
	GameOption Option = GameOption();

	Packet::bytearray ToBytes() const {
		Packet::bytearray ret;
		Packet::StoreBytes(ret, Standby);
		Packet::StoreBytes(ret, RawNoteDatas);
		Packet::StoreBytes(ret, Name);
		Packet::StoreBytes(ret, Option);
		return ret;
	}

	Packet::byte_view FromBytes(Packet::byte_view view) {
		Packet::LoadBytes(view, Standby);
		Packet::LoadBytes(view, RawNoteDatas);
		Packet::LoadBytes(view, Name);
		Packet::LoadBytes(view, Option);
		return view;
	}
};

struct PrivateData {
	int MyIndex = 0;
	int CountAll = 0;
	std::vector<PlayerData> PlayerDatas;

	Packet::bytearray ToBytes() const {
		Packet::bytearray ret;
		Packet::StoreBytes(ret, MyIndex);
		Packet::StoreBytes(ret, CountAll);
		Packet::StoreBytes(ret, PlayerDatas);
		return ret;
	}

	Packet::byte_view FromBytes(Packet::byte_view view) {
		Packet::LoadBytes(view, MyIndex);
		Packet::LoadBytes(view, CountAll);
		Packet::LoadBytes(view, PlayerDatas);
		return view;
	}
};

struct PublicData {
	int GetIndex = 0;
	HitType HitKey = HitType::Null;
	JudgeData Judge = JudgeData();
};

class GameSystem {
public:

	GameSystem();
	~GameSystem();

	STRUCTDEF(Config);
	STRUCTDEF(Skin);

	STRUCTDEF(Title);
	STRUCTDEF(SongSelect);
	STRUCTDEF(DanSelect);
	STRUCTDEF(Loading);
	STRUCTDEF(Playing);
	STRUCTDEF(Result);

	STRUCTDEF(MultiRoom);

	void Draw();
	void Proc();

	Scene NowScene = Scene::Title;
	Scene MemScene = Scene::Config;
	Scene PrevScene = Scene::Title;

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
	time_t UnixTime;

	Timer<millisecond> FadeTimer = Timer<millisecond>(true);
	double ScreenFadeTime = 300;

	PublicData Public;
	PrivateData Private;
	std::vector<PlayerData> MultiDatas;

	AES128 encoder;
	AES128::cbytearray<16> key;	

	void TextToKeyInit() {

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
			key[i] = sp[i][0];
		}
	}

	void SetState(std::string state);

	void ScreenFade() {

		double alpha = GetEasingRate(FadeTimer.GetRecordingTime() / ScreenFadeTime, ease::Base::In, ease::Line::Linear);

		if (!Config.ScreenFade || alpha > 1.0) {
			FadeTimer.End();
			return;
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * (1 - alpha));
		DrawBox(0, 0, Skin.Info.Resolution.X, Skin.Info.Resolution.Y, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(0, 0);
	}

	template<typename T>
	void Send(DataType type, T data) {
		MultiRoom.server.ASyncEncryptionSend(Packet(type, data));
		Public = PublicData();
	}

	void Recv() {

		PlayerData x;
		PrivateData data;
		Public = PublicData();

		if (MultiRoom.server.Available() > 0) {

			auto&& pak = MultiRoom.server.ASyncEncryptionRecv().get();
			auto type = pak->GetHeader()->TypeAs<DataType>();

			switch (type) {
			case DataType::Private:
				data = *pak->Get<PrivateData>();
				if (Private.CountAll != data.CountAll) {
					Private.PlayerDatas.resize(data.CountAll);
					Private.CountAll = data.CountAll;
					Private.MyIndex = data.MyIndex;
				}
				Private.PlayerDatas = data.PlayerDatas;
				MultiDatas = Private.PlayerDatas;
				x = std::move(MultiDatas[Private.MyIndex]);
				MultiDatas.erase(MultiDatas.begin() + Private.MyIndex);
				MultiDatas.insert(MultiDatas.begin(), std::move(x));
				break;
			case DataType::Public:
				Public = *pak->Get<PublicData>();
				break;
			case DataType::Chart:
				Playing.Chart.OriginalData = *pak->Get<ChartData>();
				break;
			}
		}
	}

	bool CheckStandby(std::vector<PlayerData> data, int val) {
		for (auto d : data) {
			if ((d.Standby % MultiRoom.HostVal) != val) {
				return false;
			}
		}
		return true;
	}
};