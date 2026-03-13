#pragma once
#include "Include.hpp"
#include "SongSelect.h"
#include "Config.h"
#include "Skin.h"

enum class BranchLevel : int {
	None = -1,
	Normal,
	Expert,
	Master,
	Count
};

enum class BranchType : int {
	Null = -1,
	Accuracy,
	Roll,
	Score
};

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

	Packet::bytearray ToBytes() const {
		Packet::bytearray ret;
		Packet::StoreBytes(ret, AbsTime);
		Packet::StoreBytes(ret, RelaTime);
		Packet::StoreBytes(ret, BMTime);
		Packet::StoreBytes(ret, PosTime);
		Packet::StoreBytes(ret, BMFlag);
		Packet::StoreBytes(ret, BpmChangeFlag);
		Packet::StoreBytes(ret, BpmSpawnFlag);
		Packet::StoreBytes(ret, BigNoteTime);
		Packet::StoreBytes(ret, BPM);
		Packet::StoreBytes(ret, Measure);
		Packet::StoreBytes(ret, Scroll);
		Packet::StoreBytes(ret, Scrolli);
		Packet::StoreBytes(ret, GoGoStart);
		Packet::StoreBytes(ret, GoGoEnd);
		Packet::StoreBytes(ret, NoteType);
		Packet::StoreBytes(ret, RollEndTime);
		Packet::StoreBytes(ret, RollEndIndex);
		Packet::StoreBytes(ret, RollFlag);
		Packet::StoreBytes(ret, BalloonFlag);
		Packet::StoreBytes(ret, BalloonCount);
		Packet::StoreBytes(ret, BarlineDisplay);
		Packet::StoreBytes(ret, HitFlag);
		return ret;
	}

	Packet::byte_view FromBytes(Packet::byte_view view) {
		Packet::LoadBytes(view, AbsTime);
		Packet::LoadBytes(view, RelaTime);
		Packet::LoadBytes(view, BMTime);
		Packet::LoadBytes(view, PosTime);
		Packet::LoadBytes(view, BMFlag);
		Packet::LoadBytes(view, BpmChangeFlag);
		Packet::LoadBytes(view, BpmSpawnFlag);
		Packet::LoadBytes(view, BigNoteTime);
		Packet::LoadBytes(view, BPM);
		Packet::LoadBytes(view, Measure);
		Packet::LoadBytes(view, Scroll);
		Packet::LoadBytes(view, Scrolli);
		Packet::LoadBytes(view, GoGoStart);
		Packet::LoadBytes(view, GoGoEnd);
		Packet::LoadBytes(view, NoteType);
		Packet::LoadBytes(view, RollEndTime);
		Packet::LoadBytes(view, RollEndIndex);
		Packet::LoadBytes(view, RollFlag);
		Packet::LoadBytes(view, BalloonFlag);
		Packet::LoadBytes(view, BalloonCount);
		Packet::LoadBytes(view, BarlineDisplay);
		Packet::LoadBytes(view, HitFlag);
		return view;
	}
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
	BranchLevel Level = BranchLevel::None;
	BranchLevel PrevLevel = BranchLevel::Normal;

	void Hit(JudgeType type, int addscore, char note) {

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
		SongData.Delete();
		FrameNowTime.End();
		NowTime.End();
		ScrollType = ScrollType::Normal;
		OriginalData = ChartData();
		AutoPlayLR = false;
		NowGoGo = false;
		BalloonCount = 0;
		AllNoteCount = 0;
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

	uint DanPlayCount = 0;
	uint DanBalloonIndex = 0;

	bool IsDanPlay = false;
	bool IsFall = true;
	std::vector<ExamStreamData> ExamDatas = std::vector<ExamStreamData>();
};

extern class GameSystem* gameptr;

class _Playing {
public:

	_Playing(GameSystem* ptr);
	~_Playing();

	ChartStreamData Chart;
	JudgeData HighScore[5];

	Timer<millisecond> MiniTaikoFlash[16];
	double MiniTaikoFlashTime = 160;

	double TrainingOffset = 0;
	ulonglong MeasureIndex = 0;

	Timer<millisecond> MeasureJump;
	double MeasureJumpTime = 60;
	double MemNowTime = 0;

	std::string ExamList[8] = { "Accuracy", "Good", "Ok", "Bad", "Score", "Roll", "HitNote", "MaxCombo" };

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

	void NoteAlpha(int& Alpha, double _one, AlphaType Type) {

		if (_one > 1) {
			_one = 1;
		}
		if (_one < 0) {
			_one = 0;
		}

		switch (Type) {
		case AlphaType::Hidden:
			Alpha = 255 * _one;
			break;
		case AlphaType::Sudden:
			Alpha = Alpha * (1 - _one);
			break;
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, Alpha);
	}

	void TaikoAlpha(int index) {
		double alpha = 255 * (1 - GetEasingRate(MiniTaikoFlash[index].GetRecordingTime() / MiniTaikoFlashTime, ease::Base::In, ease::Line::Cubic));
		if (alpha < 0) { MiniTaikoFlash[index].End(); }
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	}

	template<typename T>
	void NoteDraw(T&& MultiData, _Skin* Skin, _Config* Config, std::vector<NoteData>& NoteDatas, double NowTime, const Pos2D<float>& add, bool MultiFlag, int pldx, double Hidden, double Sudden) {

		const Pos2D<double>& NoteOrigin = {
	Skin->Base->Playing.Image.Note.Pos.X,
	Skin->Base->Playing.Image.Note.Pos.Y + add.Y
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
				_ret.X *= Skin->Base->Playing.Config.LaneExtendRate * (BMScroll ? 1 : data.Scroll),
				_ret.Y *= Skin->Base->Playing.Config.LaneExtendRate * (BMScroll ? 0 : data.Scrolli)
			};

			Pos2D<float> SkinPos = {
				Skin->Base->Playing.Image.Lane.Size.Width,
				Skin->Base->Playing.Image.Lane.Size.Height
			};

			_ret = {
				_ret.X + NoteOrigin.X,
				_ret.Y + NoteOrigin.Y
			};

			return _ret;

			};

#define InRange(x, y) (x > Skin->SimulationRect.Left && x < Skin->SimulationRect.Right && y > Skin->SimulationRect.Top && y < Skin->SimulationRect.Bottom)

		std::complex<double> n1{};
		std::complex<double> n2{};
		std::complex<double> n3{};
		std::complex<double> n4{};
		std::complex<double> facing{};
		Pos2D<double> NotePos{};

		const std::complex<double> n0 = { Skin->Base->Playing.Image.Note.Size.Width / 2, Skin->Base->Playing.Image.Note.Size.Height / 2 };
		const double n0r = std::abs(n0);
		const double narr[4] = {
			std::arg(std::complex<double>{ n0.real() * -1, n0.imag() * -1 }),
			std::arg(std::complex<double>{ n0.real() * 1, n0.imag() * -1 }),
			std::arg(std::complex<double>{ n0.real() * 1, n0.imag() * 1 }),
			std::arg(std::complex<double>{ n0.real() * -1, n0.imag() * 1 })
		};

		for (int i = 0; auto&& data : NoteDatas | std::ranges::views::reverse) {

			double NoteTheta = atan2(data.Scrolli, data.Scroll);

			NotePos = GetNotePos(data);

			if (data.BarlineDisplay) {

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
					double hidden = Config->HiddenLevel > 0 ? Config->HiddenLevel : MultiFlag ? Hidden : 0;
					double sudden = Config->SuddenLevel > 0 ? Config->SuddenLevel : MultiFlag ? Sudden : 0;
					bool hiddenflag = hidden > 0;
					bool suddenflag = sudden > 0;
					bool multiflag = pldx;
					if (hiddenflag || suddenflag || multiflag) {
						double _abs = std::abs(std::complex<double>{ NotePos.X - NoteOrigin.X, NotePos.Y - NoteOrigin.Y })* (data.AbsTime < NowTime ? -1 : 1);
						double leveling = (Skin->Base->Playing.Image.Lane.Size.Width / DX_PI);
						double feedrange = (Skin->Base->Playing.Image.Lane.Size.Width / DX_TWO_PI);
						if (multiflag) {
							NoteAlpha(Alpha, (_abs - (leveling * 0.025)) / feedrange, AlphaType::Hidden);
						}
						if (hiddenflag) {
							NoteAlpha(Alpha, (_abs - (leveling * hidden)) / feedrange, AlphaType::Hidden);
						}
						if (suddenflag) {
							NoteAlpha(Alpha, ((_abs - (Skin->Base->Playing.Image.Lane.Size.Width)) + (leveling * sudden)) / feedrange, AlphaType::Sudden);
						}
					}

					auto&& note = !pldx ? data.NoteType : MultiData.NoteType[i];

					Skin->Base->Playing.Image.Note.Draw(
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
				const Pos2D<double>& dnote = GetNotePos(NoteDatas[data.RollEndIndex]);

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
						Skin->Base->Playing.Image.Note.Handles[BigRollFlag ? 9 : 6],
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
						Skin->Base->Playing.Image.Note.Handles[BigRollFlag ? 10 : 7],
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
						Skin->Base->Playing.Image.Note.Handles[BigRollFlag ? 8 : 5],
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
					NotePos = GetNotePos(NoteDatas[data.RollEndIndex]);
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
						Skin->Base->Playing.Image.Note.Handles[KusudamaFlag ? 13 : 11],
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
						Skin->Base->Playing.Image.Note.Handles[KusudamaFlag ? 14 : 12],
						TRUE
					);
				}
			}
		}

#undef InRange
	}

	template<typename T>
	void NoteProc(_Skin* Skin, _Config* Config, T&& NoteDatas, double nowtime) {

		Chart.BalloonCount = 0;

		for (auto&& data : NoteDatas) {

			bool HitFlag = data.AbsTime < nowtime;

			if (data.GoGoStart && HitFlag) {
				Chart.NowGoGo = true;
			}
			if (data.GoGoEnd && HitFlag) {
				Chart.NowGoGo = false;
			}

			if (data.AbsTime - Config->JudgeBad > nowtime) { continue; }
			if (data.HitFlag) { continue; }

			const double _HitError = data.AbsTime - nowtime;
			const bool BadHit = _HitError > -Config->JudgeBad && _HitError < Config->JudgeBad;

			if (data.BigNoteTime != 0 && Config->JudgeGood < nowtime - data.BigNoteTime) {
				JudgeNote(Skin, Config, nowtime, data.NoteType - 2);
			}

			if (!data.HitFlag &&
				(data.NoteType >= '1' && data.NoteType <= '4') &&
				data.BigNoteTime == 0 &&
				_HitError < -Config->JudgeBad) {
				Chart.Judge[0].Hit(JudgeType::Bad, 0, '\0');
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

	template<typename T>
	void AutoPlayProc(_Skin* Skin, _Config* Config, T&& NoteDatas, double NowTime) {

		int RollCount = 0;
		NoteData* BalloonData = nullptr;

		bool NextImage = false;

		for (auto&& data : NoteDatas) {

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
				Chart.Judge[0].Hit(JudgeType::Good, 0, data.NoteType);
				switch (data.NoteType) {
				case '1':
					Skin->Base->Playing.SE.Don.Play();
					MiniTaikoFlash[0 + Chart.AutoPlayLR * 2].Start();
					Chart.AutoPlayLR = !Chart.AutoPlayLR;
					Action((HitType)(0 + Chart.AutoPlayLR * 2));
					break;
				case '2':
					Skin->Base->Playing.SE.Ka.Play();
					MiniTaikoFlash[1 + Chart.AutoPlayLR * 2].Start();
					Chart.AutoPlayLR = !Chart.AutoPlayLR;
					Action((HitType)(1 + Chart.AutoPlayLR * 2));
					break;
				case '3':
					Skin->Base->Playing.SE.Don.Play();
					Skin->Base->Playing.SE.Don.Play();
					MiniTaikoFlash[(int)HitType::DonLeft].Start();
					MiniTaikoFlash[(int)HitType::DonRight].Start();
					Action(HitType::DonLeft);
					Action(HitType::DonRight);
					break;
				case '4':
					Skin->Base->Playing.SE.Ka.Play();
					Skin->Base->Playing.SE.Ka.Play();
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
		const double WaitRollTime = 1 / Config->RollSpeed;
		if (RollCount > 0 && WaitRollTimer > WaitRollTime) {
			Skin->Base->Playing.SE.Don.Play();
			Chart.AutoPlayLR = !Chart.AutoPlayLR;
			Chart.Judge[0].Roll++;
			HitNote[0].Add(HitNoteData(NextImage ? '6' : '5', JudgeType::Roll));
			Chart.WaitRollTime.Start();
		}
		if (BalloonData != nullptr && WaitRollTimer > WaitRollTime) {
			Skin->Base->Playing.SE.Don.Play();
			Chart.AutoPlayLR = !Chart.AutoPlayLR;
			Chart.Judge[0].Roll++;
			--BalloonData->BalloonCount;
			Chart.WaitRollTime.Start();
			if (BalloonData->BalloonCount <= 0) {
				Skin->Base->Playing.SE.Balloon.Play();
				HitNote[0].Add(HitNoteData('3', JudgeType::Roll));
				BalloonData->NoteType = '0';
				BalloonData->HitFlag = true;
				BalloonData->BalloonFlag = 2;
				Chart.Judge[0].NoteType = '3';
			}
		}
	}

	void PlayProc(_Skin* Skin, _Config* Config, double NowTime) {

		Input.HitKeyesProcess(Config->DonInputLeft, KeyState::Down, [&] {
			Skin->Base->Playing.SE.Don.Play();
			MiniTaikoFlash[(int)HitType::DonLeft].Start();
			JudgeNote(Skin, Config, NowTime, '1');
			Action(HitType::DonLeft);
			});
		Input.HitKeyesProcess(Config->DonInputRight, KeyState::Down, [&] {
			Skin->Base->Playing.SE.Don.Play();
			MiniTaikoFlash[(int)HitType::DonRight].Start();
			JudgeNote(Skin, Config, NowTime, '1');
			Action(HitType::DonRight);
			});
		Input.HitKeyesProcess(Config->KaInputLeft, KeyState::Down, [&] {
			Skin->Base->Playing.SE.Ka.Play();
			MiniTaikoFlash[(int)HitType::KaLeft].Start();
			JudgeNote(Skin, Config, NowTime, '2');
			Action(HitType::KaLeft);
			});
		Input.HitKeyesProcess(Config->KaInputRight, KeyState::Down, [&] {
			Skin->Base->Playing.SE.Ka.Play();
			MiniTaikoFlash[(int)HitType::KaRight].Start();
			JudgeNote(Skin, Config, NowTime, '2');
			Action(HitType::KaRight);
			});
	}

	void TraningModeProc(_Config* Config, double nowtime) {

		if (!MeasureJump.GetNowRecording()) {

			static auto MoveInputProc = [&](bool direction) {
				while (!ProcessMessage()) {
					if (direction) {
						MeasureIndex < Chart.RawNoteDatas.size() - 1 ? MeasureIndex++ : MeasureIndex;
						if (Chart.RawNoteDatas[MeasureIndex].BarlineDisplay || MeasureIndex == Chart.RawNoteDatas.size() - 1) { break; }
					}
					else {
						MeasureIndex > 0 ? MeasureIndex-- : MeasureIndex;
						if (Chart.RawNoteDatas[MeasureIndex].BarlineDisplay || MeasureIndex == 0) { break; }
					}
				}
				MemNowTime = nowtime;
				MeasureJump.Start();
				};

			static auto StartInputProc = [&] {
				Chart.NowTime.Start();
				Chart.FrameNowTime.Start();
				if (Chart.SongBlankTime < nowtime) {
					Chart.SongData.SetCurrent(nowtime - Chart.SongBlankTime);
					Chart.SongData.Play(FALSE);
				}
				};

			Input.HitKeyesProcess(Config->KaInputLeft, KeyState::Down, [&] { MoveInputProc(false); });
			Input.HitKeyesProcess(Config->KaInputRight, KeyState::Down, [&] { MoveInputProc(true); });
			Input.HitKeyProcess(VK_NEXT, KeyState::Down, [&] { MoveInputProc(false); });
			Input.HitKeyProcess(VK_PRIOR, KeyState::Down, [&] { MoveInputProc(true); });

			Input.HitKeyesProcess(Config->DonInputLeft, KeyState::Down, StartInputProc);
			Input.HitKeyesProcess(Config->DonInputRight, KeyState::Down, StartInputProc);
			Input.HitKeyProcess(VK_RETURN, KeyState::Down, StartInputProc);
		}
		else {

			TrainingOffset = std::lerp(MemNowTime, Chart.RawNoteDatas[MeasureIndex].AbsTime, GetEasingRate(MeasureJump.GetRecordingTime() / MeasureJumpTime, ease::Base::In, ease::Line::Linear));

			if (MeasureJump.GetRecordingTime() >= MeasureJumpTime) {
				MeasureJump.End();
			}
		}
	}

	void DanProc(_Skin* Skin) {

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
					Skin->Base->Playing.SE.DanFall.Play();
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

	void JudgeNote(_Skin* Skin, _Config* Config, double nowtime, char type) {

		auto&& NoteDatas = Chart.RawNoteDatas;
		auto& Judge = Chart.Judge[0];

		int rollcount = 0;
		int ballooncount = 0;
		NoteData* balloondata = nullptr;

		bool NextImage = false;

		for (auto&& data : NoteDatas) {

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
				if (Config->JudgeGood < nowtime - data.BigNoteTime) {
					data.NoteType -= 2;
				}
				nowtime = data.BigNoteTime;
			}

			const double _HitError = data.AbsTime - nowtime;
			const bool GoodHit =
				_HitError > -Config->JudgeGood && _HitError < Config->JudgeGood;
			const bool OkHit =
				_HitError > -Config->JudgeOk && _HitError < Config->JudgeOk;
			const bool BadHit =
				_HitError > -Config->JudgeBad && _HitError < Config->JudgeBad;
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
				Judge.Hit(JudgeType::Good, addscore, type);
			}
			else if (OkHit) {
				HitNote[0].Add(HitNoteData(data.NoteType, JudgeType::Ok));
				Judge.Hit(JudgeType::Ok, addscore, type);
			}
			else if (BadHit) {
				HitNote[0].Add(HitNoteData('\0', JudgeType::Bad));
				Judge.Hit(JudgeType::Bad, 0, '\0');
			}

			data.HitFlag = true;
			data.NoteType = '\0';

			return;
		}

		if (rollcount > 0) {
			HitNote[0].Add(HitNoteData(NextImage ? '6' : '5', JudgeType::Roll));
			Judge.Hit(JudgeType::Roll, 100, NextImage ? '6' : '5');
		}

		if (type == '1' && balloondata != nullptr) {
			--balloondata->BalloonCount;
			Judge.Hit(JudgeType::Roll, 100, '\0');
			if (balloondata->BalloonCount <= 0) {
				Skin->Base->Playing.SE.Balloon.Play();
				HitNote[0].Add(HitNoteData('3', JudgeType::Roll));
				balloondata->NoteType = '0';
				balloondata->HitFlag = true;
				balloondata->BalloonFlag = 2;
				Judge.NoteType = '3';
			}
		}
	}

	void NoteDrawData(std::vector<NoteData>& ProcNotes, double NowTime) {
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
	}

	void GoGoFireDraw(_Skin* Skin, Pos2D<float> add, double NowTime) {

		uint drawindex = NowTime / Skin->Base->Playing.Config.GoGoFireFrameTime;

		Skin->Base->Playing.Image.GoGoFire.Draw(
			{
			Skin->Base->Playing.Image.GoGoFire.Pos.X,
			Skin->Base->Playing.Image.GoGoFire.Pos.Y + add.Y
			},
			drawindex % Skin->Base->Playing.Image.GoGoFire.Div.X);
	}

	void JudgeUnderExplosionDraw(_Skin* Skin, const Pos2D<float> add, _HitNote& HitNote) {

		int i = HitNote.Index;
		const double JudgeUnderExplosionTime = Skin->Base->Playing.Config.JudgeUpperExplosionFrameTime * Skin->Base->Playing.Image.JudgeUnderExplosion.Div.X;

		for (int c = 0; c < HitNote.Size(); ++c) {
			auto&& data = HitNote.Datas[i];
			if (!data.MoveTimer.GetNowRecording()) {
				data.MoveTimer.Start();
			}

			data.MoveElapsedTime = data.MoveTimer.GetRecordingTime() / TimerType::microsecond;

			if (data.JudgeUnderExplosion.IsActive && data.MoveElapsedTime < JudgeUnderExplosionTime) {

				uint drawindex = data.MoveElapsedTime / Skin->Base->Playing.Config.JudgeUpperExplosionFrameTime;

				drawindex += (2 * Skin->Base->Playing.Image.JudgeUnderExplosion.Div.X) * data.JudgeUnderExplosion.Big;

				if (data.JudgeUnderExplosion.Type == JudgeType::Ok) {
					drawindex += Skin->Base->Playing.Image.JudgeUnderExplosion.Div.X;
				}

				Skin->Base->Playing.Image.JudgeUnderExplosion.Draw(add, drawindex);

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

	void HitNoteDraw(_Skin* Skin, _Config* Config, _HitNote& HitNote, Pos2D<float> add, int CountAll, int pldx) {

		int i = HitNote.Index;

		for (int c = 0; c < HitNote.Size(); ++c) {
			auto& data = HitNote.Datas[i];
			if (!data.MoveTimer.GetNowRecording()) {
				data.MoveTimer.Start();
			}

			if (data.FlyingNote.IsActive && Config->HitNoteDisp && data.MoveElapsedTime < data.FlyingNote.MoveTime()) {

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
					Skin->Base->Playing.Image.Note.Draw(Pos, data.FlyingNote.Type - 48);
				}
			}
			else {
				data.FlyingNote.IsActive = false;
			}

			if (data.JudgeString.IsActive && data.MoveElapsedTime < data.JudgeString.MoveTime()) {
				double alpha = 255 * (1 - GetEasingRate(data.MoveElapsedTime / data.JudgeString.MoveTime(), ease::Base::In, ease::Line::Cubic));
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
				Skin->Base->Playing.Image.JudgeString.Draw(add, (int)data.JudgeString.Type);
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

	void MiniTaikoFlashDraw(_Skin* Skin, Pos2D<float> add, int pldx) {

#define TAIKOFLASH(type, dir, x) { type.Draw({type.Size.Width * dir, add.Y}, x); }

		if (MiniTaikoFlash[0 + 4 * pldx].GetNowRecording()) {
			TaikoAlpha(0 + 4 * pldx);
			TAIKOFLASH(Skin->Base->Playing.Image.MiniTaiko_Don, -0.5f, 0)
		}
		if (MiniTaikoFlash[1 + 4 * pldx].GetNowRecording()) {
			TaikoAlpha(1 + 4 * pldx);
			TAIKOFLASH(Skin->Base->Playing.Image.MiniTaiko_Ka, -0.5f, 0)
		}
		if (MiniTaikoFlash[2 + 4 * pldx].GetNowRecording()) {
			TaikoAlpha(2 + 4 * pldx);
			TAIKOFLASH(Skin->Base->Playing.Image.MiniTaiko_Don, 0.5f, 1)
		}
		if (MiniTaikoFlash[3 + 4 * pldx].GetNowRecording()) {
			TaikoAlpha(3 + 4 * pldx);
			TAIKOFLASH(Skin->Base->Playing.Image.MiniTaiko_Ka, 0.5f, 1)
		}
		SetDrawBlendMode(0, 0);
#undef TAIKOFLASH
	}

	void TitleDraw(_Skin* Skin, std::string str, int strlen) {
		Skin->Base->Playing.Font.Title.Draw(
			Skin->Base->Playing.Config.TitlePos,
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			strlen,
			str
		);
	}

	void SubTitleDraw(_Skin* Skin, std::string str, int strlen) {
		Skin->Base->Playing.Font.SubTitle.Draw(
			Skin->Base->Playing.Config.SubTitlePos,
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			strlen,
			str
		);
	}

	void ComboDraw(_Skin* Skin, int index, Pos2D<float> pos) {
		ulonglong combo = Chart.Judge[index].Combo;

		int digit = std::digit(combo);

		float offset = Skin->Base->Playing.Image.ComboNumber.Size.Width * (digit - 1) / 2;
		int i = 0;
		do {
			Skin->Base->Playing.Image.ComboNumber.Draw({ offset, pos.Y }, combo % 10);
			combo /= 10;
			++i;
			offset -= Skin->Base->Playing.Image.ComboNumber.Size.Width;
		} while (i < digit);
	}

	void ScoreDraw(_Skin* Skin, int index, Pos2D<float> pos) {
		ulonglong score = Chart.Judge[index].Score;

		int digit = std::digit(score);

		float offset = Skin->Base->Playing.Image.ScoreNumber.Size.Width - (digit - 1) + digit;
		int i = 0;
		do {
			Skin->Base->Playing.Image.ScoreNumber.Draw({ offset, pos.Y }, score % 10);
			score /= 10;
			++i;
			offset -= Skin->Base->Playing.Image.ScoreNumber.Size.Width;
		} while (i < digit);
	}

	void BalloonDraw(_Skin* Skin, int val, Pos2D<float> pos) {
		int c = val;
		int digit = std::digit(c);

		float offset = Skin->Base->Playing.Image.BalloonNumber.Size.Width - (digit - 1) + digit;
		int i = 0;
		do {
			Skin->Base->Playing.Image.BalloonNumber.Draw({ offset, pos.Y }, c % 10);
			c /= 10;
			++i;
			offset -= Skin->Base->Playing.Image.BalloonNumber.Size.Width;
		} while (i < digit);
	}

	void NameDraw(_Skin* Skin, std::string name, Pos2D<float> pos) {

		Skin->Base->Playing.Font.PlayerName.Draw({
			Skin->Base->Playing.Config.PlayerNamePos.X,
			Skin->Base->Playing.Config.PlayerNamePos.Y + pos.Y },
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			GetStrlen(name, Skin->Base->Playing.Font.PlayerName.Handle),
			name
			);
	}

	void ProgressBarDraw(_Skin* Skin, int index, Pos2D<float> pos) {

		Skin->Base->Playing.Image.ProgressBar.Draw(pos, 0);

		if (!Chart.Judge[index].HitNote) { return; }

		double Ratio = ((double)Chart.Judge[index].Good + (double)Chart.Judge[index].Ok * 0.5) / Chart.AllNoteCount;
		float Width = Skin->Base->Playing.Image.ProgressBar.Size.Width * Ratio;

		Skin->Base->Playing.Image.ProgressBar.RectDraw(
			pos,
			{ 0,Skin->Base->Playing.Image.ProgressBar.Size.Height },
			{ std::min(Skin->Base->Playing.Image.ProgressBar.Size.Width, Width),
			Skin->Base->Playing.Image.ProgressBar.Size.Height },
			1
		);
	}

	void ExamProgressBarDraw(_Skin* Skin) {

		for (uint i = 0; i < Chart.ExamDatas.size(); i++) {

			Skin->Base->Playing.Image.ExamProgressBar.Draw({ 0,120.0f * i }, 0);

			double Ratio = (double)Chart.ExamDatas[i].ExamVals / (double)Chart.OriginalData.ExamDatas[i].PassVal[0];
			float Width = Skin->Base->Playing.Image.ExamProgressBar.Size.Width * Ratio;

			if (!Chart.ExamDatas[i].IsFall) {
				Skin->Base->Playing.Image.ExamProgressBar.RectDraw(
					{ 0,120.0f * i },
					{ 0, Skin->Base->Playing.Image.ExamProgressBar.Size.Height },
					{ std::min(Skin->Base->Playing.Image.ExamProgressBar.Size.Width, Width),
					Skin->Base->Playing.Image.ExamProgressBar.Size.Height },
					1
				);
			}
		}
	}

	void ExamValDraw(_Skin* Skin) {

		for (uint i = 0; i < Chart.ExamDatas.size(); i++) {

			auto ExamData = Chart.OriginalData.ExamDatas[i];
			auto ExamVal = Chart.ExamDatas[i].ExamVals;
			bool IsFall = Chart.ExamDatas[i].IsFall;
			bool IsPass = ExamData.PassVal[0] <= ExamVal && ExamData.Range == ExamRange::More;
			std::string valstr = !IsFall ? std::to_string(ExamVal) : "0";
			std::string examname = ExamList[(int)ExamData.ExamType];

			Skin->Base->Playing.Font.ExamName.Draw(
				{ Skin->Base->Playing.Config.ExamNamePos.X,
				Skin->Base->Playing.Config.ExamNamePos.Y + (120.0f * i) },
				GetColor(255, 255, 255),
				GetColor(0, 0, 0),
				GetStrlen(examname, Skin->Base->Playing.Font.ExamName.Handle),
				examname
			);
			Skin->Base->Playing.Font.ExamVal.Draw(
				{ Skin->Base->Playing.Config.ExamValPos.X,
				Skin->Base->Playing.Config.ExamValPos.Y + (120.0f * i) },
				GetColor(255, 255 * !IsFall, 255 * !IsFall * !IsPass),
				GetColor(0, 0, 0),
				GetStrlen(valstr, Skin->Base->Playing.Font.ExamVal.Handle),
				valstr
			);
		}
	}

	void Action(HitType type);
};