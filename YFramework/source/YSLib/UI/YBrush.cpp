﻿/*
	Copyright by FrankHB 2012-2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBrush.cpp
\ingroup UI
\brief 图形用户界面画刷。
\version r216
\author FrankHB <frankhb1989@gmail.com>
\since build 293
\par 创建时间:
	2012-01-10 19:56:59 +0800
\par 修改时间:
	2013-09-06 23:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YBrush
*/


#include "YSLib/UI/YBrush.h"
#include "YSLib/Service/ydraw.h"
#include "YSLib/UI/ygui.h"
#include "YSLib/UI/ywidget.h"
#include "YSLib/Service/yblit.h"

namespace YSLib
{

using namespace Drawing;

namespace UI
{

void
SolidBrush::operator()(PaintEventArgs&& e)
{
	Drawing::FillRect(e.Target, e.ClipArea, Color);
}


void
ImageBrush::operator()(PaintEventArgs&& e)
{
	YAssert(bool(Update), "Null updater found.");

	if(ImagePtr)
		Update(e, *ImagePtr, DstOffset, SrcOffset);
}

void
ImageBrush::DefaultUpdate(const PaintContext& pc, const Image& img,
	const Point& dst_offset, const Point& src_offset)
{
	const auto& g(pc.Target);
	const Rect& r(pc.ClipArea);
	const auto& src(img.GetContext());

//	CopyTo(g.GetBufferPtr(), src, g.GetSize(), r.GetPoint(), Offset,
//		r.GetSize());
	BlitLines<false, false>(CopyLine<true>(), g.GetBufferPtr(),
		src.GetBufferPtr(), g.GetSize(), src.GetSize(),
		r.GetPoint() + dst_offset, src_offset,
		{r.Width - dst_offset.X, r.Height - dst_offset.Y});
}

} // namespace UI;

} // namespace YSLib;

