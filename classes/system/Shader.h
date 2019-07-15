#ifndef shaderH
#define shaderH

class MShader
{
private:
	//system
	char* VertexShaderFileName;
	char* FragmentShaderFileName;
	GLuint VertexArrayId;

public:
	//variables are public (not need to create get functions)
	GLuint ProgramId;
	GLuint MVPId;
	GLuint TextureId;
	
	MShader();
	~MShader();
	GLuint CreateShaderProgram(const char* inVertexShaderFileName, const char* inFragmentShaderFileName);
	bool PrepareShaderValues();
	void Close();
};

#endif
