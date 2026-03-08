#pragma once
#include "Include.hpp"
#include "Config.h"
#include "Skin.h"
#include "cppunzip.hpp"
#include <shobjidl.h>
#include <urlmon.h>

#pragma comment(lib, "urlmon.lib")

using namespace cppunzip;

enum class ChartCourseType {
	Null = -1,
	Easy,
	Normal,
	Hard,
	Oni,
	Edit,
	Tower,
	Dan,
	Count,
};

enum class ExamTypes {
	Null = -1,
	Accuracy,
	Good,
	Ok,
	Bad,
	Score,
	Roll,
	HitNote,
	MaxCombo,
};

enum class ExamRange {
	Null = -1,
	More,
	Less,
};

struct CourseData {
	uint CourseIndex = 0;
	int Level = 0;
	int AddScore = 0;
	std::vector<int> Balloon;
	bool IsPlayFlag = 0;

	Packet::bytearray ToBytes() const {
		Packet::bytearray ret;
		Packet::StoreBytes(ret, CourseIndex);
		Packet::StoreBytes(ret, Level);
		Packet::StoreBytes(ret, AddScore);
		Packet::StoreBytes(ret, Balloon);
		Packet::StoreBytes(ret, IsPlayFlag);
		return ret;
	}

	Packet::byte_view FromBytes(Packet::byte_view view) {
		Packet::LoadBytes(view, CourseIndex);
		Packet::LoadBytes(view, Level);
		Packet::LoadBytes(view, AddScore);
		Packet::LoadBytes(view, Balloon);
		Packet::LoadBytes(view, IsPlayFlag);
		return view;
	}
};

struct ExamData {
	ExamTypes ExamType = ExamTypes::Null;
	ulonglong PassVal[2] = { 0,0 };
	ExamRange Range = ExamRange::Null;

	Packet::bytearray ToBytes() const {
		Packet::bytearray ret;
		Packet::StoreBytes(ret, ExamType);
		for (auto&& c : PassVal) {
			Packet::StoreBytes(ret, c);
		}
		Packet::StoreBytes(ret, Range);
		return ret;
	}

	Packet::byte_view FromBytes(Packet::byte_view view) {
		Packet::LoadBytes(view, ExamType);
		for (auto&& c : PassVal) {
			Packet::LoadBytes(view, c);
		}
		Packet::LoadBytes(view, Range);
		return view;
	}
};

struct ChartData {

	ChartData() {};
	ChartData(const std::string& path, _Skin* _skinptr, _Config* _configptr) { this->ReLoad(path, _skinptr, _configptr); }

	std::string FilePath = "";
	std::string WavePath = "";
	std::string SongLink = "";

	std::string Title = "";
	std::string SubTitle = "";
	int TitleStrlen = 0;
	int SubTitleStrlen = 0;
	int PlayingTitleStrlen = 0;
	int PlayingSubTitleStrlen = 0;
	int ResultTitleStrlen = 0;
	int ResultSubTitleStrlen = 0;
	bool TitleDisplay = true;
	bool SubTitleDisplay = true;
	double Offset = 0;
	double DemoStart = 0;
	double BPM = 0;
	int SongVolume = 100;
	int SEVolume = 100;
	ulonglong ChartID = 0;

	bool IsDanFlag = false;

	std::string DanTitle = "";
	std::string DanSubTitle = "";
	int DanTitleStrlen = 0;
	int DanSubTitleStrlen = 0;

	std::vector<ExamData> ExamDatas;
	std::vector<uint> DanIndexs;

	std::vector<std::string> FileData;
	std::string WaveData;
	int CourseIndex = 0;

	CourseData CourseDatas[(uint)ChartCourseType::Count];

	Packet::bytearray ToBytes() const {
		Packet::bytearray ret;
		Packet::StoreBytes(ret, FilePath);
		Packet::StoreBytes(ret, WavePath);
		Packet::StoreBytes(ret, SongLink);
		Packet::StoreBytes(ret, Title);
		Packet::StoreBytes(ret, SubTitle);
		Packet::StoreBytes(ret, TitleStrlen);
		Packet::StoreBytes(ret, SubTitleStrlen);
		Packet::StoreBytes(ret, PlayingTitleStrlen);
		Packet::StoreBytes(ret, PlayingSubTitleStrlen);
		Packet::StoreBytes(ret, ResultTitleStrlen);
		Packet::StoreBytes(ret, ResultSubTitleStrlen);
		Packet::StoreBytes(ret, TitleDisplay);
		Packet::StoreBytes(ret, SubTitleDisplay);
		Packet::StoreBytes(ret, Offset);
		Packet::StoreBytes(ret, DemoStart);
		Packet::StoreBytes(ret, BPM);
		Packet::StoreBytes(ret, SongVolume);
		Packet::StoreBytes(ret, SEVolume);
		Packet::StoreBytes(ret, ChartID);
		Packet::StoreBytes(ret, IsDanFlag);
		Packet::StoreBytes(ret, DanTitle);
		Packet::StoreBytes(ret, DanSubTitle);
		Packet::StoreBytes(ret, DanTitleStrlen);
		Packet::StoreBytes(ret, DanSubTitleStrlen);
		Packet::StoreBytes(ret, ExamDatas);
		Packet::StoreBytes(ret, DanIndexs);
		Packet::StoreBytes(ret, FileData);
		Packet::StoreBytes(ret, WaveData);
		Packet::StoreBytes(ret, CourseIndex);
		for (auto&& c : CourseDatas) {
			Packet::StoreBytes(ret, c);
		}
		return ret;
	}

	Packet::byte_view FromBytes(Packet::byte_view view) {
		Packet::LoadBytes(view, FilePath);
		Packet::LoadBytes(view, WavePath);
		Packet::LoadBytes(view, SongLink);
		Packet::LoadBytes(view, Title);
		Packet::LoadBytes(view, SubTitle);
		Packet::LoadBytes(view, TitleStrlen);
		Packet::LoadBytes(view, SubTitleStrlen);
		Packet::LoadBytes(view, PlayingTitleStrlen);
		Packet::LoadBytes(view, PlayingSubTitleStrlen);
		Packet::LoadBytes(view, ResultTitleStrlen);
		Packet::LoadBytes(view, ResultSubTitleStrlen);
		Packet::LoadBytes(view, TitleDisplay);
		Packet::LoadBytes(view, SubTitleDisplay);
		Packet::LoadBytes(view, Offset);
		Packet::LoadBytes(view, DemoStart);
		Packet::LoadBytes(view, BPM);
		Packet::LoadBytes(view, SongVolume);
		Packet::LoadBytes(view, SEVolume);
		Packet::LoadBytes(view, ChartID);
		Packet::LoadBytes(view, IsDanFlag);
		Packet::LoadBytes(view, DanTitle);
		Packet::LoadBytes(view, DanSubTitle);
		Packet::LoadBytes(view, DanTitleStrlen);
		Packet::LoadBytes(view, DanSubTitleStrlen);
		Packet::LoadBytes(view, ExamDatas);
		Packet::LoadBytes(view, DanIndexs);
		Packet::LoadBytes(view, FileData);
		Packet::LoadBytes(view, WaveData);
		Packet::LoadBytes(view, CourseIndex);
		for (auto&& c : CourseDatas) {
			Packet::LoadBytes(view, c);
		}
		return view;
	}

	UINT GetChartCodePage(const std::string& path) const {
		FileAccess FA(path, FAO::null);
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
					std::string wpath = std::filesystem::path(path).parent_path().string() + "\\" + cp_to_utf8(data, CodePage[j]);
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

		ExamData Exam;
		std::vector<uint> DanIndex;

		for (uint i = 0; i < FA.LineCount(); ++i) {
			Exsubstr(FA[i], "CHARTID:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				this->ChartID = stoull(data);
				});
			Exsubstr(FA[i], "TITLE:", [&](std::string data) {
				if (data.find("--") == 0) {
					data = data.replace(data.find("--"), 2, "");
					this->TitleDisplay = false;
				}
				this->Title = data;
				this->TitleStrlen = GetStrlen(data, _skinptr->Base->SongSelect.Font.Title.Handle);
				this->PlayingTitleStrlen = GetStrlen(data, _skinptr->Base->Playing.Font.Title.Handle);
				this->ResultTitleStrlen = GetStrlen(data, _skinptr->Base->Result.Font.Title.Handle);
				});
			Exsubstr(FA[i], "SUBTITLE:", [&](std::string data) {
				if (data.find("--") == 0) {
					data = data.replace(data.find("--"), 2, "");
					this->SubTitleDisplay = false;
				}
				this->SubTitle = data;
				this->SubTitleStrlen = GetStrlen(data, _skinptr->Base->SongSelect.Font.SubTitle.Handle);
				this->PlayingSubTitleStrlen = GetStrlen(data, _skinptr->Base->Playing.Font.SubTitle.Handle);
				this->ResultSubTitleStrlen = GetStrlen(data, _skinptr->Base->Result.Font.SubTitle.Handle);
				});
			Exsubstr(FA[i], "BPM:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				this->BPM = stod(data);
				});
			Exsubstr(FA[i], "WAVE:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				this->WavePath = std::filesystem::path(path).parent_path().string() + "\\" + data;
				});
			Exsubstr(FA[i], "SONGLINK:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				this->SongLink = data;
				});
			Exsubstr(FA[i], "OFFSET:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				this->Offset = stod(data);
				});
			Exsubstr(FA[i], "DEMOSTART:", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}
				this->DemoStart = std::max(0.0, (stod(data) * 1000));
				});
			Exsubstr(FA[i], "EXAM" + std::to_string(this->ExamDatas.size() + 1) + ":", [&](const std::string& data) {
				if (data.empty()) {
					return;
				}

				auto sp = split(data, ',');
				if (sp[0] == "g") {
					Exam.ExamType = ExamTypes::Accuracy;
				}
				else if (sp[0] == "jp") {
					Exam.ExamType = ExamTypes::Good;
				}
				else if (sp[0] == "jg") {
					Exam.ExamType = ExamTypes::Ok;
				}
				else if (sp[0] == "jb") {
					Exam.ExamType = ExamTypes::Bad;
				}
				else if (sp[0] == "s") {
					Exam.ExamType = ExamTypes::Score;
				}
				else if (sp[0] == "r") {
					Exam.ExamType = ExamTypes::Roll;
				}
				else if (sp[0] == "h") {
					Exam.ExamType = ExamTypes::HitNote;
				}
				else if (sp[0] == "c") {
					Exam.ExamType = ExamTypes::MaxCombo;
				}

				Exam.PassVal[0] = stoull(sp[1]) * (1.0 - (0.2 * (Exam.ExamType == ExamTypes::Accuracy)));
				Exam.PassVal[1] = stoull(sp[2]) * (1.0 - (0.2 * (Exam.ExamType == ExamTypes::Accuracy)));

				if (sp[3] == "m") {
					Exam.Range = ExamRange::More;
				}
				if (sp[3] == "l") {
					Exam.Range = ExamRange::Less;
				}

				this->ExamDatas.push_back(Exam);
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
				else if (_val == "dan") {
					course = ChartCourseType::Dan;
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
				if ((int)course == 6) {
					this->IsDanFlag = true;
				}
				this->CourseDatas[(int)course].CourseIndex = courseindex;
				this->CourseDatas[(int)course].Level = level;
				this->CourseDatas[(int)course].IsPlayFlag = true;
				this->CourseDatas[(int)course].AddScore = addscore < 0 ? 0 : addscore;
				this->CourseDatas[(int)course].Balloon = balloon;
				balloon = std::vector<int>();
				});
			if (this->IsDanFlag) {
				Exsubstr(FA[i], "#NEXTSONG", [&](const std::string& data) {
					DanIndex.push_back(i);
					});
				Exsubstr(FA[i], "#END", [&](const std::string& data) {
					this->DanIndexs = DanIndex;
					});
			}
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

		auto ColorCodeParse = [&](std::string data) -> Color3<int> {
			data.replace(0, 1, "0x");
			int color = stoi(data, nullptr, 16);
			int r = 0, g = 0, b = 0;
			GetColor2(color, &r, &g, &b);
			return { (r - 128) * 2, (g - 128) * 2, (b - 128) * 2 };
			};

		for (size_t i = 0; i < FA.LineCount(); ++i) {
			Exsubstr(FA[i], "GenreName=", [&](const std::string& data) {
				Name = data;
				});
			Exsubstr(FA[i], "GenreCaption=", [&](const std::string& data) {
				Caption = data;
				});
			Exsubstr(FA[i], "GenreColor=", [&](const std::string& data) {
				GenreColor = ColorCodeParse(data);
				});
			Exsubstr(FA[i], "FontColor=", [&](const std::string& data) {
				FontColor = ColorCodeParse(data);
				});
			Exsubstr(FA[i], "FontEdgeColor=", [&](const std::string& data) {
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

	char DragFilePath[MAX_PATH];

	std::vector<std::unique_ptr<BoxData>> __BoxDatas;
	std::vector<BoxData*> BoxDatas;
	std::vector<BoxData*> DanBoxDatas;

	int BoxDataIndex = 0;
	int BoxMotionDirection = 0;
	Timer<nanosecond> BoxMotion = Timer<nanosecond>(false);
	bool TimeModify = false;
	double UseBoxMotionTime = 0;
	inline double BoxMotionTime() const {
		return 100000000 / std::pow(1.125, std::abs(BoxMotionDirection) - 1);
	}

	Timer<millisecond> DemoSongPlayBlank = Timer<millisecond>(false);
	double DemoSongPlayBlankTime() const {
		return 1000;
	}

	Timer<millisecond> HighScoreDataChange = Timer<millisecond>(false);
	double HighScoreDataChangeTime = 3200;
	bool FadeDir[2] = { true, false };

	SoundData DemoSong;

	bool CourseSelect = false;
	int CourseIndex = 0;
	std::string CourseList[5]{ "Easy","Normal","Hard","Oni","Edit" };

	bool IsDanMode = false;

	void EnumChartFile(const std::vector<std::string>& dir) {
		__BoxDatas.clear();
		__BoxDatas.reserve(dir.capacity());
		int LoadCounter = 0;

		auto recusiveproc = [&](std::vector<std::unique_ptr<BoxData>>& data, const fs::path& dirpath, const fs::path& genrepath, auto& f, const GenreData& genredata = {}) -> void {
			fs::path _genrepath = "";
			auto fpit = fs::directory_iterator(dirpath);
			bool genreflag = false;
			for (const auto& fp : fpit) {
				if (fp == genrepath) {
					break;
				}
				if (fp.path().filename() == "genre.ini") {
					_genrepath = fp.path();
					BoxData* _data = new BoxData(new GenreData(fp.path().string()));
					_data->SetBoxColor();
					data.push_back(std::unique_ptr<BoxData>(_data));
					f(data.back()->GetGenre()->Datas, fp.path().parent_path(), _genrepath, f, *data.back()->GetGenre());
					genreflag = true;
					break;
				}
			}
			if (!genreflag) {
				fpit = fs::directory_iterator(dirpath);
				for (const auto& fp : fpit) {
					if (fp.path().extension() == ".tja") {
						BoxData* _data = new BoxData(new ChartData(fp.path().string(), __SkinPtr, __ConfigPtr));
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
		DanBoxDatasUpdate();

		BoxDataIndex = std::clamp<int>(BoxDataIndex, 0, BoxDatas.size() - 1);
	}
	void BoxDatasUpdate() {
		BoxDatas.clear();
		BoxDatas.reserve(__BoxDatas.capacity());
		auto recusiveproc = [&](const std::vector<std::unique_ptr<BoxData>>& datas, auto f) -> void {
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
				if (datas[i]->GetChart()->IsDanFlag) { continue; }
				BoxDatas.push_back(datas[i].get());
			}
			};
		recusiveproc(__BoxDatas, recusiveproc);
	};
	void DanBoxDatasUpdate() {
		DanBoxDatas.clear();
		DanBoxDatas.reserve(__BoxDatas.capacity());
		auto recusiveproc = [&](const std::vector<std::unique_ptr<BoxData>>& datas, auto f) -> void {
			for (uint i = 0; i < datas.size(); ++i) {
				if (datas[i]->IsGenre()) {
					DanBoxDatas.push_back(datas[i].get());
					if (datas[i]->GetGenre()->Open) {
						f(datas[i]->GetGenre()->Datas, f);
					}
				}
			}
			for (uint i = 0; i < datas.size(); ++i) {
				if (datas[i]->IsGenre()) { continue; }
				if (!datas[i]->GetChart()->IsDanFlag) { continue; }
				DanBoxDatas.push_back(datas[i].get());
			}
			};
		recusiveproc(__BoxDatas, recusiveproc);
	};

	void SongDownload(const std::string link, const fs::path path) {

		static auto IsInstalled = [](const std::string& packageName) {
				std::string command = "winget list \"" + packageName + "\" > nul 2>&1";
				int result = std::system(command.c_str());
				return (bool)(result == 0);
			};

		if (!IsInstalled("yt-dlp")) {
			std::system("winget install yt-dlp");
		}
		else {
			std::system("winget upgrade yt-dlp");
		}

		if (fs::exists("song.ogg")) {
			fs::remove("song.ogg");
		}

		{
			
			std::string command = "yt-dlp -x --audio-format mp3 -o \"song\" " + link;

			int result = std::system(command.c_str());

			if (result != 0) {
				std::string error = "音源のダウンロードに失敗しました";
				MessageBox(NULL, TEXT(error.c_str()), TEXT("エラー"), MB_ICONERROR);
				return;
			}
		}

		{
			std::string command = "ffmpeg -i \"song.mp3\" \"song.ogg\"";
			int result = std::system(command.c_str());

			if (fs::exists("song.mp3")) {
				fs::remove("song.mp3");
			}

			if (result != 0) {
				std::string error = "音源の変換に失敗しました";
				MessageBox(NULL, TEXT(error.c_str()), TEXT("エラー"), MB_ICONERROR);
				return;
			}
		}

		if (fs::exists("song.ogg")) {
			fs::rename("song.ogg", path);
		}
	}

	ChartData ChartDataGet(ChartData Chart) {

		ChartData& Dest = Chart;

		Dest.CourseIndex = CourseIndex;

		FileAccess FA(Chart.FilePath, FAO::rc_slash);
		for (int i = 0; i < FA.LineCount(); ++i) {
			Dest.FileData.push_back(FA[i]);
		}

		if (!Chart.SongLink.empty() && !fs::exists(Chart.WavePath)) {
			std::string info = "音源ファイルがありません。ダウンロードしますか？";
			switch (MessageBox(NULL, TEXT(info.c_str()), "", MB_OKCANCEL)) {
			case IDOK:
				SongDownload(Chart.SongLink, Chart.WavePath);
				break;
			}
		}

		std::ifstream file(Chart.WavePath, std::ios::binary);
		Dest.WaveData = std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
		file.close();

		return Dest;
	}

	void ImportFile() {

		static auto UnZip = [](fs::path path, fs::path dest) {

			std::ifstream is(path, std::ios::binary);
			IStreamFile f(is);

			UnZipper unzipper(f);

			for (auto& fileEntry : unzipper.listFiles()) {

				if (!fileEntry.isDir()) {

					fs::path filepath = fileEntry.fileName();
					fs::create_directories(dest / filepath.parent_path());
					std::ofstream file(dest / filepath, std::ios::binary);

					for (auto c : fileEntry.readContent()) {
						file.write(reinterpret_cast<const char*>(&c), sizeof(c));
					}
					file.close();
				}
			}
		};

		int fileCount = GetDragFileNum();

		if (fileCount > 0) {

			std::vector<fs::path> filePaths;
			char pathBuffer[MAX_PATH];

			for (int i = 0; i < fileCount; ++i) {
				if (GetDragFilePath(pathBuffer) == 0) {
					filePaths.push_back(pathBuffer);
				}
			}

			IFileOpenDialog* pfd = nullptr;

			HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
			if (SUCCEEDED(hr)) {
				DWORD dwFlags;
				pfd->GetOptions(&dwFlags);
				pfd->SetOptions(dwFlags | FOS_PICKFOLDERS);

				fs::path SongDir = __ConfigPtr->SongDirectories[0];
				IShellItem* psi = nullptr;
				hr = SHCreateItemFromParsingName(fs::absolute(SongDir).c_str(), NULL, IID_PPV_ARGS(&psi));

				if (SUCCEEDED(hr)) {
					hr = pfd->SetFolder(psi);
					psi->Release();
				}

				if (SUCCEEDED(pfd->Show(NULL))) {
					if (SUCCEEDED(pfd->GetResult(&psi))) {
						wchar_t* pszFilePath;
						if (SUCCEEDED(psi->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) {
							for (const fs::path& path : filePaths) {

								fs::path dest(pszFilePath);

								if (path.extension() == ".zip") { UnZip(path, dest); }
								else { fs::rename(path, dest.string() + (std::string)"\\" + path.filename().string()); }
							}
						}
						psi->Release();
					}
					else {

						pfd->Release();
						CoUninitialize();
						DragFileInfoClear();
						return;
					}
				}
				pfd->Release();
			}

			CoUninitialize();
			DragFileInfoClear();
		}
	}
};

