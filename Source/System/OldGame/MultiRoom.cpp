#include "MultiRoom.h"
#include "GameSystem.h"

_MultiRoom::_MultiRoom(GameSystem *ptr) {

}

_MultiRoom::~_MultiRoom() {

}

void GameSystem::MultiRoomInit() {
	MultiRoom.Client.Name = Config.MultiPlayName;

	SongSelect.MultiFlag = true;
	Playing.MultiFlag = true;
}

void GameSystem::MultiRoomEnd() {


	Playing.MultiFlag = false;
}
void GameSystem::MultiRoomDraw() {

}

void GameSystem::MultiRoomProc() {
	MultiRoom.Client.Recv();

	auto RecvDatas = MultiRoom.Client.GetRecvDatas();

	for (const auto& Data : RecvDatas) {
		switch (Data.Type)
		{
		case DataType::Command:
			switch (Data.Command)
			{
			case CommandType::File:
				break;
			case CommandType::PlayData:
				break;
			case CommandType::Disconnection:
				MultiRoom.Client.Disconnect();
				NowScene = Scene::MultiSelect;
				MultiSelect.ModeSelector = _MultiSelect::Mode::ReturnTitle;
				break;
			}
			break;
		case DataType::Chat:
			break;
		}
	}
}
