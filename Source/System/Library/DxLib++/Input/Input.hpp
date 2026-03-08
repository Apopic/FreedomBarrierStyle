#pragma once
#include "DxLib.h"
#include <vector>

inline byte __gotkeyboardinput[256]{};
inline LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_KEYDOWN:
			__gotkeyboardinput[wParam] = 1;
			break;
		case WM_KEYUP:
			__gotkeyboardinput[wParam] = 0;
			break;
	}

	return 0;
}

enum class KeyState {
	Null,
	Down,
	Hold,
	Up,
	Release
};

class Input {

	byte *__KeyBoardInput = __gotkeyboardinput;
	KeyState __KeyBoardState[256]{};

	Input() {};
	~Input() {};

	bool InputLock = false;

public:

	static Input& GetInstance() {
		static Input Instance;
		SetHookWinProc(WndProc);
		return Instance;
	}
	
	//int Update() {
	//	//return GetKeyboardState(__KeyBoardInput);
	//	//return GetHitKeyStateAll(__KeyBoardInput);
	//}

	inline bool GetInputLock() const { return InputLock; }
	inline void SetInputLock(bool&& val) { InputLock = val; }

	template<typename Func>
	void HitKeyProcess(int keycode, KeyState state, const Func& func) {
		static const int ispress = 0b00000001;
		if (InputLock) {
			return;
		}
		
		if (__KeyBoardInput[keycode] & ispress) {
			switch (__KeyBoardState[keycode]) {
			case KeyState::Down:
				__KeyBoardState[keycode] = KeyState::Hold;
				break;
			case KeyState::Release:
				__KeyBoardState[keycode] = KeyState::Down;
				break;
			case KeyState::Null:
				__KeyBoardState[keycode] = KeyState::Down;
				break;
			case KeyState::Up:
				__KeyBoardState[keycode] = KeyState::Down;
				break;
			}
		}
		else {
			switch (__KeyBoardState[keycode]) {
			case KeyState::Down:
				__KeyBoardState[keycode] = KeyState::Up;
				break;
			case KeyState::Hold:
				__KeyBoardState[keycode] = KeyState::Up;
				break;
			case KeyState::Up:
				__KeyBoardState[keycode] = KeyState::Release;
				break;
			}
		}

		if (__KeyBoardState[keycode] != state) {
			return;
		}

		func();
	}
	
	template<typename Func>
	void HitKeyesProcess(const std::vector<int>& keycodearray, KeyState state, const Func& func) {
		if (InputLock) {
			return;
		}
		
		for (auto keycode : keycodearray) {
			if (__KeyBoardInput[keycode] == 1) {
				switch (__KeyBoardState[keycode]) {
				case KeyState::Null:
					__KeyBoardState[keycode] = KeyState::Down;
					break;
				case KeyState::Up:
					__KeyBoardState[keycode] = KeyState::Down;
					break;
				case KeyState::Release:
					__KeyBoardState[keycode] = KeyState::Down;
					break;
				case KeyState::Down:
					__KeyBoardState[keycode] = KeyState::Hold;
					break;
				}
			}
			else {
				switch (__KeyBoardState[keycode]) {
				case KeyState::Down:
					__KeyBoardState[keycode] = KeyState::Up;
					break;
				case KeyState::Hold:
					__KeyBoardState[keycode] = KeyState::Up;
					break;
				case KeyState::Up:
					__KeyBoardState[keycode] = KeyState::Release;
					break;
				}
			}

			if (__KeyBoardState[keycode] != state) {
				continue;
			}

			func();
		}
	}

	void __HitMouseButtonProcess() {
		if (InputLock) {
			return;
		}

		int type = 0, x = 0, y = 0, logtype = 0;
		while (GetMouseInputLog2(&type, &x, &y, &logtype) != -1) {
			// TODO: 
		}
	}

	void __HitScreenTouchProcess() {
		if (InputLock) {
			return;
		}

		int touchpointnum = GetTouchInputNum();
		for (unsigned int i = 0; i < touchpointnum; ++i) {
			// TODO:
		}
	}

	int GetMouseWheel() { return GetMouseWheelRotVol(); }
};

#define Input Input::GetInstance()
