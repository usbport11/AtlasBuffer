#include "../../stdafx.h"
#include "AtlasBuffer.h"

MAtlasBuffer::MAtlasBuffer()
{
	VerticiesBufferId = -1;
	UVBufferId = -1;
	BindNumber = 0;
	pTexture = NULL;
	BufferSize = 0;
}

MAtlasBuffer::~MAtlasBuffer()
{
	VerticiesBufferId = -1;
	UVBufferId = -1;
	BindNumber = 0;
	pTexture = NULL;
	BufferSize = 0;
}

bool MAtlasBuffer::Initialize(stTexture* inpTexture, unsigned int TexSizeX, unsigned int TexSizeY, unsigned int TilesCountX, unsigned int TilesCountY)
{
	if(!inpTexture)
	{
		LogFile<<"Atlas buffer: Empty texture given"<<endl;
		return false;
	}
	pTexture = inpTexture;
	
	//need extened checks
	if(!IsPowerOfTwo(TexSizeX) || !IsPowerOfTwo(TexSizeY)) {
		LogFile<<"Atlas buffer: Some of texture size not power of 2"<<endl;
		return false;
	}
	if(!IsPowerOfTwo(TexSizeX / TilesCountX) || !IsPowerOfTwo(TexSizeY / TilesCountY)) {
		LogFile<<"Atlas buffer: Some of tile size not power of 2"<<endl;
		return false;
	}
	if(TexSizeX <= TilesCountX || TexSizeY <= TilesCountY) {
		LogFile<<"Atlas buffer: Wrong tile size"<<endl;
		return false;
	}
	
	TextureSize[0] = TexSizeX;
	TextureSize[1] = TexSizeY;
	TilesCount[0] = TilesCountX;
	TilesCount[1] = TilesCountY;
	HalfPixelOffset = glm::vec2(0.5 / TextureSize[0], 0.5 / TextureSize[1]);
	TileSize = glm::vec2(0.5 / TilesCount[0] * 2, 0.5 / TilesCount[1] * 2);
	
	GLenum error;
	glGenBuffers(1, &VerticiesBufferId);
	error = glGetError();
	if(error != GL_NO_ERROR)
	{
		LogFile<<"Atlas buffer: "<<(char*)gluErrorString(error)<<" "<<error<<endl;
		return false;
	}
	glGenBuffers(1, &UVBufferId);
	error = glGetError();
	if(error != GL_NO_ERROR)
	{
		LogFile<<"Atlas buffer: "<<(char*)gluErrorString(error)<<" "<<error<<endl;
		return false;
	}
	
	return true;
}

void MAtlasBuffer::SetBindNumber(unsigned char Number)
{
	BindNumber = Number;
}

bool MAtlasBuffer::AddData(glm::vec2 vLeftBottom, glm::vec2 vRightTop, glm::vec2 tLeftBottom, glm::vec2 tRightTop) {
	//4 vecticies
	Verticies.push_back(glm::vec2(vLeftBottom.x, vLeftBottom.y));
	Verticies.push_back(glm::vec2(vRightTop.x, vLeftBottom.y));
	Verticies.push_back(glm::vec2(vRightTop.x, vRightTop.y));
	Verticies.push_back(glm::vec2(vLeftBottom.x, vRightTop.y));
	//4 uv
	UV.push_back(glm::vec2(tLeftBottom.x, tLeftBottom.y));
	UV.push_back(glm::vec2(tRightTop.x, tLeftBottom.y));
	UV.push_back(glm::vec2(tRightTop.x, tRightTop.y));
	UV.push_back(glm::vec2(tLeftBottom.x, tRightTop.y));
	
	return true;
}

bool MAtlasBuffer::AddData(glm::vec2 vLeftBottom, glm::vec2 vRightTop, unsigned int TileNumberX, unsigned int TileNumberY, unsigned char Rotation, bool UseFix) {
	if(TileNumberX >= TilesCount[0] || TileNumberY >= TilesCount[1] ) {
		LogFile<<"Atlas buffer: Wrong tile number"<<endl;
		return false;
	}
	//4 vecticies
	Verticies.push_back(glm::vec2(vLeftBottom.x, vLeftBottom.y));
	Verticies.push_back(glm::vec2(vRightTop.x, vLeftBottom.y));
	Verticies.push_back(glm::vec2(vRightTop.x, vRightTop.y));
	Verticies.push_back(glm::vec2(vLeftBottom.x, vRightTop.y));
	//4 uv
	glm::vec2 TexCoords[4];
	switch(Rotation) {
		default:
		case TX_ROT0:
			TexCoords[0] = glm::vec2(TileNumberX * TileSize.x + (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + (UseFix * HalfPixelOffset.y));
			TexCoords[1] = glm::vec2(TileNumberX * TileSize.x + TileSize.x - (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + (UseFix * HalfPixelOffset.y));
			TexCoords[2] = glm::vec2(TileNumberX * TileSize.x + TileSize.x - (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + TileSize.y - (UseFix * HalfPixelOffset.y));
			TexCoords[3] = glm::vec2(TileNumberX * TileSize.x + (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + TileSize.y - (UseFix * HalfPixelOffset.y));
			break;
		case TX_ROT90:
			TexCoords[0] = glm::vec2(TileNumberX * TileSize.x + (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + TileSize.y - (UseFix * HalfPixelOffset.y));
			TexCoords[1] = glm::vec2(TileNumberX * TileSize.x + (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + (UseFix * HalfPixelOffset.y));
			TexCoords[2] = glm::vec2(TileNumberX * TileSize.x + TileSize.x - (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + (UseFix * HalfPixelOffset.y));
			TexCoords[3] = glm::vec2(TileNumberX * TileSize.x + TileSize.x - (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + TileSize.y - (UseFix * HalfPixelOffset.y));
			break;
		case TX_ROT180:
			TexCoords[0] = glm::vec2(TileNumberX * TileSize.x + TileSize.x - (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + TileSize.y - (UseFix * HalfPixelOffset.y));
			TexCoords[1] = glm::vec2(TileNumberX * TileSize.x + (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + TileSize.y - (UseFix * HalfPixelOffset.y));
			TexCoords[2] = glm::vec2(TileNumberX * TileSize.x + (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + (UseFix * HalfPixelOffset.y));
			TexCoords[3] = glm::vec2(TileNumberX * TileSize.x + TileSize.x - (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + (UseFix * HalfPixelOffset.y));
			break;
		case TX_ROT270:
			TexCoords[0] = glm::vec2(TileNumberX * TileSize.x + TileSize.x - (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + (UseFix * HalfPixelOffset.y));
			TexCoords[1] = glm::vec2(TileNumberX * TileSize.x + TileSize.x - (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + TileSize.y - (UseFix * HalfPixelOffset.y));
			TexCoords[2] = glm::vec2(TileNumberX * TileSize.x + (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + TileSize.y - (UseFix * HalfPixelOffset.y));
			TexCoords[3] = glm::vec2(TileNumberX * TileSize.x + (UseFix * HalfPixelOffset.x), TileNumberY * TileSize.y + (UseFix * HalfPixelOffset.y));
			break;
	}
	UV.push_back(TexCoords[0]);
	UV.push_back(TexCoords[1]);
	UV.push_back(TexCoords[2]);
	UV.push_back(TexCoords[3]);
	
	return true;
}

bool MAtlasBuffer::Dispose()
{
	if(VerticiesBufferId <= 0 || UVBufferId <= 0)
	{
		LogFile<<"Atlas buffer: Not initialized"<<endl;
		return false;
	}
	BufferSize = Verticies.size();
	glBindBuffer(GL_ARRAY_BUFFER, VerticiesBufferId);
	glBufferData(GL_ARRAY_BUFFER, BufferSize * sizeof(glm::vec2), &Verticies[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, UVBufferId);
	glBufferData(GL_ARRAY_BUFFER, BufferSize * sizeof(glm::vec2), &UV[0], GL_STATIC_DRAW);
	Verticies.clear();
	UV.clear();
	
	if(!BufferSize) {
		LogFile<<"Atlas buffer: empty"<<endl;
		return false;
	}
	
	return true;
}

void MAtlasBuffer::Draw()
{
	glActiveTexture(GL_TEXTURE0 + BindNumber);
	glBindTexture(GL_TEXTURE_2D, pTexture->Id);
	glBindBuffer(GL_ARRAY_BUFFER, VerticiesBufferId);
	glVertexAttribPointer(SHR_LAYOUT_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, UVBufferId);
	glVertexAttribPointer(SHR_LAYOUT_UV, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_QUADS, 0, 4 * BufferSize);
}
 
void MAtlasBuffer::Begin()
{
	glEnableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glEnableVertexAttribArray(SHR_LAYOUT_UV);
}

void MAtlasBuffer::End()
{
	glDisableVertexAttribArray(SHR_LAYOUT_VERTEX);
	glDisableVertexAttribArray(SHR_LAYOUT_UV);
}

void MAtlasBuffer::Clear()
{
	glBindBuffer(GL_ARRAY_BUFFER, VerticiesBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * Verticies.size(), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, UVBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * UV.size(), NULL, GL_STATIC_DRAW);
	Verticies.clear();
	UV.clear();
}

void MAtlasBuffer::Close()
{
	Clear();
	if(VerticiesBufferId > 0) glDeleteBuffers(1, &VerticiesBufferId);
	if(UVBufferId > 0) glDeleteBuffers(1, &UVBufferId);
}
