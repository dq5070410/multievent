/**
 *  ����WaitForMultipleObjects�ķ������ඨ��
 *
 *  History
 *  02/01/2016 create file
 */

#include "ReactorWFMO.h"
#include "ConnectionDefines.h"
#include "Reactor.h"
#include "IO.h"
#include "LockGuardT.h"
#include "WakenerEvent.h"

#ifdef ME_WIN

ME_NAME_SPACE_BEGIN

CMEReactorWFMO::CMEReactorWFMO( IMEWakener* pWakener )
    : CMEReactorBase(
        pWakener,
        CMEReactor::REACTOR_TYPE_WFMO )
{
    INT iIndex = 0;
    for ( ; (UINT)iIndex < ME_WFMO_MAX_WAIT_NUM; ++iIndex )
    {
        m_EventArray[iIndex] = NULL;
        m_Slot[iIndex] = 0;             // �ܾ���������0���ã�������-1���з���
    }

    /**
     *  Wakener�Ƚ����⣬Ŀǰֻ����Reactor�����ʱ��ע�ᵽReactor�����ң�Ŀǰû
     *  �н���ע������Ŀǰ��ʵ�֣������������⣬���վ��ǲ�̫�Գƣ������л����ٸ�
     */
    iIndex = -1;
    Register(
        pWakener,
        pWakener,
        CMEReactor::EVENT_TYPE_ACTIVE,
        iIndex );
}

CMEReactorWFMO::~CMEReactorWFMO()
{

}

ME_Result CMEReactorWFMO::HandleEvent()
{
    ME_INFO_TRACE_THIS( "" );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    ME_Result hResult = ME_ERROR_FAILURE;
    INT iStartIndex = 0;
    INT iTotalNum = m_ioItemMap.size();
    UINT iTimeout = GetWaitTime();   // ��һ�εȴ����û����ṩ�Ľӿڻ�ȡ�ȴ�ʱ��

    /**
     *  ͨ����ε���WaitForMultipleObjects���������м�����¼���������
     *  �˴���++iStartIndex�ܹ��÷���������ǴӼ����¼������ĺ�һ��λ�ÿ�ʼ�µ�
     *  һ�β���
     */
    for ( ; iStartIndex < iTotalNum; ++iStartIndex )
    {
        DWORD iActivePos = WaitForMultipleObjects(
            iTotalNum - iStartIndex,
            &m_EventArray[iStartIndex],
            FALSE,
            iTimeout );

        /** 
         *  �ȵ������ˣ�����ֻ��������С��һ������ô��Ҫ���õȴ�ʱ�����ʼ�¼���
         *  �������������¼����
         */
        if ( WAIT_FAILED != iActivePos && WAIT_TIMEOUT != iActivePos )
        {
            iTimeout = 0;
            /* ������iActivePos - WAIT_OBJECT_0��ʽ����������ƫ�� */
            iStartIndex += ( iActivePos - WAIT_OBJECT_0 );

            /* Ŀǰ�Դ����¼�û��ʲô�ر�Ĵ������������¼���״̬ */
            INT iIOItemIndex = m_Slot[iStartIndex];
            ME_SET_BITS( m_ioItemMap[iIOItemIndex].iActiveEvent, CMEReactor::EVENT_TYPE_ACTIVE );
            ++m_iActiveIOItemNum;
        }
        /* �ȴ���ʱ */
        else if ( WAIT_TIMEOUT == iActivePos )
        {
            hResult = ME_ERROR_TIMEOUT;
            break;
        }
        /* �������� */
        else
        {
            hResult = ME_ERROR_FAILURE;
            break;
        }
    }

    /**
     *  ���ݲ��񵽵ļ�����¼�������������������ֵ����
     *  ֻ��û�в��񵽼�����¼�����ʱ���ŷ��س�ʱ�����
     *  ��Ϊ�����ѭ���᳢�Բ������м����Event���������һ�β���Ľ���ܿ�����
     *  ��ʱ��Ҳ�����ų���������Ŀ��ܣ������ʱ��Ҳ���ܹ����ǳ�ʱ����Ӧ���÷���
     *  ��������ɹ��̽����񵽵��¼������
     */
    if ( 0 < m_iActiveIOItemNum )
    {
        hResult = ME_OK;
    }

    return hResult;
}

ME_Result CMEReactorWFMO::AddWaitableObject(
    IMEIO* pIO,
    INT iEvent,
    INT iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", iEvent: " << iEvent
        << ", iIndex: " << iIndex );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /** 
     *  WaitForMultipleObjectsĿǰֻ����Windows�¼������ֻ�м���ͷǼ�������״
     *  ̬����û�о�����¼����ͣ���ȻĿǰ��������˶���д�����¼����ͣ�����ʱ��
     *  ��֧���޸��ˣ��Ѿ����ڵ��¼�������ظ���ӻ����޸ģ��Ժ�Ҫ���޸Ĺ��ܣ���
     *  ô����Ҫɾ����ǰ�ģ������һ���µ�
     */
    INT iWaitableObjectIndex = 0;
    INT iTotalNum = m_ioItemMap.size();

    for (; iWaitableObjectIndex < iTotalNum; ++iWaitableObjectIndex)
    {
        /* ֻҪ���һ������ô����Ϊ��ͬһ�� */
        if ( m_EventArray[iWaitableObjectIndex] == pIO->GetHandle() )
        {
            break;
        }
    }

    /** 
     *  ����ͻ���ע��ӿ�ʵ�ֹ�����̫ǿ��ֻ�����ȵ��õ�ǰ�����������io item��
     *  ������£�����жϲų�������Ҫ�ر�ע��
     *  û��ƥ���io�¼��������ӽ���
     */
    if ( iWaitableObjectIndex == iTotalNum )
    {
        m_EventArray[iWaitableObjectIndex] = pIO->GetHandle();
        m_Slot[iWaitableObjectIndex] = iIndex;
    }

    return ME_OK;
}

ME_Result CMEReactorWFMO::RemoveWaitableObject(
    IMEIO* pIO,
    INT iEvent,
    INT iIndex )
{
    ME_INFO_TRACE_THIS( "pIO: " << pIO
        << ", iEvent: " << iEvent
        << ", iIndex: " << iIndex );

    CMELockGuardT<CMELockThreadMutex> Guard( &m_hThreadMutex );

    /** 
     *  ��ע��ʱ�����ȡ�����¼����ͽ���Ԥ����������ٹ�עĳ���¼��������ô��
     *  �е��¼����Ͷ����뱻ȡ������Ϊ�����ж�һ��io item�Ƿ��Ƴ��Ǹ����Ƿ�ȡ��
     *  ���е��¼����ͣ���ô�������Ҫ�ͻ���ͬ�����Ͳ��ܽ�����ע����д�¼����ͣ�
     *  �����ж������¼����Ͷ���Ҫȡ��ʱ�����Ƴ��¼���������򣬻����鷳��ͬ����
     *  ��
     */
    INT iWaitableObjectIndex = 0;
    INT iTotalNum = m_ioItemMap.size();

    for (; iWaitableObjectIndex < iTotalNum; ++iWaitableObjectIndex)
    {
        /* ֻҪ���һ������ô����Ϊ��ͬһ�� */
        if ( m_EventArray[iWaitableObjectIndex] == pIO->GetHandle() )
        {
            break;
        }
    }

    /** 
     *  �ҵ��¼������������Ҫ�Ƴ�������ô����Ҫ���������е��¼��������һǰ��
     *  �Ժ����ʵ�ֿ��Ƴ������¼����ͣ���ô����Ҫ���Ƴ���ǰ�ģ�Ȼ������һ���µ�
     *  ����ǰ��λ��
     */
    if ( iWaitableObjectIndex < iTotalNum &&
        ME_BIT_CMP_MASK(iEvent, CMEReactor::EVENT_TYPE_ALL, CMEReactor::EVENT_TYPE_ALL) )
    {
        /* ����ҪС���±�Խ�� */
        for ( ; iWaitableObjectIndex < iTotalNum - 1; ++iWaitableObjectIndex )
        {
            m_EventArray[iWaitableObjectIndex] = m_EventArray[iWaitableObjectIndex + 1];
            m_Slot[iWaitableObjectIndex] = m_Slot[iWaitableObjectIndex + 1];
        }

        /* ����ĩβ�����ݣ�����ôд�ţ��������Ҫ��ɾ�� */
        m_EventArray[iWaitableObjectIndex] = NULL;
        m_Slot[iWaitableObjectIndex] = 0;
    }

    return ME_OK;
}

ME_NAME_SPACE_END

#endif	// ME_WIN