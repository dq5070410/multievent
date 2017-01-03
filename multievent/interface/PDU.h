/**
* PDU���壬Ŀǰֻ������pdu��Package��KeepAlive
* 
*
* History
*	04/26/2016		Created
*/

#ifndef __PDU_H_
#define __PDU_H_

#include "Base.h"

ME_NAME_SPACE_BEGIN

class CMEMessageBlock;

/**
 *  �������� PDU
 *
 *  Encode����ͷ�������ĸ��ֽڵĳ����ֶ�
 *  Decode�Ǹ���ͷ���ĳ����ֶ����һ���û���
 */
class CMEPDUPackage
{
public:
	/* decode */
	CMEPDUPackage();

	/* encode */
	CMEPDUPackage( 
		UINT nLength,
		CMEMessageBlock* mbBlock );

	~CMEPDUPackage(){}

public:
	ME_Result Encode( CMEMessageBlock& mbBlock );

	ME_Result Decode( CMEMessageBlock& mbBlock );

public:
	UINT GetLength() const;

	CMEMessageBlock* GetData() const;

private:
	UINT							m_nPackageLength;

	CMEMessageBlock*				m_pUserMB;

};

class ME_OS_EXPORT CMEPDUBase
{
public:
	CMEPDUBase();

protected:
	explicit CMEPDUBase( BYTE byType );

public:
	virtual ~CMEPDUBase(){}

public:
	virtual BYTE GetType();

	virtual UINT GetLength();

public:
	virtual ME_Result Encode( CMEMessageBlock& mbBlock );

	virtual ME_Result Decode( CMEMessageBlock& mbBlock );

protected:
	BYTE							m_byType;

};

class CMEPDUKeepAliveReq : public CMEPDUBase
{
public:
	/* encode */
	CMEPDUKeepAliveReq();

};

class CMEPDUKeepAliveRsp : public CMEPDUBase
{
public:
	/* encode */
	CMEPDUKeepAliveRsp();

};

ME_NAME_SPACE_END

#endif	// __PDU_H_
