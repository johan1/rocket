#include "../../common/Application.h"
#include "../../common/resource/FSResourcePackage.h"
#include "../../common/input/PointerEvent.h"

#include "../openal/OpenAlPlayer.h"
#include "KeyboardController.h"
#include "WglContextManager.h"

#include <windows.h>
#include <windowsx.h>
#include <rocket/Log.h>

using namespace rocket;

namespace rocket {
	extern void gameInit(); 
}

const char g_szClassName[] = "myWindowClass";

static bool surfaceCreated = false;

static int windowWidth = 800;
static int windowHeight = 800;

void postPointerEvent(PointerEvent::ActionType actionType, int mouseX, int mouseY) {
	float nx = static_cast<float>(2*mouseX-windowWidth)/static_cast<float>(windowWidth);
	float ny = static_cast<float>(windowHeight-2*mouseY)/static_cast<float>(windowHeight);

	Application::getApplication().post(
			PointerEvent(0, PointerEvent::PointerType::MOUSE,
			actionType, nx, ny));
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static KeyboardController keyboardController{};
	static bool mouseDown = false;
    switch(msg)
    {
        case WM_CLOSE:
			surfaceCreated = false;
			Application::getApplication().pause();
			Application::getApplication().surfaceDestroyed();
			Application::getApplication().destroy();
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
		case WM_SIZE:
		{
			if (surfaceCreated) {
				RECT rect;
				GetWindowRect(hwnd, &rect);
				windowWidth = rect.right - rect.left;
				windowHeight = rect.bottom - rect.top;
				LOGD("Window resized, " << windowWidth << ", " << windowHeight);
				Application::getApplication().surfaceChanged(0, windowWidth, windowHeight);
			}
		}
		break;
		case WM_LBUTTONDOWN: {
			mouseDown = true;
			auto mouseX = GET_X_LPARAM(lParam);
			auto mouseY = GET_Y_LPARAM(lParam);
			postPointerEvent(PointerEvent::ActionType::PRESSED, mouseX, mouseY);
			break;
		}
		case WM_LBUTTONUP: {
			mouseDown = false;
			auto mouseX = GET_X_LPARAM(lParam);
			auto mouseY = GET_Y_LPARAM(lParam);
			postPointerEvent(PointerEvent::ActionType::RELEASED, mouseX, mouseY);
			break;
		}
		case WM_MOUSEMOVE:
			if (mouseDown) {
				auto mouseX = GET_X_LPARAM(lParam);
				auto mouseY = GET_Y_LPARAM(lParam);
				postPointerEvent(PointerEvent::ActionType::MOVED, mouseX, mouseY);
			}
			break;
		case WM_KEYDOWN: // Fall-through to WM_SYSKEYDOWN
		case WM_SYSKEYDOWN:
			keyboardController.onKeyPressed(wParam);
			break;
		case WM_KEYUP: // Fall-through to WM_SYSKEYUP
		case WM_SYSKEYUP:
			keyboardController.onKeyReleased(wParam);
			break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
*/
int main()
{
	auto hInstance = GetModuleHandle(0);
	STARTUPINFOA infoA;
	GetStartupInfoA(&infoA);
	auto nCmdShow = infoA.wShowWindow;

	LOGD("Hello windows");

    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "The title of my window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

	RECT rect;
	GetWindowRect(hwnd, &rect);
	windowWidth = rect.right - rect.left;
	windowHeight = rect.bottom - rect.top;

	// Initialize rocket application
	LOGD("Hello rocket!");
	Director::getDirector().setInitFunction(rocket::gameInit);
	std::shared_ptr<ResourcePackage> rp = std::make_shared<FSResourcePackage>("./assets");
	ResourceManager rm {"assets"};
	rm.addResourcePackage("assets", rp);
	Application::init(std::move(rm), std::unique_ptr<PlatformAudioPlayer>(new OpenAlPlayer()));

	// Calling life-cycle callbacks and setting up gl context manager.
	WglContextManager contextManager{hwnd};
	Application::getApplication().create(&contextManager);
	Application::getApplication().surfaceCreated();
	Application::getApplication().resume();
	Application::getApplication().surfaceChanged(0, windowWidth, windowHeight);
	surfaceCreated = true;

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
