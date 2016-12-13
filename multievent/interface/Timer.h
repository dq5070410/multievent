/**
* ��ʱ��
* 
*
* History
*	2/1/2013		Created
*/

#ifndef __TIME_H_
#define __TIME_H_

#include "HikBase.h"

NAME_SPACE_BEGIN

class ITimerSink;
class CTimeValue;

/**
* ��ʱ��
*/
class OS_EXPORT CTimer
{
public:
	CTimer();

	~CTimer();

public:
	/**
	* ����һ��timer
	* 
	* ����㷴����scheduleͬһ��ʱ�ӣ�ֻ�����һ�ε�schedule�������õ�
	*
	* <pSink>��ʱ�ӻص����󣬱���Ҫ��
	* <htvInterval>��������೤ʱ����һ�Σ��������ٴ�ȡ����<iLoopTime>
	* <htvDelay>���ӳ٣������ʱ��������೤ʱ�䷢��
	* <iLoopTime>��һ���������ٴΣ�ע�⣬���ӳ١���һ���ǲ��������ڵģ��������
	*				���timer�������ϵķ�������ô-1��������Ҫ�����ֵ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result Schedule(
		ITimerSink* pSink,
		const CTimeValue& htvInterval,
		const CTimeValue& htvDelay,
		int iLoopTime );

	/**
	* ����һ��timer
	*
	* ����㷴����scheduleͬһ��ʱ�ӣ�ֻ�����һ�ε�schedule�������õ�
	*
	* ͨ���������������timerִ����ֻ��ִ��һ�Σ����磬��ͨ���������
	* ������timer���趨<htvInterval>Ϊ500ms����ô��ֻ����500ms����
	* һ�Σ�֮���û��
	*
	* <pSink>��ʱ�ӻص����󣬱���Ҫ��
	* <htvInterval>��������೤ʱ����һ��
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result Schedule(
		ITimerSink* pSink,
		const CTimeValue& htvInterval );

	/**
	* ����һ��timer
	*
	* ����㷴����scheduleͬһ��ʱ�ӣ�ֻ�����һ�ε�schedule�������õ�
	*
	* ͨ���������������timer��ִ��<iLoopTime>�Σ����磬��ͨ���������
	* ������timer���趨<htvInterval>Ϊ500ms����ô��ֻ����500ms����
	*
	* <pSink>��ʱ�ӻص����󣬱���Ҫ��
	* <htvInterval>��������೤ʱ����һ�Σ��������ٴ�ȡ����<iLoopTime>
	* <iLoopTime>��һ���������ٴΣ�ע�⣬���ӳ١���һ���ǲ��������ڵģ��������
	*				���timer�������ϵķ�������ô-1��������Ҫ�����ֵ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�������Ϊʧ��
	*/
	ME_Result Schedule(
		ITimerSink* pSink,
		const CTimeValue& htvInterval,
		int iLoopTime );

	/**
	* ȡ����ǰ��timer
	*
	* ����ֵ:
	*		��
	*/
	void Cancel();
};

/**
* ��ʱ���ص�����
*
* �����Ҫ����schedule��֮��������Ӧ���ͱ���̳������
*/
class ITimerSink
{
public:
	virtual ~ITimerSink() {}

public:
	/**
	* ��schedule��ʱ�ӷ�����
	*
	* <pTimerID>���ĸ�ʱ�ӷ�����
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnTimer( CHikTimer* pTimerID ) = 0;
};


ME_NAME_SPACE_END

#endif	// __ME_TIME_H_

















