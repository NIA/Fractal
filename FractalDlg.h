#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "messages.h"
#include "Thread.h"
#include "AnimationThread.h"
#include "preview.h"

// a helper to find out a size of an array defined with `array[]={...}' without doing `sizeof(array)/sizeof(array[0])'
template<size_t SIZE, class T> inline size_t array_size(T (&array)[SIZE])
{
    UNREFERENCED_PARAMETER(array);
    return SIZE;
}

// CFractalDlg dialog
class CFractalDlg : public CDialog
{
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	HICON m_hIcon;
	void MakeBitmap(CDC *dc, int w, int h);
    void ReadData();
    void Zoom(float final_zoom, bool need_to_update_data = false);
    void DoZoom(bool zoom_in = true, bool need_to_update_data = false);
    void UpdateZoomValue();
    void UpdateZoomSliderValue();


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
    virtual void OnOK();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonStart();
	afx_msg LRESULT OnProgressChanged(WPARAM,LPARAM);
	afx_msg LRESULT OnThreadFinish(WPARAM,LPARAM);
	afx_msg LRESULT OnDoZoom(WPARAM,LPARAM);
	afx_msg LRESULT OnDoRead(WPARAM,LPARAM);
	afx_msg LRESULT OnAnimationFinish(WPARAM,LPARAM);
	afx_msg LRESULT OnDoUpdateData(WPARAM,LPARAM);
	afx_msg LRESULT OnPreviewMoved(WPARAM,LPARAM);
	afx_msg LRESULT OnStart_1_10(WPARAM = 0,LPARAM = 0);
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
    afx_msg void OnBnClickedButtonAnimationGo();
	afx_msg void OnDeltaposSpinAnimationRepeats(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
private:
    void EnableStartControls(BOOL enable = TRUE);
	void EnableCriticalForAnimationControls(BOOL enable = TRUE);
    void SetDefaultPictureSize();

	CProgressCtrl m_Progress;
    CSliderCtrl m_SliderZoom;

	bool is_bitmap_made;
	UINT m_BitmapWidth;
	UINT m_BitmapHeight;
	CBitmap m_Bitmap;

	CButton m_ButtonStart;
	CButton m_ButtonStop;
	CStatic m_Canvas;
	CThread * m_Thread;

    CEvent m_Zoomed;
    CEvent m_UpdatedData;
    CEvent m_Read;

	UINT m_ItersPerPoint;
    int m_DrawStyle;

    float m_XMin;
    float m_XMax;
    float m_YMin;
    float m_YMax;

	bool m_AnimationOn;
    UINT m_AnimationRepeats;
    CPreview m_Preview;
    CEdit m_EditZoom;
    float m_Zoom;
    BOOL m_ZoomIn;

    CToolTipCtrl m_ToolTip;
public:
// Construction
	CFractalDlg(CWnd* pParent = NULL);	// standard constructor
// Accessed by animation thread
    CCalculationThread * NewCalculationThread(); // creates new calculation thread with compatible params
    void UpdatePreview();

	enum { IDD = IDD_FRACTAL_DIALOG };
    void PostZoomAndWait(bool zoom_in);
    void PostUpdateDataAndWait();
    void PostReadAndWait();
    void InvalidateCanvas(int lines_ready = -1);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};
