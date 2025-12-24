//
// kernel.h
//

#ifndef _kernel_h
#define _kernel_h

#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/types.h>
#include <circle/i2cmaster.h>
#include <circle/screen.h>
#include <circle/logger.h>

#include "lutsynth.h"
#include "sdload.h"

enum TShutdownMode
{
	ShutdownNone,
	ShutdownHalt,
	ShutdownReboot
};

class CKernel
{
public:
	CKernel (void);
	~CKernel (void);

	boolean Initialize (void);

	TShutdownMode Run (void);

private:
	// do not change this order
	CActLED				m_ActLED;
	CKernelOptions		m_Options;
	CDeviceNameService	m_DeviceNameService;
	CExceptionHandler	m_ExceptionHandler;
	CScreenDevice		m_Screen;
	CLogger				m_Logger;
	CInterruptSystem	m_Interrupt;
	CTimer				m_Timer;
	CI2CMaster 			m_I2CMaster;

	CSDLoad				m_SDLoader;
	CLutSynth			m_LutSynth;
	
};

#endif
