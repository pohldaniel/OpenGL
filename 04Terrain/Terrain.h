#include <Windows.h>
#include <iostream>
				
//represents a terrain, by storing a set of heights and normals at 2D locations
class Terrain {
	public:
		int				width, length;			
		int				widthHalf, lengthHalf;			
		float			**terrainHeightData;	//heightData
		Terrain();
		~Terrain();
		bool loadTerrain(char *filename, float scaleHeight);
};
