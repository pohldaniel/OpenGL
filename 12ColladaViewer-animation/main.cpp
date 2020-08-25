#include <windows.h>			
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include "AnimatedModel\AnimatedModel.h"
#include "Bitmap.h"
#include "Model.h"
#include "GL.h"
#include  "Extension.h"
#include "Camera.h";
#include "Object.h"
#include "ModelMatrix.h"
#include "Skybox.h"

int height = 480;
int width = 640;

POINT g_OldCursorPos;
bool g_enableVerticalSync;


enum DIRECTION {
	DIR_FORWARD = 1,
	DIR_BACKWARD = 2,
	DIR_LEFT = 4,
	DIR_RIGHT = 8,
	DIR_UP = 16,
	DIR_DOWN = 32,

	DIR_FORCE_32BIT = 0x7FFFFFFF
};


std::vector<Object*> objects;

Shader *shaderScene;
SkyBox* skyBox;

Camera *camera;
float active[2];
bool rotate = true;
bool shadow = false;
float heighscale = -0.02;
float curvature = -0.01;
float tile = 1.0;

std::vector<LightSource> lights;
float degree = 0.0;
//prototype funktions
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParma, LPARAM lParam);
void setCursortoMiddle(HWND hwnd);
void processInput(HWND hWnd);

void render();
void initApp(HWND hWnd);
void enableVerticalSync(bool enableVerticalSync);

Matrix4f modelView;
ModelMatrix modelLight;


// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){

	Vector3f camPos(0.0, 5.0, 20.0);
	Vector3f xAxis(1, 0, 0);
	Vector3f yAxis(0, 1, 0);
	Vector3f zAxis(0, 0, 1);
	Vector3f target(0.0, 5.0, 0.1);

	Vector3f up(0.0, 1.0, 0.0);

	camera = new Camera(camPos, xAxis, yAxis, zAxis, target, up);

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");

	MoveWindow(GetConsoleWindow(), 740, 0, 550, 200, true);
	std::cout << "w, a, s, d, mouse : move camera" << std::endl;
	std::cout << "space             : release capture" << std::endl;
	/*std::cout << "1, 2              : disable/enable lightsource" << std::endl;
	std::cout << "r                 : rotate light" << std::endl;
	std::cout << "v                 : disable/enable vsync" << std::endl;
	std::cout << "-, +              : decrease/increase displacement intensity" << std::endl;
	std::cout << "numpad - +        : rotate objeckts" << std::endl;
	std::cout << "return            : stop rotating objeckts" << std::endl;	
	std::cout << "b                 : shadows" << std::endl;
	std::cout << "k, l              : texture tile" << std::endl;*/

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
	mushroom.LoadModel(".\\res\\mushroom.dae", ".\\res\\mushroom.png");
	
	AnimatedModel cowboy;
	cowboy.LoadModel(".\\res\\model.dae", ".\\res\\diffuse.png");
	cowboy.translate(-5.0, 0.0, 0.0);


	std::chrono::steady_clock::time_point start = std::chrono::high_resolution_clock::now();
	std::chrono::steady_clock::time_point end;

	std::chrono::duration<double> deltaTime;

	mushroom.getAnimator()->startAnimation("");
	cowboy.getAnimator()->startAnimation("");
	// main message loop
	while (true){

		// Did we recieve a message, or are we idling ?
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){

			// test if this is a quit
			if (msg.message == WM_QUIT) break;
			// translate and dispatch message
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}else{

			end = start;
			start = std::chrono::high_resolution_clock::now();
			deltaTime = start - end;

			

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(1.0, 1.0, 1.0, 0.0);
			mushroom.Update(deltaTime.count());
			mushroom.Draw(*camera);

			cowboy.Update(deltaTime.count());
			cowboy.Draw(*camera);

			//render();
			processInput(hwnd);
			hdc = GetDC(hwnd);
			SwapBuffers(hdc);
			ReleaseDC(hwnd, hdc);
		}
	} // end while

	for (int i = 0; i < objects.size(); i++) {
		delete objects[i];
	}

	return msg.wParam;
}

// the Windows Procedure event handler
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){

	static HGLRC hRC;					// rendering context
	static HDC hDC;						// device context
	int width, height;					// window width and height
	POINT pt;
	RECT rect;

	switch (message)
	{
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
		case '1': {
			if (active[0] == 1.0) active[0] = 0.0;
			else  active[0] = 1.0;
		}break;
		case '2': {
			if (active[1] == 1.0) active[1] = 0.0;
			else  active[1] = 1.0;
		}break;
		case VK_OEM_PLUS: {

			heighscale = heighscale + 0.01;

		}break;
		case VK_OEM_MINUS: {
			heighscale = heighscale - 0.01;
			// if (heighscale < 0) heighscale = 0.0f;

		}break;
		case VK_ADD: {

			degree = degree + 0.01;
		}break;
		case VK_SUBTRACT: {
			degree = degree - 0.01;

		}break;
		case VK_RETURN: {
			degree = 0.0;

		}break;
		case 'r':case 'R': {
			if (rotate) rotate = false;
			else  rotate = true;

		}break;
		case 'b':case 'B': {
			if (shadow) shadow = false;
			else  shadow = true;

		}break;
		case 'K': {
			tile = tile + 0.1;

		}break;
		case 'L': {
			tile = tile - 0.1;

		}break;
			return 0;
		}break;

		return 0;
	}break;

	case WM_SIZE: {

		int height2 = HIWORD(lParam);		// retrieve width and height
		int width2 = LOWORD(lParam);

		if (height2 == 0) {					// avoid divide by zero

			height2 = 1;
		}

		glViewport(0, 0, width2, height2);
		camera->perspective(45.0f, (GLfloat)width2 / (GLfloat)height2, 1.0f, 2000.0f);

		for (int j = 0; j < objects.size(); j++) {

			for (int i = 0; i < objects[j]->m_model->numberOfMeshes(); i++) {

				glUseProgram(objects[j]->m_shader[i]->m_program);
				objects[j]->m_shader[i]->loadMatrix("u_projection", camera->getProjectionMatrix());
				glUseProgram(0);
			}
		}


		return 0;
	}break;

	default:
		break;
	}
	return (DefWindowProc(hWnd, message, wParam, lParam));
}






void initApp(HWND hWnd){

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


	camera->perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 2000.0f);


	glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

	glEnable(GL_DEPTH_TEST);					// hidden surface removal
												//glEnable(GL_CULL_FACE);						// do not calculate inside of poly's



	LightSource light;

	light.ambient = Vector3f(0.2, 0.2, 0.2);
	light.diffuse = Vector3f(0.5, 0.5, 0.5);
	light.specular = Vector3f(0.2, 0.2, 0.2);
	light.position = Vector3f(-600, 10, 0.0);
	active[0] = 1.0;
	lights.push_back(light);

	light.ambient = Vector3f(0.2, 0.2, 0.2);
	light.diffuse = Vector3f(0.5, 0.5, 0.5);
	light.specular = Vector3f(0.1, 0.1, 0.1);
	light.position = Vector3f(600, 10, 0.0);
	active[1] = 1.0;
	lights.push_back(light);

	// sometimes the iamges from the box have to be flipped vertical, horizontal
	skyBox = new SkyBox("../skyboxes/sor_sea", 500, camera, true, false);
	//skyBox = new SkyBox("../skyboxes/sea", 500, camera, true, true);
	//skyBox = new SkyBox("../skyboxes/hw_morning", 500, camera, true, false);

	Object *object;

	object = new Object();
	object->initModel("objs/Teapot/teapot.obj");
	object->initShader("shader/phongTexture.vert", "shader/phongTexture.frag");
	object->m_model->setRotPos(Vector3f(0.0, 1.0, 0.0), -45.0, -100.0, 0.0, 0.0);

	objects.push_back(object);

	object = new Object();
	object->initModel("objs/Teapot/teapot.obj");
	object->initShader();
	object->m_model->rotate(Vector3f(0.0, 1.0, 0.0), -45.0);

	objects.push_back(object);

	object = new Object();
	object->initModel("objs/Teapot/teapot.obj");
	object->initShader(new EnvironmentMap("shader/envmap.vert", "shader/envmap.frag", skyBox->m_cubemap));
	object->m_model->setRotPos(Vector3f(0.0, 1.0, 0.0), -45.0, 100.0, 0.0, 0.0);

	objects.push_back(object);

	object = new Object();
	object->initModel("objs/Quad/quad.obj");
	object->initShader();


	object->m_model->setRotPos(Vector3f(1.0, 0.0, 0.0), 90.0, 0.0, -50.0, 0.0);
	object->m_model->scale(50, 50, 0);

	objects.push_back(object);

	for (int j = 0; j < objects.size(); j++) {

		for (int i = 0; i < objects[j]->m_model->numberOfMeshes(); i++) {

			glUseProgram(objects[j]->m_shader[i]->m_program);
			objects[j]->m_shader[i]->loadMatrix("u_projection", camera->getProjectionMatrix());
			objects[j]->m_shader[i]->loadLightSources(lights);
			glUseProgram(0);
		}
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

	if (wglSwapIntervalEXT)
	{
		wglSwapIntervalEXT(enableVerticalSync ? 1 : 0);
		g_enableVerticalSync = enableVerticalSync;
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
	if (GetCapture() == hWnd){
		// Hide the mouse pointer
		SetCursor(NULL);

		// Retrieve the cursor position
		GetCursorPos(&CursorPos);

		// Calculate mouse rotational values
		X = (float)(g_OldCursorPos.x - CursorPos.x) * 0.1;
		Y = (float)(g_OldCursorPos.y - CursorPos.y) * 0.1;

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos(g_OldCursorPos.x, g_OldCursorPos.y);

		if (Direction > 0 || X != 0.0f || Y != 0.0f){
			// Rotate camera
			if (X || Y){
				camera->rotate(X, Y, 0.0f);

			} // End if any rotation

			if (Direction) {

				float dx = 0, dy = 0, dz = 0, speed = 0.3;

				if (Direction & DIR_FORWARD) dz = speed;
				if (Direction & DIR_BACKWARD) dz = -speed;
				if (Direction & DIR_LEFT) dx = -speed;
				if (Direction & DIR_RIGHT) dx = speed;
				if (Direction & DIR_UP) dy = speed;
				if (Direction & DIR_DOWN) dy = -speed;

				camera->move(dx, dy, dz);
			}

		}// End if any movement
	} // End if Captured
}

void render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	objects[1]->m_model->rotate(Vector3f(1.0, 0.0, 0.0).normalize(), degree);
	for (int j = 0; j < objects.size(); j++) {

		modelView = objects[j]->m_model->getTransformationMatrix() * camera->getViewMatrix() ;
		if (rotate) {
			modelLight.rotate(Vector3f(0.0, 1.0, 0.0), -0.3);
		}


		if (j != objects.size() - 1) {
			objects[j]->m_model->rotate(Vector3f(0.0, 1.0, 0.0).normalize(), degree);
		}


		for (int i = 0; i < objects[j]->m_model->numberOfMeshes(); i++) {

			glUseProgram(objects[j]->m_shader[i]->m_program);

			objects[j]->m_shader[i]->loadFloat("u_heighScale", heighscale);
			objects[j]->m_shader[i]->loadFloat("u_tile", tile);
			objects[j]->m_shader[i]->loadFloat2("active", active);
			objects[j]->m_shader[i]->loadBool("u_shadow", shadow);

			objects[j]->m_shader[i]->loadMatrix("u_modelView", modelView);
			objects[j]->m_shader[i]->loadMatrix("u_model", objects[j]->m_model->getTransformationMatrix());
			objects[j]->m_shader[i]->loadMatrix("u_normalMatrix", Matrix4f::getNormalMatrix(modelView));
			objects[j]->m_shader[i]->loadMatrix("u_modelViewLight", modelLight.getTransformationMatrix() * camera->getViewMatrix());

			objects[j]->m_shader[i]->loadMatrix("u_view", camera->getViewMatrix());


			glBindBuffer(GL_ARRAY_BUFFER, objects[j]->m_model->getMesches()[i]->getVertexName());


			objects[j]->m_shader[i]->bindAttributes(objects[j]->m_model->getMesches()[i], NULL);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objects[j]->m_model->getMesches()[i]->getIndexName());
			glDrawElements(GL_TRIANGLES, objects[j]->m_model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			objects[j]->m_shader[i]->unbindAttributes(objects[j]->m_model->getMesches()[i]);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glUseProgram(0);
		}

	}

	skyBox->render();


}


