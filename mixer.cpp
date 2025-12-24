//
// mixer.cpp
//
// To do: add fx


#include "mixer.h"
#include "lutsynth.h"
#include <circle/util.h>

CMixer::CMixer(CVoiceHandler *pVoices)
: m_pVoices(pVoices)
{
}

CMixer::~CMixer()
{
}


void CMixer::GetMixedVoices(float *pOutBuffer, unsigned nFrames, unsigned nChannels)
{
	if (!m_pVoices)
		return;

	float voiceBuf[nFrames];
	
	for (unsigned i = 0; i < nFrames; ++i)
	{
		voiceBuf[i] = m_pVoices->RenderMixedVoices();
		
		for (unsigned ch = 0; ch < nChannels; ++ch)
		{
			pOutBuffer[i * nChannels + ch] = voiceBuf[i]; //pOutBuffer stereo buffer
		}
	}
}

