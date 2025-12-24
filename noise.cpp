//
// noise.cpp
//
// adapted from:
// https://www.musicdsp.org/en/latest/Synthesis/216-fast-whitenoise-generator.html
//

#include "noise.h"

CNoise::CNoise(void)
{
    // Reference initial values
    m_x1 = 0x67452301;
    m_x2 = 0xefcdab89;

    m_scale = 1.0f / 4294967295.0f;
}

float CNoise::GetSample(void)
{
    m_x1 ^= m_x2;
    
    float out = (float)m_x2 * m_scale;
    
    m_x2 += m_x1;
    
    return out;
}
