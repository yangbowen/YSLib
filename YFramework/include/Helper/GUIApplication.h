﻿/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GUIApplication.h
\ingroup Helper
\brief GUI 应用程序。
\version r171
\author FrankHB <frankhb1989@gmail.com>
\since build 398
\par 创建时间:
	2013-04-11 10:02:53 +0800
\par 修改时间:
	2013-04-13 13:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::GUIApplication
*/


#ifndef INC_Helper_GUIApplication_h_
#define INC_Helper_GUIApplication_h_ 1

#include "Helper/yglobal.h"
#include "YCLib/Input.h"
#include "YSLib/Core/yapp.h"
#include "YSLib/Core/ValueNode.h"
#include <ystdex/cast.hpp>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)
class FontCache;
YSL_END_NAMESPACE(Drawing)
#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)
#if YCL_MINGW32
//! \since build 389
typedef ::HWND NativeWindowHandle;
#endif
class Window;
class RenderWindow;
class WindowThread;
class HostRenderer;
class Environment;
YSL_END_NAMESPACE(Host)
#endif


/*!
\brief 平台相关的应用程序类。
\note 含默认接口。
\since build 398
*/
class YF_API GUIApplication : public Application
{
#if YCL_HOSTED
private:
	/*!
	\brief 宿主状态。
	*/
	unique_ptr<Host::Environment> p_hosted;
#endif

protected:
	/*!
	\brief 默认字体缓存。
	\since build 325
	*/
	unique_ptr<Drawing::FontCache> pFontCache;

public:
	/*!
	\brief 用户界面输入响应阈值。
	\note 默认值 0x40 。
	\see DSApplication::Run 。

	用于主消息队列的消息循环中控制后台消息生成策略的全局消息优先级。
	*/
	Messaging::Priority UIResponseLimit;
	/*!
	\brief 值类型根节点。
	*/
	ValueNode Root;

	/*!
	\brief \c private 构造函数：非内联。
	\pre 断言：进程唯一性。
	*/
	GUIApplication();
	/*!
	\brief 析构：释放资源。
	*/
	~GUIApplication() override;

	/*!
	\brief 取字体缓存引用。
	\pre 断言：指针非空。
	*/
	Drawing::FontCache&
	GetFontCache() const ynothrow;
#if YCL_HOSTED
	Host::Environment&
	GetHost();
#endif

	/*!
	\brief 处理当前消息。
	\return 循环条件。
	\note 优先级小于 UIResponseLimit 的消息时视为后台消息，否则为前台消息。

	若主消息队列为空，处理空闲消息，否则从主消息队列取出并分发消息。
	当取出的消息的标识为 SM_QUIT 时视为终止循环。
	对于后台消息，分发前调用后台消息处理程序：分发空闲消息并可进行时序控制。
	*/
	bool
	DealMessage();
};


/*!
\brief 取全局应用程序实例。
\pre 内部断言检查存在应用程序实例。
\note 需要保证应用程序实例初始化后才能使用。
\since build 398
*/
//@{
YF_API GUIApplication&
FetchGlobalInstance() ynothrow;
template<class _tApp>
inline _tApp&
FetchGlobalInstance() ynothrow
{
	return ystdex::polymorphic_downcast<_tApp&>(FetchGlobalInstance());
}
//@}

/*!
\ingroup helper_functions
\brief 取默认字体缓存。
\since build 398
*/
inline Drawing::FontCache&
FetchDefaultFontCache() ynothrow
{
	return FetchGlobalInstance().GetFontCache();
}

#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)

//! \since build 398
inline Environment&
FetchEnvironment() ynothrow
{
	return FetchGlobalInstance().GetHost();
}

YSL_END_NAMESPACE(Host)
#endif

YSL_END

#endif
