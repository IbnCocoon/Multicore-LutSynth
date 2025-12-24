//
// serialmidi.cpp
//
// based on mini dexed code
//

#include "serialmidi.h"
#include <assert.h>

CSerialMidi::CSerialMidi(CInterruptSystem *pInterrupt, CMidiHandler *pTarget)
:   m_Serial(pInterrupt, TRUE),
    m_nSerialState(0),
    m_pTarget(pTarget)
{
}

CSerialMidi::~CSerialMidi(void)
{
}

bool CSerialMidi::Initialize(unsigned nBaudRate)
{
    return m_Serial.Initialize(nBaudRate);
}

void CSerialMidi::Process(void)
{
    u8 Buffer[20];
    int nResult = m_Serial.Read(Buffer, sizeof Buffer);
    if (nResult <= 0)
    {
        return;
    }

    for (int i = 0; i < nResult; i++)
    {
        u8 uchData = Buffer[i];

        switch (m_nSerialState)
        {
        case 0:
				MIDIRestart:
				if (   (uchData & 0x80) == 0x80   // Note on/off
					&& (uchData & 0xF0) != 0xF0)  // 
				{
					m_SerialMessage[m_nSerialState++] = uchData;
				}
				break;

        case 1:
        case 2:
				DATABytes:
				if (uchData & 0x80)   // unexpected status byte
				{
					m_nSerialState = 0;
					goto MIDIRestart;
				}

				m_SerialMessage[m_nSerialState++] = uchData;

				if (   (m_SerialMessage[0] & 0xE0) == 0xC0
					|| m_nSerialState == 3
					|| (m_SerialMessage[0] & 0xF0) == 0xD0)
				{
					if (m_pTarget)
					{
						m_pTarget->HandleMidiMessage(m_SerialMessage, m_nSerialState);
					}
                m_nSerialState = 4;
				}
				break;
		case 4:
				if ((uchData & 0x80) == 0)  // true data byte, false status byte
				{
					m_nSerialState = 1;
					goto DATABytes;
				}
				else 
				{
				m_nSerialState = 0;
				goto MIDIRestart; 
				}
				break;
        default:
				assert(0);
				break;
        }
    }
}

