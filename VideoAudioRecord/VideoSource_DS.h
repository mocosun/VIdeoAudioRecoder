#ifndef __CAM_VIDEOFACTORY_VIDEOSOURCE_DS__
#define __CAM_VIDEOFACTORY_VIDEOSOURCE_DS__

#pragma comment(lib, "strmiids.lib")
#include "VideoFactory.h"

#define WIN32_LEAN_AND_MEAN
#include <atlbase.h>
#include "qedit.h"
#include "dshow.h"
#define MYFREEMEDIATYPE(mt)	{if ((mt).cbFormat != 0)		\
					{CoTaskMemFree((PVOID)(mt).pbFormat);	\
					(mt).cbFormat = 0;						\
					(mt).pbFormat = NULL;					\
				}											\
				if ((mt).pUnk != NULL)						\
				{											\
				(mt).pUnk->Release();					\
				(mt).pUnk = NULL;						\
				}}	

class VideoSource_DS : public VideoSource
{
	DECLEAR_DYNCRT_CLASS_EX(VideoSource_DS)
	
public:
	virtual void Print();

protected:

	bool BindFilter(int nCamIDX, IBaseFilter **pFilter);
	void SetCrossBar();

	bool m_bConnected, m_bLock, m_bChanged;

	long m_nBufferSize;

	//IplImage *m_pFrame;

	CComPtr<IGraphBuilder> m_pGraph;

	CComPtr<ISampleGrabber> m_pSampleGrabber;

	CComPtr<IMediaControl> m_pMediaControl;

	CComPtr<IMediaEvent> m_pMediaEvent;

	CComPtr<IBaseFilter> m_pSampleGrabberFilter;
	CComPtr<IBaseFilter> m_pDeviceFilter;
	CComPtr<IBaseFilter> m_pNullFilter;

	CComPtr<IPin> m_pGrabberInput;
	CComPtr<IPin> m_pGrabberOutput;
	CComPtr<IPin> m_pCameraOutput;
	CComPtr<IPin> m_pNullInputPin;


public:
	VideoSource_DS();
	 
	virtual ~VideoSource_DS();

	//������ͷ��nCamIDָ�����ĸ�����ͷ��ȡֵ����Ϊ0,1,2,...
	//bDisplayPropertiesָʾ�Ƿ��Զ���������ͷ����ҳ
	//nWidth��nHeight���õ�����ͷ�Ŀ�͸ߣ��������ͷ��֧�����趨�Ŀ�Ⱥ͸߶ȣ��򷵻�false
	bool OpenCamera(int nCamID, bool bDisplayProperties = true, int nWidth = 320, int nHeight = 240);

	//�ر�����ͷ�������������Զ������������
	void CloseCamera();

	//��������ͷ����Ŀ
	//���Բ��ô���CCameraDSʵ��������int c=CCameraDS::CameraCount();�õ������
	static int CameraCount(); 

	//��������ͷ�ı�ŷ�������ͷ������
	//nCamID: ����ͷ���
	//sName: ���ڴ������ͷ���ֵ�����
	//nBufferSize: sName�Ĵ�С
	//���Բ��ô���CCameraDSʵ��������CCameraDS::CameraName();�õ������
	static int CameraName(int nCamID, char* sName, int nBufferSize);

	////����ͼ����
	//int width(){return m_nWidth;} 

	////����ͼ��߶�
	//int height(){return m_nHeight;}

	////ץȡһ֡�����ص�IplImage�����ֶ��ͷţ�
	////����ͼ�����ݵ�ΪRGBģʽ��Top-down(��һ���ֽ�Ϊ���Ͻ�����)����IplImage::origin=0(IPL_ORIGIN_TL)
	virtual char * queryFrame();
	virtual void setParameter(void* parameter);
};

struct VideoSource_DS_Para
{
	VideoSource_DS_Para():cameraIdx(0),height(240),width(320){}
	int cameraIdx;
	int height;
	int width;
};

#endif