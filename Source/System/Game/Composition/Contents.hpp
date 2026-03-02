#include "Include.hpp"

#define JSONKEYCOMP(data, key) if (data.find(key) != data.end())

class ContentBase {
protected:
	std::string Name;
	bool SizeFound = false;

public:

	ContentBase() { Name = "Base"; }
	virtual ~ContentBase() {}

	Pos2D<float> Pos{ 0,0 };
	Pos2D<float> PosOrigin{ 0,0 };
	Size2D<float> Size{ 0,0 };
	Color3<int> Bright{ 255,255,255 };
	Color3<int> AddColor{ 0,0,0 };
	struct {
		int Type = 0;
		int Pal = 0;
	} Blend;

	virtual void Create(const std::string& dir, const json& data) = 0;
	virtual void Init() {};
	virtual void Draw(...) = 0;
	virtual void End() = 0;
	_NODISCARD virtual ContentBase* Clone() = 0;

	void BaseCreate(const json& data) {
		SizeFound = false;
		JSONKEYCOMP(data, "Pos") { Pos = data["Pos"]; }
		JSONKEYCOMP(data, "PosOrigin") { PosOrigin = data["PosOrigin"]; }
		JSONKEYCOMP(data, "Size") { Size = data["Size"]; SizeFound = true; }
		JSONKEYCOMP(data, "Bright") { Bright = data["Bright"]; }
		JSONKEYCOMP(data, "AddColor") { AddColor = data["AddColor"]; }
		JSONKEYCOMP(data, "Blend") {
			//DX_BLENDMODE_NOBLEND;
			const std::vector<std::string> BlendList = {
				"noblned",
				"alpha",
				"add",
				"sub",
				"mul"
			};
			for (uint i = 0, size = BlendList.size(); i < size; ++i) {
				if (ToLower(data["Blend"]["Type"]) == BlendList[i]) {
					Blend.Type = i;
					Blend.Pal = data["Blend"]["Param"];
					break;
				}
			}
		}
	}
	void SetOptimization() {
		OptiGraph = MakeScreen(Size.Width, Size.Height, TRUE);

		SetDrawScreen(OptiGraph);
		SetDrawBright(Bright.R, Bright.G, Bright.B);
		SetDrawAddColor(AddColor.R, AddColor.G, AddColor.B);
	}
	const std::string& GetType() { return Name; }

	int OptiGraph = -1;
};

struct string_hash {
	using is_transparent = void;
	size_t operator()(std::string_view sv) const {
		return std::hash<std::string_view>{}(sv);
	}
};

class ContentManager {
protected:

	std::unordered_map<std::string, std::unique_ptr<ContentBase>, string_hash, std::equal_to<>> CanUseContents;

public:
	
	ContentBase* GetContentBase(const std::string& TypeName) {
		return CanUseContents[TypeName].get();
	}
	void AddContent(ContentBase* ptr) {
		CanUseContents[ptr->GetType()].reset(ptr);
	}
};

namespace Content {
	class Graph : public ContentBase {
	public:

		Graph() { Name = __func__; }
		~Graph() { End(); }

		bool AlphaChannel = true;

		void Create(const std::string& dir, const json& data) {
			BaseCreate(data);
			JSONKEYCOMP(data, "AlphaChannel") { AlphaChannel = data["AlphaChannel"]; }

			GraphData GD;
			GD.Load(dir + data["FilePath"].get<std::string>());
			if (!SizeFound) {
				Size = GD.Size;
			}
			
			SetOptimization();

			GD.Size = Size;
			GD.Draw({ Size.Width * 0.5f, Size.Height * 0.5f });

			GD.Delete();
		}
		void Draw(...) {
			DrawGraphF(
				Pos.X - (Size.Width * PosOrigin.X),
				Pos.Y - (Size.Height * PosOrigin.Y),
				OptiGraph,
				AlphaChannel
			);
		}
		void End() {
			DeleteGraph(OptiGraph);
		}
		_NODISCARD ContentBase* Clone() {
			return new Graph(*this);
		}
	};

	class MoveGraph : public Graph {
		Timer<second> MoveTimer;

	public:

		MoveGraph() { Name = __func__; }
		~MoveGraph() { End(); }

		double LoopTime = 1;
		Pos2D<uint> LoopCount{1,1};
		Pos2D<double> MoveDirection{0,0};

		void Create(const std::string& dir, const json& data) {
			Graph::Create(dir, data);

			JSONKEYCOMP(data, "LoopTime") { LoopTime = data["LoopTime"]; }
			JSONKEYCOMP(data, "LoopCount") { LoopCount = data["LoopCount"]; }
			JSONKEYCOMP(data, "MoveDirection") { MoveDirection = data["MoveDirection"]; }
			
			int oldoptigraph = OptiGraph;
			OptiGraph = MakeScreen(LoopCount.X * Size.Width, LoopCount.Y * Size.Height, TRUE);
			
			SetDrawScreen(OptiGraph);
			SetDrawBright(Bright.R, Bright.G, Bright.B);
			SetDrawAddColor(AddColor.R, AddColor.G, AddColor.B);

			for (uint i = 0; i < LoopCount.Y; ++i) {
				for (uint j = 0; j < LoopCount.X; ++j) {
					DrawGraphF(
						j * Size.Width,
						i * Size.Height,
						oldoptigraph,
						AlphaChannel
					);
				}
			}

			DeleteGraph(oldoptigraph);
		}
		void Init() {
			MoveTimer.Start();
		}
		void Draw(...) {
			double timeone = MoveTimer.GetRecordingTime() / LoopTime;
			if (timeone >= 1) {
				MoveTimer.Start();
			}
			Pos2D<float> addpos = {
				MoveDirection.X * Size.Width * timeone,
				MoveDirection.Y * Size.Height * timeone
			};
			DrawGraphF(
				Pos.X + addpos.X - (Size.Width * PosOrigin.X),
				Pos.Y + addpos.Y - (Size.Height * PosOrigin.Y),
				OptiGraph,
				AlphaChannel
			);
		}
		void End() {
			DeleteGraph(OptiGraph);
		}
		_NODISCARD ContentBase* Clone() {
			return new MoveGraph(*this);
		}
	};

	class String : public ContentBase {
	public:

		String() { Name = __func__; }
		~String() { End(); }

		std::string Drawstring;

		Color3<int> FontColor{ 255,255,255 };
		Color3<int> EdgeColor{ 0,0,0 };

		void Create(const std::string& dir, const json& data) {
			BaseCreate(data);
			JSONKEYCOMP(data, "String") { Drawstring = data["String"]; }

			int space = 0;
			float size = 0, thick = 0, edge = 0;

			JSONKEYCOMP(data, "Font") {
				JSONKEYCOMP(data, "Size") { size = data["Size"]; }
				JSONKEYCOMP(data, "Thick") { thick = data["Thick"]; }
				JSONKEYCOMP(data, "Edge") { edge = data["Edge"]; }
				JSONKEYCOMP(data, "Space") { space = data["Space"]; }

				JSONKEYCOMP(data, "Color") { FontColor = data["Color"]; }
				JSONKEYCOMP(data, "EdgeColor") { EdgeColor = data["EdgeColor"]; }
			}

			FontData FD;
			FD.Load(dir + data["Font"]["FilePath"].get<std::string>(), data["Font"]["Name"], size, thick, edge, space);
			
			Size2D<float> DrawSize{ 0,0 };
			DrawSize.Width = GetDrawStringWidthToHandle(Drawstring.c_str(), -1, FD.Handle);
			DrawSize.Height = size + (edge * 2);

			if (!SizeFound) {
				Size = DrawSize;
			}

			int FontGraph = MakeScreen(DrawSize.Width, DrawSize.Height, TRUE);
			
			SetDrawScreen(FontGraph);

			FD.Draw({ 0, size * 0.5f }, GetColor(FontColor.R, FontColor.G, FontColor.B), GetColor(EdgeColor.R, EdgeColor.G, EdgeColor.B), Drawstring);

			SetOptimization();

			DrawExtendGraphF(0, 0, Size.Width, Size.Height, FontGraph, TRUE);

			FD.Delete();
			DeleteGraph(FontGraph);
		}
		void Draw(...) {
			DrawGraphF(
				Pos.X - (Size.Width * PosOrigin.X),
				Pos.Y - (Size.Height * PosOrigin.Y),
				OptiGraph,
				TRUE
			);
		}
		void End() {
			DeleteGraph(OptiGraph);
		}
		_NODISCARD ContentBase* Clone() {
			return new String(*this);
		}
	};
}
