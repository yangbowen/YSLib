﻿/*
	© 2015-2016, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Main.cpp
\ingroup Tools
\brief SXML 文档转换工具。
\version r61
\author FrankHB <frankhb1989@gmail.com>
\since build 599
\par 创建时间:
	2015-05-09 16:32:08 +0800
\par 修改时间:
	2020-03-27 13:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::SXML2XML::Main

This is a command line tool to convert SXML document to XML document.
See readme file for details.
*/


#include <YSBuild.h>
#include YFM_NPL_NPLA
#include <iostream>

using namespace YSLib;
using namespace NPL;


int
main()
{
	return FilterExceptions([]{
		platform::SetupBinaryStdIO();

		using namespace std;
		Session sess{};

		sess.Parse(istreambuf_iterator<char>(cin), istreambuf_iterator<char>());
		SXML::PrintSyntaxNode(cout, SContext::Analyze(sess));
		cout << endl;
	}, yfsig) ? EXIT_FAILURE : EXIT_SUCCESS;
}

