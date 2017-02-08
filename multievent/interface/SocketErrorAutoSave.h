/**
* һ���Զ�����socket�������
* 
* ��linux�£�socket����ᱣ���ڹ�����errno��
* Ϊ�˱���socket error������error���ǣ��͸���
* ��ô����
* 
*
* History
*	12/11/2016		Created
*/

#ifndef __SOCKET_ERROR_AUTO_SAVE_H_
#define __SOCKET_ERROR_AUTO_SAVE_H_

#include "Base.h"

/**
* һ���Զ�����socket�������
* 
* ��linux�£�socket����ᱣ���ڹ�����errno��
* Ϊ�˱���socket error������error���ǣ��͸���
* ��ô����
*/
class CMESocketErrorAutoSave
{
public:
	CMESocketErrorAutoSave();
	~CMESocketErrorAutoSave();

public:
	/**
	* ��ȡϵͳ����
	*/
	int GetLastError() const;

private:
	int								m_iError;
};

#endif	// __SOCKET_ERROR_AUTO_SAVE_H_



