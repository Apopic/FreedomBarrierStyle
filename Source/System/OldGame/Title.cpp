#include "Title.h"
#include "GameSystem.h"

_Title::_Title(GameSystem *ptr) {

}

_Title::~_Title() {

}

void GameSystem::TitleInit() {
	Title.CaptionAlphaTimer.Start();
	Title.SelectorAlphaTimer.Start();
	Title.PlateAlphaTimer.End();

	Skin.Base->Title.SE.Don.SetVolume(Config.SEVolume);
	Skin.Base->Title.SE.Ka.SetVolume(Config.SEVolume);

	DiscordActivityChange("Idle", nullptr, nullptr, true);
}

void GameSystem::TitleEnd() {

}

void GameSystem::TitleDraw() {

	Skin.Base->Title.Font.Title.Draw(
		Skin.Base->Title.Config.TitlePos,
		GetColor(255, 255, 255),
		GetColor(0, 0, 0),
		"FreedomBarrierStyle"
	);

	Skin.Base->Title.Image.BackGround.Draw({0, 0});

	if (Title.ModeSelector == _Title::Mode::Start) {
		int _platealpha = 255 * std::cos(DX_PI * Title.PlateAlphaTimer.GetRecordingTime() / Title.PlateAlphaTime());

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, _platealpha);
		Skin.Base->Title.Image.BackPlate_Small.Draw({ 0,0 });

		int _stringalpha = 255 * std::abs(std::sin(DX_PI * Title.CaptionAlphaTimer.GetRecordingTime() / Title.CaptionAlphaLoopTime()));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, _stringalpha);
		Skin.Base->Title.Font.Caption.Draw(
			Skin.Base->Title.Config.CaptionPos,
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			"Press To \"Don\" Key "
		);
		SetDrawBlendMode(0, 0);

	}
	else {

		int _platealpha = 255 * std::cos(2 * DX_PI * Title.PlateAlphaTimer.GetRecordingTime() / Title.PlateAlphaTime());
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, _platealpha);
		Skin.Base->Title.Image.BackPlate_Big.Draw({ 0,0 });

		int _selectoralpha = 255 * std::abs(std::sin(DX_PI * Title.SelectorAlphaTimer.GetRecordingTime() / Title.SelectorAlphaLoopTime()));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, _selectoralpha);
		Skin.Base->Title.Image.Selector.Draw({ 0, Skin.Base->Title.Config.ModesLineHeight * (int)Title.ModeSelector });
		
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, _platealpha);
		for (uint i = 0; i < (int)_Title::Mode::Count; ++i) {
			Skin.Base->Title.Font.Modes.Draw(
				{
					Skin.Base->Title.Config.ModesPos.X,
					Skin.Base->Title.Config.ModesPos.Y + (Skin.Base->Title.Config.ModesLineHeight * i),
				},
				GetColor(255, 255, 255),
				GetColor(0, 0, 0),
				Title.ModeStrings[i]
			);
		}
		SetDrawBlendMode(0, 0);
	}
}

void GameSystem::TitleProc() {
	Input.HitKeyProcess(VK_ESCAPE, KeyState::Down, [&] {
		EndFlag = true;
		});

	auto DonKeyProc = [&] {
		Skin.Base->Title.SE.Don.Play();
		switch (Title.ModeSelector)
		{
		case _Title::Mode::Start:
			Title.CaptionAlphaTimer.End();
			Title.SelectorAlphaTimer.End();
			Title.PlateAlphaTimer.Start();
			break;
		case _Title::Mode::Single:
			NowScene = Scene::SongSelect;
			break;
		case _Title::Mode::Multi:
			NowScene = Scene::MultiRoom;
			break;
		case _Title::Mode::Dan:
			NowScene = Scene::SongSelect;
			break;
		case _Title::Mode::Config:
			NowScene = Scene::SongSelect;
			break;
		case _Title::Mode::End:
			EndFlag = true;
			break;
		}
	};

	if (Title.PlateAlphaTimer.GetNowRecording() &&
		Title.PlateAlphaTimer.GetRecordingTime() > Title.PlateAlphaTime() / 2) {
		if (Title.PlateAlphaTimer.GetRecordingTime() > Title.PlateAlphaTime()) {
			Title.PlateAlphaTimer.End();
			Title.SelectorAlphaTimer.Start();
		}
		else {
			if (Title.ModeSelector == _Title::Mode::Start) {
				Title.ModeSelector = _Title::Mode::Single;
			}
		}
	}
	else {
		Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, DonKeyProc);
		Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, DonKeyProc);
		Input.HitKeyProcess(VK_RETURN, KeyState::Down, DonKeyProc);
	}

	if (Title.ModeSelector != _Title::Mode::Start) {
		static auto KaKeyProc = [&](bool direction) {
			Skin.Base->Title.SE.Ka.Play();
			Title.SelectorAlphaTimer.Start();
			if (!direction) {
				(int &)Title.ModeSelector -= 1;
				if (Title.ModeSelector <= _Title::Mode::Start) { Title.ModeSelector = _Title::Mode::Single; }
			}
			else {
				(int &)Title.ModeSelector += 1;
				if (Title.ModeSelector >= _Title::Mode::Count) { Title.ModeSelector = _Title::Mode::End; }
			}
		};

		Input.HitKeyesProcess(Config.KaInputLeft, KeyState::Down, [&] { KaKeyProc(false); });
		Input.HitKeyesProcess(Config.KaInputRight, KeyState::Down, [&] { KaKeyProc(true); });
		
		Input.HitKeyesProcess({VK_UP,VK_LEFT}, KeyState::Down, [&] { KaKeyProc(false); });
		Input.HitKeyesProcess({VK_DOWN,VK_RIGHT}, KeyState::Down, [&] { KaKeyProc(true); });


	}
}

