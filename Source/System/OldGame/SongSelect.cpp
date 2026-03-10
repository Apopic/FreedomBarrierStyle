#include "SongSelect.h"
#include "GameSystem.h"

_SongSelect::_SongSelect(GameSystem* ptr) {
	__SkinPtr = &ptr->Skin;
	__ConfigPtr = &ptr->Config;
}

_SongSelect::~_SongSelect() {

}

void GameSystem::SongSelectInit() {

	SetState("SongSelect");

	Skin.Base->SongSelect.SE.Don.SetVolume((Config.SEVolume));
	Skin.Base->SongSelect.SE.Ka.SetVolume((Config.SEVolume));

	SetDragFileValidFlag(TRUE);

	if (SongSelect.BoxDatas.empty()) {
		std::string error = "譜面がありません。タイトルに戻ります";
		MessageBox(NULL, TEXT(error.c_str()), TEXT("エラー"), MB_ICONERROR);
		NowScene = Scene::Title;
		return;
	}
}

void GameSystem::SongSelectEnd() {

	SongSelect.HighScoreDataChange.End();
	SongSelect.DemoSongPlayBlank.End();
	SongSelect.DemoSong.Delete();
	SongSelect.CourseSelect = false;

	SetDragFileValidFlag(FALSE);
}

void GameSystem::SongSelectDraw() {

	Skin.Base->SongSelect.Image.BackGround.Draw(Pos2D<float>{0, 0});

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
					SongSelect.BoxDatas[j]->GetChart()->Title
				);
			}
		}
	}

	else {

		Pos2D<float> pos0 = {
				Skin.Base->SongSelect.Image.TitleBox.Pos.X,
				Skin.Base->SongSelect.Image.TitleBox.Pos.Y
		};

		Skin.Base->SongSelect.Image.TitleBox.Draw({ 0,0 });

		Skin.Base->SongSelect.Font.BoxTitle.Draw(
			{ pos0.X, pos0.Y - 20 },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->Title
		);
		Skin.Base->SongSelect.Font.BoxSubTitle.Draw(
			{ pos0.X, pos0.Y + 40 },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->SubTitle
		);

		for (uint i = 0; i < (uint)ChartCourseType::Count; ++i) {

			bool IsEdit = i == (uint)ChartCourseType::Edit && (uint)ChartCourseType::Edit == SongSelect.CourseIndex;

			Pos2D<float> pos1 = {
				Skin.Base->SongSelect.Image.LevelBox.Pos.X,
				Skin.Base->SongSelect.Image.LevelBox.Pos.Y + (100.0f * (i - IsEdit))
			};

			if (i == (uint)ChartCourseType::Oni && (uint)ChartCourseType::Edit == SongSelect.CourseIndex) {
				continue;
			}

			int c = 50 * (SongSelect.CourseIndex == i);
			SetDrawAddColor(c, c * !IsEdit, c);
			Skin.Base->SongSelect.Image.LevelBox.Draw({ 0, 100.0f * (i - IsEdit) });
			SetDrawAddColor(0, 0, 0);

			if (!SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->CourseDatas[i].IsPlayFlag) {
				continue;
			}

			int levelval = SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->CourseDatas[i].Level;

			Skin.Base->SongSelect.Font.Course.Draw(
				{ pos1.X - 420.0f, pos1.Y },
				GetColor(255, 255, 255),
				GetColor(0, 0, 0),
				SongSelect.CourseList[i] + "  ★×" + std::to_string(levelval)
			);

			int crownindex = 0;
			if (Playing.HighScore[i].Accuracy >= 75) {
				crownindex = 1;
			}
			if (Playing.HighScore[i].Accuracy >= 90) {
				crownindex = 2;
			}
			if (Playing.HighScore[i].Accuracy >= 90 && Playing.HighScore[i].Bad == 0) {
				crownindex = 3;
			}
			if (Playing.HighScore[i].Accuracy >= 90 && Playing.HighScore[i].Bad == 0 && Playing.HighScore[i].Ok == 0) {
				crownindex = 4;
			}

			Skin.Base->SongSelect.Image.Crown.Draw({ 0.0f, 100.0f * (i - IsEdit) }, crownindex);

			double time = GetEasingRate(SongSelect.HighScoreDataChange.GetRecordingTime() / SongSelect.HighScoreDataChangeTime, ease::Base::In, ease::Line::Linear);

			if (SongSelect.FadeDir[0]) {

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (255 * 4) * (SongSelect.FadeDir[0] - time));
				Skin.Base->SongSelect.Font.Course.Draw(
					{ pos1.X - 120.0f, pos1.Y },
					GetColor(255, 255, 255),
					GetColor(0, 0, 0),
					"HighScore"
				);
				Skin.Base->SongSelect.Font.HighScore.Draw(
					{ pos1.X + 360.0f, pos1.Y },
					GetColor(255, 255, 255),
					GetColor(0, 0, 0),
					std::to_string(Playing.HighScore[i].Score)
				);
				Skin.Base->SongSelect.Font.HighScore.Draw(
					{ pos1.X + 520.0f, pos1.Y },
					GetColor(255, 255, 255),
					GetColor(0, 0, 0),
					"(x" + 
					std::to_string(Playing.HighScore[i].ScoreRateGood).substr(0, std::to_string(Playing.HighScore[i].ScoreRateGood).find('.') + 3)
					+ ")"
				);
			}
			else {

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, (255 * 4) * (SongSelect.FadeDir[1] - time));

#define DRAWHIGHSCORESTR(pos,name) { std::string score = std::to_string(Playing.HighScore[i].name);\
                if(score.find('.') != std::string::npos) { score = score.substr(0, score.find('.') + 3); }\
				Skin.Base->SongSelect.Font.SubTitle.Draw(\
					{ pos1.X + pos, pos1.Y - 15.0f },\
					GetColor(255, 255, 255),\
					GetColor(0, 0, 0),\
					#name\
				);\
				Skin.Base->SongSelect.Font.SubTitle.Draw(\
					{ pos1.X + pos, pos1.Y + 10.0f },\
					GetColor(255, 255, 255),\
					GetColor(0, 0, 0),\
					score\
				);}\

				DRAWHIGHSCORESTR(-70.0f, Accuracy);
				DRAWHIGHSCORESTR(60.0f, Good);
				DRAWHIGHSCORESTR(170.0f, Ok);
				DRAWHIGHSCORESTR(270.0f, Bad);
				DRAWHIGHSCORESTR(370.0f, Roll);
				DRAWHIGHSCORESTR(490.0f, MaxCombo);

#undef DRAWHIGHSCORESTR

			}

			if (time >= 1.0) {
				SongSelect.FadeDir[0] = !SongSelect.FadeDir[0];
				SongSelect.FadeDir[1] = !SongSelect.FadeDir[1];
				SongSelect.HighScoreDataChange.Start();
			}

			SetDrawBlendMode(0, 0);

		}
	}
}

void GameSystem::SongSelectProc() {

	Input.HitKeyProcess(VK_ESCAPE, KeyState::Down, [&] {
		if (!SongSelect.CourseSelect) {
			if (MultiRoom.MultiFlag) {
				Playing.Chart.OriginalData.WaveData.clear();
				NowScene = Scene::MultiRoom;
				return;
			}
			Playing.Chart.Init();
			NowScene = Scene::Title;
		}
		else {
			for (auto&& HighScore : Playing.HighScore) {
				HighScore = JudgeData();
			}
			SongSelect.HighScoreDataChange.End();
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
				for (uint i = 0; i < (uint)ChartCourseType::Count; i++) {
					if (SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->CourseDatas[i].IsPlayFlag) {
						std::string UID = std::to_string(SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->ChartID);
						Playing.HighScore[i] = Result.ScoreDataLoad(UID, SongSelect.CourseList[i], Config.SongDirectories[0]);
					}
				}
				if (!SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->CourseDatas[(int)ChartCourseType::Edit].IsPlayFlag) {
					SongSelect.CourseIndex = SongSelect.CourseIndex == 4 ? 3 : SongSelect.CourseIndex;
				}
				SongSelect.HighScoreDataChange.Start();
				SongSelect.CourseSelect = true;
			}
			else {
				if (!SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->CourseDatas[SongSelect.CourseIndex].IsPlayFlag) {
					return;
				}

				Playing.Chart.OriginalData = SongSelect.ChartDataGet(*SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart());

				if (MultiRoom.MultiFlag) {
					Send(DataType::Chart, Playing.Chart.OriginalData);
					MultiRoom.IsSelected = true;
					NowScene = Scene::MultiRoom;
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
				SongSelect.DemoSong.Delete();
			}
			else {
				SongSelect.CourseIndex = SongSelect.CourseIndex - 1 < 0 ? 0 : SongSelect.CourseIndex - 1;
			}
		}
		else {
			if (!SongSelect.CourseSelect) {
				SongSelect.BoxDataIndex = SongSelect.BoxDataIndex + 1 >= SongSelect.BoxDatas.size() ? 0 : SongSelect.BoxDataIndex + 1;

				if (!SongSelect.BoxMotion.GetNowRecording()) { SongSelect.BoxMotion.Start(); }
				SongSelect.BoxMotionDirection -= 1;
				if (!SongSelect.TimeModify) { SongSelect.UseBoxMotionTime = SongSelect.BoxMotionTime(); SongSelect.TimeModify = true; }

				SongSelect.DemoSongPlayBlank.End();
				SongSelect.DemoSong.Delete();

			}
			else {
				bool IsEdit = SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->CourseDatas[(uint)ChartCourseType::Edit].IsPlayFlag;
				SongSelect.CourseIndex = SongSelect.CourseIndex + 1 > 3 + IsEdit ? 3 + IsEdit : SongSelect.CourseIndex + 1;
			}
		}
		};

	if (!SongSelect.CourseSelect) {
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
			SongSelect.DemoSong.Delete();
		}
	}

	static auto RandomInputProc = [&] {
		Skin.Base->SongSelect.SE.Ka.Play();
		SongSelect.BoxDataIndex = GetRand(SongSelect.BoxDatas.size() - 1);
		SongSelect.DemoSongPlayBlank.End();
		SongSelect.DemoSong.Delete();
		};

	if (!SongSelect.BoxDatas[SongSelect.BoxDataIndex]->IsGenre()) {
		SongSelect.SongPreview(&Config, SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart());
	}

	if (GetDragFileNum()) {
		SongSelect.ImportFile();
		SongSelect.EnumChartFile(Config.SongDirectories);
	}

	Input.HitKeyesProcess(Config.KaInputLeft, KeyState::Down, [&] { KaInputProc(false); });
	Input.HitKeyesProcess(Config.KaInputRight, KeyState::Down, [&] { KaInputProc(true); });
	Input.HitKeyesProcess({ VK_UP, VK_LEFT }, KeyState::Down, [&] { KaInputProc(false); });
	Input.HitKeyesProcess({ VK_DOWN, VK_RIGHT }, KeyState::Down, [&] { KaInputProc(true); });

	Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, DonInputProc);
	Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, DonInputProc);
	Input.HitKeyProcess(VK_RETURN, KeyState::Down, DonInputProc);

	if (!SongSelect.CourseSelect) {
		Input.HitKeyProcess(VK_SHIFT, KeyState::Hold, [&] {
			Input.HitKeyProcess(VK_TAB, KeyState::Down, RandomInputProc);
			});
	}

	Input.HitKeyProcess(VK_CONTROL, KeyState::Hold, [&] {
		Input.HitKeyProcess(VK_F1, KeyState::Down, [&] {
			Skin.Base->Title.SE.Don.Play();
			PrevScene = Scene::SongSelect;
			NowScene = Scene::Config;
			});
		});
}
