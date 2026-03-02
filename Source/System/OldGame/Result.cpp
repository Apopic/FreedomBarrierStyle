#include "Result.h"
#include "GameSystem.h"

_Skin *skinptr;

_Result::_Result(GameSystem* ptr) {
	skinptr = &ptr->Skin;
}

_Result::~_Result() {

}

void GameSystem::ResultInit() {
	DiscordActivityChange("Result", nullptr, nullptr, true);

}

void GameSystem::ResultEnd() {

}

void GameSystem::ResultDraw() {
	Skin.Base->Result.Image.BackGround.Draw({ 0,0 });
	Skin.Base->Result.Image.JudgeScore.Draw({ 0,0 });
	Skin.Base->Result.Image.Accuracy.Draw({ 0,0 });
	Skin.Base->Result.Image.Score.Draw({ 0,0 });

	Result.ScoreDraw(
		Skin.Base->Result.Config.ScorePos,
		Playing.Chart.Judge.Score
	);
	Result.AccurecyDraw(
		Skin.Base->Result.Config.AccurecyPos,
		Playing.Chart.Judge.Accurecy
	);
	Result.JudgesDraw(
		Skin.Base->Result.Config.GoodPos,
		Playing.Chart.Judge.Good
	);
	Result.JudgesDraw(
		Skin.Base->Result.Config.OkPos,
		Playing.Chart.Judge.Ok
	);
	Result.JudgesDraw(
		Skin.Base->Result.Config.BadPos,
		Playing.Chart.Judge.Bad
	);
	Result.JudgesDraw(
		Skin.Base->Result.Config.RollPos,
		Playing.Chart.Judge.Roll
	);
	Result.JudgesDraw(
		Skin.Base->Result.Config.MaxComboPos,
		Playing.Chart.Judge.MaxCombo
	);
	
	const JudgeData& data = Playing.Chart.Judge;
	int crownindex = 0;
	if (data.Accurecy >= 90) {
		crownindex = 1;
	}
	if (data.Bad == 0) {
		crownindex = 2;
	}
	if (data.Bad == 0 && data.Ok == 0) {
		crownindex = 3;
	}

	Skin.Base->Result.Image.Crown.Draw({ 0,0 }, crownindex);

	Skin.Base->Result.Font.Title.Draw(
		Skin.Base->Result.Config.TitlePos,
		GetColor(255, 255, 255),
		GetColor(0, 0, 0),
		Playing.Chart.OriginalData.ResultTitleStrlen,
		Playing.Chart.OriginalData.Title
	);
	Skin.Base->Result.Font.SubTitle.Draw(
		Skin.Base->Result.Config.SubTitlePos,
		GetColor(255, 255, 255),
		GetColor(0, 0, 0),
		Playing.Chart.OriginalData.ResultSubTitleStrlen,
		Playing.Chart.OriginalData.SubTitle
	);
}

void GameSystem::ResultProc() {
	Input.HitKeyProcess(VK_ESCAPE, KeyState::Down, [&] {
		NowScene = Scene::SongSelect;
		});
	Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, [&] {
		NowScene = Scene::SongSelect;
		});
	Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, [&] {
		NowScene = Scene::SongSelect;
		});
	Input.HitKeyProcess(VK_RETURN, KeyState::Down, [&] {
		NowScene = Scene::SongSelect;
		});
}

void _Result::ScoreDraw(const Pos2D<float>& Pos, ulonglong Num) {
	int digit = std::digit(Num);

	float offset = 0;
	int i = 0;
	do {
		skinptr->Base->Result.Image.Number.Draw({ offset + Pos.X, Pos.Y }, Num % 10);
		Num /= 10;
		++i;
		offset -= skinptr->Base->Result.Image.Number.Size.Width;
	} while (i < digit);
}

void _Result::AccurecyDraw(const Pos2D<float>& Pos, double Rate) {
	int iRate = Rate * 100;
	int digit = std::digit(iRate) + 2;
	if (iRate == 0) {
		digit = 5;
	}

	float offset = 0;
	skinptr->Base->Result.Image.Number.Draw({ offset + Pos.X, Pos.Y }, 11);
	offset -= skinptr->Base->Result.Image.Number.Size.Width;
	--digit;

	int i = 0;
	do {
		if (i == 2) {
			skinptr->Base->Result.Image.Number.Draw({ offset + Pos.X, Pos.Y }, 12);
		}
		else {
			skinptr->Base->Result.Image.Number.Draw({ offset + Pos.X, Pos.Y }, iRate % 10);
			iRate /= 10;
		}
		++i;
		offset -= skinptr->Base->Result.Image.Number.Size.Width;
	} while (i < digit);
}

void _Result::JudgesDraw(const Pos2D<float>& Pos, ulonglong Num) {
	
	int digit = std::digit(Num) + 1;

	float offset = skinptr->Base->Result.Image.Number.Size.Width * digit;
	skinptr->Base->Result.Image.Number.Draw({ offset + Pos.X, Pos.Y }, 10);
	offset -= skinptr->Base->Result.Image.Number.Size.Width;
	--digit;

	int i = 0;
	do {
		skinptr->Base->Result.Image.Number.Draw({ offset + Pos.X, Pos.Y }, Num % 10);
		Num /= 10;
		++i;
		offset -= skinptr->Base->Result.Image.Number.Size.Width;
	} while (i < digit);
}
