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
					f.path().u8string()
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

	std::string SkinDir = fs::path(SkinFilePath).parent_path().u8string() + "\\";

	const json& Infodata = data["Info"];
	const json& Titledata = data["Base"]["Title"];
	const json& SongSelectdata = data["Base"]["SongSelect"];
	const json& Playingdata = data["Base"]["Playing"];
	const json& Resultdata = data["Base"]["Result"];

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

#define ValLoad(base, type, keyname) Base->base.type.keyname = base##data[#type][#keyname]
#define DataLoad(base, type, keyname) Base->base.type.keyname.Load(SkinDir, base##data[#type][#keyname])

	SkinDispose();
	Base = new _Base();

#pragma region Base

#pragma region Title

	ValLoad(Title, Config, TitlePos       ).get<Pos2D<float>>();
	ValLoad(Title, Config, CaptionPos     ).get<Pos2D<float>>();
	ValLoad(Title, Config, ModesPos       ).get<Pos2D<float>>();
	ValLoad(Title, Config, ModesLineHeight);

	Base->Title.Image.BackGround.Load(SkinDir, Titledata["Image"]["BackGround"]);
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

	ValLoad(SongSelect, Config, BoxDistance   ).get<Pos2D<float>>();
	ValLoad(SongSelect, Config, SongBoxListPos).get<Pos2D<float>>();

	DataLoad(SongSelect, Image, BackGround);
	DataLoad(SongSelect, Image, Box);

	DataLoad(SongSelect, Font, Title);
	DataLoad(SongSelect, Font, SubTitle);

	DataLoad(SongSelect, SE, Don);
	DataLoad(SongSelect, SE, Ka);

#pragma endregion

#pragma region Playing

	ValLoad(Playing, Config, TitlePos   ).get<Pos2D<float>>();
	ValLoad(Playing, Config, SubTitlePos).get<Pos2D<float>>();
	ValLoad(Playing, Config, LaneExtendRate);
	ValLoad(Playing, Config, JudgeUpperExplosionFrameTime);

	DataLoad(Playing, Image, BackGround);
	DataLoad(Playing, Image, LaneFrame);
	DataLoad(Playing, Image, Lane);
	DataLoad(Playing, Image, Base);
	DataLoad(Playing, Image, NamePlate);
	DataLoad(Playing, Image, MiniTaiko);
	DataLoad(Playing, Image, MiniTaiko_Don);
	DataLoad(Playing, Image, MiniTaiko_Ka);
	DataLoad(Playing, Image, ComboNumber);
	DataLoad(Playing, Image, Note);
	DataLoad(Playing, Image, JudgeUnderExplosion);
	DataLoad(Playing, Image, JudgeUpperExplosion);

	DataLoad(Playing, Font, Title);
	DataLoad(Playing, Font, SubTitle);

	DataLoad(Playing, SE, Don);
	DataLoad(Playing, SE, Ka);
	DataLoad(Playing, SE, Balloon);

#pragma endregion

#pragma region Result

	ValLoad(Result, Config, TitlePos   ).get<Pos2D<float>>();
	ValLoad(Result, Config, SubTitlePos).get<Pos2D<float>>();
	ValLoad(Result, Config, ScorePos   ).get<Pos2D<float>>();
	ValLoad(Result, Config, AccurecyPos).get<Pos2D<float>>();
	ValLoad(Result, Config, GoodPos    ).get<Pos2D<float>>();
	ValLoad(Result, Config, OkPos      ).get<Pos2D<float>>();
	ValLoad(Result, Config, BadPos     ).get<Pos2D<float>>();
	ValLoad(Result, Config, RollPos    ).get<Pos2D<float>>();
	ValLoad(Result, Config, MaxComboPos).get<Pos2D<float>>();

	DataLoad(Result, Image, BackGround);
	DataLoad(Result, Image, JudgeScore);
	DataLoad(Result, Image, Accuracy);
	DataLoad(Result, Image, Score);
	DataLoad(Result, Image, Number);
	DataLoad(Result, Image, Crown);

	DataLoad(Result, Font, Title);
	DataLoad(Result, Font, SubTitle);

	DataLoad(Result, SE, Don);
	DataLoad(Result, SE, Ka);

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

