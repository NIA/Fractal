#pragma once

class CThread
{
private:
	bool stopped;
	HANDLE handle;
public:
    CThread() : stopped(true), handle(NULL) {}
    virtual LPTHREAD_START_ROUTINE get_routine() = 0;
    void start();
    virtual void on_stop() {}
	void stop() { stopped = true; on_stop(); }
	void wait() { WaitForSingleObject(handle, INFINITE); } 
	bool is_stopped() { return stopped; }
	virtual ~CThread();
};

class ICanvasThread // interface "having pixels"
{
public:
    virtual RGBQUAD *get_pixels() = 0;
};

class CCalculationThread : public CThread, public ICanvasThread
{
private:
	HWND dlg_hwnd;
	UINT width, height;
	UINT iters_per_point;
    int draw_style;
    float x_min;
    float x_max;
    float y_min;
    float y_max;
	RGBQUAD *pixels;
public:
	CCalculationThread(HWND hwnd, UINT width, UINT height, UINT iters_per_point, int draw_style, float x_min, float x_max, float y_min, float y_max);
	static DWORD WINAPI routine( void * param);
    virtual LPTHREAD_START_ROUTINE get_routine(){ return routine; };
	HWND get_hwnd() { return dlg_hwnd; }
	UINT get_width() { return width; }
	UINT get_height() { return height; }
	int get_style() { return draw_style; }
	virtual RGBQUAD *get_pixels() { return pixels; }
    void get_bounds(float &x_min, float &x_max, float &y_min, float &y_max)
    {
        x_min = this->x_min;
        x_max = this->x_max;
        y_min = this->y_min;
        y_max = this->y_max;
    }
	virtual ~CCalculationThread();
};
