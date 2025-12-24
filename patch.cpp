//
// patch.cpp
//

#include <circle/string.h>
#include "patch.h"
#include "sdload.h"
#include "voicehandler.h"

#define VOL_ATTACK  1
#define VOL_DECAY	2
#define VOL_SUSTAIN 3
#define VOL_RELEASE 4
#define FIL_ATTACK  5
#define FIL_DECAY	6
#define FIL_SUSTAIN 7
#define FIL_RELEASE 8

CPatch::CPatch(CVoiceHandler *pVoices)
:  m_pVoices(pVoices),
   m_pData(nullptr),
   m_dataSize(0)
{
}

void CPatch::SetBuffer(const char* pBuffer, unsigned size)
{
	m_pData = pBuffer;
	m_dataSize = size;
}


void CPatch::ApplyParam(const char* key, float f)
{
	if(!m_pVoices)
		return;
		
	// Dispatch parameters to voicehandler
	if(strcmp(key, "Osc1-Coarse") == 0)
		m_pVoices->SetCoarse(0, f);
	else if (strcmp(key, "Osc1-Fine") == 0)
		m_pVoices->SetFine(0, f);
	else if (strcmp(key, "Osc1-Saw") == 0)
		m_pVoices->SetWaveformSaw(0, f);
	else if (strcmp(key, "Osc1-Square") == 0)
		m_pVoices->SetWaveformSquare(0, f);
	else if (strcmp(key, "Osc1-PulseWidth") == 0)
		m_pVoices->SetPulseWidth(0, f);
	else if (strcmp(key, "Osc1-Sync") == 0)
		m_pVoices->SetOsc1Sync(f);
	else if (strcmp(key, "Osc1-Vol") == 0)
		m_pVoices->SetOscVol(0, f);
	else if(strcmp(key, "Osc2-Coarse") == 0)
		m_pVoices->SetCoarse(1, f);
	else if (strcmp(key, "Osc2-Fine") == 0)
		m_pVoices->SetFine(1, f);
	else if (strcmp(key, "Osc2-Saw") == 0)
		m_pVoices->SetWaveformSaw(1, f);
	else if (strcmp(key, "Osc2-Square") == 0)
		m_pVoices->SetWaveformSquare(1, f);
	else if (strcmp(key, "Osc2-Tri") == 0)
		m_pVoices->SetWaveformTri(f);
	else if (strcmp(key, "Osc2-PulseWidth") == 0)
		m_pVoices->SetPulseWidth(1, f);
	else if (strcmp(key, "Osc2-KbdTrk") == 0)
		m_pVoices->SetOsc2KbdTrk(f);
	else if (strcmp(key, "Osc2-LowFreq") == 0)
		m_pVoices->SetOsc2LowFreq(f);
	else if (strcmp(key, "Osc2-Vol") == 0)
		m_pVoices->SetOscVol(1, f);
	else if (strcmp(key, "Filter-Cutoff") == 0)
		m_pVoices->SetCutoff(f);
	else if (strcmp(key, "Filter-Res") == 0)
		m_pVoices->SetResonance(f);
	else if (strcmp(key, "Filter-EnvAmt") == 0)
		m_pVoices->SetEnvAmt(f);
	else if (strcmp(key, "Filter-KbdTrk") == 0)
		m_pVoices->SetKbdTrk(f);
	else if (strcmp(key, "Lfo-Freq") == 0)
		m_pVoices->SetLfoFreq(f);
	else if (strcmp(key, "Lfo-Wave") == 0)
		m_pVoices->SetLfoWave(f);
	else if (strcmp(key, "Lfo-Mix") == 0)
		m_pVoices->SetLfoMix(f);
	else if (strcmp(key, "Lfo-Filter") == 0)
		m_pVoices->SetLfoFilter(f);
	else if (strcmp(key, "Lfo-Fm1") == 0)
		m_pVoices->SetLfoFm(0, f);
	else if (strcmp(key, "Lfo-Fm2") == 0)
		m_pVoices->SetLfoFm(1, f);
	else if (strcmp(key, "Lfo-Pwm1") == 0)
		m_pVoices->SetPulseWidthModulation(0, f);
	else if (strcmp(key, "Lfo-Pwm2") == 0)
		m_pVoices->SetPulseWidthModulation(1, f);
	else if (strcmp(key, "Polymod-FilEnv") == 0)
		m_pVoices->SetPolyModFilEnv(f);
	else if (strcmp(key, "Polymod-Osc2") == 0)
		m_pVoices->SetPolyModOsc2(f);
	else if (strcmp(key, "Polymod-Pw") == 0)
		m_pVoices->SetPolyModPw(f);
	else if (strcmp(key, "Polymod-Filter") == 0)
		m_pVoices->SetPolyModFil(f);
	else if (strcmp(key, "Polymod-Freq") == 0)
		m_pVoices->SetPolyModFreq(f);
	else if (strcmp(key, "VolEnv-A") == 0)
		m_pVoices->SetEnv(VOL_ATTACK, f);
	else if (strcmp(key, "VolEnv-D") == 0)
		m_pVoices->SetEnv(VOL_DECAY, f);
	else if (strcmp(key, "VolEnv-S") == 0)
		m_pVoices->SetEnv(VOL_SUSTAIN, f);
	else if (strcmp(key, "VolEnv-R") == 0)
		m_pVoices->SetEnv(VOL_RELEASE, f);
	else if (strcmp(key, "FilEnv-A") == 0)
		m_pVoices->SetEnv(FIL_ATTACK, f);
	else if (strcmp(key, "FilEnv-D") == 0)
		m_pVoices->SetEnv(FIL_DECAY, f);
	else if (strcmp(key, "FilEnv-S") == 0)
		m_pVoices->SetEnv(FIL_SUSTAIN, f);
	else if (strcmp(key, "FilEnv-R") == 0)
		m_pVoices->SetEnv(FIL_RELEASE, f);
	else if (strcmp(key, "NoiseVol") == 0)
		m_pVoices->SetNoiseVol(f);
	else if (strcmp(key, "Modwheel") == 0)
		m_pVoices->SetModWheel(f);
	else if (strcmp(key, "Unison") == 0)
		m_pVoices->SetUnison(f);
	else if (strcmp(key, "UnisonSpread") == 0)
		m_pVoices->SetUnisonSpread(f);
	else if (strcmp(key, "UnisonVoices") == 0)
		m_pVoices->SetUnisonVoices(f);
	else if (strcmp(key, "GlideTime") == 0)
		m_pVoices->SetGlideTime(f);
	else if (strcmp(key, "Gain") == 0)
		m_pVoices->SetGain(f);		
}


void CPatch::Parse()
{
    if (!m_pData || !m_dataSize)
        return;

    CString key;
    CString val;
    bool readingValue = false;

    for (unsigned i = 0; i < m_dataSize; ++i)
    {
        char c = m_pData[i];
        
        if (c == '#' || c == '\n' || c == '\r')
        {
            if(key.GetLength() && val.GetLength())
            {
				float fVal = Atof(val.c_str());
				ApplyParam(key.c_str(), fVal);
			}
			
			key = "";
			val = "";
			readingValue = false;
			continue;
        }

        // Find '='
        if (c == '=')
        {
            readingValue = true;
            continue;
        }

		if (readingValue == false)
			key += c;
		else
			val += c;
	}
	
	if (key.GetLength() && val.GetLength())
    {
		float fVal = Atof(val.c_str());
		ApplyParam(key.c_str(), fVal);
	}
}


// ---------------------------------------------------------
// Minimal atof() replacement — supports ± and 3 decimal precision
// Example inputs: "0.5", "-12.345", "127", "3.14"
// ---------------------------------------------------------
float CPatch::Atof(const char* str)
{
    if (!str) return 0.0f;

    float sign = 1.0f;
    float value = 0.0f;
    float frac = 0.0f;
    int decimals = 0;
    bool pastDot = false;

    // Skip leading spaces
    while (*str == ' ' || *str == '\t') str++;

    // Handle sign
    if (*str == '-') { sign = -1.0f; ++str; }
    else if (*str == '+') { ++str; }

    // Parse digits
    for (; *str; ++str)
    {
        if (*str == '.' && !pastDot)
        {
            pastDot = true;
        }
        else if (*str >= '0' && *str <= '9')
        {
            int digit = *str - '0';
            if (!pastDot)
            {
                value = value * 10.0f + (float)digit;
            }
            else if (decimals < 3)  // limit to 3 decimal precision
            {
                frac = frac * 10.0f + (float)digit;
                decimals++;
            }
        }
        else
        {
            break;
        }
    }

    while (decimals--)
        frac *= 0.1f;

    return sign * (value + frac);
}
