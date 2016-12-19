/**
* 自定义事件接口类
*
* 原本这个类想命名为IEvent，但win平台本身有种锁就叫Event
* 所以，为了避免混淆使用者，就给起了个拗口的名字，不过还好，语义还算通顺
*
* 无论名字多么难念吧，如果你想你定制的事件能够被MultiEvent识别
* 就必须继承这个类
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
	* 当事件发生时，这个回调会通知你
	*
	* 事件有同步异步之分，在同步的情况下，或许存在需要知道
	* 事件执行结果的这么一种情况，所以给原本应当是callback
	* 的函数加上了返回值
	*/
	virtual ME_Result OnEvent() = 0;

	/**
	* 在OnEvent() callback之后，OnDestroy()紧接着会被
	* 调用，用来销毁这个事件。这个函数有默认行为，就是
	* delete this，如果你有特殊要求，可以重载这个函数
	*
	* 从功能上来讲，这个函数不是一个回调，不用在函数名上
	* 加On，但是，考虑到与OnEvent对仗，就加上了
	*
	* 事件有同步异步之分，在同步的情况下，或许存在需要知道
	* 事件执行结果的这么一种情况，所以给原本应当是callback
	* 的函数加上了返回值
	*/
	virtual ME_Result OnDestroy();
};

NAME_SPACE_END

#endif	// __CUSTOMIZE_EVENT_H_













