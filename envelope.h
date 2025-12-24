//
// envelope.h
//

#ifndef _ENVELOPE_H_
#define _ENVELOPE_H_

#include <circle/types.h>

class CEnvelope
{
public:
    CEnvelope(unsigned sampleRate);

    void NoteOn(void);
    void NoteOff(void);
    void NoteSlowKill(void);
    void NoteKill(void);

    void SetAttack(float ms);
    void SetDecay(float ms);
    void SetSustain(float level);
    void SetRelease(float ms);

    float Process(void);

    bool IsActive(void) const { return m_state != State::Idle; }

private:
    float m_sampleRate;
    
    enum class State { Idle, Attack, Decay, Sustain, Release, Kill };
    State m_state;

    float m_attackTime;
    float m_decayTime;
    float m_sustainLevel;
    float m_releaseTime;

    float m_level;
    float m_levelOff;
    float m_phase;         // 0..1 lookup position
    float m_phaseInc;      // advance per sample
    
    static const float expLUT[2048];
};

#endif
