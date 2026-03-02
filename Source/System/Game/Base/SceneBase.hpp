#include "Include.hpp"

class SceneBase {
protected:
	std::string Name = "Base";

public:

	SceneBase() {}
	virtual ~SceneBase() {}

	virtual void Init() = 0;
	virtual void Draw() = 0;
	virtual void Proc() = 0;
	virtual void End() = 0;

	const std::string& GetName() const { return Name; }
};