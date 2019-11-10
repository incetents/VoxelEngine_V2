// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "window/window.h"
#include "input/Input.h"

#include "utilities/Logger.h"
#include "utilities/Time.h"
#include "utilities/Macros.h"

#include "modules/Material.h"
#include "../game/Scene_Game.h"
#include "rendering/RenderManager.h"

/**/
using namespace Vxl;
using namespace std;

#ifdef GLOBAL_OUTPUT_CONSOLE
int main()
#else
#pragma comment(linker, "/SUBSYSTEM:Windows")// /ENTRY:mainCRTStartup")
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
#endif
{
	// Window
	Window.Setup("Vxl Engine", SCREEN_WIDTH, SCREEN_HEIGHT);
	//Window.SetCustomAspectRatio(true, 1.0f);
	Window.SetIcon("./assets/applogo/logo_32.png");

	/* Initial Call */
	RenderManager.InitImGui();
	RenderManager.InitGlobalGLResources();
	Scene_Game* _scene = new Scene_Game();
	RenderManager.SetNewScene(_scene);

	/* ~ */
	while (!Window.GetClosed())
	{
		// OpenGl Reset
		if (Input.getKeyDown(KeyCode::F5))
		{
			RenderManager.ReloadWindow();
		}
		// Shader Reset
		if (Input.getKeyDown(KeyCode::R))
		{
			RenderManager.ReloadShaders();
		}
		// FBO Rest
		if (Input.getKeyDown(KeyCode::F6))
		{
			RenderManager.ReloadFBOS();
		}

		// Before Frame
		TimeController.StartFrame();
		Window.StartFrame();

		// Scene Update/Render
		while (TimeController.GetTotalDeltaTime() >= 1.0)
		{
			RenderManager.UpdateFixed();
			TimeController.DecreaseTotalDeltaTime();
		}
		RenderManager.Update();
		RenderManager.Draw();
		RenderManager.DrawImGui();

		// End of frame update
		Input.Update();
		Window.EndFrame();
		TimeController.EndFrame();

		// Special
#if _DEBUG
		Graphics::GetRuntimeGLValues();
#endif
		
	}

	// Cleanup
	RenderManager.SetNewScene(nullptr);
	RenderManager.DestroyGlobalGLResources();
	RenderManager.DestroySceneGLResources();
	Window.Shutdown();

	return 0;
}