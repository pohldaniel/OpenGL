#include <windows.h>			
#include <iostream>
#include <fstream>

#include "Bitmap.h"
#include "Model.h"
#include "GL.h"
#include  "Extension.h"
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

GLuint u_projection, u_view, u_model, u_time, u_texture, positionID, colorID, texCoordID;
GLuint  g_texture, g_vertexBuffer, g_indexBuffer;
GLuint program;



Camera *camera;
float time = 90;
Model* model;

//prototype funktions
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParma, LPARAM lParam);
void setCursortoMiddle(HWND hwnd);
void processInput(HWND hWnd );
void renderFrame();

void initApp(HWND hWnd);
void readTextFile(const char *pszFilename, std::string &buffer);
void enableVerticalSync(bool enableVerticalSync);

GLuint loadShaderProgram(GLenum type, const char *pszFilename);
GLuint compileShader(GLenum type, const char *pszSource);
GLuint linkShaders(GLuint vertShader, GLuint fragShader);



GLuint loadVertexBuffer(const int vertexBufferSize, const float * vertexBuffer);
GLuint loadIndexBuffer(const int indexBufferSize, const int * indexBuffer);
GLuint loadTexture(const char *pszFilename);



GLuint createProgram();
void cleanup();


// the main windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Vector3f camPos{ 0, 5.0, 35.0 };
	Vector3f xAxis{ 1, 0, 0 };
	Vector3f yAxis{ 0, 1, 0 };
	Vector3f zAxis{ 0, 0, 1 };
	Vector3f target(0.0, 5.0, 0.0);
	Vector3f up(0.0, 1.0, 0.0);

	camera = new Camera(camPos, xAxis, yAxis, zAxis, target, up);

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
	windowClass.lpfnWndProc		= winProc;
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
						  "OBJViewer",							// app name
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
			
			
			processInput(hwnd);
			renderFrame();
			

			hdc=GetDC(hwnd);
			SwapBuffers(hdc);			
			ReleaseDC(hwnd,hdc);
		}
    } // end while

	cleanup();
	return msg.wParam;
}

// the Windows Procedure event handler
LRESULT CALLBACK winProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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

					enableVerticalSync(!g_enableVerticalSync);
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

void initApp(HWND hWnd)
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
	enableVerticalSync(true);

	
	camera->perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 200.0f);
	

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	
	glEnable(GL_DEPTH_TEST);					// hidden surface removal
	//glEnable(GL_CULL_FACE);						// do not calculate inside of poly's


	

	model = new Model();
	model->loadObject("objs/Dragon/dragon.obj");

	
	

	for (int i = 0; i < model->numberOfMeshes(); i++){
		
		model->getMesches()[i]->readMaterial((model->getModelDirectory() + "/" + model->getMltPath()).c_str());
	}

	
	for (int i = 0; i < model->numberOfMeshes(); i++){
		
		model->getMesches()[i]->setVertexName(loadVertexBuffer(model->getMesches()[i]->getNumberOfVertices(),
															  model->getMesches()[i]->getVertexBuffer()));

		model->getMesches()[i]->setIndexName( loadIndexBuffer(model->getMesches()[i]->getNumberOfIndices(),
																	 model->getMesches()[i]->getIndexBuffer()));

		model->getMesches()[i]->setTextureName(loadTexture((model->getModelDirectory() + "/" + 
															model->getMesches()[i]->getMaterial().colorMapPath).c_str()));

	}

	

	model->rotate(Vector3f(1.0, 0.0, 0.0), 90);

	
	

	program = createProgram();
	u_projection = glGetUniformLocation(program, "u_projection");
	u_view = glGetUniformLocation(program, "u_view");
	u_model = glGetUniformLocation(program, "u_model");
	u_time = glGetUniformLocation(program, "u_time");
	u_texture = glGetUniformLocation(program, "u_texture");
	positionID = glGetAttribLocation(program, "position");
	colorID = glGetAttribLocation(program, "color");
	texCoordID = glGetAttribLocation(program, "texCoords");

	glUseProgram(program);
	glUniformMatrix4fv(u_projection, 1, true, &camera->getProjectionMatrix()[0][0]);
	//glUniformMatrix4fv(u_model, 1, true, &model->getTransformationMatrix()[0][0]);
	glUniform1i(u_texture, 0);
	glUseProgram(0);

}

void setCursortoMiddle(HWND hwnd){

	RECT rect;

	GetClientRect(hwnd, &rect);
	SetCursorPos(rect.right / 2, rect.bottom / 2);

}
void enableVerticalSync(bool enableVerticalSync){

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

void readTextFile(const char *pszFilename, std::string &buffer){

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

GLuint loadShaderProgram(GLenum type, const char *pszFilename){

	GLuint shader = 0;
	std::string buffer;
	readTextFile(pszFilename, buffer);

	if (buffer.length() > 0)
	{


		shader = compileShader(type, reinterpret_cast<const char *>(&buffer[0]));
	}

	return shader;
}

GLuint compileShader(GLenum type, const char *pszSource){

	GLuint shader = glCreateShader(type);

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

GLuint linkShaders(GLuint vertShader, GLuint fragShader){

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

GLuint loadTexture(const char *pszFilename){

	Bitmap* bitmap = new Bitmap();
	GLuint texture;

	if (bitmap->loadBitmap24(pszFilename)){

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap->width, bitmap->height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap->data);




		glGenerateMipmap(GL_TEXTURE_2D);
	}
	delete bitmap;
	bitmap = NULL;

	return texture;
}

GLuint loadVertexBuffer(const int a_vertexBufferSize, const float *a_vertexBuffer){

	GLuint vertexBuffer = 0;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, a_vertexBufferSize * 4, a_vertexBuffer, GL_STATIC_DRAW);

	return vertexBuffer;
}


GLuint loadIndexBuffer(const int a_indexBufferSize, const int * a_indexBuffer){

	GLuint indexBuffer = 0;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, a_indexBufferSize * 4, a_indexBuffer, GL_STATIC_DRAW);

	return indexBuffer;
}


GLuint createProgram() {


	GLuint vshader = loadShaderProgram(GL_VERTEX_SHADER, "program.vert");
	GLuint fshader = loadShaderProgram(GL_FRAGMENT_SHADER, "program.frag");

	return linkShaders(vshader, fshader);

}

void cleanup(){


	if (program)
	{
		glDeleteProgram(program);
		program = 0;
	}

}


void processInput(HWND hWnd )
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

void renderFrame(){

	time = time + 0.5;
	
	if (time > 360) time = 0;
	
	//model->rotate(Vector3f(1.0, 0.0, 0.0),  0.5);
	//model->translate(0.5, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	glUniformMatrix4fv(u_view, 1, true, &camera->getViewMatrix()[0][0]);
	glUniformMatrix4fv(u_model, 1, true, &model->getTransformationMatrix()[0][0]);
	//glUniform1f(u_time, time);

	for (int i = 0; i < model->numberOfMeshes(); i++){

		glBindBuffer(GL_ARRAY_BUFFER, model->getMesches()[i]->getVertexName());


		if (model->hasTextureCoords() && model->hasNormals()){


			glEnableVertexAttribArray(positionID);
			glVertexAttribPointer(
				positionID,					// attribute
				3,						     // size
				GL_FLOAT,					 // type
				GL_FALSE,					 // normalized?
				8 * sizeof(float),			 // stride
				(void*)0      // array buffer offset
				);

			glEnableVertexAttribArray(texCoordID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model->getMesches()[i]->getTextureName());

			glVertexAttribPointer(
				texCoordID,                       // attribute
				2,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				8 * sizeof(float),	              // stride
				(void*)(6 * sizeof(float))        // array buffer offset
				);



		}else if (model->hasNormals()) {

			glEnableVertexAttribArray(positionID);
			glVertexAttribPointer(
				positionID,					// attribute
				3,						    // size
				GL_FLOAT,					// type
				GL_FALSE,					// normalized?
				6 * sizeof(float),			// stride
				(void*)0					// array buffer offset
				);

		}else if (model->hasTextureCoords()){


			glEnableVertexAttribArray(positionID);
			glVertexAttribPointer(
				positionID,					// attribute
				3,						    // size
				GL_FLOAT,					// type
				GL_FALSE,					// normalized?
				5 * sizeof(float),			// stride
				(void*)0					// array buffer offset
				);

			glEnableVertexAttribArray(texCoordID);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model->getMesches()[i]->getTextureName());

			glVertexAttribPointer(
				texCoordID,                       // attribute
				2,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				5 * sizeof(float),	              // stride
				(void*)(3 * sizeof(float))        // array buffer offset
				);
		}else{

			glEnableVertexAttribArray(positionID);
			glVertexAttribPointer(
				positionID,					// attribute
				3,						     // size
				GL_FLOAT,					 // type
				GL_FALSE,					 // normalized?
				3 * sizeof(float),			 // stride
				(void*)0      // array buffer offset
				);
		}


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->getMesches()[i]->getIndexName());
		glDrawElements(GL_TRIANGLES, model->getMesches()[i]->getNumberOfTriangles() * 3, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


		if (model->hasTextureCoords()){

			glDisableVertexAttribArray(texCoordID);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glDisableVertexAttribArray(positionID);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glUseProgram(0);

}





