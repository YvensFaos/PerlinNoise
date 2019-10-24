#pragma once

#include <random>

#define smooth(a) ((3.0f - 2.0f * a) * a * a)
#define clamp(a) std::max(0.0f, (std::min(a, 1.0f)))

class PerlinNoise {
	public:
		static void generatePerlinNoise(const unsigned int textWidth, const unsigned int textHeight, const unsigned int gridWidth, const unsigned int gridHeight, float* textureArray) {
			float gradient[gridWidth + 1][gridHeight + 1][2];
			float g0, g1, gmag;

			for(unsigned int i = 0; i < gridWidth + 1; i++)
			{
				memset(gradient[i], 0.0f, (gridHeight + 1) * sizeof(float));
				for(unsigned int j = 0; j < (gridHeight + 1); j++)
				{
					g0 = ((rand() / double(RAND_MAX)) * 2 - 1);
					g1 = ((rand() / double(RAND_MAX)) * 2 - 1);
					gmag = sqrt(g0*g0 + g1*g1);
					gradient[i][j][0] = g0 / gmag;
					gradient[i][j][1] = g1 / gmag;
				}
			}

			int x0, x1, y0, y1;
			float x, y, n0, n1, u, v, value;
			float stepX = gridWidth / (float) textWidth;
			float stepY = gridHeight / (float) textHeight;

			for(unsigned int i = 0; i < textWidth; i++)
			{
				for(unsigned int j = 0; j < textHeight; j++)
				{
					x = i * stepX;
					y = j * stepY;

					x0 = (int) x;
					x1 = x0 + 1;

					y0 = (int) y;
					y1 = y0 + 1;

					float sx = x - static_cast<float>(x0);
					float sy = y - static_cast<float>(y0);

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

					textureArray[i * textHeight * 4 + j * 4 + 0] = value;
					textureArray[i * textHeight * 4 + j * 4 + 1] = value;
					textureArray[i * textHeight * 4 + j * 4 + 2] = value;
					textureArray[i * textHeight * 4 + j * 4 + 3] = 1.0f;
				}
			}
		}
};