//
// patch.h
//

#ifndef _patch_h
#define _patch_h

#include <circle/types.h>

class CVoiceHandler;

class CPatch
{
public:
    CPatch(CVoiceHandler *pVoices = nullptr);

	void SetBuffer(const char* pBuffer, unsigned size);

	void Parse();

private:
	CVoiceHandler*   m_pVoices;
	
	const char* m_pData;
	unsigned m_dataSize;
	
	
	float Atof(const char* str);
	void ApplyParam(const char* key, float value);
};

#endif
