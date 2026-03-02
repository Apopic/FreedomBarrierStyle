#include "SongSelect.hpp"
#include "../Title/Title.hpp"

class _ScenemainSongSelect {
public:

	void Init() {
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
		Input.HitKeyesProcess(Config.KaInputLeft, KeyState::Down, [&] {
			;
			});
		Input.HitKeyesProcess(Config.KaInputRight, KeyState::Down, [&] {

			});
		Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, [&] {

			});
		Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, [&] {

			});

		Input.HitKeyProcess(KEY_INPUT_ESCAPE, KeyState::Down, [&] {
			Game.Transition.Start(0.125, ease::In, ease::Sine, new Title);
			});
	}
	void End() {
		
	}




};

SongSelect::SongSelect() {
	Name = __func__;
	_ptr = new _ScenemainSongSelect;
}

SongSelect::~SongSelect() {
	delete _ptr;
	_ptr = nullptr;
}

void SongSelect::Init() {
	_ptr->Init();
}
void SongSelect::Draw() {
	_ptr->Draw();
}
void SongSelect::Proc() {
	_ptr->Proc();
}
void SongSelect::End() {
	_ptr->End();
}
