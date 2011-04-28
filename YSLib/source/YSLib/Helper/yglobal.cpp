﻿/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yglobal.cpp
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version 0.3060;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 15:28:52 +0800;
\par 修改时间:
	2011-04-28 17:25 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YGlobal;
*/


#include "yglobal.h"
#include "../Core/yfilesys.h"
#include "../Core/yapp.h"
#include "../Core/yshell.h"
#include "../Core/ydevice.h"
#include "../Adaptor/yfont.h"
#include "../Adaptor/ysinit.h"
#include "../Shell/ydesktop.h"
//#include <clocale>

YSL_BEGIN

using namespace Runtime;

//全局常量。
extern const char* DEF_DIRECTORY; //<! 默认目录。
extern const char* G_COMP_NAME; //<! 制作组织名称。
extern const char* G_APP_NAME; //!< 产品名称。
extern const char* G_APP_VER; //!< 产品版本。
const IO::Path YApplication::CommonAppDataPath(DEF_DIRECTORY);
const String YApplication::CompanyName(G_COMP_NAME);
const String YApplication::ProductName(G_APP_NAME);
const String YApplication::ProductVersion(G_APP_VER);

//全局变量。
#ifdef YSL_USE_MEMORY_DEBUG
//MemoryList DebugMemory(nullptr);
#endif

/*!
\ingroup PublicObject
\brief 全局变量映射。
*/
//@{
//@}


const SDst Global::MainScreenWidth(SCREEN_WIDTH);
const SDst Global::MainScreenHeight(SCREEN_HEIGHT);

Global::Global()
	: hScreenUp(nullptr), hScreenDown(nullptr), hDesktopUp(nullptr), hDesktopDown(nullptr)
{}
Global::~Global()
{}

YScreen&
Global::GetScreenUp() const ynothrow
{
	YAssert(hScreenUp, "Fatal error @ Global::GetScreenUp:"
		" the up screen handle is null.");

	return *hScreenUp;
}
YScreen&
Global::GetScreenDown() const ynothrow
{
	YAssert(hScreenDown, "Fatal error @ Global::GetScreenDown:"
		" the down screen handle is null.");

	return *hScreenDown;
}
Desktop&
Global::GetDesktopUp() const ynothrow
{
	YAssert(hDesktopUp, "Fatal error @ Global::GetDesktopUp:"
		" the up desktop handle is null.");

	return *hDesktopUp;
}
Desktop&
Global::GetDesktopDown() const ynothrow
{
	YAssert(hDesktopDown, "Fatal error @ Global::GetDesktopDown:"
		" the down desktop handle is null.");

	return *hDesktopDown;
}

void
Global::InitializeDevices() ynothrow
{
	//初始化显示设备。
	try
	{
		hScreenUp = new YScreen(MainScreenWidth, MainScreenHeight);
		hScreenDown = new YScreen(MainScreenWidth, MainScreenHeight);
	}
	catch(...)
	{
		throw LoggedEvent("Screen initialization failed.");
	}
	try
	{
		hDesktopUp = new Desktop(*hScreenUp);
		hDesktopDown = new Desktop(*hScreenDown);
	}
	catch(...)
	{
		throw LoggedEvent("Desktop initialization failed.");
	}
}

void
Global::ReleaseDevices() ynothrow
{
	hDesktopUp.reset();
	hScreenUp.reset();
	hDesktopDown.reset();
	hScreenDown.reset();
}


Global&
GetGlobal() ynothrow
{
	static Global global_resource;

	return global_resource;
}

YApplication&
GetApp()
{
	GetGlobal();

	static YApplication& theApp(YApplication::GetInstance());

	return theApp;
}

const GHandle<YShell>&
GetMainShellHandle()
{
	GetApp();

	static GHandle<YShell> hMainShell(new YMainShell());

	return hMainShell;
}


namespace
{
	bool
	operator==(const KeysInfo& a, const KeysInfo& b)
	{
		return a.Up == b.Up && a.Down == b.Down && a.Held == b.Held;
	}

	inline bool
	operator!=(const KeysInfo& a, const KeysInfo& b)
	{
		return !(a == b);
	}
}

YSL_BEGIN_NAMESPACE(Messaging)

bool
InputContext::operator==(const IContext& rhs) const
{
	const InputContext* p(dynamic_cast<const InputContext*>(&rhs));

	return Key == p->Key && CursorLocation == p->CursorLocation;
}

YSL_END_NAMESPACE(Messaging)

namespace
{
	/*!
	\note 转换指针设备光标位置为屏幕点。
	*/
	inline Drawing::Point
	ToSPoint(const Runtime::CursorInfo& c)
	{
		return Drawing::Point(c.GetX(), c.GetY());
	}

	//图形用户界面输入等待函数。
	void
	WaitForGUIInput()
	{
		using namespace Messaging;

		static KeysInfo Key;
		static CursorInfo TouchPos_Old, TouchPos;
		static GHandle<InputContext> pContext;

		if(Key.Held & KeySpace::Touch)
			TouchPos_Old = TouchPos;
		scanKeys();
		WriteKeysInfo(Key, TouchPos);

		const Point pt(ToSPoint(Key.Held & KeySpace::Touch
			? TouchPos : TouchPos_Old));

		if(!pContext || ((GetApp().GetDefaultMessageQueue().IsEmpty()
			|| Key != pContext->Key || pt != pContext->CursorLocation)
			&& pt != Point::FullScreen))
		{
			pContext = new InputContext(Key, pt);
			SendMessage(Message(Shells::GetCurrentShellHandle(), SM_INPUT, 0x40,
				pContext));
		}
	}
}


void
Idle()
{
	WaitForGUIInput();
}

bool
InitConsole(YScreen& scr, Drawing::PixelType fc, Drawing::PixelType bc)
{
	using namespace platform;

	if(GetGlobal().GetScreenUpHandle() == &scr)
		YConsoleInit(true, fc, bc);
	else if(GetGlobal().GetScreenDownHandle() == &scr)
		YConsoleInit(false, fc, bc);
	else
		return false;
	return true;
}

void
Destroy_Static(YObject&, EventArgs&)
{
}

int
ShlProc(GHandle<YShell> hShl, const Message& msg)
{
	return hShl->ShlProc(msg);
}

//非 yglobal.h 声明的平台相关函数。

bool
InitAllScreens()
{
	using namespace Runtime;

	InitVideo();

	YScreen& up_scr(GetGlobal().GetScreenUp());

	up_scr.pBuffer = DS::InitScrUp(up_scr.bg);

	YScreen& down_scr(GetGlobal().GetScreenDown());

	down_scr.pBuffer = DS::InitScrDown(down_scr.bg);
	return true;
}


/*!
\brief Shell 对象释放函数。
*/
extern void
ReleaseShells();


void
ShowFatalError(const char* s)
{
	using namespace platform;

	YDebugSetStatus();
	YDebugBegin();
	std::printf("Fatal Error:\n%s\n", s);
	terminate();
}

#ifdef YSL_USE_MEMORY_DEBUG

namespace
{
	void
	OnExit_DebugMemory_continue()
	{
		std::fflush(stderr);
		std::puts("Input to continue...");
		WaitForInput();
	}
}

void
OnExit_DebugMemory()
{
	using namespace platform;

	YDebugSetStatus();
	YDebugBegin();
	std::puts("Normal exit;");

//	std::FILE* fp(std::freopen("memdbg.log", "w", stderr));
	MemoryList& debug_memory_list(GetDebugMemoryList());
	const typename MemoryList::MapType& Map(debug_memory_list.Blocks);
//	MemoryList::MapType::size_type s(DebugMemory.GetSize());

	if(!Map.empty())
	{
		std::fprintf(stderr, "%i memory leak(s) detected:\n", Map.size());

		MemoryList::MapType::size_type n(0);

		for(typename MemoryList::MapType::const_iterator i(Map.begin());
			i != Map.end(); ++i)
		{
			if(n++ < 4)
				debug_memory_list.Print(i, stderr);
			else
			{
				n = 0;
				OnExit_DebugMemory_continue();
			}
		}
	//	DebugMemory.PrintAll(stderr);
	//	DebugMemory.PrintAll(fp);
		OnExit_DebugMemory_continue();
	}

	const typename MemoryList::ListType&
		List(debug_memory_list.DuplicateDeletedBlocks);

	if(!List.empty())
	{
		std::fprintf(stderr, "%i duplicate memory deleting(s) detected:\n",
			List.size());

		MemoryList::ListType::size_type n(0);

		for(typename MemoryList::ListType::const_iterator i(List.begin());
			i != List.end(); ++i)
		{
			if(n++ < 4)
				debug_memory_list.Print(i, stderr);
			else
			{
				n = 0;
				OnExit_DebugMemory_continue();
			}
		}
	//	DebugMemory.PrintAllDuplicate(stderr);
	//	DebugMemory.PrintAllDuplicate(fp);
	}
//	std::fclose(fp);
	std::puts("Input to terminate...");
	WaitForInput();
}

#endif

YSL_END

int
main(int argc, char* argv[])
{
	using namespace YSL;

	try
	{
		//全局初始化。

		//设置默认异常终止函数。
		std::set_terminate(terminate);

		//启用设备。
		powerOn(POWER_ALL);

		//启用 LibNDS 默认异常处理。
		defaultExceptionHandler();

		//初始化主控制台。
		InitYSConsole();

	/*	if(!setlocale(LC_ALL, "zh_CN.GBK"))
		{
			EpicFail();
			platform::yprintf("setlocale() with %s failed.\n", "zh_CN.GBK");
			terminate();
		}*/

		//初始化文件系统。
		//初始化 EFSLib 和 LibFAT 。
		//当 .nds 文件大于32MB时， EFS 行为异常。
	#ifdef USE_EFS
		if(!EFS_Init(EFS_AND_FAT | EFS_DEFAULT_DEVICE, nullptr))
		{
			//如果初始化 EFS 失败则初始化 FAT 。
	#endif
			if(!fatInitDefault())
				LibfatFail();
			IO::ChangeDirectory(Text::StringToMBCS(
				YApplication::CommonAppDataPath));
	#ifdef USE_EFS
		}
	#endif

		//检查程序是否被正确安装。
		CheckInstall();

		//初始化系统字体资源。
		InitializeSystemFontCache();

		//初始化系统设备。
		GetGlobal().InitializeDevices();

		//注册全局应用程序对象。
		GetApp().ResetShellHandle();
		//GetApp().SetOutputPtr(hDesktopUp);
		//DefaultShellHandle->SetShlProc(ShlProc);

		//主体。

		using namespace Shells;

		Message msg;

		//消息循环。
		while(GetMessage(msg) != SM_QUIT)
		{
			TranslateMessage(msg);
			DispatchMessage(msg);
		}

		const int r(msg.GetContextPtr() ? 0 : -1);

		//释放 Shell 。

		YSL_ ReleaseShells();

		//释放全局非静态资源。

		//释放默认字体资源。
		GetApp().DestroyFontCache();

		//释放设备。
		GetGlobal().ReleaseDevices();

	#ifdef YSL_USE_MEMORY_DEBUG
		OnExit_DebugMemory();
	#endif
		return r;
	}
	catch(std::exception& e)
	{
		YSL_ GetApp().Log.FatalError(e.what());
	}
	catch(...)
	{
		YSL_ GetApp().Log.FatalError("Unhandled exception"
			" @ int main(int, char*[]);");
	}
}

