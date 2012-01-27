﻿/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file TextLayout.h
\ingroup Service
\brief 文本布局计算。
\version r7389;
\author FrankHB<frankhb1989@gmail.com>
\since build 275 。
\par 创建时间:
	2009-11-13 00:06:05 +0800;
\par 修改时间:
	2012-01-27 07:02 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Service::TextLayout;
*/


#ifndef YSL_INC_SERVICE_TEXTLAYOUT_H_
#define YSL_INC_SERVICE_TEXTLAYOUT_H_

#include "TextRenderer.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 取指定文本状态和文本区域高调整的底边距。
\pre 断言： <tt>GetTextLineHeightExOf(ts) != 0</tt> 。
\post <tt>ts.Margin.Bottom</tt> 不小于原值。
\return 返回调整后的底边距值（由字体大小、行距和高决定）。
\since build 252 。
*/
SDst
FetchResizedBottomMargin(const TextState&, SDst);

/*!
\brief 取指定文本状态和文本区域高所能显示的最大文本行数。
\pre 断言： <tt>GetTextLineHeightExOf(ts) != 0</tt> 。
\return 最大能容纳的文本行数。
\since build 252 。
*/
u16
FetchResizedLineN(const TextState& ts, SDst);

/*!
\brief 取指定文本状态在指定高的区域中表示的最底行的基线位置（纵坐标）。
\note 若不足一行则最底行视为首行。
\warning 不检查边距正确性。若顶边距正确，则返回值应小于输入的高。
\since build 190 。
*/
SPos
FetchLastLineBasePosition(const TextState&, SDst);


/*!
\brief 取按字体高度和行距调整文本区域的底边距。
\since build 252 。
*/
inline SDst
FetchResizedBottomMargin(const TextRegion& tr)
{
	return FetchResizedBottomMargin(tr, tr.GetHeight());
}

/*!
\brief 按字体高度和行距调整文本区域的底边距。
\since build 252 。
*/
inline SDst
AdjustBottomMarginOf(TextRegion& tr)
{
	return tr.Margin.Bottom = FetchResizedBottomMargin(tr);
}


/*!
\brief 取指定的字符使用指定字体的显示宽度。
\note 无边界限制。
\since build 280 。
*/
SDst
FetchCharWidth(const Font&, ucs4_t);


/*!
\brief 取迭代器指定的字符串在字体指定、无边界限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
\since build 214 。
*/
template<typename _tIn>
SDst
FetchStringWidth(const Font& fnt, _tIn s)
{
	SDst w(0);

	while(*s != '\0')
		w += FetchCharWidth(fnt, *s++);
	return w;
}
/*!
\brief 取迭代器指定的单行字符串在字体指定、无边界限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
\since build 251 。
*/
template<typename _tIn>
SDst
FetchStringWidth(const Font& fnt, _tIn s, _tIn g, ucs4_t c = '\0')
{
	SDst w(0);

	while(s != g && *s != c)
		w += FetchCharWidth(fnt, *s++);
	return w;
}
/*!
\brief 取单行字符串在字体指定、无边界限制时的显示宽度。
\since build 214 。
*/
inline SDst
FetchStringWidth(const Font& fnt, const String& str)
{
	return FetchStringWidth(fnt, str.c_str());
}
/*!
\brief 取迭代器指定的单行字符串在指定文本状态和高度限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至字符串结束符。
\note 字体由文本状态指定。
\since build 197 。
*/
template<typename _tIn>
SDst
FetchStringWidth(TextState& ts, SDst h, _tIn s)
{
	const SPos x(ts.PenX);
	EmptyTextRenderer r(ts, h);

	PrintString(r, s);
	return ts.PenX - x;
}
/*!
\brief 取迭代器指定的单行字符串在指定文本状态和高度限制时的显示宽度。
\note 迭代器 s 指向字符串首字符，迭代直至边界迭代器 g 或指定字符 c 。
\note 字体由文本状态指定。
\since build 251 。
*/
template<typename _tIn>
SDst
FetchStringWidth(TextState& ts, SDst h, _tIn s, _tIn g, ucs4_t c = '\0')
{
	const SPos x(ts.PenX);
	EmptyTextRenderer r(ts, h);

	PrintString(r, s, g, c);
	return ts.PenX - x;
}
/*!
\brief 取单行字符串在指定文本状态和高度限制时的显示宽度。
\note 字体由文本状态指定。
\since build 197 。
*/
inline SDst
FetchStringWidth(TextState& ts, SDst h, const String& str)
{
	return FetchStringWidth(ts, h, str.c_str());
}

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

