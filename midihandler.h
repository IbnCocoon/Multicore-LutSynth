//
// midihandler.h
//

#ifndef _midihandler_h
#define _midihandler_h

#include <circle/actled.h>
#include <circle/interrupt.h>
#include <circle/types.h>
#include <circle/string.h>

#include "serialmidi.h"
#include "voicehandler.h"

#define MIDI_NOTE_ON 	 0x9
#define MIDI_NOTE_OFF 	 0x8
#define MIDI_CC		 	 0xb
#define MIDI_PROG_CHANGE 0xc
#define MIDI_PITCH_BEND  0xe

#define CC_GENERAL_MODWHEEL 1
#define CC_GENERAL_VOLUME   7
#define CC_GENERAL_UNISON  55
#define CC_GENERAL_SPREAD  56
#define CC_GENERAL_GLIDE   57
#define CC_GENERAL_UNISON_VOICES 58

#define CC_CUTOFF 		14
#define CC_RESONANCE	15
#define CC_FIL_ENV_AMT	16
#define CC_FIL_KBDTRK	17

#define CC_VOL_ATTACK   20
#define CC_VOL_DECAY    21
#define CC_VOL_SUSTAIN  22
#define CC_VOL_RELEASE  23
#define CC_FIL_ATTACK   24
#define CC_FIL_DECAY    25
#define CC_FIL_SUSTAIN  26
#define CC_FIL_RELEASE  27

#define CC_OSC_1_COARSE	28
#define CC_OSC_1_FINE	29
#define CC_OSC_1_SAW	30
#define CC_OSC_1_SQUARE	59
#define CC_OSC_1_PW		31
#define CC_OSC_1_VOL	32
#define CC_OSC_1_SYNC	54

#define CC_OSC_2_COARSE	33
#define CC_OSC_2_FINE	34
#define CC_OSC_2_SAW	35
#define CC_OSC_2_SQUARE	60
#define CC_OSC_2_TRI	61
#define CC_OSC_2_PW		36
#define CC_OSC_2_VOL	37
#define CC_OSC_2_KBD	52
#define CC_OSC_2_LOW	53

#define CC_NOISE_VOL	51

#define CC_LFO_FM1		45
#define CC_LFO_FM2		38
#define CC_LFO_PWM1		39
#define CC_LFO_PWM2		40
#define CC_LFO_FILTER	41
#define CC_LFO_FREQ		42
#define CC_LFO_SAW		43
#define CC_LFO_SQUARE	62
#define CC_LFO_TRI		63
#define CC_LFO_MIX		44

#define CC_POLYMOD_FREQ   46
#define CC_POLYMOD_PW     47
#define CC_POLYMOD_FIL    48
#define CC_POLYMOD_OSC2   49
#define CC_POLYMOD_FILENV 50




class CSerialMidi;    // forward
class CVoiceHandler;  // forward

class CMidiHandler
{
public:
    CMidiHandler(CInterruptSystem *pInterrupt);
    ~CMidiHandler(void);

    bool Initialize(unsigned nBaudRate = 31250);
    void Process(void);

    void SetVoiceHandler(CVoiceHandler *pVoices) { m_pVoices = pVoices; }

    // Called by SerialMidi when a full 3-byte message is ready
    void HandleMidiMessage(u8 *pMessage, unsigned nLength);

private:
    CSerialMidi*     m_pSerialMidi;  // pointer to serial MIDI driver

    CActLED          m_ActLED;
    unsigned         m_nLEDTicks;
    
    CVoiceHandler*   m_pVoices;
};

#endif
