////// Includes
#include <windows.h>			// standard Windows app include
#include <math.h>
#include <iostream>
#include "GL.H"					// standard OpenGL include			
#include "Camera.h"

//// Global Variables

Camera *camera;
float	  angle;

POINT g_OldCursorPos;

enum DIRECTION {
	DIR_FORWARD = 1,
	DIR_BACKWARD = 2,
	DIR_LEFT = 4,
	DIR_RIGHT = 8,
	DIR_UP = 16,
	DIR_DOWN = 32,

	DIR_FORCE_32BIT = 0x7FFFFFFF
};
//prototype Callback-Funktion
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParma, LPARAM lParam);
void ProcessInput(HWND hWnd);
// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){

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
	windowClass.lpszClassName	= "MyClass";
	windowClass.hIconSm			= LoadIcon(NULL, IDI_WINLOGO);			// windows logo small icon

	// register the windows class
	if (!RegisterClassEx(&windowClass))
		return 0;

	
	// class registered, so now create our window
	hwnd = CreateWindowEx(NULL,									// extended style
						  "MyClass",							// class name
						  "Cube OpenGL",					// app name
						  WS_OVERLAPPEDWINDOW,
						  0, 0,									// x,y coordinate
						  800,
						  600,									// width, height
						  NULL,									// handle to parent
						  NULL,									// handle to menu
						  hInstance,							// application instance
						  NULL);								// no extra params

	// check if window creation failed (hwnd would equal NULL)
	if (!hwnd)
		return 0;

	ShowWindow(hwnd, SW_SHOW);			// display the window
	UpdateWindow(hwnd);					// update the windows
	
	


	
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
				
					//UpdateCamera(timer.GetTimeElapsed() );

//----------------------------RenderFrame()--------------------------------------------------
				ProcessInput(hwnd);
				
					glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glMatrixMode(GL_MODELVIEW);
					glLoadMatrixf(&camera->getViewMatrix()[0][0]);

					angle =angle +  0.4;
					if(angle>360) angle =0;

					glRotatef(angle, 0.0f,1.0f,0.0f);
		

					//giving different colors to different sides
					glBegin(GL_QUADS); // Start Drawing The Cube
					glColor3f(1,0,0); //red color
					glVertex3f(1.0f, 1.0f, -1.0f); // Top Right Of The Quad (Top)
					glVertex3f( -1.0f, 1.0f, -1.0f); // Top Left Of The Quad (Top)
					glVertex3f( -1.0f, 1.0f, 1.0f ); // Bottom Left Of The Quad (Top)
					glVertex3f( 1.0f, 1.0f, 1.0f ); // Bottom Right Of The Quad (Top)
		
					glColor3f( 0,1,0 ); //green color
					glVertex3f( 1.0f, -1.0f, 1.0f ); // Top Right Of The Quad
					glVertex3f( -1.0f, -1.0f, 1.0f ); // Top Left Of The Quad
					glVertex3f( -1.0f, -1.0f, -1.0f ); // Bottom Left Of The Quad
					glVertex3f( 1.0f, -1.0f, -1.0f ); // Bottom Right Of The Quad 

					glColor3f( 0,0,1 ); //blue color
					glVertex3f( 1.0f, 1.0f, 1.0f ); // Top Right Of The Quad (Front)
					glVertex3f( -1.0f, 1.0f, 1.0f ); // Top Left Of The Quad (Front)
					glVertex3f( -1.0f, -1.0f, 1.0f ); // Bottom Left Of The Quad
					glVertex3f( 1.0f, -1.0f, 1.0f ); // Bottom Right Of The Quad 

					glColor3f( 1,1,0 ); //yellow (red + green)
					glVertex3f( 1.0f, -1.0f, -1.0f ); // Bottom Left Of The Quad
					glVertex3f( -1.0f, -1.0f, -1.0f ); // Bottom Right Of The Quad
					glVertex3f( -1.0f, 1.0f, -1.0f ); // Top Right Of The Quad (Back)
					glVertex3f( 1.0f, 1.0f, -1.0f ); // Top Left Of The Quad (Back)

					glColor3f( 1,0,1 ); //purple (red + green)
					glVertex3f( -1.0f, 1.0f, 1.0f ); // Top Right Of The Quad (Left)
					glVertex3f( -1.0f, 1.0f, -1.0f ); // Top Left Of The Quad (Left)
					glVertex3f( -1.0f, -1.0f, -1.0f ); // Bottom Left Of The Quad
					glVertex3f( -1.0f, -1.0f, 1.0f ); // Bottom Right Of The Quad 

					glColor3f( 0,1, 1 ); //sky blue (blue +green)
					glVertex3f( 1.0f, 1.0f, -1.0f ); // Top Right Of The Quad (Right)
					glVertex3f( 1.0f, 1.0f, 1.0f ); // Top Left Of The Quad
					glVertex3f( 1.0f, -1.0f, 1.0f ); // Bottom Left Of The Quad
					glVertex3f( 1.0f, -1.0f, -1.0f ); // Bottom Right Of The Quad
					glEnd(); // Done Drawing The Quad
					glFlush();


			hdc=GetDC(hwnd);
			SwapBuffers(hdc);			// bring backbuffer to foreground
			ReleaseDC(hwnd,hdc);
					
					
		} // end If messages waiting
			
			
    } // end while

	return (int)(msg.wParam);	
}// WinMain



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
		case WM_CREATE:					// window is being created
		{
			hDC = GetDC(hWnd);			// get current window's device context
			

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

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glShadeModel(GL_SMOOTH);					// use smooth shading
			glEnable(GL_DEPTH_TEST);					// hidden surface removal
			glEnable(GL_CULL_FACE);						// do not calculate inside of poly's
			glFrontFace(GL_CCW);


			// set the cursor to the middle of the window and capture the window via "SendMessage"
			GetClientRect(hWnd, &rect);
			pt.x = rect.right / 2;
			pt.y = rect.bottom / 2;
			ClientToScreen(hWnd, &pt);
			SetCursorPos(pt.x, pt.y);
			SendMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
			return 0;
			
			return 0;
		}break;

		

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
		{
			int height = HIWORD(lParam);		// retrieve width and height
			int width = LOWORD(lParam);

			if (height==0)					// avoid divide by zero
			{
				height=1;					
			}

			glViewport(0, 0, width, height);									// reset the viewport to new dimensions
			glMatrixMode(GL_PROJECTION);										// set projection matrix current matrix
			glLoadIdentity();													// reset projection matrix
			camera->perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 8000.0f);
			glLoadMatrixf(&camera->getProjectionMatrix()[0][0]);
			return 0;
		}break;

	case WM_LBUTTONDOWN:
	{ // Capture the mouse
						   SetCapture(hWnd);
						   GetCursorPos(&g_OldCursorPos);
	} break;



	case WM_KEYDOWN:
	{
					   switch (wParam)
					   {

					   case VK_ESCAPE:
					   {
										 PostQuitMessage(0);
										 return 0;
					   }break;
					   case VK_SPACE:
					   {
										ReleaseCapture();
										return 0;
					   }break;


					   return 0;
					   }break;

					   return 0;
			}break;
			default:
		break;

  
    }
	return (DefWindowProc(hWnd, message, wParam, lParam));
}


void ProcessInput(HWND hWnd)
{
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
	if (GetCapture() == hWnd)
	{
		// Hide the mouse pointer
		SetCursor(NULL);

		// Retrieve the cursor position
		GetCursorPos(&CursorPos);

		// Calculate mouse rotational values
		X = (float)(g_OldCursorPos.x - CursorPos.x) * 0.1;
		Y = (float)(g_OldCursorPos.y - CursorPos.y) * 0.1;

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos(g_OldCursorPos.x, g_OldCursorPos.y);

		if (Direction > 0 || X != 0.0f || Y != 0.0f)
		{
			// Rotate camera
			if (X || Y)
			{
				camera->rotate(X, Y, 0.0f);


			} // End if any rotation


			if (Direction)
			{

				float dx = 0, dy = 0, dz = 0;

				if (Direction & DIR_FORWARD) dz = + 0.5f;
				if (Direction & DIR_BACKWARD) dz = - 0.5f ;
				if (Direction & DIR_LEFT) dx = - 0.5f ;
				if (Direction & DIR_RIGHT) dx = 0.5f ;
				if (Direction & DIR_UP) dy = 0.5f;
				if (Direction & DIR_DOWN) dy = -0.5f ;

				camera->move(dx, dy, dz);

			}

		}// End if any movement
	} // End if Captured
}








