#ifndef pngH
#define pngH

#include <png.h>

class MPng
{
private:
	png_uint_32  width, height;
    int bit_depth, color_type, image_channels;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	double display_exponent;
	double LUT_exponent;
	double CRT_exponent;
	FILE* pFile;
	
	unsigned char* pImageData;
    unsigned long Width;
    unsigned long Height;
	
	int ReadPngInit(FILE* infile, unsigned long* pWidth, unsigned long* pHeight);
	int ReadPngGetBgcolor(unsigned char* red, unsigned char* green, unsigned char* blue);
	unsigned char* ReadPngGetImage(double display_exponent, int *pChannels, unsigned long *pRowbytes);
	void ReadPngCleanup(int free_image_data);
public:
	MPng();
	~MPng();
	unsigned char* LoadPNG(const char* inFileName);
	void FreePng();
	unsigned long GetWidth();
	unsigned long GetHeight();
	unsigned int GetBitsPerPixel();
	unsigned char* GetImageData();
};

#endif
