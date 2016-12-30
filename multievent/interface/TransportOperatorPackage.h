/**
* ����TCP������Ĵ�����
* 
*
* History
*	2/19/2016		Created
*/

#ifndef __TRANSPORT_OPERATOR_PACKAGE_H_
#define __TRANSPORT_OPERATOR_PACKAGE_H_

#include "Base.h"
#include "TransportOperatorBase.h"
#include "ConnectionDefines.h"
#include "MessageBlock.h"
#include "PDU.h"

ME_NAME_SPACE_BEGIN

/**
* ����TCP������Ĵ�����
* 
* ���������������Ҫ�������ݵ�ǰ�����4���ֽڵĳ��ȱ�ʶ��
* ���ڱ�֤�շ�����������
*
* ���½ṹ���£�
* 
*  4 bytes  <�û�����>
* |_ _ _ _|________________________
*
* ����˵���㷢��ȥ�೤�İ����յ���Ҳ�Ƕ೤�İ���������
* �ܵ�TCP��Ƭ�ĸ���
*/
class CMETransportOperatorPackage : public CMETransportOperatorBase
{
public:
	CMETransportOperatorPackage();

	virtual ~CMETransportOperatorPackage();

	/* �̳���IMETransportOperator */
public:
	/**
	* �ڰ�ͷ����4���ֽڵİ������ֶ� 
	* ����Ǵ��������64K����ֿ�����
	*
	* ����ֵ��
	*      RETURN_VALUE_OK���������
	*      RETURN_VALUE_AGAIN����û�д����꣬����Ҫ��������һ�ε��ô˷���
	*/
	virtual ME_Result PreSendData( CMEMessageBlock*& pmbBlock );

	/**
	* ��鷢�ͽ�����������ʧ������˵���һ��PreSendData֮ǰ������MessageBlock��дλ�ã���
	*
	* ����ֵ��
	*		RETURN_VALUE_OK���������
	*/
	virtual ME_Result PostSendData( CMEMessageBlock*& pmbBlock );

	/**
	* �Դ������Ͻ��յ��ð����д�����Ҫ�����Ƭ��ճ����PreSendData����Ϊ�ְ�
	* 
	* ����ֵ��
	*		RETURN_VALUE_OK���������
	*		RETURN_VALUE_AGAIN��������ճ������Ҫ�����ŵ���һ�δ˷���
	*		RETURN_VALUE_FINISH�������˷�Ƭ����δ�ռ���һ���������û�������Ҫ�ȴ���һ��������ĵ���
	*/
	virtual ME_Result PreOnReceive( CMEMessageBlock*& pmbBlock );

	/**
	* PreOnReceive�������ڴ棬����������ڴ���л���
	*/
	virtual ME_Result PostOnReceive( CMEMessageBlock*& pmbBlock );

	virtual DWORD GetType();

private:
	CHAR* SelfInfo();

	ME_Result RecvOK();

	ME_Result RecvSlice();

	ME_Result RecvStick();

private:
	/**
	* ���������ã�ȷ�е�˵�Ƿ��ʹ�����ݻ��õ�������64k��
	* ���ʹ��ʱ��Ҫ��ͣ���л�дλ�ã�����������Ҫ����ԭʼ��дλ��
	*/
	BOOL							m_bNewPackage;

	BOOL							m_bStickPackage;

	CMEMessageBlockAutoPtr			m_pMB;

    CMEMessageBlockAutoPtr         m_pmbRecvOut;

#ifdef ME_DEBUG
private:
	/* ��Ԫ������Ԫ�� */
	friend class TestTOP;
#endif

};

ME_NAME_SPACE_END

#endif	// __TRANSPORT_OPERATOR_PACKAGE_H_
