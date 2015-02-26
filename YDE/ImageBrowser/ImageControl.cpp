﻿/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ImageControl.cpp
\ingroup UI
\brief 图像显示控件。
\version r1040
\author FrankHB <frankhb1989@gmail.com>
\since build 436
\par 创建时间:
	2013-08-13 12:48:27 +0800
\par 修改时间:
	2015-02-26 19:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	ImageBrowser::ImageControl
*/


#include "ImageControl.h"
#include YFM_YSLib_Service_YPixel

namespace ImageBrowser
{

ImagePanel::ImagePanel(const Rect& r, const Size& min_size,
	const Size& max_size)
	: Panel(r),
	min_panel_size(min_size), max_panel_size(max_size), btnClose({{}, 24, 24}),
	hover_state(std::bind(TimedHoverState::LocateForOffset,
	std::placeholders::_1, Point(0, 24))), border(*this, 8, min_size),
	mnuContext({}, make_shared<Menu::ListType, String>(
	{u"退出", u"翻转", u"顺时针旋转", u"逆时针旋转"}))
{
	*this += btnClose,
	Host::SetupTimedTips(hover_state, btnClose, lblCloseTips, u"关闭"),
	yunseq(
	Background = SolidBrush({0x00, 0x00, 0x00, 0xC0}),
	btnClose.Background = [this](PaintEventArgs&& e){
		const auto& g(e.Target);
		const auto& pt(e.Location);
		auto& r(e.ClipArea);

		if(hover_updater.IsEntered())
			FillCircle(g, r, pt + Vec(12, 12), 10, {225, 96, 76});
		DrawCross(g, e.ClipArea, {pt + Vec(4, 4), {16, 16}}, ColorSpace::White);
		r = Rect(pt, GetSizeOf(e.GetSender()));
	},
	mnuContext.Confirmed += [this](IndexEventArgs&& e){
		switch(e.Value)
		{
		case 0:
			YSLib::PostQuitMessage(0);
			break;
		case 1:
			Flip();
			UpdateBrush();
			break;
		case 2:
			RotateCW();
			UpdateBrush();
			break;
		case 3:
			RotateCCW();
			UpdateBrush();
		default:
			break;
		}
	},
	FetchEvent<Resize>(*this) += [this]{
		SetLocationOf(btnClose, CalcCloseButtonLocation());
		if(session_ptr)
		{
			GetPagesRef().Resize(GetSizeOf(*this));
			Invalidate(*this);
		}
	},
	FetchEvent<Click>(*this) += [this](CursorEventArgs&& e){
		if(session_ptr && e.Keys[KeyCodes::Tertiary] && GetPagesRef().Zoom(
			round((1 - GetPagesRef().GetScale()) * 100), e))
			Invalidate(*this);
	},
	FetchEvent<CursorWheel>(*this) += [this](CursorWheelEventArgs&& e){
		if(session_ptr
			&& GetPagesRef().ZoomByRatio(e.GetDelta() > 0 ? 1.2F : 0.8F, e))
			Invalidate(*this);
	},
	FetchEvent<Paint>(*this).Add([this](PaintEventArgs&& e){
		if(session_ptr)
			GetPagesRef().Brush(std::move(e));
	}, BackgroundPriority),
	FetchEvent<Click>(btnClose) += []{
		YSLib::PostQuitMessage(0);
	}
	);
	SetLocationOf(btnClose, CalcCloseButtonLocation());
}

void
ImagePanel::Load(ImagePages&& src)
{
	session_ptr.reset(new Session(forward_as_tuple(std::move(src),
		GAnimationSession<InvalidationUpdater>(), Timers::Timer(),
		vector<std::chrono::milliseconds>())));
	auto& pages(GetPagesRef());

	SetSizeOf(*this, pages.GetViewSize());
	Invalidate(*this);

	// TODO: Check "Loop" metadata.
	const auto& bmps(pages.GetBitmaps());
	const auto n(bmps.size());
	auto& frame_delays(get<3>(*session_ptr));

	YTraceDe(Notice, "Loaded page count = %u.", unsigned(n));
	if(n > 1)
	{
		frame_delays.reserve(n);
		for(const auto& bmp : bmps)
		{
			// TODO: Allow user set minimal frame time.
			auto d(std::chrono::milliseconds(20));

			TryExpr(d = YSLib::max(GetFrameTimeOf(bmp), d))
			CatchExpr(LoggedEvent& e,
				YTraceDe(e.GetLevel(), "Invalid frame time found."))
			YTraceDe(Informative, "Loaded frame time = %s milliseconds.",
				to_string(d.count()).c_str());
			frame_delays.push_back(d);
		}

		const auto refresh_frame([this]{
			const auto d(get<3>(*session_ptr).at(GetPagesRef().GetIndex()));

			YTraceDe(Informative, "Set frame time = %s ms.",
				to_string(d.count()).c_str());
			get<2>(*session_ptr).Interval = d;
		});

		refresh_frame();
		UI::SetupByTimer(get<1>(*session_ptr), *this, get<2>(*session_ptr), [=]{
			if(GetPagesRef().SwitchPageDiff(1))
			{
				refresh_frame();
				Invalidate(*this);
			}
		});
	}
}

void
ImagePanel::SetupContextMenu()
{
	ResizeForContent(mnuContext);
	Host::SetupTopLevelContextMenu(mhMain, mnuContext, *this, *this, true);
}

void
ImagePanel::Unload()
{
	session_ptr.reset();
}

void
ImagePanel::UpdateBrush()
{
	using namespace Drawing::Shaders;
	auto& update(GetPagesRef().Brush.Update);

	switch(rot)
	{
	case RDeg0:
		update = ImageBrush::UpdateComposite;
		break;
	case RDeg90:
 		update = UpdateRotatedBrush<RDeg90>;
		break;
	case RDeg180:
		update = UpdateRotatedBrush<RDeg180>;
		break;
	case RDeg270:
		update = UpdateRotatedBrush<RDeg270>;
	default:
		break;
	}
	Invalidate(*this);
}

} // namespace ImageBrowser;

