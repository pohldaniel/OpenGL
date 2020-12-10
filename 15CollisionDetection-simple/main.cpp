#define NOMINMAX

#include <windows.h>			
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include "GL.h"
#include "Extension.h"
#include "Animation\AnimatedModel\AnimatedModel.h"
#include "MeshObject\MeshSphere.h"
#include "MeshObject\MeshQuad.h"
#include "MeshObject\MeshCube.h"
#include "MeshObject\MeshTorus.h"
#include "MeshObject\MeshSpiral.h"
#include "Model.h"
#include "Camera.h";
#include "Entity3D.h"
#include "Skybox.h"
#include "Collision.h"
#include "Object.h"

int height = 960;
int width = 1280;

int g_windowWidth = 640;
int g_windowHeight = 480;

POINT g_OldCursorPos;
bool g_enableVerticalSync;
bool g_enableWireframe;

enum DIRECTION {
	DIR_FORWARD = 1,
	DIR_BACKWARD = 2,
	DIR_LEFT = 4,
	DIR_RIGHT = 8,
	DIR_UP = 16,
	DIR_DOWN = 32,
	RESET = 64,

	DIR_FORCE_32BIT = 0x7FFFFFFF
};

Entity3D ballEntity;
Entity3D cowboyEntity;

SkyBox* skyBox;
ThirdPersonCamera camera;
MeshSphere *sphere;
MeshQuad *quad;
MeshCube *cube;
MeshTorus *torus;
MeshSpiral *spiral;
Collision collision;

const float moveSpeed = 10000.0f;
double physDeltaTime = 1.0 / 60.0;
const double slowest_frame = 1.0 / 15.0;
double delta_Time, accumulator = 0.0;
const float friction = 0.9;

const float HEADING_SPEED = 150.0f;
const float ROLLING_SPEED = 280.0f;
bool UPDATEFRAME = true;

//prototype funktions
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParma, LPARAM lParam);
void setCursortoMiddle(HWND hwnd);
void enableWireframe(bool enableWireframe);
void enableVerticalSync(bool enableVerticalSync);

void initApp(HWND hWnd);
void processInput(HWND hWnd);
void updateFrame(HWND hWnd, float elapsedTimeSec);

// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 1300, 0, 550, 300, true);
	std::cout << "w, a, s, d, mouse : move camera" << std::endl;
	std::cout << "arrow keys        : move sphere" << std::endl;
	std::cout << "space             : release capture" << std::endl;
	std::cout << "v                 : toggle vsync" << std::endl;
	std::cout << "z                 : toggle wireframe" << std::endl;

	WNDCLASSEX		windowClass;		// window class
	HWND			hwnd;				// window handle
	MSG				msg;				// message
	HDC				hdc;				// device context handle

										// fill out the window class structure
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = winProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);		// default icon
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);			// default arrow
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	// white background
	windowClass.lpszMenuName = NULL;									// no menu
	windowClass.lpszClassName = "WINDOWCLASS";
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);			// windows logo small icon

																// register the windows class
	if (!RegisterClassEx(&windowClass))
		return 0;

	// class registered, so now create our window
	hwnd = CreateWindowEx(NULL,									// extended style
		"WINDOWCLASS",						// class name
		"ColladaViewer",					// app name
		WS_OVERLAPPEDWINDOW,
		0, 0,									// x,y coordinate
		width,
		height,									// width, height
		NULL,									// handle to parent
		NULL,									// handle to menu
		hInstance,							// application instance
		NULL);								// no extra params

											// check if window creation failed (hwnd would equal NULL)
	if (!hwnd)
		return 0;

	ShowWindow(hwnd, SW_SHOW);			// display the window
	UpdateWindow(hwnd);					// update the window

	initApp(hwnd);

	AnimatedModel mushroom;
	mushroom.loadModel(".\\res\\mushroom.dae", ".\\res\\mushroom.png");
	mushroom.scale(10.0f);
	mushroom.translate(-10.0f, 0.0f, -10.0f);

	AnimatedModel cowboy;
	cowboy.loadModel(".\\res\\cowboy.dae", ".\\res\\cowboy.png");
	cowboy.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180.0f);
	cowboy.translate(100.0f, 0.0f, -10.0f);
	cowboy.scale(10.0f);

	AnimatedModel dragon;
	dragon.loadModel(".\\res\\dragon.dae", ".\\res\\dragon.png");
	dragon.scale(20.0f);
	dragon.translate(0.0f, 5.0f, -10.0f);

	mushroom.getAnimator()->startAnimation("");
	cowboy.getAnimator()->startAnimation("");
	dragon.getAnimator()->startAnimation("");

	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point end;
	std::chrono::duration<double> deltaTime;

	// main message loop
	while (true) {

		// Did we recieve a message, or are we idling ?
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			// test if this is a quit
			if (msg.message == WM_QUIT) break;
			// translate and dispatch message
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else {

			end = start;
			start = std::chrono::high_resolution_clock::now();
			deltaTime = start - end;
			delta_Time = deltaTime.count();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(1.0, 1.0, 1.0, 0.0);

			cowboy.update(delta_Time);
			cowboy.draw(camera, cowboyEntity);

			mushroom.update(delta_Time);
			mushroom.draw(camera);

			dragon.update(delta_Time);
			dragon.draw(camera);

			// prevent spiral of death
			if (delta_Time > slowest_frame)
				delta_Time = slowest_frame;

			// update at a constant rate to keep physics in check
			accumulator += delta_Time;
			while (accumulator >= physDeltaTime) {
				processInput(hwnd);
				updateFrame(hwnd, physDeltaTime);

				Vector3f velocity = ballEntity.getVelocity();
				Vector3f collExtentsMin, collExtentsMax, newPos, newVelocity;
				Vector3f radius = (sphere->m_max - sphere->m_min) * 0.5;


				velocity[1] -= (200.0f * physDeltaTime);
				// test for collision against the scene
				if (collision.collideEllipsoid(ballEntity.getPosition(), radius, velocity * physDeltaTime, newPos, newVelocity, collExtentsMin, collExtentsMax)) {

					// scale new frame based velocity into per-second
					newVelocity = newVelocity / physDeltaTime;

					if (collExtentsMin[1] < -(radius[1] - (radius[1] / 4.25f))) {
						// smooth down the velocity
						velocity[0] = newVelocity[0] * (1 - friction);
						velocity[2] = newVelocity[2] * (1 - friction);
						velocity[1] = std::max(0.0f, velocity[1]);

						ballEntity.setVelocity(velocity);
						ballEntity.setGrounded(true);
					}

					// store our new vertical velocity. it's important to ignore
					// the x / z velocity changes to allow us to 'bump' over objects
					velocity[1] = newVelocity[1];

					// truncate the impact extents so the interpolation below begins
					// and ends at the above the player's "feet"
					radius[1] -= (radius[1] / 4.25f);
					collExtentsMax[1] = std::max(-radius[1], collExtentsMax[1]);
					collExtentsMax[1] = std::min(radius[1], collExtentsMax[1]);

					// linearly interpolate (based on the height of the maximum intersection
					// point) the diminishing x/z velocity values returned from the collision function
					float fScale = 1 - (fabsf(collExtentsMax[1]) / radius[1]);
					velocity[0] = velocity[0] + (((newVelocity[0]) - velocity[0]) * fScale);
					velocity[2] = velocity[2] + (((newVelocity[2]) - velocity[2]) * fScale);

					// update our position and velocity to the new one
					ballEntity.setPosition(newPos[0], newPos[1], newPos[2]);
					ballEntity.setVelocity(velocity);

				}else {
					// smooth down the velocity
					velocity[0] = velocity[0] * (1 - friction);
					velocity[2] = velocity[2] * (1 - friction);
					ballEntity.setVelocity(velocity);
				}
				accumulator -= physDeltaTime;
			}

			sphere->draw(camera, ballEntity);
			cube->draw(camera);
			quad->draw(camera);
			torus->draw(camera);
			spiral->draw(camera);


			skyBox->render(camera);

			hdc = GetDC(hwnd);
			SwapBuffers(hdc);
			ReleaseDC(hwnd, hdc);
		}
	} // end while

	delete cube;
	delete sphere;
	delete quad;
	delete torus;
	delete spiral;
	delete skyBox;

	return msg.wParam;
}

// the Windows Procedure event handler
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static HGLRC hRC;					// rendering context
	static HDC hDC;						// device context
	int width, height;					// window width and height
	POINT pt;
	RECT rect;

	switch (message) {

	case WM_DESTROY: {

		PostQuitMessage(0);
		return 0;
	}

	case WM_CREATE: {

		GetClientRect(hWnd, &rect);
		g_OldCursorPos.x = rect.right / 2;
		g_OldCursorPos.y = rect.bottom / 2;
		pt.x = rect.right / 2;
		pt.y = rect.bottom / 2;
		SetCursorPos(pt.x, pt.y);
		// set the cursor to the middle of the window and capture the window via "SendMessage"
		SendMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
		return 0;
	}break;

	case WM_LBUTTONDOWN: { // Capture the mouse

		setCursortoMiddle(hWnd);
		SetCapture(hWnd);

		return 0;
	} break;

	case WM_KEYDOWN: {

		switch (wParam) {

		case VK_ESCAPE: {

			PostQuitMessage(0);
			return 0;

		}break;
		case VK_SPACE: {

			ReleaseCapture();
			return 0;

		}break;
		case 'v': case 'V': {
			enableVerticalSync(!g_enableVerticalSync);
			return 0;

		}break;
		case 'z': case 'Z': {
			enableWireframe(!g_enableWireframe);
		}break;
		case 'm': case 'M': {
			camera.resetCamera(ballEntity);

		}break;
			return 0;
		}break;

		return 0;
	}break;

	case WM_SIZE: {

		int _height = HIWORD(lParam);		// retrieve width and height
		int _width = LOWORD(lParam);

		if (_height == 0) {					// avoid divide by zero
			_height = 1;
		}

		glViewport(0, 0, _width, _height);
		camera.perspective(45.0f, static_cast<float>(_width) / static_cast<float>(_height), 1.0f, 5000.0f);

		if (skyBox) skyBox->setProjectionMatrix(camera.getProjectionMatrix());

		return 0;
	}break;

	default:
		break;
	}
	return (DefWindowProc(hWnd, message, wParam, lParam));
}

void initApp(HWND hWnd) {

	static HGLRC hRC;					// rendering context
	static HDC hDC;						// device context

	hDC = GetDC(hWnd);
	int nPixelFormat;					// our pixel format index

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// size of structure
		1,								// default version
		PFD_DRAW_TO_WINDOW |			// window drawing support
		PFD_SUPPORT_OPENGL |			// OpenGL support
		PFD_DOUBLEBUFFER,				// double buffering support
		PFD_TYPE_RGBA,					// RGBA color mode
		32,								// 32 bit color mode
		0, 0, 0, 0, 0, 0,				// ignore color bits, non-palettized mode
		0,								// no alpha buffer
		0,								// ignore shift bit
		0,								// no accumulation buffer
		0, 0, 0, 0,						// ignore accumulation bits
		16,								// 16 bit z-buffer size
		0,								// no stencil buffer
		0,								// no auxiliary buffer
		PFD_MAIN_PLANE,					// main drawing plane
		0,								// reserved
		0, 0, 0 };						// layer masks ignored

	nPixelFormat = ChoosePixelFormat(hDC, &pfd);	// choose best matching pixel format
	SetPixelFormat(hDC, nPixelFormat, &pfd);		// set pixel format to device context


													// create rendering context and make it current
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
	enableVerticalSync(true);

	glEnable(GL_DEPTH_TEST);					// hidden surface removal
	glEnable(GL_CULL_FACE);						// do not calculate inside of poly's

												// Setup the camera.
	camera.perspective(45.0f, static_cast<float>(g_windowWidth) / static_cast<float>(g_windowHeight), 1.0f, 5000.0f);
	camera.lookAt(Vector3f(0.0f, 20.0f, 220.f), Vector3f(0.0f, 20.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));

	// Initialize the skybox
	// sometimes the iamges from the box have to be flipped vertical, horizontal
	skyBox = new SkyBox("../skyboxes/sor_sea", 1000, true, false, Vector3f(0.0f, 0.5f, 0.0f));
	skyBox->setProjectionMatrix(camera.getProjectionMatrix());

	// initialize the ball and cowboy
	ballEntity.constrainToWorldYAxis(true);
	ballEntity.setPosition(0.0f, 45.0f, 0.0f);
	ballEntity.setCamOffset(0.0f, 40.0f, 60.0f);
	ballEntity.update(0.0f);

	cowboyEntity.constrainToWorldYAxis(true);
	cowboyEntity.setPosition(0.0f, 0.0f, 0.0f);

	// Setup some meshes
	cube = new MeshCube(Vector3f(0.0f, 50.01f, -200.0f), 100, 100, 100, ".\\res\\marble.png");
	cube->setPrecision(1, 1);
	cube->buildMesh4Q();

	quad = new MeshQuad(1024, 1024, ".\\res\\floor_color_map.png");
	quad->setPrecision(1, 1);
	quad->buildMesh();

	sphere = new MeshSphere(Vector3f(0.0f, 0.0f, 0.0), 20.0f, ".\\res\\earth2048.png");
	sphere->setPrecision(20, 20);
	sphere->buildMesh();

	torus = new MeshTorus(Vector3f(-150.0f, 0.01f, -200.0f), 50.0f, 30.0f, ".\\res\\darkchecker.png");
	torus->setPrecision(30, 30);
	torus->buildMesh();

	spiral = new MeshSpiral(Vector3f(150.0f, 25.0f, -200.0f), 50.0f, 30.0f, 2, 150.0f, ".\\res\\checker.png");
	spiral->setPrecision(20, 20);
	spiral->buildMesh();

	if (collision.addIndexedPrimitive(cube->m_positions, cube->m_indexBuffer) &&
		collision.addIndexedPrimitive(torus->m_positions, torus->m_indexBuffer) &&
		collision.addIndexedPrimitive(spiral->m_positions, spiral->m_indexBuffer) &&
		collision.addIndexedPrimitive(quad->m_positions, quad->m_indexBuffer)) {

		std::cout << "Primitive added!" << std::endl;
	}
}

void setCursortoMiddle(HWND hwnd) {
	RECT rect;

	GetClientRect(hwnd, &rect);
	SetCursorPos(rect.right / 2, rect.bottom / 2);
}

void enableVerticalSync(bool enableVerticalSync) {

	// WGL_EXT_swap_control.
	typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC)(GLint);

	static PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
		reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(
			wglGetProcAddress("wglSwapIntervalEXT"));

	if (wglSwapIntervalEXT) {
		wglSwapIntervalEXT(enableVerticalSync ? 1 : 0);
		g_enableVerticalSync = enableVerticalSync;
	}
}

void enableWireframe(bool enableWireframe) {

	g_enableWireframe = enableWireframe;

	if (g_enableWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void processInput(HWND hWnd) {

	static UCHAR pKeyBuffer[256];
	ULONG        Direction = 0;
	POINT        CursorPos;
	float        X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if (!GetKeyboardState(pKeyBuffer)) return;

	// Check the relevant keys
	if (pKeyBuffer['W'] & 0xF0) Direction |= DIR_FORWARD;
	if (pKeyBuffer['S'] & 0xF0) Direction |= DIR_BACKWARD;
	if (pKeyBuffer['A'] & 0xF0) Direction |= DIR_LEFT;
	if (pKeyBuffer['D'] & 0xF0) Direction |= DIR_RIGHT;
	if (pKeyBuffer['E'] & 0xF0) Direction |= DIR_UP;
	if (pKeyBuffer['Q'] & 0xF0) Direction |= DIR_DOWN;

	// Now process the mouse (if the button is pressed)
	if (GetCapture() == hWnd) {
		// Hide the mouse pointer
		SetCursor(NULL);
		// Retrieve the cursor position
		GetCursorPos(&CursorPos);

		// Calculate mouse rotational values
		X = (float)(g_OldCursorPos.x - CursorPos.x) * 0.1;
		Y = (float)(g_OldCursorPos.y - CursorPos.y) * 0.1;

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos(g_OldCursorPos.x, g_OldCursorPos.y);

		if (Direction > 0 || X != 0.0f || Y != 0.0f) {
			UPDATEFRAME = false;
			// Rotate camera
			if (X || Y) {
				camera.rotate(X, Y, 0.0f);

			} // End if any rotation

			if (Direction) {

				float dx = 0, dy = 0, dz = 0, speed = 4.3;

				if (Direction & DIR_FORWARD) dz = speed;
				if (Direction & DIR_BACKWARD) dz = -speed;
				if (Direction & DIR_LEFT) dx = -speed;
				if (Direction & DIR_RIGHT) dx = speed;
				if (Direction & DIR_UP) dy = speed;
				if (Direction & DIR_DOWN) dy = -speed;

				camera.move(dx, dy, dz);
			}

		}// End if any movement
	} // End if Captured
}

void updateFrame(HWND hWnd, float elapsedTimeSec) {
	static UCHAR pKeyBuffer[256];
	ULONG        Direction = 0;
	POINT        CursorPos;
	float        X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if (!GetKeyboardState(pKeyBuffer)) return;

	// Check the relevant keys
	if (pKeyBuffer[VK_UP] & 0xF0) Direction |= DIR_FORWARD;
	if (pKeyBuffer[VK_DOWN] & 0xF0) Direction |= DIR_BACKWARD;
	if (pKeyBuffer[VK_LEFT] & 0xF0) Direction |= DIR_LEFT;
	if (pKeyBuffer[VK_RIGHT] & 0xF0) Direction |= DIR_RIGHT;
	if (pKeyBuffer[WM_KEYUP] & 0xF0) Direction |= RESET;

	float pitch = 0.0f;
	float heading = 0.0f;

	if (GetCapture() == hWnd) {
		// Hide the mouse pointer
		SetCursor(NULL);

		if ((pKeyBuffer[VK_LCONTROL] & 0xF0) || (pKeyBuffer[VK_RCONTROL] & 0xF0) && ballEntity.isGrounded()) {
			ballEntity.setVelocityY((pKeyBuffer[VK_RCONTROL] & 0xF0) ? 130.0f : 200.0f);
			ballEntity.setGrounded((!pKeyBuffer[VK_RCONTROL] & 0xF0));
		}

		if (Direction) UPDATEFRAME = true;

		if (UPDATEFRAME) {

			if (Direction & DIR_FORWARD) {
				pitch = -ROLLING_SPEED;

				Vector3f dir = ballEntity.getForwardVector();
				dir = dir * moveSpeed * elapsedTimeSec;
				ballEntity.setVelocityXZ(dir[0], dir[2]);
			}

			if (Direction & DIR_BACKWARD) {
				pitch = ROLLING_SPEED;

				Vector3f dir = ballEntity.getForwardVector();
				dir = -dir * moveSpeed * elapsedTimeSec;
				ballEntity.setVelocityXZ(dir[0], dir[2]);
			}

			if (Direction & DIR_LEFT) {
				heading = HEADING_SPEED;
			}

			if (Direction & DIR_RIGHT) {
				heading = -HEADING_SPEED;
			}

			// first move the ball.			
			ballEntity.orient(heading, 0.0f, 0.0f);
			ballEntity.rotate(0.0f, pitch, 0.0f);
			ballEntity.update(physDeltaTime);

			// then move the camera based on where the ball has moved to.
			// when the ball is moving backwards rotations are inverted to match
			// the direction of travel. Consequently the camera's rotation needs to be
			// inverted as well.		
			camera.rotate(Direction & DIR_FORWARD ? heading : -heading, 0.0f);
			camera.lookAt(ballEntity.getPosition() + Vector3f(0.0f, 30.0f, 0.0f));
			camera.update(physDeltaTime, ballEntity);
		}
	}
}