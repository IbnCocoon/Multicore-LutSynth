//
// oscillator.cpp
//

#include "oscillator.h"
#include "sdload.h"

const float s_KeyFrequency[128] =
{
	8.17580, 8.66196, 9.17702, 9.72272, 10.3009, 10.9134, 11.5623, 12.2499, 12.9783, 13.7500,
	14.5676, 15.4339, 16.3516, 17.3239, 18.3540, 19.4454, 20.6017, 21.8268, 23.1247, 24.4997,
	25.9565, 27.5000, 29.1352, 30.8677, 32.7032, 34.6478, 36.7081, 38.8909, 41.2034, 43.6535,
	46.2493, 48.9994, 51.9131, 55.0000, 58.2705, 61.7354, 65.4064, 69.2957, 73.4162, 77.7817,
	82.4069, 87.3071, 92.4986, 97.9989, 103.826, 110.000, 116.541, 123.471, 130.813, 138.591,
	146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220.000, 233.082, 246.942,
	261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440.000,
	466.164, 493.883, 523.251, 554.365, 587.330, 622.254, 659.255, 698.456, 739.989, 783.991,
	830.609, 880.000, 932.328, 987.767, 1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91,
	1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53, 2093.00, 2217.46, 2349.32, 2489.02,
	2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520.00, 3729.31, 3951.07, 4186.01, 4434.92,
	4698.64, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040.00, 7458.62, 7902.13,
	8372.02, 8869.84, 9397.27, 9956.06, 10548.1, 11175.3, 11839.8, 12543.9
};

static inline float KeyToFreq(unsigned key)
{
	return s_KeyFrequency[key];
}


COscillator::COscillator()
: m_waveformSaw(0.0f),
  m_waveformSquare(0.0f),
  m_waveformTri(0.0f), 
  m_phase(0.0f),
  m_phaseInc(0.0f),
  m_phaseOffset(0.0f),
  m_pulseWidth(0.5f),
  m_frequency(0.0f),
  m_currentFrequency(0.0f),
  m_targetFrequency(0.0f),
  m_triState(0.0f),
  m_glideTime(1.0f),
  m_tableIndex(0),
  m_tableFrac(0.0f),
  m_osc2Kbd(1),
  m_osc2Low(0),
  m_sync(false)
{
}

void COscillator::Reset()
{
	m_phase = 0.0f;
	m_sync = false;
}

void COscillator::SetGlideTime(float time) 
{
	time = (time<0.0f) ? 0.0f : (time>1.0f) ? 1.0f : time;
	
	if (!time)
		m_glideTime = 1.0f;
	else
		m_glideTime = (1.0f - time) * 0.003f + 0.0001f; // 0.003f->fast 0.0001f->slow
}

// --- Frequency to Table mapping ---
void COscillator::SetFrequency(unsigned key, float transpose, float fine, float lfo, float polyMod, unsigned sampleRate)
{
	m_note = key;
	
	if (!m_osc2Kbd && !m_osc2Low)
		m_currentFrequency = 440.0f + transpose * 35.0f;
	
	if (!m_osc2Kbd && m_osc2Low)
		m_currentFrequency = 12.1f + transpose;
	
	if (m_osc2Kbd && !m_osc2Low)
	{
		m_note += transpose;
		m_currentFrequency = KeyToFreq(m_note);
	}
	
	if (m_osc2Kbd && m_osc2Low)
		m_currentFrequency = KeyToFreq(m_note)/100.0f + 12.1f + transpose;
		
		
	float f = fine * 0.11892621789f - 0.05946309436f;
	f = f * m_currentFrequency;
	m_currentFrequency += f;

	float mod = (lfo + polyMod) * m_currentFrequency;
	m_currentFrequency += mod;
	
	if (m_currentFrequency < 0.0f) 
		m_currentFrequency = 0.0f;	
			
	m_targetFrequency = m_currentFrequency;
	m_frequency += (m_targetFrequency - m_frequency) * m_glideTime;
	
    float tableIndex = m_note * 0.5f;
    m_tableIndex = (unsigned)tableIndex;
    m_tableFrac  = tableIndex - m_tableIndex;

	m_phaseInc = m_frequency / (float)sampleRate;
}

void COscillator::SetPulseWidth(float pw)
{
	m_pulseWidth = pw;
	m_pulseWidth = (m_pulseWidth<0.1f) ? 0.1f : (m_pulseWidth>0.9f) ? 0.9f : m_pulseWidth;
}

 /////////////////////////////////
 //		Poly Blep Oscillator    //
 /////////////////////////////////
 
inline float COscillator::PolyBlep(float t, float dt)
{
    if (t < dt) {
        float x = t / dt;
        return x + x - x * x - 1.0f;
    }
    else if (t > 1.0f - dt) {
        float x = (t - 1.0f) / dt;
        return x * x + x + x + 1.0f;
    }
    return 0.0f;
}

float COscillator::GetSaw()
{
    float phase = m_phase;        // 0..1
    float dt = m_phaseInc;

    // Naive saw
    float out = 2.0f * phase - 1.0f;

    // Fix discontinuity at phase wrap
    out -= PolyBlep(phase, dt);

    return out;
}

float COscillator::GetSquare()
{
    float phase = m_phase;
    float dt = m_phaseInc;
    float pw = m_pulseWidth;  // 0..1

    float out = (phase < pw) ? 1.0f : -1.0f;

    // Apply polyBLEP at rising and falling edges
    out += PolyBlep(phase, dt);               // rising edge
    
    float t2 = phase - pw;
    if (t2 < 0.0f)
		t2 += 1.0f;
    out -= PolyBlep(t2, dt);                  // falling edge

    return out;
}

float COscillator::GetTri()
{
    float phase = m_phase;
    float dt = m_phaseInc;
    float pw = 0.5f;  // 0..1
    float sqOut = (phase < pw) ? 1.0f : -1.0f;
    sqOut += PolyBlep(phase, dt);
    float t2 = phase - pw;
    if (t2 < 0.0f)
		t2 += 1.0f;
    sqOut -= PolyBlep(t2, dt);

    m_triState = m_triState * 0.999f + sqOut * m_phaseInc;
    return m_triState * 4.0f;
}

float COscillator::GetSample()
{
    if (m_currentFrequency <= 0.0f)
        return 0.0f;

    float sample = 0.0f;

    float sawSample = GetSaw();
    float squareSample = GetSquare();
    float triSample = GetTri();
    
    sample = m_waveformSaw * sawSample - m_waveformSquare * squareSample +
			 m_waveformTri * triSample;

    m_phase += m_phaseInc;
    if (m_phase >= 1.0f)
    {
        m_phase -= 1.0f;
        m_sync = true;
    }

    return sample;
}
