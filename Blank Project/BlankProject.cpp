#include "../nclgl/window.h"
#include "Renderer.h"
#include "UISystem.h"

int main() {
	Window w("Coursework!", 1600, 900, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	UISystem::Initialize(w.GetHandle());
	w.ShowOSPointer(true);
	w.LockMouseToWindow(true);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		w.LockMouseToWindow(true);
		float timestep = w.GetTimer()->GetTimeDeltaSeconds();
		renderer.UpdateScene(timestep);
		renderer.RenderScene();
		renderer.SwapBuffers();
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_F5)) {
			Shader::ReloadAllShaders();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F1)) {
			renderer.changeScene();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F2)) {
			renderer.LockCamera();
		}
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_F3)) {
			renderer.TogglePostProcess();
		}
	}
	return 0;
}