﻿/*
	© 2012-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Lexical.cpp
\ingroup NPL
\brief NPL 词法处理。
\version r1954
\author FrankHB <frankhb1989@gmail.com>
\since build 335
\par 创建时间:
	2012-08-03 23:04:26 +0800
\par 修改时间:
	2020-03-22 15:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Lexical
*/


#include "NPL/YModules.h"
#include YFM_NPL_Lexical // for YSLib::octet;
#include <ystdex/string.hpp> // for ystdex::get_mid;

namespace NPL
{

bool
HandleBackslashPrefix(string_view buf, UnescapeContext& uctx)
{
	YAssert(!buf.empty(), "Invalid buffer found.");
	if(!uctx.IsHandling() && buf.back() == '\\')
	{
		yunseq(uctx.Start = buf.length() - 1, uctx.Length = 1);
		return true;
	}
	return {};
}

bool
NPLUnescape(string& buf, char c, UnescapeContext& uctx, char ld)
{
	if(uctx.Length == 1)
	{
		uctx.VerifyBufferLength(buf.length());

		const auto i(uctx.Start);

		YAssert(i == buf.length() - 1, "Invalid buffer found.");
		switch(c)
		{
		case '\\':
			buf[i] = '\\';
			break;
		case 'a':
			buf[i] = '\a';
			break;
		case 'b':
			buf.pop_back();
			break;
		case 'f':
			buf[i] = '\f';
			break;
		case 'n':
			buf[i] = '\n';
			break;
		case 'r':
			buf[i] = '\r';
			break;
		case 't':
			buf[i] = '\t';
			break;
		case 'v':
			buf[i] = '\v';
			break;
		case '\n':
			buf.pop_back();
			break;
		case '\'':
		case '"':
			if(c == ld)
			{
				buf[i] = ld;
				break;
			}
			YB_ATTR_fallthrough;
		default:
			uctx.Clear();
			buf += c;
			return {};
		}
		uctx.Clear();
		return true;
	}
	buf += c;
	return {};
}


LexicalAnalyzer::DefDeCtor(LexicalAnalyzer)
LexicalAnalyzer::LexicalAnalyzer(pmr::memory_resource& rsrc)
	: cbuf(string::allocator_type(&rsrc)),
	qlist(vector<size_t>::allocator_type(&rsrc))
{}

void
LexicalAnalyzer::ReplaceBack(char c)
{
	YAssert(!cbuf.empty(), "Invalid buffer found.");
	[&]() YB_FLATTEN{
		switch(c)
		{
			case char():
				cbuf.pop_back();
				break;
			case '\'':
			case '"':
				if(ld == char())
				{
					ld = c;
					qlist.push_back(cbuf.size() - 1);
				}
				else if(ld == c)
				{
					ld = char();
					qlist.push_back(cbuf.size());
				}
				break;
			// XXX: Case ' ' is equivalent in the default branch.
		//	case ' ':
			case '\f':
			case '\n':
			// XXX: Case '\r' is ignored.
		//	case '\r':
			case '\t':
			case '\v':
				if(ld == char())
				{
					cbuf.back() = ' ';
					break;
				}
				YB_ATTR_fallthrough;
			default:
				break;
		}
	}();
}

TokenViewList
LexicalAnalyzer::Literalize() const
{
	size_t i(0);
	TokenViewList result(cbuf.get_allocator());

	std::for_each(qlist.cbegin(), qlist.cend(), [&](size_t s){
		if(s != i)
		{
			result.push_back(TokenViewList::value_type(&cbuf[i], s - i));
			i = s;
		}
	});
	result.push_back(TokenViewList::value_type(&cbuf[i], cbuf.size() - i));
	return result;
}


char
CheckLiteral(string_view sv) ynothrowv
{
	YAssertNonnull(sv.data());
	if(sv.length() > 1)
		if(const char c = ystdex::get_quote_mark_nonstrict(sv))
		{
			if(c == '\'' || c == '"')
				return c;
		}
	return {};
}

string_view
Deliteralize(string_view sv) ynothrowv
{
	return CheckLiteral(sv) != char() ? DeliteralizeUnchecked(sv) : sv;
}

string
Escape(string_view sv)
{
	YAssertNonnull(sv.data());

	char last{};
	string res;

	res.reserve(sv.length());
	for(char c : sv)
	{
		char unescaped{};

		switch(c)
		{
		case '\a':
			unescaped = 'a';
			break;
		case '\b':
			unescaped = 'b';
			break;
		case '\f':
			unescaped = 'f';
			break;
		case '\n':
			unescaped = 'n';
			break;
		case '\r':
			unescaped = 'r';
			break;
		case '\t':
			unescaped = 't';
			break;
		case '\v':
			unescaped = 'v';
			break;
		case '\'':
		case '"':
			unescaped = c;
		}
		if(last == '\\')
		{
			if(c == '\\')
			{
				yunseq(last = char(), res += '\\');
				continue;
			}
			switch(c)
			{
			case 'a':
			case 'b':
			case 'f':
			case 'n':
			case 'r':
			case 't':
			case 'v':
			case '\'':
			case '"':
				res += '\\';
			}
		}
		if(unescaped == char())
			res += c;
		else
		{
			res += '\\';
			res += unescaped;
			unescaped = char();
		}
		last = c;
	}
	return res;
}

string
EscapeLiteral(string_view sv)
{
	const char c(CheckLiteral(sv));
	// TODO: Use %get_mid for %string_view.
	auto content(Escape(c == char() ? sv : ystdex::get_mid(string(sv))));

	if(!content.empty() && content.back() == '\\')
		content += '\\';
	return c == char() ? std::move(content) : ystdex::quote(content, c);
}

string
EscapeXML(string_view sv)
{
	YAssertNonnull(sv.data());

	string res;

	res.reserve(sv.length());
	for(char c : sv)
		switch(c)
		{
		case '\0':
			YTraceDe(YSLib::Warning, "Invalid character '#x%X' found, ignored.",
				unsigned(c));
			break;
		case '&':
			res += "&amp;";
			break;
		case '<':
			res += "&lt;";
			break;
		case '>':
			res += "&gt;";
			break;
		default:
			res += c;
		}
	return res;
}

// XXX: The code generated by x86_64-pc-linux G++ 8.3 at '-O3' is wrong. This
//	seems not filed at https://gcc.gnu.org/bugzilla.
// XXX: There is no need to specifiy GCC version because the lowest version
//	supported in YFramework is more recent. See
//	https://gcc.gnu.org/onlinedocs/gcc-4.4.0/gcc/Function-Attributes.html#Function-Attributes.
#if (YB_IMPL_GNUCPP < 90100 && !YB_IMPL_CLANGPP) && YCL_Linux && \
	defined(NDEBUG) && __OPTIMIZE__ && !__OPTIMIZE_SIZE__
// XXX: As there is no way to easily get the optimization level, leave all
//	optimized builds as '-O2'. All '-O2' options plus all documented additional
//	options for '-O3' or options used in 'gcc/opts.c' in trunk code as of
//	writing (2019-05-16, also cf.
//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81191) does not reappear the
//	bug, but as now just use '-O2' instead.
YB_ATTR(optimize("O2"))
#endif
string
Literalize(string_view sv, char c)
{
	return CheckLiteral(sv) == char() && c != char()
		? ystdex::quote(string(sv), c) : string(sv);
}


TokenList
Decompose(string_view src, TokenList::allocator_type a)
{
	YAssertNonnull(src.data());

	TokenList dst(a);
	using iter_type = typename string_view::const_iterator;

	ystdex::split_l(src.cbegin(), src.cend(), IsDelimiter,
		// TODO: Blocked. Use C++14 generic lambda expressions.
		[&](iter_type b, iter_type e){
		YAssert(e >= b, "Invalid split result found.");

		string_view sv(&*b, size_t(e - b));

		YAssert(!sv.empty(), "Null token found.");
		if(IsGraphicalDelimiter(*b))
		{
			dst.push_back({sv.front()});
			sv.remove_prefix(1);
		}
		ystdex::trim(sv);
		if(!sv.empty())
			dst.push_back({sv.data(), sv.size()});
	});
	return dst;
}

TokenList
Tokenize(const TokenViewList& src)
{
	const auto a(src.get_allocator());
	TokenList dst(a);

	for(const auto sv : src)
		if(!sv.empty())
		{
			if(sv.front() != '\'' && sv.front() != '"')
				dst.splice(dst.end(), Decompose(sv, a));
			else
				dst.push_back(TokenList::value_type(sv.begin(), sv.end()));
		}
	return dst;
}

} // namespace NPL;

