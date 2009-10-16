#include "stdafx.h"
#include "Thread.h"
#include <complex>
#include <cmath>

// CThread

void CThread::start()
{
	stopped = false;
	handle = CreateThread( NULL, 0, get_routine(), this, 0, NULL );
	if( handle == NULL)
		throw "Thread not created";
}

CThread::~CThread()
{
	if( handle != NULL)
	    CloseHandle( handle );
}

// CCalculatingThread

CCalculationThread::CCalculationThread(HWND hwnd, UINT width, UINT height, UINT iters_per_point, int draw_style, float x_min, float x_max, float y_min, float y_max)
	: dlg_hwnd(hwnd), pixels(NULL),
      width(width), height(height), iters_per_point(iters_per_point),
      draw_style(draw_style),
      x_min(x_min), x_max(x_max), y_min(y_min), y_max(y_max)
{
	pixels = new RGBQUAD[width*height];
	memset(pixels, 0, width*height*sizeof(RGBQUAD));
}

enum DRAW_STYLE
{
	DRAW_STYLE_GRAYSCALE,
	DRAW_STYLE_FORMULA,
	DRAW_STYLE_MODULO,
};

RGBQUAD mandelbrot_color(float x, float y, unsigned iters_per_point, DRAW_STYLE style)
{
	std::complex<double> z(0, 0); // Complex plane point
	std::complex<double> c((double)x, (double)y);
	unsigned iter = 0;
	for(; iter < iters_per_point; ++iter)
	{
		z = z*z + c;
		if( z.real()*z.real() + z.imag()*z.imag() > 4) // if |z| > 2
			break;
	}
	BYTE lightness = (BYTE)( 255 - logf( (float)iter + 1 )/logf( (float)iters_per_point + 1 )*255 );
	DWORD color;
	
	const unsigned colors_count = 12;
	DWORD colors[colors_count] = 
	{
		RGB(255, 0, 0),
		RGB(0, 0, 255),
		RGB(0, 255, 0),

		RGB(0, 255, 255),
		RGB(255, 0, 255),
		RGB(255, 255, 0),

		RGB(128, 255, 255),
		RGB(255, 128, 255),
		RGB(255, 255, 128),

		RGB(255, 128, 128),
		RGB(128, 255, 128),
		RGB(128, 128, 255),
	};
	switch(style)
	{
	default:
	case DRAW_STYLE_GRAYSCALE:
		color = RGB(lightness, lightness, lightness);
		break;
	case DRAW_STYLE_FORMULA:
		if(lightness == 0)
			color = RGB(0,0,0);
		else
		    //color = RGB(255 - lightness,lightness,50 % (lightness + 1) * 3);
		    color = RGB(lightness, 255 - lightness, 255 - 50 % (lightness + 1) * 3);
		break;
	case DRAW_STYLE_MODULO:
		if(lightness == 0)
			color = RGB(0,0,0);
		else
			color = colors[lightness % colors_count];
		break;
	}

	return *reinterpret_cast<RGBQUAD*>(&color);
}

DWORD __stdcall CCalculationThread::routine( void * param )
{
	CCalculationThread *thread = static_cast<CCalculationThread*>( param );
	UINT width = thread->get_width();
	UINT height = thread->get_height();
    float x_min, x_max, y_min, y_max;
    thread->get_bounds(x_min, x_max, y_min, y_max);
	OutputDebugString(_T("Calculating Thread Started\n"));


	for(unsigned j = 0; j < height; ++j)
	{
		for(unsigned i = 0; i < width; ++i)
		{
			if( thread->is_stopped() )
			{
				OutputDebugString(_T("\nCalculating Thread Interrupted\n"));
				return 1;
			}
			float x = static_cast<float>(i)*(x_max - x_min)/width + x_min;
            float y = static_cast<float>(j)*(y_max - y_min)/height + y_min;
			thread->pixels[j*width + i] =
                mandelbrot_color( x, y, thread->iters_per_point, static_cast<DRAW_STYLE>(thread->get_style()) );
		}
		OutputDebugString(_T("."));
		::PostMessage(thread->get_hwnd(), MSG_PROGRESS, 0, static_cast<int>(j+1));
	}
	Sleep(100);
	OutputDebugString(_T("\nCalculating Thread Finished\n"));
	::PostMessage(thread->get_hwnd(), MSG_FINISHED, 0, 0);
	return 0;
}

CCalculationThread::~CCalculationThread()
{
	ASSERT( pixels != NULL);
	delete[] pixels;
}
