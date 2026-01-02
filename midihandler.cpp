
#include <circle/logger.h>
#include "midihandler.h"
#include "lutsynth.h"

static const char FromMidiHandler[] = "MidiHandler";

#define VOL_ATTACK  1
#define VOL_DECAY	2
#define VOL_SUSTAIN 3
#define VOL_RELEASE 4
#define FIL_ATTACK  5
#define FIL_DECAY	6
#define FIL_SUSTAIN 7
#define FIL_RELEASE 8

#define LED_HI_TIME 50

#define EXP_KNOB_1 4.0236220472441f // 0-127 -> 0.0f ... 1.0f range

static const float ExpLUT[512] = {
    0.00000000,    0.00000001,    0.00000006,    0.00000020,
    0.00000048,    0.00000094,    0.00000162,    0.00000257,
    0.00000384,    0.00000546,    0.00000749,    0.00000998,
    0.00001295,    0.00001647,    0.00002056,    0.00002529,
    0.00003070,    0.00003682,    0.00004371,    0.00005140,
    0.00005996,    0.00006941,    0.00007980,    0.00009118,
    0.00010360,    0.00011710,    0.00013172,    0.00014751,
    0.00016452,    0.00018278,    0.00020235,    0.00022327,
    0.00024558,    0.00026933,    0.00029456,    0.00032132,
    0.00034966,    0.00037961,    0.00041123,    0.00044456,
    0.00047964,    0.00051652,    0.00055525,    0.00059586,
    0.00063840,    0.00068293,    0.00072948,    0.00077809,
    0.00082882,    0.00088171,    0.00093680,    0.00099414,
    0.00105377,    0.00111574,    0.00118010,    0.00124688,
    0.00131614,    0.00138791,    0.00146225,    0.00153919,
    0.00161879,    0.00170109,    0.00178613,    0.00187395,
    0.00196461,    0.00205815,    0.00215461,    0.00225404,
    0.00235648,    0.00246198,    0.00257058,    0.00268233,
    0.00279727,    0.00291545,    0.00303691,    0.00316170,
    0.00328986,    0.00342144,    0.00355649,    0.00369504,
    0.00383714,    0.00398284,    0.00413218,    0.00428520,
    0.00444197,    0.00460250,    0.00476686,    0.00493509,
    0.00510723,    0.00528332,    0.00546342,    0.00564757,
    0.00583581,    0.00602818,    0.00622473,    0.00642552,
    0.00663057,    0.00683994,    0.00705368,    0.00727182,
    0.00749441,    0.00772150,    0.00795313,    0.00818934,
    0.00843019,    0.00867571,    0.00892596,    0.00918097,
    0.00944079,    0.00970547,    0.00997506,    0.01024958,
    0.01052910,    0.01081366,    0.01110329,    0.01139806,
    0.01169799,    0.01200314,    0.01231355,    0.01262927,
    0.01295034,    0.01327680,    0.01360870,    0.01394610,
    0.01428902,    0.01463751,    0.01499163,    0.01535142,
    0.01571691,    0.01608816,    0.01646521,    0.01684811,
    0.01723690,    0.01763162,    0.01803232,    0.01843905,
    0.01885185,    0.01927077,    0.01969584,    0.02012712,
    0.02056465,    0.02100848,    0.02145865,    0.02191520,
    0.02237818,    0.02284764,    0.02332362,    0.02380616,
    0.02429531,    0.02479112,    0.02529363,    0.02580288,
    0.02631892,    0.02684180,    0.02737155,    0.02790824,
    0.02845189,    0.02900255,    0.02956028,    0.03012511,
    0.03069709,    0.03127627,    0.03186268,    0.03245638,
    0.03305741,    0.03366581,    0.03428164,    0.03490493,
    0.03553572,    0.03617407,    0.03682002,    0.03747362,
    0.03813490,    0.03880392,    0.03948072,    0.04016534,
    0.04085783,    0.04155824,    0.04226660,    0.04298297,
    0.04370738,    0.04443990,    0.04518054,    0.04592938,
    0.04668644,    0.04745178,    0.04822543,    0.04900745,
    0.04979788,    0.05059676,    0.05140414,    0.05222006,
    0.05304458,    0.05387772,    0.05471955,    0.05557009,
    0.05642941,    0.05729754,    0.05817453,    0.05906042,
    0.05995526,    0.06085909,    0.06177196,    0.06269392,
    0.06362500,    0.06456526,    0.06551473,    0.06647347,
    0.06744151,    0.06841891,    0.06940571,    0.07040195,
    0.07140767,    0.07242293,    0.07344777,    0.07448223,
    0.07552636,    0.07658020,    0.07764380,    0.07871720,
    0.07980045,    0.08089359,    0.08199667,    0.08310973,
    0.08423282,    0.08536598,    0.08650926,    0.08766271,
    0.08882635,    0.09000026,    0.09118446,    0.09237900,
    0.09358392,    0.09479929,    0.09602512,    0.09726148,
    0.09850841,    0.09976595,    0.10103415,    0.10231304,
    0.10360269,    0.10490313,    0.10621440,    0.10753656,
    0.10886964,    0.11021369,    0.11156876,    0.11293490,
    0.11431214,    0.11570053,    0.11710012,    0.11851095,
    0.11993306,    0.12136651,    0.12281133,    0.12426758,
    0.12573529,    0.12721452,    0.12870530,    0.13020768,
    0.13172170,    0.13324742,    0.13478488,    0.13633412,
    0.13789518,    0.13946811,    0.14105296,    0.14264977,
    0.14425859,    0.14587946,    0.14751242,    0.14915753,
    0.15081482,    0.15248434,    0.15416614,    0.15586025,
    0.15756674,    0.15928563,    0.16101698,    0.16276084,
    0.16451723,    0.16628622,    0.16806784,    0.16986215,
    0.17166917,    0.17348897,    0.17532159,    0.17716706,
    0.17902545,    0.18089678,    0.18278110,    0.18467847,
    0.18658892,    0.18851250,    0.19044926,    0.19239924,
    0.19436248,    0.19633903,    0.19832894,    0.20033225,
    0.20234900,    0.20437924,    0.20642302,    0.20848038,
    0.21055136,    0.21263601,    0.21473438,    0.21684650,
    0.21897244,    0.22111222,    0.22326589,    0.22543351,
    0.22761511,    0.22981074,    0.23202044,    0.23424426,
    0.23648225,    0.23873445,    0.24100090,    0.24328165,
    0.24557674,    0.24788623,    0.25021015,    0.25254854,
    0.25490146,    0.25726896,    0.25965106,    0.26204782,
    0.26445929,    0.26688551,    0.26932652,    0.27178237,
    0.27425310,    0.27673877,    0.27923940,    0.28175506,
    0.28428578,    0.28683160,    0.28939259,    0.29196877,
    0.29456019,    0.29716690,    0.29978895,    0.30242637,
    0.30507922,    0.30774754,    0.31043137,    0.31313075,
    0.31584575,    0.31857639,    0.32132272,    0.32408479,
    0.32686264,    0.32965633,    0.33246588,    0.33529136,
    0.33813279,    0.34099024,    0.34386374,    0.34675333,
    0.34965907,    0.35258100,    0.35551916,    0.35847360,
    0.36144436,    0.36443149,    0.36743503,    0.37045503,
    0.37349153,    0.37654458,    0.37961422,    0.38270050,
    0.38580346,    0.38892315,    0.39205961,    0.39521289,
    0.39838303,    0.40157008,    0.40477408,    0.40799508,
    0.41123312,    0.41448825,    0.41776051,    0.42104995,
    0.42435661,    0.42768054,    0.43102178,    0.43438037,
    0.43775637,    0.44114982,    0.44456075,    0.44798923,
    0.45143528,    0.45489897,    0.45838032,    0.46187939,
    0.46539623,    0.46893087,    0.47248336,    0.47605375,
    0.47964208,    0.48324839,    0.48687274,    0.49051516,
    0.49417571,    0.49785442,    0.50155135,    0.50526653,
    0.50900001,    0.51275184,    0.51652205,    0.52031071,
    0.52411785,    0.52794351,    0.53178774,    0.53565059,
    0.53953210,    0.54343232,    0.54735129,    0.55128905,
    0.55524566,    0.55922115,    0.56321557,    0.56722897,
    0.57126139,    0.57531287,    0.57938347,    0.58347322,
    0.58758217,    0.59171036,    0.59585785,    0.60002467,
    0.60421088,    0.60841651,    0.61264161,    0.61688622,
    0.62115040,    0.62543418,    0.62973761,    0.63406074,
    0.63840360,    0.64276625,    0.64714874,    0.65155109,
    0.65597337,    0.66041561,    0.66487787,    0.66936017,
    0.67386258,    0.67838513,    0.68292788,    0.68749085,
    0.69207411,    0.69667769,    0.70130165,    0.70594601,
    0.71061084,    0.71529617,    0.72000205,    0.72472853,
    0.72947564,    0.73424344,    0.73903197,    0.74384127,
    0.74867140,    0.75352238,    0.75839428,    0.76328713,
    0.76820098,    0.77313588,    0.77809186,    0.78306898,
    0.78806728,    0.79308680,    0.79812759,    0.80318970,
    0.80827316,    0.81337803,    0.81850435,    0.82365216,
    0.82882151,    0.83401244,    0.83922500,    0.84445924,
    0.84971519,    0.85499291,    0.86029244,    0.86561382,
    0.87095710,    0.87632233,    0.88170954,    0.88711879,
    0.89255011,    0.89800356,    0.90347917,    0.90897700,
    0.91449709,    0.92003948,    0.92560422,    0.93119136,
    0.93680093,    0.94243298,    0.94808757,    0.95376472,
    0.95946450,    0.96518693,    0.97093208,    0.97669998,
    0.98249067,    0.98830421,    0.99414064,    1.00000000,
};



CMidiHandler::CMidiHandler(CInterruptSystem *pInterrupt)
:   m_pSerialMidi(nullptr),
    m_nLEDTicks(0),
    m_pVoices(nullptr)
{
    m_pSerialMidi = new CSerialMidi(pInterrupt, this);
}

CMidiHandler::~CMidiHandler(void)
{
}



bool CMidiHandler::Initialize(unsigned nBaudRate)
{
    return m_pSerialMidi->Initialize(nBaudRate);
}

void CMidiHandler::Process(void)
{
    if (m_nLEDTicks > 0)
    {
		m_nLEDTicks--;
		if (m_nLEDTicks == 0)
            m_ActLED.Off();
    }

    if (m_pSerialMidi)
        m_pSerialMidi->Process();
}

void CMidiHandler::HandleMidiMessage(u8 *pMessage, unsigned nLength)
{
    m_ActLED.On();
    m_nLEDTicks = LED_HI_TIME;

    if (!m_pVoices)
        return;

    u8 status = pMessage[0];
    u8 type   = status >> 4;
    u8 key    = pMessage[1];
    u8 vel    = pMessage[2];

    if (type == MIDI_NOTE_ON && vel > 0)
        m_pVoices->NoteOn(key, vel);
    else if (type == MIDI_NOTE_OFF || (type == MIDI_NOTE_ON && vel == 0))
        m_pVoices->NoteOff(key);
    else if (type == MIDI_CC)
    {
		//////////////////////////
		//	Osc parameter   	//
		//////////////////////////
		if (pMessage[1] == CC_OSC_1_COARSE)
		{
			float c = (float)pMessage[2]/127.0f * 24.0f - 12.0f; // -12 ... +12
			m_pVoices->SetCoarse(0, c);
		}
		if (pMessage[1] == CC_OSC_1_FINE)
		{
			float f = (float)pMessage[2]/127.0f; // 0.0f->-100ct // 1.0f->+100ct
			m_pVoices->SetFine(0, f);
		}	
		if (pMessage[1] == CC_OSC_1_SAW)
		{
			bool saw = pMessage[2];
			m_pVoices->SetWaveformSaw(0, saw);
		}
		if (pMessage[1] == CC_OSC_1_SQUARE)
		{
			bool sq = pMessage[2];
			m_pVoices->SetWaveformSquare(0, sq);
		}
		if (pMessage[1] == CC_OSC_1_PW)
		{
			float pw = (float)pMessage[2]/158.0f + 0.1f; // 0.1f ... 0.9f
			m_pVoices->SetPulseWidth(0, pw);
		}	
		if (pMessage[1] == CC_OSC_1_SYNC) // 0 = Off / 1 = On
		{
			bool sync = pMessage[2];
			m_pVoices->SetOsc1Sync(sync);
		}
		if (pMessage[1] == CC_OSC_1_VOL)
		{
			float v = (float)pMessage[2]/127.0f; // 0.0f ... 1.0f
			m_pVoices->SetOscVol(0, v);
		}	
		if (pMessage[1] == CC_OSC_2_KBD) // 0 = No pitch kbd trk / 1 = On
		{
			bool kbd = pMessage[2];
			m_pVoices->SetOsc2KbdTrk(kbd);
		}
		if (pMessage[1] == CC_OSC_2_LOW) // 0 = Normal pitch / 1 = Low Freq
		{
			bool low = pMessage[2];
			m_pVoices->SetOsc2LowFreq(low);
		}
		
		if (pMessage[1] == CC_OSC_2_COARSE)
		{
			float c = (float)pMessage[2]/127.0f * 24.0f - 12.0f;
			m_pVoices->SetCoarse(1, c);
		}
		if (pMessage[1] == CC_OSC_2_FINE)
		{
			float f = (float)pMessage[2]/127.0f;
			m_pVoices->SetFine(1, f);
		}	
		if (pMessage[1] == CC_OSC_2_SAW)
		{
			bool saw = pMessage[2];
			m_pVoices->SetWaveformSaw(1, saw);
		}
		if (pMessage[1] == CC_OSC_2_SQUARE)
		{
			bool sq = pMessage[2];
			m_pVoices->SetWaveformSquare(1, sq);
		}
		if (pMessage[1] == CC_OSC_2_TRI)
		{
			bool tri = pMessage[2];
			m_pVoices->SetWaveformTri(tri);
		}
		if (pMessage[1] == CC_OSC_2_PW)
		{
			float pw = (float)pMessage[2]/158.0f + 0.1f;
			m_pVoices->SetPulseWidth(1, pw);
		}	
		if (pMessage[1] == CC_OSC_2_VOL)
		{
			float v = (float)pMessage[2]/127.0f;
			m_pVoices->SetOscVol(1, v);
		}	


		//////////////////////////
		//	Filter parameter	//
		//////////////////////////
		if (pMessage[1] == CC_CUTOFF)
		{
			unsigned idx = pMessage[2] * EXP_KNOB_1;
			float f = ExpLUT[idx] * NYQUIST_RATE; 
			m_pVoices->SetCutoff(f);
		}
		if (pMessage[1] == CC_RESONANCE) // 0.0f ... 1.0f
		{
			float q = (float)pMessage[2]/127.0f;
			m_pVoices->SetResonance(q);
		}
		if (pMessage[1] == CC_FIL_ENV_AMT) // 0.0f ... 1.0f
		{
			float amt = (float)pMessage[2]/127.0f;
			m_pVoices->SetEnvAmt(amt);
		}
		if (pMessage[1] == CC_FIL_KBDTRK) // 0.0f ... 1.0f
		{
			unsigned idx = pMessage[2] * EXP_KNOB_1;
			float amt = ExpLUT[idx];
			m_pVoices->SetKbdTrk(amt);
		}


		//////////////////////////
		//	lfo parameter   	//
		//////////////////////////
		if (pMessage[1] == CC_LFO_FREQ)
		{
			//float f = (float)pMessage[2]/4.23f + 0.01f; // 0.01f ... 30.0f [Hz]
			unsigned idx = pMessage[2] * EXP_KNOB_1;
			float f = ExpLUT[idx] * 30.0f + 0.01f;
			m_pVoices->SetLfoFreq(f);
		}	
		if (pMessage[1] == CC_LFO_SAW)
		{
			bool saw = pMessage[2];
			m_pVoices->SetLfoWaveSaw(saw);
		}	
		if (pMessage[1] == CC_LFO_SQUARE)
		{
			bool sq = pMessage[2];
			m_pVoices->SetLfoWaveSquare(sq);
		}
		if (pMessage[1] == CC_LFO_TRI)
		{
			bool tri = pMessage[2];
			m_pVoices->SetLfoWaveTri(tri);
		}
		if (pMessage[1] == CC_LFO_FM1)
		{
			bool fm1 = pMessage[2];				// 0 ... 1
			m_pVoices->SetLfoFm(0, fm1);
		}	
		if (pMessage[1] == CC_LFO_FM2)
		{
			bool fm2 = pMessage[2];				// 0 ... 1
			m_pVoices->SetLfoFm(1, fm2);
		}	
		if (pMessage[1] == CC_LFO_PWM1)
		{
			bool pwm1 = pMessage[2];				// 0 ... 1
			m_pVoices->SetPulseWidthModulation(0, pwm1);
		}	
		if (pMessage[1] == CC_LFO_PWM2)
		{
			bool pwm2 = pMessage[2];				// 0 ... 1
			m_pVoices->SetPulseWidthModulation(1, pwm2);
		}	
		if (pMessage[1] == CC_LFO_FILTER)
		{
			bool f = pMessage[2];				// 0 ... 1
			m_pVoices->SetLfoFilter(f);
		}	
		if (pMessage[1] == CC_LFO_MIX)
		{
			float mix = (float)pMessage[2]/127.0f;
			m_pVoices->SetLfoMix(mix);
		}
		
		
				
		//////////////////
		//	POLY MOD	//
		//////////////////
		if (pMessage[1] == CC_POLYMOD_FREQ)
		{
			bool polyFreq = pMessage[2];
			m_pVoices->SetPolyModFreq(polyFreq);
		}
		if (pMessage[1] == CC_POLYMOD_PW)
		{
			bool polyPw = pMessage[2];
			m_pVoices->SetPolyModPw(polyPw);
		}
		if (pMessage[1] == CC_POLYMOD_FIL)
		{
			bool polyFil = pMessage[2];
			m_pVoices->SetPolyModFil(polyFil);
		}
		if (pMessage[1] == CC_POLYMOD_OSC2)
		{
			float polyOsc2 = (float)pMessage[2]/127.0f;
			m_pVoices->SetPolyModOsc2(polyOsc2);
		}
		if (pMessage[1] == CC_POLYMOD_FILENV)
		{
			float polyFilEnv = (float)pMessage[2]/256.0f;
			m_pVoices->SetPolyModFilEnv(polyFilEnv);
		}



		//////////////////
		//	Vol ADSR	//
		//////////////////
		if (pMessage[1] == CC_VOL_ATTACK)
		{
			//float a = (float)pMessage[2]/127.0f * 7400.0f + 1.5f;
			unsigned idx = pMessage[2] * EXP_KNOB_1;
			float a = ExpLUT[idx] * 7400.0f + 1.5f;
			m_pVoices->SetEnv(VOL_ATTACK, a);
		}
		if (pMessage[1] == CC_VOL_DECAY)
		{
			//float d = (float)pMessage[2]/127.0f * 11000.0f + 0.5f;
			unsigned idx = pMessage[2] * EXP_KNOB_1;
			float d = ExpLUT[idx] * 11000.0f + 0.5f;
			m_pVoices->SetEnv(VOL_DECAY, d);
		}
		if (pMessage[1] == CC_VOL_SUSTAIN)
		{
			float s = (float)pMessage[2]/127.0f;
			m_pVoices->SetEnv(VOL_SUSTAIN, s);
		}
		if (pMessage[1] == CC_VOL_RELEASE)
		{
			//float r = (float)pMessage[2]/127.0f * 11000.0f + 0.5f;
			unsigned idx = pMessage[2] * EXP_KNOB_1;
			float r = ExpLUT[idx] * 11000.0f + 0.5f;
			m_pVoices->SetEnv(VOL_RELEASE, r);
		}
		
		//////////////////
		//	Fil ADSR	//
		//////////////////
		if (pMessage[1] == CC_FIL_ATTACK)
		{
			//float a = (float)pMessage[2]/127.0f * 7400.0f + 1.5f;
			unsigned idx = pMessage[2] * EXP_KNOB_1;
			float a = ExpLUT[idx] * 7400.0f + 1.5f;
			m_pVoices->SetEnv(FIL_ATTACK, a);
		}
		if (pMessage[1] == CC_FIL_DECAY)
		{
			//float d = (float)pMessage[2]/127.0f * 11000.0f + 0.5f;
			unsigned idx = pMessage[2] * EXP_KNOB_1;
			float d = ExpLUT[idx] * 11000.0f + 0.5f;
			m_pVoices->SetEnv(FIL_DECAY, d);
		}
		if (pMessage[1] == CC_FIL_SUSTAIN)
		{
			float s = (float)pMessage[2]/127.0f;
			m_pVoices->SetEnv(FIL_SUSTAIN, s);
		}
		if (pMessage[1] == CC_FIL_RELEASE)
		{
			//float r = (float)pMessage[2]/127.0f * 11000.0f + 0.5f;
			unsigned idx = pMessage[2] * EXP_KNOB_1;
			float r = ExpLUT[idx] * 11000.0f + 0.5f;
			m_pVoices->SetEnv(FIL_RELEASE, r);
		}
		
		
		// Noise Parameters //
		if (pMessage[1] == CC_NOISE_VOL)
		{
			float n = (float)pMessage[2]/127.0f;
			m_pVoices->SetNoiseVol(n);
		}
		
		//////////////////////////
		//	General Parameters	//
		//////////////////////////
		if (pMessage[1] == CC_GENERAL_MODWHEEL)
		{
			float mod = (float)pMessage[2]/127.0f;
			m_pVoices->SetModWheel(mod);
		}
		if (pMessage[1] == CC_GENERAL_UNISON)
		{
			bool u = pMessage[2];
			m_pVoices->SetUnison(u);
		}
		if (pMessage[1] == CC_GENERAL_SPREAD)
		{
			unsigned idx = pMessage[2] * EXP_KNOB_1;
			float sp = ExpLUT[idx]; 
			m_pVoices->SetUnisonSpread(sp);
		}
		if (pMessage[1] == CC_GENERAL_GLIDE)
		{
			float time = (float)pMessage[2]/127.0f; // 0.0f ... 1.0f
			m_pVoices->SetGlideTime(time);
		}
		if (pMessage[1] == CC_GENERAL_UNISON_VOICES)
		{
			unsigned v = (pMessage[2]/26) + 1; // 1 ... 5 voices
			m_pVoices->SetUnisonVoices(v);
		}
		if (pMessage[1] == CC_GENERAL_VOLUME) // 0.0f ... 2.0f
		{
			float gain = (float)pMessage[2]/64.0f;
			m_pVoices->SetGain(gain);
		}
	}
	else if (type == MIDI_PROG_CHANGE)
	{
		unsigned p = pMessage[1];
		m_pVoices->SetPatch(p);
	}
	else if (type == MIDI_PITCH_BEND)
	{
		unsigned lsb = pMessage[1];    
		unsigned msb = pMessage[2];   

		// 14-bit pitch bend value (0–16383)
		unsigned bend = (msb << 7) | lsb;
		// Normalize to 0.0–1.0
		float f = bend / 16383.0f;
		
		m_pVoices->SetFine(0, f);
		m_pVoices->SetFine(1, f);
	}
}
