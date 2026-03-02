#include "Main.h"

static inline std::atomic_bool _waitvsyncLog = false;
static inline std::mutex _syncmtx;
static inline void _LogUpdate(bool *endflag) {
	WaitVSync(1);
	Timer<framecount> timer(true);
	ulonglong mem = 0, now = 0;
	while (true) {
	#ifndef TRUE
		while (now == mem) { now = (ulonglong)timer.GetRecordingTime(); }
		mem = now;
	#else
		WaitVSync(1);
	#endif
		if (*endflag) { break; }
		std::lock_guard<std::mutex> lock(_syncmtx);
		_waitvsyncLog = true;
	}
}
static inline bool GetVSyncWaitLog() {
	std::lock_guard<std::mutex> lock(_syncmtx);
	return _waitvsyncLog.exchange(false);
}

/// <summary>
/// MainProcess
/// </summary>
/// <returns></returns>
int Main::Process() {

	if (!InitSuccessFlag) { return -1; }

	Timer<nanosecond> WaitTimer(true);
	ulonglong waittime = 0;
	bool drawflag = false;

	SetWindowVisibleFlag(TRUE);
	SetWindowZOrder(DX_WIN_ZTYPE_TOP, false);

	std::thread thd([&] { _LogUpdate(&Game.EndFlag); });

	while (1) {
#ifdef _DEBUG
		//clsDx();
#endif
		if (ProcessMessage() == -1) {
			Game.EndFlag = true;
		}
		if (Game.EndFlag) {
			goto PROC_END;
		}
		if (Game.Config.WaitVSync && Game.Config.FastInput) {
			if (GetVSyncWaitLog() && drawflag) {
				WaitTimer.Start();
				waittime = (ulonglong)(nanosecond * Game.Config.FastDrawRate / GetRefreshRate());
				drawflag = false;
			}
			Game.Proc();
			if ((ulonglong)WaitTimer.GetRecordingTime() > waittime) {
				Game.Draw();
				ScreenFlip();
				ClearDrawScreen();
				WaitTimer.End();
				drawflag = true;
			}
		}
		else {
			Game.Proc();
			Game.Draw();
			ScreenFlip();
			ClearDrawScreen();
		}
	}

PROC_END:
	thd.join();

	return 0;
}

/// <summary>
/// Program Entry Point
/// </summary>
/// <param name="hInstance"></param>
/// <param name="hPrevInstance"></param>
/// <param name="lpCmdLine"></param>
/// <param name="nCmdShow"></param>
/// <returns></returns>
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	Main _main;
	int ret = _main.Process();
	return ret;
}

Main::Main() {
#ifndef _DEBUG
	SetOutApplicationLogValidFlag(FALSE);
#endif
	SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
	
	ChangeWindowMode(!Game.Config.FullScreen);
	SetWindowIconID(IDI_ICON1);
	SetMainWindowText("FreedomBarrierStyle");
	SetWaitVSyncFlag(Game.Config.WaitVSync ? !Game.Config.FastInput : false);
	SetAlwaysRunFlag(TRUE);

	SetUseDivGraphFlag(FALSE);

	SetWindowVisibleFlag(FALSE);

	SoundDevice.SetDevice(
		Game.Config.SoundDeviceType,
		Game.Config.Exclusive,
		Game.Config.BufferSize,
		Game.Config.SampleRate
	);

	//SetUseDirect3D11SwapEffect(DX_SWAP_EFFECT_FLIP_DISCARD);

	if (FAILED(DxLib_Init())) { InitSuccessFlag = false; return; }

	Game.Skin.SkinLoad(Game.Config.SkinName);
	Game.SongSelect.EnumChartFile(Game.Config.SongDirectories);

	InitSuccessFlag = true;
}

Main::~Main() {
	DxLib_End();
}
