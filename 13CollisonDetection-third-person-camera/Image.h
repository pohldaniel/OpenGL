//-----------------------------------------------------------------------------
#ifndef IMAGE_H
#define IMAGE_H

#include <windows.h>
#include <tchar.h>


class Image{

public:
	HDC dc;
	HBITMAP hBitmap;
	int width;
	int height;
	int pitch;
	BITMAPINFO info;

	Image();
	Image(const Image &bitmap);
	~Image();

	Image &operator=(const Image &bitmap);

	BYTE *operator[](int row) const
	{
		return &m_pBits[pitch * row];
	}

	void blt(HDC hdcDest);
	void blt(HDC hdcDest, int x, int y);
	void blt(HDC hdcDest, int x, int y, int w, int h);
	void blt(HDC hdcDest, const RECT &rcDest, const RECT &rcSrc);

	bool clone(const Image &bitmap);
	bool create(int widthPixels, int heightPixels);
	void destroy();

	void fill(int r, int g, int b, int a);
	void fill(float r, float g, float b, float a);

	BYTE *getPixel(int x, int y) const
	{
		return &m_pBits[(y * pitch) + (x * 4)];
	}

	BYTE *getPixels() const
	{
		return m_pBits;
	}

	bool loadDesktop();
	bool loadBitmap(LPCTSTR pszFilename);
	bool loadPicture(LPCTSTR pszFilename);
	bool loadTarga(LPCTSTR pszFilename);

	bool saveBitmap(LPCTSTR pszFilename) const;
	bool saveTarga(LPCTSTR pszFilename) const;

	void selectObject();
	void deselectObject();

	void copyBytes24Bit(BYTE *pDest) const;
	void copyBytes32Bit(BYTE *pDest) const;

	void copyBytesAlpha8Bit(BYTE *pDest) const;
	void copyBytesAlpha32Bit(BYTE *pDest) const;

	void setPixels(const BYTE *pPixels, int w, int h, int bytesPerPixel);

	void flipHorizontal();
	void flipVertical();

	void resize(int newWidth, int newHeight);

private:
	DWORD createPixel(int r, int g, int b, int a) const;
	DWORD createPixel(float r, float g, float b, float a) const;

	static const int HIMETRIC_INCH = 2540; // matches constant in MFC CDC class

	static int m_logpixelsx;
	static int m_logpixelsy;

	HGDIOBJ m_hPrevObj;
	BYTE *m_pBits;
};

#endif