#include "Loading.h"
#include "GameSystem.h"

_Loading::_Loading(GameSystem *ptr) {

}

_Loading::~_Loading() {

}

void GameSystem::LoadingInit() {

}

void GameSystem::LoadingEnd() {

}

void GameSystem::LoadingDraw() {
	DrawString(0, 16, "NowLoading...", GetColor(255, 255, 255));
	DrawString(0, 32, "Tips:", GetColor(255, 255, 255));
}

void GameSystem::LoadingProc() {

	Playing.Chart.Init();

	ChartData& LoadData = *SongSelect.BoxDatas[SongSelect.BoxDataIndex]->GetChart();
	LoadData.ReLoad(LoadData.FilePath, &Skin, &Config);

	FileAccess FA(LoadData.FilePath, FAO::rc_slash);
	TJANoteData TJAMainData;

	Playing.Chart.OriginalData = LoadData;
	Playing.Chart.NowBPM = LoadData.BPM;

	TJAMainData.BPM = LoadData.BPM;
	
	// Set Offset
	double _offset = 0;
	if (240 / LoadData.BPM > -LoadData.Offset) {
		_offset = 240000 / LoadData.BPM;
		Playing.Chart.SongBlankTime = _offset - (LoadData.Offset * -1000) + Config.SongOffset;
		TJAMainData.RelaTime = _offset;
	}
	else {
		_offset = LoadData.Offset * -1000 - Config.SongOffset;
		Playing.Chart.SongBlankTime = 0;
		TJAMainData.RelaTime = _offset;
	}

	Playing.Chart.RawNoteDatas.push_back(TJAMainData);

	TJAMainData.AbsTime = _offset;

	// TrimCourse
	for (uint i = 0; i < (uint)ChartCourseType::Count; ++i) {
		if (LoadData.CourseDatas[i].IsPlayFlag && i != SongSelect.CourseIndex) {
			for (uint j = LoadData.CourseDatas[i].CourseIndex; j < FA.LineCount(); ++j) {
				if (FA[j].find("#END") != std::string::npos) {
					FA[j] = "";
					break;
				}
				FA[j] = "";
			}
		}
	}

	int BarlineCounter[2]{};
	int BarlineNoteCount = 0;

	bool StartFlag = false;
	bool BarlineDisplay = true;
	bool BarlineLoading = false;
	bool AddBarline = false;

	bool NowRollFlag = false;
	uint RollStartIndex = 0;
	char RollType = '\0';
	uint BalloonIndex = 0;

	uint NoteCount = 0;

	// MainLoading
	for (size_t i = 0, size = FA.LineCount(); i < size; ++i) {
		Exsubstr(FA[i], "#START", [&](const std::string& data) {
			StartFlag = true;
			});
		Exsubstr(FA[i], "#END", [&](const std::string& data) {
			StartFlag = false;
			});
		Exsubstr(FA[i], "#GOGOSTART", [&](const std::string& data) {
			TJAMainData.GoGoStart = true;
			});
		Exsubstr(FA[i], "#GOGOSTART", [&](const std::string& data) {
			TJAMainData.GoGoEnd = true;
			});
		Exsubstr(FA[i], "#BARLINEON", [&](const std::string& data) {
			BarlineDisplay = true;
			});
		Exsubstr(FA[i], "#BARLINEOFF", [&](const std::string& data) {
			BarlineDisplay = false;
			});
		if (!StartFlag) {
			Exsubstr(FA[i], "#BMSCROLL", [&](const std::string& data) {
				Playing.Chart.ScrollType = ScrollType::BMSCROLL;
				});
			Exsubstr(FA[i], "#HBSCROLL", [&](const std::string& data) {
				Playing.Chart.ScrollType = ScrollType::HBSCROLL;
				});
		}

		// getvalue
		Exsubstr(FA[i], "#SCROLL", [&](const std::string& data) {
			if (data.find("i") != std::string::npos) {
				int Uindex = data.rfind("+") == std::string::npos ? 0 : data.rfind("+");
				int Dindex = data.rfind("-") == std::string::npos ? 0 : data.rfind("-");

				bool Flag = Uindex > Dindex;

				if (data.rfind("+") != std::string::npos && Flag) {
					std::string real = strtrim(data.substr(0, data.rfind("+")));
					std::string imag = strtrim(data.substr(data.rfind("+") + 1, data.rfind("i") - (data.rfind("+") + 1)));
					TJAMainData.Scroll = real.empty() ? 0 : stod(real);
					TJAMainData.Scrolli = imag.empty() ? -1 : stod(imag) * -1;
				}
				if (data.rfind("-") != std::string::npos && !Flag) {
					std::string real = strtrim(data.substr(0, data.rfind("-")));
					std::string imag = strtrim(data.substr(data.rfind("-") + 1, data.rfind("i") - (data.rfind("-") + 1)));
					TJAMainData.Scroll = real == "" ? 0 : stod(real);
					TJAMainData.Scrolli = imag == "" ? 1 : stod(imag);
				}
			}
			else {
				TJAMainData.Scroll = stod(data);
				TJAMainData.Scrolli = 0;
			}
			});
		Exsubstr(FA[i], "#BPMCHANGE", [&](const std::string& data) {
			TJAMainData.BPM = stod(data);
			TJAMainData.BpmChangeFlag = true;
			});
		Exsubstr(FA[i], "#MEASURE", [&](const std::string& data) {
			auto sp = split(data, '/');
			TJAMainData.Measure = stod(sp[0]) / stod(sp[1]);
			});
		Exsubstr(FA[i], "#DELAY", [&](const std::string& data) {
			Playing.Chart.RawNoteDatas.back().RelaTime += stod(data) * 1000;
			TJAMainData.AbsTime += stod(data) * 1000;
			});

		if (FA[i].find("#") != std::string::npos) { continue; }

		if (!StartFlag) { continue; }

		if (!BarlineLoading) {
			BarlineLoading = true;
			for (size_t j = i; j < size; ++j) {
				if (FA[j].find("#") != std::string::npos) { continue; }
				for (size_t k = 0, strsize = FA[j].size(); k < strsize; ++k) {
					if (FA[j][k] == ',') {
						goto BARLINEREADEND;
					}
					else if (FA[j][k] >= '0' && FA[j][k] <= '9') {
						++BarlineNoteCount;
					}
				}
			}
		BARLINEREADEND:;
		}

		for (size_t j = 0, strsize = FA[i].size(); j < strsize; ++j) {
			bool ChartFlag = (FA[i][j] >= '0' && FA[i][j] <= '9');
			bool EndFlag = FA[i][j] == ',';
			bool EmptyFlag = BarlineNoteCount == 0;
			if (ChartFlag || EndFlag || EmptyFlag) {

				if (EndFlag && !EmptyFlag) {
					BarlineLoading = false;
					AddBarline = false;
					BarlineNoteCount = 0;
					break;
				}

				TJAMainData.NoteType = FA[i][j];

				double barlinetime = (240000 / TJAMainData.BPM) * TJAMainData.Measure;
				double divtime = barlinetime / (EmptyFlag ? 1 : BarlineNoteCount);

				TJAMainData.RelaTime = divtime;

				if (!AddBarline) {
					AddBarline = true;
					if (BarlineDisplay) {
						TJAMainData.BarlineDisplay = true;
					}
					else {
						TJAMainData.BarlineDisplay = false;
					}
				}
				else {
					TJAMainData.BarlineDisplay = false;
				}

#define Matched if (RollType == _ch) {\
break;\
TJAMainData.NoteType = '\0';\
}


#define SetOtherRollEnd Playing.Chart.RawNoteDatas[RollStartIndex].RollEndTime = TJAMainData.AbsTime;\
Playing.Chart.RawNoteDatas[RollStartIndex].RollEndIndex = Playing.Chart.RawNoteDatas.size();\
RollStartIndex = Playing.Chart.RawNoteDatas.size();\
RollType = TJAMainData.NoteType

#define Set8RollEnd Playing.Chart.RawNoteDatas[RollStartIndex].RollEndTime = TJAMainData.AbsTime;\
Playing.Chart.RawNoteDatas[RollStartIndex].RollEndIndex = Playing.Chart.RawNoteDatas.size();\
NowRollFlag = false;\
RollType = '\0'

				if (NowRollFlag) {
					const char _ch = TJAMainData.NoteType;
					switch (_ch) {
					case '5':
						Matched;
						SetOtherRollEnd;
						break;
					case '6':
						Matched;
						SetOtherRollEnd;
						break;
					case '7':
						Matched;
						SetOtherRollEnd;
						break;
					case '8':
						Set8RollEnd;
						break;
					case '9':
						Matched;
						SetOtherRollEnd;
						break;
					}
				}

#undef Matched
#undef SetOtherRollEnd
#undef Set8RollEnd

				if (!NowRollFlag && (TJAMainData.NoteType >= '5' && TJAMainData.NoteType <= '7') || TJAMainData.NoteType == '9') {
					RollStartIndex = Playing.Chart.RawNoteDatas.size();
					NowRollFlag = true;
					RollType = TJAMainData.NoteType;
				}

				bool removeflag =
					TJAMainData.NoteType == '0' &&
					TJAMainData.GoGoStart == false &&
					TJAMainData.GoGoEnd == false &&
					TJAMainData.BpmChangeFlag == false &&
					!TJAMainData.BarlineDisplay;

				if (TJAMainData.NoteType >= '1' && TJAMainData.NoteType <= '4') {
					++NoteCount;
				}
				if (TJAMainData.NoteType == '0') {
					TJAMainData.HitFlag = true;
				}
				if (GetRand(99) < Config.RandomRate) {
					switch (TJAMainData.NoteType)
					{
					case '1':
						TJAMainData.NoteType = '2';
						break;
					case '2':
						TJAMainData.NoteType = '1';
						break;
					case '3':
						TJAMainData.NoteType = '4';
						break;
					case '4':
						TJAMainData.NoteType = '3';
						break;
					}
				}

				if (!removeflag) {
					TJANoteData data(TJAMainData);
					data.Scroll *= Config.ChartSpeed;
					data.Scrolli *= Config.ChartSpeed;
					data.PosTime *= std::abs(std::complex(data.Scroll, data.Scrolli));
					Playing.Chart.RawNoteDatas.push_back(std::move(data));
				}
				else {
					Playing.Chart.RawNoteDatas.back().RelaTime += TJAMainData.RelaTime;
				}

				if (TJAMainData.NoteType == '7' || TJAMainData.NoteType == '9') {
					auto& balloon = LoadData.CourseDatas[SongSelect.CourseIndex].Balloon;
					int ballooncount = 0;
					if (BalloonIndex < balloon.size()) {
						ballooncount = balloon[BalloonIndex];
					}
					else {
						ballooncount = 5;
					}

					Playing.Chart.RawNoteDatas.back().BalloonCount = ballooncount;
					++BalloonIndex;
				}

				TJAMainData.HitFlag = false;
				TJAMainData.GoGoStart = false;
				TJAMainData.GoGoEnd = false;
				TJAMainData.BpmChangeFlag = false;

				TJAMainData.AbsTime += divtime;

				TJAMainData.PosTime += TJAMainData.RelaTime * (std::signbit(TJAMainData.BPM) || std::signbit(TJAMainData.Measure) ? -1 : 1);

				if (EndFlag) {
					BarlineLoading = false;
					AddBarline = false;
					BarlineNoteCount = 0;
					break;
				}
			}
		}
	}
	
	Playing.Chart.ProcNotes.ChunkSet(Playing.Chart.RawNoteDatas, Config.ChartChunkSize);
	Playing.Chart.DrawNotes.ChunkSet(Playing.Chart.RawNoteDatas, Config.ChartChunkSize);

	for (auto&& item : Playing.Chart.DrawNotes) {
		std::sort(item.begin(), item.end(), [&](const auto lhs, const auto rhs) {
			return lhs.get().PosTime < rhs.get().PosTime;
		});
	}

	SetCreateSoundDataType(DX_SOUNDDATATYPE_FILE);
	Playing.Chart.SongData.Load(LoadData.WavePath, 1);
	SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);

	Playing.Chart.SongData.SetVolume(LoadData.SongVolume * (Config.SongVolume / 100));
	int freq = Playing.Chart.SongData.Frequency * Config.SongSpeed;
	Playing.Chart.SongData.SetFrequency(freq);
	Playing.Chart.SongSpeed = (double)freq / Playing.Chart.SongData.Frequency;

	Playing.Chart.AddScore = LoadData.CourseDatas[SongSelect.CourseIndex].AddScore;
	if (Playing.Chart.AddScore == 0) {
		Playing.Chart.AddScore = 100'0000 / (double)NoteCount;
	}

	Skin.Base->Playing.SE.Don.SetVolume(LoadData.SEVolume * (Config.SEVolume / 100));
	Skin.Base->Playing.SE.Ka.SetVolume(LoadData.SEVolume * (Config.SEVolume / 100));
	Skin.Base->Playing.SE.Balloon.SetVolume(LoadData.SEVolume * (Config.SEVolume / 100));

	Playing.HitNote = _Playing::_HitNote();

	WaitVSync(2);
	Playing.Chart.NowTime.Start();
	Playing.Chart.FrameNowTime.Start();

	NowScene = Scene::Playing;
}
