﻿/*
	© 2012-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file InputManager.h
\ingroup Helper
\brief 输入管理器。
\version r187
\author FrankHB <frankhb1989@gmail.com>
\since build 323
\par 创建时间:
	2012-07-06 11:22:04 +0800
\par 修改时间:
	2016-02-27 14:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::InputManager
*/


#ifndef INC_Helper_InputManager_h_
#define INC_Helper_InputManager_h_ 1

#include "YModules.h"
#include YFM_Helper_YGlobal
#include YFM_YSLib_UI_YGUI

namespace YSLib
{

namespace Devices
{

/*!
\brief 输入管理器。
\since build 323
*/
class YF_API InputManager : private noncopyable
{
private:
	/*!
	\brief GUI 状态。
	\since build 554
	*/
	lref<UI::GUIState> GUI_state;
	/*!
	\brief 指针设备光标位置。
	\note 没有必要把输入内容以消息队列传输，因为总是有且仅有一个实例被同时处理。
	*/
	Drawing::Point cursor_state;
	/*!
	\brief GUI 宿主。
	\since build 689
	*/
	lref<GUIHost> host;

#if YCL_Win32
	/*!
	\brief 文本焦点缓存。
	\since build 672
	*/
	observer_ptr<UI::IWidget> p_text_focus_cache = {};
	/*!
	\brief 插入符位置缓存。
	\since build 512
	*/
	Drawing::Point caret_location_cache{Drawing::Point::Invalid};
#endif

public:
	/*!
	\sa FetchGUIHost
	\sa UI::FetchGUIState
	*/
	InputManager();

	/*!
	\brief 向指定部件分发响应输入状态。
	\note Win32 平台：更新文本焦点缓存和插入符位置缓存。
	\since build 387

	指定平台相关的用户界面输入处理。
	*/
	void
	DispatchInput(UI::IWidget&);

	/*!
	\brief 更新输入状态。
	\return 非宿主实现总是空指针，否则是待分发输入状态的顶层部件指针。
	\note 对宿主实现，当前忽略顶级窗口不是 Host::Window 的情形。
	\since build 670
	*/
	observer_ptr<UI::IWidget>
	Update();
};

} // namespace Devices;

} // namespace YSLib;

#endif

