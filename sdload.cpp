//
// sdload.cpp
//
// To do: more patches
// To do: saving patches
//

#include <circle/string.h>
#include <circle/util.h>

#include "sdload.h"

float CSDLoad::s_Buffer[MAX_TABLES * TABLE_SIZE] = { 0.0f };
unsigned CSDLoad::s_TableSize = TABLE_SIZE;
unsigned CSDLoad::s_NumTables = 0;


CSDLoad::CSDLoad(CInterruptSystem* pInterrupt, CTimer* pTimer)
: m_EMMC(pInterrupt, pTimer),
  m_patchCount(0)
{
	memset(m_patchBuffer, 0, sizeof(m_patchBuffer));
	memset(m_patchBufferSize, 0, sizeof(m_patchBufferSize));
}

float* CSDLoad::GetBuffer() { return s_Buffer; }
unsigned CSDLoad::GetNumTables() { return s_NumTables; }
unsigned CSDLoad::GetTableSize() { return s_TableSize; }


bool CSDLoad::Initialize()
{
    if (!m_EMMC.Initialize())
        return false;

    return true;
}


bool CSDLoad::LoadWavetable() // LOAD WAVETABLES
{
    if (f_mount(&m_FileSystem, DRIVE, 1) != FR_OK)
    { return false; }
    
    FIL File;
    FRESULT Result = f_open(&File, DRIVE FILENAME, FA_READ | FA_OPEN_EXISTING);
    if (Result != FR_OK)
    {
        f_mount(nullptr, DRIVE, 0);
        return false;
    }

    // Optional 8-byte header (ignored)
    uint32_t header[2] = {0, 0};
    UINT bytesRead = 0;
    f_read(&File, header, sizeof(header), &bytesRead);

	s_NumTables = header[0];
	s_TableSize = header[1];
	
	unsigned totalTables = s_NumTables * 2;
	unsigned totalSamples = totalTables * s_TableSize;
	
	if (totalTables > MAX_TABLES)
		totalTables = MAX_TABLES;
    
    if (totalSamples > MAX_TABLES * TABLE_SIZE)
		totalSamples = MAX_TABLES * TABLE_SIZE;

    memset(s_Buffer, 0, sizeof(s_Buffer));
    Result = f_read(&File, s_Buffer, totalSamples * sizeof(float), &bytesRead);
    f_close(&File);
    f_mount(nullptr, DRIVE, 0);

    if (Result != FR_OK || bytesRead != totalSamples * sizeof(float))
    {
        memset(s_Buffer, 0, sizeof(s_Buffer));
        return false;
    }

    return true;
}


bool CSDLoad::LoadPatches() // LOAD PATCHES
{
	m_patchCount = 0;
	
	// Fixed array of filenames
    const char* patchFiles[MAX_PATCHES] =
    {
        "SD:/patches/patch0.txt",
        "SD:/patches/patch1.txt",
        "SD:/patches/patch2.txt",
        "SD:/patches/patch3.txt",
        "SD:/patches/patch4.txt",
        "SD:/patches/patch5.txt",
        "SD:/patches/patch6.txt",
        "SD:/patches/patch7.txt"
    };
	
	if (f_mount(&m_FileSystem, DRIVE, 1) != FR_OK)
	{	return false;  }
		
	
	for (unsigned i = 0; i < MAX_PATCHES; ++i)
    {
		FIL File;
		if(f_open(&File, patchFiles[i], FA_READ | FA_OPEN_EXISTING) != FR_OK)
			break;  // stop if file not found

		UINT bytesRead = 0;
		if (f_read(&File, m_patchBuffer[i], sizeof(m_patchBuffer[i])-1, &bytesRead) != FR_OK)
			return false;
			
		if (!bytesRead)
			return false;	

		f_close(&File);
		
		m_patchBuffer[i][bytesRead] = '\0'; // Ensure null terminated
		
		m_patchBufferSize[i] = bytesRead;
		
		++m_patchCount;
	}

    return (m_patchCount > 0);
}
