#include "../../stdafx.h"
#include "Shader.h"

MShader::MShader()
{
	VertexArrayId = 0;
	ProgramId = 0;
	MVPId = 0;
	TextureId = 0;

	VertexShaderFileName = 0;
	FragmentShaderFileName = 0;
	VertexShaderFileName = NULL;
	FragmentShaderFileName = NULL;
}

MShader::~MShader()
{
	VertexArrayId = 0;
	ProgramId = 0;
	MVPId = 0;
	TextureId = 0;
	
	VertexShaderFileName = 0;
	FragmentShaderFileName = 0;
	VertexShaderFileName = NULL;
	FragmentShaderFileName = NULL;
}
	
GLuint MShader::CreateShaderProgram(const char* inVertexShaderFileName, const char* inFragmentShaderFileName)
{
	if(!inVertexShaderFileName || !inFragmentShaderFileName)
	{
		LogFile<<"Shader: Some of shader file name is empty"<<endl;
		return 0;
	}
	
	//vertex array
	glGenVertexArrays(1, &VertexArrayId);
	glBindVertexArray(VertexArrayId);
	GLenum Error = glGetError();
	if(Error != GL_NO_ERROR)
	{
		LogFile<<"Game2: "<<(char*)gluErrorString(Error)<<" "<<Error<<endl;
		return false;
	}

	VertexShaderFileName = (char*)inVertexShaderFileName;
	FragmentShaderFileName = (char*)inFragmentShaderFileName;
	
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(VertexShaderFileName, ios::in);
	if(VertexShaderStream.is_open())
	{
		string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else
	{
		LogFile<<"Shader(vertex): Can not open shader file"<<endl;
		return 0;
	}

	// Read the Fragment Shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(FragmentShaderFileName, ios::in);
	if(FragmentShaderStream.is_open())
	{
		string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	LogFile<<"Shader(vertex): Compiling shader"<<endl;
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if(InfoLogLength > 0)
	{
		vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		LogFile<<"Shader(vertex): Vertex: "<<&VertexShaderErrorMessage[0]<<endl;
	}

	// Compile Fragment Shader
	LogFile<<"Shader(fragment): Compiling shader"<<endl;
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
	{
		vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		LogFile<<"Shader: Fragment: "<<&FragmentShaderErrorMessage[0]<<endl;
	}
		
	// Link the program
	LogFile<<"Shader: Linking program"<<endl;
	ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderID);
	glAttachShader(ProgramId, FragmentShaderID);
	glLinkProgram(ProgramId);

	// Check the program
	glGetProgramiv(ProgramId, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramId, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
	{
		vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramId, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		LogFile<<&ProgramErrorMessage[0]<<endl;
	}

	glDetachShader(ProgramId, VertexShaderID);
	glDetachShader(ProgramId, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	
	VertexShaderCode.clear();
	FragmentShaderCode.clear();
	
	return ProgramId;
}
	
bool MShader::PrepareShaderValues()
{
	if(!ProgramId)
	{
		LogFile<<"Shader: Program not loaded"<<endl;
		return false;
	}
	
	MVPId = glGetUniformLocation(ProgramId, "MVP");
	if(MVPId == -1)
	{
		LogFile<<"Shader: Can't get MVP uniform"<<endl;
		return false;
	}
	
	TextureId = glGetUniformLocation(ProgramId, "myTextureSampler");
	if(TextureId == -1)
	{
		LogFile<<"Shader: Can't get texture uniform"<<endl;
		return false;
	}
	
	return true;
}
	
void MShader::Close()
{
	glDeleteProgram(ProgramId);
	glDeleteVertexArrays(1, &VertexArrayId);
}
