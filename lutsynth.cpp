//
// lutsynth.cpp
//
// sound output code based on MT-32 Pi
//


#include <circle/sound/i2ssoundbasedevice.h>
#include <circle/timer.h>
#include <circle/synchronize.h>
#include <circle/memory.h>
#include <circle/logger.h>

#include "lutsynth.h"

static const char FromLutSynth[] = "LutSynth";

#define BAUD_RATE 31250

constexpr float Sample24BitMax = ( 1 << (24-1) ) - 1;

CLutSynth::CLutSynth(CI2CMaster* pI2CMaster, CInterruptSystem* pInterrupt, CSDLoad* pSD)
:   m_pI2CMaster(pI2CMaster),
    m_pInterrupt(pInterrupt),
    m_pSDLoad(pSD),
    m_pSound(nullptr),
    m_pVoices(nullptr),
    m_pMixer(nullptr),
    m_pMidiHandler(nullptr)
{
}

CLutSynth::~CLutSynth(void)
{
    delete m_pMidiHandler;
    delete m_pMixer;
    delete m_pVoices;
    delete m_pSound;
}

boolean CLutSynth::Initialize(void)
{
    // Create I2S sound device (base class)
    m_pSound = new CI2SSoundBaseDevice(m_pInterrupt,
                                       SAMPLE_RATE,
                                       CHUNK_SIZE,
                                       false,
                                       m_pI2CMaster);

    m_pSound->SetWriteFormat(SoundFormatSigned24_32, 2);
    m_pSound->AllocateQueueFrames(CHUNK_SIZE);

    // Midi handler (serial MIDI created inside the handler)
    m_pMidiHandler = new CMidiHandler(m_pInterrupt);
    
    // Voices + mixer
    m_pVoices = new CVoiceHandler(CMemorySystem::Get (), SAMPLE_RATE);
    
    m_pMixer  = new CMixer(m_pVoices);
    
    m_pMidiHandler->SetVoiceHandler(m_pVoices);
    
    if (!m_pMidiHandler->Initialize(BAUD_RATE))
    {
		CLogger::Get ()->Write(FromLutSynth, LogNotice, "m_pMidiHandler->Initialize() failed");
		return false;
    }
    
    if (!m_pVoices->Initialize())
    {
		CLogger::Get ()->Write(FromLutSynth, LogNotice, "m_pVoices->Initialize() failed");
		return false;
    }

    return true;
}

bool CLutSynth::StartSoundDevice(void)
{
    if (!m_pSound->Start())
        return false;
    return true;
 }
 
bool CLutSynth::InitPatches(void)
{
    m_pVoices->InitPatches(m_pSDLoad);
    
    return true;
}


void CLutSynth::Process(void)
{
    constexpr u8 nChannels = 2;
    const u8 nBytesPerSample = 4;
    const u8 nBytesPerFrame  = nChannels * nBytesPerSample;
    const size_t nQueueSizeFrames = m_pSound->GetQueueSizeFrames();

    // interleaved float buffer (frames * channels)
    float FloatBuffer[nQueueSizeFrames * nChannels];
    s8 IntBuffer[nQueueSizeFrames * nBytesPerFrame];

    const size_t nFrames = nQueueSizeFrames - m_pSound->GetQueueFramesAvail();
    if (nFrames == 0) return;

    // First, let midihandler (which owns serialmidi) process incoming bytes
    if (m_pMidiHandler)
        m_pMidiHandler->Process();

    // render mixed audio into float buffer
    if (m_pMixer)
        m_pMixer->GetMixedVoices(FloatBuffer, nFrames, nChannels);

    // convert float -> signed24/32
    for (size_t i = 0; i < nFrames * nChannels; ++i)
    {
        s32* const pSample = reinterpret_cast<s32*>(IntBuffer + i * nBytesPerSample);
        *pSample = (s32)(FloatBuffer[i] * Sample24BitMax);
    }

    m_pSound->Write(IntBuffer, nFrames * nBytesPerFrame);
}
