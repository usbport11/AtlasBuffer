#ifndef sceneH
#define sceneH

#define MIN_WINDOW_SIZE_X 640
#define MIN_WINDOW_SIZE_Y 480

class MScene
{
private:
	int* pWindowWidth;
	int* pWindowHeight;
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;
	glm::mat4 DynamicMVP;
	glm::mat4 StaticMVP;
	glm::vec3 CameraPosition;
	glm::vec3 CameraDirection;
	float CameraRound;
	float CoordinateScale; //diffrence between resolution and cordinate system (needed for box2d better reaction)
	float ViewScale;
	glm::vec2 ViewOffset;
	glm::vec2 ViewCenter;
	glm::vec2 VectorZ;
public:
	MScene();
	~MScene();
	bool Initialize(int* inpWindowWidth, int* inpWindowHeight);
	void CalculateMVP();
	void MoveCamera(glm::vec2 Delta);
	void Size();
	void SetViewScale(float inViewScale);
	GLfloat* GetDynamicMVP();
	GLfloat* GetStaticMVP();
	void ViewAt(glm::vec2 Point);
	glm::vec2 WindowPosToWorldPos(double PosX, double PosY);
};

#endif
