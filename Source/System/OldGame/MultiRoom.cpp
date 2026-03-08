#include "MultiRoom.h"
#include "GameSystem.h"

_MultiRoom::_MultiRoom(GameSystem* ptr) {

}

_MultiRoom::~_MultiRoom() {

}

void GameSystem::MultiRoomInit() {

	SetState("MultiRoom");

	if (!MultiRoom.MultiFlag) {

		MultiRoom.server = TCPSocket();

		if (!MultiRoom.InviteFlag) {

			MultiRoom.ConnectAddress = Config.ServerAddress;
			MultiRoom.ConnectPort = Config.ServerPort;

		}
	}
}

void GameSystem::MultiRoomEnd() {

	SongSelect.DemoSong.Delete();
	SongSelect.DemoSongPlayBlank.End();

}

void GameSystem::MultiRoomDraw() {

	Skin.Base->MultiRoom.Image.BackGround.Draw({ 0, 0 });

	if (!MultiRoom.MultiFlag) {
		Skin.Base->Other.Font.Game.DrawFontString({ 0,8 }, "サーバーに接続中…");
		return;
	}

	Skin.Base->MultiRoom.Image.TitleBox.Draw({ 0,0 });

	if (!Playing.Chart.OriginalData.WaveData.empty()) {
		Pos2D<float> pos = {
				Skin.Base->SongSelect.Image.TitleBox.Pos.X,
				Skin.Base->SongSelect.Image.TitleBox.Pos.Y
		};

		Skin.Base->SongSelect.Font.BoxTitle.Draw(
			{ pos.X, pos.Y - 40 },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			Playing.Chart.OriginalData.Title
		);
		Skin.Base->SongSelect.Font.BoxSubTitle.Draw(
			{ pos.X, pos.Y + 10 },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			Playing.Chart.OriginalData.SubTitle
		);

		int levelval = Playing.Chart.OriginalData.CourseDatas[Playing.Chart.OriginalData.CourseIndex].Level;
		std::string levelstr = SongSelect.CourseList[Playing.Chart.OriginalData.CourseIndex] + "  ★×" + std::to_string(levelval);

		Skin.Base->SongSelect.Font.Course.Draw(
			{ pos.X - 100.0f, pos.Y + 50 },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			levelstr
		);
	}

	for (int i = 0; auto data : Private.PlayerDatas) {

		float y = 100.0f * i;
		bool a = 1 - (MultiRoom.GrantIndex == i) * MultiRoom.HostSelectMode;
		bool b = 1 - data.Standby % MultiRoom.HostVal;

		Skin.Base->MultiRoom.Image.PlayersBox.Draw({ 0, y });

		Skin.Base->MultiRoom.Font.PlayerName.Draw({
			Skin.Base->MultiRoom.Config.PlayerNamePos.X,
			Skin.Base->MultiRoom.Config.PlayerNamePos.Y + y },
			GetColor(255 * a, 255 * a, 255 * b),
			GetColor(0, 0, 0),
			GetStrlen(data.Name, Skin.Base->MultiRoom.Font.PlayerName.Handle),
			data.Name
			);

		if (data.Standby >= MultiRoom.HostVal) {
			Skin.Base->MultiRoom.Image.Crown.Draw({ 0, y }, 3);
		}

#define DRAWOPTION(Name) \
        MultiRoom.OptionDraw(Skin.Base->MultiRoom.Font.OptionData, {Skin.Base->MultiRoom.Config.Name##Pos.X, Skin.Base->MultiRoom.Config.Name##Pos.Y + y - 10.0f}, #Name, FALSE);\
		MultiRoom.OptionDraw(Skin.Base->MultiRoom.Font.OptionData, {Skin.Base->MultiRoom.Config.Name##Pos.X, Skin.Base->MultiRoom.Config.Name##Pos.Y + y + 10.0f}, std::to_string(data.Option.Name), TRUE);

		DRAWOPTION(Hidden)
			DRAWOPTION(Sudden)
			DRAWOPTION(Random)
			DRAWOPTION(Good)
			DRAWOPTION(Ok)
			DRAWOPTION(Bad)
			DRAWOPTION(ChartSpeed)

			if (data.Standby >= MultiRoom.HostVal) {
				DRAWOPTION(SongSpeed)
			}

		i++;
	}
}

void GameSystem::MultiRoomProc() {

	Input.HitKeyProcess(VK_ESCAPE, KeyState::Down, [&] {
		if (MultiRoom.IsStandby) {
			Private.PlayerDatas[Private.MyIndex].Standby--;
			Send(DataType::List, Private.PlayerDatas[Private.MyIndex]);
			return;
		}
		MultiRoom.Init();
		MultiDatas.clear();
		Public = PublicData();
		Private = PrivateData();
		NowScene = Scene::Title;
		return;
		});

	if (MultiRoom.MultiFlag) {

		if (Config.DiscordSDK && !MultiRoom.InviteFlag) {
			SetState("MultiRoom");
		}

		if (Public.HitKey == HitType::Enter && MultiRoom.IsSelected) {
			Skin.Base->Title.SE.Don.Play();
			NowScene = Scene::Loading;
			return;
		}

		if (!Playing.Chart.OriginalData.WaveData.empty()) {
			if (!MultiRoom.IsSelected && !MultiRoom.IsHost) {
				MultiRoom.IsSelected = true;
			}
			if (!SongSelect.DemoSongPlayBlank.GetNowRecording()) {
				SongSelect.DemoSong.Delete();
				SongSelect.DemoSongPlayBlank.Start();
			}
			else if (SongSelect.DemoSongPlayBlank.GetRecordingTime() > SongSelect.DemoSongPlayBlankTime() && !SongSelect.DemoSong.IsPlay()) {
				SetCreateSoundDataType(DX_SOUNDDATATYPE_FILE);
				SongSelect.DemoSong.Load(Playing.Chart.OriginalData.WaveData.data(), Playing.Chart.OriginalData.WaveData.size(), 1);
				SongSelect.DemoSong.SetVolume(Playing.Chart.OriginalData.SongVolume * (Config.SongVolume / 100));
				SongSelect.DemoSong.SetCurrent(Playing.Chart.OriginalData.DemoStart);
				SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);
				SongSelect.DemoSong.Play(FALSE);
			}
		}

		static auto DonInputProc = [&] {
			if (MultiRoom.HostSelectMode) {
				Skin.Base->Title.SE.Don.Play();
				MultiRoom.HostSelectMode = false;
				Private.PlayerDatas[MultiRoom.GrantIndex].Standby = MultiRoom.HostVal;
				Send(DataType::Host, MultiRoom.GrantIndex);
				if (Private.MyIndex != MultiRoom.GrantIndex) {
					Private.PlayerDatas[Private.MyIndex].Standby = 0;
				}
			}
			else {
				if (MultiRoom.IsHost) {
					if (!MultiRoom.IsSelected) {
						Skin.Base->Title.SE.Don.Play();
						NowScene = Scene::SongSelect;
					}
					if (MultiRoom.IsStandby) {
						if (CheckStandby(Private.PlayerDatas, 1)) {
							Skin.Base->Title.SE.Don.Play();
							Public.HitKey = HitType::Enter;
							Send(DataType::Public, Public);
							NowScene = Scene::Loading;
							return;
						}
					}
				}
				if (!MultiRoom.IsStandby && MultiRoom.IsSelected) {
					Skin.Base->Title.SE.Don.Play();
					Private.PlayerDatas[Private.MyIndex].Standby++;
					Send(DataType::List, Private.PlayerDatas[Private.MyIndex]);
				}
			}
			};

		static auto KaInputProc = [&](bool direction) {
			if (MultiRoom.HostSelectMode) {
				Skin.Base->Title.SE.Ka.Play();
				if (!direction) {
					MultiRoom.GrantIndex <= 0 ? 0 : MultiRoom.GrantIndex--;
				}
				else {
					MultiRoom.GrantIndex >= Private.CountAll - 1 ? Private.CountAll - 1 : MultiRoom.GrantIndex++;
				}
			}
			};

		static auto F1InputProc = [&] {
			if (!(Private.PlayerDatas[Private.MyIndex].Standby % MultiRoom.HostVal)) {
				Skin.Base->Title.SE.Don.Play();
				PrevScene = Scene::MultiRoom;
				NowScene = Scene::Config;
			}
			};

		static auto TABInputProc = [&] {
			if (MultiRoom.IsHost && !MultiRoom.IsSelected && Private.CountAll > 1) {
				Skin.Base->Title.SE.Don.Play();
				MultiRoom.HostSelectMode = true;
				MultiRoom.GrantIndex = Private.MyIndex;
			}
			};

		Input.HitKeyesProcess(Config.KaInputLeft, KeyState::Down, [&] { KaInputProc(false); });
		Input.HitKeyesProcess(Config.KaInputRight, KeyState::Down, [&] { KaInputProc(true); });
		Input.HitKeyesProcess({ VK_UP, VK_LEFT }, KeyState::Down, [&] { KaInputProc(false); });
		Input.HitKeyesProcess({ VK_DOWN, VK_RIGHT }, KeyState::Down, [&] { KaInputProc(true); });

		Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, DonInputProc);
		Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, DonInputProc);
		Input.HitKeyProcess(VK_RETURN, KeyState::Down, DonInputProc);

		Input.HitKeyProcess(VK_CONTROL, KeyState::Hold, [&] {
			Input.HitKeyProcess(VK_F1, KeyState::Down, F1InputProc);
			});

		Input.HitKeyProcess(VK_TAB, KeyState::Down, TABInputProc);
	}

	else {

		MultiRoom.MultiFlag = MultiRoom.server.Connect(IPAddress::SolveHostName(MultiRoom.ConnectAddress)->Port(MultiRoom.ConnectPort));

		if (MultiRoom.MultiFlag) {

			MultiRoom.KeyInit();

			PlayerData data;
			data.Name = Config.PlayerName;
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
