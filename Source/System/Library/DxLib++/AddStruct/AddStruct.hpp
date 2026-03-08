#pragma once
#include "../Json/json.hpp"
#include <string>
#include "DxLib.h"

using json = nlohmann::json;

template<class T>
struct Pos2D {
	T X = 0;
	T Y = 0;
};
template<class T>
void from_json(const json& data, Pos2D<T>& v);

template<class T>
struct Size2D {
	T Width = 0;
	T Height = 0;
};
template<class T>
void from_json(const json& data, Size2D<T>& v);

template<class T>
struct Rect2D {
	T Top = 0;
	T Bottom = 0;
	T Left = 0;
	T Right = 0;
};
template<class T>
void from_json(const json& data, Rect2D<T>& v);

template<class T>
struct Color3 {
	T R = 0;
	T G = 0;
	T B = 0;
};
template<class T>
void from_json(const json& data, Color3<T>& v);

template<class T>
struct Color4 {
	T R = 0;
	T G = 0;
	T B = 0;
	T A = 0;
};
template<class T>
void from_json(const json& data, Color4<T>& v);


#define JSONPARSE(name) if (data.find(#name) != data.end()) name = data[#name]

struct GraphData {

	Pos2D<float> Pos;
	Size2D<float> Size;
	Pos2D<int> Div{ 1,1 };

	void Load(const std::string& filepath, Pos2D<int> div = {1,1}) {

		int __handle = LoadGraph(filepath.c_str());
		float _w = 0, _h = 0;
		GetGraphSizeF(__handle, &_w, &_h);
		Size = {_w,_h};
		Div = div;
		DeleteGraph(__handle);

		int* __handles = new int[Div.X * Div.Y];
		LoadDivGraphF(
			filepath.c_str(),
			Div.X * Div.Y,
			Div.X,
			Div.Y,
			_w / Div.X,
			_h / Div.Y,
			__handles
		);

		this->Delete();

		for (size_t i = 0, size = static_cast<size_t>(Div.X) * Div.Y; i < size; ++i) {
			Handles.push_back(__handles[i]);
		}

		delete[] __handles;
	}
	void Load(const std::string& dirpath, const json& data) {

		std::string FilePath = "";

		JSONPARSE(FilePath);

		FilePath = (dirpath + FilePath);

		int _w = 0, _h = 0;
		GetImageSize_File(FilePath.c_str(), &_w, &_h);
		Size = {(float)_w, (float)_h};
		
		JSONPARSE(Size);
		JSONPARSE(Pos);
		JSONPARSE(Div);

		int* __handles = new int[Div.X * Div.Y];
		LoadDivGraphF(
			FilePath.c_str(),
			Div.X * Div.Y,
			Div.X,
			Div.Y,
			(float)_w / Div.X,
			(float)_h / Div.Y,
			__handles
		);

		this->Delete();

		for (size_t i = 0, size = static_cast<size_t>(Div.X) * Div.Y; i < size; ++i) {
			Handles.push_back(__handles[i]);
		}

		delete[] __handles;
	}
	void Delete() {
		for (size_t i = 0, size = Handles.size(); i < size; ++i) {
			DeleteGraph(Handles[i]);
		}
		Handles.clear();
	}
	void Draw(Pos2D<float> pos, int index = 0) {
		if (index < 0 || Handles.size() <= index) {
			return;
		}
		DrawExtendGraphF(
			Pos.X + pos.X - (Size.Width * 0.5f),
			Pos.Y + pos.Y - (Size.Height * 0.5f),
			Pos.X + pos.X + (Size.Width * 0.5f),
			Pos.Y + pos.Y + (Size.Height * 0.5f),
			Handles[index],
			TRUE
		);
	}
	void RectDraw(Pos2D<float> pos, Pos2D<float> orgpos, Size2D<float> size, int index = 0) {
		if (index < 0 || Handles.size() <= index) {
			return;
		}
		DrawRectGraphF(
			Pos.X + pos.X - (Size.Width * 0.5f),
			Pos.Y + pos.Y - (Size.Height * 0.5f),
			orgpos.X,
			orgpos.Y,
			size.Width,
			size.Height,
			Handles[index],
			TRUE
		);
	}

	std::vector<int> Handles;
};
struct FontData {

	float Size = 0;
	float Thick = 0;
	float Edge = 0;
	int Space = 0;
	enum Position : int {
		Left,
		Center,
		Right
	} Position = Left;

	void Load(const std::string& filepath, const std::string& fontname, float size, float thick, float edge, int space) {

		MemHandle = AddFontFile(filepath.c_str());

		Size = size;
		Thick = thick * 0.5f;
		Edge = edge;
		Space = space;

		Handle = CreateFontToHandle(fontname.c_str(), Size, Thick, DX_FONTTYPE_ANTIALIASING_EDGE_16X16, DX_CHARSET_UTF8, Edge);
		SetFontSpaceToHandle(Space, Handle);
	}
	void Load(const std::string& dirpath, const json& data) {

		std::string FontName = "";
		std::string FilePath = "";

		JSONPARSE(FilePath);

		FilePath = (dirpath + FilePath);

		MemHandle = AddFontFile(FilePath.c_str());


		JSONPARSE(Size);
		JSONPARSE(Thick) * 0.5f;
		JSONPARSE(Edge);
		if (data.find("Position") != data.end()) ((int&)Position) = data["Position"].get<int>();
		JSONPARSE(Space);

		JSONPARSE(FontName);

		Handle = CreateFontToHandle(FontName.c_str(), Size, Thick, DX_FONTTYPE_ANTIALIASING_EDGE_16X16, DX_CHARSET_UTF8, Edge);
		SetFontSpaceToHandle(Space, Handle);
	}
	void Delete() {
		DeleteFontToHandle(Handle);
		RemoveFontFile(MemHandle);
		Handle = 0;
		MemHandle = nullptr;
	}
	void Draw(Pos2D<float> pos, unsigned int color, unsigned int edgecolor, const std::string& str) {
		if (Handle == -1) { return; }
		if (str.empty()) { return; }
		float offset = 0;
		switch (Position)
		{
		case Left:
			break;
		case Center:
			offset = GetDrawStringWidthToHandle(str.c_str(), -1, Handle) * 0.5f;
			break;
		case Right:
			offset = GetDrawStringWidthToHandle(str.c_str(), -1, Handle);
			break;
		}
		DrawStringFToHandle(
			pos.X - offset,
			pos.Y - Size * 0.5f,
			str.c_str(),
			color,
			Handle,
			edgecolor
		);
	}
	void Draw(Pos2D<float> pos, unsigned int color, unsigned int edgecolor, unsigned int strlen, const std::string& str) {
		if (Handle == -1) { return; }
		if (str.empty()) { return; }
		float offset = 0;
		switch (Position)
		{
		case Left:
			break;
		case Center:
			offset = strlen * 0.5f;
			break;
		case Right:
			offset = strlen;
			break;
		}
		DrawStringFToHandle(
			pos.X - offset,
			pos.Y - Size * 0.5f,
			str.c_str(),
			color,
			Handle,
			edgecolor
		);
	}
	void DrawFontString(Pos2D<float> pos, std::string str, unsigned int color = GetColor(255, 255, 255)) {
		Draw(
			pos,
			color,
			GetColor(0, 0, 0),
			str.length(),
			str
		);
	}

	int Handle = 0;
	HANDLE MemHandle = nullptr;
};

struct SoundData {

	int BufferNum = 0;
	int Frequency = 0;
	int Volume = 0;

	void Load(const std::string filepath, int buffernum = 1) {
		BufferNum = buffernum;
		Handle = LoadSoundMem(filepath.c_str(), BufferNum);

		Frequency = GetFrequencySoundMem(Handle);
		Volume = GetVolumeSoundMem(Handle);
	}
	void Load(const std::string& dirpath, const json& data) {

		std::string FilePath = "";

		JSONPARSE(FilePath);
		JSONPARSE(BufferNum);

		FilePath = (dirpath + FilePath);
		
		Handle = LoadSoundMem(FilePath.c_str(), BufferNum);
		
		Frequency = GetFrequencySoundMem(Handle);
		Volume = GetVolumeSoundMem(Handle);
	}
	void Load(const void* filedata, size_t filesize, int buffernum = 1) {
		BufferNum = buffernum;
		Handle = LoadSoundMemByMemImage(filedata, filesize, BufferNum);

		Frequency = GetFrequencySoundMem(Handle);
		Volume = GetVolumeSoundMem(Handle);
	}
	void Delete() {
		DeleteSoundMem(Handle);
		Handle = 0;
	}
	void Play(bool toppositionflag = true) const {
		PlaySoundMem(Handle, DX_PLAYTYPE_BACK, toppositionflag);
	}
	void Stop() const {
		StopSoundMem(Handle);
	}
	bool IsPlay() const {
		return CheckSoundMem(Handle) == TRUE ? true : false;
	}
	void SetFrequency(int frequency) const {
		SetFrequencySoundMem(frequency, Handle);
	}
	void SetVolume(float volume) const {
		ChangeVolumeSoundMem(((volume) / 100.0) * 255, Handle);
	}
	void SetCurrent(double time) const {
		SetSoundCurrentTime(time, Handle);
	}
	
	int Handle = 0;
};

#undef JSONPARSE

#define FROMJSON(name) data.at(#name).get_to(v.name)
template<class T>
inline void from_json(const json& data, Pos2D<T>& v) {
	FROMJSON(X);
	FROMJSON(Y);
}

template<class T>
inline void from_json(const json& data, Size2D<T>& v) {
	FROMJSON(Width);
	FROMJSON(Height);
}

template<class T>
inline void from_json(const json& data, Rect2D<T>& v) {
	FROMJSON(Top);
	FROMJSON(Bottom);
	FROMJSON(Left);
	FROMJSON(Right);
}

template<class T>
inline void from_json(const json& data, Color3<T>& v) {
	FROMJSON(R);
	FROMJSON(G);
	FROMJSON(B);
}

template<class T>
inline void from_json(const json& data, Color4<T>& v) {
	FROMJSON(R);
	FROMJSON(G);
	FROMJSON(B);
	FROMJSON(A);
}
#undef FROMJSON
