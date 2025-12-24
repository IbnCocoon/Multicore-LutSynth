//
// lutsynth.h
//

#ifndef _lutsynth_h
#define _lutsynth_h

#include <circle/interrupt.h>
#include <circle/i2cmaster.h>
#include <circle/types.h>
#include <circle/sound/soundbasedevice.h>

#include "mixer.h"
#include "voicehandler.h"
#include "midihandler.h"


#define SAMPLE_RATE     48000
#define NYQUIST_RATE    24000
#define CHUNK_SIZE      256
#define DAC_I2C_ADDRESS 0   // I2C slave address of DAC (0 for auto probing)

class CLutSynth
{
public:
    CLutSynth(CI2CMaster* pI2CMaster, CInterruptSystem* pInterrupt, CSDLoad* pSD);
    ~CLutSynth(void);

    boolean Initialize(void);
    bool InitPatches(void);
    
    bool StartSoundDevice(void);
    
    void Process(void);

private:
    CI2CMaster*        m_pI2CMaster;
    CInterruptSystem*  m_pInterrupt;
	CSDLoad*		   m_pSDLoad;
    CSoundBaseDevice*  m_pSound;         // Audio device
    CVoiceHandler*     m_pVoices;        // Voices
    CMixer*            m_pMixer;         // Mixer
    CMidiHandler*      m_pMidiHandler;   // Midihandler (serial -> midi semantics)
};

#endif
