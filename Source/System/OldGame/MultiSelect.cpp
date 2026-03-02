#include "GameSystem.h"
#include "MultiSelect.h"

_MultiSelect::_MultiSelect(GameSystem *ptr) {

}

_MultiSelect::~_MultiSelect() {

}

void GameSystem::MultiSelectDraw() {

	if (MultiSelect.ModeSelector <= _MultiSelect::Mode::Connecting) {
		for (uint i = 0; i < 2; ++i) {
			DrawString(16, 16 + (16 * i), MultiSelect.ModeStrings[i].c_str(), GetColor(255, 255, 255));
		}
		DrawString(0, 16 + (16 * (int)MultiSelect.ModeSelector), ">", GetColor(255, 255, 255));
	}
	else {
		switch (MultiSelect.ModeSelector)
		{
		case _MultiSelect::Mode::AddressInput:
			DrawString(0, 16, "AddressInput", GetColor(255, 255, 255));
			DrawKeyInputString(0, 32, MultiSelect.InputHandle);
			break;
		case _MultiSelect::Mode::Connect:
			DrawString(0, 16, "Connecting...", GetColor(255, 255, 255));
			break;
		}
	}
}

void GameSystem::MultiSelectProc() {

	if (MultiSelect.ModeSelector <= _MultiSelect::Mode::Connecting) {
		Input.HitKeyProcess(VK_ESCAPE, KeyState::Down, [&] {
			NowScene = Scene::Title;
			});

		auto DonKeyProc = [&] {
			switch (MultiSelect.ModeSelector)
			{
			case _MultiSelect::Mode::ReturnTitle:
				NowScene = Scene::Title;
				break;
			case _MultiSelect::Mode::Connecting:
				MultiSelect.ModeSelector = _MultiSelect::Mode::AddressInput;
				DeleteKeyInput(MultiSelect.InputHandle);
				MultiSelect.InputHandle = MakeKeyInput(32, TRUE, TRUE, FALSE);
				SetActiveKeyInput(MultiSelect.InputHandle);
				break;
			}
			};

		Input.HitKeyesProcess(Config.DonInputLeft, KeyState::Down, DonKeyProc);
		Input.HitKeyesProcess(Config.DonInputRight, KeyState::Down, DonKeyProc);

		Input.HitKeyesProcess(Config.KaInputLeft, KeyState::Down, [&] {
			(int&)MultiSelect.ModeSelector -= 1;
			if (MultiSelect.ModeSelector <= _MultiSelect::Mode::ReturnTitle) { MultiSelect.ModeSelector = _MultiSelect::Mode::ReturnTitle; }
			});
		Input.HitKeyesProcess(Config.KaInputRight, KeyState::Down, [&] {
			(int&)MultiSelect.ModeSelector += 1;
			if (MultiSelect.ModeSelector >= _MultiSelect::Mode::Connecting) { MultiSelect.ModeSelector = _MultiSelect::Mode::Connecting; }
			});
	}

	if (MultiSelect.ModeSelector == _MultiSelect::Mode::AddressInput) {
		switch (CheckKeyInput(MultiSelect.InputHandle))
		{
		case 1:
		{
			char string[32]{};
			GetKeyInputString(string, MultiSelect.InputHandle);
			auto addressport = split(string, ':');
			IPDATA ip;
			
			GetHostIPbyName(addressport[0].c_str(), &ip);

			unsigned short port = addressport.size() == 1 ? 34020 : stoi(addressport[1]);

			MultiSelect.ModeSelector = _MultiSelect::Mode::Connect;
			MultiRoom.Client.Connect(ip, port);
			if (MultiRoom.Client.ConnectFlag) {
				
			}
			else {
				MultiSelect.ModeSelector = _MultiSelect::Mode::Connecting;
				DeleteKeyInput(MultiSelect.InputHandle);
				MultiSelect.InputHandle = MakeKeyInput(32, TRUE, TRUE, FALSE);
				SetActiveKeyInput(MultiSelect.InputHandle);
			}
		}
			break;
		case 2:
			MultiSelect.ModeSelector = _MultiSelect::Mode::Connecting;
			DeleteKeyInput(MultiSelect.InputHandle);
			MultiSelect.InputHandle = MakeKeyInput(32, TRUE, TRUE, FALSE);
			SetActiveKeyInput(MultiSelect.InputHandle);
			break;
		}
	}
}

void GameSystem::MultiSelectInit() {
	
}

void GameSystem::MultiSelectEnd() {
	DeleteKeyInput(MultiSelect.InputHandle);
}


