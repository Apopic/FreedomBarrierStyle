#include "Playing.h"
#include "GameSystem.h"

GameSystem* gameptr;

_Playing::_Playing(GameSystem* ptr) {
	::gameptr = ptr;
	__SkinPtr = &ptr->Skin;
	__ConfigPtr = &ptr->Config;
}

_Playing::~_Playing() {

}

void GameSystem::PlayingInit() {

	SetState(
		"Playing:" +
		Playing.Chart.OriginalData.Title +
		"[" +
		SongSelect.CourseList[SongSelect.CourseIndex] +
		"]"
	);

	for (auto&& taiko : Playing.MiniTaikoFlash) {
		taiko.End();
	}
}

void GameSystem::PlayingEnd() {

	Playing.Chart.SongData.Delete();
	PauseMovieToGraph(Playing.Chart.BGMovieHandle);
	DeleteGraph(Playing.Chart.BGMovieHandle);

}

void GameSystem::PlayingDraw() {

	const double NowTime = Playing.ChartNowTime(Config.FrameCountTimer, Config.FastDrawRate, Config.FrameExtendRate) + Playing.TrainingOffset;

	Skin.Base->Playing.Image.BackGround.Draw({ 0,0 });
	Playing.MovieDraw(NowTime);

	int pldx = 0;

	do {

		PlayerData MultiData;
		if (MultiRoom.MultiFlag) {
			MultiData = MultiDatas[pldx];
		}

		Pos2D<float> add = { 0.0f, 180.0f * (float)pldx};
		add.Y -= 155.0f * (Private.CountAll == 4);

		auto& HitNote = Playing.HitNote[pldx];

		Skin.Base->Playing.Image.LaneFrame.Draw(add);
		if (Playing.Chart.BGMovieHandle != 0) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, Skin.Base->Playing.Config.LaneAlpha);
		}
		Skin.Base->Playing.Image.Lane.Draw(add);
		SetDrawBlendMode(0, 0);

		if (!SongSelect.IsDanMode) {
			Playing.ProgressBarDraw(pldx, add);
		}
		Skin.Base->Playing.Image.Note.Draw(add, 0);

		if (Playing.Chart.NowGoGo) {
			Playing.GoGoFireDraw(add, NowTime);
		}

		Playing.JudgeUnderExplosionDraw(&Skin, add, HitNote);

		Playing.NoteDrawData(NowTime);
		Playing.NoteDraw(MultiData, NowTime, add, MultiRoom.MultiFlag, pldx);

		Skin.Base->Playing.Image.Base.Draw(add);
		Skin.Base->Playing.Image.NamePlate.Draw(add);
		Skin.Base->Playing.Image.MiniTaiko.Draw(add);

		if (!MultiRoom.MultiFlag) { Playing.NameDraw(Config.PlayerName, add); }
		else { Playing.NameDraw(MultiData.Name, add); }

		Playing.HitNoteDraw(HitNote, add, Private.CountAll, pldx);

		Playing.MiniTaikoFlashDraw(add, pldx);

		if (Playing.Chart.BalloonCount > 0) {
			Playing.BalloonDraw(Playing.Chart.BalloonCount, add);
		}
		if (Playing.Chart.Judge[pldx].Combo >= 3) {
			Playing.ComboDraw(pldx, add);
		}

		Playing.ScoreDraw(pldx, add);

		pldx++;

	} while (pldx < Private.CountAll);

	if (Private.CountAll < 4) {

		auto&& ChartData = Playing.Chart.OriginalData;

		if (!SongSelect.IsDanMode) {
			Playing.TitleDraw(ChartData.Title, ChartData.PlayingTitleStrlen);
			Playing.SubTitleDraw(ChartData.SubTitle, ChartData.PlayingSubTitleStrlen);
		}
		else {
			Playing.TitleDraw(ChartData.DanTitle, ChartData.DanTitleStrlen);
			Playing.SubTitleDraw(ChartData.DanSubTitle, ChartData.DanSubTitleStrlen);
		}
	}

	if (SongSelect.IsDanMode) {
		Skin.Base->Playing.Image.Box.Draw({ 0,0 });
		Playing.ExamProgressBarDraw();
		Playing.ExamValDraw();
	}

	if (Config.ViewDebugData) {
		DrawFormatString(0, 0, GetColor(255, 255, 255), "\n\n\nNowTime:%lf\nBPM:%lf", NowTime / Playing.Chart.SongSpeed, Playing.Chart.NowBPM * Playing.Chart.SongSpeed);
	}
}

void GameSystem::PlayingProc() {

	const double NowTime = Playing.ChartNowTime() + Playing.TrainingOffset;
	auto&& NoteDatas = Playing.Chart.RawNoteDatas;

	if (!MultiRoom.MultiFlag) {

		Input.HitKeyProcess(VK_ESCAPE, KeyState::Down, [&] {
			if (!SongSelect.IsDanMode) {
				Playing.TrainingOffset = 0;
				Playing.MeasureIndex = 0;
				NowScene = Scene::SongSelect;
			}
			else {
				Playing.Chart.IsDanPlay = false;
				NowScene = Scene::DanSelect;
			}
			});

		if (!SongSelect.IsDanMode) {

			Input.HitKeyProcess(VK_SHIFT, KeyState::Hold, [&] {
				Input.HitKeyProcess('Q', KeyState::Down, [&] {
					NowScene = Scene::Loading;
					});
				});

			if (!Config.TrainingMode) {

				Input.HitKeyProcess(VK_SHIFT, KeyState::Hold, [&] {
					Input.HitKeyProcess(VK_TAB, KeyState::Down, [&] {
						NowScene = Scene::Result;
						});
					});

			}
			else if (!Playing.Chart.NowTime.GetNowRecording()) {
				Playing.TraningModeProc(NowTime);
				return;
			}
		}
	}

	if (Playing.Chart.SongBlankTime < NowTime && Playing.Chart.SongBlankTime + 128 > NowTime && !Playing.Chart.SongData.IsPlay()) {
		Playing.Chart.SongData.Play(TRUE);
	}
	else if (Playing.Chart.SongBlankTime + 5000 < NowTime && !Playing.Chart.SongData.IsPlay()) {
		if ((!Config.TrainingMode && !SongSelect.IsDanMode) || MultiRoom.MultiFlag) {
			if (!MultiRoom.MultiFlag || CheckStandby(Private.PlayerDatas, 3)) {
				NowScene = Scene::Result;
				return;
			}
			else if (Private.PlayerDatas[Private.MyIndex].Standby % MultiRoom.HostVal == 2) {
				Private.PlayerDatas[Private.MyIndex].Standby++;
				Send(DataType::List, Private.PlayerDatas[Private.MyIndex]);
			}
		}
		if (!MultiRoom.MultiFlag) {
			NowScene = Scene::Loading;
			return;
		}
	}

	Playing.NoteProc(NowTime);

	if (Config.AutoPlayFlag) { 
		Playing.AutoPlayProc(NowTime);
	}
	else {
		Playing.PlayProc(NowTime);
	}

	if (SongSelect.IsDanMode) {
		Playing.DanProc();
	}

	if (MultiRoom.MultiFlag) {
		Playing.MultiProc(Public, Private);	
	}
}

void _Playing::Action(HitType type) {

	if (gameptr->MultiRoom.MultiFlag) {
		gameptr->Public.HitKey = type;
		gameptr->Public.Judge = Chart.Judge[0];
		gameptr->Public.GetIndex = gameptr->Private.MyIndex;
		gameptr->Send(DataType::Public, gameptr->Public);
		Chart.Judge[0].NoteType = '\0';
		Chart.Judge[0].HitJudge = JudgeType::None;
	}
}