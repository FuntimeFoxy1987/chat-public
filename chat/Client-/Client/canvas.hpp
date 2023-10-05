/*
Copyright (c) 2008-2012 by Andrew Stroganov <savhte@gmail.com>

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions 
are met:
Redistributions of source code must retain the above copyright 
notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright 
notice, this list of conditions and the following disclaimer in 
the documentation and/or other materials provided with the 
distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
*/

/*
russian:
Примеры и документация на русском языке в файле canvas_rus.txt

english:
See canvas_eng.txt for documentation and examples
*/

/*
ver: 0.3.5a
*/

#ifndef CANVASPP_HPP
#define CANVASPP_HPP

#include <sstream>
#include <string>
#include <windows.h>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <map>
#include <cstdlib>
#include <Windowsx.h>
#include <iostream>

#ifdef DEBUG
	#include <iostream>
	#define PRINT(s) std::cout << s << std::endl
	#define PRINT2(s1, s2) std::cout << s1 << " " << s2 << std::endl
#else
	#define PRINT(s)
	#define PRINT2(s1, s2)
#endif 

namespace cnv {

typedef unsigned int uint;
enum {nostart = 1};

class NonCopyable
{
public:
        NonCopyable() {};

private:
        NonCopyable(const NonCopyable&);
        NonCopyable& operator=(const NonCopyable&);
};

class Mutex: public NonCopyable
{
public:
	Mutex()
	{
		mutex_ = CreateMutex(NULL, FALSE, NULL);
		assert(mutex_ != NULL && "Couldn't create mutex");
	}
	
	~Mutex()
	{
		CloseHandle(mutex_);
	}

	void lock()
	{
		DWORD result = WaitForSingleObject(mutex_, INFINITE);
		assert(result != WAIT_ABANDONED && "Couldn't lock mutex");
	}

	void unlock()
	{
		ReleaseMutex(mutex_);
	}

private:
	HANDLE mutex_;
};

class ScopedLock: public NonCopyable
{
public:
	ScopedLock(Mutex& m): mutex_(m)
	{
		mutex_.lock();
	}

	~ScopedLock()
	{
		mutex_.unlock();
	}
	
private:
	Mutex& mutex_;
};

class WindowCore;

class Manager: public NonCopyable
{
public:
	static Manager& instance()
	{
		static Manager m;
		return m;
	}

	inline WindowCore* get(HWND h) const
	{
		ScopedLock lock(mutex_);
		Table::const_iterator i = windows_.find(h);
		WindowCore* w = 0;
		if(i != windows_.end()) w = i->second;
		return w;
	}

	void add(HWND h, WindowCore* w)
	{
		ScopedLock lock(mutex_);
		windows_[h] = w;
	}


private:
	Manager() {}

	typedef std::map<HWND, WindowCore*> Table;
	Table windows_;
	mutable Mutex mutex_;
};

struct Rect
{
	Rect(): x(0), y(0), width(0), height(0) {}
	Rect(int xx, int yy, int ww, int hh): x(xx), y(yy), width(ww), height(hh) {}

	bool operator==(const Rect& rhs) const 
	{ 
		return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
	}

	int x;
	int y;
	int width;
	int height;
};

inline POINT to_point(int x, int y)
{
	POINT p;
	p.x = x;
	p.y = y;
	return p;
}

struct details
{
	template <typename T> static HDC create_dc(T& v)
	{
		HDC dc = CreateCompatibleDC(0);
		if(NULL == SelectObject(dc, v))
			std::logic_error("Couldn't select bitmap object");
		return dc;
	}

	static PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
	{ 
		/*
		this function is borrowed from: 
		http://social.msdn.microsoft.com/Forums/en/vcgeneral/thread/18320092-bd6a-45b3-a53c-8dc51b409aad

		Sav.
		*/

		BITMAP bmp; 
		PBITMAPINFO pbmi; 
		WORD    cClrBits; 

		// Retrieve the bitmap color format, width, and height. 
		if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
			throw std::runtime_error("GetObject"); 

		// Convert the color format to a count of bits. 
		cClrBits  =  (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
		if (cClrBits  ==  1) cClrBits  =  1; 
		else if (cClrBits <= 4) cClrBits  =  4; 
		else if (cClrBits <= 8) cClrBits  =  8; 
		else if (cClrBits <= 16) cClrBits  =  16; 
		else if (cClrBits <= 24) cClrBits  =  24; 
		else cClrBits  =  32; 

		// Allocate memory for the BITMAPINFO structure. (This structure 
		// contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
		// data structures.) 
		if (cClrBits !=  24) 
			pbmi  =  (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1 << cClrBits)); 
		else // There is no RGBQUAD array for the 24-bit-per-pixel format. 
			pbmi  =  (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER)); 

		// Initialize the fields in the BITMAPINFO structure. 
		pbmi->bmiHeader.biSize  =  sizeof(BITMAPINFOHEADER); 
		pbmi->bmiHeader.biWidth  =  bmp.bmWidth; 
		pbmi->bmiHeader.biHeight  =  bmp.bmHeight; 
		pbmi->bmiHeader.biPlanes  =  bmp.bmPlanes; 
		pbmi->bmiHeader.biBitCount  =  bmp.bmBitsPixel; 
		if (cClrBits < 24) pbmi->bmiHeader.biClrUsed  =  (1 << cClrBits); 

		// If the bitmap is not compressed, set the BI_RGB flag. 
		pbmi->bmiHeader.biCompression  =  BI_RGB; 

		// Compute the number of bytes in the array of color 
		// indices and store the result in biSizeImage. 
		// For Windows NT, the width must be DWORD aligned unless 
		// the bitmap is RLE compressed. This example shows this. 
		// For Windows 95/98/Me, the width must be WORD aligned unless the 
		// bitmap is RLE compressed.
		pbmi->bmiHeader.biSizeImage  =  ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8 * pbmi->bmiHeader.biHeight; 
		// Set biClrImportant to 0, indicating that all of the 
		// device colors are important. 
		pbmi->bmiHeader.biClrImportant  =  0; 
		return pbmi; 
	} 

	static void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC) 
	{ 
		/*
		this function is borrowed from: 
		http://social.msdn.microsoft.com/Forums/en/vcgeneral/thread/18320092-bd6a-45b3-a53c-8dc51b409aad

		Sav.
		*/

		HANDLE hf;                 // file handle 
		BITMAPFILEHEADER hdr;       // bitmap file-header 
		PBITMAPINFOHEADER pbih;     // bitmap info-header 
		LPBYTE lpBits;              // memory pointer 
		DWORD dwTotal;              // total count of bytes 
		DWORD cb;                   // incremental count of bytes 
		BYTE *hp;                   // byte pointer 
		DWORD dwTmp; 
	
		pbih  =  (PBITMAPINFOHEADER) pbi; 
		lpBits  =  (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

		if (!lpBits) throw std::runtime_error("GlobalAlloc"); 
	
		// Retrieve the color table (RGBQUAD array) and the bits (array of palette indices) from the DIB. 
		if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, DIB_RGB_COLORS)) 
			throw std::runtime_error("GetDIBits"); 

		// Create the .BMP file. 
		hf  =  CreateFile(pszFile, GENERIC_READ | GENERIC_WRITE, (DWORD) 0, NULL, 
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL); 

		if (hf  ==  INVALID_HANDLE_VALUE) throw std::runtime_error("CreateFile"); 
		hdr.bfType  =  0x4d42;        // 0x42  =  "B" 0x4d  =  "M" 
		// Compute the size of the entire file. 
		hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof(RGBQUAD) + pbih->biSizeImage); 
		hdr.bfReserved1  =  0; 
		hdr.bfReserved2  =  0; 

		// Compute the offset to the array of color indices. 
		hdr.bfOffBits  =  (DWORD) sizeof(BITMAPFILEHEADER) + pbih->biSize + pbih->biClrUsed * sizeof (RGBQUAD); 

		// Copy the BITMAPFILEHEADER into the .BMP file. 
		if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), (LPDWORD) &dwTmp,  NULL)) 
			throw std::runtime_error("WriteFile"); 

		// Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
		if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD), (LPDWORD) &dwTmp,  NULL) )
		    throw std::runtime_error("WriteFile");

		// Copy the array of color indices into the .BMP file. 
		dwTotal  =  cb  =  pbih->biSizeImage; 
		hp  =  lpBits; 
		if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp, NULL)) 
			throw std::runtime_error("WriteFile"); 

		// Close the .BMP file. 
		if (!CloseHandle(hf)) throw std::runtime_error("CloseHandle"); 

		// Free memory. 
		GlobalFree((HGLOBAL)lpBits);
	}
};

struct Pen
{
	Pen():
		style(PS_SOLID), size(1), red(0), green(0), blue(0) {}

	Pen(int rr, int gg, int bb):
		style(PS_SOLID), size(1), red(rr), green(gg), blue(bb) {}

	Pen(int st):
		style(st), size(1), red(0), green(0), blue(0) {}

	Pen(int st, int sz, int rr, int gg, int bb):
		style(st), size(sz), red(rr), green(gg), blue(bb) {}

	COLORREF color_ref() const { return RGB(red, green, blue); }

        uint style;
        uint size;
        int red;
        int green;
        int blue;
};

struct Brush
{
        Brush():
            style(BS_NULL), hatch(HS_BDIAGONAL), red(0), green(0), blue(0) {}

        Brush(int rr, int gg, int bb):
            style(BS_SOLID), hatch(HS_BDIAGONAL), red(rr), green(gg), blue(bb) {}

        Brush(long st, int rr, int gg, int bb):
            style(st), hatch(HS_BDIAGONAL), red(rr), green(gg), blue(bb) {}

        Brush(int st, long ht, int rr, int gg, int bb):
            style(st), hatch(ht), red(rr), green(gg), blue(bb) {}

        COLORREF color_ref() const { return RGB(red, green, blue); }

        LOGBRUSH get_brush() const
        {
            LOGBRUSH b;
            b.lbStyle = style;
            b.lbColor = RGB(red, green, blue);
            b.lbHatch = hatch;
            return b;
        }

        int style;
        long hatch;
        int red;
        int green;
        int blue;
};

class Bezier
{
public:
        Bezier(int x1, int y1, int cx1, int cy1, int cx2, int cy2, int x2, int y2)
        {
            points_.push_back(to_point(x1, y1));
            points_.push_back(to_point(cx1, cy1));
            points_.push_back(to_point(cx2, cy2));
            points_.push_back(to_point(x2, y2));
        }

        void operator()(HDC hdc, const Pen& pen, const Brush& brush) const
        {
            HPEN old_pen;
            HPEN cur_pen=CreatePen(pen.style, pen.size, pen.color_ref());
            old_pen = (HPEN)SelectObject(hdc, cur_pen);
            PolyBezier(hdc, &points_[0], points_.size());
            SelectObject(hdc, old_pen);
            DeleteObject(cur_pen);
        }

protected:
            std::vector<POINT> points_;

};

class Bitmap
{
public:
	Bitmap(): bmp_(0) {}
	Bitmap(const std::string& fname): bmp_(0) { load(fname); }
	Bitmap(const Bitmap& b): bmp_(0) { assign(b); }
	Bitmap(const Bitmap& b, int x, int y, int w, int h): bmp_(0) { assign(b, x, y, w, h); }

	Bitmap& operator=(const Bitmap& b)
	{
		if(this != &b) assign(b);
		return *this;
	}

	virtual ~Bitmap() { clear(); }

	int height() const { return info().bmHeight; }
	int width() const { return info().bmWidth; }
	bool empty() const { return !bmp_; }

	void swap(Bitmap& b) { std::swap(bmp_, b.bmp_); }

	void crop(int x, int y, int w, int h)
	{
		Bitmap b(*this, x, y, w, h);
		swap(b);
	}

	inline COLORREF get_pixel(int x, int y) const 
	{ 
		if(empty()) throw std::logic_error("Image is empty");

		HDC dc = details::create_dc(bmp_);
		const COLORREF r = GetPixel(dc, x, y); 
		DeleteDC(dc);
		return r;
	}

	inline int get_pixel_r(int x, int y) const { return get_pixel(x, y) & 0xFF; }
	inline int get_pixel_g(int x, int y) const { return (get_pixel(x, y) >> 8 ) & 0xFF; }
	inline int get_pixel_b(int x, int y) const { return (get_pixel(x, y) >> 16) & 0xFF; }

	void set_pixel(int x, int y, int r = 0, int g = 0, int b = 0)
	{
		if(empty()) throw std::logic_error("Image is empty");
		const COLORREF c = ((b & 0xff) << 16) + ((g & 0xff) << 8) + (r & 0xff);
		HDC dc = details::create_dc(bmp_);
		SetPixel(dc, x, y, c);
		DeleteDC(dc);
	}

	bool load(const std::string& s)
	{
		clear();
		bmp_ = (HBITMAP) LoadImage (0, s.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		return !empty();
	}

	void stretch(int w, int h)
	{
		Bitmap b;
		BITMAP i = info();

		b.bmp_ = CreateBitmap(w, h, i.bmPlanes, i.bmBitsPixel, NULL);
		HDC dc = details::create_dc(bmp_);
		HDC dc_to = details::create_dc(b.bmp_);
		StretchBlt(dc_to, 0, 0, w, h, dc, 0, 0, i.bmWidth, i.bmHeight, SRCCOPY);
		DeleteDC(dc);
		DeleteDC(dc_to);
		swap(b);
	}

	void clear()
	{
		if(bmp_)
		{
			DeleteObject(bmp_);
			bmp_ = 0;
		}
	}

	BITMAP info() const
	{
		if(!bmp_) throw std::logic_error("Bitmap is empty");

		BITMAP info;
		memset(&info, 0, sizeof(info));
		GetObject(bmp_, sizeof(BITMAP), &info);
		return info;
	}

	HBITMAP native() const { return bmp_; }

	void assign(const Bitmap& b)
	{
		assign(b, 0, 0, b.width(), b.height());
	}

	void assign(const HBITMAP b, int x, int y, int w, int h)
	{
		if(b == bmp_) return;
		if(!bmp_ || height() != h || width() != w)
		{
			clear();

			if(!b) return;

			BITMAP bm;
        	memset(&bm, 0, sizeof(BITMAP));
        	GetObject(b, sizeof(bm), &bm);

        	// bmp_ = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel, NULL);
        	bmp_ = CreateBitmap(w, h, bm.bmPlanes, bm.bmBitsPixel, NULL);
		}

		HDC dc = details::create_dc(bmp_);
		HDC dc_from = details::create_dc(b);

        BitBlt(dc, 0, 0, w, h, dc_from, x, y, SRCCOPY);
				
		DeleteDC(dc);
		DeleteDC(dc_from);
	}

	void assign(const Bitmap& b, int x, int y, int w, int h)
	{
		if(&b == this) return;

		clear();
		if(b.empty()) return;

		BITMAP bm;
        memset(&bm, 0, sizeof(BITMAP));
        GetObject(b.bmp_, sizeof(bm), &bm);

        // bmp_ = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel, NULL);
        bmp_ = CreateBitmap(w, h, bm.bmPlanes, bm.bmBitsPixel, NULL);

		HDC dc = details::create_dc(bmp_);
		HDC dc_from = details::create_dc(b.bmp_);

        BitBlt(dc, 0, 0, w, h, dc_from, x, y, SRCCOPY);
				
		DeleteDC(dc);
		DeleteDC(dc_from);
	}

private:

	HBITMAP bmp_;
};

class Image
{
public:
	/*
	Image(const std::string& fname, int tx, int ty): toX(tx), toY(ty) 
	{ 
		bmp_.load(fname);
	}
	*/

	Image(const Bitmap& b, int tx, int ty): bmp_(b), toX(tx), toY(ty) { }

	int toX;
	int toY;

	void operator()(HDC dcOut, const Pen& pen, const Brush& brush) const
	{
		if(bmp_.empty())
		{
			const std::string err = "Image is empty!";
			TextOut(dcOut, toX, toY, err.c_str(), err.size());
			return;
		}

		const bool transparent = brush.style == BS_SOLID;
		HBITMAP hbmp = bmp_.native();
		HDC dc = details::create_dc(hbmp);//CreateCompatibleDC(NULL);
//		SelectObject(dc, bmp_.native());
		const int w = bmp_.width();
		const int h = bmp_.height();

		if(transparent)
		{
			BITMAP bm = bmp_.info();
			HBITMAP mask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
			HDC dcMask = CreateCompatibleDC(NULL);
			SelectObject(dcMask, mask);
			SetBkColor(dc, RGB(brush.red, brush.green, brush.blue));
			BitBlt(dcMask, 0, 0, bmp_.width(), bmp_.height(), dc, 0, 0, SRCCOPY);
			BitBlt(dcOut, toX, toY, w, h, dc, 0, 0, SRCINVERT);
			BitBlt(dcOut, toX, toY, w, h, dcMask, 0, 0, SRCAND);
			BitBlt(dcOut, toX, toY, w, h, dc, 0, 0, SRCINVERT);
			DeleteDC(dcMask);
			DeleteObject(mask);
                }
                else
                {
			BitBlt(dcOut, toX, toY, w, h, dc, 0, 0, SRCCOPY);
                }
               DeleteDC(dc);
	}
protected:

	/*
	HBITMAP mask(int r, int g, int b) const
	{
                HDC hdcMem, hdcMem2;
                HBITMAP hbmMask;
                BITMAP bm = info();

                hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

                hdcMem = CreateCompatibleDC(0);
                hdcMem2 = CreateCompatibleDC(0);

                SelectObject(hdcMem, bmp_);
                SelectObject(hdcMem2, hbmMask);

                SetBkColor(hdcMem, RGB(r, g, b));

                BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

                BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

                DeleteDC(hdcMem);
                DeleteDC(hdcMem2);

                return bmp_;//hbmMask;
	}
	*/

	const Bitmap& bmp_;
};


class Line
{
public:
	Line(int xx1, int yy1, int xx2, int yy2):
		x1(xx1), y1(yy1), x2(xx2), y2(yy2) {}

	Line(): x1(0), y1(0), x2(0), y2(0) {}

	int x1;
	int y1;
	int x2;
	int y2;

	void operator()(HDC hdc, const Pen& pen, const Brush& brush) const
	{
		HPEN old_pen;
		HPEN cur_pen = CreatePen(pen.style, pen.size, pen.color_ref());
		old_pen = (HPEN)SelectObject(hdc, cur_pen);
		MoveToEx(hdc, x1, y1, (LPPOINT) NULL);
		LineTo(hdc, x2, y2);
		SelectObject(hdc, old_pen);
		DeleteObject(cur_pen);
	}
};

class CnvEllipse
{
public:
        CnvEllipse(int xx1, int yy1, int xx2, int yy2):
		x1(xx1), y1(yy1), x2(xx2), y2(yy2) {}

        CnvEllipse(): x1(0), y1(0), x2(0), y2(0) {}

        int x1;
        int y1;
        int x2;
        int y2;

        void operator()(HDC hdc, const Pen& pen, const Brush& brush) const
        {
		HPEN old_pen;
		LOGBRUSH b = brush.get_brush();
		HPEN cur_pen = CreatePen(pen.style, pen.size, pen.color_ref());
		HBRUSH old_brush;
		HBRUSH cur_brush = CreateBrushIndirect(&b);
		old_brush = (HBRUSH)SelectObject(hdc, cur_brush);
		old_pen = (HPEN)SelectObject(hdc, cur_pen);
		::Ellipse(hdc, x1, y1, x2, y2);
		SelectObject(hdc, old_pen);
		SelectObject(hdc, old_brush);
		DeleteObject(cur_pen);
		DeleteObject(cur_brush);
	}
};

class Circle
{
public:
	Circle(int xx, int yy, int rr): x(xx), y(yy), r(rr) {}
        Circle(): x(0), y(0), r(0) {}

        void operator()(HDC hdc, const Pen& pen, const Brush& brush) const
        {
		CnvEllipse e(x - r, y - r, x + r, y + r);
		e(hdc, pen, brush);
        }

        int x;
        int y;
        int r;
};

class Dot
{
public:
        Dot(int xx, int yy): x(xx), y(yy) {}
        Dot(): x(0), y(0) {}

        void operator()(HDC hdc, const Pen& pen, const Brush& brush) const
        {
            Circle c(x, y, 1);
            c(hdc, pen, brush);
        }

        int x;
        int y;
};

class CnvRectangle
{
public:
	CnvRectangle(int xx1, int yy1, int xx2, int yy2):
		x1(xx1), y1(yy1), x2(xx2), y2(yy2) {}

        CnvRectangle(): x1(0), y1(0), x2(0), y2(0) {}

        void operator()(HDC hdc, const Pen& pen, const Brush& brush) const
        {
		HPEN old_pen;
		LOGBRUSH b = brush.get_brush();
		HPEN cur_pen = CreatePen(pen.style, pen.size, pen.color_ref());
		HBRUSH old_brush;
		HBRUSH cur_brush = CreateBrushIndirect(&b);
		old_brush = (HBRUSH)SelectObject(hdc, cur_brush);
		old_pen = (HPEN)SelectObject(hdc, cur_pen);
		::Rectangle(hdc, x1, y1, x2, y2);
		SelectObject(hdc, old_pen);
		SelectObject(hdc, old_brush);
		DeleteObject(cur_pen);
		DeleteObject(cur_brush);
	}

        int x1;
        int y1;
        int x2;
        int y2;
};

class Label
{
public:
        Label(): text(), x(0), y(0), height(0), width(0), orientation(0) {}

        Label(const std::string& txt, int xx, int yy, int hgt, int wdt, double orient):
            text(txt), x(xx), y(yy), height(hgt), width(wdt), orientation(orient) {}

        Label(const std::string& txt, int xx, int yy):
            text(txt), x(xx), y(yy), height(0), width(0), orientation(0) {}

        Label(const std::string& txt, int xx, int yy, int hgt):
            text(txt), x(xx), y(yy), height(hgt), width(0), orientation(0) {}

        void operator()(HDC hdc, const Pen& pen, const Brush& brush) const
        {
		int old_bk = SetBkMode(hdc, TRANSPARENT);
		HFONT cur_font = CreateFont(
				height,
				width,
				orientation*10,
				0,
				FW_DONTCARE,
				FALSE,
				FALSE,
				FALSE,
				RUSSIAN_CHARSET,
				OUT_CHARACTER_PRECIS,
				CLIP_CHARACTER_PRECIS,
				DEFAULT_QUALITY,
				FF_DONTCARE,
				0
		);
		HFONT prev_font = (HFONT)SelectObject(hdc, cur_font);
		SetTextColor(hdc, RGB(pen.red, pen.green, pen.blue));
		TextOut(hdc, x, y, text.c_str(), text.size());
		SetBkMode(hdc, old_bk);
		SelectObject(hdc, prev_font);
		DeleteObject(cur_font);
	}

        std::string text;
        int x;
        int y;
        int height;
        int width;
        double orientation;
};

class WindowCore: public NonCopyable
{
public:
	WindowCore(const std::string& title, uint w, uint h, size_t s):
		title_(title), width_(w), height_(h), wnd_(0), bmp_(0), out_bmp_(0), scale_(s), fullscreen_(false)
	{}

	uint height() const { return height_; }
	uint width() const { return width_; }

	void title(const std::string& s)
	{
		if(wnd_) throw std::logic_error("Window is already constructed");
		title_ = s;
	}

	void geometry(size_t w, size_t h, size_t z)
	{
		if(wnd_) throw std::logic_error("Window is already constructed");
		width_ = w;
		height_ = h;
		scale_ = z;
	}

	void set_fullscreen()
	{
		assert(!wnd_);
		fullscreen_ = true;
	}

	void copy_to(Bitmap& b, int x, int y, int w, int h)
	{
		b.assign(bmp_, x, y, w, h);
	}

	void start()
	{
		assert(!wnd_);
		started_ = false;
		PRINT("Creating thread");
        	thread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)loop, (void*)this, 0, &id_);

        	while(!started_) Sleep(10); // =)
		PRINT("Thread started");

		create_bitmap();
	}
        void start(WNDPROC handler)
	{
		init(this, handler);
		create_bitmap();
	}

		void start(HWND hwnd)
		{
			wnd_ = hwnd;
			assert(wnd_);
			RECT r;
			GetClientRect(wnd_, &r);
			width_ = r.right;
			height_ = r.bottom;
		}


	void paint()
    	{
        	HDC wndDc = GetDC(wnd_);
        	HDC hdc = CreateCompatibleDC(NULL);
        	SelectObject(hdc, bmp_);

			if(scale_ == 1)
        		BitBlt(wndDc, 0, 0, width_, height_, hdc, 0, 0, SRCCOPY);
			else
				StretchBlt(wndDc, 0, 0, scale_*width_, scale_*height_, hdc, 0, 0, width_, height_, SRCCOPY);

        	DeleteDC(hdc);
        	ReleaseDC(wnd_, wndDc);
    	}

	HWND wnd() { return wnd_; }
	HBITMAP bmp() { return bmp_; }

	void lock() { mutex_.lock(); }
	void unlock() { mutex_.unlock(); }

private:
	size_t actual_height() const { return height_ * scale_; }
	size_t actual_width() const { return width_ * scale_; }

	void create_bitmap()
	{
            	HDC dc = GetDC(wnd_);
            	bmp_ = CreateCompatibleBitmap(dc, width_, height_);
            	ReleaseDC(wnd_, dc);

            	dc = GetDC(wnd_);
            	out_bmp_ = CreateCompatibleBitmap(dc, scale_*width_, scale_*height_);
            	ReleaseDC(wnd_, dc);
	}

	static void init(WindowCore* w, WNDPROC handler)
	{
		static uint number = 0;
		++number;

		std::stringstream ss;
		ss << number;
		w->name_ = w->title_ + ss.str();

		PRINT2("Window name:", w->name_);

		WNDCLASS wnd;
		memset(&wnd, 0, sizeof(wnd));
		wnd.style = CS_HREDRAW|CS_VREDRAW;
		wnd.lpfnWndProc = handler;
		wnd.hInstance = NULL;
		wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
		wnd.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wnd.lpszClassName = w->name_.c_str();

		PRINT2("Registering class:", w->name_);
       	if(RegisterClass(&wnd))
       	{
			PRINT2("Creating window:", w->name_);
           	w->wnd_ = CreateWindow(
				w->name_.c_str(),
				w->title_.c_str(),
   	    		WS_SYSMENU|WS_MINIMIZEBOX |WS_CAPTION ,
   	    		CW_USEDEFAULT,
   	    		CW_USEDEFAULT,
    	   		w->width_*w->scale_ + GetSystemMetrics(SM_CXFIXEDFRAME),
      			w->height_*w->scale_ + GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION),
       			NULL,
       			NULL,
       			NULL,
       			NULL
    		);
			assert(w->wnd_ != NULL);
			PRINT2("Setting window:", w->name_);
			Manager::instance().add(w->wnd_, w);
			//std::cout << "ADDED" << std::endl;
			//std::cout << pCnv->hWnd_ << std::endl;;
			if(w->fullscreen_)
			{
				SetWindowLong(w->wnd_, GWL_STYLE, 0);
            	ShowWindow(w->wnd_, SW_SHOWMAXIMIZED);
				RECT r;
				GetClientRect(w->wnd_, &r);
				w->width_ = r.right;
				w->height_ = r.bottom;
			}
			else
            	ShowWindow(w->wnd_, SW_SHOWNORMAL);
            		UpdateWindow(w->wnd_);

        	}
        	else throw std::runtime_error("RegisterClass failed");
        }

	static DWORD loop(void* p)
    	{
		WindowCore* w = static_cast<WindowCore*>(p);
		PRINT2("Inititalizing window", w->title_);
        	init(w, wnd_proc);
		MSG msg;
		const UINT msg_started = WM_USER + 0x01;
		PostMessage(0, msg_started, 0, 0);
		PRINT2("Entering message loop for", w->title_);
		while (GetMessage(&msg, NULL, 0, 0))
		{
			if(msg.message == msg_started)
				w->started_ = true;
			DispatchMessage(&msg);
		}
        	return 0;
    	}

	static LRESULT CALLBACK wnd_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
	//	std::cout << "WndProc" << std::endl;
		WindowCore* w = Manager::instance().get(hwnd);

		//DWORD thread = GetWindowThreadProcessId(hwnd, NULL);
       		switch (uMsg)
		{
       			case WM_PAINT:
				if(w) w->paint();
				break;
           		case WM_DESTROY:
       				PostQuitMessage(0);
				break;
        	}

		if(w) w->lock();
       		const LRESULT r = DefWindowProc(hwnd, uMsg, wParam, lParam);
		if(w) w->unlock();
		return r;
	}
	
	std::string title_;
	std::string name_;
	uint width_;
	uint height_;
	HWND wnd_;
	HBITMAP bmp_;
	HBITMAP out_bmp_;
	Mutex mutex_;
	HANDLE thread_;
	DWORD id_;
	bool started_;
	size_t scale_;
	bool fullscreen_;
};

inline bool is_pressed(int key)
{
	return GetKeyState(key) < 0;
}

inline void kill_timer(size_t n)
{
	KillTimer(0, n);
}

inline int set_timer(size_t msec)
{
	return SetTimer(0, 0, msec, 0);
}

inline void sleep(unsigned int t)
{
	Sleep(t);
}

inline HWND get_preview_hwnd(int argc, char** argv)
{
	HWND h = 0;
	for(int i = 0; i < argc; ++i)
		if(strcmp(argv[i], "/P") == 0 || strcmp(argv[i], "/p") == 0)
		{
			if(i == argc - 1) break;
			const long n = atol(argv[i + 1]);
			h = reinterpret_cast<HWND>(n);
			break;
		}

	return h;
}
class Message
{
public:
	bool wait()
	{
		if(!GetMessage(&msg_, NULL, 0, 0))
			return false;
		DispatchMessage(&msg_);
		return true;
	}

	bool peek(unsigned int mode = PM_REMOVE)
	{
		const bool b = PeekMessage(&msg_, 0, 0, 0, mode);

		if(b) DispatchMessage(&msg_);

		return b;
	}

	MSG msg() const { return msg_; }
	int type() const { return msg_.message; }

	bool is_timer() const { return msg_.message == WM_TIMER; }
	bool is_paint() const { return msg_.message == WM_PAINT; }
	bool is_mousemove() const { return msg_.message == WM_MOUSEMOVE; }
	bool is_keydown() const { return msg_.message == WM_KEYDOWN; }
	bool is_destroy() const { return msg_.message == WM_DESTROY; }
	bool is_lbuttondown() const { return msg_.message == WM_LBUTTONDOWN; }
	bool is_lbuttonup() const { return msg_.message == WM_LBUTTONUP; }
	bool is_rbuttonup() const { return msg_.message == WM_RBUTTONUP; }

	int mouse_x() const { return GET_X_LPARAM(msg_.lParam); }
	int mouse_y() const { return GET_Y_LPARAM(msg_.lParam); }
	int vkey() const { return msg_.wParam; }
	int timer_id() const { return msg_.wParam; }

private:
	MSG msg_;
};

class Window: private NonCopyable
{
public:
//        typedef LRESULT(*HandlerFunction)(HWND, UINT, WPARAM, LPARAM);

	Window(): wnd_("Canvas++", 640, 480, 1), fb_enabled_(false) 
	{ 
		start();
	}

	Window(int params): wnd_("Canvas++", 640, 480, 1), fb_enabled_(false) 
	{
	}

	Window(const std::string& s): wnd_(s, 640, 480, 1), fb_enabled_(false)
	{
		start();
	}

	Window(const std::string& s, int w, int h, int z = 1): wnd_(s, w, h, z), fb_enabled_(false)
	{
		start();
	}

	Window(int w, int h, int z = 1): wnd_("Canvas++", w, h, z), fb_enabled_(false)
	{
		start();
	}

	/*
	Window(const std::string& s, start_t st = auto_start): wnd_(s, 640, 480, 1), fb_enabled_(false) { on_create(st); }
    	Window(uint w, uint h, size_t sc = 1): wnd_("Canvas", w, h, sc), fb_enabled_(false) { on_create(); }
	Window(const std::string& s, uint w, uint h, size_t sc = 1): wnd_(s, w, h, sc), fb_enabled_(false) { on_create(); }
	*/

	HWND hwnd() { return wnd_.wnd(); }

	~Window()
	{
        	//PostThreadMessage(threadId_, GRPH_MESSAGE_DESTROY, 0, 0);
	}

	void set_fullscreen()
	{
		wnd_.set_fullscreen();
	}

	void title(const std::string& s)
	{
		wnd_.title(s);
	}

	void geometry(size_t w, size_t h, size_t z = 1)
	{
		wnd_.geometry(w, h, z);
	}

	void start(WNDPROC handler)
	{
		wnd_.start(handler);
		clear();
	}

	void start(HWND hwnd)
	{
		wnd_.start(hwnd);
		clear();
	}

	void paint()
	{
		lock();
		wnd_.paint();
		unlock();
	}

	template<typename F> void draw(const F& f, const Pen& pen, const Brush& brush)
    {
		lock();
       		HDC hdc = CreateCompatibleDC(NULL);
       		SelectObject(hdc, wnd_.bmp());
       		f(hdc, pen, brush);
       		DeleteDC(hdc);
       		if(!fb_enabled_) wnd_.paint();
		unlock();
    }

	template<typename F> void draw(const F& f, const Pen& pen) { draw(f, pen, brush_); }
	template<typename F> void draw(const F& f, const Brush& brush) { draw(f, pen_, brush); }
	template<typename F> void draw(const F& f) { draw(f, pen_, brush_); }

	void copy_to(Bitmap& b, int x, int y, int w, int h)
	{
		wnd_.copy_to(b, x, y, w, h);
	}

	void clear(int r = 255, int g = 255, int b = 255)
    {
       	RECT rect;
       	rect.top = 0;
       	rect.left = 0;
       	rect.right = wnd_.width();
       	rect.bottom = wnd_.height();

		lock();
        	HDC hdc = CreateCompatibleDC(NULL);
        	SelectObject(hdc, wnd_.bmp());
        	HBRUSH br = CreateSolidBrush(RGB(r, g, b));
        	FillRect(hdc, &rect, br);
        	DeleteObject(br);
        	DeleteDC(hdc);
        	if(!fb_enabled_) wnd_.paint();
		unlock();
    }

    inline uint height() const { return wnd_.height(); }
    inline uint width() const { return wnd_.width(); }

    void pen(const Pen& p) { pen_ = p; }
    Pen pen() const { return pen_; }

    void brush(const Brush& b) { brush_ = b; }
    Brush brush() const { return brush_; }
	
    void enable_frame_buffer() { fb_enabled_ = true; }
    void disable_frame_buffer() { fb_enabled_ = false; }

	void save(const std::string& fname)
	{
		lock();
		HDC hdc = GetDC(wnd_.wnd());
		HDC hdc_mem = CreateCompatibleDC(hdc);
		RECT r;
		GetClientRect(wnd_.wnd(), &r);
		HBITMAP hbmp = CreateCompatibleBitmap(hdc, r.right - r.left, r.bottom - r.top);
		HBITMAP holdbmp = (HBITMAP)SelectObject(hdc_mem, hbmp);
		BitBlt(hdc_mem, 0, 0, r.right - r.left, r.bottom - r.top, hdc, r.left, r.top, SRCCOPY);
		hbmp = (HBITMAP)SelectObject(hdc_mem, holdbmp);

		PBITMAPINFO pbi = details::CreateBitmapInfoStruct(wnd_.wnd(), hbmp);
		details::CreateBMPFile(wnd_.wnd(), (char*)fname.c_str(), pbi, hbmp, hdc_mem);
		DeleteDC(hdc_mem);
		DeleteObject(hbmp);
		DeleteObject(holdbmp);
		ReleaseDC(wnd_.wnd(), hdc);
		DeleteObject(hbmp);
		unlock();
	}


private:
	void start()
	{
		wnd_.start();
		clear();
	}

	inline void lock() { wnd_.lock(); }
	inline void unlock() { wnd_.unlock(); }
	
	WindowCore wnd_;
	Pen pen_;
	Brush brush_;
	bool fb_enabled_;
};

static LRESULT CALLBACK default_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WindowCore* w = Manager::instance().get(hwnd);

   	switch (uMsg)
	{
   		case WM_PAINT:
			if(w) w->paint();
			break;
       	case WM_DESTROY:
   			PostQuitMessage(0);
			break;
	}

	const LRESULT r = DefWindowProc(hwnd, uMsg, wParam, lParam);
	return r;
}

template <typename T> std::string to_string(const T& t)
{
	std::stringstream s;
	s << t;
	return s.str();
}

} //namespace cnv

using cnv::Window;

using cnv::Brush;
using cnv::Pen;

using cnv::Circle;
using cnv::Line;
using cnv::CnvRectangle;
using cnv::CnvEllipse;
using cnv::Bezier;
using cnv::Bitmap;
using cnv::Image;
using cnv::Dot;
using cnv::Label;

using cnv::Message;

using cnv::is_pressed;
using cnv::set_timer;
using cnv::kill_timer;
using cnv::get_preview_hwnd;
using cnv::nostart;
using cnv::default_proc;


using cnv::sleep;
using cnv::to_string;

#endif //CANVASPP_HPP

