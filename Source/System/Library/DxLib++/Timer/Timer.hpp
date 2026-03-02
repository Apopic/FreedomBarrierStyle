#pragma once
//#include <chrono>
#include "DxLib.h"

enum TimerType {
	second =      1,
	millisecond = 1'000,
	microsecond = 1'000'000,
	nanosecond =  1'000'000'000,
	framecount = 0x7fffffff
};

template<TimerType t_type = nanosecond>
class Timer{
	ULONGLONG m_memtp = 0;
	bool m_NowRecording = false;
	inline static double RefreshRate = GetRefreshRate();

public:

	Timer(bool startflag = true) {
		if (startflag) {
			Start();
		}
	}

	void Start() {
		m_memtp = _nowcount();
		m_NowRecording = true;
		if constexpr (t_type == framecount) {
			RefreshRate = ::GetRefreshRate();
		}
	}

	void End() {
		m_NowRecording = false;
	}

	void Delay(long long nanosec) {
		m_memtp += ConvNanoSecondsToSysPerformanceCount(nanosec);
	}

	inline bool GetNowRecording() const { return m_NowRecording; }

	double GetNowTime(TimerType type = t_type) const {
		ULONGLONG val = syscount_cast(TimerType::nanosecond, _nowcount() - m_memtp);
		switch (type)
		{
		case second:
			return val * 0.000000001;
			break;
		case millisecond:
			return val * 0.000001;
			break;
		case microsecond:
			return val * 0.001;
			break;
		case nanosecond:
			return val * 1;
			break;
		case framecount:
			return (ULONGLONG)(GetNowTime(second) * GetRefreshRate());
			break;
		}
		return val;
	}

	double GetRecordingTime(TimerType type = t_type) const {
		if (!m_NowRecording) { return 0; }
		ULONGLONG val = syscount_cast(TimerType::nanosecond, _nowcount() - m_memtp);
		switch (type) {
			case second:
				return val * 0.000000001;
				break;
			case millisecond:
				return val * 0.000001;
				break;
			case microsecond:
				return val * 0.001;
				break;
			case nanosecond:
				return val * 1;
				break;
			case framecount:
				return (ULONGLONG)(GetNowTime(second) * RefreshRate);
				break;
		}
		return val;
	}

	double GetRefreshRate() const {
		return RefreshRate;
	}

private:

	ULONGLONG _nowcount() const {
		return GetNowSysPerformanceCount();
	}

	ULONGLONG syscount_cast(TimerType _to, ULONGLONG syscount) const {
		switch (_to) {
			case TimerType::second:
				return ConvSysPerformanceCountToSeconds(syscount);
				break;
			case TimerType::millisecond:
				return ConvSysPerformanceCountToMilliSeconds(syscount);
				break;
			case TimerType::microsecond:
				return ConvSysPerformanceCountToMicroSeconds(syscount);
				break;
			case TimerType::nanosecond:
				return ConvSysPerformanceCountToNanoSeconds(syscount);
				break;
		}
		return ConvSysPerformanceCountToNanoSeconds(syscount);
	}
};
