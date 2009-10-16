
// FractalDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CFractalDlg dialog
class CFractalDlg : public CDialog
{
	class Thread
	{
	private:
		bool stopped;
		HANDLE handle;
		HWND dlg_hwnd;
		UINT width, height;
		UINT iters_per_point;
        int draw_style;
        float x_min;
        float x_max;
        float y_min;
        float y_max;
	public:
		RGBQUAD *pixels;
		Thread(HWND hwnd, UINT width, UINT height, UINT iters_per_point, int draw_style, float x_min, float x_max, float y_min, float y_max);
		static DWORD WINAPI routine( void * param);
		void stop() { stopped = true; }
		void wait() { WaitForSingleObject(handle, INFINITE); } 
		HWND get_hwnd() { return dlg_hwnd; }
		UINT get_width() { return width; }
		UINT get_height() { return height; }
		int get_style() { return draw_style; }
        void get_bounds(float &x_min, float &x_max, float &y_min, float &y_max)
        {
            x_min = this->x_min;
            x_max = this->x_max;
            y_min = this->y_min;
            y_max = this->y_max;
        }
		bool is_stopped() { return stopped; }
		~Thread();
	};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	HICON m_hIcon;
	void MakeBitmap(CDC *dc, int w, int h);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
    virtual void OnOK();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
    void EnableStartControls(BOOL enable = TRUE);
    void SetDefaultPictureSize();

	CProgressCtrl m_Progress;

	bool is_bitmap_made;
	UINT m_BitmapWidth;
	UINT m_BitmapHeight;
	CBitmap m_Bitmap;

	CButton m_ButtonStart;
	CButton m_ButtonStop;
	CStatic m_Canvas;
	Thread * m_Thread;

	UINT m_ItersPerPoint;
    int m_DrawStyle;

    float m_XMin;
    float m_XMax;
    float m_YMin;
    float m_YMax;
public:
// Construction
	CFractalDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FRACTAL_DIALOG };
// Handlers
	afx_msg void OnBnClickedButtonStart();
	afx_msg LRESULT OnProgressChanged(WPARAM,LPARAM);
	afx_msg LRESULT OnThreadFinish(WPARAM,LPARAM);
	afx_msg void OnBnClickedButtonStop();
    afx_msg void OnBnClickedButtonUp();
    afx_msg void OnBnClickedButtonDown();
    afx_msg void OnBnClickedButtonZoomIn();
    afx_msg void OnBnClickedButtonZoomOut();
    afx_msg void OnBnClickedButtonLeft();
    afx_msg void OnBnClickedButtonRight();
    afx_msg void OnBnClickedButtonZoomDefault();
    afx_msg void OnBnClickedButtonZoomDemo();
    afx_msg void OnBnClickedButtonZoomDemo2();
    afx_msg void OnBnClickedButtonImage110();
    afx_msg void OnBnClickedButtonImageActual();
    afx_msg void OnBnClickedButtonDemo3();
};
