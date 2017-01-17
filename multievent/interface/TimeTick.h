/**
* tick count�ķ�װ��
*
* History
*	2/05/2016		Created
*/

#ifndef __TIME_TICK_H_
#define __TIME_TICK_H_

#include "Base.h"
#include "TimeValue.h"

NAME_SPACE_BEGIN

typedef struct tagTimeTick
{
	DWORD dwRound;		// ���������ڱ�ʶ�߹��˶�����(ÿ��ΪDWORD�����ֵ)
	DWORD dwTickCount;	// ��ǰ��tickcount
} ME_TIME_TICK;


/**
* tick count�ķ�װ��
*/
class ME_OS_EXPORT CTimeTick
{
public:
	static const DWORD MAX_TICK_COUNT;

public:
	CTimeTick();

	CTimeTick( 
		DWORD dwRound,
		DWORD dwTimeCount );

	CTimeTick( const ME_TIME_TICK& httTick );

	~CTimeTick();

public:
	/**
	* ��ȡϵͳtickcount
	*
	* ����ֵ��
	*		DWORD��Χ������ֵ
	*/
	static DWORD GetSysTickCount();

public:
	/**
	* ����time tick
	*
	* ����ֵ��
	*		��
	*/
	void Set( const ME_TIME_TICK& httTimeTick );

	/**
	* ����time tick
	*
	* <dwRound>������
	* <dwTickCount>��tick count
	*
	* ����ֵ��
	*		��
	*/
	void Set( 
		DWORD dwRound, 
		DWORD dwTickCount );

	/**
	* ��ȡtime tick
	*
	* ����ֵ��
	*		ME_TIME_TICK
	*/
	const ME_TIME_TICK& Get() const;

	DWORD GetRound() const;

	/**
	* ��ȡtick count
	* 
	* ����ֵ��
	*		DWORD��Χ������ֵ
	*/
	DWORD GetTickCount() const;

	/**
	* ����<dwTickCount>���±����ME_TIME_TICK
	*
	* <dwTickCount>����Ҫ���Ӷ���tickcount
	* <bAddRound>�������Ƿ�Ҫ����
	*/
	void Update( 
		DWORD dwTickCount,
		BOOL bAddRound = FALSE );

	const CTimeTick& operator = ( const CMETimeValue& htvTime );

	const CTimeTick& operator = ( const ME_TIME_TICK& httTick );

	BOOL operator < ( const CTimeTick& httDest ) const;

	BOOL operator > ( const CTimeTick& httDest ) const;

private:
	ME_TIME_TICK					m_httTick;
};

NAME_SPACE_END

#endif	// __TIME_TICK_H_









