﻿/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NativeAPI.h
\ingroup YCLib
\brief 通用平台应用程序接口描述。
\version r1069
\author FrankHB <frankhb1989@gmail.com>
\since build 202
\par 创建时间:
	2011-04-13 20:26:21 +0800
\par 修改时间:
	2015-08-30 00:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::NativeAPI
*/


#ifndef YCL_INC_NativeAPI_h_
#define YCL_INC_NativeAPI_h_ 1

#include "YModules.h"
#include YFM_YCLib_Platform
#include <ystdex/type_op.hpp> // for ystdex::make_signed_t, std::is_signed;
#include YFM_YBaseMacro

#ifndef YF_Platform
#	error "Unknown platform found."
#endif

/*!	\defgroup workarounds Workarounds
\brief 替代方案。
\since build 297
*/

/*!	\defgroup name_collision_workarounds Name collision workarounds
\brief 名称冲突替代方案。
\note Windows API 冲突时显式使用带 A 或 W 的全局函数名称。
\since build 381
*/


#if YCL_API_Has_dirent_h
#	include <dirent.h>
#endif

// TODO: Test whether <fcntl.h> is available.
#include <fcntl.h>

#if YCL_API_Has_unistd_h
#	include <unistd.h>
//! \since build 625
//@{
namespace platform
{
	using ssize_t = ::ssize_t;
} // namespace platform_ex;
#	else
namespace platform
{
#	if YCL_Win32
	using ssize_t = int;
#	else
	using ssize_t = ystdex::make_signed_t<std::size_t>;
#	endif
} // namespace platform;

static_assert(std::is_signed<platform::ssize_t>(),
	"Invalid signed size type found.");
//@}
#endif


#if YCL_DS
#	include <nds.h>

namespace platform_ex
{

/*!
\brief DMA 异步填充字。
\param chan 使用的通道编号（取值范围 0 ~ 3 ）。
\param val 填充的 32 位字。
\param p_dst 填充目标。
\param size 填充的字节数。
\note 当前仅适用于 ARM9 ；填充的字节数会被按字（ 4 字节）截断。
\since build 405
*/
inline void
DMAFillWordsAsync(std::uint8_t chan, std::uint32_t val, void* p_dst,
	std::uint32_t size)
{
	DMA_FILL(chan) = std::uint32_t(val);
	DMA_SRC(3) = std::uint32_t(&DMA_FILL(3));
	DMA_DEST(3) = std::uint32_t(p_dst);

	DMA_CR(3) = DMA_SRC_FIX | DMA_COPY_WORDS | size >> 2;
}

/*!
\brief 初始化文件系统。
\return 是否成功。
\note 若已初始化则直接返回不成功。
\since build 601
*/
YF_API bool
InitializeFileSystem() ynothrow;

/*!
\brief 反初始化文件系统。
\return 是否成功。
\note 若未初始化则直接返回不成功。
\since build 611
*/
YF_API bool
UninitializeFileSystem() ynothrow;

} // namespace platform_ex;


#elif YCL_Win32

#	ifndef UNICODE
#		define UNICODE 1
#	endif

#	ifndef WINVER
//! \since build 448
#		define WINVER 0x0501
#	endif

#	ifndef WIN32_LEAN_AND_MEAN
//! \since build 448
#		define WIN32_LEAN_AND_MEAN 1
#	endif

#	ifndef NOMINMAX
//! \since build 521
#		define NOMINMAX 1
#	endif

#	include <Windows.h>
#	include <direct.h> // for ::_wmkdir;

//! \ingroup name_collision_workarounds
//@{
//! \since build 297
#	undef DialogBox
//! \since build 298
#	undef DrawText
//! \since build 592
#	undef ExpandEnvironmentStrings
//! \since build 381
#	undef FindWindow
//! \since build 592
#	undef GetMessage
//! \since build 313
#	undef GetObject
//! \since build 298
#	undef PostMessage
//@}

extern "C"
{

#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
//! \since build 465
YF_API struct ::tm* __cdecl __MINGW_NOTHROW
_gmtime32(const ::__time32_t*);
#	endif

} // extern "C";


namespace platform_ex
{

/*!
\brief 判断 WIN32_FIND_DATAA 指定的节点是否为目录。
\since build 298
*/
inline PDefH(bool, IsDirectory, const ::WIN32_FIND_DATAA& d) ynothrow
	ImplRet(d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
/*!
\brief 判断 WIN32_FIND_DATAW 指定的节点是否为目录。
\since build 299
*/
inline PDefH(bool, IsDirectory, const ::WIN32_FIND_DATAW& d) ynothrow
	ImplRet(d.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)

} // namespace platform_ex;

#endif

#if YCL_Win32 || YCL_API_POSIXFileSystem
#	include <sys/stat.h>

namespace platform
{

//! \since build 626
enum class Mode
#	if YCL_Win32
	: unsigned short
#	else
	: ::mode_t
#	endif
{
#	if YCL_Win32
	FileType = _S_IFMT,
	Directory = _S_IFDIR,
	Character = _S_IFCHR,
	FIFO = _S_IFIFO,
	Regular = _S_IFREG,
	UserRead = _S_IREAD,
	UserWrite = _S_IWRITE,
	UserExecute = _S_IEXEC,
	GroupRead = _S_IREAD >> 3,
	GroupWrite = _S_IWRITE >> 3,
	GroupExecute = _S_IEXEC >> 3,
	OtherRead = _S_IREAD >> 6,
	OtherWrite = _S_IWRITE >> 6,
	OtherExecute = _S_IEXEC >> 6,
#	else
	FileType = S_IFMT,
	Directory = S_IFDIR,
	Character = S_IFCHR,
	Block = S_IFBLK,
	Regular = S_IFREG,
	Link = S_IFLNK,
	Socket = S_IFSOCK,
	FIFO = S_IFIFO,
	UserRead = S_IRUSR,
	UserWrite = S_IWUSR,
	UserExecute = S_IXUSR,
	GroupRead = S_IRGRP,
	GroupWrite = S_IWGRP,
	GroupExecute = S_IXGRP,
	OtherRead = S_IROTH,
	OtherWrite = S_IWOTH,
	OtherExecute = S_IXOTH,
#	endif
	UserReadWrite = UserRead | UserWrite,
	User = UserReadWrite | UserExecute,
	GroupReadWrite = GroupRead | GroupWrite,
	Group = GroupReadWrite | GroupExecute,
	OtherReadWrite = OtherRead | OtherWrite,
	Other = OtherReadWrite | OtherExecute,
	Read = UserRead | GroupRead | OtherRead,
	Write = UserWrite | GroupWrite | OtherWrite,
	Execute = UserExecute | GroupExecute | OtherExecute,
	ReadWrite = Read | Write,
	Access = ReadWrite | Execute,
	//! \since build 627
	//@{
#	if !YCL_Win32
	SetUserID = S_ISUID,
	SetGroupID = S_ISGID,
#	else
	SetUserID = 0,
	SetGroupID = 0,
#	endif
#	if YCL_Linux || _XOPEN_SOURCE
	VTX = S_ISVTX,
#	else
	VTX = 0,
#	endif
	PMode = SetUserID | SetGroupID | VTX | Access,
	All = PMode | FileType
	//@}
};

//! \relates Mode
//@{
//! \since build 626
DefBitmaskEnum(Mode)

//! \since build 627
yconstfn PDefH(bool, HasExtraMode, Mode m)
	ImplRet(bool(m & ~(Mode::Access | Mode::FileType)))
//@}

} // namespace platform;
#endif

#endif

