#include <iostream>
#include <assert.h>
#include <fstream>
#include "Terrain.h"

Terrain::Terrain()
{
	Terrain::terrainHeightData = NULL;				
}

Terrain::~Terrain() 
{
	if(Terrain::terrainHeightData){
		for(int j=0;j<Terrain::length; j++  )
		{
		free(Terrain::terrainHeightData[j]);
		Terrain::terrainHeightData[j]=NULL;
		}
		free(Terrain::terrainHeightData);
		Terrain::terrainHeightData=NULL;
	}
}


bool Terrain::loadTerrain(char* filename, float scaleHeight)
{	
	BITMAPFILEHEADER	bmfh;
	BITMAPINFOHEADER    bmih;
	FILE				*filePtr;				// the file pointer
	unsigned char		*data;					// storing the image data
	int					padWidth;				// widht of a padded row
	int					paddingByte;			// number of bytes to fill up the row to a multiple of fou

	// open filename in "read binary" mode
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return false;

	// read the bitmap file header
	fread(&(bmfh), sizeof(BITMAPFILEHEADER), 1, filePtr);
	
	// verify that this is a bitmap by checking for the universal bitmap id
	if (bmfh.bfType != 0x4D42)
	{
		fclose(filePtr);
		return false;
	}

	// read the bitmap information header
	fread(&(bmih), sizeof(BITMAPINFOHEADER), 1, filePtr);
	Terrain::width = bmih.biWidth;
	Terrain::length =bmih.biHeight;
	Terrain::widthHalf = bmih.biWidth/2;
	Terrain::lengthHalf =bmih.biHeight/2;
	
	padWidth = 3*bmih.biWidth;
	 while(padWidth%4!=0) {
        padWidth++;
    }

	paddingByte=padWidth-3*bmih.biWidth;

	// move file pointer to beginning of bitmap data
	fseek(filePtr, bmfh.bfOffBits, SEEK_SET);

	// allocate enough memory for the bitmap image data
	data = (unsigned char*)malloc(bmih.biSizeImage);
	
	// verify memory allocation
	if (!data)
	{
		free(data);
		fclose(filePtr);
		return false;
	}

	// read in the bitmap image data
	fread(data, 1, (bmih).biSizeImage, filePtr);

	// make sure bitmap image data was read
	if (data == NULL)
	{
		fclose(filePtr);
		return false;
	}
	
	// allocate enough memory for the B-color data
	Terrain::terrainHeightData = (float**)malloc(bmih.biHeight* sizeof(float*) );

	for(int j=0; j<bmih.biHeight; j++  )
	{
	   Terrain::terrainHeightData[j] = (float *)malloc(bmih.biWidth* sizeof(float) );
	}

		
	// calculating the heigh values, the heights of the terrain range from
	//-height / 2 to height / 2.
	for(int j=0;j<bmih.biHeight;j++) {

		for(int i=0;i<bmih.biWidth;i++) {
			float h = scaleHeight *( (float)data[j*padWidth + (3*i)]-127);
			Terrain::terrainHeightData[j][i] =h;
		}
	}

	// close the file and return the bitmap image data
	fclose(filePtr);
	free(data);
	return true;
}

