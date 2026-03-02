#include "Result.hpp"

class _ScenemainResult {
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

Result::Result() {
	Name = __func__;
	_ptr = new _ScenemainResult;
}

Result::~Result() {
	delete _ptr;
	_ptr = nullptr;
}

void Result::Init() {
	_ptr->Init();
}
void Result::Draw() {
	_ptr->Draw();
}
void Result::Proc() {
	_ptr->Proc();
}
void Result::End() {
	_ptr->End();
}
