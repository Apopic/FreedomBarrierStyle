#pragma once
#include "Include.hpp"
#include "Config.hpp"
#include "Contents.hpp"

extern void SkinRestoreCallBack();

class Skin {

	Skin() { Init(); };
	~Skin() { End(); };

public:

	static Skin& GetInstance() {
		static Skin instance;
		return instance;
	}

	void Init() {
		ContentManager.AddContent(new Content::Graph);
		ContentManager.AddContent(new Content::MoveGraph);
		ContentManager.AddContent(new Content::String);

		for (const auto& f : fs::recursive_directory_iterator("Skins")) {
			if (f.path().extension() == ".json") {
				std::ifstream ifs(f.path());

				json data = json::parse(ifs);

				if (data.find("Info") == data.end()) {
					continue;
				}

				SkinInfos.push_back(
					SkinInfo{
						f.path().u8string(),
						data["Info"].at("Name"),
						data["Info"].at("Version"),
						data["Info"].at("Resolution"),
						data["Info"].at("ColorBit")
					}
				);

				ifs.close();
			}
		}
		SetRestoreGraphCallback(&SkinRestoreCallBack);
	}

	void End() {
		this->Delete();
	}

	void Read() {

		for (uint i = 0, size = SkinInfos.size(); i < size; ++i) {
			if (Config.SkinName != SkinInfos[i].Name) {
				continue;
			}
			UseSkinInfo = SkinInfos[i];
			break;
		}

		std::ifstream ifs(UseSkinInfo.Path);
		SkinRawData = json::parse(ifs);
		ifs.close();

		SetGraphMode(
			UseSkinInfo.Resolution.Width,
			UseSkinInfo.Resolution.Height,
			UseSkinInfo.ColorBit,
			Config.RefreshRate
		);
		SetDrawMode(DX_DRAWMODE_BILINEAR);
	}

	void SceneLoad(const std::string& SceneName) {
		this->Delete();

		Base = new _Base;

		const json& SceneData = SkinRawData["Scene"][SceneName]["Contents"];

		try {
			for (const auto& PolyData : SceneData) {
				std::string KeyName = PolyData.begin().key();
				ContentBase* ptr = ContentManager.GetContentBase(KeyName)->Clone();
				ptr->Create(std::filesystem::path(UseSkinInfo.Path).parent_path().u8string() + "\\", PolyData[KeyName]);
				std::unique_ptr<ContentBase> retptr(ptr);
				Base->Scene.Contents.push_back(std::move(retptr));
			}
		}
		catch (std::exception& e) {
			SendException(e.what());
		}

		SetDrawScreen(DX_SCREEN_BACK);

		for (const auto& data : Base->Scene.Contents) {
			data->Init();
		}
	}

	void Delete() {
		delete Base;
	}

	struct SkinInfo {
		std::string Path;
		std::string Name;
		std::string Version;
		Size2D<int> Resolution{0,0};
		int ColorBit = 0;
	};
	std::vector<SkinInfo> SkinInfos;
	SkinInfo UseSkinInfo;
	json SkinRawData;

	ContentManager ContentManager;

	struct _Base {
		SkinInfo Info;
		struct {
		std::unordered_map<std::string, std::unique_ptr<SoundData>, string_hash, std::equal_to<>> BGM;
		std::unordered_map<std::string, std::unique_ptr<SoundData>, string_hash, std::equal_to<>> SE;
		std::deque<std::unique_ptr<ContentBase>> Contents;
		} Scene;
	}* Base = nullptr;
};

#define Skin Skin::GetInstance()