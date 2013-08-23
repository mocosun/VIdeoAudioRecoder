#include "VideoFactory.h"

void VideoSource::Print()
{
	printf("VideoBase!\n");
}

VideoSource::VideoSource()
{
	m_imgData = NULL;
	m_isOpen = false;
}

//VideoSource::VideoSource(void* parameter)
//{
//	m_imgData = NULL;
//}

void VideoSource::setParameter(void* parameter)
{
	
}


char* VideoSource::queryFrame()
{

	return NULL;	
}
std::string VideoSource::getDeviceName()
{
	return string("");
}
int VideoSource::width()
{
	return m_nWidth;
}
int VideoSource::height()
{
	return m_nHeight;
}


bool VideoSource::isOpen()
{
	return m_isOpen;
}

char* VideoSource::decode()
{
	return m_imgData;
}
int VideoSource::info(char* dst)
{
	return 0;
}

IMPLEMENT_DYNCRT_BASE(VideoSource)
IMPLEMENT_DYNCRT_CLASS(VideoSource)