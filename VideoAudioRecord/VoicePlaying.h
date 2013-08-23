#ifndef __AUDIORECORD_VOICEPLAYING__
#define __AUDIORECORD_VOICEPLAYING__

#if _MSC_VER > 100
#pragma once
#endif // _MSC_VER > 100

#include "VoiceBase.h"
BOOL CALLBACK VoiceWaveOutProc(
						  HWAVEOUT hwi,       
						  UINT uMsg,         
						  DWORD dwInstance,  
						  DWORD dwParam1,    
						  DWORD dwParam2     
						  );

class CVoicePlaying : public CVoiceBase  
{
public:
	void PlayFinished();
	BOOL IsOpen();
	BOOL Close();
	BOOL Open();
    BOOL Play();
	HWAVEOUT hWaveOut;
	CVoicePlaying();
	virtual ~CVoicePlaying();
   
};

#endif
