#pragma once
#include "DxLib.h"

class SoundDevice final {
public:

	SoundDevice() {};
	~SoundDevice() = default;

	SoundDevice(const SoundDevice &) = delete;
	SoundDevice &operator=(const SoundDevice &) = delete;
	SoundDevice(SoundDevice &&) = delete;
	SoundDevice &operator=(SoundDevice &&) = delete;

	static SoundDevice &GetInstance() {
		static SoundDevice instance;
		return instance;
	}

	enum SoundDeviceType : int {
		None = 0,
		WASAPI,
		XAudio,
		MMEwaveOut,
		ASIO
	};

	void SetDevice(int type, bool exclusive, int bufsize, int samplerate) {
		
		int ns = (size_t)10000000 * bufsize / samplerate;
		switch (type) {
			case SoundDeviceType::WASAPI:
				SetEnableWASAPIFlag(TRUE, exclusive, ns, samplerate);
				break;
			case SoundDeviceType::XAudio:
				SetEnableXAudioFlag(TRUE);
				break;
			case SoundDeviceType::MMEwaveOut:
				SetEnableMMEwaveOutFlag(TRUE, bufsize, samplerate);
				break;
			case SoundDeviceType::ASIO:
				SetEnableASIOFlag(TRUE, bufsize, samplerate);
				break;
			default:
				SetDevice(SoundDeviceType::WASAPI, exclusive, bufsize, samplerate);
				break;
		}
	}
};

#define SoundDevice SoundDevice::GetInstance()