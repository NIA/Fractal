#pragma once
#include "Thread.h"

class CFractalDlg;

class CAnimationThread : public CThread, public ICanvasThread
{
private:
    CCalculationThread * current_thread;
    CFractalDlg * dialog;
    unsigned zoom_count;
    bool zoom_in;
public:
    CAnimationThread(CFractalDlg * dialog, unsigned zoom_count, bool zoom_in);
    unsigned get_zoom_count() { return zoom_count; }
	virtual RGBQUAD *get_pixels();
    void calculate_frame(bool zoom);
    void post_message(UINT msg);
	static DWORD WINAPI routine( void * param);
    virtual LPTHREAD_START_ROUTINE get_routine(){ return routine; };
    virtual void on_stop();
};