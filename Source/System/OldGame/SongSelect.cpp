#include "SongSelect.h"
#include "GameSystem.h"

_SongSelect::_SongSelect(GameSystem *ptr) {
	__SkinPtr = &ptr->Skin;
	__ConfigPtr = &ptr->Config;
}

_SongSelect::~_SongSelect() {

}

void GameSystem::SongSelectInit() {
	Skin.Base->SongSelect.SE.Don.SetVolume((Config.SEVolume));
	Skin.Base->SongSelect.SE.Ka.SetVolume((Config.SEVolume));
	DiscordActivityChange("Selecting...", "\0", nullptr, true);
}

void GameSystem::SongSelectEnd() {
	SongSelect.DemoSong.Delete();
	SongSelect.CourseSelect = false;
	SongSelect.DemoSongPlayBlank.End();
}

void GameSystem::SongSelectDraw() {
	Skin.Base->SongSelect.Image.BackGround.Draw(Pos2D<float>{0,0});

	if (!SongSelect.CourseSelect) {

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

		{
			int i = (SongSelect.BoxDataIndex + SongSelect.BoxMotionDirection) - 6;
			int j = i;
			int size = SongSelect.BoxDatas.size();
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
					SongSelect.BoxDatas[j]->GenreColor.R,
					SongSelect.BoxDatas[j]->GenreColor.G,
					SongSelect.BoxDatas[j]->GenreColor.B
					);
				Skin.Base->SongSelect.Image.Box.Draw(pos);
				SetDrawAddColor(0, 0, 0);

				if (SongSelect.BoxDatas[j]->IsGenre()) {
					if (SongSelect.BoxDatas[j]->GetGenre()->Open) {
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
							SongSelect.BoxDatas[j]->GetGenre()->Name
						);
					}
				}
				else {
					Skin.Base->SongSelect.Font.Title.Draw(
						pos,
						GetColor(255, 255, 255),
						GetColor(0, 0, 0),
						SongSelect.BoxDatas[j]->GetChart()->TitleStrlen,
						SongSelect.BoxDatas[j]->GetChart()->Title
					);
				}
			}
		}
	}
	else {
		DrawString(16, 54, SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->Title.c_str(), GetColor(255, 255, 255));
		DrawString(16, 72, SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->SubTitle.c_str(), GetColor(255, 255, 255));
		DrawString(16, 90, "Return SongSelect", GetColor(255, 255, 255));
		for (uint i = 0; i < (uint)ChartCourseType::Count; ++i) {
			if (!SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->CourseDatas[i].IsPlayFlag) {
				continue;
			}
			DrawString(16, 108 + 16 * i, SongSelect.CourseList[i].c_str(), GetColor(255, 255, 255));
		}
		DrawString(0, 108 + 16 * SongSelect.CourseIndex, ">", GetColor(255, 255, 255));
	}

}

void GameSystem::SongSelectProc() {
	Input.HitKeyProcess(VK_ESCAPE, KeyState::Down, [&] {
		if (!SongSelect.CourseSelect) {
			NowScene = Scene::Title;
		}
		else {
			SongSelect.CourseSelect = false;
		}
		});

	static auto DonInputProc = [&] {
		Skin.Base->SongSelect.SE.Don.Play();
		if (SongSelect.BoxDatas[SongSelect.BoxDataIndex]->IsGenre()) {
			bool& _f = SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetGenre()->Open;
			_f = !_f;
			SongSelect.BoxDatasUpdate();
		}
		else {
			if (!SongSelect.CourseSelect) {
				SongSelect.CourseSelect = true;
			}
			else {
				if (SongSelect.CourseIndex == -1) {
					SongSelect.CourseSelect = false;
					return;
				}
				if (!SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->CourseDatas[SongSelect.CourseIndex].IsPlayFlag) {
					return;
				}
				NowScene = Scene::Loading;
			}
		}
		};
	static auto KaInputProc = [&](bool direction) {
		Skin.Base->SongSelect.SE.Ka.Play();
		if (!direction) {
			if (!SongSelect.CourseSelect) {
				SongSelect.BoxDataIndex = SongSelect.BoxDataIndex - 1 < 0 ? SongSelect.BoxDatas.size() - 1 : SongSelect.BoxDataIndex - 1;
				if (!SongSelect.BoxMotion.GetNowRecording()) { SongSelect.BoxMotion.Start(); }
				SongSelect.BoxMotionDirection += 1;
				if (!SongSelect.TimeModify) { SongSelect.UseBoxMotionTime = SongSelect.BoxMotionTime(); SongSelect.TimeModify = true; }

				SongSelect.DemoSongPlayBlank.End();
			}
			else {
				SongSelect.CourseIndex = SongSelect.CourseIndex - 1 < -1 ? -1 : SongSelect.CourseIndex - 1;
			}
		}
		else {
			if (!SongSelect.CourseSelect) {
				SongSelect.BoxDataIndex = SongSelect.BoxDataIndex + 1 >= SongSelect.BoxDatas.size() ? 0 : SongSelect.BoxDataIndex + 1;
				if (!SongSelect.BoxMotion.GetNowRecording()) { SongSelect.BoxMotion.Start(); }
				SongSelect.BoxMotionDirection -= 1;
				if (!SongSelect.TimeModify) { SongSelect.UseBoxMotionTime = SongSelect.BoxMotionTime(); SongSelect.TimeModify = true; }

				SongSelect.DemoSongPlayBlank.End();
			}
			else {
				SongSelect.CourseIndex = SongSelect.CourseIndex + 1 > 4 ? 4 : SongSelect.CourseIndex + 1;
			}
		}
		};

	const int MouseWheel = Input.GetMouseWheel();

	if (MouseWheel != 0) {
		Skin.Base->SongSelect.SE.Ka.Play();
		const int& _mousewheel = (std::abs(MouseWheel) % SongSelect.BoxDatas.size()) * (std::signbit(MouseWheel) ? -1 : 1) * -1;
		if (std::signbit(_mousewheel)) {
			SongSelect.BoxDataIndex = SongSelect.BoxDataIndex + _mousewheel <= -1 ? SongSelect.BoxDatas.size() + _mousewheel : SongSelect.BoxDataIndex + _mousewheel;
		}
		else {
			SongSelect.BoxDataIndex = SongSelect.BoxDataIndex + _mousewheel >= SongSelect.BoxDatas.size() ? (SongSelect.BoxDataIndex + _mousewheel) - SongSelect.BoxDatas.size() : SongSelect.BoxDataIndex + _mousewheel;
		}
		if (!SongSelect.BoxMotion.GetNowRecording()) { SongSelect.BoxMotion.Start(); }
		SongSelect.BoxMotionDirection += MouseWheel;
		if (!SongSelect.TimeModify) { SongSelect.UseBoxMotionTime = SongSelect.BoxMotionTime(); SongSelect.TimeModify = true; }

		SongSelect.DemoSongPlayBlank.End();
	}

	if (!SongSelect.DemoSongPlayBlank.GetNowRecording()) {
		SongSelect.DemoSong.Delete();
		SongSelect.DemoSongPlayBlank.Start();
	}
	else if (SongSelect.DemoSongPlayBlank.GetRecordingTime() > SongSelect.DemoSongPlayBlankTime() && !SongSelect.DemoSong.IsPlay() && !SongSelect.BoxDatas[SongSelect.BoxDataIndex]->IsGenre()) {
		SetCreateSoundDataType(DX_SOUNDDATATYPE_FILE);
		SongSelect.DemoSong.Load(SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->WavePath, 3);
		SongSelect.DemoSong.SetVolume(SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->SongVolume * (Config.SongVolume / 100));
		SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);
		SongSelect.DemoSong.Play();
	}

	Input.HitKeyesProcess(Config.KaInputLeft, KeyState::Down, [&] { KaInputProc(false); });
	Input.HitKeyesProcess(Config.KaInputRight, KeyState::Down, [&] { KaInputProc(true); });
	Input.HitKeyesProcess({VK_UP, VK_LEFT}, KeyState::Down, [&] { KaInputProc(false); });
	Input.HitKeyesProcess({VK_DOWN, VK_RIGHT}, KeyState::Down, [&] { KaInputProc(true); });

	Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, DonInputProc);
	Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, DonInputProc);
	Input.HitKeyProcess(VK_RETURN, KeyState::Down, DonInputProc);
}
