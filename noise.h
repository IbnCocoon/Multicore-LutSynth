//
// noise.h
//
// adapted from:
// https://www.musicdsp.org/en/latest/Synthesis/216-fast-whitenoise-generator.html
//

#ifndef _NOISE_H
#define _NOISE_H

#include <circle/types.h>

class CNoise
{
public:
    CNoise(void);

    float GetSample(void);

private:
    s32 m_x1;
    s32 m_x2;

    // Output scaling
    float m_scale;
};

#endif
