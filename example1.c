/* example1.c                                                      */
/*                                                                 */
/* This small program shows how to print a rotated string with the */
/* FreeType 2 library.                                             */


#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "FreeImage.h"


#define WIDTH   640
#define HEIGHT  480


/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];


/* Replace this function with something useful. */

void
	draw_bitmap(FT_Bitmap*  bitmap,
	FT_Int      x,
	FT_Int      y)
{
	FT_Int  i, j, p, q;
	FT_Int  x_max = x + bitmap->width;
	FT_Int  y_max = y + bitmap->rows;


	for (i = x, p = 0; i < x_max; i++, p++)
	{
		for (j = y, q = 0; j < y_max; j++, q++)
		{
			if (i < 0 || j < 0 ||
				i >= WIDTH || j >= HEIGHT)
				continue;

			image[j][i] |= bitmap->buffer[q * bitmap->width + p];
		}
	}
}

void
	show_image(void)
{
	int  i, j;


	for (i = 0; i < HEIGHT; i++)
	{
		for (j = 0; j < WIDTH; j++)
			putchar(image[i][j] == 0 ? ' '
			: image[i][j] < 128 ? '+'
			: '*');
		putchar('\n');
	}
}

inline BOOL AtxTopBottomMirror(unsigned char* pSrc, int nCols, int nRows, int nColor){
	if (!pSrc || nCols < 1 || nRows < 1)
		return FALSE;

	int i, byte0, byte1; unsigned char *p0, *q0, *line;
	byte0 = nCols*nColor;
	byte1 = byte0*sizeof(unsigned char);
	line  = new unsigned char[byte0];

	p0 = pSrc; q0 = pSrc + (nRows-1)*byte0;
	for (i=0; i<nRows/2; i++){
		memcpy(line, p0, byte1);
		memcpy(p0,   q0, byte1);
		memcpy(q0, line, byte1);
		p0 += byte0; q0 -= byte0;
	}

	if (line) delete[] line;
	return TRUE;
};


int
	main(int     argc,
	char**  argv)
{
	FT_Library    library;
	FT_Face       face;

	FT_GlyphSlot  slot;
	FT_Matrix     matrix;                 /* transformation matrix */
	FT_Vector     pen;                    /* untransformed origin  */
	FT_Error      error;

	char*         filename;
	char*         text;

	double        angle;
	int           target_height;
	int           n, num_chars;


	//if (argc != 3)
	//{
	//	fprintf(stderr, "usage: %s font sample-text\n", argv[0]);
	//	exit(1);
	//}
	char strFilename[512] = "C:\\Users\\wangf.5FUN\\Desktop\\freetypeEx\\test\\Debug\\simsun.ttc";
	char strText[512] = "ABC";
  	filename = strFilename;//argv[1];                           /* first argument     */
	text = strText;//argv[2];                           /* second argument    */
	num_chars = strlen(text);
	angle = (25.0 / 360) * 3.14159 * 2;      /* use 25 degrees     */
	target_height = HEIGHT;

	error = FT_Init_FreeType(&library);              /* initialize library */
	/* error handling omitted */

	error = FT_New_Face(library, filename, 0, &face);/* create face object */
	/* error handling omitted */

	/* use 50pt at 100dpi */
	error = FT_Set_Char_Size(face, 50 * 64, 0,
		100, 0);                /* set character size */
	/* error handling omitted */

	slot = face->glyph;

	/* set up matrix */
	matrix.xx = (FT_Fixed)(cos(angle) * 0x10000L);
	matrix.xy = (FT_Fixed)(-sin(angle) * 0x10000L);
	matrix.yx = (FT_Fixed)(sin(angle) * 0x10000L);
	matrix.yy = (FT_Fixed)(cos(angle) * 0x10000L);

	/* the pen position in 26.6 cartesian space coordinates; */
	/* start at (300,200) relative to the upper left corner  */
	pen.x = 300 * 64;
	pen.y = (target_height - 200) * 64;

	for (n = 0; n < num_chars; n++)
	{
		/* set transformation */
		//FT_Set_Transform(face, &matrix, &pen);

		/* load glyph image into the slot (erase previous one) */
		error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
		if (error)
			continue;                 /* ignore errors */

		/* now, draw to our target surface (convert position) */
		draw_bitmap(&slot->bitmap,
			slot->bitmap_left,
			target_height - slot->bitmap_top);

		/* increment pen position */
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;

		{
			char imgpath[512] = "";
			sprintf(imgpath, "C:\\font%d.png", n);
			int width = (&slot->bitmap)->width;
			int height = (&slot->bitmap)->rows;
			unsigned char *mpixels =new unsigned char[width*height];
			FIBITMAP* bitmap =FreeImage_Allocate(width, height, 32, 8, 8, 8);

			memcpy(mpixels, (&slot->bitmap)->buffer, width*height);
			AtxTopBottomMirror(mpixels, width, height, 1);

			for(unsigned y =0; y<FreeImage_GetHeight(bitmap); y++)
			{
				BYTE *bits = FreeImage_GetScanLine(bitmap, y);
				for(unsigned x =0; x<FreeImage_GetWidth(bitmap); x++)
				{
					bits[0] = (BYTE)mpixels[y*width+x];//(&slot->bitmap)->buffer[y*width+x];
					bits[1] = (BYTE)mpixels[y*width+x];//(&slot->bitmap)->buffer[y*width+x];
					bits[2] = (BYTE)mpixels[y*width+x];//(&slot->bitmap)->buffer[y*width+x];
					bits[3] = 255;
					bits +=4;
				}
			}

			bool bSuccess = FreeImage_Save(FIF_PNG, bitmap, imgpath, PNG_DEFAULT) == TRUE;
			FreeImage_Unload(bitmap);

			if (mpixels)
				delete [] mpixels;
		}
	}

	//show_image();

	FT_Done_Face(face);
	FT_Done_FreeType(library);

	return 0;
}

/* EOF */