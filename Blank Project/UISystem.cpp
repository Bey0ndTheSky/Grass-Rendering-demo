#include "../imgui/imgui.h"
#include "UISystem.h"
#include <filesystem>    
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_opengl3.h"

UISystem* UISystem::instance = nullptr;

UISystem::UISystem(HWND handle) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_InitForOpenGL(handle);
	ImGui_ImplOpenGL3_Init();
}

UISystem::~UISystem() {
	ImGui::DestroyContext();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
}

void UISystem::StartFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	RenderFrame();
}

void UISystem::RenderFrame() {
	//ImGui::ShowDemoWindow();
	FramerateUI();
	DebugUI();
}

void UISystem::EndFrame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::EndFrame();
}

void UISystem::FramerateUI() {
	ImGui::Begin("Framerate", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavInputs);
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();
}

void UISystem::DebugUI() {
	ImGui::SetNextWindowPos(ImVec2(1200, 20), ImGuiCond_Always);
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
	ImGui::SliderFloat("Size", &vertexScale, 1.0f, 25.0f);
	ImGui::SliderFloat("Height", &heightScale, 0.0f, 5.0f);
	ImGui::Checkbox("ColourMode", &colourMode);
	
	ImGui::Checkbox("Grass colour mode", &grassColourMode);
	ImGui::SliderFloat("Grass Height", &grassHeight, 10.0f, 50.0f);
	ImGui::SliderFloat("Grass Width", &grassWidth, 1.0f, 15.0f);
	ImGui::ColorEdit3("Select grass base colour", (float*)&grassColourBase);
	ImGui::ColorEdit3("Select grass top colour", (float*)&grassColourTop);
	ImGui::SliderFloat("Wind strength", &windStrength, 0.0f, 20.0f);

	ImGui::SliderFloat("Light Height", &lightPosition, 1.0f, 50.0f);
	ImGui::SliderFloat("Light Radius", &lightRadius, 0.5f, 10.0f);
	ImGui::ColorEdit3("Select light colour", (float*)&lightColour);
	
	ImGui::End();
}