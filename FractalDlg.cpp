
// FractalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Fractal.h"
#include "FractalDlg.h"

#include <complex>
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFractalDlg dialog
#define MSG_PROGRESS (WM_USER + 1)
#define MSG_FINISHED (WM_USER + 2)


CFractalDlg::CFractalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFractalDlg::IDD, pParent)
	, m_BitmapHeight(100)
	, m_BitmapWidth(100)
	, m_Thread(NULL)
	, is_bitmap_made(false)
	, m_ItersPerPoint(500)
    , m_DrawStyle(0)
    , m_XMin(-2.5)
    , m_XMax(1.5)
    , m_YMin(-1.5)
    , m_YMax(1.5)
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
}

BEGIN_MESSAGE_MAP(CFractalDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_START, &CFractalDlg::OnBnClickedButtonStart)
	ON_MESSAGE(MSG_PROGRESS, &CFractalDlg::OnProgressChanged)
	ON_MESSAGE(MSG_FINISHED, &CFractalDlg::OnThreadFinish)
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
END_MESSAGE_MAP()


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
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFractalDlg::SetDefaultPictureSize()
{
	CRect rect;
	m_Canvas.GetWindowRect(&rect);
	m_BitmapWidth = rect.Width();
	m_BitmapHeight = rect.Height();
}

void CFractalDlg::MakeBitmap(CDC *dc, int w, int h)
{
	if(!is_bitmap_made)
	{
		m_Bitmap.CreateCompatibleBitmap(dc, w, h);
		is_bitmap_made = true;
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
		if( m_Thread != NULL)
		{
			m_Bitmap.SetBitmapBits(m_BitmapWidth*m_BitmapHeight*sizeof(RGBQUAD), m_Thread->pixels);
		}

		CRect cr; // canvas rect
		m_Canvas.GetWindowRect(&cr);
		ScreenToClient(&cr);
					
		dc.StretchBlt(cr.left, cr.top, cr.Width(), cr.Height(), &mem_dc, 0, 0, m_BitmapWidth, m_BitmapHeight, SRCCOPY);
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFractalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


CFractalDlg::Thread::Thread(HWND hwnd, UINT width, UINT height, UINT iters_per_point, int draw_style, float x_min, float x_max, float y_min, float y_max)
	: stopped(false), dlg_hwnd(hwnd), pixels(NULL),
      width(width), height(height), iters_per_point(iters_per_point),
      draw_style(draw_style),
      x_min(x_min), x_max(x_max), y_min(y_min), y_max(y_max)
{
	pixels = new RGBQUAD[width*height];
	memset(pixels, 0, width*height*sizeof(RGBQUAD));

	handle = CreateThread( NULL, 0, routine, this, 0, NULL );
	if( handle == NULL)
		throw "Thread not created";
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


DWORD __stdcall CFractalDlg::Thread::routine( void * param )
{
	Thread *thread = static_cast<Thread*>( param );
	UINT width = thread->get_width();
	UINT height = thread->get_height();
    float x_min, x_max, y_min, y_max;
    thread->get_bounds(x_min, x_max, y_min, y_max);
	OutputDebugString(_T("Thread Started\n"));


	for(unsigned j = 0; j < height; ++j)
	{
		for(unsigned i = 0; i < width; ++i)
		{
			if( thread->is_stopped() )
			{
				OutputDebugString(_T("\nThread Interrupted\n"));
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
	OutputDebugString(_T("\nThread Finished\n"));
	::PostMessage(thread->get_hwnd(), MSG_FINISHED, 0, 0);
	return 0;
}

CFractalDlg::Thread::~Thread()
{
	ASSERT( handle != NULL);
	CloseHandle( handle );
	ASSERT( pixels != NULL);
	delete[] pixels;
}

void CFractalDlg::EnableStartControls(BOOL enable)
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
}

void CFractalDlg::OnBnClickedButtonStart()
{
    if (m_Thread == NULL)
    {
	    m_Bitmap.DeleteObject();
	    is_bitmap_made = false;

	    if( UpdateData() )
        {
	        m_Progress.SetRange(0, m_BitmapHeight);
	        m_Progress.SetPos(0);
            EnableStartControls(FALSE);
	        m_Thread = new Thread(m_hWnd,
                                  m_BitmapWidth, m_BitmapHeight,
                                  m_ItersPerPoint, m_DrawStyle,
                                  m_XMin, m_XMax, m_YMin, m_YMax);
	        GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
        }
    }
}

afx_msg LRESULT CFractalDlg::OnThreadFinish(WPARAM,LPARAM)
{
	if( m_Thread != NULL)
	{
        GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	    m_Thread->wait();
	    delete m_Thread;
	    m_Thread = NULL;

        int lower, upper;
        m_Progress.GetRange(lower, upper);
        m_Progress.SetPos( upper );
        EnableStartControls(TRUE);
    }
	return S_OK;
}

void CFractalDlg::OnBnClickedButtonStop()
{
	if( m_Thread != NULL)
	{
	    m_Thread->stop();
	    OnThreadFinish(0,0);
    }
}

LRESULT CFractalDlg::OnProgressChanged(WPARAM wparam,LPARAM lparam)
{
    int lines_ready = static_cast<int>(lparam);
	m_Progress.SetPos( lines_ready );
	if( m_Thread != NULL)
	{
		CRect canvas_rect;
		m_Canvas.GetWindowRect(&canvas_rect);
		ScreenToClient(&canvas_rect);
        canvas_rect.bottom = canvas_rect.top + canvas_rect.Height()*lines_ready/m_BitmapHeight;
		InvalidateRect(canvas_rect, FALSE);
	}
	return S_OK;
}

void CFractalDlg::OnOK()
{
    OnBnClickedButtonStart();
}

const float MOVE_QUOTIENT = 10.0f; // it means that picture is moved/zoomed to 1/10 each step
                                   // > 2 !

typedef bool (*WIDTH_PREDICATE)(float);

bool too_narrow(float width) { return width < 1e-5f; }
bool too_wide(float width) { return width > 1e+38f; }

bool Move(float &lower, float &upper,
          int lower_sign, int upper_sign,
          WIDTH_PREDICATE is_width_bad = NULL,
          float quotient = MOVE_QUOTIENT)
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
    if( Move(m_YMin, m_YMax, -1, -1) )
    {
        UpdateData(FALSE);
        OnOK();
    }
}

void CFractalDlg::OnBnClickedButtonDown()
{
    if( Move(m_YMin, m_YMax, 1, 1) )
    {
        UpdateData(FALSE);
        OnOK();
    }
}

void CFractalDlg::OnBnClickedButtonLeft()
{
    if( Move(m_XMin, m_XMax, -1, -1) )
    {
        UpdateData(FALSE);
        OnOK();
    }
}

void CFractalDlg::OnBnClickedButtonRight()
{
    if( Move(m_XMin, m_XMax, 1, 1) )
    {
        UpdateData(FALSE);
        OnOK();
    }
}

void CFractalDlg::OnBnClickedButtonZoomIn()
{
    _ASSERT(MOVE_QUOTIENT > 2);
    if( Move(m_XMin, m_XMax, 1, -1, too_narrow) &&
        Move(m_YMin, m_YMax, 1, -1, too_narrow) )
    {
        UpdateData(FALSE);
        OnOK();
    }
}

void CFractalDlg::OnBnClickedButtonZoomOut()
{
    _ASSERT(MOVE_QUOTIENT > 2);
    if( Move(m_XMin, m_XMax, -1, 1, too_wide, MOVE_QUOTIENT-2) &&
        Move(m_YMin, m_YMax, -1, 1, too_wide, MOVE_QUOTIENT-2) )
    {
        UpdateData(FALSE);
        OnOK();
    }
}


void CFractalDlg::OnBnClickedButtonZoomDefault()
{
    m_XMin = -2.5;
    m_XMax = 1.5;
    m_YMin = -1.5;
    m_YMax = 1.5;
    UpdateData(FALSE);
    OnOK();
}

void CFractalDlg::OnBnClickedButtonZoomDemo()
{
    m_XMin = -1.78671f;
    m_XMax = -1.78541f;
    m_YMin = -0.00049f;
    m_YMax = +0.00049f;
    m_BitmapWidth = 200;
    m_BitmapHeight = 180;
    m_ItersPerPoint = 160;
    UpdateData(FALSE);
    OnOK();
}

void CFractalDlg::OnBnClickedButtonZoomDemo2()
{
    m_XMin = -1.09643f;
    m_XMax = -0.66694f;
    m_YMin =  0.04792f;
    m_YMax =  0.37004f;
	SetDefaultPictureSize();
    m_ItersPerPoint = 500;
    UpdateData(FALSE);
    OnOK();
}

void CFractalDlg::OnBnClickedButtonImage110()
{
	SetDefaultPictureSize();
    m_BitmapWidth /= 10;
    m_BitmapHeight /= 10;
    UpdateData(FALSE);
    OnOK();
}

void CFractalDlg::OnBnClickedButtonImageActual()
{
	SetDefaultPictureSize();
    UpdateData(FALSE);
    OnOK();
}

void CFractalDlg::OnBnClickedButtonDemo3()
{
    if (m_Thread != NULL)
    {
        return;
    }
    EnableStartControls(FALSE);

    m_XMin = -2.38582f;
    m_XMax =  0.845086f;
    m_YMin = -1.24116f;
    m_YMax =  1.4624f;
    m_BitmapWidth = 320;
    m_BitmapHeight = 240;
    m_ItersPerPoint = 200;

    for(unsigned i=0; i<53; ++i)
    {
        Move(m_XMin, m_XMax, 1, -1, too_narrow); // zoom in
        Move(m_YMin, m_YMax, 1, -1, too_narrow); // zoom in
        UpdateData(FALSE);

        m_Bitmap.DeleteObject();
        is_bitmap_made = false;
        m_Progress.SetRange(0, m_BitmapHeight);
        m_Progress.SetPos(0);

        m_Thread = new Thread(m_hWnd,
                              m_BitmapWidth, m_BitmapHeight,
                              m_ItersPerPoint, m_DrawStyle,
                              m_XMin, m_XMax, m_YMin, m_YMax);
        m_Thread->wait();
        Invalidate(FALSE);
        OnPaint();
	    delete m_Thread;
	    m_Thread = NULL;
        Sleep(20);
    }
    EnableStartControls(TRUE);
}
