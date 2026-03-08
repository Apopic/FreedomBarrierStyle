#include "DanSelect.h"
#include "GameSystem.h"

_DanSelect::_DanSelect(GameSystem* ptr) {
}

_DanSelect::~_DanSelect() {
}

void GameSystem::DanSelectInit() {

	SongSelect.IsDanMode = true;

	SetState("DanSelect");

	Skin.Base->SongSelect.SE.Don.SetVolume((Config.SEVolume));
	Skin.Base->SongSelect.SE.Ka.SetVolume((Config.SEVolume));

}

void GameSystem::DanSelectEnd() {
;
	SongSelect.CourseSelect = false;

}

void GameSystem::DanSelectDraw() {

	Skin.Base->SongSelect.Image.BackGround.Draw(Pos2D<float>{0, 0});

	if (SongSelect.BoxMotion.GetRecordingTime() > SongSelect.UseBoxMotionTime) {
		SongSelect.BoxMotion.End();
		SongSelect.TimeModify = false;
		if (SongSelect.BoxMotionDirection != 0) {
			if (std::signbit(SongSelect.BoxMotionDirection)) {
				SongSelect.BoxMotionDirection += 1;
			}
			else {
				SongSelect.BoxMotionDirection -= 1;
			}
			if (SongSelect.BoxMotionDirection != 0) {
				SongSelect.BoxMotion.Start();
				SongSelect.UseBoxMotionTime = SongSelect.BoxMotionTime();
				SongSelect.TimeModify = true;
			}
		}
	}

	int i = (SongSelect.BoxDataIndex + SongSelect.BoxMotionDirection) - 6;
	int j = i;
	int size = SongSelect.DanBoxDatas.size();
	if (j < 0) {
		j = size + (j % size);
	}
	if (j >= size) {
		j = (j % size);
	}

	const float _motion_t = ((SongSelect.BoxMotion.GetRecordingTime()) / SongSelect.UseBoxMotionTime);
	const int _offsetindex = std::abs(SongSelect.BoxMotionDirection);
	const int _signdirection = (std::signbit(SongSelect.BoxMotionDirection) ? -1 : 1);
	const bool _nowmotion = SongSelect.BoxMotion.GetNowRecording();

	for (int c = 0; c < 13; ++c, ++i, (j + 1 >= size ? j = 0 : ++j)) {

		Pos2D<float> pos = {
			(i - SongSelect.BoxDataIndex) * Skin.Base->SongSelect.Config.BoxDistance.X,
			(i - SongSelect.BoxDataIndex) * Skin.Base->SongSelect.Config.BoxDistance.Y
		};

		if (_nowmotion) {
			pos = {
				pos.X - ((-_motion_t + _offsetindex) * (Skin.Base->SongSelect.Config.BoxDistance.X * _signdirection)),
				pos.Y - ((-_motion_t + _offsetindex) * (Skin.Base->SongSelect.Config.BoxDistance.Y * _signdirection))
			};
		}

		const int _signpos = (std::signbit(pos.Y) ? -1 : 1);

		if (c != 6 && c != (_nowmotion ? 6 - _signdirection : 6)) {
			pos.Y += Skin.Base->SongSelect.Config.BoxDistance.Y * _signpos;
		}
		else if (_nowmotion) {
			if (c == 6) {
				pos.Y -= (-_motion_t * (Skin.Base->SongSelect.Config.BoxDistance.Y * _signdirection));
			}
			else {
				pos.Y -= ((1 - _motion_t) * (Skin.Base->SongSelect.Config.BoxDistance.Y * _signdirection));
			}
		}

		pos = {
			pos.X + Skin.Base->SongSelect.Config.SongBoxListPos.X,
			pos.Y + Skin.Base->SongSelect.Config.SongBoxListPos.Y
		};

		if (!(pos.Y > Skin.SimulationRect.Top && pos.Y < Skin.SimulationRect.Bottom)) {
			continue;
		}

		SetDrawAddColor(
			SongSelect.DanBoxDatas[j]->GenreColor.R,
			SongSelect.DanBoxDatas[j]->GenreColor.G,
			SongSelect.DanBoxDatas[j]->GenreColor.B
		);
		Skin.Base->SongSelect.Image.Box.Draw(pos);
		SetDrawAddColor(0, 0, 0);

		if (SongSelect.DanBoxDatas[j]->IsGenre()) {
			if (SongSelect.DanBoxDatas[j]->GetGenre()->Open) {
				Skin.Base->SongSelect.Font.Title.Draw(
					pos,
					GetColor(255, 255, 255),
					GetColor(0, 0, 0),
					"Close"
				);
			}
			else {
				Skin.Base->SongSelect.Font.Title.Draw(
					pos,
					GetColor(255, 255, 255),
					GetColor(0, 0, 0),
					SongSelect.DanBoxDatas[j]->GetGenre()->Name
				);
			}
		}
		else {
			Skin.Base->SongSelect.Font.Title.Draw(
				pos,
				GetColor(255, 255, 255),
				GetColor(0, 0, 0),
				SongSelect.DanBoxDatas[j]->GetChart()->Title
			);
		}
	}
}

void GameSystem::DanSelectProc() {

	Input.HitKeyProcess(VK_ESCAPE, KeyState::Down, [&] {
		SongSelect.IsDanMode = false;		
		Playing.Chart.Init();
		NowScene = Scene::Title;
		});

	static auto DonInputProc = [&] {
		Skin.Base->SongSelect.SE.Don.Play();
		if (SongSelect.DanBoxDatas[SongSelect.BoxDataIndex]->IsGenre()) {
			bool& _f = SongSelect.DanBoxDatas[SongSelect.BoxDataIndex]->GetGenre()->Open;
			_f = !_f;
			SongSelect.DanBoxDatasUpdate();
		}
		else {
			SongSelect.CourseIndex = (int)ChartCourseType::Dan;
			NowScene = Scene::Loading;
		}
		};
	static auto KaInputProc = [&](bool direction) {
		Skin.Base->SongSelect.SE.Ka.Play();
		if (!direction) {
			SongSelect.BoxDataIndex = SongSelect.BoxDataIndex - 1 < 0 ? SongSelect.DanBoxDatas.size() - 1 : SongSelect.BoxDataIndex - 1;

			if (!SongSelect.BoxMotion.GetNowRecording()) { SongSelect.BoxMotion.Start(); }
			SongSelect.BoxMotionDirection += 1;
			if (!SongSelect.TimeModify) { SongSelect.UseBoxMotionTime = SongSelect.BoxMotionTime(); SongSelect.TimeModify = true; }
		}
		else {
			SongSelect.BoxDataIndex = SongSelect.BoxDataIndex + 1 >= SongSelect.DanBoxDatas.size() ? 0 : SongSelect.BoxDataIndex + 1;

			if (!SongSelect.BoxMotion.GetNowRecording()) { SongSelect.BoxMotion.Start(); }
			SongSelect.BoxMotionDirection -= 1;
			if (!SongSelect.TimeModify) { SongSelect.UseBoxMotionTime = SongSelect.BoxMotionTime(); SongSelect.TimeModify = true; }
		}
		};

	const int MouseWheel = Input.GetMouseWheel();

	if (MouseWheel != 0) {
		Skin.Base->SongSelect.SE.Ka.Play();
		const int& _mousewheel = (std::abs(MouseWheel) % SongSelect.DanBoxDatas.size()) * (std::signbit(MouseWheel) ? -1 : 1) * -1;
		if (std::signbit(_mousewheel)) {
			SongSelect.BoxDataIndex = SongSelect.BoxDataIndex + _mousewheel <= -1 ? SongSelect.DanBoxDatas.size() + _mousewheel : SongSelect.BoxDataIndex + _mousewheel;
		}
		else {
			SongSelect.BoxDataIndex = SongSelect.BoxDataIndex + _mousewheel >= SongSelect.DanBoxDatas.size() ? (SongSelect.BoxDataIndex + _mousewheel) - SongSelect.DanBoxDatas.size() : SongSelect.BoxDataIndex + _mousewheel;
		}
		if (!SongSelect.BoxMotion.GetNowRecording()) { SongSelect.BoxMotion.Start(); }
		SongSelect.BoxMotionDirection += MouseWheel;
		if (!SongSelect.TimeModify) { SongSelect.UseBoxMotionTime = SongSelect.BoxMotionTime(); SongSelect.TimeModify = true; }
	}

	Input.HitKeyesProcess(Config.KaInputLeft, KeyState::Down, [&] { KaInputProc(false); });
	Input.HitKeyesProcess(Config.KaInputRight, KeyState::Down, [&] { KaInputProc(true); });
	Input.HitKeyesProcess({ VK_UP, VK_LEFT }, KeyState::Down, [&] { KaInputProc(false); });
	Input.HitKeyesProcess({ VK_DOWN, VK_RIGHT }, KeyState::Down, [&] { KaInputProc(true); });

	Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, DonInputProc);
	Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, DonInputProc);
	Input.HitKeyProcess(VK_RETURN, KeyState::Down, DonInputProc);

	Input.HitKeyProcess(VK_CONTROL, KeyState::Hold, [&] {
		Input.HitKeyProcess(VK_F1, KeyState::Down, [&] {
			Skin.Base->Title.SE.Don.Play();
			PrevScene = Scene::DanSelect;
			NowScene = Scene::Config;
			});
		});
}
