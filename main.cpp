#include "stdafx.h"
#include "classes/system/Shader.h"
#include "classes/system/Scene.h"
#include "classes/system/FPSController.h"
#include "classes/image/TextureLoader.h"
#include "classes/buffers/AtlasBuffer.h"

bool Pause;
bool keys[1024] = {0};
int WindowWidth = 800, WindowHeight = 600;
bool EnableVsync = 1;
GLFWwindow* window;
stFPSController FPSController;

MShader Shader;
MScene Scene;
MAtlasBuffer AtlasBuffer;
MTextureLoader TextureLoader;
stTexture* txAtlas;
unsigned int txAtlas_cnt;
glm::vec2 CameraVelocity(2, 2);

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
	if(action == GLFW_PRESS)
    	keys[key] = true;
    else if (action == GLFW_RELEASE)
    	keys[key] = false;
}

void MoveKeys() {
	if(keys[GLFW_KEY_LEFT] || keys[GLFW_KEY_RIGHT] || keys[GLFW_KEY_UP] || keys[GLFW_KEY_DOWN]) {
		if(keys[GLFW_KEY_LEFT]) {
			Scene.MoveCamera(glm::vec2(-CameraVelocity.x, 0));
		}
		if(keys[GLFW_KEY_RIGHT]) {
			Scene.MoveCamera(glm::vec2(CameraVelocity.x, 0));
		}
		if(keys[GLFW_KEY_UP]) {
			Scene.MoveCamera(glm::vec2(0, CameraVelocity.y));
		}
		if(keys[GLFW_KEY_DOWN]) {
			Scene.MoveCamera(glm::vec2(0, -CameraVelocity.y));
		}
		Scene.CalculateMVP();
	}
}

bool CheckOpenglSupport() {
	//get opengl data (here was CRASH on suspisious notebook)
	string OpenGLVersion = (char*)glGetString(GL_VERSION);
	string OpenGLVendor = (char*)glGetString(GL_VENDOR);
	string OpenGLRenderer = (char*)glGetString(GL_RENDERER);
	string ShadersVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	LogFile<<"Window: OpenGL version: "<<OpenGLVersion<<endl;
	LogFile<<"Window: OpenGL vendor: "<<OpenGLVendor<<endl;
	LogFile<<"Window: OpenGL renderer: "<<OpenGLRenderer<<endl;
	LogFile<<"Window: Shaders version: "<<ShadersVersion<<endl;
	
	float VersionOGL, VersionSHD;
	sscanf(OpenGLVersion.c_str(), "%f", &VersionOGL);
	if(VersionOGL < 3.0f) {
		LogFile<<"Window: Old version of OpenGL. Sorry"<<endl;
		return false;
	}
	sscanf(ShadersVersion.c_str(), "%f", &VersionSHD);
	if(VersionSHD < 3.3f) {
		LogFile<<"Window: Old version of shaders. Sorry"<<endl;
		return false;
	}
	
	return true;
}

bool InitApp() {
	LogFile<<"Starting application"<<endl;    
    glfwSetErrorCallback(error_callback);
    
    if(!glfwInit()) return false;
    window = glfwCreateWindow(WindowWidth, WindowHeight, "TestApp", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return false;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    if(glfwExtensionSupported("WGL_EXT_swap_control")) {
    	LogFile<<"Window: V-Sync supported. V-Sync: "<<EnableVsync<<endl;
		glfwSwapInterval(EnableVsync);//0 - disable, 1 - enable
	}
	else LogFile<<"Window: V-Sync not supported"<<endl;
    LogFile<<"Window created: width: "<<WindowWidth<<" height: "<<WindowHeight<<endl;

	//glew
	GLenum Error = glewInit();
	if(GLEW_OK != Error) {
		LogFile<<"Window: GLEW Loader error: "<<glewGetErrorString(Error)<<endl;
		return false;
	}
	LogFile<<"GLEW initialized"<<endl;
	
	if(!CheckOpenglSupport()) return false;

	//shaders
	if(!Shader.CreateShaderProgram("shaders/main.vertexshader.glsl", "shaders/main.fragmentshader.glsl")) return false;
	if(!Shader.PrepareShaderValues()) return false;
	LogFile<<"Shaders loaded"<<endl;

	//scene
	if(!Scene.Initialize(&WindowWidth, &WindowHeight)) return false;
	LogFile<<"Scene initialized"<<endl;

	//randomize
    srand(time(NULL));
    LogFile<<"Randomized"<<endl;
    
    //texture
    txAtlas = TextureLoader.LoadTexture("textures/atlas.png", 1, 1, 0, txAtlas_cnt, GL_LINEAR, GL_REPEAT);
    if(!txAtlas) return false;
    
    //buffer
    if(!AtlasBuffer.Initialize(txAtlas, 512, 512, 8, 8)) return false;
	if(!AtlasBuffer.AddData(glm::vec2(0, 0), glm::vec2(512, 512), glm::vec2(0, 0), glm::vec2(1, 1))) return false;
	if(!AtlasBuffer.AddData(glm::vec2(512, 448), glm::vec2(576, 512), 3, 3, 0, true)) return false;
	if(!AtlasBuffer.AddData(glm::vec2(576, 448), glm::vec2(640, 512), 3, 3, 0, false)) return false;
	if(!AtlasBuffer.Dispose()) return false;

	Pause = false;
    
    return true;
}

void RenderStep() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(Shader.ProgramId);
	glUniformMatrix4fv(Shader.MVPId, 1, GL_FALSE, Scene.GetDynamicMVP());
	glUniform1i(Shader.TextureId, 0);
	AtlasBuffer.Begin();
	AtlasBuffer.Draw();
	AtlasBuffer.End();
}

void ClearApp() {
	AtlasBuffer.Close();
	TextureLoader.DeleteTexture(txAtlas, txAtlas_cnt);
	TextureLoader.Close();
	Shader.Close();
	LogFile<<"Application: closed"<<endl;
}

int main(int argc, char** argv) {
	LogFile<<"Application: started"<<endl;
	if(!InitApp()) {
		ClearApp();
		glfwTerminate();
		return 0;
	}
	FPSController.Initialize(glfwGetTime());
	while(!glfwWindowShouldClose(window)) {
		FPSController.FrameStep(glfwGetTime());
    	FPSController.FrameCheck();
    	MoveKeys();
		RenderStep();
        glfwSwapBuffers(window);
        glfwPollEvents();
	}
	ClearApp();
    glfwTerminate();
	
	return 0;
}
