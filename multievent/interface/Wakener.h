/**
 *  �����������߽ӿ���
 *
 *
 *  History
 *  01/28/2016 create file
 */

#ifndef __WAKENER_H_
#define __WAKENER_H_

#include "IO.h"

ME_NAME_SPACE_BEGIN

/**
 *  ��������������
 *
 *  �����ڷ������ȴ���ʱ��֪ͨ�������ȴ�����Ȼ����Ϊ��ͬ�ķ��������õĶ�·�����
 *  �Ʋ�һ����������Ҳ��Ҫ���������õĶ�·������ƶ��ֱ�ʵ��
 *
 */
class IMEWakener : public IMEIO
                  , public IMEIOSink
{
public:
    virtual ~IMEWakener() {}

    /**
     *  ֪ͨ�����������ȴ�
     *
     *  ����ֵ: 
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result Notify() = 0;

    /**
     *  ��ʼ��������
     *
     *  �������ߵ���Դ��SocketPair��Windows�¼�����ȣ���������������������ɣ�
     *  �����Ƚ����׻�֪��Щ��Դ�Ƿ񴴽��ɹ�
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result Initialize() = 0;

    /**
     *  ����ʼ��������
     *
     *  �������ߵ���Դ���ٹ����������������
     *
     *  ����ֵ:
     *      ME_OK��ʾ�ɹ�; ��ME_OK��ʾʧ��
     */
    virtual ME_Result Finalize() = 0;
};

ME_NAME_SPACE_END

#endif // __WAKENER_H_

