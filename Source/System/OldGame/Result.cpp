#include "Result.h"
#include "GameSystem.h"

_Skin* skinptr;
_Result* Result;

_Result::_Result(GameSystem* ptr) {
	skinptr = &ptr->Skin;
}

_Result::~_Result() {

}

void GameSystem::ResultInit() {

	SetState("Result");
	Playing.Chart.IsDanPlay = false;
}

void GameSystem::ResultEnd() {

	Result.Index = 0;

	if (MultiRoom.MultiFlag) {

		Playing.Chart.OriginalData.WaveData.clear();
		Playing.Chart.Init();
		MultiRoom.IsSelected = false;

		for (auto&& data : Private.PlayerDatas) {
			data.Standby = 0;
			data.NoteType.clear();
		}
		if (MultiRoom.IsHost) {
			Private.PlayerDatas[Private.MyIndex].Standby = MultiRoom.HostVal;
			Send(DataType::Reset, 0);
			Send(DataType::List, Private.PlayerDatas[Private.MyIndex]);
		}
	}
	else if (!Config.AutoPlayFlag && !SongSelect.IsDanMode) {

		std::string UID = std::to_string(Playing.Chart.OriginalData.ChartID);
		auto LoadHighScore = Result.ScoreDataLoad(UID, SongSelect.CourseList[SongSelect.CourseIndex], Config.SongDirectories[0]);
		
		if (Playing.Chart.Judge[0].Score > LoadHighScore.Score) {	

			if (Playing.Chart.OriginalData.ChartID == 0) {
				std::ofstream ofs(Playing.Chart.OriginalData.FilePath, std::ios::app);
				if (ofs.is_open()) {
					UID = Result.GenUID();
					SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart()->ChartID = stoull(UID);
					ofs << "\nCHARTID:" + UID << std::endl;
					ofs.close();
				}
			}

			Result.ScoreDataSave(UID, SongSelect.CourseList[SongSelect.CourseIndex], Playing.Chart.Judge[0], Config.SongDirectories[0]);
		}
	}
}

void GameSystem::ResultDraw() {

	Skin.Base->Result.Image.BackGround.Draw({ 0,0 });
	Skin.Base->Result.Image.JudgeScore.Draw({ 0,0 });
	Skin.Base->Result.Image.Accuracy.Draw({ 0,0 });
	Skin.Base->Result.Image.Score.Draw({ 0,0 });

#define ResultDraw(name, index) Result.name##Draw({Skin.Base->Result.Config.JudgeDataPos.X[index], Skin.Base->Result.Config.JudgeDataPos.Y[index] }, Playing.Chart.Judge[Result.Index].name);

	ResultDraw(Score, 0)
	ResultDraw(Accuracy, 1)

#define JudgeDraw(name, index) Result.JudgesDraw({Skin.Base->Result.Config.JudgeDataPos.X[index],Skin.Base->Result.Config.JudgeDataPos.Y[index], }, Playing.Chart.Judge[Result.Index].name);

	JudgeDraw(Good, 2)
	JudgeDraw(Ok, 3)
	JudgeDraw(Bad, 4)
	JudgeDraw(Roll, 5)
	JudgeDraw(MaxCombo, 6)

	const JudgeData& data = Playing.Chart.Judge[Result.Index];
	int crownindex = 0;

	bool IsRedPass = SongSelect.IsDanMode && !Playing.Chart.IsFall;
	int IsGoldPass = -1;
	if (SongSelect.IsDanMode) {
		if (IsGoldPass == -1) {
			IsGoldPass = 1;
			for (uint i = 0; i < Playing.Chart.OriginalData.ExamDatas.size(); i++) {

				auto ExamData = Playing.Chart.OriginalData.ExamDatas[i];

#define EXAMVAL(type) case ExamTypes::type:\
if (ExamData.Range == ExamRange::More && data.type < ExamData.PassVal[1]) { IsGoldPass = 0; }\
if (ExamData.Range == ExamRange::Less && data.type >= ExamData.PassVal[1]) { IsGoldPass = 0; }\
break;\

				switch (ExamData.ExamType) {
					EXAMVAL(Accuracy);
					EXAMVAL(Good);
					EXAMVAL(Ok);
					EXAMVAL(Bad);
					EXAMVAL(Score);
					EXAMVAL(Roll);
					EXAMVAL(HitNote);
					EXAMVAL(MaxCombo);
				}
			}
		}
	}

	if (data.Accuracy >= 75 && !SongSelect.IsDanMode) {
		crownindex = 1;
	}
	if (data.Accuracy >= 90 || IsRedPass) {
		crownindex = 2;
	}
	if ((data.Accuracy >= 90 && data.Bad == 0) || IsGoldPass == 1) {
		crownindex = 3;
	}
	if (data.Accuracy >= 90 && data.Bad == 0 && data.Ok == 0 && !SongSelect.IsDanMode) {
		crownindex = 4;
	}

	Skin.Base->Result.Image.Crown.Draw({ 0, 0 }, crownindex);

	if (MultiRoom.MultiFlag) {	
		Skin.Base->Result.Font.PlayerName.Draw(
			Skin.Base->Result.Config.PlayerNamePos,
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			GetStrlen(MultiDatas[Result.Index].Name, Skin.Base->Result.Font.PlayerName.Handle),
			MultiDatas[Result.Index].Name
		);
	}

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

	Input.HitKeyProcess(VK_RETURN, KeyState::Down, [&] {
		if (!MultiRoom.MultiFlag) {
			if (SongSelect.IsDanMode) {
				NowScene = Scene::DanSelect;
			}
			else {
				NowScene = Scene::SongSelect;
			}
		}
		if (MultiRoom.IsHost) {
			NowScene = Scene::MultiRoom;
			Public.HitKey = HitType::Back;
			Send(DataType::Public, Public);
		}
	});

	if (Private.CountAll >= 2) {

		Input.HitKeyProcess(VK_LEFT, KeyState::Down, [&] {
			Skin.Base->SongSelect.SE.Ka.Play();
			Result.Index > 0 ? --Result.Index : 0;
			});
		Input.HitKeyProcess(VK_RIGHT, KeyState::Down, [&] {
			Skin.Base->SongSelect.SE.Ka.Play();
			Result.Index < Private.CountAll - 1 ? ++Result.Index : Result.Index;
			});

		if (!MultiRoom.IsHost) {
			if (Public.HitKey == HitType::Back) {
				NowScene = Scene::MultiRoom;
			}
		}
	}
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

void _Result::AccuracyDraw(const Pos2D<float>& Pos, double Rate) {
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
