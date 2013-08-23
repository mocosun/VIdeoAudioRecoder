#ifndef __AUDIORECORD_RECORDING__
#define __AUDIORECORD_RECORDING__

#if _MSC_VER > 100
#pragma once
#endif // _MSC_VER > 100

#include "VoiceBase.h"
#include "VoicePlaying.h"
BOOL CALLBACK VoiceWaveInProc(
							  HWAVEIN hwi,       
							  UINT uMsg,         
							  DWORD dwInstance,  
							  DWORD dwParam1,    
							  DWORD dwParam2     
							  );


class CVoiceRecording : public CVoiceBase  
{
public:
	virtual void RecordFinished();
	BOOL IsOpen();
	BOOL Close();
	BOOL Open();
	BOOL Record();
	HWAVEIN hWaveIn;
	CVoiceRecording();
	virtual ~CVoiceRecording();
public:
	WAVEHDR *pCurrentRecordedHeader;
	bool IsDataIncomming;
};

#endif 
