#include "VideoSource_DS.h"
#include "VoiceRecording.h"
#include "VoicePlaying.h"
#include "media.h"
#include <stdio.h>

DWORD WINAPI VideoRecordThreadFunc(LPVOID lp);
DWORD WINAPI AudioRecordThreadFunc(LPVOID lp);

class videoRecorder
{
public:
	videoRecorder()
	{
		setRecordTime();
	}	

	void setRecordTime(int width=640, int height=320, int video_bitrate = 15000, int nSamplesPerSec=44100, int nBitPerSample=16)
	{
		m_videoWidth = width;
		m_videoHeight = height;
		m_wBitsPerSample = 16;
		m_nSamplesPerSec = nSamplesPerSec;
		m_VideoBitrate = video_bitrate;
	}

	bool startRecording(string fileName)
	{
		m_media.setAudioForamt(8000, m_nSamplesPerSec);
		m_media.setVideoFormat(m_videoWidth, m_videoHeight, m_VideoBitrate);

		if(0 != m_media.openFile(fileName))
		{
			printf( "open file fail!\n");
			return false;
		}

		if(!m_videosource.OpenCamera(0,false, 640, 480))
		{
			printf( "Open camera failed!\n");
			return false;
		}

		m_Record.SetFormat(m_nSamplesPerSec, 16, 1);
		m_Record.CreateHeader(8, 2048);   
		m_Record.Open();

		m_Play.SetFormat(m_nSamplesPerSec, 16, 1);
		m_Play.CreateHeader(8, 2048);    
		m_Play.Open(); 

		if (!(m_Play.IsOpen() && m_Record.IsOpen()))
		{
			printf("Open voice device failed!");
			return false;
		}

		isRunning = true;
		DWORD dwThreadId;
		CreateThread(NULL, 0, VideoRecordThreadFunc, (LPVOID*)this, 0, &dwThreadId);
		CreateThread(NULL, 0, AudioRecordThreadFunc, (LPVOID*)this, 0, &dwThreadId);
		return true;
	}

	void endRecording()
	{
		isRunning = false;
		while (!isAudioThreadEnd || !isVideoThreadEnd)
			Sleep(10);
		m_media.closeFile();
		m_videosource.CloseCamera();
		m_Play.Close();
		m_Record.Close();
		
	}


	media m_media;
	CVoiceRecording m_Record;
	CVoicePlaying m_Play;
	VideoSource_DS m_videosource;
	bool isRunning;
	bool isVideoThreadEnd;
	bool isAudioThreadEnd;
	time_t m_numOfSeconds;


	int m_videoWidth;
	int m_videoHeight;
	int m_VideoBitrate;
	int m_nSamplesPerSec;
	int m_wBitsPerSample;
protected:

};

void WINAPI onVideoRecordTimeFunc(UINT wTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2)
{
	videoRecorder *p = (videoRecorder*)dwUser;
	
	char *data = p->m_videosource.queryFrame();
 
	if (data!=NULL)
	{
		p->m_media.addPicture((unsigned char*)data);
	}
}

DWORD WINAPI VideoRecordThreadFunc(LPVOID lp)
{

	int ms_frame = 0;
	int frameCount = 0;

	videoRecorder* pRecorder = (videoRecorder*)lp;
	pRecorder->isVideoThreadEnd = false;

	time_t time_limit = pRecorder->m_numOfSeconds;
	
	MMRESULT timer_id;
	int n = 0;
	timer_id = timeSetEvent(40, 1, (LPTIMECALLBACK)onVideoRecordTimeFunc, DWORD(pRecorder), TIME_PERIODIC);
	if(NULL == timer_id)
	{
		printf("timeSetEvent() failed with error %d\n", GetLastError());
		return -1;
	}

	while (pRecorder->isRunning)
	{
		Sleep(10);
	}

	timeKillEvent(timer_id);        //ÊÍ·Å¶¨Ê±Æ÷
	
	pRecorder->isVideoThreadEnd = true;

	return 0;
}

DWORD WINAPI AudioRecordThreadFunc(LPVOID lp)
{
	videoRecorder* pRecorder = (videoRecorder*)lp;
	media *m = &(pRecorder->m_media);
	CVoicePlaying* play = &(pRecorder->m_Play);
	CVoiceRecording* record = &(pRecorder->m_Record); 
	pRecorder->isAudioThreadEnd = false;


	if (play->IsOpen())
	{
		play->Play();
	}

	if (record->IsOpen())
	{
		record->Record();
	}


	while (pRecorder->isRunning/* && (currentTime-startTime) < time_limit*/)
	{
		for (int i = 0; i < play->headerNum; i++)
		{
			if ((play->pWaveHeader[i].dwFlags & WHDR_INQUEUE) == 0)
			{
				try
				{
					if(record->pCurrentRecordedHeader !=NULL)
					{
						waveOutPrepareHeader (play->hWaveOut, &play->pWaveHeader[i], sizeof(WAVEHDR));
						memcpy(play->pWaveHeader[i].lpData,record->pCurrentRecordedHeader->lpData,  record->bufferSize);

						m->addAudio((unsigned char*)record->pCurrentRecordedHeader->lpData, record->bufferSize);

						waveOutWrite(play->hWaveOut, &play->pWaveHeader[i], sizeof(WAVEHDR));
						break;
					}
				}
				catch (exception ex)
				{
					printf(ex.what());
				}
			}
		}

		for (int i = 0; i < record->headerNum; i++)
		{
			if (( record->pWaveHeader[i].dwFlags & WHDR_INQUEUE) == 0)
			{
				waveInPrepareHeader( record->hWaveIn, &record->pWaveHeader[i], sizeof(WAVEHDR));
				waveInAddBuffer( record->hWaveIn, &record->pWaveHeader[i], sizeof(WAVEHDR));
			}
		}

		Sleep(10);
	}

	pRecorder->isAudioThreadEnd = true;
	return 0;
}


int main()
{
	videoRecorder r;
	r.setRecordTime(640, 480, 10000, 44100);
	r.startRecording("new2.MP4");
	Sleep(20*1000);
	r.endRecording();
	return 0;
}