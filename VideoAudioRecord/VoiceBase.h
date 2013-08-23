#ifndef __AUDIORECORD_VOICEBASE__
#define __AUDIORECORD_VOICEBASE__
#include <windows.h>
#include <cstring>
#include <mmsystem.h>
#include <string>
using std::string;

#if _MSC_VER > 100
#pragma once
#endif // _MSC_VER > 100

class CVoiceBase  
{
public:
	
	string m_result;
	MMRESULT res;
	enum 
	{
		SPS_8K=8000,
		SPS_11K=11025,
		SPS_22K=22050,
		SPS_44K=44100
	};

	enum 
	{
		CH_MONO=1,
		CH_STEREO=2
	};

	WAVEHDR* pWaveHeader;
	WAVEFORMATEX PCMfmt;
	int headerNum;
	int bufferSize;

	void SetFormat(DWORD nSamplesPerSec,  WORD  wBitsPerSample,WORD  nChannels);
	string GetLastError();
	void GetMMResult(MMRESULT res);
	void DestroyHeader();
	BOOL CreateHeader(int headerNum, int bufferSize);
	
	CVoiceBase();
	virtual ~CVoiceBase();

};

#endif 
