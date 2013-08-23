#include "VoiceRecording.h"

CVoiceRecording::CVoiceRecording()
{
	hWaveIn=NULL;	
	pCurrentRecordedHeader = NULL;
}

CVoiceRecording::~CVoiceRecording()
{
	if (IsOpen())
		Close();
}

BOOL CVoiceRecording::Record()
{
	for (int i=0; i<headerNum; i++)
	{
		res=waveInPrepareHeader(hWaveIn,&pWaveHeader[i],sizeof(WAVEHDR));
		GetMMResult(res);
		if (res!=MMSYSERR_NOERROR)
			return FALSE;


		res=waveInAddBuffer(hWaveIn,&pWaveHeader[i],sizeof(WAVEHDR));
		GetMMResult(res);
		if (res!=MMSYSERR_NOERROR)
			return FALSE;
			
		res=waveInStart(hWaveIn) ;
		GetMMResult(res);
		if (res!=MMSYSERR_NOERROR)
			return FALSE;
		else
			return TRUE;
	}
}

BOOL CVoiceRecording::Open()
{
	if (IsOpen())
		return FALSE;
	
	res=waveInOpen(&hWaveIn, (UINT) WAVE_MAPPER, &PCMfmt, (DWORD) VoiceWaveInProc, (DWORD) this, CALLBACK_FUNCTION);
	GetMMResult(res);
	if (res!=MMSYSERR_NOERROR)
	{
		hWaveIn=NULL;
		return FALSE;
	}
	else
		return TRUE;
}

BOOL CVoiceRecording::Close()
{
	//res=waveInReset(hWaveIn);
	//GetMMResult(res);
	//if (res!=MMSYSERR_NOERROR)
	//	return FALSE;
		
	res=waveInClose (hWaveIn);
	GetMMResult(res);
	if (res!=MMSYSERR_NOERROR)
		return FALSE;
	else
		return TRUE;
}

BOOL CVoiceRecording::IsOpen()
{
	if (hWaveIn!=NULL)
		return TRUE;
	else
		return FALSE;
}

BOOL CALLBACK VoiceWaveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{ 
	CVoiceRecording* pVoice=(CVoiceRecording*) dwInstance;
	LPWAVEHDR pwh = (LPWAVEHDR)dwParam1;
	if (uMsg==WIM_DATA)
	{
		pVoice->pCurrentRecordedHeader = pwh;
		pVoice->RecordFinished();
		pVoice->IsDataIncomming = true;
	}
	
	if(uMsg == WIM_CLOSE )
	{
		pVoice->DestroyHeader();
		pVoice->IsDataIncomming = false;
	}

	return TRUE;
}


void CVoiceRecording::RecordFinished()
{
	
	//write your handler here

	//or create your own classes that derived from this class
	//and override this virtual function
}
