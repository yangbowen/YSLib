﻿/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textlist.cpp
\ingroup UI
\brief 样式相关的文本列表。
\version r1796
\author FrankHB <frankhb1989@gmail.com>
\since build 214
\par 创建时间:
	2011-04-20 09:28:38 +0800
\par 修改时间:
	2014-08-14 05:04 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::TextList
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_UI_TextList
#include YFM_YSLib_UI_YWindow
#include YFM_YSLib_UI_Border
#include YFM_YSLib_Service_YBlit
#include YFM_YSLib_Service_TextLayout

namespace YSLib
{

using namespace Drawing;

namespace UI
{

SDst
AMUnitList::GetFullViewHeight() const
{
	return GetItemHeight() * GetTotal();
}
SDst
AMUnitList::GetItemHeight() const
{
	return GetSizeOf(GetUnitRef()).Height;
}
Point
AMUnitList::GetUnitLocation(size_t idx) const
{
	// XXX: Conversion to 'SPos' might be implementation-defined.
	return Point(0, -SPos(uTopOffset) + SPos(GetItemHeight()) * (SPos(idx)
		- SPos(vwList.GetHeadIndex())));
}
SDst
AMUnitList::GetViewPosition() const
{
	return GetItemHeight() * vwList.GetHeadIndex() + uTopOffset;
}

SDst
AMUnitList::AdjustBottomForHeight(SDst item_h, SDst h)
{
	const auto d(uTopOffset);

	uTopOffset = 0;
	AdjustViewLengthForHeight(item_h, h);
	return d;
}

SDst
AMUnitList::AdjustOffsetForHeight(SDst h, bool is_top)
{
	if(GetFullViewHeight() > h)
	{
		const SDst ln_h(GetItemHeight());

		if(YB_UNLIKELY(ln_h == 0))
			return 0;
		vwList.RestrictSelected();
		if(is_top)
			return AdjustBottomForHeight(ln_h, h);
		else
		{
			const auto d((h + uTopOffset) % ln_h);

			if(d != 0)
			{
				const auto tmp(uTopOffset + ln_h - d);

				uTopOffset = tmp % ln_h;
				AdjustViewLengthForHeight(ln_h, h);
				vwList.IncreaseHead(tmp / ln_h, GetTotal());
			}
			return d;
		}
	}
	return 0;
}

void
AMUnitList::AdjustViewForContent(SDst h)
{
	if(vwList.AdjustForContent(GetTotal()) && vwList.IsSelected())
	{
		AdjustOffsetForHeight(h,
			vwList.GetSelectedIndex() == vwList.GetHeadIndex());
		return;
	}
	if(GetFullViewHeight() < GetViewPosition() + h)
		uTopOffset = 0;
	AdjustViewLengthForHeight(GetItemHeight(), h);
}

void
AMUnitList::AdjustViewLengthForHeight(SDst item_h, SDst h)
{
	if(YB_LIKELY(item_h != 0 && h != 0))
		vwList.Length = h / item_h + (uTopOffset != 0 || h % item_h != 0);
}

void
AMUnitList::ResetView()
{
	bool b(vwList.IsSelected());

	vwList.Reset();
	if(b)
		vwList.SetSelectedIndex(0, GetTotal());
	uTopOffset = 0;
}


MTextList::MTextList(const shared_ptr<ListType>& h)
	: AMUnitList(),
	hList(h ? h : make_shared<ListType>())
{
	Unit.Margin = Padding(2, 2, 1, 1),
	Unit.SetHeight(Unit.Font.GetHeight() + GetVerticalOf(Unit.Margin));
}

size_t
MTextList::GetLastLabelIndexClipped(SPos v_off, SDst height) const
{
	// XXX: Conversion to 'SPos' might be implementation-defined.
	return vwList.GetHeadIndex() + min<size_t>((SPos(height + uTopOffset)
		- v_off - 1) / SPos(GetItemHeight()) + 1,
		vwList.GetValid(GetList().size()));
}
MTextList::ItemType*
MTextList::GetItemPtr(const IndexType& idx)
{
	auto& lst(GetListRef());

	return IsInInterval<IndexType>(idx, lst.size()) ? &lst[idx] : nullptr;
}
SDst
MTextList::GetItemHeight() const
{
	const auto item_h(Unit.Font.GetHeight() + GetVerticalOf(Unit.Margin));

	YAssert(item_h != 0, "Invalid item height found.");
	return item_h;
}
const MTextList::ItemType*
MTextList::GetItemPtr(const IndexType& idx) const
{
	const auto& lst(GetList());

	return IsInInterval<IndexType>(idx, lst.size()) ? &lst[idx] : nullptr;
}
SDst
MTextList::GetTotal() const
{
	return GetList().size();
}
IWidget&
MTextList::GetUnitRef() const
{
	return Unit;
}
SDst
MTextList::GetMaxTextWidth() const
{
	return FetchMaxTextWidth(Unit.Font, GetList().cbegin(), GetList().cend());
}

void
MTextList::SetList(const shared_ptr<ListType>& h)
{
	if(YB_LIKELY(h))
		hList = h;
}

WidgetIterator
MTextList::MakeIterator(size_t item_idx)
{
	return ystdex::make_prototyped_iterator(Unit, item_idx,
		[this](IWidget& wgt, size_t idx){
		if(idxShared != idx && idx < GetList().size())
			yunseq(Unit.Text = GetList()[idx], idxShared = idx),
			SetLocationOf(wgt, GetUnitLocation(idx));
	});
}


TextList::TextList(const Rect& r, const shared_ptr<ListType>& h,
	const pair<Color, Color>& hilight_pair)
	: Control(r, MakeBlankBrush()), MTextList(h), MHilightText(hilight_pair)
{
	const auto invalidator([this]{
		Invalidate(*this);
	});

	SetContainerPtrOf(Unit, this);
	SetSizeOf(Unit, {GetWidth(), GetItemHeight()});
	yunseq(
	FetchEvent<KeyDown>(*this) += [this](KeyEventArgs&& e){
		if(!GetList().empty())
		{
			using namespace KeyCodes;
			const auto& k(e.GetKeys());

			if(k.count() != 1)
				return;
			if(k[Up] || k[Down] || k[PgUp] || k[PgDn])
			{
				const auto old_sel(vwList.GetSelectedIndex());
				const auto old_off(vwList.GetOffset());
				const auto old_hid(vwList.GetHeadIndex());
				const auto old_top(uTopOffset);

				{
					const bool up(k[Up] || k[PgUp]);

					if(vwList.IsSelected())
					{
						vwList.IncreaseSelected((up ? -1 : 1) * (k[Up]
							|| k[Down] ? 1 : GetHeight() / GetItemHeight()),
							GetList().size());
						if(old_sel == vwList.GetSelectedIndex()
							&& CyclicTraverse)
							goto bound_select;
						if(vwList.GetOffset()
							== (up ? 0 : ptrdiff_t(vwList.Length - 1)))
							AdjustOffsetForHeight(GetHeight(), up);
					}
					else
bound_select:
						up ? SelectLast() : SelectFirst();
				}

				const auto new_off(vwList.GetOffset());

				if(vwList.GetSelectedIndex() != old_sel)
					CallSelected();
				if(old_top != uTopOffset || vwList.GetHeadIndex() != old_hid)
					UpdateView(*this);
				else if(old_off != new_off)
					InvalidateSelected2(old_off, new_off);
			}
			else if(vwList.IsSelected())
			{
				// NOTE: Do not confuse with %UI::Enter.
				if(k[KeyCodes::Enter])
					InvokeConfirmed(vwList.GetSelectedIndex());
				else if(k[Esc])
				{
					InvalidateSelected(vwList.GetOffset());
					ClearSelected();
				// TODO: Create new event for canceling selection.
					CallSelected();
				}
			}
		}
	},
	FetchEvent<KeyHeld>(*this) += OnKeyHeld,
	FetchEvent<TouchDown>(*this) += [this](CursorEventArgs&& e){
		SetSelected(e);
		UpdateView(*this);
	},
	FetchEvent<TouchHeld>(*this) += [this](CursorEventArgs&& e){
		if(&e.GetSender() == this)
		{
			SetSelected(e);
			UpdateView(*this);
		}
	},
	FetchEvent<Click>(*this) += [this](CursorEventArgs&& e){
		InvokeConfirmed(CheckPoint(e));
	},
	FetchEvent<Paint>(*this).Add(BorderBrush(), BoundaryPriority),
	FetchEvent<GotFocus>(*this) += invalidator,
	FetchEvent<LostFocus>(*this) += invalidator,
	Unit.Background = [this](PaintEventArgs&& e){
		if(vwList.CheckSelected(idxShared))
			Unit.ForeColor = HilightTextColor,
			FillRect(e.Target, e.ClipArea, Rect(e.Location.X + 1, e.Location.Y,
				Unit.GetWidth() - 2, Unit.GetHeight()),
				HilightBackColor);
		else
			Unit.ForeColor = ForeColor;
	}
	);
	AdjustViewLength();
}

void
TextList::SetList(const shared_ptr<ListType>& h)
{
	if(h)
	{
		MTextList::SetList(h);
		AdjustViewLength();
	}
}

size_t
TextList::GetLastLabelIndex() const
{
	return GetLastLabelIndexClipped(0, GetHeight());
}

void
TextList::SetSelected(ListType::size_type i)
{
	if(vwList.Contains(i))
	{
		const auto old_off(vwList.GetOffset());

		if(vwList.SetSelectedIndex(i, GetList().size()))
		{
			CallSelected();
			InvalidateSelected2(old_off, vwList.GetOffset());
		}
	}
}
void
TextList::SetSelected(SPos x, SPos y)
{
	SetSelected(CheckPoint(x, y));
}

void
TextList::CallSelected()
{
	Selected(IndexEventArgs(*this, vwList.GetSelectedIndex()));
}

TextList::ListType::size_type
TextList::CheckPoint(SPos x, SPos y)
{
	return Rect(GetSizeOf(*this)).Contains(x, y) ? (y + uTopOffset)
		/ GetItemHeight() + vwList.GetHeadIndex() : ListType::size_type(-1);
}

void
TextList::InvalidateSelected(ListType::difference_type offset,
	ListType::size_type n)
{
	if(offset >= 0 && n != 0)
	{
		const auto ln_h(GetItemHeight());
		Rect r(0, ln_h * offset - uTopOffset, GetWidth(), ln_h * n);

		if(r.Y < 0 || SDst(r.Y) < GetHeight())
		{
			r.Y = max<SPos>(0, r.Y);
			RestrictUnsignedStrict(r.Height, GetHeight() - r.Y);
			Invalidate(*this, r);
		}
	}
}

void
TextList::InvalidateSelected2(ListType::difference_type x,
	ListType::difference_type y)
{
	if(y < x)
		std::swap(x, y);
	InvalidateSelected(x < 0 ? 0 : x, y - x + 1);
}

void
TextList::InvokeConfirmed(ListType::size_type idx)
{
	if(CheckConfirmed(idx))
		Confirmed(IndexEventArgs(*this, idx));
}

void
TextList::LocateViewPosition(SDst h)
{
	SDst fvh(GetFullViewHeight());
	const SDst height(GetHeight());

	if(fvh <= height)
		return;
	RestrictUnsignedStrict(h, fvh - height - 1);
	if(GetViewPosition() != h)
	{
		const SDst item_height(GetItemHeight());

		//先保证避免部分显示的项目使视图超长，再设置视图位置。
		AdjustViewLength();
		vwList.SetHeadIndex(h / item_height, GetList().size());
		uTopOffset = h % item_height;
		//更新视图。
		UpdateView(*this, true);
	}
}

void
TextList::Refresh(PaintEventArgs&& e)
{
	const auto h(GetHeight());

	if(h != 0)
	{
		const Rect& bounds(e.ClipArea);

		if(!bounds.IsUnstrictlyEmpty())
		{
			if(!GetList().empty() && bool(bounds))
			{
				// NOTE: View length could be already changed by contents.
				AdjustViewLength();
				SetSizeOf(Unit, {GetWidth(), GetItemHeight()});
				for(WidgetIterator first(MakeIterator(GetHeadIndex())),
					last(MakeIterator(GetLastLabelIndexClipped(e.Location.Y
					- bounds.Y, bounds.Height))); first != last; ++first)
					PaintVisibleChildAndCommit(*first, e);
			}
		}
	}
}

void
TextList::ResetView()
{
	MTextList::ResetView();
	UpdateView(*this);
}

void
TextList::SelectFirst()
{
	vwList.SetSelectedIndex(0, GetList().size());
	AdjustOffsetForHeight(GetHeight(), true);
}

void
TextList::SelectLast()
{
	const auto s(GetList().size());

	vwList.SetSelectedIndex(s - 1, s);
	AdjustOffsetForHeight(GetHeight(), {});
}


void
ResizeForContent(TextList& tl)
{
	SetSizeOf(tl, Size(tl.GetMaxTextWidth() + GetHorizontalOf(tl.Unit.Margin),
		tl.GetFullViewHeight()));
	tl.AdjustViewLength();
}

void
UpdateView(TextList& tl, bool is_active)
{
	tl.ViewChanged(TextList::ViewArgs(tl, is_active));
	tl.AdjustViewLength();
	Invalidate(tl);
}

} // namespace UI;

} // namespace YSLib;

