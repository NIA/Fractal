#pragma once


// CPreview

class CPreview : public CStatic
{
private:
	DECLARE_DYNAMIC(CPreview)

    float x_min, x_max, y_min, y_max; // 0..1
protected:
	DECLARE_MESSAGE_MAP()
public:
	CPreview();
	virtual ~CPreview();

    void Set(float _x_min, float _x_max, float _y_min, float _y_max);
    afx_msg void OnPaint();
};
