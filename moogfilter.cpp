//
// moogfilter.cpp
//
// adapted from:
// https://github.com/ddiakopoulos/MoogLadders/blob/master/src/HuovilainenModel.h
// Based on implementation in CSound5 (LGPLv2.1)
// https://github.com/csound/csound/blob/develop/COPYING
/*
Huovilainen developed an improved and physically correct model of the Moog
Ladder filter that builds upon the work done by Smith and Stilson. This model
inserts nonlinearities inside each of the 4 one-pole sections on account of the
smoothly saturating function of analog transistors. The base-emitter voltages of
the transistors are considered with an experimental value of 1.22070313 which
maintains the characteristic sound of the analog Moog. This model also permits
self-oscillation for resonances greater than 1. The model depends on five
hyperbolic tangent functions (tanh) for each sample, and an oversampling factor
of two (preferably higher, if possible). Although a more faithful
representation of the Moog ladder, these dependencies increase the processing
time of the filter significantly. Lastly, a half-sample delay is introduced for 
phase compensation at the final stage of the filter. 

References: Huovilainen (2004), Huovilainen (2010), DAFX - Zolzer (ed) (2nd ed)
Original implementation: Victor Lazzarini for CSound5

Considerations for oversampling: 
http://music.columbia.edu/pipermail/music-dsp/2005-February/062778.html
http://www.synthmaker.co.uk/dokuwiki/doku.php?id=tutorials:oversampling
*/ 


#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

#include <circle/logger.h>
#include "moogfilter.h"

static const char FromFilter[] = "filter";

CMoogFilter::CMoogFilter (float sampleRate)
: m_sampleRate(sampleRate),
  m_nyquist(0),
  m_cutoff(5000.0f),
  m_resonance(0),
  m_thermal(0.000025f),
  m_tune(0),
  m_acr(0),
  m_resQuad(0)
{
	Reset();
}

void CMoogFilter::Reset()
{
	m_nyquist = m_sampleRate / 2.0f;

	for (int i = 0; i < 3; i++)
		m_stageTanh[i] = 0.0f;
		
	for (int i = 0; i < 4; i++)
		m_stage[i] = 0.0f;
		
	for (int i = 0; i < 6; i++)
		m_delay[i] = 0.0f;
	
	SetCutoff(m_cutoff);
}

void CMoogFilter::SetCutoff(float cut)
{
	m_cutoff = cut; //m_cutoff = m_nyquist * cut;
	m_cutoff = (m_cutoff < 20.0f) ? 20.0f : (m_cutoff > m_nyquist) ? m_nyquist : m_cutoff;

	float fc =  m_cutoff / m_sampleRate;
	float fc2 = fc * fc;
	float fc3 = fc * fc * fc;

	float fcr = 1.8730 * fc3 + 0.4955 * fc2 - 0.6490 * fc + 0.9988;
	m_acr = -3.9364 * fc2 + 1.8409 * fc + 0.9968;

	float x = (2.0f * M_PI) * fc * fcr;
	m_tune = (1.0f - fast_exp_neg(x)) / m_thermal; 

	SetResonance(m_resonance);
}


void CMoogFilter::SetResonance(float res)
{		
	m_resonance = res; // if m_resQuad  > 2.6f the filter blows 
	m_resQuad = 2.6f * m_resonance * m_acr; // m_resQuad = 4.0 * m_resonance * m_acr;
}

float CMoogFilter::Process(float sig)
{
	float input = sig - m_resQuad * m_delay[5];
	m_stage[0] = m_delay[0] + m_tune * (fast_tanh(input * m_thermal) - m_stageTanh[0]);
	m_delay[0] = m_stage[0];
	for (int k = 1; k < 4; k++) 
	{
		input = m_stage[k-1];
		m_stage[k] = m_delay[k] + m_tune * ((m_stageTanh[k-1] = fast_tanh(input * m_thermal)) - (k != 3 ? m_stageTanh[k] : fast_tanh(m_delay[k] * m_thermal)));
		m_delay[k] = m_stage[k];
	}
	// 0.5 sample delay for phase compensation
	m_delay[5] = (m_stage[3] + m_delay[4]) * 0.5;
	m_delay[4] = m_stage[3];
	
	return m_delay[5];
}

inline float CMoogFilter::fast_tanh(float x)
{
    float x2 = x * x;
    return x * (27.f + x2) / (27.f + 9.f * x2);
}

inline float CMoogFilter::fast_tanf(float x)
{
	float x2 = x * x;
	return x * (27.0f + x2) / (27.0f - x2);
}

inline float CMoogFilter::fast_exp_neg(float x)
{
	float x2 = x * x;
	float x3 = x2 * x;
	return 1.0f / (1.0f + x + 0.48f * x2 + 0.235f * x3);
}

