#pragma once
#include "Include.hpp"
#include "SongSelect.h"

enum class AlphaType {
	Hidden,
	Sudden
};

enum class ScrollType {
	Normal,
	BMSCROLL,
	HBSCROLL,
};

enum class HitType : int {
	Null = -2,
	Empty,
	DonLeft,
	KaLeft,
	DonRight,
	KaRight,
	Enter,
	Back
};

struct NoteData {

	double AbsTime = 0;
	double RelaTime = 0;
	double BMTime = 0;
	double PosTime = 0;
	bool BMFlag = false;
	bool BpmChangeFlag = false;
	bool BpmSpawnFlag = false;

	double BigNoteTime = 0;

	double BPM = 0;
	double Measure = 1;

	double Scroll = 1;
	double Scrolli = 0;

	bool GoGoStart = false;
	bool GoGoEnd = false;

	char NoteType = '\0';
	double RollEndTime = 0;
	uint RollEndIndex = 0;
	unsigned short RollFlag = 0;
	unsigned short BalloonFlag = 0;
	int BalloonCount = 0;
	bool BarlineDisplay = false;

	bool HitFlag = false;
};

enum class JudgeType : int {
	None = -1,
	Good,
	Ok,
	Bad,
	Roll
};

struct JudgeData {

	ulonglong Score = 0;
	ulonglong Good = 0;
	ulonglong Ok = 0;
	ulonglong Bad = 0;
	ulonglong Roll = 0;
	ulonglong Combo = 0;
	ulonglong MaxCombo = 0;
	ulonglong MemCombo = 0;
	ulonglong HitNote = 0;
	double ScoreRateGood = 0;
	double ScoreRateOk = 0;
	double Accuracy = 0;
	char NoteType = '\0';
	JudgeType HitJudge = JudgeType::None;

	void Hit(JudgeType type, int addscore, char note, bool autoflag) {

		HitJudge = type;
		NoteType = note;

		if (HitNote != 0) {
			Accuracy = ((Good / (double)HitNote) + ((Ok / (double)HitNote) * 0.5)) * 100;
		}

		switch (type) {
		case JudgeType::Good:
			++Good;
			++Combo;
			Score += addscore * ScoreRateGood;
			++HitNote;
			break;
		case JudgeType::Ok:
			++Ok;
			++Combo;
			Score += addscore / 2 * ScoreRateOk;
			++HitNote;
			break;
		case JudgeType::Bad:
			++Bad;
			Combo = 0;
			++HitNote;
			break;
		case JudgeType::Roll:
			++Roll;
			Score += 100;
			break;
		}

		if (Combo > MaxCombo) { ++MaxCombo; }

		Score *= !autoflag;
	}
};

struct HitNoteData {

	HitNoteData() {}
	HitNoteData(char type, JudgeType judgetype) {

		if (type == '6') {
			FlyingNote.Type = 8 + 48;
		}
		else {
			FlyingNote.Type = type;
		}

		JudgeUnderExplosion.Type = judgetype;
		JudgeUnderExplosion.Big = type == '3' || type == '4';
		JudgeString.Type = judgetype;

		switch (judgetype)
		{
		case JudgeType::Bad:
			JudgeString.IsActive = true;
			break;
		case JudgeType::Roll:
			FlyingNote.IsActive = true;
			break;
		default:
			JudgeUpperExplosion.IsActive = true;
			JudgeUnderExplosion.IsActive = true;
			FlyingNote.IsActive = true;
			JudgeString.IsActive = true;
			break;
		}

		MoveTimer.End();
	}

	Timer<nanosecond> MoveTimer;
	double MoveElapsedTime = 0;

	struct FlyingNote {
		bool IsActive = false;
		char Type = '0';
		double MoveTime() { return 500; }
	} FlyingNote;

	struct JudgeString {
		bool IsActive = false;
		JudgeType Type = JudgeType::None;
		double MoveTime() { return 500; }
	} JudgeString;

	struct JudgeUnderExplosion {
		bool IsActive = false;
		bool Big = false;
		JudgeType Type = JudgeType::None;
	} JudgeUnderExplosion;

	struct JudgeUpperExplosion {
		bool IsActive = false;
		bool Big = false;
		JudgeType Type = JudgeType::None;
	} JudgeUpperExplosion;
};

struct ExamStreamData {
	ulonglong ExamVals = 0;
	bool IsFall = false;
};

struct ChartStreamData {

	void Init() {
		RawNoteDatas.clear();
		HitErrorTime.clear();
		SongData.Delete();
		FrameNowTime.End();
		NowTime.End();
		ScrollType = ScrollType::Normal;
		OriginalData = ChartData();
		AutoPlayLR = false;
		NowGoGo = false;
		BalloonCount = 0;
		AllNoteCount = 0;
		BGMovieHandle = 0;
		CursorPos = 0;
		BGMovieSize = { 1280, 720 };
		if (!IsDanPlay) {
			for (auto&& judge : Judge) { judge = JudgeData(); }
			ExamDatas.clear();
			DanBalloonIndex = 0;
			DanPlayCount = 0;
			IsFall = false;
		}
	}

	std::vector<NoteData> RawNoteDatas = std::vector<NoteData>();

	SoundData SongData;

	ChartData OriginalData;
	int AddScore = 0;
	double SongBlankTime = 0;

	bool NowGoGo = false;
	double NowBPM = 0;
	Timer<nanosecond> NowTime;
	Timer<framecount> FrameNowTime;

	double SongSpeed = 1;

	ScrollType ScrollType = ScrollType::Normal;

	bool AutoPlayLR = false;
	Timer<microsecond> WaitRollTime;

	JudgeData Judge[4];

	int BalloonCount = 0;
	uint AllNoteCount = 0;

	int BGMovieHandle = -1;
	Size2D<float> BGMovieSize = { 1280,720 };

	uint DanPlayCount = 0;
	uint DanBalloonIndex = 0;

	bool IsDanPlay = false;
	bool IsFall = true;
	std::vector<ExamStreamData> ExamDatas = std::vector<ExamStreamData>();

	std::vector<double> HitErrorTime;
	float CursorPos = 0;

	Timer<millisecond> CursorMove;
	double CursorMoveTime = 3600;
};

extern class GameSystem* gameptr;

class _Playing {
public:

	_Playing(GameSystem* ptr);
	~_Playing();

	_Skin* __SkinPtr = nullptr;
	_Config* __ConfigPtr = nullptr;

	ChartStreamData Chart;
	JudgeData HighScore[5];

	Timer<millisecond> MiniTaikoFlash[16];
	double MiniTaikoFlashTime = 160;

	Timer<millisecond> MeasureJump;
	double MeasureJumpTime = 60;
	double MemNowTime = 0;

	std::string ExamList[8] = { "Accuracy", "Good", "Ok", "Bad", "Score", "Roll", "HitNote", "MaxCombo" };

	double TrainingOffset = 0;
	ulonglong NoteDataIndex = 0;
	ulonglong MeasureIndex = 0;
	ulonglong AllMeasureCount = 0;

	Timer<millisecond> KeyFlash[4][4];
	double KeyFlashTime = 160;

	double ChartNowTime(bool FrameCounter = false, double fastdrawrate = 0, double extendrate = 1) const {
		double ret = 0;
		if (FrameCounter) {
			double hz = Chart.FrameNowTime.GetRefreshRate() * extendrate;
			double frame = ((long long)(Chart.FrameNowTime.GetRecordingTime(second) * hz) + fastdrawrate) / hz;
			ret = millisecond * frame;
		}
		else {
			ret = Chart.NowTime.GetRecordingTime() / microsecond;
		}
		return ret * Chart.SongSpeed;
	};

	struct _HitNote {
	private:

		static const size_t m_size = 64;

	public:

		_HitNote() {};
		~_HitNote() {};

		HitNoteData Datas[m_size]{};
		uint Index = 0;
		uint Size() { return m_size; }

		void Add(HitNoteData&& data) {

			Datas[Index] = std::move(data);

			++Index;

			if (!(Index < m_size)) {
				Index = 0;
			}
		}
	} HitNote[4];

	void MovieDraw(double nowtime) {

		if (Chart.BGMovieHandle != -1 && __ConfigPtr->BGBrightness > 0) {

			DrawFillBox(0, 0, __SkinPtr->Info.Resolution.X, __SkinPtr->Info.Resolution.Y, GetColor(0, 0, 0));
			DrawExtendGraphF(
				__SkinPtr->Info.Resolution.X / 2 - Chart.BGMovieSize.Width / 2,
				__SkinPtr->Info.Resolution.Y / 2 - Chart.BGMovieSize.Height / 2,
				__SkinPtr->Info.Resolution.X / 2 + Chart.BGMovieSize.Width / 2,
				__SkinPtr->Info.Resolution.Y / 2 + Chart.BGMovieSize.Height / 2,
				Chart.BGMovieHandle,
				FALSE);

			if ((nowtime + (Chart.OriginalData.BGMovieOffset * -1000)) > 128 && Chart.NowTime.GetNowRecording()) {
				PlayMovieToGraph(Chart.BGMovieHandle);
			}

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 * (1 - (__ConfigPtr->BGBrightness / 100)));
			DrawFillBox(0, 0, __SkinPtr->Info.Resolution.X, __SkinPtr->Info.Resolution.Y, GetColor(0, 0, 0));
			SetDrawBlendMode(0, 0);
		}
	}

	template<typename T>
	void NoteDraw(T&& MultiData, double NowTime, const Pos2D<float>& add, bool MultiFlag, int pldx) {

		static auto NoteAlpha = [&](int& Alpha, double _one, AlphaType Type) {

			_one = std::clamp(_one, 0.0, 1.0);

			switch (Type) {
			case AlphaType::Hidden:
				Alpha = 255 * _one;
				break;
			case AlphaType::Sudden:
				Alpha = Alpha * (1 - _one);
				break;
			}
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
			};

		const Pos2D<double>& NoteOrigin = {
	__SkinPtr->Base->Playing.Image.Note.Pos.X,
	__SkinPtr->Base->Playing.Image.Note.Pos.Y + add.Y
		};

		auto GetNotePos = [&](NoteData& data)->Pos2D<double> {
			bool BMScroll = ScrollType::BMSCROLL == Chart.ScrollType;
			Pos2D<double> _ret;
			if (ScrollType::Normal == Chart.ScrollType) {
				double _temp = ((data.AbsTime - NowTime) / (240 / data.BPM));
				_ret = { _temp,_temp };
			}
			else {
				double _bpm = data.BMFlag || data.BpmSpawnFlag ? data.BPM : Chart.NowBPM;
				double optime = (((data.BMFlag || data.BpmSpawnFlag ? data.AbsTime : data.BMTime) - NowTime) / (240 / _bpm));
				_ret = { optime,optime };
			}

			_ret = {
				_ret.X *= __SkinPtr->Base->Playing.Config.LaneExtendRate * (BMScroll ? 1 : data.Scroll),
				_ret.Y *= __SkinPtr->Base->Playing.Config.LaneExtendRate * (BMScroll ? 0 : data.Scrolli)
			};

			Pos2D<float> SkinPos = {
				__SkinPtr->Base->Playing.Image.Lane.Size.Width,
				__SkinPtr->Base->Playing.Image.Lane.Size.Height
			};

			_ret = {
				_ret.X + NoteOrigin.X,
				_ret.Y + NoteOrigin.Y
			};

			return _ret;

			};

		auto&& ProcNotes = Chart.RawNoteDatas;
		double _addms = ProcNotes[0].AbsTime;
		for (int i = 0, size = ProcNotes.size(); i < size; ++i) {
			NoteData& data = ProcNotes[i];

			if (data.AbsTime < NowTime) {
				data.BMFlag = true;
				Chart.NowBPM = data.BPM;
			}

			if (data.BpmChangeFlag) {
				if (data.BPM * data.Measure > 0) {
					for (int j = i + 1; j < size; ++j) {
						auto& jdata = ProcNotes[j];
						if (jdata.BpmChangeFlag && jdata.BPM * jdata.Measure < 0) {
							data.BpmChangeFlag = false;
							for (int k = j; k < size; ++k) {
								auto& kdata = ProcNotes[k];
								if (kdata.AbsTime < jdata.AbsTime) {
									kdata.BpmSpawnFlag = true;
								}
								else {
									kdata.BpmSpawnFlag = false;
								}
							}
							break;
						}
						if (jdata.BpmChangeFlag && jdata.BPM * jdata.Measure > 0) {
							data.BpmChangeFlag = false;
							for (int k = j; k < size; ++k) {
								ProcNotes[k].BpmSpawnFlag = false;
							}
							break;
						}
					}
				}
				else {
					data.BpmChangeFlag = false;
				}
			}

			if (data.BMFlag || i == 0) { _addms = data.AbsTime; data.BMTime = data.AbsTime; continue; }

			double _bpm = (Chart.NowBPM / ProcNotes[i - 1].BPM);
			_addms += ProcNotes[i - 1].RelaTime / _bpm;
			data.BMTime = _addms;
		}

#define InRange(x, y) (x > __SkinPtr->SimulationRect.Left && x < __SkinPtr->SimulationRect.Right && y > __SkinPtr->SimulationRect.Top && y < __SkinPtr->SimulationRect.Bottom)

		std::complex<double> n1{};
		std::complex<double> n2{};
		std::complex<double> n3{};
		std::complex<double> n4{};
		std::complex<double> facing{};
		Pos2D<double> NotePos{};

		const std::complex<double> n0 = { __SkinPtr->Base->Playing.Image.Note.Size.Width / 2, __SkinPtr->Base->Playing.Image.Note.Size.Height / 2 };
		const double n0r = std::abs(n0);
		const double narr[4] = {
			std::arg(std::complex<double>{ n0.real() * -1, n0.imag() * -1 }),
			std::arg(std::complex<double>{ n0.real() * 1, n0.imag() * -1 }),
			std::arg(std::complex<double>{ n0.real() * 1, n0.imag() * 1 }),
			std::arg(std::complex<double>{ n0.real() * -1, n0.imag() * 1 })
		};

		for (int i = 0; auto&& data : Chart.RawNoteDatas | std::ranges::views::reverse) {

			double NoteTheta = atan2(data.Scrolli, data.Scroll);

			NotePos = GetNotePos(data);

			if (data.BarlineDisplay) {

				SetDrawBlendMode(0, 0);
				if (InRange(NotePos.X, NotePos.Y)) {
					DrawLineAA(
						NotePos.X,
						NotePos.Y - 65,
						NotePos.X,
						NotePos.Y + 65,
						GetColor(255, 255, 255)
					);
				}
			}

			if (data.NoteType == '0') {
				continue;
			}

			if (data.NoteType >= '1' &&
				data.NoteType <= '4') {
				if (InRange(NotePos.X, NotePos.Y)) {
					int Alpha = 255;
					double hidden = __ConfigPtr->HiddenLevel > 0 ? __ConfigPtr->HiddenLevel : MultiFlag ? MultiData.Option.Hidden : 0;
					double sudden = __ConfigPtr->SuddenLevel > 0 ? __ConfigPtr->SuddenLevel : MultiFlag ? MultiData.Option.Sudden : 0;
					bool hiddenflag = hidden > 0;
					bool suddenflag = sudden > 0;
					bool multiflag = pldx;
					if (hiddenflag || suddenflag || multiflag) {
						double _abs = std::abs(std::complex<double>{ NotePos.X - NoteOrigin.X, NotePos.Y - NoteOrigin.Y })* (data.AbsTime < NowTime ? -1 : 1);
						double leveling = (__SkinPtr->Base->Playing.Image.Lane.Size.Width / DX_PI);
						double feedrange = (__SkinPtr->Base->Playing.Image.Lane.Size.Width / DX_TWO_PI);
						if (multiflag) {
							NoteAlpha(Alpha, (_abs - (leveling * 0.025)) / feedrange, AlphaType::Hidden);
						}
						if (hiddenflag) {
							NoteAlpha(Alpha, (_abs - (leveling * hidden)) / feedrange, AlphaType::Hidden);
						}
						if (suddenflag) {
							NoteAlpha(Alpha, ((_abs - (__SkinPtr->Base->Playing.Image.Lane.Size.Width)) + (leveling * sudden)) / feedrange, AlphaType::Sudden);
						}
					}

					auto&& note = !pldx ? data.NoteType : MultiData.NoteType[i];

					__SkinPtr->Base->Playing.Image.Note.Draw(
						{
							(float)(NotePos.X - NoteOrigin.X),
							(float)(NotePos.Y - NoteOrigin.Y) + add.Y
						},
						note - 48
					);
				}
				i++;
				continue;
			}
			SetDrawBlendMode(0, 0);

			if (data.NoteType >= '5' &&
				data.NoteType <= '6') {
				const Pos2D<double>& cnote = NotePos;
				const Pos2D<double>& dnote = GetNotePos(Chart.RawNoteDatas[data.RollEndIndex]);

				bool DispFlag =
					InRange(cnote.X, cnote.Y) ||
					(data.AbsTime < NowTime && data.RollEndTime > NowTime) ||
					InRange(dnote.X, dnote.Y);

				if (DispFlag) {

					bool BigRollFlag = data.NoteType == '6';

					const std::complex<double>& cdnote = { dnote.X - cnote.X, dnote.Y - cnote.Y };

					double RollTheta = std::arg(cdnote);
					facing = std::polar(n0.real() - 3, RollTheta);

					n1 = std::polar(n0r, narr[0] + RollTheta);
					n2 = std::polar(n0r, narr[1] + RollTheta);
					n3 = std::polar(n0r, narr[2] + RollTheta);
					n4 = std::polar(n0r, narr[3] + RollTheta);

					DrawModiGraphF(
						cnote.X + facing.real() + n1.real(),
						cnote.Y + facing.imag() + n1.imag(),
						dnote.X - facing.real() + n2.real(),
						dnote.Y - facing.imag() + n2.imag(),
						dnote.X - facing.real() + n3.real(),
						dnote.Y - facing.imag() + n3.imag(),
						cnote.X + facing.real() + n4.real(),
						cnote.Y + facing.imag() + n4.imag(),
						__SkinPtr->Base->Playing.Image.Note.Handles[BigRollFlag ? 9 : 6],
						TRUE
					);
					DrawModiGraphF(
						dnote.X + n1.real(),
						dnote.Y + n1.imag(),
						dnote.X + n2.real(),
						dnote.Y + n2.imag(),
						dnote.X + n3.real(),
						dnote.Y + n3.imag(),
						dnote.X + n4.real(),
						dnote.Y + n4.imag(),
						__SkinPtr->Base->Playing.Image.Note.Handles[BigRollFlag ? 10 : 7],
						TRUE
					);
					DrawModiGraphF(
						cnote.X + n1.real(),
						cnote.Y + n1.imag(),
						cnote.X + n2.real(),
						cnote.Y + n2.imag(),
						cnote.X + n3.real(),
						cnote.Y + n3.imag(),
						cnote.X + n4.real(),
						cnote.Y + n4.imag(),
						__SkinPtr->Base->Playing.Image.Note.Handles[BigRollFlag ? 8 : 5],
						TRUE
					);

				}
			}

			if (data.NoteType == '7' ||
				data.NoteType == '9') {

				if (data.BalloonFlag == 1) {
					NotePos = NoteOrigin;
				}
				if (data.BalloonFlag == 2) {
					NotePos = GetNotePos(Chart.RawNoteDatas[data.RollEndIndex]);
				}

				if (InRange(NotePos.X, NotePos.Y)) {

					bool KusudamaFlag = data.NoteType == '9';

					facing = std::polar(n0.real() * 2, NoteTheta);

					n1 = std::polar(n0r, narr[0] + NoteTheta);
					n2 = std::polar(n0r, narr[1] + NoteTheta);
					n3 = std::polar(n0r, narr[2] + NoteTheta);
					n4 = std::polar(n0r, narr[3] + NoteTheta);

					DrawModiGraphF(
						NotePos.X + n1.real(),
						NotePos.Y + n1.imag(),
						NotePos.X + n2.real(),
						NotePos.Y + n2.imag(),
						NotePos.X + n3.real(),
						NotePos.Y + n3.imag(),
						NotePos.X + n4.real(),
						NotePos.Y + n4.imag(),
						__SkinPtr->Base->Playing.Image.Note.Handles[KusudamaFlag ? 13 : 11],
						TRUE
					);
					DrawModiGraphF(
						NotePos.X + facing.real() + n1.real(),
						NotePos.Y + facing.imag() + n1.imag(),
						NotePos.X + facing.real() + n2.real(),
						NotePos.Y + facing.imag() + n2.imag(),
						NotePos.X + facing.real() + n3.real(),
						NotePos.Y + facing.imag() + n3.imag(),
						NotePos.X + facing.real() + n4.real(),
						NotePos.Y + facing.imag() + n4.imag(),
						__SkinPtr->Base->Playing.Image.Note.Handles[KusudamaFlag ? 14 : 12],
						TRUE
					);
				}
			}
		}

#undef InRange
	}

	void NoteProc(double nowtime) {

		Chart.BalloonCount = 0;

		for (auto&& data : Chart.RawNoteDatas) {

			bool HitFlag = data.AbsTime < nowtime;

			if (data.GoGoStart && HitFlag) {
				Chart.NowGoGo = true;
			}
			if (data.GoGoEnd && HitFlag) {
				Chart.NowGoGo = false;
			}

			if (data.AbsTime - __ConfigPtr->JudgeBad > nowtime) { continue; }
			if (data.HitFlag) { continue; }

			const double _HitError = data.AbsTime - nowtime;
			const bool BadHit = _HitError > -__ConfigPtr->JudgeBad && _HitError < __ConfigPtr->JudgeBad;

			if (data.BigNoteTime != 0 && __ConfigPtr->JudgeGood < nowtime - data.BigNoteTime) {
				JudgeNote(nowtime, data.NoteType - 2);
			}

			if (!data.HitFlag &&
				(data.NoteType >= '1' && data.NoteType <= '4') &&
				data.BigNoteTime == 0 &&
				_HitError < -__ConfigPtr->JudgeBad) {
				Chart.Judge[0].Hit(JudgeType::Bad, 0, '\0', __ConfigPtr->AutoPlayFlag);
				Action(HitType::Empty);
				data.HitFlag = true;
			}

			if ((data.NoteType >= '5' && data.NoteType <= '6') &&
				HitFlag) {

				data.RollFlag = 1;

				if (data.RollEndTime < nowtime) {
					data.RollFlag = 2;
					data.HitFlag = true;
				}
			}

			if ((data.NoteType == '7' || data.NoteType == '9') &&
				HitFlag) {

				data.BalloonFlag = 1;
				Chart.BalloonCount = data.BalloonCount;

				if (data.RollEndTime < nowtime) {
					data.BalloonFlag = 2;
					data.HitFlag = true;
				}
			}
		}
	}

	void AutoPlayProc(double NowTime) {

		int RollCount = 0;
		NoteData* BalloonData = nullptr;

		bool NextImage = false;

		for (auto&& data : Chart.RawNoteDatas) {

			bool HitFlag = data.AbsTime < NowTime;
			bool IsHitNote = (data.NoteType >= '1' && data.NoteType <= '4');

			if (data.RollFlag == 1) {
				++RollCount;
				NextImage = data.NoteType == '6';
			}

			if (data.BalloonFlag == 1) {
				BalloonData = &data;
			}

			if (HitFlag && !data.HitFlag && IsHitNote) {
				HitNote[0].Add(HitNoteData(data.NoteType, JudgeType::Good));
				Chart.Judge[0].Hit(JudgeType::Good, 0, data.NoteType, __ConfigPtr->AutoPlayFlag);
				switch (data.NoteType) {
				case '1':
					__SkinPtr->Base->Playing.SE.Don.Play();
					MiniTaikoFlash[0 + Chart.AutoPlayLR * 2].Start();
					Chart.AutoPlayLR = !Chart.AutoPlayLR;
					Action((HitType)(0 + Chart.AutoPlayLR * 2));
					break;
				case '2':
					__SkinPtr->Base->Playing.SE.Ka.Play();
					MiniTaikoFlash[1 + Chart.AutoPlayLR * 2].Start();
					Chart.AutoPlayLR = !Chart.AutoPlayLR;
					Action((HitType)(1 + Chart.AutoPlayLR * 2));
					break;
				case '3':
					__SkinPtr->Base->Playing.SE.Don.Play();
					__SkinPtr->Base->Playing.SE.Don.Play();
					MiniTaikoFlash[(int)HitType::DonLeft].Start();
					MiniTaikoFlash[(int)HitType::DonRight].Start();
					Action(HitType::DonLeft);
					Action(HitType::DonRight);
					break;
				case '4':
					__SkinPtr->Base->Playing.SE.Ka.Play();
					__SkinPtr->Base->Playing.SE.Ka.Play();
					MiniTaikoFlash[(int)HitType::KaLeft].Start();
					MiniTaikoFlash[(int)HitType::KaRight].Start();
					Action(HitType::KaLeft);
					Action(HitType::KaRight);
					break;
				}
				data.NoteType = '\0';
				data.HitFlag = true;
			}
		}

		const double WaitRollTimer = Chart.WaitRollTime.GetRecordingTime() / TimerType::microsecond;
		const double WaitRollTime = 1 / __ConfigPtr->RollSpeed;
		if (RollCount > 0 && WaitRollTimer > WaitRollTime) {
			__SkinPtr->Base->Playing.SE.Don.Play();
			Chart.AutoPlayLR = !Chart.AutoPlayLR;
			Chart.Judge[0].Roll++;
			HitNote[0].Add(HitNoteData(NextImage ? '6' : '5', JudgeType::Roll));
			Chart.WaitRollTime.Start();
		}
		if (BalloonData != nullptr && WaitRollTimer > WaitRollTime) {
			__SkinPtr->Base->Playing.SE.Don.Play();
			Chart.AutoPlayLR = !Chart.AutoPlayLR;
			Chart.Judge[0].Roll++;
			--BalloonData->BalloonCount;
			Chart.WaitRollTime.Start();
			if (BalloonData->BalloonCount <= 0) {
				__SkinPtr->Base->Playing.SE.Balloon.Play();
				HitNote[0].Add(HitNoteData('3', JudgeType::Roll));
				BalloonData->NoteType = '0';
				BalloonData->HitFlag = true;
				BalloonData->BalloonFlag = 2;
				Chart.Judge[0].NoteType = '3';
			}
		}
	}

	void PlayProc(double NowTime) {

		static auto KeyDownProc = [&](int array, HitType type, SoundData SE, std::vector<int> keys) {
			for (int i = 0; i < keys.size(); i++) {
				Input.HitKeyProcess(keys[i], KeyState::Down, [&] {
					SE.Play();
					MiniTaikoFlash[(int)type].Start();
					KeyFlash[array][i].Start();
					JudgeNote(NowTime, (char)(((int)type % 2) + '1'));
					Action(type);
					});
			}
			};

		KeyDownProc(0, HitType::KaLeft, __SkinPtr->Base->Playing.SE.Ka, __ConfigPtr->KaInputLeft);
		KeyDownProc(1, HitType::DonLeft, __SkinPtr->Base->Playing.SE.Don, __ConfigPtr->DonInputLeft);
		KeyDownProc(2, HitType::DonRight, __SkinPtr->Base->Playing.SE.Don, __ConfigPtr->DonInputRight);
		KeyDownProc(3, HitType::KaRight, __SkinPtr->Base->Playing.SE.Ka, __ConfigPtr->KaInputRight);
	}

	void TraningModeProc(double nowtime) {

		if (!MeasureJump.GetNowRecording()) {

			static auto MoveInputProc = [&](bool direction) {

				bool is_find = false;
				auto v = Chart.RawNoteDatas;
				auto barline_find = [](NoteData data) { return data.BarlineDisplay; };

				if (direction) {
					if (MeasureIndex < AllMeasureCount) {
						auto begin = std::next(v.begin() + NoteDataIndex);
						auto it = std::find_if(begin, v.end(), barline_find);
						if (it != v.end()) {
							NoteDataIndex = std::distance(v.begin(), it);
							MeasureIndex++;
							is_find = true;
						}
					}
				}
				else {
					if (MeasureIndex > 0) {
						auto rbegin = v.begin() + NoteDataIndex;
						auto rit = std::find_if(std::reverse_iterator(rbegin), v.rend(), barline_find);
						if (rit != v.rend()) {
							NoteDataIndex = std::distance(v.begin(), std::prev(rit.base()));
							MeasureIndex--;
							is_find = true;
						}
					}
				}

				if (is_find) {
					MemNowTime = nowtime;
					MeasureJump.Start();
				}
			};

			static auto StartInputProc = [&] {
				Chart.NowTime.Start();
				Chart.FrameNowTime.Start();
				if (Chart.SongBlankTime < nowtime) {
					Chart.SongData.SetCurrent(nowtime - Chart.SongBlankTime);
					Chart.SongData.Play(FALSE);
				}
				};

			Input.HitKeyesProcess(__ConfigPtr->KaInputLeft, KeyState::Down, [&] { MoveInputProc(false); });
			Input.HitKeyesProcess(__ConfigPtr->KaInputRight, KeyState::Down, [&] { MoveInputProc(true); });
			Input.HitKeyProcess(VK_NEXT, KeyState::Down, [&] { MoveInputProc(false); });
			Input.HitKeyProcess(VK_PRIOR, KeyState::Down, [&] { MoveInputProc(true); });

			Input.HitKeyesProcess(__ConfigPtr->DonInputLeft, KeyState::Down, StartInputProc);
			Input.HitKeyesProcess(__ConfigPtr->DonInputRight, KeyState::Down, StartInputProc);
			Input.HitKeyProcess(VK_RETURN, KeyState::Down, StartInputProc);
		}
		else {

			TrainingOffset = std::lerp(MemNowTime, Chart.RawNoteDatas[NoteDataIndex].AbsTime, GetEasingRate(MeasureJump.GetRecordingTime() / MeasureJumpTime, ease::Base::In, ease::Line::Linear));

			if (MeasureJump.GetRecordingTime() >= MeasureJumpTime) {
				if (Chart.BGMovieHandle != -1) {
					SeekMovieToGraph(Chart.BGMovieHandle, (bool)(Chart.OriginalData.BGMovieOffset < 0) ? TrainingOffset + Chart.OriginalData.BGMovieOffset * -1000 : TrainingOffset - Chart.SongBlankTime);
				}
				MeasureJump.End();
			}
		}
	}

	void DanProc() {

		for (uint i = 0; i < Chart.OriginalData.ExamDatas.size(); i++) {

			auto ExamData = Chart.OriginalData.ExamDatas[i];

#define EXAMVAL(type) case ExamTypes::type:\
Chart.ExamDatas[i].ExamVals = std::abs(((int)ExamData.PassVal[0] * (int)ExamData.Range) - (int)Chart.Judge[0].type);\
break;\

			switch (ExamData.ExamType) {
				EXAMVAL(Accuracy);
				EXAMVAL(Good);
				EXAMVAL(Ok);
				EXAMVAL(Bad);
				EXAMVAL(Score);
				EXAMVAL(Roll);
				EXAMVAL(HitNote);
				EXAMVAL(MaxCombo);
			}

			if (ExamData.Range == ExamRange::Less && Chart.ExamDatas[i].ExamVals <= 0) {
				if (!Chart.ExamDatas[i].IsFall) {
					__SkinPtr->Base->Playing.SE.DanFall.Play();
					Chart.ExamDatas[i].IsFall = true;
					Chart.IsFall = true;
				}
			}
		}
	}

	template<typename T, typename Y>
	void MultiProc(T& Public, Y& Private) {
		if (Public.HitKey != HitType::Null) {
			if (Public.GetIndex <= Private.MyIndex) {
				Public.GetIndex++;
			}
			if (Public.HitKey >= HitType::DonLeft && Public.HitKey <= HitType::KaRight) {

				MiniTaikoFlash[(int)Public.HitKey + 4 * Public.GetIndex].Start();

				if (Public.Judge.HitJudge != JudgeType::None) {
					HitNote[Public.GetIndex].Add(HitNoteData(Public.Judge.NoteType, Public.Judge.HitJudge));
				}
			}
			Chart.Judge[Public.GetIndex] = Public.Judge;
		}
	}

	void JudgeNote(double nowtime, char type) {

		auto& Judge = Chart.Judge[0];

		int rollcount = 0;
		int ballooncount = 0;
		NoteData* balloondata = nullptr;

		bool NextImage = false;

		for (auto&& data : Chart.RawNoteDatas) {

			if (data.HitFlag) {
				continue;
			}

			if (data.RollFlag == 1) {
				++rollcount;
				NextImage = data.NoteType == '6';
			}

			if (data.BalloonFlag == 1) {
				balloondata = &data;
			}

			if (data.BigNoteTime != 0) {
				if (__ConfigPtr->JudgeGood < nowtime - data.BigNoteTime) {
					data.NoteType -= 2;
				}
				nowtime = data.BigNoteTime;
			}

			const double _HitError = data.AbsTime - nowtime;
			const bool GoodHit =
				_HitError > -__ConfigPtr->JudgeGood && _HitError < __ConfigPtr->JudgeGood;
			const bool OkHit =
				_HitError > -__ConfigPtr->JudgeOk && _HitError < __ConfigPtr->JudgeOk;
			const bool BadHit =
				_HitError > -__ConfigPtr->JudgeBad && _HitError < __ConfigPtr->JudgeBad;
			bool TypeMatch = type == data.NoteType;

			switch (data.NoteType) {
			case '3':
			case '4':
				TypeMatch = type == data.NoteType - 2;
				break;
			}

			if (!(BadHit && TypeMatch)) { continue; }

			switch (data.NoteType) {
			case '3':
			case '4':
				if (data.BigNoteTime == 0) {
					data.BigNoteTime = nowtime;
					return;
				}
			}

			const int addscore = Chart.AddScore;
			if (GoodHit) {
				HitNote[0].Add(HitNoteData(data.NoteType, JudgeType::Good));
				Judge.Hit(JudgeType::Good, addscore, type, __ConfigPtr->AutoPlayFlag);
			}
			else if (OkHit) {
				HitNote[0].Add(HitNoteData(data.NoteType, JudgeType::Ok));
				Judge.Hit(JudgeType::Ok, addscore, type, __ConfigPtr->AutoPlayFlag);
			}
			else if (BadHit) {
				HitNote[0].Add(HitNoteData('\0', JudgeType::Bad));
				Judge.Hit(JudgeType::Bad, 0, '\0', __ConfigPtr->AutoPlayFlag);
			}

			Chart.HitErrorTime.push_back(_HitError);
			Chart.CursorMove.Start();

			data.HitFlag = true;
			data.NoteType = '\0';

			return;
		}

		if (rollcount > 0) {
			HitNote[0].Add(HitNoteData(NextImage ? '6' : '5', JudgeType::Roll));
			Judge.Hit(JudgeType::Roll, 100, NextImage ? '6' : '5', __ConfigPtr->AutoPlayFlag);
		}

		if (type == '1' && balloondata != nullptr) {
			--balloondata->BalloonCount;
			Judge.Hit(JudgeType::Roll, 100, '\0', __ConfigPtr->AutoPlayFlag);
			if (balloondata->BalloonCount <= 0) {
				__SkinPtr->Base->Playing.SE.Balloon.Play();
				HitNote[0].Add(HitNoteData('3', JudgeType::Roll));
				balloondata->NoteType = '0';
				balloondata->HitFlag = true;
				balloondata->BalloonFlag = 2;
				Judge.NoteType = '3';
			}
		}
	}

	void JudgeUnderExplosionDraw(const Pos2D<float> add, _HitNote& HitNote) {

		int i = HitNote.Index;
		const double JudgeUnderExplosionTime = __SkinPtr->Base->Playing.Config.JudgeUpperExplosionFrameTime * __SkinPtr->Base->Playing.Image.JudgeUnderExplosion.Div.X;

		for (int c = 0; c < HitNote.Size(); ++c) {
			auto&& data = HitNote.Datas[i];
			if (!data.MoveTimer.GetNowRecording()) {
				data.MoveTimer.Start();
			}

			data.MoveElapsedTime = data.MoveTimer.GetRecordingTime() / TimerType::microsecond;

			if (data.JudgeUnderExplosion.IsActive && data.MoveElapsedTime < JudgeUnderExplosionTime) {

				uint drawindex = data.MoveElapsedTime / __SkinPtr->Base->Playing.Config.JudgeUpperExplosionFrameTime;

				drawindex += (2 * __SkinPtr->Base->Playing.Image.JudgeUnderExplosion.Div.X) * data.JudgeUnderExplosion.Big;

				if (data.JudgeUnderExplosion.Type == JudgeType::Ok) {
					drawindex += __SkinPtr->Base->Playing.Image.JudgeUnderExplosion.Div.X;
				}

				__SkinPtr->Base->Playing.Image.JudgeUnderExplosion.Draw(add, drawindex);

			}
			else {
				data.JudgeUnderExplosion.IsActive = false;
			}

			++i;
			if (!(i < HitNote.Size())) {
				i = 0;
			}
		}
	}

	void HitNoteDraw(_HitNote& HitNote, Pos2D<float> add, int CountAll, int pldx) {

		int i = HitNote.Index;

		for (int c = 0; c < HitNote.Size(); ++c) {
			auto& data = HitNote.Datas[i];
			if (!data.MoveTimer.GetNowRecording()) {
				data.MoveTimer.Start();
			}

			if (data.FlyingNote.IsActive && __ConfigPtr->HitNoteDisp && data.MoveElapsedTime < data.FlyingNote.MoveTime()) {

				float _one = (data.MoveElapsedTime / data.FlyingNote.MoveTime());

				std::complex<float> _pos1 = { 840, -90 };
				std::complex<float> _pos2 = std::polar(280.0f, (DX_PI_F / 2) + std::arg(_pos1 - add.Y));
				std::complex<float> _r = { (_pos1.real() / 2) + _pos2.real(), (_pos1.imag() / 2) + _pos2.imag() };
				float mem0arg = std::arg(_r);
				_r += { -840, 90 };
				float mem1arg = std::arg(_r);
				_r = { (_pos1.real() / 2) + _pos2.real(), (_pos1.imag() / 2) + _pos2.imag() };
				float allarg = mem1arg - mem0arg;
				std::complex<float> _c1 = std::polar(std::abs(_r), allarg * _one + mem0arg);

				float imagpos = !pldx ? _r.imag() - _c1.imag() : _c1.imag() - _r.imag();

				Pos2D<float> Pos = {
						_r.real() - _c1.real(),
						imagpos + add.Y
				};

				if (CountAll < 3) {
					__SkinPtr->Base->Playing.Image.Note.Draw(Pos, data.FlyingNote.Type - 48);
				}
			}
			else {
				data.FlyingNote.IsActive = false;
			}

			if (data.JudgeString.IsActive && data.MoveElapsedTime < data.JudgeString.MoveTime()) {
				double alpha = 255 * (1 - GetEasingRate(data.MoveElapsedTime / data.JudgeString.MoveTime(), ease::Base::In, ease::Line::Cubic));
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
				__SkinPtr->Base->Playing.Image.JudgeString.Draw(add, (int)data.JudgeString.Type);
				SetDrawBlendMode(0, 0);
			}
			else {
				data.JudgeString.IsActive = false;
			}

			if (!data.FlyingNote.IsActive && !data.JudgeUnderExplosion.IsActive && !data.JudgeString.IsActive) {
				data = HitNoteData();
			}

			++i;
			if (!(i < HitNote.Size())) {
				i = 0;
			}
		}
	}

	void MiniTaikoFlashDraw(Pos2D<float> add, int pldx) {

		static auto TaikoAlpha = [&](int index) {
			double alpha = 255 * (1 - GetEasingRate(MiniTaikoFlash[index].GetRecordingTime() / MiniTaikoFlashTime, ease::Base::In, ease::Line::Cubic));
			if (alpha < 0) { MiniTaikoFlash[index].End(); }
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			};

#define TAIKOFLASH(type, dir, x) { type.Draw({type.Size.Width * dir, add.Y}, x); }

		if (MiniTaikoFlash[0 + 4 * pldx].GetNowRecording()) {
			TaikoAlpha(0 + 4 * pldx);
			TAIKOFLASH(__SkinPtr->Base->Playing.Image.MiniTaiko_Don, -0.5f, 0)
		}
		if (MiniTaikoFlash[1 + 4 * pldx].GetNowRecording()) {
			TaikoAlpha(1 + 4 * pldx);
			TAIKOFLASH(__SkinPtr->Base->Playing.Image.MiniTaiko_Ka, -0.5f, 0)
		}
		if (MiniTaikoFlash[2 + 4 * pldx].GetNowRecording()) {
			TaikoAlpha(2 + 4 * pldx);
			TAIKOFLASH(__SkinPtr->Base->Playing.Image.MiniTaiko_Don, 0.5f, 1)
		}
		if (MiniTaikoFlash[3 + 4 * pldx].GetNowRecording()) {
			TaikoAlpha(3 + 4 * pldx);
			TAIKOFLASH(__SkinPtr->Base->Playing.Image.MiniTaiko_Ka, 0.5f, 1)
		}
		SetDrawBlendMode(0, 0);
#undef TAIKOFLASH
	}

	void TitleDraw(FontData font, Pos2D<float> pos, std::string str, int strlen) {
		font.Draw(
			pos,
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			strlen,
			str
		);
	}

	void NumberDraw(GraphData Image, ulonglong num, uint type, Pos2D<float> pos) {
		int digit = std::digit(num);
		float offset = 0;

		if (type == 0) {
			offset = Image.Size.Width * (digit - 1) / 2;
		}
		if (type == 1) {
			offset = Image.Size.Width - (digit - 1) + digit;
		}

		int i = 0;
		do {
			Image.Draw({ offset, pos.Y }, num % 10);
			num /= 10;
			++i;
			offset -= Image.Size.Width;
		} while (i < digit);
	}

	void NameDraw(std::string name, Pos2D<float> pos) {
		__SkinPtr->Base->Playing.Font.PlayerName.Draw({
			__SkinPtr->Base->Playing.Config.PlayerNamePos.X,
			__SkinPtr->Base->Playing.Config.PlayerNamePos.Y + pos.Y },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			GetStrlen(name, __SkinPtr->Base->Playing.Font.PlayerName.Handle),
			name
			);
	}

	void ProgressBarDraw(int index, Pos2D<float> pos) {

		__SkinPtr->Base->Playing.Image.ProgressBar.Draw(pos, 0);

		if (!Chart.Judge[index].HitNote) { return; }

		double Ratio = ((double)Chart.Judge[index].Good + (double)Chart.Judge[index].Ok * 0.5) / Chart.AllNoteCount;
		float Width = __SkinPtr->Base->Playing.Image.ProgressBar.Size.Width * Ratio;
		float MaxWidth = __SkinPtr->Base->Playing.Image.ProgressBar.Size.Width;

		__SkinPtr->Base->Playing.Image.ProgressBar.RectDraw(
			pos,
			{ 0,__SkinPtr->Base->Playing.Image.ProgressBar.Size.Height },
			{ Width < MaxWidth ? Width : MaxWidth,
			__SkinPtr->Base->Playing.Image.ProgressBar.Size.Height },
			1
		);
	}

	void ExamProgressBarDraw() {

		for (uint i = 0; i < Chart.ExamDatas.size(); i++) {

			__SkinPtr->Base->Playing.Image.ExamProgressBar.Draw({ 0,120.0f * i }, 0);

			double Ratio = (double)Chart.ExamDatas[i].ExamVals / (double)Chart.OriginalData.ExamDatas[i].PassVal[0];
			float Width = __SkinPtr->Base->Playing.Image.ExamProgressBar.Size.Width * Ratio;
			float MaxWidth = __SkinPtr->Base->Playing.Image.ExamProgressBar.Size.Width;

			if (!Chart.ExamDatas[i].IsFall) {
				__SkinPtr->Base->Playing.Image.ExamProgressBar.RectDraw(
					{ 0,120.0f * i },
					{ 0, __SkinPtr->Base->Playing.Image.ExamProgressBar.Size.Height },
					{ Width < MaxWidth ? Width : MaxWidth,
					__SkinPtr->Base->Playing.Image.ExamProgressBar.Size.Height },
					1
				);
			}
		}
	}

	void ExamValDraw() {

		for (uint i = 0; i < Chart.ExamDatas.size(); i++) {

			auto ExamData = Chart.OriginalData.ExamDatas[i];
			auto ExamVal = Chart.ExamDatas[i].ExamVals;
			bool IsFall = Chart.ExamDatas[i].IsFall;
			bool IsPass = ExamData.PassVal[0] <= ExamVal && ExamData.Range == ExamRange::More;
			std::string valstr = !IsFall ? std::to_string(ExamVal) : "0";
			std::string examname = ExamList[(int)ExamData.ExamType];

			__SkinPtr->Base->Playing.Font.ExamName.Draw(
				{ __SkinPtr->Base->Playing.Config.ExamNamePos.X,
				__SkinPtr->Base->Playing.Config.ExamNamePos.Y + (120.0f * i) },
				GetColor(255, 255, 255),
				GetColor(0, 0, 0),
				GetStrlen(examname, __SkinPtr->Base->Playing.Font.ExamName.Handle),
				examname
			);
			__SkinPtr->Base->Playing.Font.ExamVal.Draw(
				{ __SkinPtr->Base->Playing.Config.ExamValPos.X,
				__SkinPtr->Base->Playing.Config.ExamValPos.Y + (120.0f * i) },
				GetColor(255, 255 * !IsFall, 255 * !IsFall * !IsPass),
				GetColor(0, 0, 0),
				GetStrlen(valstr, __SkinPtr->Base->Playing.Font.ExamVal.Handle),
				valstr
			);
		}
	}

	void KeyCharDraw(int array, std::vector<int> keys, std::vector<std::string> strs) {

		static auto KeyAlpha = [&](int array, int index) {
			if (!KeyFlash[array][index].GetNowRecording()) {
				return 0.0;
			}
			double alpha = 255 * (1 - GetEasingRate(KeyFlash[array][index].GetRecordingTime() / KeyFlashTime, ease::Base::In, ease::Line::Cubic));
			if (alpha < 0) { KeyFlash[array][index].End(); }
			return alpha;
			};

		for (int i = 0; i < keys.size(); i++) {

			double alpha = KeyAlpha(array, i);

			if (strs[i] != "*") {

				__SkinPtr->Base->Playing.Image.KeyViewBack.Draw({
					__SkinPtr->Base->Playing.Image.KeyViewBack.Size.Width * array,
					__SkinPtr->Base->Playing.Image.KeyViewBack.Size.Height * i });

				__SkinPtr->Base->Playing.Font.KeyStr.Draw({
					__SkinPtr->Base->Playing.Config.KeyStrPos.X + __SkinPtr->Base->Playing.Image.KeyViewFlash.Size.Width * array,
					__SkinPtr->Base->Playing.Config.KeyStrPos.Y + __SkinPtr->Base->Playing.Image.KeyViewFlash.Size.Height * i },
					GetColor(255, 255, 255),
					GetColor(0, 0, 0),
					GetStrlen(strs[i], __SkinPtr->Base->Playing.Font.KeyStr.Handle),
					strs[i]
					);

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
				__SkinPtr->Base->Playing.Image.KeyViewFlash.Draw({
					__SkinPtr->Base->Playing.Image.KeyViewFlash.Size.Width * array,
					__SkinPtr->Base->Playing.Image.KeyViewFlash.Size.Height * i });
				SetDrawBlendMode(0, 0);
			}
		}
	}

	void ScoreMeterDraw() {

		const float Center = __SkinPtr->Info.Resolution.X / 2;
		const float Edge = (Center / 640) * 320;
		const float GoodBorder[2] = { Center - Edge * (__ConfigPtr->JudgeGood / __ConfigPtr->JudgeBad), Center + Edge * (__ConfigPtr->JudgeGood / __ConfigPtr->JudgeBad) };
		const float OkBorder[2]   = { Center - Edge * (__ConfigPtr->JudgeOk / __ConfigPtr->JudgeBad), Center + Edge * (__ConfigPtr->JudgeOk / __ConfigPtr->JudgeBad) };
		const float BadBorder[2]  = { Center - Edge, Center + Edge };

		DrawLineAA(BadBorder[0] - 2.5f, 700, BadBorder[1] + 2.5f, 700, GetColor(100, 100, 100), 15.0f);
		DrawLineAA(BadBorder[0], 700, BadBorder[1], 700, GetColor(0, 200, 225), 10.0f);
		DrawLineAA(std::max(OkBorder[0], BadBorder[0]), 700, std::min(OkBorder[1], BadBorder[1]), 700, GetColor(100, 255, 25), 10.0f);
		DrawLineAA(std::max(GoodBorder[0], BadBorder[0]), 700, std::min(GoodBorder[1], BadBorder[1]), 700, GetColor(255, 125, 25), 10.0f);
		DrawLineAA(640, 705, 640, 695, GetColor(255, 255, 255), 8.0f);

		float dest = 0;
		float average = 0;
		int i = 0;

		for (auto&& errortime : Chart.HitErrorTime | std::views::reverse) {

			float dest = Edge * (errortime / __ConfigPtr->JudgeBad);

			const Pos2D<float> ScoreLinePos = { 640 + dest, 700 };

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::lerp(255, -255, (double)(i + 1) / __ConfigPtr->ScoreMeterBuffer));
			DrawLineAA(ScoreLinePos.X, ScoreLinePos.Y - 10, ScoreLinePos.X, ScoreLinePos.Y + 10, GetColor(255, 255, 255), 2.0f);
			SetDrawBlendMode(0, 0);

			if (i > __ConfigPtr->ScoreMeterBuffer) {
				Chart.HitErrorTime.erase(Chart.HitErrorTime.end() - i);
				continue;
			}

			average += dest;
			i++;
		}

		if (i > 0) {		
			dest = average / i;
		}

		double time = GetEasingRate(Chart.CursorMove.GetRecordingTime() / Chart.CursorMoveTime, ease::Base::In, ease::Line::Linear);
		Chart.CursorPos = std::lerp(Chart.CursorPos, dest, time);

		__SkinPtr->Base->Playing.Image.ScoreCursor.Draw({ Chart.CursorPos, 0 });

	}

	void Action(HitType type);
};