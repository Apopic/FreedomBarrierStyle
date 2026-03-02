#pragma once
#include "Include.hpp"
#include "SongSelect.h"

enum class ScrollType{
	Normal,
	BMSCROLL,
	HBSCROLL,
};

struct TJANoteData {

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

	bool GoGoStart = true;
	bool GoGoEnd = false;

	char NoteType = '\0';
	double RollEndTime = 0;
	size_t RollEndIndex = 0;
	unsigned short RollFlag = 0;
	unsigned short BalloonFlag = 0;
	int BalloonCount = 0;
	bool BarlineDisplay = false;

	bool HitFlag = false;

};

struct JudgeData {
	ulonglong Score = 0;
	ulonglong Good = 0;
	ulonglong Ok = 0;
	ulonglong Bad = 0;
	ulonglong Roll = 0;
	ulonglong Combo = 0;
	ulonglong MaxCombo = 0;
	ulonglong HitNote = 0;
	double Accurecy = 0;
};

struct HitNoteData {

	enum JudgeType {
		Null = -1,
		Good,
		Ok,
		Bad,
		Roll
	};

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
	
		JudgeUpperExplosion.Type = judgetype;
		JudgeUpperExplosion.Big = type == '3' || type == '4';

		switch (judgetype)
		{
		case HitNoteData::Bad:
			break;
		case HitNoteData::Roll:
			FlyingNote.IsActive = true;
			break;
		default:
			JudgeUpperExplosion.IsActive = true;
			JudgeUnderExplosion.IsActive = true;
			FlyingNote.IsActive = true;
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

	struct JudgeUnderExplosion {
		bool IsActive = false;
		bool Big = false;
		JudgeType Type = Null;
	} JudgeUnderExplosion;
	
	struct JudgeUpperExplosion {
		bool IsActive = false;
		bool Big = false;
		JudgeType Type = Null;
	} JudgeUpperExplosion;
};

struct ChartStreamData {

	void Init();

	/// <summary>
	/// MainNotes!!!!!!!!!!!!!!
	/// </summary>
	std::vector<TJANoteData> RawNoteDatas = std::vector<TJANoteData>();
	ChunkVector<TJANoteData> ProcNotes;
	ChunkVector<TJANoteData> DrawNotes;

	SoundData SongData;

	ChartData OriginalData;
	int AddScore = 0;
	double SongBlankTime = 0;

	bool NowGoGO = false;
	double NowBPM = 0;
	Timer<nanosecond> NowTime;
	Timer<framecount> FrameNowTime;

	double SongSpeed = 1;

	ScrollType ScrollType = ScrollType::Normal;

	bool AutoPlayLR = false;
	Timer<microsecond> WaitRollTime;

	JudgeData Judge;
	JudgeData JudgeBranch;




};

extern class GameSystem;

class _Playing {
public:

	_Playing(GameSystem* ptr);
	~_Playing();

	ChartStreamData Chart;

	bool MultiFlag = false;
	bool DanFlag = false;

	Timer<millisecond> MiniTaikoFlash[4];
	double MiniTaikoFlashTime = 160;

	double ChartNowTime(bool FrameCounter = false, double fastdrawrate = 0) const {
		double ret = 0;
		if (FrameCounter) {
			double hz = Chart.FrameNowTime.GetRefreshRate();
			double frame = ((Chart.FrameNowTime.GetRecordingTime() + fastdrawrate) / hz);
			//if (std::round((Chart.FrameNowTime.GetRecordingTime(second) - frame) * hz) == 0) {
			//	ret = millisecond * frame;
			//}
			//else {
			//	ret = (millisecond * (frame + 1 / hz));
			//}
			ret = millisecond * frame;
		}
		else {
			ret = Chart.NowTime.GetRecordingTime() / microsecond;
		}
		return ret * Chart.SongSpeed;
	};

	std::string ActivityDispJudges() const {
		return (
			"Accurecy: " + std::to_string(Chart.Judge.Accurecy) +
			", Score: " + std::to_string(Chart.Judge.Score) +
			", Good: " + std::to_string(Chart.Judge.Good) +
			", Ok: " + std::to_string(Chart.Judge.Ok) +
			", Bad: " + std::to_string(Chart.Judge.Bad) +
			", Roll: " + std::to_string(Chart.Judge.Roll) +
			", Combo: " + std::to_string(Chart.Judge.Combo)
			);
	}

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
	} HitNote;

	void JudgeNote(double nowtime, char type);
	void ComboDraw();

};

