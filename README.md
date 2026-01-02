This is the updated multicore version, much cleaner but with a lot of improvement to do.
- There is a Ctrlr panel that helps controlling via serial midi the synth parameters.
- The midi implementation can be seen in midihandler.h
- The patches files must be located in "/patches/patchX.txt". By now the code only reads eight patches files, but that can be later expanded with a better load implementation.

The parameters are:



Osc1-Coarse=0  // Oscillator 1 Pitch. +-12st. Stepped.

Osc1-Fine=0.5   //  Oscillator 1 Fine Pitch. 0<->1  /  0-> -100ct. 0.5->0ct. 1->100ct.

Osc1-Saw=1      // Oscillator 1 Waveform Select. 0->off 1->on.

Osc1-Square=0 // Oscillator 1 Waveform Select. 0->off 1->on.

Osc1-PulseWidth=0.5 // Oscillator 1 Pw. 0<->1  /   0->10% 1->90%.

Osc1-Sync=0 // Oscillator 1 Hard Sync. 0->off 1->on.

Osc1-Vol=1.0 // Oscillator 1 Volume. 0<->1

Osc2-Coarse=0

Osc2-Fine=0.5

Osc2-Saw=0

Osc2-Square=1

Osc2-Tri=0

Osc2-PulseWidth=0.5

Osc2-KbdTrk=1    // Oscillator 2 Keyboard tracking. 0->off 1->on.

Osc2-LowFreq=0 // Oscillator 2 Low frequency mode. 0->Normal 1->Low.

Osc2-Vol=0

Filter-Cutoff=6000 // Filter Cutoff in Hz. 20hz lower limit. Fs/2 upper limit.

Filter-Res=0   // Filter Resonance. 0<->1

Filter-EnvAmt=0 // Filter Envelope Amount. 0<->1

Filter-KbdTrk=0   // Filter Keyboard tracking. 0<->1

Lfo-Freq=5.0  // Lfo Frequency in Hz. 0.01hz lower limit. 30hz upper limit.

Lfo-Wave=2 // Lfo waveform /  0=Saw 1=Sq 2=Tri 3=SawSquare 4=SawTri 5=SquareTri 6=SawSquareTri

Lfo-Mix=1.0 // Lfo Noise Mix. 0<->1. / 0->Noise 1->Lfo

Lfo-Filter=0 // Lfo to filter cutoff modulation.  0->off 1->on.

Lfo-Fm1=1 // Lfo to Osc1 pitch modulation.  0->off 1->on.

Lfo-Fm2=1 // Lfo to Osc2 pitch modulation.  0->off 1->on.

Lfo-Pwm1=0  // Lfo to Osc1 pulse width modulation.  0->off 1->on.

Lfo-Pwm2=0  // Lfo to Osc2 pulse width modulation.  0->off 1->on.

Polymod-FilEnv=0  // Polymod Filter envelope amount. 0<->1

Polymod-Osc2=0  // Polymod Osc2 output amount. 0<->1

Polymod-Pw=0  // Polymod to Osc1 pulse width modulation.  0->off 1->on.

Polymod-Filter=0  // Polymod to Filter cutoff modulation. 0->off 1->on.

Polymod-Freq=0  // Polymod to Osc1 pitch modulation.  0->off 1->on.

VolEnv-A=15  // Volume envelope attack in ms. 1.5ms < attack time < 7400ms

VolEnv-D=100 // Volume envelope decay in ms. 0.5ms < decay time < 11000ms

VolEnv-S=1  // Volume envelope sustain. 0<->1

VolEnv-R=100 // Volume envelope release in ms. 0.5ms < release time < 11000ms

FilEnv-A=150 // Filter envelope attack in ms. 1.5ms < attack time < 7400ms

FilEnv-D=150 // Filter envelope decay in ms. 0.5ms < decay time < 11000ms

FilEnv-S=1 // Filter envelope sustain. 0<->1

FilEnv-R=150 // Filter envelope release in ms. 0.5ms < release time < 11000ms

NoiseVol=0 // White noise output. 0<->1

Modwheel=0 // Lfo modulation output amount. 0<->1

Unison=0  // Unison mode. 0->off 1->on.

UnisonSpread=0  // Unison spread voices.  0<->1

UnisonVoices=1 // Unison max voices.  1<->5 (zero is mono mode)

GlideTime=0 // Portamento time.  0<->1

Gain=1.0 // Patch Gain.  0<->2
