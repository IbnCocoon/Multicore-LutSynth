//
// mixer.h
//
#ifndef _MIXER_H_
#define _MIXER_H_

#include <circle/types.h>
#include "voicehandler.h"

class CMixer
{
public:
	CMixer(CVoiceHandler *pVoices);
	~CMixer();

	// Render interleaved float buffer with nChannels (duplicate mono voice into channels)
	// pOutBuffer should be length >= nFrames * nChannels
	
	void GetMixedVoices(float *pOutBuffer, unsigned nFrames, unsigned nChannels);

private:
	CVoiceHandler *m_pVoices;
};

#endif
