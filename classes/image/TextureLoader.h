#ifndef textureloaderH
#define textureloaderH

#include "Tga.h"
#include "Png.h"

//texture wrapping
#ifndef GL_COMBINE
	#define GL_COMBINE 0x8570
#endif
#ifndef GL_CLAMP_TO_BORDER
	#define GL_CLAMP_TO_BORDER 0x812D
#endif
#ifndef GL_CLAMP_TO_EDGE
	#define GL_CLAMP_TO_EDGE 0x812F
#endif
#ifndef GL_MIRRORED_REPEAT
	#define GL_MIRRORED_REPEAT 0x8370
#endif

class MTextureLoader
{
private:
	stTexture* pTexture;
	MTga* Tga;
	MPng* Png;
	unsigned char* pImageData;
	unsigned long ImageWidth;
	unsigned long ImageHeight;
	unsigned int BitsPerPixel;
	
	float FilterType;
	float WrapType;
	
	unsigned char FileType[1];
	bool LoadFile(const char* inFileName);
	bool CreateTexture(unsigned int& Id, unsigned long Width, unsigned long Height, unsigned char* Data);
	void FreeImage();
public:
	MTextureLoader();
	~MTextureLoader();
	stTexture* LoadTexture(const char* inFileName, unsigned int CountX, unsigned int CountY, unsigned int CountToRead, unsigned int& ReturnCount, float inFilterType, float inWrapType);
	bool DeleteTexture(stTexture* Texture, unsigned int Count);
	void Close();
};

#endif
