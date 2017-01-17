/**
* IP�����࣬���е�socket�඼Ӧ�ô�����̳�
* 
*
* History
*	12/08/2016		Created
*/

#ifndef __IP_BASE_H_
#define __IP_BASE_H_

#include "Base.h"
#include "IO.h"

NAME_SPACE_BEGIN

/**
* IP�����࣬���е�socketӦ���඼������̳�
*/
class ME_OS_EXPORT CIPBase : public IMEIO
{
public:
	/* ������ѡ�� */
	enum
	{
		/**
		* ����option��������д
		*
		* Ŀǰ����NON_BLOCK�͹��ˣ������Ժ��ٲ�
		*/
		NON_BLOCK,
	};

public:
	virtual ~CIPBase();

public:
	/**
	* ���þ��
	*
	* <hHandle>, Ҫ���õľ��
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�
	*/
	virtual void SetHandle( ME_HANDLE hHandle );

	/**
	* ��ȡ���
	*
	* ����ֵ:
	*		��ΪNULL�Ҳ�ΪME_INVALID_HANDLEΪ�ɹ�
	*/
	virtual ME_HANDLE GetHandle() const;

public:

	/**
	* ����ĳ��I/Oģʽ, Ŀǰ, ֻ����������/����������
	*
	* <iValue>, Ҫ����ʲôģʽ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�
	*/
	ME_Result Enable( int iValue );

	/**
	* �رյ�ĳ��I/Oģʽ, Ŀǰ, ֻ����������/����������
	*
	* <iValue>, Ҫ�ر�ʲôģʽ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�
	*/
	ME_Result Disable( int iValue );

	/**
	* ����ĳ��I/Oģʽ
	*
	* <iCmd>, Ҫ����ʲôģʽ
	* <pArg>, ��Ӧ<iCmd>��ֵ
	*
	* ����ֵ:
	*		ME_OKΪ�ɹ�
	*/
	ME_Result Control( 
		int iCmd,
		void* pArg );

protected:
	CIPBase();

	CIPBase( ME_HANDLE hHandle );

protected:
	ME_HANDLE						m_hHandle;
};

NAME_SPACE_END

#endif	// __ME_IP_BASE_H_








