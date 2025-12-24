//
// serialmidi.h
//

#ifndef _serialmidi_h
#define _serialmidi_h

#include <circle/interrupt.h>
#include <circle/serial.h>
#include <circle/types.h>
#include "midihandler.h"

class CMidiHandler;   // forward declaration

class CSerialMidi
{
public:
    CSerialMidi(CInterruptSystem *pInterrupt, CMidiHandler *pTarget);
    ~CSerialMidi(void);

    bool Initialize(unsigned nBaudRate = 31250);
    void Process(void);

private:
    CSerialDevice m_Serial;
    unsigned m_nSerialState;
    u8 m_SerialMessage[3];

    CMidiHandler* m_pTarget;   // to deliver MIDI messages
};

#endif
