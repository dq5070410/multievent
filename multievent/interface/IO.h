/**
* IO��װ�ӿ���
* 
* History
*	12/11/2016		Created
*/

#ifndef __IO_H_
#define __IO_H_

#include "Base.h"

NAME_SPACE_BEGIN

/**
* IO��װ�ӿ���
*/
class IIO
{
public:
	virtual ~IIO() {}

public:
	virtual void SetHandle( ME_HANDLE hHandle ) = 0;

	virtual ME_HANDLE GetHandle() const = 0;
};

/**
* IO��װ�ӿڻص���
*/
class IIOSink
{
public:
	virtual ~IIOSink() {}

public:
	/**
	* input�¼�����
	*
	* <hHandle>���ĸ����������input�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnInput(
		ME_HANDLE hHandle,
		int iEvent ) = 0;

	/**
	* output�¼�����
	*
	* <hHandle>���ĸ����������output�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnOutput(
		ME_HANDLE hHandle,
		int iEvent ) = 0;

	/**
	* close�¼�����
	*
	* <hHandle>���ĸ����������close�¼�
	* <iEvent>��������ʲô�����¼�
	*
	* ����ֵ:
	*		��
	*/
	virtual void OnClose(
		ME_HANDLE hHandle,
		int iEvent ) = 0;
};

NAME_SPACE_END

#endif	// __IO_H_








