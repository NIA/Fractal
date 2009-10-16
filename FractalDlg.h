#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "messages.h"
#include "Thread.h"

// CFractalDlg dialog
class CFractalDlg : public CDialog
{
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
	CCalculationThread * m_Thread;

	UINT m_ItersPerPoint;
    int m_DrawStyle;

    float m_XMin;
    float m_XMax;
    float m_YMin;
    float m_YMax;
public:
// Construction
	CFractalDlg(CWnd* pParent = NULL);	// standard constructor
    CCalculationThread * NewCalculationThread(); // creates new calculation thread with compatible params

// Dialog Data
	enum { IDD = IDD_FRACTAL_DIALOG };
// Handlers
    void Zoom(bool zoom_in = true);
    
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
