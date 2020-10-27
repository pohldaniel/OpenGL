#include <windows.h>			
#include <iostream>
#include <fstream>

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

GLuint u_projection, u_view, u_time, positionID, colorID;
GLuint vboCube, indexCube, vboCubeColor;
GLuint program;


Camera *camera;
float time = 0;
//prototype funktions
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParma, LPARAM lParam);
void setCursortoMiddle(HWND hwnd);
void ProcessInput(HWND hWnd );


void InitApp(HWND hWnd);
void ReadTextFile(const char *pszFilename, std::string &buffer);
void EnableVerticalSync(bool enableVerticalSync);

GLuint LoadShaderProgram(GLenum type, const char *pszFilename);
GLuint CompileShader(GLenum type, const char *pszSource);
GLuint LinkShaders(GLuint vertShader, GLuint fragShader);

GLuint createProgram();
void Cleanup();

void createCube();
// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Vector3f camPos{ 0, 0, 5.0f };
	Vector3f target(0.0, 0.0, 0.0);
	Vector3f up(0.0, 1.0, 0.0);

	camera = new Camera(camPos, target, up);

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
	
	
	
	//camera.setPosition(terrain->widthHalf,0,terrain->lengthHalf);

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
		} 
        else 
        {
			
			
			// do rendering here
		
			ProcessInput(hwnd);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program);

			glUniformMatrix4fv(u_view, 1, true, &camera->getViewMatrix()[0][0]);

			glEnableVertexAttribArray(positionID);
			glBindBuffer(GL_ARRAY_BUFFER, vboCube);
			glVertexAttribPointer(
				positionID,			// attribute
				3,                  // number of elements per vertex, here (x,y)
				GL_FLOAT,           // the type of each element
				GL_FALSE,           // take our values as-is
				0,                  // no extra data between each position
				0                   // offset of first element
				);

			glEnableVertexAttribArray(colorID);
			glBindBuffer(GL_ARRAY_BUFFER, vboCubeColor);
			glVertexAttribPointer(
				colorID,			 // attribute
				3,                   // number of elements per vertex, here (r,g,b)
				GL_FLOAT,            // the type of each element
				GL_FALSE,            // take our values as-is
				0,					 // stride
				0					 // offset
				);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexCube);

			//number of triangles
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glDisableVertexAttribArray(colorID);
			glDisableVertexAttribArray(positionID);
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

	glMatrixMode(GL_PROJECTION);										// set projection matrix current matrix
	glLoadIdentity();													// reset projection matrix
	camera->perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 8000.0f);
	glLoadMatrixf(&camera->getProjectionMatrix()[0][0]);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	
	glEnable(GL_DEPTH_TEST);					// hidden surface removal
	glEnable(GL_CULL_FACE);						// do not calculate inside of poly's
	
	program = createProgram();
	createCube();

	u_projection = glGetUniformLocation(program, "u_projection");
	u_view = glGetUniformLocation(program, "u_view");
	u_time = glGetUniformLocation(program, "u_time");
	positionID = glGetAttribLocation(program, "position");
	colorID = glGetAttribLocation(program, "color");

	glUseProgram(program);
	glUniformMatrix4fv(u_projection, 1, true, &camera->getProjectionMatrix()[0][0]);
	//glUniformMatrix4fv(u_view, 1, true, &camera->getViewMatrix()[0][0]);
	glUseProgram(0);

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

				if (Direction & DIR_FORWARD) dz = +0.5f ;
				if (Direction & DIR_BACKWARD) dz = -0.5f;
				if (Direction & DIR_LEFT) dx = -0.5f ;
				if (Direction & DIR_RIGHT) dx = 0.6f ;
				if (Direction & DIR_UP) dy = 0.5f ;
				if (Direction & DIR_DOWN) dy = -0.5f ;

				camera->move(dx,dy,dz);

			} 
	
		}// End if any movement
	} // End if Captured
}

void createCube(){

	static const GLfloat vertex[] = {
		// front
		-1.0, -1.0, -4.0,
		1.0, -1.0, -4.0,
		1.0, 1.0, -4.0,
		-1.0, 1.0, -4.0,
		// back
		-1.0, -1.0, -6.0,
		1.0, -1.0, -6.0,
		1.0, 1.0, -6.0,
		-1.0, 1.0, -6.0,
	};

	vboCube = 0;
	glGenBuffers(1, &vboCube);
	glBindBuffer(GL_ARRAY_BUFFER, vboCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	static const GLfloat color[] = {
		// front colors
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0,
		// back colors
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 1.0, 1.0,

		
	};

	vboCubeColor = 0;
	glGenBuffers(1, &vboCubeColor);
	glBindBuffer(GL_ARRAY_BUFFER, vboCubeColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

	static const GLushort index[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3,
	};

	indexCube = 0;
	glGenBuffers(1, &indexCube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexCube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);


}