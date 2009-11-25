// Preview.cpp : implementation file
//

#include "stdafx.h"
#include "Fractal.h"
#include "Preview.h"

namespace
{
    const COLORREF PREVIEW_FRAME_COLOR  = RGB(200,20,2);
    const COLORREF PREVIEW_BKG_COLOR    = RGB(220,220,220);
}

// CPreview

IMPLEMENT_DYNAMIC(CPreview, CStatic)

CPreview::CPreview()
: x_min(0), x_max(1), y_min(0), y_max(1) // 0..1
{
}

CPreview::~CPreview()
{
}


BEGIN_MESSAGE_MAP(CPreview, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CPreview message handlers


void CPreview::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    CRect control_rect;
    GetClientRect(&control_rect);

    int w = control_rect.Width();
	int h = control_rect.Height();

    CDC bg_dc;
    bg_dc.CreateCompatibleDC(&dc);

    CBitmap bg_bmp;
    BITMAP bg_bmp_info;
    bg_bmp.LoadBitmap(IDB_PREVIEW);
    bg_bmp.GetBitmap(&bg_bmp_info);

    bg_dc.SelectObject(&bg_bmp);

	dc.StretchBlt(0, 0, w, h, &bg_dc, 0, 0, bg_bmp_info.bmWidth, bg_bmp_info.bmHeight, SRCCOPY);

    CRect draw_rect;
    draw_rect.left = max(0, (LONG) ( x_min*w ) - 1);
    draw_rect.right = min(w, (LONG) ( x_max*w ) + 1);
    draw_rect.top = max(0, (LONG) ( y_min*h ) - 1);
    draw_rect.bottom = min(h, (LONG) ( y_max*h ) + 1);

    CBrush brush(PREVIEW_FRAME_COLOR);
    dc.FrameRect(draw_rect, &brush);
}

void CPreview::Set(float _x_min, float _x_max, float _y_min, float _y_max)
{
    x_max = _x_max;
    x_min = _x_min;
    y_max = _y_max;
    y_min = _y_min;

    Invalidate(TRUE);
}
