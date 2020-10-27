#define NOMINMAX

#include <windows.h>			
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <functional>

#include "Camera.h"
#include "Extension.h"
#include "Model.h"
#include "Shader.h"
#include "Bitmap.h"
#include "Quad.h"


int height = 582;
int width = 732;
unsigned int gPositionScreen, gPositionWorld, gNormal, gTexCoord, gBuffer;
unsigned int deferredPosition, deferredNormal, deferredTexCoord, deferredBuffer;
unsigned int dipoleFbo, dipoleTexture;
unsigned int sampleVAO;
unsigned int sampleIndexSize;
unsigned int sampleVBuf, sampleIBuf;

float *(*minDepthImage);
float *(*maxDepthImage);
Vector3f *(*posImage);
Vector3f *(*normalImage);
Vector3f *(*texcoordImage);

POINT g_OldCursorPos;
bool g_enableVerticalSync;
Model* dragon;
Shader *shader, *renderShader, *dipoleShader, *gbufShader;
Quad *quad;

static const Vector3f lightPos = Vector3f(-3.0f, 4.0f, 5.0f);

enum DIRECTION {
	DIR_FORWARD = 1,
	DIR_BACKWARD = 2,
	DIR_LEFT = 4,
	DIR_RIGHT = 8,
	DIR_UP = 16,
	DIR_DOWN = 32,

	DIR_FORCE_32BIT = 0x7FFFFFFF
};

struct Sample {
	Vector3f position;
	Vector3f normal;
	Vector2f texcoord;
	float radius;
};

bool toggleDiffuse = true;
bool toggleTransmission = true;



static constexpr int SAMPLE_POSITION_LOC = 0;
static constexpr int SAMPLE_TEXCOORD_LOC = 1;
static constexpr int SAMPLE_NORMAL_LOC = 2;
static constexpr int SAMPLE_RADIUS_LOC = 3;

Camera *camera;

//prototype funktions
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParma, LPARAM lParam);
void setCursortoMiddle(HWND hwnd);
void processInput(HWND hWnd);
void render();
void calcGBuffers();
void initApp(HWND hWnd);
void enableVerticalSync(bool enableVerticalSync);

// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	Vector3f camPos(0.0f, 0.0f, 3.0f);	
	Vector3f target(0.0f, 0.0f, 0.0f);
	Vector3f up(0.0, 1.0, 0.0);
	camera = new Camera(camPos, target, up);

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle(L"Debug console");

	MoveWindow(GetConsoleWindow(), 740, 0, 550, 200, true);
	std::cout << "w, a, s, d, mouse : move camera" << std::endl;
	std::cout << "space             : release capture" << std::endl;
	std::cout << "z                 : toggle reflection" << std::endl;
	std::cout << "t                 : toggle transmission" << std::endl;

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
	windowClass.lpszClassName = L"WINDOWCLASS";
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);			// windows logo small icon

																// register the windows class
	if (!RegisterClassEx(&windowClass))
		return 0;

	// class registered, so now create our window
	hwnd = CreateWindowEx(NULL,									// extended style
		L"WINDOWCLASS",						// class name
		L"OBJViewer",						// app name
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

	minDepthImage = (float**)malloc(1024 * sizeof(float*));
	for (int j = 0; j < 1024; j++) {
		minDepthImage[j] = (float *)malloc(1024 * 1 * sizeof(float));
	}

	maxDepthImage = (float**)malloc(1024 * sizeof(float*));
	for (int j = 0; j < 1024; j++) {
		maxDepthImage[j] = (float *)malloc(1024 * 1 * sizeof(float));
	}

	posImage = (Vector3f**)malloc(1024 * sizeof(Vector3f*));
	for (int j = 0; j < 1024; j++) {
		posImage[j] = (Vector3f*)malloc(1024 * sizeof(Vector3f));
	}

	normalImage = (Vector3f**)malloc(1024 * sizeof(Vector3f*));
	for (int j = 0; j < 1024; j++) {
		normalImage[j] = (Vector3f*)malloc(1024 * sizeof(Vector3f));
	}

	texcoordImage = (Vector3f**)malloc(1024 * sizeof(Vector3f*));
	for (int j = 0; j < 1024; j++) {
		texcoordImage[j] = (Vector3f*)malloc(1024 * sizeof(Vector3f));
	}

	initApp(hwnd);


	// main message loop
	while (true) {

		// Did we recieve a message, or are we idling ?
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			// test if this is a quit
			if (msg.message == WM_QUIT) break;
			// translate and dispatch message
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}else {

			render();
			processInput(hwnd);
			hdc = GetDC(hwnd);
			SwapBuffers(hdc);
			ReleaseDC(hwnd, hdc);
		}
	} // end while

	return msg.wParam;
}


// the Windows Procedure event handler
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static HGLRC hRC;					// rendering context
	static HDC hDC;						// device context
	int width, height;					// window width and height
	POINT pt;
	RECT rect;

	switch (message){
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
			case 'Z': {
				toggleDiffuse = !toggleDiffuse;
				return 0;
			}break;
			case 'T': {
				toggleTransmission = !toggleTransmission;
				return 0;
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
			camera->perspective(45.0f, (GLfloat)_width / (GLfloat)_height, 1.0f, 2000.0f);


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


	camera->perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	dragon = new Model();
	dragon->loadObject("objs/dragon.obj", Vector3f(0.0, 0.0, 0.0), 1.0);

	//Bind buffer
	for (int i = 0; i < dragon->numberOfMeshes(); i++) {
		glGenBuffers(1, &dragon->getMesches()[i]->m_vertexName);
		glBindBuffer(GL_ARRAY_BUFFER, dragon->getMesches()[i]->m_vertexName);
		glBufferData(GL_ARRAY_BUFFER, dragon->getMesches()[i]->getNumberOfVertices() * 4, dragon->getMesches()[i]->getVertexBuffer(), GL_STATIC_DRAW);

		glGenBuffers(1, &dragon->getMesches()[i]->m_indexName);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dragon->getMesches()[i]->m_indexName);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, dragon->getMesches()[i]->getNumberOfIndices() * 4, dragon->getMesches()[i]->getIndexBuffer(), GL_STATIC_DRAW);
	}

	renderShader = new Shader("shader/render.vs", "shader/render.fs");
	dipoleShader = new Shader("shader/dipole.vs", "shader/dipole.fs", "shader/dipole.gs");
	gbufShader = new Shader("shader/gbuffers.vs", "shader/gbuffers.fs");
	quad = new Quad();
	
	glGenTextures(1, &deferredPosition);
	glBindTexture(GL_TEXTURE_2D, deferredPosition);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &deferredNormal);
	glBindTexture(GL_TEXTURE_2D, deferredNormal);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &deferredTexCoord);
	glBindTexture(GL_TEXTURE_2D, deferredTexCoord);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenFramebuffers(1, &deferredBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredBuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, deferredPosition, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, deferredNormal, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, deferredTexCoord, 0);

	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = {GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(4, attachments);

	// buffer for depth
	unsigned int rbDepth;
	glGenRenderbuffers(1, &rbDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbDepth);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenTextures(1, &dipoleTexture);
	glBindTexture(GL_TEXTURE_2D, dipoleTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &dipoleFbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dipoleFbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dipoleTexture, 0);

	GLenum bufs[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, bufs);

	// buffer for depth
	unsigned int rbDepthDef;
	glGenRenderbuffers(1, &rbDepthDef);
	glBindRenderbuffer(GL_RENDERBUFFER, rbDepthDef);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbDepthDef);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(dipoleShader->m_program);
		glUniform1i(glGetUniformLocation(dipoleShader->m_program, "uPositionMap"), 0);
		glUniform1i(glGetUniformLocation(dipoleShader->m_program, "uNormalMap"), 1);
		glUniform1i(glGetUniformLocation(dipoleShader->m_program, "uTexCoordMap"), 2);
	glUseProgram(0);

	glUseProgram(renderShader->m_program);
		glUniform1i(glGetUniformLocation(renderShader->m_program, "uTransMap"), 0);
	glUseProgram(0);

	calcGBuffers();

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
			// Rotate camera
			if (X || Y) {
				camera->rotate(X, Y, 0.0f);

			} // End if any rotation

			if (Direction) {

				float dx = 0, dy = 0, dz = 0, speed = 0.03;

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

	// Compute deferred shading buffers.
	Matrix4f proj, view, model;
	proj.perspective(45.0f, (float)width/height, 1.0f, 1000.0f);
	view.lookAt(Vector3f(0.0f, 0.0f, 3.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.1f, 0.0f));
	model.scale(5.0f, 5.0f, 5.0f);

	Matrix4f mvp = model * camera->getViewMatrix() * camera->getProjectionMatrix();
	Matrix4f mv  = model * camera->getViewMatrix();

	glUseProgram(gbufShader->m_program);
	glBindFramebuffer(GL_FRAMEBUFFER, deferredBuffer);

	gbufShader->loadMatrix("uMVPMat", mvp, true);
	gbufShader->loadFloat("isMaxDepth", 0);

	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0,  GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < dragon->numberOfMeshes(); i++) {

		glBindBuffer(GL_ARRAY_BUFFER, dragon->getMesches()[i]->getVertexName());

		gbufShader->bindAttributes(dragon->getMesches()[i], NULL);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dragon->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, dragon->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		gbufShader->unbindAttributes(dragon->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	
	// Translucent part.
	glUseProgram(dipoleShader->m_program);
	glBindFramebuffer(GL_FRAMEBUFFER, dipoleFbo);

	glBindVertexArray(sampleVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deferredPosition);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferredNormal);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferredTexCoord);
	

	dipoleShader->loadMatrix("uMVPMat", mvp, true);
	dipoleShader->loadMatrix("uMVMat", mv, true);
	dipoleShader->loadVector("uLightPos", lightPos);

	dipoleShader->loadVector("sigma_a", Vector3f(0.0015333, 0.0046, 0.019933) * 50.0f);
	dipoleShader->loadVector("sigmap_s", Vector3f(4.5513, 5.8294, 7.136) * 50.0f);
	dipoleShader->loadFloat("eta", 1.3f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDrawElements(GL_POINTS, sampleIndexSize, GL_UNSIGNED_INT, 0);
	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glEnable(GL_DEPTH_TEST);

	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Main rendering.
	glUseProgram(renderShader->m_program);

	renderShader->loadMatrix("uMVPMat", mvp, true);
	renderShader->loadMatrix("uMVMat", mv, true);
	renderShader->loadVector("uLightPos", lightPos);

	renderShader->loadMatrix("u_projection", proj);
	renderShader->loadMatrix("u_modelView", mv);

	renderShader->loadFloat("refFactor", toggleDiffuse ? 1.0f : 0.0f);
	renderShader->loadFloat("transFactor", toggleTransmission ? 1.0f : 0.0f);

	for (int i = 0; i < dragon->numberOfMeshes(); i++) {

		glBindBuffer(GL_ARRAY_BUFFER, dragon->getMesches()[i]->getVertexName());

		renderShader->bindAttributes(dragon->getMesches()[i], NULL);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, dipoleTexture);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dragon->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, dragon->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		renderShader->unbindAttributes(dragon->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glUseProgram(0);

	//debug fbo textures
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//quad->render(gPositionScreen);
	//quad->render(gPositionWorld);
	//quad->render(gNormal);
	//quad->render(gTexCoord);

	//quad->render(deferredPosition);
	//quad->render(deferredNormal);
	//quad->render(deferredTexCoord);

	quad->render(dipoleTexture);*/
}

void takeFloatImage(unsigned int fbo, int _width, int _height, float *(*&image), int channels, int attachmentIndex) {
	const int width = _width;
	const int height = _height;

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	std::vector<float> pixels(width * height * 4);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, &pixels[0]);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			for (int ch = 0; ch < channels; ch++) {
				image[height - y - 1][x * channels + ch] = pixels[(y * width + x) * 4 + ch];
			}
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void takeFloatImageVec(unsigned int fbo, int _width, int _height, Vector3f *(*&image), int channels, int attachmentIndex) {
	const int width = _width;
	const int height = _height;

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	std::vector<float> pixels(width * height * 4);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, &pixels[0]);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			for (int ch = 0; ch < channels; ch++) {
				image[height - y - 1][x][ch] = pixels[(y * width + x) * 4 + ch];
				//image[height - y - 1][x * channels + ch] = pixels[(y * width + x) * 4 + ch];
			}
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

template <class T>
void pyrDown(T *(*lower), T **&upper, const std::function<T(T, T, T, T)>& f, int height, int width) {
	upper = (T**)malloc(height / 2 * sizeof(T*));
	for (int j = 0; j < height / 2; j++) {
		upper[j] = (T *)malloc(width / 2 * sizeof(T));
	}

	for (int y = 0; y < height / 2; y++) {
		for (int x = 0; x < width / 2; x++) {
			T& t0 = lower[y * 2][x * 2];
			T& t1 = lower[y * 2][x * 2 + 1];
			T& t2 = lower[y * 2 + 1][x * 2];
			T& t3 = lower[y * 2 + 1][x * 2 + 1];
			*(*(upper + y) + x) = f(t0, t1, t2, t3);
		}
	}
}

void calcGBuffers() {

	static const int bufSize = 1024;	
	//////////////////////////////////////////////////////////gbuffer/////////////////////////////////////////
	glGenTextures(1, &gPositionScreen);
	glBindTexture(GL_TEXTURE_2D, gPositionScreen);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, bufSize, bufSize, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &gPositionWorld);
	glBindTexture(GL_TEXTURE_2D, gPositionWorld);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, bufSize, bufSize, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, bufSize, bufSize, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &gTexCoord);
	glBindTexture(GL_TEXTURE_2D, gTexCoord);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, bufSize, bufSize, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionScreen, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gPositionWorld, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gTexCoord, 0);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	// buffer for depth
	unsigned int rbDepth;
	glGenRenderbuffers(1, &rbDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, bufSize, bufSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbDepth);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	Matrix4f proj, view, model;
	proj.perspective(45.0f, 1.0f, 0.1f, 100.0f);
	view.lookAt(lightPos, Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	model.scale(7.0f, 7.0f, 7.0f);

	Matrix4f mvp = model * view * proj;

	glViewport(0, 0, bufSize, bufSize);
	glUseProgram(gbufShader->m_program);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	gbufShader->loadMatrix("uMVPMat", mvp, true);
	gbufShader->loadFloat("isMaxDepth", 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glClearBufferfv(GL_COLOR, 0, white);

	for (int i = 0; i < dragon->numberOfMeshes(); i++) {

		glBindBuffer(GL_ARRAY_BUFFER, dragon->getMesches()[i]->getVertexName());

		gbufShader->bindAttributes(dragon->getMesches()[i], NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dragon->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, dragon->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		gbufShader->unbindAttributes(dragon->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glViewport(0, 0, width, height);

	takeFloatImage(gBuffer, 1024, 1024, minDepthImage, 1, 0);
	takeFloatImageVec(gBuffer, 1024, 1024, posImage, 3, 1);
	takeFloatImageVec(gBuffer, 1024, 1024, normalImage, 3, 2);
	takeFloatImageVec(gBuffer, 1024, 1024, texcoordImage, 3, 3);


	glViewport(0, 0, bufSize, bufSize);
	glUseProgram(gbufShader->m_program);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	gbufShader->loadFloat("isMaxDepth", 1);
	GLenum bufs[] = { GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(1, bufs);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	white[1] = 0.0f; white[2] = 0.0f;
	glClearBufferfv(GL_COLOR, 0, white);

	for (int i = 0; i < dragon->numberOfMeshes(); i++) {

		glBindBuffer(GL_ARRAY_BUFFER, dragon->getMesches()[i]->getVertexName());

		gbufShader->bindAttributes(dragon->getMesches()[i], NULL);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dragon->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, dragon->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		gbufShader->unbindAttributes(dragon->getMesches()[i]);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glViewport(0, 0, width, height);

	takeFloatImage(gBuffer, 1024, 1024, maxDepthImage, 1, 4);

	// Compute sample point hierarchy.
	static const int maxPyrLevels = 3;
	std::vector<float **> minDepthPyr(maxPyrLevels);
	std::vector<float **> maxDepthPyr(maxPyrLevels);
	std::vector<Vector3f**> positionPyr(maxPyrLevels);
	std::vector<Vector3f**> normalPyr(maxPyrLevels);
	std::vector<Vector3f**> texCoordPyr(maxPyrLevels);

	minDepthPyr[maxPyrLevels - 1] = minDepthImage;
	maxDepthPyr[maxPyrLevels - 1] = maxDepthImage;
	positionPyr[maxPyrLevels - 1] = posImage;
	normalPyr[maxPyrLevels - 1] = normalImage;
	texCoordPyr[maxPyrLevels - 1] = texcoordImage;

	std::function<float(float, float, float, float)> fTakeMin =
		[&](float f1, float f2, float f3, float f4) {
		return std::min(std::min(f1, f2), std::min(f3, f4));
	};
	std::function<float(float, float, float, float)> fTakeMax =
		[&](float f1, float f2, float f3, float f4) {
		return std::max(std::max(f1, f2), std::max(f3, f4));
	};
	std::function<Vector3f(Vector3f, Vector3f, Vector3f, Vector3f)> fTakeAvg =
		[&](Vector3f v1, Vector3f v2, Vector3f v3, Vector3f v4) {

		return Vector3f((v1[0] + v2[0] + v3[0] + v4[0]) / 4.0f,
			(v1[1] + v2[1] + v3[1] + v4[1]) / 4.0f,
			(v1[2] + v2[2] + v3[2] + v4[2]) / 4.0f);
	};

	for (int i = maxPyrLevels - 1, k = 1; i >= 1; i--, k = k * 2) {
		pyrDown(minDepthPyr[i], minDepthPyr[i - 1], fTakeMin, 1024 / k, 1024 / k);
		pyrDown(maxDepthPyr[i], maxDepthPyr[i - 1], fTakeMax, 1024 / k, 1024 / k);
		pyrDown(positionPyr[i], positionPyr[i - 1], fTakeAvg, 1024 / k, 1024 / k);
		pyrDown(normalPyr[i], normalPyr[i - 1], fTakeAvg, 1024 / k, 1024 / k);
		pyrDown(texCoordPyr[i], texCoordPyr[i - 1], fTakeAvg, 1024 / k, 1024 / k);

	}

	static const double alpha = 30.0;
	static const double Rw = 1.0;
	static const double RPx = 0.1;
	static const double z0 = 0.03;
	double T = 256.0;

	std::vector<unsigned char **> samplePyr(maxPyrLevels);
	for (int l = 0, k = 4; l < maxPyrLevels; l++, k = k / 2) {

		const int subRows = 1024 / k;
		const int subCols = 1024 / k;

		samplePyr[l] = (unsigned char **)malloc(subRows * sizeof(unsigned char *));
		for (int j = 0; j <subCols; j++) {
			samplePyr[l][j] = (unsigned char *)malloc(subCols * sizeof(unsigned char));
		}

		for (int i = 0; i < subRows; i++) {
			for (int j = 0; j < subCols; j++) {
				float depthGap = (maxDepthPyr[l][i][j] - minDepthPyr[l][i][j]) * 10.0f;

				Vector3f pos = positionPyr[l][i][j];
				Vector3f L = (lightPos - pos).normalize();
				Vector3f nrm = normalPyr[l][i][j];
				
				double Mx = alpha * Rw / (RPx * std::abs(Vector3f::dot(nrm, L)));

				//adaptive sample selection
				if (depthGap < z0 && T > Mx) {
					samplePyr[l][i][j] = 255;
				}else {
					samplePyr[l][i][j] = 0;
				}
			}
		}
		T *= 2.0;
	}

	for (int l = maxPyrLevels - 1, k = 1; l >= 1; l--, k = k * 2) {

		const int subRows = 1024 / k;
		const int subCols = 1024 / k;

		for (int y = 0; y < subRows; y++) {
			for (int x = 0; x < subCols; x++) {
				if (samplePyr[l - 1][y / 2][x / 2] != 0) {
					samplePyr[l][y][x] = 0;
				}
			}
		}
	}

	std::vector<Sample> samples;
	std::vector<unsigned int> sampleIds;
	for (int l = 0, k = 4; l < maxPyrLevels; l++, k = k / 2) {

		const int subRows = 1024 / k;
		const int subCols = 1024 / k;

		for (int y = 0; y < subRows; y++) {
			for (int x = 0; x < subCols; x++) {
				if (samplePyr[l][y][x] != 0) {
					Sample samp;

					Vector3f pos = positionPyr[l][y][x];
					samp.position = pos;

					Vector3f nrm = normalPyr[l][y][x];
					samp.normal = nrm;

					Vector3f crd = texCoordPyr[l][y][x];
					samp.texcoord = Vector2f(crd[0], crd[1]);

					samp.radius = std::pow(0.5, l);
					samples.push_back(samp);
					sampleIds.push_back(sampleIds.size());
				}
			}
		}
	}

	sampleIndexSize = sampleIds.size();
	glGenVertexArrays(1, &sampleVAO);
	glBindVertexArray(sampleVAO);

		glGenBuffers(1, &sampleVBuf);
		glBindBuffer(GL_ARRAY_BUFFER, sampleVBuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Sample) * samples.size(), &samples[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(SAMPLE_POSITION_LOC);
		glEnableVertexAttribArray(SAMPLE_NORMAL_LOC);
		glEnableVertexAttribArray(SAMPLE_TEXCOORD_LOC);
		glEnableVertexAttribArray(SAMPLE_RADIUS_LOC);
		glVertexAttribPointer(SAMPLE_POSITION_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Sample), (void*)0);
		glVertexAttribPointer(SAMPLE_NORMAL_LOC, 3, GL_FLOAT, GL_FALSE, sizeof(Sample), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(SAMPLE_TEXCOORD_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(Sample), (void*)(sizeof(float) * 6));
		glVertexAttribPointer(SAMPLE_RADIUS_LOC, 1, GL_FLOAT, GL_FALSE, sizeof(Sample), (void*)(sizeof(float) * 8));

		glGenBuffers(1, &sampleIBuf);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sampleIBuf);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sampleIds.size() * sizeof(unsigned int), &sampleIds[0], GL_STATIC_DRAW);		
	glBindVertexArray(0);

}