#include "Skin.h"
#include "GameSystem.h"

_Skin::_Skin(GameSystem *ptr) {
	for (const auto& f : fs::recursive_directory_iterator("Skins")) {
		if (f.path().extension() == ".json") {
			std::ifstream ifs(f.path());

			json data = json::parse(ifs);

			if (data.find("Info") == data.end()) {
				continue;
			}

			SkinInfos.push_back(
				SkinInfo{
					data["Info"]["Name"],
					f.path().string()
				}
			);

			ifs.close();
		}
	}
}

_Skin::~_Skin() {
	SkinDispose();
}

void GameSystem::SkinInit() {

}

void GameSystem::SkinEnd() {

}

void GameSystem::SkinDraw() {
	
}

void GameSystem::SkinProc() {

}

void _Skin::SkinLoad(const std::string& name) {
	Loading = true;
	for (uint i = 0, size = SkinInfos.size(); i < size; ++i) {
		if (name == SkinInfos[i].Name) {
			SkinFilePath = SkinInfos[i].Path;
			break;
		}
	}

	std::ifstream ifs(SkinFilePath);
	json data = json::parse(ifs);
	ifs.close();

	std::string SkinDir = fs::path(SkinFilePath).parent_path().string() + "\\";

	const json& Infodata = data["Info"];
	const json& Titledata = data["Base"]["Title"];
	const json& SongSelectdata = data["Base"]["SongSelect"];
	const json& Playingdata = data["Base"]["Playing"];
	const json& Resultdata = data["Base"]["Result"];
	const json& MultiRoomdata = data["Base"]["MultiRoom"];
	const json& Otherdata = data["Base"]["Other"];

	// structResource

	Info.Name = Infodata["Name"];
	Info.Version = Infodata["Version"];
	Info.Resolution = Infodata["Resolution"].get<Pos2D<int>>();
	Info.SimulationDistance = Infodata["SimulationDistance"].get<Pos2D<float>>();
	Info.ColorBit = Infodata["ColorBit"];

	SimulationRect = Rect2D<float>{
		Info.Resolution.Y * 0.5f - Info.SimulationDistance.Y,
		Info.Resolution.Y * 0.5f + Info.SimulationDistance.Y,
		Info.Resolution.X * 0.5f - Info.SimulationDistance.X,
		Info.Resolution.X * 0.5f + Info.SimulationDistance.X,
	};

	ResolutionRect = Rect2D<float>{
		0,
		(float)Info.Resolution.Y,
		0,
		(float)Info.Resolution.X,
	};

	SetGraphMode(
		Info.Resolution.X,
		Info.Resolution.Y,
		Info.ColorBit
	);
	SetDrawMode(DX_DRAWMODE_BILINEAR);
	SetDrawScreen(DX_SCREEN_BACK);

	if (!DxLib_IsInit()) {
		return;
	}

#define ValLoad(base, type, keyname) Base->base.type.keyname = base##data[#type].value(#keyname, Base->base.type.keyname)
#define DataLoad(base, type, keyname) Base->base.type.keyname.Load(SkinDir, base##data[#type][#keyname])

	SkinDispose();
	Base = new _Base();

#pragma region Base

#pragma region Title

	ValLoad(Title, Config, TitlePos       );
	ValLoad(Title, Config, CaptionPos     );
	ValLoad(Title, Config, ModesPos       );
	ValLoad(Title, Config, ModesLineHeight);

	DataLoad(Title, Image, BackGround);
	DataLoad(Title, Image, BackPlate_Small);
	DataLoad(Title, Image, BackPlate_Big);
	DataLoad(Title, Image, Selector);

	DataLoad(Title, Font, Title);
	DataLoad(Title, Font, Caption);
	DataLoad(Title, Font, Modes);

	DataLoad(Title, SE, Don);
	DataLoad(Title, SE, Ka);

#pragma endregion

#pragma region SongSelect

	ValLoad(SongSelect, Config, BoxDistance   );
	ValLoad(SongSelect, Config, SongBoxListPos);

	DataLoad(SongSelect, Image, BackGround);
	DataLoad(SongSelect, Image, Box);
	DataLoad(SongSelect, Image, TitleBox);
	DataLoad(SongSelect, Image, LevelBox);
	DataLoad(SongSelect, Image, Crown);

	DataLoad(SongSelect, Font, Title);
	DataLoad(SongSelect, Font, SubTitle);
	DataLoad(SongSelect, Font, BoxTitle);
	DataLoad(SongSelect, Font, BoxSubTitle);
	DataLoad(SongSelect, Font, Course);
	DataLoad(SongSelect, Font, HighScore);

	DataLoad(SongSelect, SE, Don);
	DataLoad(SongSelect, SE, Ka);

#pragma endregion

#pragma region Playing

	ValLoad(Playing, Config, TitlePos   );
	ValLoad(Playing, Config, SubTitlePos);
	ValLoad(Playing, Config, PlayerNamePos);
	ValLoad(Playing, Config, ExamNamePos);
	ValLoad(Playing, Config, ExamValPos);
	ValLoad(Playing, Config, LaneExtendRate);
	ValLoad(Playing, Config, JudgeUpperExplosionFrameTime);
	ValLoad(Playing, Config, GoGoFireFrameTime);
	ValLoad(Playing, Config, BranchSlideAnimation);
	ValLoad(Playing, Config, BranchSlideTime);
	ValLoad(Playing, Config, KeyInputView);

	DataLoad(Playing, Image, Box);
	DataLoad(Playing, Image, BackGround);
	DataLoad(Playing, Image, LaneFrame);
	DataLoad(Playing, Image, Lane);
	DataLoad(Playing, Image, NormalLane);
	DataLoad(Playing, Image, ExpertLane);
	DataLoad(Playing, Image, MasterLane);
	DataLoad(Playing, Image, Base);
	DataLoad(Playing, Image, NamePlate);
	DataLoad(Playing, Image, MiniTaiko);
	DataLoad(Playing, Image, MiniTaiko_Don);
	DataLoad(Playing, Image, MiniTaiko_Ka);
	DataLoad(Playing, Image, ComboNumber);
	DataLoad(Playing, Image, ScoreNumber);
	DataLoad(Playing, Image, BalloonNumber);
	DataLoad(Playing, Image, Note);
	DataLoad(Playing, Image, JudgeUnderExplosion);
	DataLoad(Playing, Image, JudgeUpperExplosion);
	DataLoad(Playing, Image, GoGoFire);
	DataLoad(Playing, Image, JudgeString);
	DataLoad(Playing, Image, ProgressBar);
	DataLoad(Playing, Image, ExamProgressBar);
	DataLoad(Playing, Image, KeyViewBack);
	DataLoad(Playing, Image, KeyViewFlash);

	DataLoad(Playing, Font, Title);
	DataLoad(Playing, Font, SubTitle);
	DataLoad(Playing, Font, PlayerName);
	DataLoad(Playing, Font, KeyStr);
	DataLoad(Playing, Font, ExamName);
	DataLoad(Playing, Font, ExamVal);

	DataLoad(Playing, SE, Don);
	DataLoad(Playing, SE, Ka);
	DataLoad(Playing, SE, Balloon);
	DataLoad(Playing, SE, DanFall);

#pragma endregion

#pragma region Result

	ValLoad(Result, Config, TitlePos   );
	ValLoad(Result, Config, SubTitlePos);
	ValLoad(Result, Config, PlayerNamePos);
	ValLoad(Result, Config, JudgeDataPos );

	DataLoad(Result, Image, BackGround);
	DataLoad(Result, Image, JudgeScore);
	DataLoad(Result, Image, Accuracy);
	DataLoad(Result, Image, Score);
	DataLoad(Result, Image, Number);
	DataLoad(Result, Image, Crown);

	DataLoad(Result, Font, Title);
	DataLoad(Result, Font, SubTitle);
	DataLoad(Result, Font, PlayerName);

	DataLoad(Result, SE, Don);
	DataLoad(Result, SE, Ka);

#pragma endregion

#pragma region MultiRoom

	ValLoad(MultiRoom, Config, PlayerNamePos);
	ValLoad(MultiRoom, Config, OptionDataPos);

	DataLoad(MultiRoom, Image, BackGround);
	DataLoad(MultiRoom, Image, TitleBox);
	DataLoad(MultiRoom, Image, PlayersBox);
	DataLoad(MultiRoom, Image, Crown);

	DataLoad(MultiRoom, Font, PlayerName);
	DataLoad(MultiRoom, Font, OptionData);

#pragma endregion

#pragma region Other

	DataLoad(Other, Font, Game);

#pragma endregion

#pragma endregion

#undef ConfLoad
#undef DataLoad

	Loading = false;
}

void _Skin::SkinDispose() {
	if (Base != nullptr) {
		delete Base;
	}
}