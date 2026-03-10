#include "MultiRoom.h"
#include "GameSystem.h"

_MultiRoom::_MultiRoom(GameSystem* ptr) {
	gameptr = ptr;
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

		PlayerData data = PlayerData();

		if (MultiRoom.ConnectProc(&Config, data)) {		
			Send(DataType::List, data);
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
		Skin.Base->Other.Font.Game.DrawFontString({ 0,8 }, "サーバーに接続中…\n(長時間繋がらない場合はTabキーで再接続)");
		return;
	}

	Skin.Base->MultiRoom.Image.TitleBox.Draw({ 0,0 });

	if (!Playing.Chart.OriginalData.WaveData.empty()) {
		MultiRoom.ChartStrDraw(&Skin, &SongSelect, &Playing.Chart.OriginalData);
	}

	MultiRoom.PlayerDatasDraw(&Skin, Private);
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

			SongSelect.SongPreview(&Config, &Playing.Chart.OriginalData, true);
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

		static auto ReConnectProc = [&]() {
			Skin.Base->Title.SE.Don.Play();
			PlayerData data = PlayerData();
			if (MultiRoom.ConnectProc(&Config, data)) {
				Send(DataType::List, data);
			}
			};

		Input.HitKeyProcess(VK_TAB, KeyState::Down, ReConnectProc);
	}
}