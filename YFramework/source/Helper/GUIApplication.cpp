﻿/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GUIApplication.cpp
\ingroup Helper
\brief GUI 应用程序。
\version r185
\author FrankHB <frankhb1989@gmail.com>
\since build 396
\par 创建时间:
	2013-04-06 22:42:54 +0800
\par 修改时间:
	2013-04-13 12:56 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::GUIApplication
*/


#include "Helper/GUIApplication.h"
#include "Host.h"
#include "Helper/Initialization.h"
#include "YSLib/Adaptor/Font.h"
#if YCL_MULTITHREAD == 1
#	include <thread> // for std::this_thread::*;
#endif

YSL_BEGIN

using namespace Drawing;

namespace
{

#if YCL_MINGW32
yconstexpr double g_max_free_fps(1000);
std::chrono::nanoseconds idle_sleep(u64(1000000000 / g_max_free_fps));
#endif

//! \since build 398
GUIApplication* pApp;

} // unnamed namespace;


namespace
{

/*!
\brief 取空闲消息。
\since build 320
*/
inline Message
FetchIdleMessage()
{
	return Message(SM_INPUT);
}

/*!
\brief 后台消息处理程序。
\since build 320
*/
inline void
Idle(Messaging::Priority prior)
{
	// Note: Wait for GUI input of any shells. Post message for specific shell
	//	would cause low performance when there are many candidate messages
	//	of distinct shells.
	PostMessage(FetchIdleMessage(), prior);
}

} // unnamed namespace;


GUIApplication::GUIApplication()
	: Application(),
#if YCL_HOSTED
	p_hosted(),
#endif
	pFontCache(), UIResponseLimit(0x40), Root()
{
	YAssert(!YSLib::pApp, "Duplicate instance found.");

	YSLib::pApp = this;
	InitializeEnviornment();
#if YCL_HOSTED
	p_hosted = make_unique<Host::Environment>();
#endif
	Root = InitializeInstalled();
	try
	{
		pFontCache = make_unique<FontCache>();
	}
	catch(...)
	{
		throw LoggedEvent("Error occurred in creating font cache.");
	}
	{
		const auto& node(Root["YFramework"]);

		InitializeSystemFontCache(*pFontCache, AccessChild<string>(
			node, "FontFile"), AccessChild<string>(node, "FontDirectory"));
	}
}

GUIApplication::~GUIApplication()
{
	Uninitialize();
}

FontCache&
GUIApplication::GetFontCache() const ynothrow
{
	YAssert(bool(pFontCache), "Null pointer found.");

	return *pFontCache;
}
#if YCL_HOSTED
Host::Environment&
GUIApplication::GetHost()
{
	YAssert(bool(p_hosted), "Null pointer found.");

	return *p_hosted;
}
#endif

bool
GUIApplication::DealMessage()
{
	using namespace Shells;

	if(Queue.IsEmpty())
	{
	//	Idle(UIResponseLimit);
		OnGotMessage(FetchIdleMessage());
#if YCL_MINGW32
	//	std::this_thread::yield();
		std::this_thread::sleep_for(idle_sleep);
#endif
	}
	else
	{
		// TODO: Consider the application queue to be locked for thread safety.
		const auto i(Queue.GetBegin());

		if(YB_UNLIKELY(i->second.GetMessageID() == SM_QUIT))
			return false;
		if(i->first < UIResponseLimit)
		{
			Idle(UIResponseLimit);
#if YCL_MINGW32
			std::this_thread::sleep_for(idle_sleep);
#endif
		}
		OnGotMessage(i->second);
		Queue.Erase(i);
	}
	return true;
}


GUIApplication&
FetchGlobalInstance() ynothrow
{
	YAssert(pApp, "Null pointer found.");

	return *pApp;
}

/* extern */Application&
FetchAppInstance() ynothrow
{
	return FetchGlobalInstance();
}

YSL_END
