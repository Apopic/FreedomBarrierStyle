#pragma once
#include "Include.hpp"
#include "Cryptgraphy/KeyManager.h"
#include "SongSelect.h"
#include <curl/curl.h>

extern class GameSystem;

class _MultiRoom {
public:

	_MultiRoom(GameSystem* ptr);
	~_MultiRoom();

	void Init() {
		server.Close();
		GrantIndex = 0;
		IsHost = false;
		IsStandby = false;
		IsSelected = false;
		HostSelectMode = false;
		MultiFlag = false;
		InviteFlag = false;
		ConnectAddress = "";
		ConnectPort = 8080;
	}

	TCPSocket server = TCPSocket();

	std::string ConnectAddress = "";
	int ConnectPort = 8080;

	int GrantIndex = 0;
	int HostVal = 4;

	bool MultiFlag = false;
	bool InviteFlag = false;
	bool IsHost = false;
	bool IsStandby = false;
	bool IsSelected = false;
	bool HostSelectMode = false;

	void OptionDraw(FontData font, Pos2D<float> pos, std::string str, bool flg) {

		if (flg && str.find('.') != std::string::npos) { str = str.substr(0, str.length() - 3); }

		font.Draw(
			pos,
			GetColor(255, 255, 255),
			GetColor(0, 0, 0),
			GetDrawStringWidthToHandle(str.c_str(), strlenDx(str.c_str()), font.Handle),
			str);

	}

	void KeyInit() {

		KeyManager key;
		std::string message = "I have skill is write low level programing language.";

		auto q = ECDSA::MakePublicKey(key.GetSecretKey());
		auto v = ECDSA::Sign(key.GetSecretKey(), { message.begin(), message.end() });
		bool ret = ECDSA::Verify(q, v, { message.begin(), message.end() });

		AES128::cbytearray<16> sharedkey{ '1','x','2','3','x','0','2','1','5','4','8','x','2','0','x','1' };
		server.CryptEngine.Init(sharedkey);
	}
};