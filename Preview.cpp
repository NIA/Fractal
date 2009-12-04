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
: x_min(0), x_max(1), y_min(0), y_max(1), // 0..1
  drag(false)
{
}

CPreview::~CPreview()
{
}


BEGIN_MESSAGE_MAP(CPreview, CStatic)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()



// CPreview message handlers

LONG convert_x_min(float x_min, int w, int h) { return (LONG) ( x_min*w ) - 1; }
LONG convert_x_max(float x_max, int w, int h) { return (LONG) ( x_max*w ) + 1; }
LONG convert_y_min(float y_min, int w, int h) { return (LONG) ( y_min*h ) - 1; }
LONG convert_y_max(float y_max, int w, int h) { return (LONG) ( y_max*h ) + 1; }

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
    draw_rect.left   = min(w, max( 0, convert_x_min(x_min, w, h) ) );
    draw_rect.right  = min(w, max( 0, convert_x_max(x_max, w, h) ) );
    draw_rect.top    = min(h, max( 0, convert_y_min(y_min, w, h) ) );
    draw_rect.bottom = min(h, max( 0, convert_y_max(y_max, w, h) ) );

    CBrush brush(PREVIEW_FRAME_COLOR);
    if( (x_min < 1) && (x_max > 0) && (y_min < 1) && (y_max > 0) )
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
void CPreview::Get(float &_x_min, float &_x_max, float &_y_min, float &_y_max)
{
    _x_max = x_max;
    _x_min = x_min;
    _y_max = y_max;
    _y_min = y_min;
}

void CPreview::OnLButtonDown(UINT nFlags, CPoint point)
{
    CRect control_rect;
    GetClientRect(&control_rect);

    int w = control_rect.Width();
	int h = control_rect.Height();

    drag = true;
    SetCapture();
    set_drag_start(
                    CPoint( convert_x_min( (x_min + x_max)/2, w, h ),
                            convert_y_min( (y_min + y_max)/2, w, h ) )
                   ); // a center of the rect
    MoveTo(point);
    set_drag_start( point );
    CStatic::OnLButtonDown(nFlags, point);
}

void CPreview::OnLButtonUp(UINT nFlags, CPoint point)
{
    drag = false;
    ReleaseCapture();
    CStatic::OnLButtonUp(nFlags, point);
}

void CPreview::MoveTo(CPoint point)
{
        CRect control_rect;
        GetClientRect(&control_rect);

        CPoint diff = point - drag_start;
        float inc_x = (float)diff.x/(float)control_rect.Width();
        float inc_y = (float)diff.y/(float)control_rect.Height();
        
        x_min = start_x_min + inc_x;
        x_max = start_x_max + inc_x;
        y_min = start_y_min + inc_y;
        y_max = start_y_max + inc_y;

        ::PostMessage(GetParent()->m_hWnd, MSG_PREVIEW_MOVED, 0, 0);
        Invalidate();
}

void CPreview::OnMouseMove(UINT nFlags, CPoint point)
{
    if( drag )
    {
        MoveTo(point);
    }
    CStatic::OnMouseMove(nFlags, point);
}

void CPreview::OnMouseLeave()
{
    drag = false;
    CStatic::OnMouseLeave();
}
