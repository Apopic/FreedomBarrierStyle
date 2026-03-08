#pragma once
#include "Include.hpp"
#include "../../resource.h"

extern class _SongSelect;
extern class GameSystem;

class _Skin {
public:

	_Skin(GameSystem* ptr);
	~_Skin();

	bool Loading = false;

	void SkinLoad(const std::string& name);
	void SkinDispose();

	void* loadZipResource(int assetid) {

		HMODULE hModule = GetModuleHandle(NULL);
		HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(assetid), "ZIPFILE");
		if (hRes == NULL) {
			return nullptr;
		}

		HGLOBAL hGlobal = LoadResource(hModule, hRes);
		if (hGlobal == NULL) {
			return nullptr;
		}

		return LockResource(hGlobal);
	}

	struct SkinInfo {
		std::string Name;
		std::string Path;
	};
	std::vector<SkinInfo> SkinInfos;
	std::string SkinFilePath;

	Rect2D<float> ResolutionRect;
	Rect2D<float> SimulationRect;

	struct Info {
		std::string Name = "Skin";
		std::string Version = "Default";
		Pos2D<int> Resolution = { 1280,720 };
		Pos2D<float> SimulationDistance = { 960,540 };
		int ColorBit = 32;
	} Info;

	struct _Base {
		struct _Title {
			struct _Config {
				Pos2D<float> TitlePos = { 640,360 };
				Pos2D<float> CaptionPos = { 640,600 };
				Pos2D<float> ModesPos = { 640,486 };
				float ModesLineHeight = 32;
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
				Pos2D<float> BoxDistance = { 40,80 };
				Pos2D<float> SongBoxListPos = { 640,360 };
			} Config;
			struct _Image {
				GraphData BackGround;
				GraphData Box;
				GraphData TitleBox;
				GraphData LevelBox;
				GraphData Crown;
			} Image;
			struct _Font {
				FontData Title;
				FontData SubTitle;
				FontData BoxTitle;
				FontData BoxSubTitle;
				FontData Course;
				FontData HighScore;
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
				Pos2D<float> TitlePos = { 1260,50 };
				Pos2D<float> SubTitlePos = { 1260,90 };
				Pos2D<float> PlayerNamePos = { 80, 320 };
				Pos2D<float> ExamNamePos = { 160, 380 };
				Pos2D<float> ExamValPos = { 160, 420 };
				double LaneExtendRate = 0.948;
				double JudgeUpperExplosionFrameTime = 12;
				double GoGoFireFrameTime = 60;
				bool BranchSlideAnimation = true;
				double BranchSlideTime = 120;
			} Config;
			struct _Image {
				GraphData Box;
				GraphData BackGround;
				GraphData LaneFrame;
				GraphData Lane;
				GraphData NormalLane;
				GraphData ExpertLane;
				GraphData MasterLane;
				GraphData Base;
				GraphData NamePlate;
				GraphData MiniTaiko;
				GraphData MiniTaiko_Don;
				GraphData MiniTaiko_Ka;
				GraphData ComboNumber;
				GraphData ScoreNumber;
				GraphData BalloonNumber;
				GraphData Note;
				GraphData JudgeUnderExplosion;
				GraphData JudgeUpperExplosion;
				GraphData GoGoFire;
				GraphData JudgeString;
				GraphData ProgressBar;
				GraphData ExamProgressBar;
			} Image;
			struct _Font {
				FontData Title;
				FontData SubTitle;
				FontData PlayerName;
				FontData ExamName;
				FontData ExamVal;
			} Font;
			struct _SE {
				SoundData Don;
				SoundData Ka;
				SoundData Balloon;
				SoundData DanFall;
			} SE;
			struct _BGM {
			} BGM;
		} Playing;
		struct _Result {
			struct _Config {
				Pos2D<float> TitlePos = { 25,75 };
				Pos2D<float> SubTitlePos = { 25,125 };
				Pos2D<float> PlayerNamePos = { 860,360 };
				Pos2D<float> ScorePos = { 290,200 };
				Pos2D<float> AccuracyPos = { 480,200 };
				Pos2D<float> GoodPos = { 230,311 };
				Pos2D<float> OkPos = { 230,370 };
				Pos2D<float> BadPos = { 230,429 };
				Pos2D<float> RollPos = { 230,524 };
				Pos2D<float> MaxComboPos = { 230,583 };
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
				FontData PlayerName;
			} Font;
			struct _SE {
				SoundData Don;
				SoundData Ka;
			} SE;
			struct _BGM {

			} BGM;
		} Result;
		struct _MultiRoom {
			struct _Config {
				Pos2D<float> PlayerNamePos = { 240,355 };
				Pos2D<float> HiddenPos = { 400,360 };
				Pos2D<float> SuddenPos = { 500,360 };
				Pos2D<float> RandomPos = { 600,360 };
				Pos2D<float> GoodPos = { 700,360 };
				Pos2D<float> OkPos = { 800,360 };
				Pos2D<float> BadPos = { 900,360 };
				Pos2D<float> ChartSpeedPos = { 1020,360 };
				Pos2D<float> SongSpeedPos = { 1150,360 };
			} Config;
			struct _Image {
				GraphData BackGround;
				GraphData TitleBox;
				GraphData PlayersBox;
				GraphData Crown;
			} Image;
			struct _Font {
				FontData PlayerName;
				FontData OptionData;
			} Font;
		} MultiRoom;
		struct _Other {
			struct _Font {
				FontData Game;
			} Font;
		} Other;
	} *Base = nullptr;
};
