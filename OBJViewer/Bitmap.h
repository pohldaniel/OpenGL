#include <Windows.h>

class Bitmap {
public:
    //variables
    unsigned char *data;
	unsigned char *(*dataMatrix);
    int width,height;
    //methods
	Bitmap();
	~Bitmap();
	bool loadBitmap24(const char *filename);
	bool loadBitmap24B(const char *filename);
	bool readMonochrome(const char *filename);

private:
    //variables
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    

};