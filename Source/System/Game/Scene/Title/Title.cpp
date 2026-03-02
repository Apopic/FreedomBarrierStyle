#include "Title.hpp"
#include "../SongSelect/SongSelect.hpp"
#include "Structs/ModeSelectBoard.hpp"

class _ScenemainTitle {
public:

	enum Mode : int {
		Mode_Start = -1,
		Mode_Single,
		Mode_Multi,
		Mode_Dan,
		Mode_Config,
		Mode_End,
		Mode_Count
	} ModeSelector = Mode_Start;

	bool SceneChangeFlag = false;

	void DonInput() {
		switch (ModeSelector)
		{
		case _ScenemainTitle::Mode_Start:
			ModeSelector = Mode_Single;
			break;
		case _ScenemainTitle::Mode_Single:
		case _ScenemainTitle::Mode_Multi:
		case _ScenemainTitle::Mode_Dan:
		case _ScenemainTitle::Mode_Config:
		case _ScenemainTitle::Mode_End:
			SceneChangeFlag = true;
			break;
		}
	}

	void KaInput(bool Left) {
		if (Left) {
			(int&)ModeSelector -= 1;
			if (ModeSelector <= Mode_Start) {
				ModeSelector = Mode_Single;
			}
		}
		else {
			(int&)ModeSelector += 1;
			if (ModeSelector >= Mode_Count) {
				ModeSelector = Mode_End;
			}
		}
	}

	void SceneChange() {
		Game.Transition.Start(0.25, ease::In, ease::Sine);
		switch (ModeSelector)
		{
		case _ScenemainTitle::Mode_Single:
			Game.Transition.NextScenePtr = new SongSelect;
			break;
		case _ScenemainTitle::Mode_Multi:
			Game.Transition.End();
			MessageBox(NULL, "Not Created", "Not Created", MB_OK);
			break;
		case _ScenemainTitle::Mode_Dan:
			Game.Transition.End();
			MessageBox(NULL, "Not Created", "Not Created", MB_OK);
			break;
		case _ScenemainTitle::Mode_Config:
			Game.Transition.End();
			MessageBox(NULL, "Not Created", "Not Created", MB_OK);
			break;
		case _ScenemainTitle::Mode_End:
			Game.Transition.End();
			Game.EndFlag = true;
			break;
		}
		SceneChangeFlag = false;
	}

	void Init() {
		Skin.ContentManager.AddContent(new ModeSelectBoard);
		Game.Transition.Start(0.5, ease::Out, ease::Sine);
	}

	void Draw() {
		for (auto&& content : Skin.Base->Scene.Contents) {
			switch_h (content->GetType())
			{
			default:
				content->Draw();
				break;
			}
		}
	}

	void Proc() {
		if (ModeSelector != Mode_Start) {
			Input.HitKeyesProcess(Config.KaInputLeft, KeyState::Down, [&] {
				KaInput(true);
				});
			Input.HitKeyesProcess(Config.KaInputRight, KeyState::Down, [&] {
				KaInput(false);
				});
		}

		Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, [&] {
			DonInput();
			});
		Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, [&] {
			DonInput();
			});

		if (SceneChangeFlag) {
			SceneChange();
		}

		Input.HitKeyProcess(KEY_INPUT_ESCAPE, KeyState::Down, [&] {
			Game.EndFlag = true;
			});
	}
	void End() {

	}
};

Title::Title() {
	Name = __func__;
	_ptr = new _ScenemainTitle;
}

Title::~Title() {
	delete _ptr;
	_ptr = nullptr;
}

void Title::Init() {
	_ptr->Init();
}
void Title::Draw() {
	_ptr->Draw();
}
void Title::Proc() {
	_ptr->Proc();
}
void Title::End() {
	_ptr->End();
}