//
// moogFilter.h
//

#ifndef _moogfilter_h
#define _moogfilter_h


class CMoogFilter
{
public:
    CMoogFilter(float sampleRate);

	void SetCutoff(float cut);
	void SetResonance(float res);
    
    void Reset();
    
    inline float fast_tanh(float x);
	inline float fast_tanf(float x);
	inline float fast_exp_neg(float x);
	
	float Process(float sample);
    
private:
	float m_sampleRate;
	float m_nyquist;

	float m_cutoff;
	float m_resonance;
    
    // Filter state variables
	float m_stage[4];
	float m_stageTanh[3];
	float m_delay[6];

	float m_thermal;
	float m_tune;
	float m_acr;
	float m_resQuad;

};
#endif
