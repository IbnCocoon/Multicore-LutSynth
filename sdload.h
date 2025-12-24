//
// sdload.h
//

#ifndef _SDLOAD_H_
#define _SDLOAD_H_

#include <circle/actled.h>
#include <circle/types.h>
#include <SDCard/emmc.h>
#include <fatfs/ff.h>

#define TABLE_SIZE 512
#define MAX_TABLES  512
#define MAX_PATCHES     8

#define DRIVE 	 "SD:"
#define FILENAME "/sawtri64.bin" 
#define PATCH_FILENAME "/patches.txt" 

class CInterruptSystem;
class CTimer;

class CSDLoad
{
public:
    CSDLoad(CInterruptSystem* pInterrupt, CTimer* pTimer);
	
	bool Initialize();                 	// Mount and read file from SD

    static float* GetBuffer();    		// Lut
	static unsigned GetNumTables();		// Lut
	static unsigned GetTableSize();		// Lut
    
    const char* GetPatchBuffer(unsigned index) const { return m_patchBuffer[index]; }
    unsigned GetPatchBufferSize(unsigned index) const { return m_patchBufferSize[index]; }
    unsigned GetPatchCount() const { return m_patchCount; }

    bool LoadWavetable();              
    bool LoadPatches();
	
private:
    CEMMCDevice m_EMMC;
    FATFS       m_FileSystem;
    CActLED          m_ActLED;
    
    static float s_Buffer[MAX_TABLES * TABLE_SIZE];
    static unsigned s_TableSize;
    static unsigned s_NumTables;
    
    static const unsigned MAX_BUFFER_SIZE = 4096; //4kb max text size
    char m_patchBuffer[MAX_PATCHES][MAX_BUFFER_SIZE];
    unsigned m_patchBufferSize[MAX_PATCHES];
    unsigned m_patchCount;
};

#endif
