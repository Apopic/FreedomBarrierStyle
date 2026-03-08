#pragma once
#include "Include.hpp"
#include "Playing.h"

extern class GameSystem;

class _Result {
public:

	_Result(GameSystem* ptr);
	~_Result();

	int Index = 0;

	void ScoreDraw(const Pos2D<float> &Pos, ulonglong Num);
	void AccuracyDraw(const Pos2D<float> &Pos, double Rate);
	void JudgesDraw(const Pos2D<float> &Pos, ulonglong Num);

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
