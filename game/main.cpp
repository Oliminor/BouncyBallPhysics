#include "stdafx.h"
#include "MyGame.h"

CMyGame game;
CGameApp app;

int main(int argc, char* argv[])
{
	//app.DebugMemoryLeaks();
	app.OpenConsole();
	//app.OpenWindow(1280, 720, "My Game!");
	app.OpenFullScreen(1920, 1080, 0);
	app.SetFPS(60);
	app.SetClearColor(CColor::Black());
	app.Run(&game);
	return(0);
}
