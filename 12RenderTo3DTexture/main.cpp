#include <windows.h>			
#include <iostream>
#include <fstream>
#include <vector>

#include "GL.h"
#include "Extension.h"
#include "Camera.h";

int height = 600;
int width = 800;

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

GLuint u_projection, u_view, u_model, u_modelView, u_texture, positionID, colorID, texCoordID;
GLuint vboCube, indexCube, vboCubeColor, vboCubeTexcoord, textureID, slicedCubeId;
GLuint program;

Camera *camera;
float time = 0;
//prototype funktions
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParma, LPARAM lParam);
void setCursortoMiddle(HWND hwnd);
void ProcessInput(HWND hWnd );


void InitApp(HWND hWnd);
void ReadTextFile(const char *pszFilename, std::string &buffer);
void ReadRawFile(const char *pszFilename, int width, int height, int slices);
void EnableVerticalSync(bool enableVerticalSync);

GLuint LoadShaderProgram(GLenum type, const char *pszFilename);
GLuint CompileShader(GLenum type, const char *pszSource);
GLuint LinkShaders(GLuint vertShader, GLuint fragShader);
Matrix4f view, model;

GLuint createProgram();
void Cleanup();


void createSlicedCube();
// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	

	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);
	SetConsoleTitle("Debug console");
	MoveWindow(GetConsoleWindow(), 790, 0, 500, 200, true);

	WNDCLASSEX		windowClass;		// window class
	HWND			hwnd;				// window handle
	MSG				msg;				// message
	HDC				hdc;				// device context handle

	// fill out the window class structure
	windowClass.cbSize			= sizeof(WNDCLASSEX);
	windowClass.style			= CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc		= WinProc;
	windowClass.cbClsExtra		= 0;
	windowClass.cbWndExtra		= 0;
	windowClass.hInstance		= hInstance;
	windowClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);		// default icon
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);			// default arrow
	windowClass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// white background
	windowClass.lpszMenuName	= NULL;									// no menu
	windowClass.lpszClassName	= "WINDOWCLASS";
	windowClass.hIconSm			= LoadIcon(NULL, IDI_WINLOGO);			// windows logo small icon

	// register the windows class
	if (!RegisterClassEx(&windowClass))
		return 0;

	// class registered, so now create our window
	hwnd = CreateWindowEx(NULL,									// extended style
						  "WINDOWCLASS",						// class name
						  "Cube GlSl",							// app name
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

	InitApp(hwnd);
	

	// main message loop
	while (true) 
    {
        // Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
        {
			// test if this is a quit
			if (msg.message == WM_QUIT) break;
			// translate and dispatch message
			TranslateMessage( &msg );
			DispatchMessage ( &msg );

		} else {
			
			
			// do rendering here
		
			ProcessInput(hwnd);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER, 0.05f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glUseProgram(program);
			view = camera->getViewMatrix() ;
			glUniformMatrix4fv(u_modelView, 1, true, &(view *model)[0][0]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_3D, textureID);
			glUniform1i(u_texture, 0);

			glBindBuffer(GL_ARRAY_BUFFER, slicedCubeId);

			glEnableVertexAttribArray(positionID);
			glVertexAttribPointer(
				positionID,				// attribute
				3,						// number of elements per vertex, here (x,y,z)
				GL_FLOAT,				// the type of each element
				GL_FALSE,				// take our values as-is
				6 * sizeof(float),      // no extra data between each position
				(void*)0				// offset of first element
				);

			glEnableVertexAttribArray(texCoordID);
			
			glVertexAttribPointer(
				texCoordID,						// attribute
				3,								// number of elements per vertex, here (x,y)
				GL_FLOAT,						// the type of each element
				GL_FALSE,						// take our values as-is
				6 * sizeof(float),              // stride
				(void*)(3 * sizeof(float))      // offset of first element
				);
			

			glDrawArrays(GL_QUADS, 0, 4 * 200);

			glDisableVertexAttribArray(positionID);
			glDisableVertexAttribArray(texCoordID);

			glUseProgram(0);

			hdc=GetDC(hwnd);
			SwapBuffers(hdc);			// bring backbuffer to foreground
			ReleaseDC(hwnd,hdc);
		} // end If messages waiting
    } // end while

	Cleanup();
	return msg.wParam;
}

// the Windows Procedure event handler
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HGLRC hRC;					// rendering context
	static HDC hDC;						// device context
	int width, height;					// window width and height
	POINT pt;
	RECT rect;

	switch(message)
	{
		case WM_DESTROY:{

			PostQuitMessage(0);
			return 0;
		}

		case WM_CREATE:{
			
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

		case WM_LBUTTONDOWN:{ // Capture the mouse

			setCursortoMiddle(hWnd);
			SetCapture(hWnd);
			
			return 0;
		} break;

		case WM_KEYDOWN:{

			switch (wParam){

				case VK_ESCAPE:{

					PostQuitMessage(0);
					return 0;

				}break;
				case VK_SPACE: {

					ReleaseCapture();
					return 0;

				}break;
				case 'v': case 'V':{

					EnableVerticalSync(!g_enableVerticalSync);
				    return 0;
					
				}break;

				return 0;
			}break;

			return 0;
		}break;
		case WM_SIZE:{

			int height2 = HIWORD(lParam);		// retrieve width and height
		    int width2 = LOWORD(lParam);
			
			if (height2 == 0){					// avoid divide by zero
				
				height2 = 1;
			}

			glViewport(0, 0, width2, height2);
			return 0;

		}break;
		default:
		 break;
    }
	return (DefWindowProc(hWnd, message, wParam, lParam));
}

void InitApp(HWND hWnd)
{
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
	EnableVerticalSync(true);

	Vector3f camPos{ 0, 0, 5.0f };
	Vector3f xAxis{ 1, 0, 0 };
	Vector3f yAxis{ 0, 1, 0 };
	Vector3f zAxis{ 0, 0, 1 };
	Vector3f target(0.0, 0.0, 0.0);
	Vector3f up(0.0, 1.0, 0.0);

	camera = new Camera(camPos, xAxis, yAxis, zAxis, target, up);

	camera->perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 8000.0f);
	

	glClearColor(0.5f, 0.5f, 1.0f, 0.0f);
	
	program = createProgram();
	createSlicedCube();
	ReadRawFile("head256x256x109.raw",256, 256, 109);

	u_projection = glGetUniformLocation(program, "u_projection");
	u_view = glGetUniformLocation(program, "u_view");
	u_modelView = glGetUniformLocation(program, "u_modelView");
	u_texture = glGetUniformLocation(program, "u_texture");
	positionID = glGetAttribLocation(program, "position");
	colorID = glGetAttribLocation(program, "color");
	texCoordID = glGetAttribLocation(program, "texCoords");

	glUseProgram(program);
	glUniformMatrix4fv(u_projection, 1, true, &camera->getProjectionMatrix()[0][0]);
	glUseProgram(0);

	Matrix4f m1;
	m1.rotate(Vector3f(1.0f, 0.0f, 0.0f), 180);
	Matrix4f m2;
	m2.rotate(Vector3f(0.0f, 1.0f, 0.0f), 180);
	model = m1 * m2;
}

void setCursortoMiddle(HWND hwnd){

	RECT rect;

	GetClientRect(hwnd, &rect);
	SetCursorPos(rect.right / 2, rect.bottom / 2);

}
void EnableVerticalSync(bool enableVerticalSync){

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

void ReadTextFile(const char *pszFilename, std::string &buffer){

	std::ifstream file(pszFilename, std::ios::binary);

	if (file.is_open())
	{
		file.seekg(0, std::ios::end);

		std::ifstream::pos_type fileSize = file.tellg();

		buffer.resize(static_cast<unsigned int>(fileSize));
		file.seekg(0, std::ios::beg);
		file.read(&buffer[0], fileSize);
	}
}

GLuint LoadShaderProgram(GLenum type, const char *pszFilename){

	GLuint shader = 0;
	std::string buffer;
	ReadTextFile(pszFilename, buffer);

	if (buffer.length() > 0)
	{


		shader = CompileShader(type, reinterpret_cast<const char *>(&buffer[0]));
	}

	return shader;
}

GLuint CompileShader(GLenum type, const char *pszSource){

	GLuint shader = glCreateShader(type);

	//std::cout << pszSource << std::endl;

	if (shader)
	{
		GLint compiled = 0;

		glShaderSource(shader, 1, &pszSource, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			GLsizei infoLogSize = 0;
			std::string infoLog;

			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogSize);
			infoLog.resize(infoLogSize);
			glGetShaderInfoLog(shader, infoLogSize, &infoLogSize, &infoLog[0]);
			std::cout << "Compile status: \n" << &infoLog << std::endl;

		}

	}
	return shader;
}

GLuint LinkShaders(GLuint vertShader, GLuint fragShader){

	GLuint program = glCreateProgram();

	if (program)
	{
		GLint linked = 0;

		if (vertShader)
			glAttachShader(program, vertShader);

		if (fragShader)
			glAttachShader(program, fragShader);

		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &linked);

		if (!linked)
		{
			GLsizei infoLogSize = 0;
			std::string infoLog;

			glGetShaderiv(program, GL_INFO_LOG_LENGTH, &infoLogSize);
			infoLog.resize(infoLogSize);
			glGetShaderInfoLog(program, infoLogSize, &infoLogSize, &infoLog[0]);
			std::cout << "Compile status: \n" << &infoLog << std::endl;
		}

		// Mark the two attached shaders for deletion. These two shaders aren't
		// deleted right now because both are already attached to a shader
		// program. When the shader program is deleted these two shaders will
		// be automatically detached and deleted.

		if (vertShader)
			glDeleteShader(vertShader);

		if (fragShader)
			glDeleteShader(fragShader);

	}

	return program;
}



GLuint createProgram() {


	GLuint vshader = LoadShaderProgram(GL_VERTEX_SHADER, "program.vert");
	GLuint fshader = LoadShaderProgram(GL_FRAGMENT_SHADER, "program.fraq");

	return LinkShaders(vshader, fshader);

}

void Cleanup(){


	if (program)
	{
		glDeleteProgram(program);
		program = 0;
	}

}


void ProcessInput(HWND hWnd )
{
    static UCHAR pKeyBuffer[ 256 ];
    ULONG        Direction = 0;
    POINT        CursorPos;
    float        X = 0.0f, Y = 0.0f;

    // Retrieve keyboard state
    if ( !GetKeyboardState( pKeyBuffer ) ) return;

	// Check the relevant keys
    if ( pKeyBuffer['W'] & 0xF0 ) Direction |= DIR_FORWARD;
    if ( pKeyBuffer['S'] & 0xF0 ) Direction |= DIR_BACKWARD;
    if ( pKeyBuffer['A'] & 0xF0 ) Direction |= DIR_LEFT;
    if ( pKeyBuffer['D'] & 0xF0 ) Direction |= DIR_RIGHT;
    if ( pKeyBuffer['E'] & 0xF0 ) Direction |= DIR_UP;
    if ( pKeyBuffer['Q'] & 0xF0 ) Direction |= DIR_DOWN;

	 // Now process the mouse (if the button is pressed)
    if ( GetCapture() == hWnd )
    {
        // Hide the mouse pointer
        SetCursor( NULL );

        // Retrieve the cursor position
        GetCursorPos( &CursorPos );

        // Calculate mouse rotational values
		X = (float)(g_OldCursorPos.x - CursorPos.x) * 0.1;
		Y = (float)(g_OldCursorPos.y - CursorPos.y) * 0.1;

        // Reset our cursor position so we can keep going forever :)
        SetCursorPos( g_OldCursorPos.x, g_OldCursorPos.y );

		if ( Direction > 0 || X != 0.0f || Y != 0.0f )
		{
			// Rotate camera
			if ( X || Y ) 
			{
				camera->rotate( X, Y, 0.0f );
            
        
			} // End if any rotation


			if ( Direction ) 
			{
				float dx=0,dy=0,dz=0;
				float speed = 0.1f;

				if (Direction & DIR_FORWARD) dz = +speed;
				if (Direction & DIR_BACKWARD) dz = -speed;
				if (Direction & DIR_LEFT) dx = -speed;
				if (Direction & DIR_RIGHT) dx = speed;
				if (Direction & DIR_UP) dy = speed;
				if (Direction & DIR_DOWN) dy = -speed;

				camera->move(dx,dy,dz);

			} 
	
		}// End if any movement
	} // End if Captured
}



void createSlicedCube(){

	std::vector <float> vertexBuffer;

	

	for (float fIndx = -1.0f; fIndx <= 1.0f; fIndx += 0.01f){
		
		vertexBuffer.push_back(-1.0), vertexBuffer.push_back(-1.0), vertexBuffer.push_back(fIndx);
		vertexBuffer.push_back(0.0), vertexBuffer.push_back(0.0), vertexBuffer.push_back((fIndx + 1.0f) / 2.0f);

		vertexBuffer.push_back(1.0), vertexBuffer.push_back(-1.0), vertexBuffer.push_back(fIndx);
		vertexBuffer.push_back(1.0), vertexBuffer.push_back(0.0), vertexBuffer.push_back((fIndx + 1.0f) / 2.0f);

		vertexBuffer.push_back(1.0), vertexBuffer.push_back(1.0), vertexBuffer.push_back(fIndx);
		vertexBuffer.push_back(1.0), vertexBuffer.push_back(1.0), vertexBuffer.push_back((fIndx + 1.0f) / 2.0f);

		vertexBuffer.push_back(-1.0), vertexBuffer.push_back(1.0), vertexBuffer.push_back(fIndx);
		vertexBuffer.push_back(0.0), vertexBuffer.push_back(1.0), vertexBuffer.push_back((fIndx + 1.0f) / 2.0f);
	}

	slicedCubeId = 0;
	glGenBuffers(1, &slicedCubeId);
	glBindBuffer(GL_ARRAY_BUFFER, slicedCubeId);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * 4, &vertexBuffer[0], GL_STATIC_DRAW);
}


void ReadRawFile(const char *pszFilename, int width, int height, int slices){

	FILE *filePtr;

	// open filename in "read binary" mode
	filePtr = fopen(pszFilename, "rb");
	if (filePtr == NULL)
		return;


	// File has only image data. The dimension of the data should be known.



	unsigned char *data = (unsigned char*)malloc(width*height*slices * 4);

	// verify memory allocation
	if (!data){
		free(data);
		fclose(filePtr);
		return;
	}

	fread(data, 1, width*height*slices, filePtr);


	unsigned char *RGBAdata = (unsigned char*)malloc(width*height*slices * 4);

	if (!RGBAdata){
		free(RGBAdata);
		fclose(filePtr);
		return;
	}

	// Convert the data to RGBA data.
	// Here we are simply putting the same value to R, G, B and A channels.
	// Usually for raw data, the alpha value will be constructed by a threshold value given by the user 

	for (int i = 0; i < width*height*slices; i++){
		RGBAdata[i * 4] = data[i];
		RGBAdata[i * 4 + 1] = data[i];
		RGBAdata[i * 4 + 2] = data[i];
		RGBAdata[i * 4 + 3] = data[i];
	}

	glGenTextures(1, (GLuint*)&textureID);

	glBindTexture(GL_TEXTURE_3D, textureID);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, slices, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, RGBAdata);
	glBindTexture(GL_TEXTURE_3D, 0);

	fclose(filePtr);
	return;
}
