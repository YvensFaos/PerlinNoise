// Include standard headers
#include <stdio.h>
#include <string>
#include <random>
#include <time.h>
#include "FreeImage.h"

#define min(a,b) (a < b) ? a : b
#define max(a,b) (a > b) ? a : b
#define smooth(a) ((3 - 2 * a) * a * a)
#define clamp(a) max(0, (min(a, 1)))

int main(int argc, char *argv[])
{
	if(argc < 5) {
		printf("Error! At least 5 parameters (texture width, texture height, grid width, grid height and file) should be given. Number of parameters: %d.\n", argc);
		return -1;
	}
	int textWidth = atoi(argv[1]);
	int textHeight = atoi(argv[2]);
	int gridWidth = atoi(argv[3]);
	int gridHeight = atoi(argv[4]);	

	std::string fileName(argv[5]);

	if(argc > 6) {
		srand(atoi(argv[6]));
	} else {
		srand(time(NULL));
	}

	float gradient[gridWidth + 1][gridHeight + 1][2];

	float g0, g1, gmag;
	for(size_t i = 0; i < gridWidth + 1; i++)
	{
		memset(gradient[i], 0, (gridHeight + 1) * sizeof(float));
		for(size_t j = 0; j < (gridHeight + 1); j++)
		{
			g0 = ((rand() / double(RAND_MAX)) * 2 - 1);
			g1 = ((rand() / double(RAND_MAX)) * 2 - 1);
			gmag = sqrt(g0*g0 + g1*g1);
			gradient[i][j][0] = g0 / gmag;
			gradient[i][j][1] = g1 / gmag;
		}
	}

	FreeImage_Initialise();
	FIBITMAP* bitmap = FreeImage_Allocate(textWidth, textHeight, 24);
	RGBQUAD color;

	if(!bitmap) {
		printf("Unable to create bitmap.\n");
		return -1;
	}

	int x0, x1, y0, y1;
	float x, y, n0, n1, u, v, value;
	float stepX = gridWidth / (float) textWidth;
	float stepY = gridHeight / (float) textHeight;

	for(size_t i = 0; i < textWidth; i++)
	{
		for(size_t j = 0; j < textHeight; j++)
		{
			x = i * stepX;
			y = j * stepY;

			x0 = (int) x;
			x1 = x0 + 1;

			y0 = (int) y;
			y1 = y0 + 1;

			float sx = x - (float)x0;
			float sy = y - (float)y0;

			sx = smooth(sx);
			sy = smooth(sy);

			n0 = (x - (float)x0)*gradient[x0][y0][0] + (y - (float)y0)*gradient[x0][y0][1];
			n1 = (x - (float)x1)*gradient[x1][y0][0] + (y - (float)y0)*gradient[x1][y0][1];
			u = n0 + sx * (n1 - n0);

			n0 = (x - (float)x0)*gradient[x0][y1][0] + (y - (float)y1)*gradient[x0][y1][1];
			n1 = (x - (float)x1)*gradient[x1][y1][0] + (y - (float)y1)*gradient[x1][y1][1];
			v = n0 + sx * (n1 - n0);
			value = u + sy * (v - u);
			value = clamp(((value * 2.0f) + 1) / 2.0f);

			color.rgbRed   = value * 255.0;
			color.rgbGreen = value * 255.0;
			color.rgbBlue  = value * 255.0;
			FreeImage_SetPixelColor(bitmap, i, j, &color);
		}
	}

	if(FreeImage_Save(FIF_PNG, bitmap, fileName.c_str(), 0)) {
		printf("Image %s successfully saved.", fileName.c_str());
	}
	FreeImage_DeInitialise();

	return 0;
}