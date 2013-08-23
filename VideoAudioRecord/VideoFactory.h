#ifndef __CAM_VIDEOFACTORY__
#define __CAM_VIDEOFACTORY__
#include <string>
#include <map>
#include <iostream>
using namespace std;


// �������ж�̬�����Ļ���
#define DECLEAR_DYNCRT_BASE(CBase) \
public: \
	typedef CBase *(*ClassGen)(void* parameter); /* ��������ָ��*/ \
	static CBase *Create(const string &class_name, void* parameter=NULL) /* �������� */ \
	{ \
		std::map<string, ClassGen>::iterator iter = m_class_set.find(class_name); \
		if (m_class_set.end() != iter) \
		{ \
			return ((*iter).second(parameter)); \
		} \
		return NULL; \
	} \
protected: \
	static void Register(const string &class_name, ClassGen class_gen) /* ע�ắ�� */ \
	{ \
		m_class_set.insert(map<string, ClassGen>::value_type(class_name, class_gen)); \
	} \
	static std::map<string, ClassGen> m_class_set; /* �洢������Ϣ */


// ����ʵ�ֻ���
#define IMPLEMENT_DYNCRT_BASE(CBase)  \
	std::map<string, CBase::ClassGen> CBase::m_class_set;

// ��������һ���ܹ�����̬��������(��һ��ȫ�ֶ������ע��)
#define DECLEAR_DYNCRT_CLASS(CDerived, CBase) \
public: \
struct CDerived##Register /* �����࣬����ע�� */ \
	{ \
	CDerived##Register() \
		{ \
		static bool bRegistered = false; /* ע�����࣬��Ȼmap�ܱ�֤Ψһ������ֻע��һ�� */ \
		if(!bRegistered) \
			{ \
			CBase::Register(#CDerived, CDerived::Create); /* ע��������Ϣ */ \
			bRegistered = true; \
			} \
		} \
	} ; \
	static CBase *Create(void* parameter) /* �������� */ \
	{ \
		CBase * ret= new CDerived(); \
		ret->setParameter(parameter); \
		return ret; \
	} \
	static struct CDerived##Register m_t##CDerived##Register;


// ����ʵ��һ���ܱ���̬��������
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