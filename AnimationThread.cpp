#include "stdafx.h"
#include "AnimationThread.h"
#include "Fractal.h"
#include "FractalDlg.h"
#include "messages.h"

CAnimationThread::CAnimationThread(CFractalDlg * dialog, unsigned zoom_count) 
    : dialog(dialog), current_thread(NULL), zoom_count(zoom_count)
{
    ASSERT(dialog != NULL);
}


void CAnimationThread::calculate_frame(bool zoom)
{
    current_thread = dialog->NewCalculationThread();
    current_thread->start();
    current_thread->wait();
    if( !is_stopped() )
        dialog->PostDrawAndWait();
    delete current_thread;
    current_thread = NULL;
    if( !is_stopped() && zoom )
        dialog->PostZoomAndWait();
}

void CAnimationThread::post_message(UINT msg)
{
	::PostMessage(dialog->m_hWnd, msg, 0, 0);
}

DWORD __stdcall CAnimationThread::routine( void * param )
{
	CAnimationThread *thread = static_cast<CAnimationThread*>( param );
	OutputDebugString(_T("Animation Thread Started\n"));
    unsigned zoom_count = thread->get_zoom_count();
	for(unsigned i = 0; i < zoom_count; ++i)
	{
		if( thread->is_stopped() )
		{
			OutputDebugString(_T("\nAnimation Thread Interrupted\n"));
			return 1;
		}
        thread->calculate_frame( i != zoom_count - 1);
	}
	Sleep(100);
	OutputDebugString(_T("\nAnimation Thread Finished\n"));
	thread->post_message(MSG_ANIMATION_FINISHED);
	return 0;
}
RGBQUAD *CAnimationThread::get_pixels()
{
    if (current_thread == NULL)
        return NULL;
    else
        return current_thread->get_pixels();
}


void CAnimationThread::on_stop()
{
    if( current_thread != NULL )
        current_thread->stop();
}
