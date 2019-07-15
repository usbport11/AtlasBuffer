#ifndef atlasbufferH
#define atlasbufferH

#define TX_ROT0 0
#define TX_ROT90 1
#define TX_ROT180 2
#define TX_ROT270 3

class MAtlasBuffer
{
protected:
	GLuint VerticiesBufferId;
	GLuint UVBufferId;
	int BufferSize;
	vector<glm::vec2> Verticies;
	vector<glm::vec2> UV;
	stTexture* pTexture;
	unsigned int BindNumber;
	unsigned int TextureSize[2];
	unsigned int TilesCount[2];
	glm::vec2 HalfPixelOffset;
	glm::vec2 TileSize;
public:
	MAtlasBuffer();
	~MAtlasBuffer();
	bool Initialize(stTexture* inpTexture, unsigned int TexSizeX, unsigned int TexSizeY, unsigned int TilesCountX, unsigned int TilesCountY);
	void SetBindNumber(unsigned char Number);
	bool AddData(glm::vec2 vLeftBottom, glm::vec2 vRightTop, glm::vec2 tLeftBottom, glm::vec2 tRightTop);
	bool AddData(glm::vec2 vLeftBottom, glm::vec2 vRightTop, unsigned int TileNumberX, unsigned int TileNumberY, unsigned char Rotation, bool UseFix);
	bool Dispose();
	void Draw();
	void Begin();
	void End();
	void Clear();
	void Close();
};

#endif
