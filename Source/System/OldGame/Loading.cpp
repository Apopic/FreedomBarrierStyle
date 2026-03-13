#include "Loading.h"
#include "GameSystem.h"

_Loading::_Loading(GameSystem* ptr) {

}

_Loading::~_Loading() {

}

void GameSystem::LoadingInit() {

}

void GameSystem::LoadingEnd() {

}

void GameSystem::LoadingDraw() {
	Skin.Base->Other.Font.Game.DrawFontString({ 0,8 }, "譜面読み込み中…");
}

void GameSystem::LoadingProc() {

	ChartData LoadData = ChartData();

	if (SongSelect.IsDanMode) {

		if (Playing.Chart.IsDanPlay) {
			if (Playing.Chart.IsFall || Playing.Chart.DanPlayCount >= Playing.Chart.OriginalData.DanIndexs.size() - 1) {
				NowScene = Scene::Result;
				return;
			}
			Playing.Chart.DanPlayCount++;
		}

		Playing.Chart.Init();

		LoadData = SongSelect.ChartDataGet(*SongSelect.DanBoxDatas[SongSelect.BoxDataIndex]->GetChart());
		LoadData.ReLoad(LoadData.FilePath, &Skin, &Config);

		if (!Playing.Chart.IsDanPlay) {
			Playing.Chart.IsDanPlay = true;
			Playing.Chart.ExamDatas.resize(LoadData.ExamDatas.size());
		}
	}
	else {

		LoadData = Playing.Chart.OriginalData;
		LoadData.ReLoad(LoadData.FilePath, &Skin, &Config);
		Playing.Chart.Init();

	}

	SongSelect.CourseIndex = LoadData.CourseIndex;
	std::vector<std::string> FA = LoadData.FileData;

	NoteData MainData;

	Playing.Chart.OriginalData = LoadData;
	Playing.Chart.NowBPM = LoadData.BPM;
	MainData.BPM = LoadData.BPM;

	double _offset = 0;
	if (240 / LoadData.BPM > -LoadData.Offset) {
		_offset = 240000 / LoadData.BPM;
		Playing.Chart.SongBlankTime = _offset - (LoadData.Offset * -1000) + Config.SongOffset;
		MainData.RelaTime = _offset;
	}
	else {
		_offset = LoadData.Offset * -1000 - Config.SongOffset;
		Playing.Chart.SongBlankTime = 0;
		MainData.RelaTime = _offset;
	}

	Playing.Chart.RawNoteDatas.push_back(MainData);

	MainData.AbsTime = _offset;

	for (uint i = 0; i < (uint)ChartCourseType::Count; ++i) {
		if (LoadData.CourseDatas[i].IsPlayFlag && i != SongSelect.CourseIndex) {
			for (uint j = LoadData.CourseDatas[i].CourseIndex; j < FA.size(); ++j) {
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
	bool NextFlag = false;
	bool BarlineDisplay = true;
	bool BarlineLoading = false;
	bool AddBarline = false;

	bool NowRollFlag = false;
	uint RollStartIndex = 0;
	char RollType = '\0';
	uint BalloonIndex = 0;

	uint NoteCount = 0;

	uint GoGoNoteCount = 0;
	size_t delayindex = 0;

	if (Playing.Chart.DanPlayCount > 0) {
		delayindex = LoadData.DanIndexs[Playing.Chart.DanPlayCount];
		StartFlag = true;
	}

	for (size_t i = delayindex, size = FA.size(); i < size; ++i) {

		try {
			Exsubstr(FA[i], "#START", [&](const std::string& data) {
				StartFlag = true;
				});
			Exsubstr(FA[i], "#END", [&](const std::string& data) {
				StartFlag = false;
				});
			Exsubstr(FA[i], "#GOGOSTART", [&](const std::string& data) {
				MainData.GoGoStart = true;
				});
			Exsubstr(FA[i], "#GOGOEND", [&](const std::string& data) {
				MainData.GoGoEnd = true;
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
			Exsubstr(FA[i], "#SCROLL", [&](const std::string& data) {
				if (data.find("i") != std::string::npos) {
					int Uindex = data.rfind("+") == std::string::npos ? 0 : data.rfind("+");
					int Dindex = data.rfind("-") == std::string::npos ? 0 : data.rfind("-");

					bool Flag = Uindex > Dindex;

					if (data.rfind("+") != std::string::npos && Flag) {
						std::string real = strtrim(data.substr(0, data.rfind("+")));
						std::string imag = strtrim(data.substr(data.rfind("+") + 1, data.rfind("i") - (data.rfind("+") + 1)));
						MainData.Scroll = real.empty() ? 0 : stod(real);
						MainData.Scrolli = imag.empty() ? -1 : stod(imag) * -1;
					}
					if (data.rfind("-") != std::string::npos && !Flag) {
						std::string real = strtrim(data.substr(0, data.rfind("-")));
						std::string imag = strtrim(data.substr(data.rfind("-") + 1, data.rfind("i") - (data.rfind("-") + 1)));
						MainData.Scroll = real == "" ? 0 : stod(real);
						MainData.Scrolli = imag == "" ? 1 : stod(imag);
					}
				}
				else {
					MainData.Scroll = stod(data);
					MainData.Scrolli = 0;
				}
				});
			Exsubstr(FA[i], "#BPMCHANGE", [&](const std::string& data) {
				MainData.BPM = stod(data);
				MainData.BpmChangeFlag = true;
				});
			Exsubstr(FA[i], "#MEASURE", [&](const std::string& data) {
				auto sp = split(data, '/');
				MainData.Measure = stod(sp[0]) / stod(sp[1]);
				});
			Exsubstr(FA[i], "#DELAY", [&](const std::string& data) {
				Playing.Chart.RawNoteDatas.back().RelaTime += stod(data) * 1000;
				MainData.AbsTime += stod(data) * 1000;
				});
			Exsubstr(FA[i], "#NEXTSONG", [&](const std::string& data) {
				if (!StartFlag || NextFlag) {
					StartFlag = false;
					NextFlag = false;
					return;
				}

				auto sp = split(data, ',');
				auto&& item = Playing.Chart.OriginalData;

				item.DanTitle = sp[0];
				item.DanTitleStrlen = GetStrlen(sp[0], Skin.Base->Playing.Font.Title.Handle);
			    item.DanSubTitle = sp[1];
				item.DanSubTitleStrlen = GetStrlen(sp[1], Skin.Base->Playing.Font.SubTitle.Handle);
				LoadData.CourseDatas[SongSelect.CourseIndex].AddScore = stoi(sp[4]);

				fs::path WavePath = fs::path(LoadData.FilePath).parent_path().string() + "\\" + sp[3];
				std::ifstream file(WavePath, std::ios::binary);
				LoadData.WaveData = std::string((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
				file.close();

				NextFlag = true;
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
		}
		catch (const std::invalid_argument) {
			std::string error = std::to_string(i + 1) + "行目の記述が不正です。";
			MessageBox(NULL, TEXT(error.c_str()), TEXT("エラー"), MB_ICONERROR);
			NowScene = Scene::SongSelect;
			return;
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

				MainData.NoteType = FA[i][j];

				double barlinetime = (240000 / MainData.BPM) * MainData.Measure;
				double divtime = barlinetime / (EmptyFlag ? 1 : BarlineNoteCount);

				MainData.RelaTime = divtime;

				if (!AddBarline) {
					AddBarline = true;
					if (BarlineDisplay) {
						MainData.BarlineDisplay = true;
					}
					else {
						MainData.BarlineDisplay = false;
					}
				}
				else {
					MainData.BarlineDisplay = false;
				}

#define Matched if (RollType == _ch) {\
break;\
MainData.NoteType = '\0';\
}


#define SetOtherRollEnd Playing.Chart.RawNoteDatas[RollStartIndex].RollEndTime = MainData.AbsTime;\
Playing.Chart.RawNoteDatas[RollStartIndex].RollEndIndex = Playing.Chart.RawNoteDatas.size();\
RollStartIndex = Playing.Chart.RawNoteDatas.size();\
RollType = MainData.NoteType

#define Set8RollEnd Playing.Chart.RawNoteDatas[RollStartIndex].RollEndTime = MainData.AbsTime;\
Playing.Chart.RawNoteDatas[RollStartIndex].RollEndIndex = Playing.Chart.RawNoteDatas.size();\
NowRollFlag = false;\
RollType = '\0'

				if (NowRollFlag) {
					const char _ch = MainData.NoteType;
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

				if (!NowRollFlag && (MainData.NoteType >= '5' && MainData.NoteType <= '7') || MainData.NoteType == '9') {
					RollStartIndex = Playing.Chart.RawNoteDatas.size();
					NowRollFlag = true;
					RollType = MainData.NoteType;
				}

				bool removeflag =
					MainData.NoteType == '0' &&
					!MainData.GoGoStart &&
					!MainData.GoGoEnd &&
					!MainData.BpmChangeFlag &&
					!MainData.BarlineDisplay;

				if (MainData.NoteType >= '1' && MainData.NoteType <= '4') {
					++NoteCount;
					++Playing.Chart.AllNoteCount;
				}
				if (MainData.NoteType == '0') {
					MainData.HitFlag = true;
				}
				if (GetRand(99) < Config.RandomRate) {
					switch (MainData.NoteType) {
					case '1':
						MainData.NoteType = '2';
						break;
					case '2':
						MainData.NoteType = '1';
						break;
					case '3':
						MainData.NoteType = '4';
						break;
					case '4':
						MainData.NoteType = '3';
						break;
					}
				}

				if (!removeflag) {
					NoteData data(MainData);
					data.Scroll *= Config.ChartSpeed;
					data.Scrolli *= Config.ChartSpeed;
					data.PosTime *= std::abs(std::complex(data.Scroll, data.Scrolli));
					Playing.Chart.RawNoteDatas.push_back(std::move(data));
				}
				else {
					Playing.Chart.RawNoteDatas.back().RelaTime += MainData.RelaTime;
				}

				if (MainData.NoteType == '7' || MainData.NoteType == '9') {
					auto& balloon = LoadData.CourseDatas[SongSelect.CourseIndex].Balloon;
					int ballooncount = 0;
					if (BalloonIndex + Playing.Chart.DanBalloonIndex < balloon.size()) {
						ballooncount = balloon[BalloonIndex + Playing.Chart.DanBalloonIndex];
					}
					else {
						ballooncount = 5;
					}

					Playing.Chart.RawNoteDatas.back().BalloonCount = ballooncount;
					++BalloonIndex;
				}

				MainData.HitFlag = false;
				MainData.GoGoStart = false;
				MainData.GoGoEnd = false;
				MainData.BpmChangeFlag = false;

				MainData.AbsTime += divtime;
				MainData.PosTime += MainData.RelaTime * (std::signbit(MainData.BPM) || std::signbit(MainData.Measure) ? -1 : 1);

				if (EndFlag) {
					BarlineLoading = false;
					AddBarline = false;
					BarlineNoteCount = 0;
					break;
				}
			}
		}
	}

	if (SongSelect.IsDanMode) {
		Playing.Chart.DanBalloonIndex += BalloonIndex;
	}

	Playing.Chart.AddScore = LoadData.CourseDatas[SongSelect.CourseIndex].AddScore;
	if (Playing.Chart.AddScore == 0) {
		Playing.Chart.AddScore = 100'0000 / (double)NoteCount;
	}

	double SongSpeed = Config.SongSpeed;
	if (MultiRoom.MultiFlag) {

		while (!ProcessMessage()) {
			if (Private.PlayerDatas[Private.MyIndex].Standby % MultiRoom.HostVal == 1) {

				int findval = MultiRoom.HostVal;
				auto it = std::find_if(Private.PlayerDatas.begin(), Private.PlayerDatas.end(), [findval](PlayerData n) { return n.Standby >= findval; });
				SongSpeed = Private.PlayerDatas[std::distance(Private.PlayerDatas.begin(), it)].Option.SongSpeed;
				for (auto&& data : Playing.Chart.RawNoteDatas | std::views::reverse) {
					if (data.NoteType >= '1' && data.NoteType <= '4') {
						Private.PlayerDatas[Private.MyIndex].NoteType.push_back(data.NoteType);
					}
				}

				Private.PlayerDatas[Private.MyIndex].Standby++;
				Send(DataType::List, Private.PlayerDatas[Private.MyIndex]);

			}
			if (CheckStandby(Private.PlayerDatas, 2)) {
				break;
			}
			Recv();
		}
	}

	SetCreateSoundDataType(DX_SOUNDDATATYPE_FILE);
	Playing.Chart.SongData.Load(LoadData.WaveData.data(), LoadData.WaveData.size(), 1);
	SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);

	Playing.Chart.SongData.SetVolume(Playing.Chart.OriginalData.SongVolume * (Config.SongVolume / 100));
	int freq = Playing.Chart.SongData.Frequency * SongSpeed;
	Playing.Chart.SongData.SetFrequency(freq);
	Playing.Chart.SongSpeed = (double)freq / Playing.Chart.SongData.Frequency;

	Skin.Base->Playing.SE.Don.SetVolume(Playing.Chart.OriginalData.SEVolume * (Config.SEVolume / 100));
	Skin.Base->Playing.SE.Ka.SetVolume(Playing.Chart.OriginalData.SEVolume * (Config.SEVolume / 100));
	Skin.Base->Playing.SE.Balloon.SetVolume(Playing.Chart.OriginalData.SEVolume * (Config.SEVolume / 100));

	for (auto&& note : Playing.HitNote) {
		note = _Playing::_HitNote();
	}

	Playing.Chart.Judge[0].ScoreRateGood = Loading.ScoreRateCalc(Config.JudgeGood, 25.0) * Loading.SongSpeedRateCalc(SongSpeed);
	Playing.Chart.Judge[0].ScoreRateOk = Loading.ScoreRateCalc(Config.JudgeOk, 75.0) * Loading.SongSpeedRateCalc(SongSpeed);

	Playing.Chart.FrameNowTime.ExtendRate = Config.FrameExtendRate;

	WaitVSync(2);
	if (!Config.TrainingMode || MultiRoom.MultiFlag || SongSelect.IsDanMode) {
		Playing.Chart.NowTime.Start();
		Playing.Chart.FrameNowTime.Start();
	}

	NowScene = Scene::Playing;

}