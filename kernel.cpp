//
// kernel.cpp
//
// based on Circle's examples

#include "kernel.h"
#include <circle/machineinfo.h>
#include <assert.h>


static const char FromKernel[] = "kernel";

CKernel::CKernel (void)
:	m_Screen (m_Options.GetWidth (), m_Options.GetHeight (), Font12x22, CCharGenerator::FontFlagsDoubleBoth),
	m_Logger (m_Options.GetLogLevel ()),
	m_Timer (&m_Interrupt),
	m_I2CMaster(CMachineInfo::Get()->GetDevice(DeviceI2CMaster), true),
	m_SDLoader(&m_Interrupt, &m_Timer),
	m_LutSynth(&m_I2CMaster, &m_Interrupt, &m_SDLoader)
{
	m_ActLED.Blink (1);	// show we are alive
}

CKernel::~CKernel (void)
{
}

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;
	
	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}
	
	if (bOK)
	{
		CDevice *pTarget = m_DeviceNameService.GetDevice (m_Options.GetLogDevice (), FALSE);
		if (pTarget == 0)
		{
			pTarget = &m_Screen;
		}

		bOK = m_Logger.Initialize (pTarget);
	}	

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}
	
	if (bOK)
	{
		bOK = m_I2CMaster.Initialize ();
	}

	if (bOK)
	{
		bOK = m_SDLoader.Initialize();
	}
	if (bOK)
	{
		bOK = m_SDLoader.LoadWavetable();
	}

	if (bOK)
	{
		bOK = m_LutSynth.Initialize ();
	}
	
	if (bOK)
	{
		bOK = m_SDLoader.LoadPatches();
	}
	
	if (bOK)
	{
		bOK = m_LutSynth.InitPatches();
	}

	if (bOK)
	{
		bOK = m_LutSynth.StartSoundDevice();
	}

	return bOK;
}

TShutdownMode CKernel::Run (void)
{	
	CLogger::Get ()->Write(FromKernel, LogNotice, "INIT OK");
	while (TRUE)
	{
		m_LutSynth.Process ();	
	}

	return ShutdownHalt;
}
