#define DISCORDPP_IMPLEMENTATION
#include "discordpp.h"
#include "GameSystem.h"
#include <curl/curl.h>

#define SCENEBRANCH(name, type) \
case Scene::name:\
name##type();\
break;

#define INSWITCH(type)\
SCENEBRANCH(Title, type)\
SCENEBRANCH(SongSelect, type)\
SCENEBRANCH(DanSelect, type)\
SCENEBRANCH(Loading, type)\
SCENEBRANCH(Playing, type)\
SCENEBRANCH(Result, type)\
\
SCENEBRANCH(MultiRoom, type)\
\
SCENEBRANCH(Config, type)\
SCENEBRANCH(Skin, type)\

void GameSystem::Draw() {

	BackFPS = (BackFPS + (1 / FPSTimer.GetRecordingTime())) / 2;
	FPSTimer.Start();

	if (MemScene != NowScene) {

		FadeTimer.Start();

		switch (MemScene) {
			INSWITCH(End);
		}

		MemScene = NowScene;

		switch (NowScene) {
			INSWITCH(Init);
		}
	}

	DrawTimer.Start();
	switch (NowScene) {
		INSWITCH(Draw);
	}
	BackDrawTime = (BackDrawTime + DrawTimer.GetRecordingTime()) / 2;

	if (Config.ViewDebugData) {

		if (AverageTimer.GetRecordingTime() > 0.5) {
			AverageTimer.Start();
			NowFPS = BackFPS;
			NowDrawTime = BackDrawTime;
		}

		DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS:%d\nTPS:%d\nDrawTime:%.12lf", NowFPS, NowTPS, NowDrawTime);
	}

	if (FadeTimer.GetNowRecording()) {
		ScreenFade();
	}
}

void GameSystem::Proc() {

	NowTPS = (NowTPS + (1 / TPSTimer.GetRecordingTime())) / 2;
	TPSTimer.Start();

	switch (NowScene) {
		INSWITCH(Proc);
	}

	if (MultiRoom.MultiFlag) {

		Recv();

		if (!Private.PlayerDatas.empty()) {
			MultiRoom.IsStandby = Private.PlayerDatas[Private.MyIndex].Standby % MultiRoom.HostVal;
			MultiRoom.IsHost = Private.PlayerDatas[Private.MyIndex].Standby >= MultiRoom.HostVal;
		}
	}

	Input.HitKeyProcess(VK_CONTROL, KeyState::Hold, [&] {
		Input.HitKeyProcess(VK_F2, KeyState::Down, [&] {
			SongSelect.EnumChartFile(Config.SongDirectories);
			});
		Input.HitKeyProcess(VK_F3, KeyState::Down, [&] {
			Config.ConfigLoad();
			});
		Input.HitKeyProcess(VK_F4, KeyState::Down, [&] {
			Skin.SkinLoad(Config.SkinName);
			});
		});

	if (Config.DiscordSDK) {
		discordpp::RunCallbacks();
	}
}

auto client = std::make_shared<discordpp::Client>();
const uint64_t APPLICATION_ID = 1443583770430144534;
discordpp::Activity activity;

GameSystem::GameSystem() {

	if (Config.DiscordSDK) {

		static auto TokenDecrypt = [&] {

			std::string GetToken = "";
			std::ifstream ifs("secure_token.dat", std::ios::binary);

			if (ifs.is_open()) {

				std::istreambuf_iterator<char> beg(ifs), end;
				auto infile = Cryptgraphy::bytearray(beg, end);
				AES128::bytearray ret = encoder.SizeAlloc(infile.size());
				encoder.CTRDecrypt(infile, ret, ret.size());

				for (int i = 16; i < ret.size(); i++) {
					GetToken += *Packet(ret[i]).Get<char>();
				}
			}

			ifs.close();
			return GetToken;
			};

		static auto TokenEncrypt = [&](AES128::byte_view Token) {

			AES128::bytearray ret = encoder.SizeAlloc(Token.size());
			encoder.CTREncrypt(Token, ret, ret.size());

			std::ofstream ofs("secure_token.dat", std::ios::binary);
			for (auto r : ret) { ofs << r; }
			ofs.close();
			};

		client->AddLogCallback([](auto message, auto severity) {
			std::cout << "[" << EnumToString(severity) << "] " << message << std::endl;
			}, discordpp::LoggingSeverity::Info);

		client->SetStatusChangedCallback([](discordpp::Client::Status status, discordpp::Client::Error error, int32_t errorDetail) {
			std::cout << "🔄 Status changed: " << discordpp::Client::StatusToString(status) << std::endl;

			if (status == discordpp::Client::Status::Ready) {
				std::cout << "✅ Client is ready! You can now call SDK functions.\n";

				activity.SetType(discordpp::ActivityTypes::Playing);
				activity.SetDetails("FreedomBarrierStyle");
				activity.SetState("Title");

				client->UpdateRichPresence(activity, [](discordpp::ClientResult result) {
					if (result.Successful()) {
						std::cout << "🎮 Rich Presence updated successfully!\n";
					}
					else {
						std::cerr << "❌ Rich Presence update failed";
					}
					});
			}
			else if (error != discordpp::Client::Error::None) {
				std::cerr << "❌ Connection Error: " << discordpp::Client::ErrorToString(error) << " - Details: " << errorDetail << std::endl;
			}
			});

		encoder.Init(key);
		auto codeVerifier = client->CreateAuthorizationCodeVerifier();

		discordpp::AuthorizationArgs args{};
		args.SetClientId(APPLICATION_ID);
		args.SetScopes(discordpp::Client::GetDefaultPresenceScopes());
		args.SetCodeChallenge(codeVerifier.Challenge());

		client->RefreshToken(
			APPLICATION_ID, TokenDecrypt(),
			[&, args, codeVerifier](discordpp::ClientResult result, std::string accessToken,
				std::string refreshToken,
				discordpp::AuthorizationTokenType tokenType, int32_t expiresIn,
				std::string scope) {
					if (result.Successful()) {

						TokenEncrypt(Packet(refreshToken).GetBuffer());

						client->UpdateToken(discordpp::AuthorizationTokenType::Bearer, accessToken, [](discordpp::ClientResult result) {
							if (result.Successful()) {
								std::cout << "🔑 Token updated, connecting to Discord...\n";
								client->Connect();
							}
							});
					}
					else {
						std::cout << "❌ Error refreshing token: " << result.Error() << std::endl;

						client->Authorize(args, [&, codeVerifier](auto result, auto code, auto redirectUri) {
							if (!result.Successful()) {
								std::cerr << "❌ Authentication Error: " << result.Error() << std::endl;
								return;
							}
							else {
								std::cout << "✅ Authorization successful! Getting access token...\n";
								client->GetToken(APPLICATION_ID, code, codeVerifier.Verifier(), redirectUri,
									[&](discordpp::ClientResult result,
										std::string accessToken,
										std::string refreshToken,
										discordpp::AuthorizationTokenType tokenType,
										int32_t expiresIn,
										std::string scope) {

											std::cout << "🔓 Access token received! Establishing connection...\n";

											TokenEncrypt(Packet(refreshToken).GetBuffer());

											client->UpdateToken(discordpp::AuthorizationTokenType::Bearer, accessToken, [](discordpp::ClientResult result) {
												if (result.Successful()) {
													std::cout << "🔑 Token updated, connecting to Discord...\n";
													client->Connect();
													client->RegisterLaunchCommand(APPLICATION_ID, GetExePath());
												}
												});
									});
							}
							});
					}
			});
	}
}

GameSystem::~GameSystem() {

}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return (size_t)(size * nmemb);
}

std::string GetIpAddress(std::string address) {

	CURL* curl;
	CURLcode res;
	char* ip = nullptr;
	std::string readBuffer;

	if (address == "localhost") {
		curl = curl_easy_init();
		if (curl) {
			curl_easy_setopt(curl, CURLOPT_URL, "https://api.ipify.org");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);

			if (res == CURLE_OK) {
				return readBuffer;
			}
			else {
				std::cerr << "エラー: " << curl_easy_strerror(res) << std::endl;
			}
		}
	}

	return address;
}

void GameSystem::SetState(std::string state) {

	activity.SetType(discordpp::ActivityTypes::Playing);
	activity.SetDetails("FreedomBarrierStyle");
	activity.SetState(state);

	if (MultiRoom.MultiFlag) {

		discordpp::ActivityParty party;
		party.SetId("FBSMulti");
		party.SetCurrentSize(Private.CountAll);
		party.SetMaxSize(4);
		activity.SetParty(party);

		if (MultiRoom.IsHost && !MultiRoom.InviteFlag) {

			discordpp::ActivitySecrets secrets;
			secrets.SetJoin(GetIpAddress(Config.ServerAddress) + "|" + std::to_string(Config.ServerPort));
			activity.SetSecrets(secrets);
			activity.SetSupportedPlatforms(discordpp::ActivityGamePlatforms::Desktop);
			activity.SetSupportedPlatforms(discordpp::ActivityGamePlatforms::Android);
			MultiRoom.InviteFlag = true;
		}
	}

	if (!MultiRoom.IsHost) {
		client->SetActivityJoinCallback([&](std::string joinSecret) {
			auto sp = split(joinSecret, '|');
			MultiRoom.ConnectAddress = sp[0];
			MultiRoom.ConnectPort = stoi(sp[1]);
			MultiRoom.InviteFlag = true;
			NowScene = Scene::MultiRoom;
			});
	}

	client->UpdateRichPresence(activity, [](discordpp::ClientResult result) {
		if (result.Successful()) {
			std::cout << "🎮 Rich Presence updated successfully!\n";
		}
		else {
			std::cerr << "❌ Rich Presence update failed";
		}
		});
}