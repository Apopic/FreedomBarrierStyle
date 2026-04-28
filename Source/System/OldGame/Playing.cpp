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

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			Playing.KeyFlash[i][j].End();
		}
	}

	Playing.Chart.CursorMove.End();
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

		if (Playing.Chart.BGMovieHandle != -1) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, Skin.Base->Playing.Config.LaneAlpha);
		}
		Skin.Base->Playing.Image.LaneFrame.Draw(add);
		Skin.Base->Playing.Image.Lane.Draw(add);
		SetDrawBlendMode(0, 0);

		if (!SongSelect.IsDanMode) {
			Playing.ProgressBarDraw(pldx, add);
		}
		Skin.Base->Playing.Image.Note.Draw(add, 0);

		if (Playing.Chart.NowGoGo) {
			Skin.Base->Playing.Image.GoGoFire.Draw({ 0,0 + add.Y }, (uint)(NowTime / Skin.Base->Playing.Config.GoGoFireFrameTime) % Skin.Base->Playing.Image.GoGoFire.Div.X);
		}

		Playing.JudgeUnderExplosionDraw(add, HitNote);

		Playing.NoteDraw(MultiData, NowTime, add, MultiRoom.MultiFlag, pldx);

		Skin.Base->Playing.Image.Base.Draw(add);
		Skin.Base->Playing.Image.NamePlate.Draw(add);
		Skin.Base->Playing.Image.MiniTaiko.Draw(add);

		if (!MultiRoom.MultiFlag) { Playing.NameDraw(Config.PlayerName, add); }
		else { Playing.NameDraw(MultiData.Name, add); }

		Playing.HitNoteDraw(HitNote, add, Private.CountAll, pldx);

		Playing.MiniTaikoFlashDraw(add, pldx);

		if (Playing.Chart.BalloonCount > 0) {
			Playing.NumberDraw(Skin.Base->Playing.Image.BalloonNumber, Playing.Chart.BalloonCount, 1, add);
		}
		if (Playing.Chart.Judge[pldx].Combo >= 3) {
			Playing.NumberDraw(Skin.Base->Playing.Image.ComboNumber, Playing.Chart.Judge[pldx].Combo, 0, add);
		}

		Playing.NumberDraw(Skin.Base->Playing.Image.ScoreNumber, Playing.Chart.Judge[pldx].Score, 1, add);

		pldx++;

	} while (pldx < Private.CountAll);

	if (Private.CountAll < 4) {
		if (!SongSelect.IsDanMode) {
			Playing.TitleDraw(Skin.Base->Playing.Font.Title, Skin.Base->Playing.Config.TitlePos, Playing.Chart.OriginalData.Title, Playing.Chart.OriginalData.PlayingTitleStrlen);
			Playing.TitleDraw(Skin.Base->Playing.Font.SubTitle, Skin.Base->Playing.Config.SubTitlePos, Playing.Chart.OriginalData.SubTitle, Playing.Chart.OriginalData.PlayingSubTitleStrlen);
		}
		else {
			Playing.TitleDraw(Skin.Base->Playing.Font.Title, Skin.Base->Playing.Config.TitlePos, Playing.Chart.OriginalData.DanTitle, Playing.Chart.OriginalData.DanTitleStrlen);
			Playing.TitleDraw(Skin.Base->Playing.Font.SubTitle, Skin.Base->Playing.Config.SubTitlePos, Playing.Chart.OriginalData.DanSubTitle, Playing.Chart.OriginalData.DanSubTitleStrlen);
		}
	}

	if (!MultiRoom.MultiFlag) {

		if (SongSelect.IsDanMode) {
			Skin.Base->Playing.Image.Box.Draw({ 0,0 });
			Playing.ExamProgressBarDraw();
			Playing.ExamValDraw();
		}

		if (Skin.Base->Playing.Config.KeyInputView) {
			Playing.KeyCharDraw(0, Config.KaInputLeft, Config.KeyCharKaLeft);
			Playing.KeyCharDraw(1, Config.DonInputLeft, Config.KeyCharDonLeft);
			Playing.KeyCharDraw(2, Config.DonInputRight, Config.KeyCharDonRight);
			Playing.KeyCharDraw(3, Config.KaInputRight, Config.KeyCharKaRight);
		}
		if (Skin.Base->Playing.Config.ScoreMeterView) {
			Playing.ScoreMeterDraw();
		}

		if (Config.TrainingMode) {
			if (!Playing.Chart.NowTime.GetNowRecording()) {
				DrawFormatString(
					Skin.Base->Playing.Image.Note.Pos.X,
					Skin.Base->Playing.Image.Note.Pos.Y - 100,
					GetColor(255, 255, 255),
					"(%d/%d)",
					Playing.MeasureIndex, Playing.AllMeasureCount
				);
			}
		}
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
					Playing.Chart.Init();
					Playing.Chart.OriginalData = SongSelect.ChartDataGet(*SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart());
					NowScene = Scene::Loading;
					return;
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