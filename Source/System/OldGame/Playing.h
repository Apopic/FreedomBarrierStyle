#pragma once
#include "Include.hpp"
#include "SongSelect.h"

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

enum class ScrollType{
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

struct BranchData {

	double AbsTime = 0;
	double StartTime = 0;
	double Border[3] = { 0,0,0 };
	bool Start = false;
	BranchType Type = BranchType::Null;

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
	
	BranchLevel BranchLevel = BranchLevel::None;
	bool BranchStart = false;
	bool Section = false;
	bool LevelHold = false;

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
		Packet::StoreBytes(ret, BranchLevel);
		Packet::StoreBytes(ret, BranchStart);
		Packet::StoreBytes(ret, Section);
		Packet::StoreBytes(ret, LevelHold);
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
		Packet::LoadBytes(view, BranchLevel);
		Packet::LoadBytes(view, BranchStart);
		Packet::LoadBytes(view, Section);
		Packet::LoadBytes(view, LevelHold);
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
		BranchDatas.clear();
		SongData.Delete();
		FrameNowTime.End();
		NowTime.End();
		ScrollType = ScrollType::Normal;
		OriginalData = ChartData();
		BranchJudge = JudgeData();
		IsBranchedChart = false;
		LevelHold = false;
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
	std::vector<BranchData> BranchDatas = std::vector<BranchData>();

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
	JudgeData BranchJudge;

	bool IsBranchedChart = false;
	bool LevelHold = false;

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

	Timer<millisecond> BranchLaneMove;
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

		void Add(HitNoteData &&data) {

			Datas[Index] = std::move(data);

			++Index;

			if (!(Index < m_size)) {
				Index = 0;
			}
		}
	} HitNote[4];

	void NoteAlpha(int &Alpha, double _one, AlphaType Type) {

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

	void JudgeNote(double nowtime, char type);

	void BranchLaneClip(bool isclip, Pos2D<float> pos);
	void BranchLaneDraw(int index, Pos2D<float> pos);

	void TitleDraw(std::string str, int strlen);
	void SubTitleDraw(std::string str, int strlen);

	void ComboDraw(int index, Pos2D<float> pos);
	void ScoreDraw(int index, Pos2D<float> pos);
	void BalloonDraw(int val, Pos2D<float> pos);
	void NameDraw(std::string name, Pos2D<float> pos);
	void ProgressBarDraw(int index, Pos2D<float> pos);
	void ExamProgressBarDraw();
	void ExamValDraw();
	void Action(HitType type);

	double ScoreRateCalc(double judge, double basis) {
		const double c = 0.9;
		const double b = basis;
		const double m = 10;
		const double d = std::pow(b, std::pow(m, -1 / c));
		return 1 / std::pow(std::log(judge * (b - d) / b + d) / std::log(b), c);
	}

	double SongSpeedRateCalc(double speed) {
		return (speed - 1.0) * 0.2 + 1.0;
	}

};