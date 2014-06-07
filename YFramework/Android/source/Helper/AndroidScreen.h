﻿/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file AndroidScreen.h
\ingroup Helper
\ingroup Android
\brief Android 屏幕。
\version r91
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2014-06-04 22:53:58 +0800
\par 修改时间:
	2014-06-07 13:40 +0800
\par 文本编码:
	UTF-8
\par 非公开模块名称:
	Helper_(Android)::AndroidScreen
*/


#ifndef Inc_Android_Helper_AndroidScreen_h_
#define Inc_Android_Helper_AndroidScreen_h_ 1

#include "Helper/YModules.h"
#include YFM_YSLib_Core_YDevice
#include YFM_Helper_ScreenBuffer
#if YCL_Android
#	include <android/native_window.h>
#else
#	error "Currently only Android is supported."
#endif

namespace YSLib
{

namespace Devices
{

/*!
\brief Android 屏幕。
\since build 502 。
*/
class AndroidScreen : public Screen
{
private:
	std::reference_wrapper<::ANativeWindow> window_ref;
	Host::ScreenRegionBuffer rbuf;

public:
	Drawing::Point Offset{};

	AndroidScreen(::ANativeWindow&, const Drawing::Size&);

	DefGetter(const ynothrow, ::ANativeWindow&, WindowRef, window_ref)

	/*!
	\brief 更新。
	\pre 断言：本机窗口上下文非空。
	\pre 间接断言：参数非空。
	\note 复制到本机窗口或本机窗口缓冲区。
	\note 部分线程安全：在不同线程上更新到屏幕和屏幕缓冲区之间线程间未决定有序。
	*/
	void
	Update(Drawing::BitmapPtr) ynothrow override;
};

} // namespace Devices;

namespace Android
{

/*!
\brief 取屏幕大小。
\note 当前直接返回 {480, 800} 。
\since build 503
\todo 因为 Android NDK 不提供直接的接口，使用 JNI 实现。
*/
YF_API Drawing::Size
FetchScreenSize();

} // namespace Android;

} // namespace YSLib;

#endif

