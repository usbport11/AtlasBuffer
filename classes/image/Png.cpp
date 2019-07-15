#include "../../stdafx.h"
#include "Png.h"

MPng::MPng()
{
	pImageData = NULL;
	LUT_exponent = 1.0;
	CRT_exponent = 2.2;
}

MPng::~MPng()
{
}

unsigned char* MPng::LoadPNG(const char* inFileName)
{
	if(!inFileName) return NULL;
	pFile = fopen(inFileName, "rb");
	if(!pFile)
	{
		LogFile<<"Can not open png file"<<endl;
		return NULL;
	}
	
	char *p;
	unsigned char bg_red=0, bg_green=0, bg_blue=0;
	unsigned long image_rowbytes;
	if ((p = getenv("SCREEN_GAMMA")) != NULL) display_exponent = atof(p);
	else display_exponent = LUT_exponent * CRT_exponent;
	int rc = ReadPngInit(pFile, &Width, &Height);
	//LogFile<<"Png width: "<<Width<<endl;
	//LogFile<<"Png height: "<<Height<<endl;
	//LogFile<<"Png display gamma: "<<display_exponent<<endl;
	if(rc != 0)
	{
		switch (rc)
		{
			case 1:
				LogFile<<"This is not a PNG file: incorrect signature"<<endl;
				return NULL;
			case 2:
				LogFile<<"There is bad IHDR (libpng longjmp)"<<endl;
				return NULL;
			case 4:
				LogFile<<"Insufficient memory"<<endl;
				return NULL;
			default:
				LogFile<<"Unknown readpng_init() error"<<endl;
				return NULL;
		}
	}
	if(ReadPngGetBgcolor(&bg_red, &bg_green, &bg_blue) > 1)
	{
		ReadPngCleanup(true);//TRUE
		LogFile<<"ReadPngGetBgcolor error"<<endl;
		return NULL;
	}
	//LogFile<<"Png bgcolor red: "<<(int)bg_red<<" green: "<<(int)bg_green<<" blue: "<<(int)bg_blue<<endl;
	pImageData = ReadPngGetImage(display_exponent, &image_channels, &image_rowbytes);
	//LogFile<<"Png image channels: "<<image_channels<<" image rowbytes: "<<image_rowbytes<<endl;
	ReadPngCleanup(false);//FALSE
	if(!pImageData)
	{
		LogFile<<"Png no data readed"<<endl;
		return NULL;
	}
}

unsigned long MPng::GetWidth()
{
	return Width;
}

unsigned long MPng::GetHeight()
{
	return Height;
}

unsigned int MPng::GetBitsPerPixel()
{
	return bit_depth * image_channels;
}

unsigned char* MPng::GetImageData()
{
	return pImageData;
}

void MPng::FreePng()
{
	if(pImageData) free(pImageData);
}

int MPng::ReadPngInit(FILE* infile, unsigned long* pWidth, unsigned long* pHeight)
{
    unsigned char sig[8];

    fread(sig, 1, 8, infile);
    if (!png_check_sig(sig, 8))
        return 1;   /* bad signature */

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        return 4;   /* out of memory */

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
	{
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 4;   /* out of memory */
    }

    if (setjmp(png_jmpbuf(png_ptr)))
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 2;
    }

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
    //LogFile<<"Png bit per channel: "<<bit_depth<<" color type: "<<color_type<<endl;
    *pWidth = width;
    *pHeight = height;

    return 0;
}

int MPng::ReadPngGetBgcolor(unsigned char* red, unsigned char* green, unsigned char* blue)
{
    png_color_16p pBackground;
    
    if (setjmp(png_jmpbuf(png_ptr)))
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 2;
    }

    if (!png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD)) return 1;

    png_get_bKGD(png_ptr, info_ptr, &pBackground);

    if (bit_depth == 16) 
	{
        *red   = pBackground->red   >> 8;
        *green = pBackground->green >> 8;
        *blue  = pBackground->blue  >> 8;
    } 
	else if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) 
	{
        if (bit_depth == 1)
            *red = *green = *blue = pBackground->gray? 255 : 0;
        else if (bit_depth == 2)
            *red = *green = *blue = (255/3) * pBackground->gray;
        else /* bit_depth == 4 */
            *red = *green = *blue = (255/15) * pBackground->gray;
    } 
	else
	{
        *red   = (unsigned char)pBackground->red;
        *green = (unsigned char)pBackground->green;
        *blue  = (unsigned char)pBackground->blue;
    }

    return 0;
}

unsigned char *MPng::ReadPngGetImage(double display_exponent, int* pChannels, unsigned long* pRowbytes)
{
    double  gamma;
    png_uint_32  i, rowbytes;
    png_bytepp  row_pointers = NULL;

    if (setjmp(png_jmpbuf(png_ptr)))
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_expand(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand(png_ptr);
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
        png_set_expand(png_ptr);
    if (bit_depth == 16)
        png_set_strip_16(png_ptr);
    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    if (png_get_gAMA(png_ptr, info_ptr, &gamma))
        png_set_gamma(png_ptr, display_exponent, gamma);

    png_read_update_info(png_ptr, info_ptr);

    *pRowbytes = rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    *pChannels = (int)png_get_channels(png_ptr, info_ptr);

    if ((pImageData = (unsigned char *)malloc(rowbytes*height)) == NULL)
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }
    if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL)
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(pImageData);
        pImageData = NULL;
        return NULL;
    }
    //for (i = 0;  i < height;  ++i)
    //    row_pointers[i] = pImageData + i*rowbytes;
	for (i = 0;  i < height;  ++i)
        row_pointers[height-i-1] = pImageData + i*rowbytes;
		
    png_read_image(png_ptr, row_pointers);
    free(row_pointers);
    row_pointers = NULL;
    png_read_end(png_ptr, NULL);
    return pImageData;
}

void MPng::ReadPngCleanup(int free_image_data)
{
    if (free_image_data && pImageData) 
	{
        free(pImageData);
        pImageData = NULL;
    }
    if (png_ptr && info_ptr) 
	{
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        png_ptr = NULL;
        info_ptr = NULL;
    }
}

