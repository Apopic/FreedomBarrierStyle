#include "Playing.h"
#include "GameSystem.h"

GameSystem* gameptr;

_Playing::_Playing(GameSystem* ptr) {
	::gameptr = ptr;
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

}

void GameSystem::PlayingDraw() {

	const double NowTime = Playing.ChartNowTime(Config.FrameCountTimer, Config.FastDrawRate, Config.FrameExtendRate) + Playing.TrainingOffset;

	Skin.Base->Playing.Image.BackGround.Draw({ 0,0 });

	int pldx = 0;

	do {

		PlayerData MultiData;
		if (MultiRoom.MultiFlag) {
			MultiData = MultiDatas[pldx];
		}

		Pos2D<float> add = { 0.0f, 180.0f * (float)pldx};
		add.Y -= 155.0f * (Private.CountAll == 4);

		auto& HitNote = Playing.HitNote[pldx];

		Skin.Base->Playing.Image.LaneFrame.Draw(add, 0);
		Skin.Base->Playing.Image.Lane.Draw(add, 0);

		if (!SongSelect.IsDanMode) {
			Playing.ProgressBarDraw(&Skin, pldx, add);
		}
		Skin.Base->Playing.Image.Note.Draw(add, 0);

		if (Playing.Chart.NowGoGo) {
			Playing.GoGoFireDraw(&Skin, add, NowTime);
		}

		Playing.JudgeUnderExplosionDraw(&Skin, add, HitNote);

		auto&& NoteDatas = Playing.Chart.RawNoteDatas;
		Playing.NoteDrawData(NoteDatas, NowTime);
		Playing.NoteDraw(MultiData, &Skin, &Config, NoteDatas, NowTime, add, MultiRoom.MultiFlag, pldx, MultiData.Option.Hidden, MultiData.Option.Sudden);

		Skin.Base->Playing.Image.Base.Draw(add, 0);
		Skin.Base->Playing.Image.NamePlate.Draw(add, 0);
		Skin.Base->Playing.Image.MiniTaiko.Draw(add, 0);

		if (!MultiRoom.MultiFlag) { Playing.NameDraw(&Skin, Config.PlayerName, add); }
		else { Playing.NameDraw(&Skin, MultiData.Name, add); }

		Playing.HitNoteDraw(&Skin, &Config, HitNote, add, Private.CountAll, pldx);

		Playing.MiniTaikoFlashDraw(&Skin, add, pldx);

		if (Playing.Chart.BalloonCount > 0) {
			Playing.BalloonDraw(&Skin, Playing.Chart.BalloonCount, add);
		}
		if (Playing.Chart.Judge[pldx].Combo >= 3) {
			Playing.ComboDraw(&Skin, pldx, add);
		}

		Playing.ScoreDraw(&Skin, pldx, add);

		pldx++;

	} while (pldx < Private.CountAll);

	if (Private.CountAll < 4) {
		if (!SongSelect.IsDanMode) {
			Playing.TitleDraw(&Skin, Playing.Chart.OriginalData.Title, Playing.Chart.OriginalData.PlayingTitleStrlen);
			Playing.SubTitleDraw(&Skin, Playing.Chart.OriginalData.SubTitle, Playing.Chart.OriginalData.PlayingSubTitleStrlen);
		}
		else {
			Playing.TitleDraw(&Skin, Playing.Chart.OriginalData.DanTitle, Playing.Chart.OriginalData.DanTitleStrlen);
			Playing.SubTitleDraw(&Skin, Playing.Chart.OriginalData.DanSubTitle, Playing.Chart.OriginalData.DanSubTitleStrlen);
		}
	}

	if (SongSelect.IsDanMode) {
		Skin.Base->Playing.Image.Box.Draw({ 0,0 });
		Playing.ExamProgressBarDraw(&Skin);
		Playing.ExamValDraw(&Skin);
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
				Playing.TraningModeProc(&Config, NowTime);
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

	Playing.NoteProc(&Skin, &Config, NoteDatas, NowTime);

	if (Config.AutoPlayFlag) { 
		Playing.AutoPlayProc(&Skin, &Config, NoteDatas, NowTime);
	}
	else {
		Playing.PlayProc(&Skin, &Config, NowTime);
	}

	if (SongSelect.IsDanMode) {
		Playing.DanProc(&Skin);
	}

	if (MultiRoom.MultiFlag) {
		Playing.MultiProc(Public, Private);	
	}
}

void _Playing::Action(HitType type) {

	if (gameptr->Config.AutoPlayFlag) {
		Chart.Judge[0].Score = 0;
	}

	if (gameptr->MultiRoom.MultiFlag) {
		gameptr->Public.HitKey = type;
		gameptr->Public.Judge = Chart.Judge[0];
		gameptr->Public.GetIndex = gameptr->Private.MyIndex;
		gameptr->Send(DataType::Public, gameptr->Public);
		Chart.Judge[0].NoteType = '\0';
		Chart.Judge[0].HitJudge = JudgeType::None;
	}
}