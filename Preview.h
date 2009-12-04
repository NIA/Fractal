#pragma once

#include "messages.h"

// CPreview

class CPreview : public CStatic
{
private:
	DECLARE_DYNAMIC(CPreview)

    float x_min, x_max, y_min, y_max; // 0..1
    // Drag-n-drop
    bool drag;
    CPoint drag_start;
    float start_x_min, start_x_max, start_y_min, start_y_max; // 0..1
    inline void store_start_coords()
    {
        start_x_max = x_max;
        start_x_min = x_min;
        start_y_max = y_max;
        start_y_min = y_min;
    }
    inline void set_drag_start( CPoint _drag_start )
    {
        drag_start = _drag_start;
        store_start_coords();
    }

protected:
	DECLARE_MESSAGE_MAP()
public:
	CPreview();
	virtual ~CPreview();

    void Set(float _x_min, float _x_max, float _y_min, float _y_max);
    void Get(float &_x_min, float &_x_max, float &_y_min, float &_y_max);
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    void MoveTo(CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
};
