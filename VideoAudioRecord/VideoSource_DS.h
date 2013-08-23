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

	//打开摄像头，nCamID指定打开哪个摄像头，取值可以为0,1,2,...
	//bDisplayProperties指示是否自动弹出摄像头属性页
	//nWidth和nHeight设置的摄像头的宽和高，如果摄像头不支持所设定的宽度和高度，则返回false
	bool OpenCamera(int nCamID, bool bDisplayProperties = true, int nWidth = 320, int nHeight = 240);

	//关闭摄像头，析构函数会自动调用这个函数
	void CloseCamera();

	//返回摄像头的数目
	//可以不用创建CCameraDS实例，采用int c=CCameraDS::CameraCount();得到结果。
	static int CameraCount(); 

	//根据摄像头的编号返回摄像头的名字
	//nCamID: 摄像头编号
	//sName: 用于存放摄像头名字的数组
	//nBufferSize: sName的大小
	//可以不用创建CCameraDS实例，采用CCameraDS::CameraName();得到结果。
	static int CameraName(int nCamID, char* sName, int nBufferSize);

	////返回图像宽度
	//int width(){return m_nWidth;} 

	////返回图像高度
	//int height(){return m_nHeight;}

	////抓取一帧，返回的IplImage不可手动释放！
	////返回图像数据的为RGB模式的Top-down(第一个字节为左上角像素)，即IplImage::origin=0(IPL_ORIGIN_TL)
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