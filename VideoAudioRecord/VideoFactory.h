#ifndef __CAM_VIDEOFACTORY__
#define __CAM_VIDEOFACTORY__
#include <string>
#include <map>
#include <iostream>
using namespace std;


// 声明具有动态创建的基类
#define DECLEAR_DYNCRT_BASE(CBase) \
public: \
	typedef CBase *(*ClassGen)(void* parameter); /* 声明函数指针*/ \
	static CBase *Create(const string &class_name, void* parameter=NULL) /* 工厂函数 */ \
	{ \
		std::map<string, ClassGen>::iterator iter = m_class_set.find(class_name); \
		if (m_class_set.end() != iter) \
		{ \
			return ((*iter).second(parameter)); \
		} \
		return NULL; \
	} \
protected: \
	static void Register(const string &class_name, ClassGen class_gen) /* 注册函数 */ \
	{ \
		m_class_set.insert(map<string, ClassGen>::value_type(class_name, class_gen)); \
	} \
	static std::map<string, ClassGen> m_class_set; /* 存储子类信息 */


// 用于实现基类
#define IMPLEMENT_DYNCRT_BASE(CBase)  \
	std::map<string, CBase::ClassGen> CBase::m_class_set;

// 用于声明一个能够被动态创建的类(用一个全局对象进行注册)
#define DECLEAR_DYNCRT_CLASS(CDerived, CBase) \
public: \
struct CDerived##Register /* 辅助类，用于注册 */ \
	{ \
	CDerived##Register() \
		{ \
		static bool bRegistered = false; /* 注册子类，虽然map能保证唯一，但仍只注册一次 */ \
		if(!bRegistered) \
			{ \
			CBase::Register(#CDerived, CDerived::Create); /* 注册子类信息 */ \
			bRegistered = true; \
			} \
		} \
	} ; \
	static CBase *Create(void* parameter) /* 工厂函数 */ \
	{ \
		CBase * ret= new CDerived(); \
		ret->setParameter(parameter); \
		return ret; \
	} \
	static struct CDerived##Register m_t##CDerived##Register;


// 用于实现一个能被动态创建的类
#define IMPLEMENT_DYNCRT_CLASS(CDerived) \
	static CDerived::CDerived##Register m_t##CDerived##Register;



class VideoSource
{
	DECLEAR_DYNCRT_BASE(VideoSource)
	DECLEAR_DYNCRT_CLASS(VideoSource, VideoSource)
public:
	VideoSource();
	virtual void setParameter(void* parameter);
	virtual char* queryFrame();
	virtual std::string getDeviceName();
	virtual int width();
	virtual int height();
	virtual void Print();
	virtual bool isOpen();
	virtual char* decode();
	virtual int info(char* dst);
protected:
	int m_nWidth;
	int m_nHeight;
	bool m_isOpen;
	char* m_imgData;
};

#define DECLEAR_DYNCRT_CLASS_EX(CDerived) DECLEAR_DYNCRT_CLASS(CDerived, VideoSource)

class Sender
{
public:
	Sender(){}
	virtual int sendData(char* header, int headerLen, char* data, int dataLen){return 0;};
protected:
	char* data2send;
};


#endif