#pragma once
#include "Thread.h"

class CFractalDlg;

class CAnimationThread : public CThread
{
private:
    CCalculationThread * current_thread;
    CFractalDlg * dialog;
    unsigned zoom_count;
public:
    CAnimationThread(CFractalDlg * dialog, unsigned zoom_count);
    unsigned get_zoom_count() { return zoom_count; }
    void calculate_frame();
    void post_message(UINT msg);
	static DWORD WINAPI routine( void * param);
    virtual LPTHREAD_START_ROUTINE get_routine(){ return routine; };
    virtual void on_stop();
};