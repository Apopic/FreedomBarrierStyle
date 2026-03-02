#include "Playing.hpp"

class _ScenemainPlaying {
public:

	void Init() {
	
	}
	void Draw() {
	
	}
	void Proc() {
	
	}
	void End() {
	
	}
};

Playing::Playing() {
	Name = __func__;
	_ptr = new _ScenemainPlaying;
}

Playing::~Playing() {
	delete _ptr;
	_ptr = nullptr;
}

void Playing::Init() {
	_ptr->Init();
}
void Playing::Draw() {
	_ptr->Draw();
}
void Playing::Proc() {
	_ptr->Proc();
}
void Playing::End() {
	_ptr->End();
}
