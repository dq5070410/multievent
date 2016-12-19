/**
* �Զ����¼��ӿ���
*
* ԭ�������������ΪIEvent����winƽ̨�����������ͽ�Event
* ���ԣ�Ϊ�˱������ʹ���ߣ��͸����˸��ֿڵ����֣��������ã����廹��ͨ˳
*
* �������ֶ�ô����ɣ���������㶨�Ƶ��¼��ܹ���MultiEventʶ��
* �ͱ���̳������
*
*
* History
*	12/12/2012		Created
*/

#ifndef __CUSTOMIZE_EVENT_H_
#define __CUSTOMIZE_EVENT_H_

NAME_SPACE_BEGIN

class OS_EXPORT ICustomizeEvent
{
public:
	virtual ~ICustomizeEvent() {}

public:
	/**
	* ���¼�����ʱ������ص���֪ͨ��
	*
	* �¼���ͬ���첽֮�֣���ͬ��������£����������Ҫ֪��
	* �¼�ִ�н������ôһ����������Ը�ԭ��Ӧ����callback
	* �ĺ��������˷���ֵ
	*/
	virtual ME_Result OnEvent() = 0;

	/**
	* ��OnEvent() callback֮��OnDestroy()�����Żᱻ
	* ���ã�������������¼������������Ĭ����Ϊ������
	* delete this�������������Ҫ�󣬿��������������
	*
	* �ӹ����������������������һ���ص��������ں�������
	* ��On�����ǣ����ǵ���OnEvent���̣��ͼ�����
	*
	* �¼���ͬ���첽֮�֣���ͬ��������£����������Ҫ֪��
	* �¼�ִ�н������ôһ����������Ը�ԭ��Ӧ����callback
	* �ĺ��������˷���ֵ
	*/
	virtual ME_Result OnDestroy();
};

NAME_SPACE_END

#endif	// __CUSTOMIZE_EVENT_H_













