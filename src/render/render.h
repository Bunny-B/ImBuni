#ifndef RENDER_H
#define RENDER_H
#include <thread>
#include <atomic>

namespace render
{
	static std::thread renderThread;//sadly cant use good friend CreateThread for xplatform
	static std::atomic<bool> running(false);

	extern void startRenderThread(const char* window_name, void(*renderFunction)(), bool is_overlay, void* hIcon = nullptr, void* hIconSm = nullptr);

	extern void stopRenderThread();
};

#endif // !RENDER_H


/*
	USAGE

void myRenderFunction()
{
	
	ImGui::PushStyleColor(ImGuiCol_Text, { 255,0,0,255 });
	ImGui::Text("HELLO WORLD");
	if (ImGui::Button("aloo"))
	{
		Log("Real \n");
	}
	ImGui::PopStyleColor();

}

	render::startRenderThread("Overlay Window", myRenderFunction, true);

	// Do other stuff here...

	render::stopRenderThread(); // Stop rendering and cleanup

*/