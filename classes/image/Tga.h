#ifndef tgaH
#define tgaH

struct stTGAHeader
{
    unsigned char Header[12];
};

struct stTGAData
{
    unsigned char Header[6];
    unsigned int BytesPerPixel;
    unsigned int ImageSize;
    unsigned int Type;
    unsigned int Width;
    unsigned int Height;
    unsigned int BitsPerPixel;
};

class MTga
{
private:
	//file data
	FILE* pFile;
	char* pFileName;
	//header data
	stTGAHeader TGAHeader;
    stTGAData TGAData;
    //image data
	unsigned char* pImageData;
    unsigned int Width;
    unsigned int Height;
	unsigned int BitsPerPixel;
    unsigned int Type;
    //functions
	bool CompressedTGA;
	bool GetTGAHeader();
	bool LoadTGACompressed();
	bool LoadTGAUncompressed();
public:
	MTga();
	~MTga();
	unsigned char* LoadTGA(const char* inFileName);
	void FreeTga();
	unsigned int GetWidth();
	unsigned int GetHeight();
	unsigned int GetBitsPerPixel();
	unsigned char* GetImageData();
};

#endif
