#pragma once
#include "Include.hpp"
#include "Playing.h"

extern class GameSystem;

class _Result {
public:

	_Result(GameSystem* ptr);
	~_Result();

	_Skin* skinptr = nullptr;

	int Index = 0;

	std::string GenUID() {
		ulonglong high = (ulonglong)GetRand(INT_MAX);
		ulonglong low = (ulonglong)GetRand(INT_MAX);
		ulonglong result = (high << 32) | low;
		return std::to_string(result);
	}

	void ScoreDraw(const Pos2D<float>& Pos, ulonglong Num) {
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

	void AccuracyDraw(const Pos2D<float>& Pos, double Rate) {
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

	void JudgesDraw(const Pos2D<float>& Pos, ulonglong Num) {

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

	JudgeData ScoreDataLoad(std::string ChartID, std::string Course, std::string FileDir) {

		auto&& LoadData = JudgeData();

		std::ifstream ifs(FileDir + "/scoredata.json", std::ios::binary);

		if (!ifs.is_open()) {
			ifs.close();
			return LoadData;
		}

		std::vector<std::uint8_t> bson_vec((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		json data = json::from_bson(bson_vec);

		if(data.contains(ChartID) && data[ChartID].contains(Course)) {

#define JSONDATA(name) LoadData.name = data[ChartID][Course][#name]
			JSONDATA(Score);
			JSONDATA(Accuracy);
			JSONDATA(Good);
			JSONDATA(Ok);
			JSONDATA(Bad);
			JSONDATA(Roll);
			JSONDATA(MaxCombo);
			JSONDATA(ScoreRateGood);
#undef JSONDATA

			ifs.close();

		}
		
		return LoadData;
	}

	void ScoreDataSave(std::string ChartID, std::string Course, JudgeData SaveData, std::string FileDir) {

		std::ifstream ifs(FileDir + "/scoredata.json", std::ios::binary);
		json data;

		if (!ifs.is_open()) {
			ifs.close();
			std::ofstream ofs(FileDir + "/scoredata.json");
			data = json::object();
		}
		else {
			std::vector<std::uint8_t> bson_vec((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			data = json::from_bson(bson_vec);
			ifs.close();
		}

#define JSONDATA(name) data[ChartID][Course][#name] = SaveData.name
		    JSONDATA(Score);
			JSONDATA(Accuracy);
			JSONDATA(Good);
			JSONDATA(Ok);
			JSONDATA(Bad);
			JSONDATA(Roll);
			JSONDATA(MaxCombo);
			JSONDATA(ScoreRateGood);
#undef JSONDATA

		std::vector<std::uint8_t> v_bson = json::to_bson(data);

		std::ofstream ofs(FileDir + "/scoredata.json", std::ios::binary);
		ofs.write(reinterpret_cast<const char*>(v_bson.data()), v_bson.size());
		ofs.close();
	}
};
