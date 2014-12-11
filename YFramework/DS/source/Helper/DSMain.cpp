﻿/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSMain.cpp
\ingroup Helper
\ingroup DS
\brief DS 平台框架。
\version r3164
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2012-03-25 12:48:49 +0800
\par 修改时间:
	2014-12-07 12:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper_(DS)::DSMain
*/


#include "DSScreen.h"
#include YFM_Helper_HostRenderer // for Host::WindowThread;
#include YFM_Helper_Initialization
#include YFM_YSLib_Adaptor_Font
#include "DSWindow.h"
#if YCL_MULTITHREAD == 1
#	include <thread> // for std::this_thread::*;
#endif
#ifdef YCL_DS
#	include YFM_YSLib_Service_YBlit // for Drawing::FillPixel;
#elif YCL_Android
#	include YFM_Android_Helper_AndroidHost // for Android::FetchDefaultWindow;
#endif
#include YFM_YCLib_Debug
#include YFM_Helper_Environment

namespace YSLib
{

using namespace Drawing;

namespace
{

#if YCL_Win32
yconstexpr double g_max_free_fps(1000);
std::chrono::nanoseconds host_sleep(std::uint64_t(1000000000 / g_max_free_fps));
#endif


//! \since build 499
DSApplication* ApplicationPtr;

} // unnamed namespace;


DSApplication::DSApplication()
	: GUIApplication(), DSVideoState(),
	scrs()
#if YCL_Win32
	, p_wnd_thrd()
#endif
{
	using Devices::DSScreen;

	YAssert(!ApplicationPtr, "Duplicate instance found.");
	//注册全局应用程序实例。
	ApplicationPtr = this;
	//初始化系统设备。
#if YCL_DS
	FetchDefaultFontCache();
	InitVideo();
#endif
	Devices::InitDSScreen(scrs[0], scrs[1]);
#if YCL_DS
	FillPixel<Pixel>(scrs[0]->GetCheckedBufferPtr(),
		GetAreaOf(scrs[0]->GetSize()), ColorSpace::Blue),
	FillPixel<Pixel>(scrs[1]->GetCheckedBufferPtr(),
		GetAreaOf(scrs[1]->GetSize()), ColorSpace::Green);
#elif YCL_Win32

	using namespace Host;

	YAssert(IsScreenReady(), "Screen is not ready.");
	p_wnd_thrd.reset(new WindowThread([this]{
		return unique_ptr<Window>(new DSWindow(CreateNativeWindow(
			WindowClassName, {256, 384}, L"YSTest", WS_TILED | WS_CAPTION
			| WS_SYSMENU | WS_MINIMIZEBOX), *scrs[0], *scrs[1],
			GetEnvironment()));
	}));
	while(!p_wnd_thrd->GetWindowPtr())
		// TODO: Resolve magic sleep duration.
		std::this_thread::sleep_for(host_sleep);

	const auto h_wnd(p_wnd_thrd->GetWindowPtr()->GetNativeHandle());
#elif YCL_Android

	const auto h_wnd(&Android::FetchDefaultWindow());

	FetchEnvironment().MapPoint = [](const Point& pt){
		// XXX: Use alternative implementation rather than Win32's.
		const Rect
			bounds(0, MainScreenHeight, MainScreenWidth, MainScreenHeight << 1);

		return bounds.Contains(pt) ? pt - bounds.GetPoint() : Point::Invalid;
	};
#endif
#if YF_Hosted

	yunseq(scrs[0]->WindowHandle = h_wnd, scrs[1]->WindowHandle = h_wnd);
#endif
}

DSApplication::~DSApplication()
{
#if YCL_Win32
	p_wnd_thrd.reset();
	YTraceDe(Notice, "Host thread dropped.");
#endif
	//等待并确保所有 Shell 被释放。
//	hShell = {};
	//释放全局非静态资源。
	//当主 Shell 句柄为静态存储期对象时需要通过 reset 释放。
	//释放设备。
	reset(scrs[0]),
	reset(scrs[1]);
	ApplicationPtr = {};
}

Devices::DSScreen&
DSApplication::GetDSScreenUp() const ynothrow
{
	return Deref(scrs[0]);
}
Devices::DSScreen&
DSApplication::GetDSScreenDown() const ynothrow
{
	return Deref(scrs[1]);
}
Devices::Screen&
DSApplication::GetScreenUp() const ynothrow
{
	return GetDSScreenUp();
}
Devices::Screen&
DSApplication::GetScreenDown() const ynothrow
{
	return GetDSScreenDown();
}

void
DSApplication::SwapScreens()
{
	UI::FetchGUIState().Reset();
	SwapLCD();
#if YF_Hosted
	std::swap(GetDSScreenUp().Offset, GetDSScreenDown().Offset);
#	if !YCL_Android
	if(const auto p_wnd = GetEnvironment().GetForegroundWindow())
		p_wnd->Invalidate();
#	endif
#endif
}


bool
#if YCL_DS
InitConsole(Devices::Screen& scr, Drawing::Pixel fc, Drawing::Pixel bc)
{
	using namespace platform;

	if(&FetchGlobalInstance<DSApplication>().GetScreenUp() == &scr)
		YConsoleInit(true, fc, bc);
	else if(&FetchGlobalInstance<DSApplication>().GetScreenDown() == &scr)
		YConsoleInit(false, fc, bc);
	else
		return {};
#elif YCL_Win32 || YCL_Android
InitConsole(Devices::Screen&, Drawing::Pixel, Drawing::Pixel)
{
#else
#	error "Unsupported platform found."
#endif
	return true;
}

#if YCL_Win32
namespace MinGW32
{

void
TestFramework(size_t idx)
{
	YTraceDe(Notice, ("Test beginned, idx = " + to_string(idx) + " .").c_str());
	YTraceDe(Notice, "Test ended.");
	yunused(idx);
}

} // namespace MinGW32;
#endif

} // namespace YSLib;

