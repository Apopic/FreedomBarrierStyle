#pragma once
#include "Include.hpp"

extern class _SongSelect;
extern class GameSystem;

class _Skin {
public:

	_Skin(GameSystem* ptr);
	~_Skin();

	bool Loading = false;

	void SkinLoad(const std::string& name);
	void SkinDispose();

	struct SkinInfo {
		std::string Name;
		std::string Path;
	};
	std::vector<SkinInfo> SkinInfos;
	std::string SkinFilePath;

	Rect2D<float> ResolutionRect;
	Rect2D<float> SimulationRect;

	struct Info {
		std::string Name = "";
		std::string Version = "";
		Pos2D<int> Resolution;
		Pos2D<float> SimulationDistance;
		int ColorBit = 0;
	} Info;

	struct _Base {
		struct _Title {
			struct _Config {
				Pos2D<float> TitlePos;
				Pos2D<float> CaptionPos;
				Pos2D<float> ModesPos;
				float ModesLineHeight = 0;
			} Config;
			struct _Image {
				GraphData BackGround;
				GraphData BackPlate_Small;
				GraphData BackPlate_Big;
				GraphData Selector;
			} Image;
			struct _Font {
				FontData Title;
				FontData Caption;
				FontData Modes;
			} Font;
			struct _SE {
				SoundData Don;
				SoundData Ka;
			} SE;
			struct _BGM {

			} BGM;
		} Title;
		struct _SongSelect {
			struct _Config {
				Pos2D<float> BoxDistance;
				Pos2D<float> SongBoxListPos;
			} Config;
			struct _Image {
				GraphData BackGround;
				GraphData Box;
			} Image;
			struct _Font {
				FontData Title;
				FontData SubTitle;
			} Font;
			struct _SE {
				SoundData Don;
				SoundData Ka;
			} SE;
			struct _BGM {

			} BGM;
		} SongSelect;
		struct _Playing {
			struct _Config {
				Pos2D<float> TitlePos;
				Pos2D<float> SubTitlePos;
				double LaneExtendRate = 0;
				double JudgeUpperExplosionFrameTime = 0;
			} Config;
			struct _Image {
				GraphData BackGround;
				GraphData LaneFrame;
				GraphData Lane;
				GraphData Base;
				GraphData NamePlate;
				GraphData MiniTaiko;
				GraphData MiniTaiko_Don;
				GraphData MiniTaiko_Ka;
				GraphData ComboNumber;
				GraphData Note;
				GraphData JudgeUnderExplosion;
				GraphData JudgeUpperExplosion;
			} Image;
			struct _Font {
				FontData Title;
				FontData SubTitle;
			} Font;
			struct _SE {
				SoundData Don;
				SoundData Ka;
				SoundData Balloon;
			} SE;
			struct _BGM {

			} BGM;
		} Playing;
		struct _Result {
			struct _Config {
				Pos2D<float> TitlePos;
				Pos2D<float> SubTitlePos;

				Pos2D<float> ScorePos;
				Pos2D<float> AccurecyPos;
				Pos2D<float> GoodPos;
				Pos2D<float> OkPos;
				Pos2D<float> BadPos;
				Pos2D<float> RollPos;
				Pos2D<float> MaxComboPos;
			} Config;
			struct _Image {
				GraphData BackGround;
				GraphData JudgeScore;
				GraphData Accuracy;
				GraphData Score;
				GraphData Number;
				GraphData Crown;
			} Image;
			struct _Font {
				FontData Title;
				FontData SubTitle;
			} Font;
			struct _SE {
				SoundData Don;
				SoundData Ka;
			} SE;
			struct _BGM {

			} BGM;
		} Result;
	} *Base = nullptr;
};
