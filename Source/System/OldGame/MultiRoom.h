#pragma once
#include "Include.hpp"

static constexpr inline const char* NetWorkPasscode = "NDtStart";
static constexpr inline const char* ClientVersion = "1.0";

enum class DataType : char {
	Null = -1,
	Check,
	Command,
	Chat
};

enum class CommandType : char {
	Null = -1,
	File,
	Name,
	PlayData,
	Disconnection,
};

struct PlayData {
	double AbsTime = 0;
	char Type = '\0';
	bool LR = false;
};

struct RecvData {
	DataType Type = DataType::Null;
	CommandType Command = CommandType::Null;
	std::string Data;
};

struct ClientData {
	IPDATA IP = { 0,0,0,0 };
	int Handle = -1;
	std::string Name = "";
	bool ConnectFlag = false;
	std::vector<PlayData> PlayDatas;

	void Send(const RecvData& senddata) {
		size_t sendsize = senddata.Data.size() + 2;
		std::string passcode = NetWorkPasscode;
		const char* datasizestr = (const char*)&sendsize;
		std::string data = passcode + std::string(datasizestr, 8) + ((char)senddata.Type + ((char)senddata.Command + senddata.Data));
		std::cout << data << " -> (size: " << data.size() << ")\n";
		NetWorkSend(Handle, data.c_str(), data.size());
	}
	void Send(DataType type, CommandType command, const char* senddata) {
		std::string data = senddata;
		size_t sendsize = strlen(senddata) + 2;
		std::string passcode = NetWorkPasscode;
		const char* datasizestr = (const char*)&sendsize;
		data = passcode + std::string(datasizestr, 8) + ((char)type + ((char)command + data));
		std::cout << data << " -> (size: " << data.size() << ")\n";
		NetWorkSend(Handle, data.c_str(), data.size());
	}
	void Recv() {
		int size = GetNetWorkDataLength(Handle);
		if (size <= 0) {
			return;
		}
		char* buffer = new char[size];
		NetWorkRecv(Handle, buffer, size);
		RecvBuffer += std::string(buffer, size);
		delete[] buffer;
	}
	std::vector<RecvData> GetRecvDatas() {
		std::vector<RecvData> ret;
		size_t start = RecvBuffer.find(NetWorkPasscode);

		if (start == std::string::npos) {
			return ret;
		}
		size_t size = 0;
		while ((size = RecvBuffer.size()) != 0) {

			// BufferConvert
			size_t datasize = *((size_t*)(RecvBuffer.substr(start + 8, 8).c_str()));
			std::string _val = RecvBuffer.substr(start + 16, datasize);
			const char* val = _val.c_str();

			// DataConvert
			RecvData pushdata;
			pushdata.Type = (DataType)(*val);
			pushdata.Command = (CommandType)(*(val + 1));
			_val.substr(2).swap(pushdata.Data);
			ret.push_back(pushdata);

			// DisposeBuffer
			RecvBuffer = RecvBuffer.substr(start + datasize + 16);
		}

		return ret;
	}
	void Connect(const IPDATA& ip, unsigned short Port = 34020) {
		Handle = ConnectNetWork(ip, Port);
		GetNetWorkIP(Handle, &IP);
		ConnectFlag = Handle != -1;
		if (ConnectFlag) {
			Send(DataType::Check, CommandType::Null, ClientVersion);
			Send(DataType::Command, CommandType::Name, Name.c_str());
		}
	}
	void Disconnect() {
		CloseNetWork(Handle);
	}

private:
	std::string RecvBuffer;
};

extern class GameSystem;

class _MultiRoom {
public:

	_MultiRoom(GameSystem* ptr);
	~_MultiRoom();

	enum class Mode {
		Disconnect,



	} ModeSelector;

	ClientData Client;
};
