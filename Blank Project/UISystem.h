#pragma once
#include <windows.h>
#include "Renderer.h"
#include <list>
#include <string>

class UISystem {
	std::list<int> uiList;

public:
	static UISystem* GetInstance() { return instance; }
	static void Initialize(HWND handle) {
		instance = (instance == nullptr) ? new UISystem(handle) : instance;
	}
	static void Shutdown() { if (instance) { delete instance; } }

	void StartFrame();
	void EndFrame();

	

protected:
	UISystem(HWND handle);
	virtual ~UISystem();

	void RenderFrame();
	void FramerateUI();
	void DebugUI();

	static UISystem* instance;
	HWND handle;
};
