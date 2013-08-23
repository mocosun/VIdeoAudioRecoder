#include "VoicePlaying.h"

CVoicePlaying::CVoicePlaying()
{
	hWaveOut=NULL;
}

CVoicePlaying::~CVoicePlaying()
{
	if (IsOpen())
		Close();
}

BOOL CVoicePlaying::Play()
{
	for (int i=0; i<headerNum; i++)
	{
		res=waveOutPrepareHeader (hWaveOut,&pWaveHeader[i],sizeof(WAVEHDR));
		GetMMResult(res);
		if (res!=MMSYSERR_NOERROR)
			return FALSE;

		res=waveOutWrite(hWaveOut,&pWaveHeader[i],sizeof(WAVEHDR));	
		GetMMResult(res);
		if (res!=MMSYSERR_NOERROR)
			return FALSE;
		else
			return TRUE;
	}

}

BOOL CVoicePlaying::Open()
{
	if (IsOpen())
		return FALSE;
	
	res=waveOutOpen (&hWaveOut,WAVE_MAPPER,&PCMfmt,(DWORD) VoiceWaveOutProc,(DWORD) this, CALLBACK_FUNCTION);
	GetMMResult(res);
	
	if (res!=MMSYSERR_NOERROR)
	{
		hWaveOut=NULL;
		return FALSE;
	}
	else
		return TRUE;
}

BOOL CVoicePlaying::Close()
{
	//res=waveOutReset(hWaveOut);
	//GetMMResult(res);
	//if (res!=MMSYSERR_NOERROR)
	//	return FALSE;
	
	res=waveOutClose(hWaveOut);
	GetMMResult(res);

	if (res!=MMSYSERR_NOERROR)
		return FALSE;
	else
		return TRUE;
}

BOOL CVoicePlaying::IsOpen()
{
  if(hWaveOut!=NULL)
	 return TRUE;
  else
     return FALSE;
	

}

BOOL CALLBACK VoiceWaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	CVoicePlaying* pVoice=(CVoicePlaying*) dwInstance;
	LPWAVEHDR pwh = (LPWAVEHDR)dwParam1;

 	if (uMsg==WIM_CLOSE)
	{
		pVoice->DestroyHeader();
		pVoice->GetMMResult(pVoice->res);
		pVoice->PlayFinished();
		if (pVoice->res!=MMSYSERR_NOERROR)
			return FALSE;
		else
			return TRUE;
	}

	return TRUE;
}


void CVoicePlaying::PlayFinished()
{
	//write your own handler here

	//or simply create your own class and override this virtual function
}
