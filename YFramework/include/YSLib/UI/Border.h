﻿/*
	© 2013-2015, 2018, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Border.h
\ingroup UI
\brief 图形用户界面边框。
\version r196
\author FrankHB <frankhb1989@gmail.com>
\since build 443
\par 创建时间:
	2013-09-06 23:23:56 +0800
\par 修改时间:
	2020-05-12 15:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Border
*/


#ifndef YSL_INC_UI_Border_h_
#define YSL_INC_UI_Border_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YComponent
#include YFM_YSLib_Service_YBrush

namespace YSLib
{

namespace UI
{

/*!
\brief 边框样式。
\warning 非虚析构。
\since build 276
*/
class YF_API BorderStyle
{
public:
	Color ActiveColor, InactiveColor;

	BorderStyle();
	//! \since build 584
	BorderStyle(Color c)
		: BorderStyle(c, c)
	{}
	//! \since build 584
	BorderStyle(Color a, Color i)
		: ActiveColor(a), InactiveColor(i)
	{}
};


/*!
\brief 边框画刷。
\warning 非虚析构。
\since build 284
*/
class YF_API BorderBrush
{
public:
	/*!
	\brief 边框样式。
	\since build 295
	*/
	BorderStyle Style;

	DefDeCtor(BorderBrush)
	/*!
	\brief 复制构造：默认实现。
	\since build 297
	*/
	DefDeCopyCtor(BorderBrush)
	/*!
	\brief 初始化；使用指定边框样式。
	\since build 295
	*/
	BorderBrush(const BorderStyle& style)
		: Style(style)
	{}

	DefDeCopyAssignment(BorderBrush)

	//! \since build 848
	void
	operator()(PaintEventArgs&&) const;
};


/*!
\brief 边框区域。
\since build 444
*/
enum class BorderArea : std::uint8_t
{
	Left = 0,
	Up = 0,
	Center = 1,
	Right = 2,
	Down = 2
};


/*!
\brief 调整大小的边框。
\note 使用相对指定部件的坐标系。
\since build 444

当接触点位于 Margin 指定的边界上时 TouchHeld 使用 SetBoundsOf 按内部状态调整大小
（满足大小不小于MinSize ）并引起 Move 和 Resize 事件。
*/
class YF_API BorderResizer
{
public:
	//! \brief 区域类型。
	using Area = pair<BorderArea, BorderArea>;

private:
	Point orig_loc{Point::Invalid};
	/*!
	\brief 锁定的部件边界。
	\note 以此为基准调整部件的新的位置和大小。
	\since build 445
	*/
	Rect locked_bounds{};
	Area focused{BorderArea::Center, BorderArea::Center};
	//! \since build 554
	lref<IWidget> widget;

public:
	/*!
	\brief 根模式：指定部件为通过外部接口和宿主交互的顶层部件。
	\note 部件的实际位置需要另行修正。
	\since build 571

	指定部件是否在设置边界后同时调整锁定的边界位置。
	*/
	bool RootMode = {};
	//! \brief 指定边框相对部件边界以内的范围的边距。
	Drawing::Padding Margin;
	/*!
	\brief 指定部件应该保持的最小大小。
	\warning 若小于边框范围（边框重叠）则不保证通过边框调整大小的行为确定。
	*/
	Size MinSize;

	BorderResizer(IWidget& wgt, size_t w = 4, const Size& min_size = {})
		: widget(wgt), Margin(SPos(w)),
		MinSize(min_size.Width + w + w, min_size.Height + w + w)
	{
		Wrap();
	}

	DefGetter(const ynothrow, IWidget&, WidgetRef, widget)

	/*!
	\brief 检查相对部件点指定区域。
	\since build 889
	*/
	YB_ATTR_nodiscard YB_PURE Area
	CheckArea(const Point&) const;

private:
	/*!
	\brief 加载事件。
	\note 优先级 E0 。
	*/
	void
	Wrap();
};

} // namespace UI;

} // namespace YSLib;

#endif

