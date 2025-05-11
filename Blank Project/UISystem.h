#pragma once
#include <windows.h>
#include "Renderer.h"

class UISystem {

public:
	static UISystem* GetInstance() { return instance; }
	static void Initialize(HWND handle) {
		instance = (instance == nullptr) ? new UISystem(handle) : instance;
	}
	static void Shutdown() { if (instance) { delete instance; } }

	void StartFrame();
	void EndFrame();

	float getVertexScale() { return vertexScale; }
	float getheightScale() { return heightScale; }
	bool getColourMode() { return colourMode; }
	bool getGrassColour() { return grassColourMode; }

protected:
	UISystem(HWND handle);
	virtual ~UISystem();

	void RenderFrame();
	void FramerateUI();
	void DebugUI();

	static UISystem* instance;
	HWND handle;

	float vertexScale = 1.0f;
	float heightScale = 0.0f;
	bool colourMode = false;
	bool grassColourMode = true;
};
