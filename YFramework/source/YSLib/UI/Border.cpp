﻿/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Border.cpp
\ingroup UI
\brief 图形用户界面边框。
\version r140
\author FrankHB <frankhb1989@gmail.com>
\since build 443
\par 创建时间:
	2013-09-06 23:25:42 +0800
\par 修改时间:
	2013-09-11 08:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::Border
*/


#include "YSLib/UI/Border.h"
#include "YSLib/Service/ydraw.h"
#include "YSLib/UI/ygui.h"
#include "YSLib/UI/ycontrol.h"

namespace YSLib
{

using namespace Drawing;

namespace UI
{

BorderStyle::BorderStyle()
	: ActiveColor(FetchGUIState().Colors[Styles::ActiveBorder]),
	InactiveColor(FetchGUIState().Colors[Styles::InactiveBorder])
{}


void
BorderBrush::operator()(PaintEventArgs&& e)
{
	auto& sender(e.GetSender());

	DrawRect(e.Target, Rect(e.Location, GetSizeOf(sender)), IsFocused(sender)
		? Style.ActiveColor : Style.InactiveColor);
}


BorderResizer::Area
BorderResizer::CheckArea(const Point& pt)
{
	const auto comp([](SPos q, SPos left, SPos left2){
		if(q >= 0)
		{
			const SPos a[]{SPos(), left, left2};
			const auto n(SwitchInterval(q, a, 3));

			if(n < 3)
				return BorderArea(n);
		}
		return BorderArea::Center;
	});
	const Size& size(GetSizeOf(widget));

	return {comp(pt.X, Margin.Left, size.Width - GetHorizontalOf(Margin)),
		comp(pt.Y, Margin.Top, size.Height - GetVerticalOf(Margin))};
}

void
BorderResizer::Wrap()
{
	auto& controller(widget.get().GetController());
	yunseq(
	FetchEvent<TouchDown>(controller).Add([this](CursorEventArgs&& e){
		yunseq(orig_loc = FetchGUIState().ControlLocation,
			orig_bounds = GetBoundsOf(widget), focused = CheckArea(e));
	}, 0xE0),
	FetchEvent<TouchHeld>(controller).Add([this](CursorEventArgs&& e){
		if(e.Strategy == RoutedEventArgs::Direct
			&& focused != Area(BorderArea::Center, BorderArea::Center))
		{
			auto& st(FetchGUIState());

			if(st.CheckDraggingOffset())
			{
				const auto offset(st.ControlLocation - orig_loc);
				auto bounds(orig_bounds);

				switch(focused.first)
				{
				case BorderArea::Left:
					bounds.Width = max<SPos>(MinSize.Width,
						orig_bounds.Width - offset.X);
					bounds.X += orig_bounds.Width - bounds.Width;
					break;
				case BorderArea::Right:
					bounds.Width = max<SPos>(MinSize.Width,
						orig_bounds.Width + offset.X);
					break;
				default:
					;
				}
				switch(focused.second)
				{
				case BorderArea::Up:
					bounds.Height = max<SPos>(MinSize.Height,
						orig_bounds.Height - offset.Y);
					bounds.Y += orig_bounds.Height - bounds.Height;
					break;
				case BorderArea::Down:
					bounds.Height = max<SPos>(MinSize.Height,
						orig_bounds.Height + offset.Y);
					break;
				default:
					;
				}

				YTraceDe(Notice, "BorderResizer: new bounds = %s.\n",
					to_string(bounds).c_str());

				InvalidateParent(widget),
				SetBoundsOf(widget, bounds);
			}
			e.Handled = true;
			// XXX: Paint context target invalidated.
		}
	}, 0xE0),
	FetchEvent<Click>(controller).Add([this](CursorEventArgs&& e){
		CallEvent<ClickAcross>(widget, e);
	}, 0xE0),
	FetchEvent<ClickAcross>(controller).Add([this](CursorEventArgs&&){
		yunseq(orig_loc = Point::Invalid, orig_bounds = Rect(),
			focused = {BorderArea::Center, BorderArea::Center});
	}, 0xE0)
	);
}

} // namespace UI;

} // namespace YSLib;

