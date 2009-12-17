#include "stdafx.h"
#include "Fractal.h"
#include "FractalDlg.h"

#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const float DEFAULT_X_MIN = -2.5f;
const float DEFAULT_X_MAX = 1.5f;
const float DEFAULT_Y_MIN = -1.5f;
const float DEFAULT_Y_MAX = 1.5f;

const float DEFAULT_WIDTH = DEFAULT_X_MAX - DEFAULT_X_MIN;
const float DEFAULT_HEIGHT = DEFAULT_Y_MAX - DEFAULT_Y_MIN;

const float DEFAULT_MOVE_QUOTIENT = 10.0f; // it means that picture is moved/zoomed to 1/10 each step
                                           // > 2 !
const float DEFAULT_ZOOM_FACTOR = 1/(1 - 2/DEFAULT_MOVE_QUOTIENT);

const UINT MAX_ZOOM_EXPONENT = 6;
const UINT UNITS_PER_ZOOM_EXPONENT = 10;

const UINT MAX_TOOLTIP_LENGTH = 80;

const unsigned CONTROLS_WITH_TOOLTIPS[] = {
    IDCANCEL,
    IDC_INPUT_WIDTH,
    IDC_INPUT_HEIGHT,
    IDC_EDIT_ITERS,
    IDC_GRAYSCALE,
    IDC_FORMULA,
    IDC_MODULO,
    IDC_BUTTON_START,
    IDC_BUTTON_STOP,
    IDC_INPUT_XMIN,
    IDC_INPUT_XMAX,
    IDC_INPUT_YMIN,
    IDC_INPUT_YMAX,
    IDC_BUTTON_UP,
    IDC_BUTTON_DOWN,
    IDC_BUTTON_RIGHT,
    IDC_BUTTON_LEFT,
    IDC_BUTTON_ZOOM_IN,
    IDC_BUTTON_ZOOM_OUT,
    IDC_BUTTON_ZOOM_DEFAULT,
    IDC_BUTTON_ZOOM_DEMO,
    IDC_BUTTON_ZOOM_DEMO2,
    IDC_BUTTON_IMAGE_1_10,
    IDC_BUTTON_IMAGE_ACTUAL,
    IDC_BUTTON_DEMO3,
    IDC_ANIMATION_REPEATS,
    IDC_BUTTON_ANIMATION_GO,
    IDC_PREVIEW,
    IDC_INPUT_ZOOM,
    IDC_SLIDER_ZOOM,
};

BEGIN_MESSAGE_MAP(CFractalDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CFractalDlg::OnBnClickedButtonStart)
	ON_MESSAGE(MSG_PROGRESS, &CFractalDlg::OnProgressChanged)
	ON_MESSAGE(MSG_FINISHED, &CFractalDlg::OnThreadFinish)
	ON_MESSAGE(MSG_ANIMATION_DO_ZOOM, &CFractalDlg::OnDoZoom)
	ON_MESSAGE(MSG_ANIMATION_DO_READ, &CFractalDlg::OnDoRead)
	ON_MESSAGE(MSG_ANIMATION_FINISHED, &CFractalDlg::OnAnimationFinish)
	ON_MESSAGE(MSG_ANIMATION_DO_UPDATE_DATA, &CFractalDlg::OnDoUpdateData)
	ON_MESSAGE(MSG_PREVIEW_MOVED, &CFractalDlg::OnPreviewMoved)
	ON_MESSAGE(MSG_START_1_10, &CFractalDlg::OnStart_1_10)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CFractalDlg::OnBnClickedButtonStop)
    ON_BN_CLICKED(IDC_BUTTON_UP, &CFractalDlg::OnBnClickedButtonUp)
    ON_BN_CLICKED(IDC_BUTTON_DOWN, &CFractalDlg::OnBnClickedButtonDown)
    ON_BN_CLICKED(IDC_BUTTON_ZOOM_IN, &CFractalDlg::OnBnClickedButtonZoomIn)
    ON_BN_CLICKED(IDC_BUTTON_ZOOM_OUT, &CFractalDlg::OnBnClickedButtonZoomOut)
    ON_BN_CLICKED(IDC_BUTTON_LEFT, &CFractalDlg::OnBnClickedButtonLeft)
    ON_BN_CLICKED(IDC_BUTTON_RIGHT, &CFractalDlg::OnBnClickedButtonRight)
    ON_BN_CLICKED(IDC_BUTTON_ZOOM_DEFAULT, &CFractalDlg::OnBnClickedButtonZoomDefault)
    ON_BN_CLICKED(IDC_BUTTON_ZOOM_DEMO, &CFractalDlg::OnBnClickedButtonZoomDemo)
    ON_BN_CLICKED(IDC_BUTTON_ZOOM_DEMO2, &CFractalDlg::OnBnClickedButtonZoomDemo2)
    ON_BN_CLICKED(IDC_BUTTON_IMAGE_1_10, &CFractalDlg::OnBnClickedButtonImage110)
    ON_BN_CLICKED(IDC_BUTTON_IMAGE_ACTUAL, &CFractalDlg::OnBnClickedButtonImageActual)
    ON_BN_CLICKED(IDC_BUTTON_DEMO3, &CFractalDlg::OnBnClickedButtonDemo3)
    ON_BN_CLICKED(IDC_BUTTON_ANIMATION_GO, &CFractalDlg::OnBnClickedButtonAnimationGo)
	ON_WM_CLOSE()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ANIMATION_REPEATS, &CFractalDlg::OnDeltaposSpinAnimationRepeats)
    ON_WM_HSCROLL()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

CFractalDlg::CFractalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFractalDlg::IDD, pParent)
	, m_BitmapHeight(100)
	, m_BitmapWidth(100)
	, m_Thread(NULL)
	, m_AnimationOn(false)
	, is_bitmap_made(false)
	, m_ItersPerPoint(500)
    , m_DrawStyle(0)
    , m_XMin(DEFAULT_X_MIN)
    , m_XMax(DEFAULT_X_MAX)
    , m_YMin(DEFAULT_Y_MIN)
    , m_YMax(DEFAULT_Y_MAX)
    , m_AnimationRepeats(20)
    , m_Zoom(100)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void PASCAL DDV_LessFloat(CDataExchange* dx, LPCTSTR minor_field, float minor_value,
                                             LPCTSTR major_field, float major_value)
{
    if( minor_value >= major_value )
    {
        // Code based on standard DDV_* functions
	    if (!dx->m_bSaveAndValidate)
	    {
		    TRACE(traceAppMsg, 0, "Warning: initial dialog data is out of range.\n");
		    return;     // don't stop now
	    }
	    CString prompt;
	    AfxFormatString2(prompt, IDS_PARSE_LESS, minor_field, major_field);
	    AfxMessageBox(prompt, MB_ICONEXCLAMATION);
	    prompt.Empty(); // exception prep
	    dx->Fail();
    }
}

void CFractalDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_INPUT_WIDTH, m_BitmapWidth);
    DDV_MinMaxUInt(pDX, m_BitmapWidth, 10, 5000);
    DDX_Text(pDX, IDC_INPUT_HEIGHT, m_BitmapHeight);
    DDV_MinMaxUInt(pDX, m_BitmapHeight, 10, 5000);
    DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
    DDX_Control(pDX, IDC_BUTTON_START, m_ButtonStart);
    DDX_Control(pDX, IDC_BUTTON_STOP, m_ButtonStop);
    DDX_Control(pDX, IDC_CANVAS, m_Canvas);
    DDX_Text(pDX, IDC_EDIT_ITERS, m_ItersPerPoint);
    DDV_MinMaxUInt(pDX, m_ItersPerPoint, 2, 10000000);
    DDX_Radio(pDX, IDC_GRAYSCALE, m_DrawStyle);
    DDX_Text(pDX, IDC_INPUT_XMIN, m_XMin);
    DDX_Text(pDX, IDC_INPUT_XMAX, m_XMax);
    DDV_LessFloat(pDX, _T("X min"), m_XMin, _T("X max"), m_XMax);
    DDX_Text(pDX, IDC_INPUT_YMIN, m_YMin);
    DDX_Text(pDX, IDC_INPUT_YMAX, m_YMax);
    DDV_LessFloat(pDX, _T("Y min"), m_YMin, _T("Y max"), m_YMax);
    DDX_Text(pDX, IDC_ANIMATION_REPEATS, m_AnimationRepeats);
    DDV_MinMaxUInt(pDX, m_AnimationRepeats, 0, 100);
    DDX_Control(pDX, IDC_PREVIEW, m_Preview);
    DDX_Text(pDX, IDC_INPUT_ZOOM, m_Zoom);
    DDX_Control(pDX, IDC_SLIDER_ZOOM, m_SliderZoom);
    DDX_Control(pDX, IDC_INPUT_ZOOM, m_EditZoom);
}

// CFractalDlg message handlers

BOOL CFractalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetDefaultPictureSize();
	UpdateData(FALSE);

    m_SliderZoom.SetRange(1*UNITS_PER_ZOOM_EXPONENT, MAX_ZOOM_EXPONENT*UNITS_PER_ZOOM_EXPONENT, TRUE); // 10^1 ... 10^7
    m_SliderZoom.SetPos(2*UNITS_PER_ZOOM_EXPONENT);  // 10^2 = 100%
    
    EnableToolTips(TRUE);
    m_ToolTip.Create(this);
    for(unsigned i = 0; i < array_size(CONTROLS_WITH_TOOLTIPS); ++i)
    {
        unsigned id = CONTROLS_WITH_TOOLTIPS[i];
        CString tooltip_text;
        if(!tooltip_text.LoadString(id))   
            continue;
        m_ToolTip.AddTool(GetDlgItem(id), tooltip_text);
    }

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFractalDlg::SetDefaultPictureSize()
{
	CRect rect;
	m_Canvas.GetWindowRect(&rect);
	m_BitmapWidth = rect.Width();
	m_BitmapHeight = rect.Height();
    m_Zoom = 100;
}

void CFractalDlg::MakeBitmap(CDC *dc, int w, int h)
{
	if(!is_bitmap_made)
	{
		m_Bitmap.CreateCompatibleBitmap(dc, w, h);
		is_bitmap_made = true;
	}
}

void CFractalDlg::UpdatePreview()
{
    m_Preview.Set( (m_XMin - DEFAULT_X_MIN)/DEFAULT_WIDTH,
                   (m_XMax - DEFAULT_X_MIN)/DEFAULT_WIDTH,
                   (m_YMin - DEFAULT_Y_MIN)/DEFAULT_HEIGHT,
                   (m_YMax - DEFAULT_Y_MIN)/DEFAULT_HEIGHT );
}

LRESULT CFractalDlg::OnPreviewMoved(WPARAM,LPARAM)
{
    float x_min, x_max, y_min, y_max; // 0..1
    m_Preview.Get(x_min, x_max, y_min, y_max);
    m_XMin = DEFAULT_X_MIN + x_min*DEFAULT_WIDTH;
    m_XMax = DEFAULT_X_MIN + x_max*DEFAULT_WIDTH;
    m_YMin = DEFAULT_Y_MIN + y_min*DEFAULT_HEIGHT;
    m_YMax = DEFAULT_Y_MIN + y_max*DEFAULT_HEIGHT;
    UpdatePreview();
    UpdateZoomValue();
    UpdateZoomSliderValue();
    return 0;
}
LRESULT CFractalDlg::OnStart_1_10(WPARAM,LPARAM)
{
    OnBnClickedButtonImage110();
    return 0;
}

void CFractalDlg::ReadData()
{
	if( m_Thread != NULL)
	{
        ICanvasThread* canvas = dynamic_cast<ICanvasThread*>(m_Thread);
		m_Bitmap.SetBitmapBits(m_BitmapWidth*m_BitmapHeight*sizeof(RGBQUAD), canvas->get_pixels());
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFractalDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);
	if (IsIconic())
	{

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDC mem_dc;
		mem_dc.CreateCompatibleDC(&dc);
		MakeBitmap(&dc, m_BitmapWidth, m_BitmapHeight);
		mem_dc.SelectObject(&m_Bitmap);

        ReadData();
        m_Read.SetEvent();


		CRect cr; // canvas rect
		m_Canvas.GetWindowRect(&cr);
		ScreenToClient(&cr);
					
		//CDialog::OnPaint();
		dc.StretchBlt(cr.left, cr.top, cr.Width(), cr.Height(), &mem_dc, 0, 0, m_BitmapWidth, m_BitmapHeight, SRCCOPY);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFractalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFractalDlg::EnableStartControls(BOOL enable /*=TRUE*/)
{
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_UP)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_LEFT)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_RIGHT)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_ZOOM_IN)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_ZOOM_OUT)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_ZOOM_DEFAULT)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_ZOOM_DEMO)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_ZOOM_DEMO2)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_DEMO3)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_IMAGE_1_10)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_IMAGE_ACTUAL)->EnableWindow(enable);
    GetDlgItem(IDC_BUTTON_ANIMATION_GO)->EnableWindow(enable);
}

void CFractalDlg::EnableCriticalForAnimationControls(BOOL enable /*=TRUE*/)
{
	GetDlgItem(IDC_INPUT_WIDTH)->EnableWindow(enable);
	GetDlgItem(IDC_INPUT_HEIGHT)->EnableWindow(enable);
}

CCalculationThread * CFractalDlg::NewCalculationThread()
{
    return new CCalculationThread( m_hWnd,
                                   m_BitmapWidth, m_BitmapHeight,
                                   m_ItersPerPoint, m_DrawStyle,
                                   m_XMin, m_XMax, m_YMin, m_YMax );
}

void CFractalDlg::OnBnClickedButtonStart()
{
    if (m_Thread == NULL)
    {
	    m_Bitmap.DeleteObject();
	    is_bitmap_made = false;

	    if( UpdateData() )
        {
            UpdatePreview();
            UpdateZoomValue();
            UpdateZoomSliderValue();
            m_Progress.SetRange(0, m_BitmapHeight);
	        m_Progress.SetPos(0);
            EnableStartControls(FALSE);
	        m_Thread = NewCalculationThread();
            m_Thread->start();
	        GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
        }
    }
}

afx_msg LRESULT CFractalDlg::OnAnimationFinish(WPARAM,LPARAM)
{
    return OnThreadFinish(TRUE,0);
}

afx_msg LRESULT CFractalDlg::OnThreadFinish(WPARAM force,LPARAM)
{
	if( m_Thread != NULL )
	{
        if( !m_AnimationOn || force )
        {
            GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	        m_Thread->wait();
	        delete m_Thread;
	        m_Thread = NULL;
            m_AnimationOn = false;

            int lower, upper;
            m_Progress.GetRange(lower, upper);
            m_Progress.SetPos( upper );
            EnableStartControls(TRUE);
			EnableCriticalForAnimationControls(TRUE);
        }
    }
	return S_OK;
}

void CFractalDlg::OnBnClickedButtonStop()
{
	if( m_Thread != NULL)
	{
	    m_Thread->stop();
	    OnThreadFinish(TRUE,0);
    }
}

void CFractalDlg::InvalidateCanvas(int lines_ready)
{
	CRect canvas_rect;
	m_Canvas.GetWindowRect(&canvas_rect);
	ScreenToClient(&canvas_rect);
    if( lines_ready >= 0 )
        canvas_rect.bottom = canvas_rect.top + canvas_rect.Height()*lines_ready/m_BitmapHeight;
	InvalidateRect(canvas_rect, FALSE);
}

LRESULT CFractalDlg::OnProgressChanged(WPARAM wparam,LPARAM lparam)
{
    int lines_ready = static_cast<int>(lparam);
	m_Progress.SetPos( lines_ready );
	if( m_Thread != NULL)
	{
        if( !m_AnimationOn  )
            InvalidateCanvas(lines_ready);
	}
	return S_OK;
}

void CFractalDlg::OnOK()
{
    OnBnClickedButtonStart();
}

typedef bool (*WIDTH_PREDICATE)(float);

bool too_narrow(float width) { return width < 1e-5f; }
bool too_wide(float width) { return width > 1e+38f; }

bool Move(float &lower, float &upper,
          int lower_sign, int upper_sign,
          WIDTH_PREDICATE is_width_bad = NULL,
          float quotient = DEFAULT_MOVE_QUOTIENT)
{
    float width = (upper - lower);
    if( is_width_bad != NULL && is_width_bad( width ) )
        return false;

    float delta = width/quotient;
    upper += delta*((float)upper_sign);
    lower += delta*((float)lower_sign);
    return true;
}

void CFractalDlg::OnBnClickedButtonUp()
{
    if( UpdateData() && Move(m_YMin, m_YMax, -1, -1) )
    {
        UpdateData(FALSE);
        UpdatePreview();
        OnOK();
    }
}

void CFractalDlg::OnBnClickedButtonDown()
{
    if( UpdateData() && Move(m_YMin, m_YMax, 1, 1) )
    {
        UpdateData(FALSE);
        UpdatePreview();
        OnOK();
    }
}

void CFractalDlg::OnBnClickedButtonLeft()
{
    if( UpdateData() && Move(m_XMin, m_XMax, -1, -1) )
    {
        UpdateData(FALSE);
        UpdatePreview();
        OnOK();
    }
}

void CFractalDlg::OnBnClickedButtonRight()
{
    if( UpdateData() && Move(m_XMin, m_XMax, 1, 1) )
    {
        UpdateData(FALSE);
        UpdatePreview();
        OnOK();
    }
}

void CFractalDlg::Zoom(float final_zoom, bool need_to_update_data /*=false*/)
{
    bool res = false;
    bool zoom_in;
    float move_quotient;

    if( final_zoom == m_Zoom )
        return;

    if( final_zoom > m_Zoom )
    {
        zoom_in = true;
        move_quotient = 2/(1 - m_Zoom/final_zoom);
    }
    else
    {
        zoom_in = false;
        move_quotient = 2/(1 - final_zoom/m_Zoom);
    }

    _ASSERT(move_quotient > 2);
    if( !need_to_update_data || UpdateData() )
    {
        if(zoom_in)
        {
            res = ( Move(m_XMin, m_XMax, 1, -1, too_narrow, move_quotient) &&
                    Move(m_YMin, m_YMax, 1, -1, too_narrow, move_quotient) );
        }
        else
        {
            res = ( Move(m_XMin, m_XMax, -1, 1, too_wide, move_quotient-2) &&
                    Move(m_YMin, m_YMax, -1, 1, too_wide, move_quotient-2) );
        }

        if(res)
        {
            m_Zoom = final_zoom;
            UpdateData(FALSE);
        }
    }
}
void CFractalDlg::DoZoom(bool zoom_in /*=true*/, bool need_to_update_data /*=false*/)
{
    float final_zoom = zoom_in ? m_Zoom*DEFAULT_ZOOM_FACTOR : m_Zoom/DEFAULT_ZOOM_FACTOR;
    Zoom(final_zoom, need_to_update_data);
}

void CFractalDlg::UpdateZoomValue()
{
    _ASSERT( m_XMax - m_XMin != 0);
    m_Zoom = (DEFAULT_Y_MAX - DEFAULT_X_MIN)/(m_XMax - m_XMin)*100;
    UpdateData(FALSE);
}

void CFractalDlg::UpdateZoomSliderValue()
{
    float exponent = log10(m_Zoom);
    m_SliderZoom.SetPos( (int)(exponent*UNITS_PER_ZOOM_EXPONENT) );
}

LRESULT CFractalDlg::OnDoZoom(WPARAM,LPARAM)
{
    UpdatePreview();
    UpdateZoomValue();
    UpdateZoomSliderValue();
    DoZoom();
    m_Zoomed.SetEvent();
    return 0;
}

LRESULT CFractalDlg::OnDoRead(WPARAM,LPARAM)
{
    InvalidateCanvas();
    //m_Read.SetEvent();
    return 0;
}

void CFractalDlg::OnBnClickedButtonZoomIn()
{
    DoZoom(true, true);
    UpdatePreview();
    OnOK();
}

void CFractalDlg::OnBnClickedButtonZoomOut()
{
    DoZoom(false, true);
    UpdatePreview();
    OnOK();
}

void CFractalDlg::PostZoomAndWait()
{
    m_Zoomed.ResetEvent();
    ::PostMessage(m_hWnd, MSG_ANIMATION_DO_ZOOM, 0, 0);
    ::WaitForSingleObject(m_Zoomed.m_hObject, INFINITE);
}

void CFractalDlg::PostReadAndWait()
{
    m_Read.ResetEvent();
    ::PostMessage(m_hWnd, MSG_ANIMATION_DO_READ, 0, 0);
    ::WaitForSingleObject(m_Read.m_hObject, INFINITE);
}

void CFractalDlg::PostUpdateDataAndWait()
{
    m_UpdatedData.ResetEvent();
    ::PostMessage(m_hWnd, MSG_ANIMATION_DO_UPDATE_DATA, 0, 0);
    ::WaitForSingleObject(m_UpdatedData.m_hObject, INFINITE);
}

LRESULT CFractalDlg::OnDoUpdateData(WPARAM,LPARAM)
{
	UpdateData(); // TODO: Confirm that bitmap width and height are not changed

	m_UpdatedData.SetEvent();
	return 0;
}

void CFractalDlg::OnBnClickedButtonZoomDefault()
{
    if( UpdateData() )
    {
        m_XMin = DEFAULT_X_MIN;
        m_XMax = DEFAULT_X_MAX;
        m_YMin = DEFAULT_Y_MIN;
        m_YMax = DEFAULT_Y_MAX;
        UpdateData(FALSE);
        UpdatePreview();
        OnOK();
    }
}

void CFractalDlg::OnBnClickedButtonZoomDemo()
{
    if( UpdateData() )
    {
        m_XMin = -4.25283f;
        m_XMax =  0.68069f;
        m_YMin = -1.88741f;
        m_YMax =  1.88741f;
        SetDefaultPictureSize();
        m_ItersPerPoint = 300;
        m_AnimationRepeats = 36;
        UpdateData(FALSE);

        OnBnClickedButtonAnimationGo();
    }
}

void CFractalDlg::OnBnClickedButtonZoomDemo2()
{
    if( UpdateData() )
    {
        m_XMin = -1.93677f;
        m_XMax = 1.73458f;
        m_YMin = -2.51409f;
        m_YMax =  0.6015348f;
        m_BitmapWidth = 320;
        m_BitmapHeight = 240;
        m_ItersPerPoint = 150;
        m_AnimationRepeats = 50;
        UpdateData(FALSE);
        
        OnBnClickedButtonAnimationGo();
    }
}

void CFractalDlg::OnBnClickedButtonImage110()
{
    if( UpdateData() )
    {
	    SetDefaultPictureSize();
        m_BitmapWidth /= 10;
        m_BitmapHeight /= 10;
        UpdateData(FALSE);
        OnOK();
    }
}

void CFractalDlg::OnBnClickedButtonImageActual()
{
    if( UpdateData() )
    {
	    SetDefaultPictureSize();
        UpdateData(FALSE);
        OnOK();
    }
}

void CFractalDlg::OnBnClickedButtonDemo3()
{
    if( UpdateData() )
    {
        m_XMin = -2.38582f;
        m_XMax =  0.845086f;
        m_YMin = -1.24116f;
        m_YMax =  1.4624f;
        m_BitmapWidth = 480;
        m_BitmapHeight = 380;
        m_ItersPerPoint = 500;
        m_AnimationRepeats = 50;
        UpdateData(FALSE);
        
        OnBnClickedButtonAnimationGo();
    }
}

void CFractalDlg::OnBnClickedButtonAnimationGo()
{
    if (m_Thread != NULL)
    {
        return;
    }

    m_Bitmap.DeleteObject();
    is_bitmap_made = false;

    if( UpdateData() && m_AnimationRepeats > 0 )
    {
        EnableStartControls(FALSE);
		EnableCriticalForAnimationControls(FALSE);
        m_AnimationOn = true;
        
        m_Progress.SetRange(0, m_BitmapHeight);
        m_Progress.SetPos(0);

        m_Thread = new CAnimationThread(this, m_AnimationRepeats);
        m_Thread->start();
        GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
    }
}

void CFractalDlg::OnDeltaposSpinAnimationRepeats(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    if(UpdateData())
    {
        if( m_AnimationRepeats > 0 || pNMUpDown->iDelta < 0 )
        {
            m_AnimationRepeats -= pNMUpDown->iDelta;
            UpdateData(FALSE);
        }
	    *pResult = 0;
    }
    else
    {
        *pResult = E_FAIL;
    }
}

void CFractalDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
    float zoom = pow(10.0f, ((float)m_SliderZoom.GetPos())/UNITS_PER_ZOOM_EXPONENT);
    Zoom(zoom, true);
    if ( nSBCode == TB_THUMBPOSITION || nSBCode == TB_PAGEUP || nSBCode == TB_PAGEDOWN )
        ::PostMessage(m_hWnd, MSG_START_1_10, 0, 0);
    UpdatePreview();
}

void CFractalDlg::OnDestroy()
{
	if( is_bitmap_made )
        m_Bitmap.DeleteObject();

	if(m_Thread != NULL)
	{
		m_Thread->stop();
		m_Thread->wait();
		delete m_Thread;
		m_Thread = NULL;
	}
    CDialog::OnDestroy();
}

BOOL CFractalDlg::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST)
        m_ToolTip.RelayEvent(pMsg);
    
    return CDialog::PreTranslateMessage(pMsg);
}
