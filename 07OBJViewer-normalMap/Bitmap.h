#ifndef __bitmapH__
#define __bitmapH__

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

	BYTE *getPixels() const
	{
		return m_pBits;
	}
	void flipVertical();
	bool loadPicture(LPCTSTR pszFilename);
	bool create(int widthPixels, int heightPixels);
	void destroy();
	void selectObject();
	void deselectObject();

	HDC dc;
	HBITMAP hBitmap;
	HGDIOBJ m_hPrevObj;
	BYTE *m_pBits;
	int pitch;
	BITMAPINFO info;
	 int m_logpixelsx;
	 int m_logpixelsy;

private:
    //variables
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    
	

};

#endif // __bitmapH__