#include "VoiceBase.h"
#include  "Mmsystem.h"

CVoiceBase::CVoiceBase()
{
	PCMfmt.wFormatTag=WAVE_FORMAT_PCM;
	PCMfmt.nChannels=2;
	PCMfmt.nSamplesPerSec=SPS_44K;
	PCMfmt.wBitsPerSample=16;
	PCMfmt.nBlockAlign=(PCMfmt.wBitsPerSample * PCMfmt.nChannels) >> 3;
	PCMfmt.nAvgBytesPerSec= PCMfmt.nBlockAlign*PCMfmt.nSamplesPerSec;
	pWaveHeader = NULL;
}

CVoiceBase::~CVoiceBase()
{
	DestroyHeader();
}
BOOL CVoiceBase::CreateHeader(int num, int size)
{
	DestroyHeader();
	if (num>=0 && size>=0)
		pWaveHeader = new WAVEHDR[num];
	else
		return FALSE;
	
	headerNum = num;
	bufferSize = size;
	for(int i=0; i<headerNum; i++)
	{
		pWaveHeader[i].lpData=new char[size];
		pWaveHeader[i].dwBufferLength=size;
	//	pWaveHeader[i].dwBytesRecorded=0;
		pWaveHeader[i].dwUser=NULL;
		pWaveHeader[i].dwFlags=0;
		pWaveHeader[i].reserved=NULL;
		pWaveHeader[i].lpNext=NULL;
		pWaveHeader[i].dwLoops = 0;
	}
	
	return TRUE;
}

void CVoiceBase::DestroyHeader()
{
	if (pWaveHeader!=NULL)
	{
		for (int i=0; i<headerNum; i++)
		{
			if(pWaveHeader[i].lpData != NULL)
				delete  [] (pWaveHeader[i].lpData);
			pWaveHeader[i].lpData = NULL;
		}
		delete []pWaveHeader;
		pWaveHeader = NULL;
	}
	headerNum = 0;
	bufferSize = 0;
}

void  CVoiceBase::GetMMResult(MMRESULT res)
{
	switch (res)
	{
	case MMSYSERR_ALLOCATED: 
		m_result="Specified resource is already allocated.";
		break;
		
	case MMSYSERR_BADDEVICEID:
		m_result="Specified device identifier is out of range.";
		break;
		
	case MMSYSERR_NODRIVER:
		m_result="No device driver is present. ";
		break;
		
	case MMSYSERR_NOMEM:
		m_result="Unable to allocate or lock memory. ";
		break;
		
	case WAVERR_BADFORMAT:
		m_result="Attempted to open with an unsupported waveform-audio format.";
		break;
		
	case WAVERR_UNPREPARED:
		m_result="The buffer pointed to by the pwh parameter hasn't been prepared. ";
		break;
		
	case WAVERR_SYNC:
		m_result="The device is synchronous but waveOutOpen was called"
			"without using the WAVE_ALLOWSYNC flag. ";
		break;
		
	case WAVERR_STILLPLAYING:
		m_result="The buffer pointed to by the pwh parameter is still in the queue.";
		break;
		
	case MMSYSERR_NOTSUPPORTED:
		m_result="Specified device is synchronous and does not support pausing. ";
		break;
		
	case MMSYSERR_NOERROR:
		break;
		
	default:
		m_result="Unspecified error";
	}
}

string CVoiceBase::GetLastError()
{
	return m_result;
}

void CVoiceBase::SetFormat( DWORD nSamplesPerSec,  WORD  wBitsPerSample, WORD  nChannels)
{
   	PCMfmt.cbSize=0;
	PCMfmt.wFormatTag=WAVE_FORMAT_PCM;
	PCMfmt.nChannels=nChannels;
	PCMfmt.nSamplesPerSec=nSamplesPerSec;
	PCMfmt.wBitsPerSample=wBitsPerSample;
	PCMfmt.nBlockAlign=(PCMfmt.wBitsPerSample * PCMfmt.nChannels) >> 3;
	PCMfmt.nAvgBytesPerSec= PCMfmt.nBlockAlign*PCMfmt.nSamplesPerSec;
}
