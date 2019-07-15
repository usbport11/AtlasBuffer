#include "../../stdafx.h"
#include "Tga.h"

unsigned char TGA_UNCOMPRESSED[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
unsigned char TGA_COMPRESSED[12] = {0,0,10,0,0,0,0,0,0,0,0,0};

MTga::MTga()
{
	pImageData = NULL;
	pFileName = NULL;
	CompressedTGA = false;
}

MTga::~MTga()
{
	pImageData = NULL;
	pFileName = NULL;
	CompressedTGA = false;
}

unsigned int MTga::GetWidth()
{
	return Width;
}

unsigned int MTga::GetHeight()
{
	return Height;
}

unsigned int MTga::GetBitsPerPixel()
{
	return BitsPerPixel;
}

unsigned char* MTga::GetImageData()
{
	return pImageData;
}

bool MTga::GetTGAHeader()
{
    FILE* pTGA = fopen(pFileName, "rb");
    if(!pTGA)
    {     
        LogFile<<"Cannot open file: "<<pFileName<<endl;
        return false;
    }
    if(!fread(&TGAHeader, sizeof(stTGAHeader), 1, pTGA))
    {    
        LogFile<<"Cannot read header"<<endl;
        return false;
    }
	fclose(pTGA);
	
	return true;
}

bool MTga::LoadTGACompressed()
{ 
	FILE* pTGA = fopen(pFileName, "rb");
	fseek(pTGA, 12, 0);
	if(!pTGA)
    {     
        LogFile<<"Cannot open file"<<endl;
        return false;
    }
	
	LogFile<<"Start reading compressed TGA file"<<endl;
	if(!fread(TGAData.Header, sizeof(TGAData.Header), 1, pTGA))
    {
        LogFile<<"Failed read TGA header"<<endl;
		fclose(pTGA);
        return false;
    }
	
    Width = TGAData.Header[1] * 256 + TGAData.Header[0];
    Height = TGAData.Header[3] * 256 + TGAData.Header[2];
    BitsPerPixel = TGAData.Header[4];
    TGAData.Width = Width;
    TGAData.Height = Height;
    TGAData.BitsPerPixel = BitsPerPixel;
    if(Width <=0 || Height <=0)
	{
		fclose(pTGA);
		LogFile<<"Null width or height"<<endl;
		return false;
	}
    if(BitsPerPixel !=24 || BitsPerPixel != 32)
	{
		fclose(pTGA);
        LogFile<<"Bits per pixel not equal nor 24 nor 32"<<endl;
		return false;
	}
    if(BitsPerPixel == 24) Type = GL_RGB;
    if(BitsPerPixel == 32) Type = GL_RGBA;
    TGAData.BytesPerPixel = TGAData.BitsPerPixel / 8;
    TGAData.ImageSize = TGAData.BytesPerPixel * TGAData.Width * TGAData.Height;
    try
    {
        pImageData = new unsigned char [TGAData.ImageSize];
    }
    catch(bad_alloc& ba)
    {
		fclose(pTGA);
		LogFile<<"Cant allocate memory"<<endl;
        return false;
    }
    
    unsigned int PixelCount = TGAData.Width * TGAData.Height;
    unsigned int CurrentPixel = 0;
    unsigned int CurrentByte = 0;
    unsigned char* ColorBuffer = new unsigned char [TGAData.BytesPerPixel];
    unsigned char ChunkHeader;
    do
    {
        ChunkHeader = 0;
        if(fread(&ChunkHeader, sizeof(unsigned char), 1, pTGA))
		{
			fclose(pTGA);
			return false;
		}
        if(ChunkHeader < 128)
        {
            ChunkHeader ++;
            for(short int i=0; i<ChunkHeader; i++)
            {
                if(fread(ColorBuffer, 1, TGAData.BytesPerPixel, pTGA))
				{
					fclose(pTGA);
					return false;
				}
                pImageData[CurrentByte] = ColorBuffer[2];
                pImageData[CurrentByte + 1] = ColorBuffer[1];
                pImageData[CurrentByte + 2] = ColorBuffer[0];
                if(TGAData.BitsPerPixel == 4) pImageData[CurrentByte + 3] = ColorBuffer[3];
                CurrentByte += TGAData.BytesPerPixel;
                CurrentPixel ++;
            }
        }
        else
        {
            ChunkHeader -= 127;
            if(fread(ColorBuffer, 1, TGAData.BytesPerPixel, pTGA))
			{
				fclose(pTGA);
				return false;
			}
            for(short int i=0; i<ChunkHeader; i++)
            {
                pImageData[CurrentByte] = ColorBuffer[2];
                pImageData[CurrentByte + 1] = ColorBuffer[1];
                pImageData[CurrentByte + 2] = ColorBuffer[0];
                if(TGAData.BitsPerPixel == 4) pImageData[CurrentByte + 3] = ColorBuffer[3];
                CurrentByte += TGAData.BytesPerPixel;
                CurrentPixel ++;
            }
        }
    }
    while(CurrentPixel < PixelCount);
    fclose(pTGA);
    
	return true;
}

bool MTga::LoadTGAUncompressed()
{
	FILE* pTGA = fopen(pFileName, "rb");
	fseek(pTGA, 12, 0);
	if(!pTGA)
    {     
        LogFile<<"Cannot open file"<<endl;
        return false;
    }
	
	LogFile<<"Start reading uncompressed TGA file"<<endl;
    if(!fread(TGAData.Header, sizeof(TGAData.Header), 1, pTGA))
    {
        LogFile<<"Failed read TGA header"<<endl;
		fclose(pTGA);
        return false;
    }
	
    Width = TGAData.Header[1] * 256 + TGAData.Header[0];
    Height = TGAData.Header[3] * 256 + TGAData.Header[2];
    BitsPerPixel = TGAData.Header[4];
    TGAData.Width = Width;
    TGAData.Height = Height;
    TGAData.BitsPerPixel = BitsPerPixel;
    LogFile<<"TGAData.Width: "<<TGAData.Width<<endl;
    LogFile<<"TGAData.Height: "<<TGAData.Height<<endl;
    LogFile<<"TGAData.BitsPerPixel: "<<TGAData.BitsPerPixel<<endl;
    if(Width <=0 || Height <=0)
    {
        LogFile<<"Width or height are zero"<<endl;
		fclose(pTGA);
        return false;
    }
    if(BitsPerPixel != 24 && BitsPerPixel != 32) 
    {
        LogFile<<"Bits per pixel not 24 and not 32: "<<BitsPerPixel<<endl;
		fclose(pTGA);
        return false;
    }
    if(BitsPerPixel == 24) Type = GL_RGB;
    if(BitsPerPixel == 32) Type = GL_RGBA;
    TGAData.BytesPerPixel = TGAData.BitsPerPixel / 8;
    TGAData.ImageSize = TGAData.BytesPerPixel * TGAData.Width * TGAData.Height;
    LogFile<<"Type: "<<Type<<endl;
    LogFile<<"TGAData.BytesPerPixel: "<<TGAData.BytesPerPixel<<endl;
    try
    {
        pImageData = new unsigned char [TGAData.ImageSize];
        memset(pImageData, 0, TGAData.ImageSize);
    }
    catch(bad_alloc& ba)
    {
        LogFile<<"Can not allocate memmory"<<endl;
		fclose(pTGA);
        return false;
    }
    
    LogFile<<"Creating texture"<<endl;
    if(fread(pImageData, 1, TGAData.ImageSize, pTGA) != TGAData.ImageSize)
    {
        LogFile<<"Cant read image data"<<endl;
		fclose(pTGA);
        return false;
    }
    LogFile<<"Replacing color bytes in image"<<endl;
    for(unsigned int i = 0; i<(int)TGAData.ImageSize; i += TGAData.BytesPerPixel)
    {
        pImageData[i] ^= pImageData[i+2] ^= pImageData[i] ^= pImageData[i+2];
    }
    fclose(pTGA);
    
    return true;
}

unsigned char* MTga::LoadTGA(const char* inFileName)
{
	if(!inFileName)
    {
        LogFile<<"Empty filename given"<<endl;
        return NULL;
    }
	pFileName = (char*)inFileName;
	if(!GetTGAHeader()) return NULL;
    if(!memcmp(TGA_UNCOMPRESSED, &TGAHeader, sizeof(stTGAHeader))) 
    {
    	if(!LoadTGAUncompressed())
		{
			if(pImageData) delete [] pImageData;
			LogFile<<"Failed get uncompressed TGA"<<endl;
			return NULL;
		}
	}
    if(!memcmp(TGA_COMPRESSED, &TGAHeader, sizeof(stTGAHeader)))
    {
    	if(!LoadTGACompressed())
		{
			if(pImageData) delete [] pImageData;
			LogFile<<"Failed get compressed TGA"<<endl;
			return NULL;
		}
	}
	return pImageData;
}

void MTga::FreeTga()
{
	if(pImageData) delete [] pImageData;
}
