// Copyright (c) 2019 Emmanuel Lajeunesse
#include "Precompiled.h"

#include "window/window.h"
#include "input/Input.h"

#include "utilities/Loader.h"
#include "utilities/logger.h"
#include "utilities/Time.h"

#include "modules/Material.h"
#include "../game/Scene_Game.h"
#include "modules/RenderManager.h"

#include "opengl/Debug.h"

/**/
using namespace Vxl;
using namespace std;

Scene_Game* _scene = new Scene_Game();

#if _DEBUG
// Console Mode
int main()
#else
// Disable Console in release mode
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
#endif
{
	// GLFW Setup
	if (!Window.InitGLFW())
		return -1;

	// Window
	Window.Setup("Vxl Engine", SCREEN_WIDTH, SCREEN_HEIGHT);
	//Window.SetCustomAspectRatio(true, 1.0f);

	/* Special */
	RenderManager.CreateSpecialManagers();
	/* ~ */
	RenderManager.SetNewScene(_scene);
	//_scene->setup();

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
	}

	// Cleanup
	RenderManager.Destroy();
	Window.Shutdown();

	return 0;
}