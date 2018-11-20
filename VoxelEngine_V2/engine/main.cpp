// Copyright (c) 2018 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "opengl/glUtil.h"

#include "window/window.h"
#include "input/Input.h"

#include "utilities/Loader.h"
#include "utilities/logger.h"

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

#include "../game/Scene.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h> 

/**/
using namespace Vxl;
using namespace std;

Scene* _scene = new Scene();

#if _DEBUG
// Console Mode
int main()
#else
// Disable Console in release mode
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
#endif
{
	// WINDOW Setup
	if (!glfwInit())
		return 1;

	Window.Setup("Vxl Engine", SCREEN_WIDTH, SCREEN_HEIGHT);

	// IMGUI Setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForOpenGL(Window.GetContext(), true);
	ImGui_ImplOpenGL3_Init("#version 430");

	ImGui::StyleColorsDark();


	// Glew/Opengl Init
	glUtil::initGlew();
	glUtil::initHints();

	std::cout << "OpenGL Vers. " << glUtil::getOpenGLVersion() << std::endl;
	std::cout << glUtil::getRendererVersion() << std::endl;
	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;

	// Special
	/* ~ */
	Geometry::Setup();
	Loader::LoadScript_ImportFiles("./scripts/ImportFiles.txt");

	/* ~ */
	_scene->setup();

	/* ~ */
	while (!Window.GetClosed())
	{
		// OpenGl Reset
		if (Input.getKeyDown(KeyCode::R))
		{
			ShaderProgram::m_database.DeleteAndClear();
			Shader::m_database.DeleteAndClear();
			Texture::m_database.DeleteAndClear();
			// Reload Framebuffer Objects too

			Loader::LoadScript_ImportFiles("./scripts/ImportFiles.txt");

			//Shader_Gbuffer = ShaderProgram::m_database.Get("gbuffer");
			//Shader_Passthrough = ShaderProgram::m_database.Get("passthrough");

			//tex = Texture::m_database.Get("beato");
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		_scene->update();
		_scene->draw();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		//
		Input.Update();
		Window.Update();
	}


	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	Window.Terminate();

	return 0;
}