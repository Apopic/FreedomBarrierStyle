#pragma once
#include "Include.hpp"
#include "Config.h"
#include "Skin.h"

enum class ChartCourseType {
	Null = -1,
	Easy,
	Normal,
	Hard,
	Oni,
	Edit,
	Count,
};

struct CourseData {
	uint CourseIndex = 0;
	int Level = 0;
	int AddScore = 0;
	std::vector<int> Balloon;
	bool IsPlayFlag = 0;
};

struct ChartData {
	ChartData() {};
	ChartData(const std::string& path, _Skin* _skinptr, _Config* _configptr) { this->ReLoad(path, _skinptr, _configptr); }

	std::string FilePath;
	std::string WavePath;
	
	std::string Title;
	std::string SubTitle;
	int TitleStrlen = 0;
	int SubTitleStrlen = 0;
	int PlayingTitleStrlen = 0;
	int PlayingSubTitleStrlen = 0;
	int ResultTitleStrlen = 0;
	int ResultSubTitleStrlen = 0;
	bool TitleDisplay = true;
	bool SubTitleDisplay = true;
	double Offset = 0;
	double BPM = 0;
	int SongVolume = 100;
	int SEVolume = 100;

	CourseData CourseDatas[(int)ChartCourseType::Count];

	UINT GetChartCodePage(const std::string& path) const {
		FileAccess FA(path);
		if (!FA.GetIsOpen()) {
			return DX_CHARCODEFORMAT_UTF8;
		}

		const UINT CodePage[6]{
			DX_CHARCODEFORMAT_UTF8,
			DX_CHARCODEFORMAT_SHIFTJIS,
			DX_CHARCODEFORMAT_UTF16BE,
			DX_CHARCODEFORMAT_UTF16LE,
			DX_CHARCODEFORMAT_UTF32BE,
			DX_CHARCODEFORMAT_UTF32LE,
		};

		int ret = DX_CHARCODEFORMAT_UTF8;
		bool breakflag = false;

		for (uint i = 0; i < FA.LineCount(); ++i) {
			Exsubstr(FA[i], "WAVE:", [&](const std::string& data) {
				
				for (uint j = 0; j < 6; ++j) {
					std::string wpath = std::filesystem::path(path).parent_path().u8string() + "\\" + cp_to_utf8(data, CodePage[j]);
					if (FilePathExists(wpath)) {
						ret = CodePage[j];
						breakflag = true;
						break;
					}
				}
				});
			if (breakflag) {
				break;
			}
		}

		return ret;
	}
	void ReLoad(const std::string& path, _Skin* _skinptr, _Config* _configptr) {
		FileAccess FA(path, FAO::rc_slash);
		if (!FA.GetIsOpen()) { return; }

		FilePath = path;

		FA.CodePageToUTF8(GetChartCodePage(path));
		FA.Write();

		int level = 0;
		int addscore = -1;
		ChartCourseType course = ChartCourseType::Null;
		uint courseindex = 0;
		std::vector<int> balloon;

		for (uint i = 0; i < FA.LineCount(); ++i) {
			Exsubstr(FA[i], "TITLE:", [&](std::string data) {
				if (data.find("--") == 0) {
					data = data.replace(data.find("--"), 2, "");
					this->TitleDisplay = false;
				}
				this->Title = data.c_str();
				this->TitleStrlen = GetDrawStringWidthToHandle(Title.c_str(), strlenDx(this->Title.c_str()), _skinptr->Base->SongSelect.Font.Title.Handle);
				this->PlayingTitleStrlen = GetDrawStringWidthToHandle(Title.c_str(), strlenDx(this->Title.c_str()), _skinptr->Base->Playing.Font.Title.Handle);
				this->ResultTitleStrlen = GetDrawStringWidthToHandle(Title.c_str(), strlenDx(this->Title.c_str()), _skinptr->Base->Result.Font.Title.Handle);
				});
			Exsubstr(FA[i], "SUBTITLE:", [&](std::string data) {
				if (data.find("--") == 0) {
					data = data.replace(data.find("--"), 2, "");
					this->SubTitleDisplay = false;
				}
				this->SubTitle = data.c_str();
				this->SubTitleStrlen = GetDrawStringWidthToHandle(SubTitle.c_str(), strlenDx(this->SubTitle.c_str()), _skinptr->Base->SongSelect.Font.SubTitle.Handle);
				this->PlayingSubTitleStrlen = GetDrawStringWidthToHandle(SubTitle.c_str(), strlenDx(this->SubTitle.c_str()), _skinptr->Base->Playing.Font.SubTitle.Handle);
				this->ResultSubTitleStrlen = GetDrawStringWidthToHandle(SubTitle.c_str(), strlenDx(this->SubTitle.c_str()), _skinptr->Base->Result.Font.SubTitle.Handle);
				});
			Exsubstr(FA[i], "BPM:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				this->BPM = stod(data);
				});
			Exsubstr(FA[i], "WAVE:", [&](const std::string& data) {
				this->WavePath = std::filesystem::path(path).parent_path().u8string() + "\\" + data;
				});
			Exsubstr(FA[i], "OFFSET:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				this->Offset = stod(data);
				});
			Exsubstr(FA[i], "SONGVOL:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				this->SongVolume = stoi(data);
				});
			Exsubstr(FA[i], "SEVOL:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				this->SEVolume = stoi(data);
				});

			Exsubstr(FA[i], "COURSE:", [&](const std::string& data) {
				course = ChartCourseType::Null;
				std::string _val;
				courseindex = i;
				for (size_t j = 0, size = data.size(); j < size; ++j) {
					_val += (char)tolower(data[j]);
				}
				if (_val == "easy") {
					course = ChartCourseType::Easy;
				}
				else if (_val == "normal") {
					course = ChartCourseType::Normal;
				}
				else if (_val == "hard") {
					course = ChartCourseType::Hard;
				}
				else if (_val == "oni") {
					course = ChartCourseType::Oni;
				}
				else if (_val == "edit") {
					course = ChartCourseType::Edit;
				}

				if ((int)course != -1) {
					return;
				}
				course = (ChartCourseType)stoi(data);
				});
			Exsubstr(FA[i], "LEVEL:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				level = stoi(data);
				});
			Exsubstr(FA[i], "SCOREINIT:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				addscore = stoi(data);
				});
			Exsubstr(FA[i], "BALLOON:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				auto datas = split(data, ',');

				for (size_t i = 0; i < datas.size(); ++i) {
					if (!datas[i].empty()) {
						balloon.push_back(stoi(datas[i]));
					}
				}

				});

			Exsubstr(FA[i], "#START", [&](const std::string& data) {
				if ((int)course == -1) {
					course = ChartCourseType::Oni;
				}
				this->CourseDatas[(int)course].CourseIndex = courseindex;
				this->CourseDatas[(int)course].Level = level;
				this->CourseDatas[(int)course].IsPlayFlag = true;
				this->CourseDatas[(int)course].AddScore = addscore < 0 ? 0 : addscore;
				this->CourseDatas[(int)course].Balloon = balloon;
				balloon = std::vector<int>();
				});
		}
	}
};

struct BoxData;
struct GenreData {
	GenreData() {}
	GenreData(std::string path) { this->ReLoad(path); }

	bool Open = false;

	std::string FilePath = "";
	std::string Name = "";
	std::string Caption = "";
	Color3<int> GenreColor{};
	Color3<int> FontColor{};
	Color3<int> FontEdgeColor{};

	std::vector<std::unique_ptr<BoxData>> Datas;

	void ReLoad(std::string path) {
		FilePath = path;

		FileAccess FA(path);

		auto ColorCodeParse = [&] (std::string data) -> Color3<int> {
			data.replace(0, 1, "0x");
			int color = stoi(data, nullptr, 16);
			int r = 0, g = 0, b = 0;
			GetColor2(color, &r, &g, &b);
			return { (r - 128) * 2, (g - 128) * 2, (b - 128) * 2 };
			};

		for (size_t i = 0; i < FA.LineCount(); ++i) {
			Exsubstr(FA[i], "GenreName=", [&] (const std::string& data) {
				Name = data;
				});
			Exsubstr(FA[i], "GenreCaption=", [&] (const std::string& data) {
				Caption = data;
				});
			Exsubstr(FA[i], "GenreColor=", [&] (const std::string& data) {
				GenreColor = ColorCodeParse(data);
				});
			Exsubstr(FA[i], "FontColor=", [&] (const std::string& data) {
				FontColor = ColorCodeParse(data);
				});
			Exsubstr(FA[i], "FontEdgeColor=", [&] (const std::string& data) {
				FontEdgeColor = ColorCodeParse(data);
				});
		}
	}
};

struct BoxData {
	BoxData() {}
	BoxData(ChartData* ptr) {
		delete m_Genre;
		m_Genre = nullptr;
		m_Chart = ptr;
	}
	BoxData(GenreData* ptr) {
		delete m_Chart;
		m_Chart = nullptr;
		m_Genre = ptr;
	}
	~BoxData() {
		delete m_Chart;
		delete m_Genre;
	}

	bool IsGenre() {
		return m_Genre != nullptr;
	};

	Color3<int> GenreColor{};
	Color3<int> FontColor{};
	Color3<int> FontEdgeColor{};

	ChartData* GetChart() {
		return m_Chart;
	};
	GenreData* GetGenre() {
		return m_Genre;
	};

	void SetBoxColor() {
		if (m_Genre == nullptr) {
			return;
		}
		GenreColor = m_Genre->GenreColor;
		FontColor = m_Genre->FontColor;
		FontEdgeColor = m_Genre->FontEdgeColor;
	};
	void SetBoxColor(const GenreData& data) {
		GenreColor = data.GenreColor;
		FontColor = data.FontColor;
		FontEdgeColor = data.FontEdgeColor;
	};

private:

	ChartData* m_Chart = nullptr;
	GenreData* m_Genre = nullptr;
};

extern class GameSystem;

class _SongSelect {
public:

	_SongSelect(GameSystem* ptr);
	~_SongSelect();

	_Skin* __SkinPtr = nullptr;
	_Config* __ConfigPtr = nullptr;

	bool MultiFlag = false;
	bool DanFlag = false;

	std::vector<std::unique_ptr<BoxData>> __BoxDatas;
	std::vector<BoxData*> BoxDatas;

	int BoxDataIndex = 0;
	int BoxMotionDirection = 0;
	Timer<nanosecond> BoxMotion = Timer<nanosecond>(false);
	bool TimeModify = false;
	double UseBoxMotionTime = 0;
	inline double BoxMotionTime() const  {
		return 100000000 / std::pow(1.125, std::abs(BoxMotionDirection) - 1);
	}
	
	Timer<millisecond> DemoSongPlayBlank = Timer<millisecond>(false);
	double DemoSongPlayBlankTime() const  {
		return 1000;
	}
	SoundData DemoSong;

	bool CourseSelect = false;
	int CourseIndex = 0;
	std::string CourseList[5]{"Easy","Normal","Hard","Oni","Edit"};

	void EnumChartFile(const std::vector<std::string>& dir) {
		__BoxDatas.clear();
		__BoxDatas.reserve(dir.capacity());
		int LoadCounter = 0;

		auto recusiveproc = [&](std::vector<std::unique_ptr<BoxData>>& data, const std::filesystem::path& dirpath, const std::filesystem::path& genrepath, auto& f, const GenreData& genredata = {}) -> void {
			std::filesystem::path _genrepath = "";
			auto fpit = std::filesystem::directory_iterator(dirpath);
			bool genreflag = false;
			for (const auto& fp : fpit) {
				if (fp == genrepath) {
					break;
				}
				if (fp.path().filename() == "genre.ini") {
					_genrepath = fp.path();
					BoxData* _data = new BoxData(new GenreData(fp.path().u8string()));
					_data->SetBoxColor();
					data.push_back(std::unique_ptr<BoxData>(_data));
					f(data.back()->GetGenre()->Datas, fp.path().parent_path(), _genrepath, f, *data.back()->GetGenre());
					genreflag = true;
					break;
				}
			}
			if (!genreflag) {
				fpit = std::filesystem::directory_iterator(dirpath);
				for (const auto& fp : fpit) {
					if (fp.path().extension() == ".tja") {
						BoxData* _data = new BoxData(new ChartData(fp.path().u8string(), __SkinPtr, __ConfigPtr));
						_data->SetBoxColor(genredata);
						data.push_back(std::unique_ptr<BoxData>(_data));
					}
					if (fp.is_directory()) {
						f(data, fp.path(), "", f, genredata);
					}
				}
			}
			};

		for (uint i = 0, size = dir.size(); i < size; ++i) {
			recusiveproc(__BoxDatas, dir[i], "", recusiveproc);
		}

		BoxDatasUpdate();

		BoxDataIndex = std::clamp<int>(BoxDataIndex, 0, BoxDatas.size() - 1);
	}
	void BoxDatasUpdate() {
		BoxDatas.clear();
		BoxDatas.reserve(__BoxDatas.capacity());
		auto recusiveproc = [&] (const std::vector<std::unique_ptr<BoxData>>& datas, auto f) -> void {
			for (uint i = 0; i < datas.size(); ++i) {
				if (datas[i]->IsGenre()) {
					BoxDatas.push_back(datas[i].get());
					if (datas[i]->GetGenre()->Open) {
						f(datas[i]->GetGenre()->Datas, f);
					}
				}
			}
			for (uint i = 0; i < datas.size(); ++i) {
				if (datas[i]->IsGenre()) { continue; }
				BoxDatas.push_back(datas[i].get());
			}
			};
		recusiveproc(__BoxDatas, recusiveproc);
	};
};


