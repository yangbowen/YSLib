﻿/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file base.h
\ingroup Adaptor
\brief 通用基础设施。
\version 0.2101;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-10-09 09:25:27 + 08:00;
\par 修改时间:
	2011-01-21 23:06 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::Base;
*/


#ifndef INCLUDED_BASE_H_
#define INCLUDED_BASE_H_

/*
 ISO/IEC C++ 未确定宏定义内 # 和 ## 操作符求值顺序。
 GCC 中，宏定义内 ## 操作符修饰的形式参数为宏时，该宏不会被展开。
详见：http://gcc.gnu.org/onlinedocs/cpp/Concatenation.html 。
解决方案来源：
 https://www.securecoding.cert.org/confluence/display/cplusplus/ \
 PRE05-CPP.+Understand+macro+replacement+when+concatenating \
 +tokens+or+performing+stringification 。
*/
#define _yJOIN(x, y) x ## y


// YSLib 命名空间宏。

#define YSL_BEGIN	namespace YSLib {
#define YSL_END		}
#define YSL_			::YSLib::
#define YSL			::YSLib

#define YSL_BEGIN_NAMESPACE(s)	namespace s {
#define YSL_END_NAMESPACE(s)		}

#define YSL_BEGIN_SHELL(s)	namespace _yJOIN(_YSHL_, s) {
#define YSL_END_SHELL(s)	}
#define YSL_SHL_(s)		YSL_ _yJOIN(_YSHL_, s)::
#define YSL_SHL(s)		YSL_ _yJOIN(_YSHL_, s)


//异常规范宏。

// ythrow = "yielded throwing";
#ifdef YSL_USE_EXCEPTION_SPECIFICATION
#	define ythrow throw
#else
#	define ythrow(...)
#endif


//接口类型宏。

#define _yInterface struct

#define implements public

#define _yInterfaceHead(_name) { \
	virtual ~_name() {}

//prefix "PDecl" = Pre-declare;
#define PDeclInterface(_name) _yInterface _name;

#define DeclInterface(_name) _yInterface _name _yInterfaceHead(_name)

//对于基接口需要显式指定访问权限和继承方式。
//由于接口定义为 struct 类型，因此通常只需指定是否为 virtual 继承。
// ImplI = Implements Interface;
//抽象实现：保留接口供派生类实现（可以提供接口函数的默认实现）。
// ImplA = Implements Abstractly;
#ifdef __GNUC__
#	define DeclBasedInterface(_name, _base...) \
	_yInterface _name : _base _yInterfaceHead(_name)
#	define ImplI(_interface, ...) virtual
#	define ImplA(_interface, ...)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define DeclBasedInterface(_name, _base, ...) \
	_yInterface _name : _base, __VA_ARGS__ _yInterfaceHead(_name)
#	define ImplI(_interface, ...) virtual
#	define ImplA(_interface, ...)
#else
//注意 ISO/IEC C++ 不支持宏的可变参数列表，因此无法实现接口多继承。
#	define DeclBasedInterface(_name, _base) \
	_yInterface _name : _base _yInterfaceHead(_name)
#	define ImplI(_interface) virtual
#	define ImplA(_interface)
#endif

//"DeclIEntry" = Declare Interface Entry;
#define DeclIEntry(_signature) virtual _signature = 0;

#define EndDecl };


//函数宏。

//通用函数头定义。
//prefix "PDefH" = Partially Define Head;
#ifdef __GNUC__
#	define PDefH(_type, _name, _paralist...) \
	_type _name(_paralist)
#	define PDefHOperator(_type, _op, _paralist...) \
	_type operator _op(_paralist)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define PDefH(_type, _name, _paralist, ...) \
	_type _name(_paralist, __VA_ARGS__)
#	define PDefHOperator(_type, _op, _paralist, ...) \
	_type operator _op(_paralist, __VA_ARGS__)
#else
#	define PDefH(_type, _name, _paralist) \
	_type _name(_paralist)
#	define PDefHOperator(_type, _op, _paralist) \
	_type operator _op(_paralist)
#endif
#define PDefConverter(_type) \
	operator _type()


//简单通用函数实现。
//prefix "Impl" = Implement;
#define ImplExpr(_expr) \
	{ \
		_expr; \
	}
#define ImplRet(_expr) \
	{ \
		return _expr; \
	}

//基类同名函数映射和成员同名函数映射实现。
//prefix "Impl" = Implement;
#ifdef __GNUC__
#	define ImplBodyBase(_base, _name, _arglist...) \
	{ \
		return _base::_name(_arglist); \
	}
#	define ImplBodyBaseVoid(_base, _name, _arglist...) \
	{ \
		_base::_name(_arglist); \
	}
#	define ImplBodyMember(_member, _name, _arglist...) \
	{ \
		return (_member)._name(_arglist); \
	}
#	define ImplBodyMemberVoid(_member, _name, _arglist...) \
	{ \
		(_member)._name(_arglist); \
	}
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#	define ImplBodyBase(_base, _name, _arglist, ...) \
	{ \
		return _base::_name(_arglist, __VA_ARGS__); \
	}
#	define ImplBodyBaseVoid(_base, _name, _arglist, ...) \
	{ \
		_base::_name(_arglist, __VA_ARGS__); \
	}
#	define ImplBodyMember(_member, _name, _arglist...) \
	{ \
		return (_member)._name(_arglist); \
	}
#	define ImplBodyMemberVoid(_member, _name, _arglist...) \
	{ \
		(_member)._name(_arglist); \
	}
#else
#	define ImplBodyBase(_base, _name, _arglist) \
	{ \
		return _base::_name(_arglist); \
	}
#	define ImplBodyBaseVoid(_base, _name, _arglist) \
	{ \
		_base::_name(_arglist); \
	}
#	define ImplBodyMember(_member, _name, _arglist) \
	{ \
		return (_member)._name(_arglist); \
	}
#	define ImplBodyMemberVoid(_member, _name, _arglist) \
	{ \
		(_member)._name(_arglist); \
	}
#endif


//简单通用成员函数定义。
//prefix "Def" = Define;
#define DefEmptyDtor(_type) \
	~_type() ythrow() \
	{}

#define DefConverter(_type, _expr) \
	operator _type() const ythrow() \
	{ \
		return _expr; \
	}
#define DefConverterBase(_type, _base) \
	operator _type() const ythrow() \
	{ \
		return _base::operator _type(); \
	}
#define DefConverterMember(_type, _member) \
	operator _type() const ythrow() \
	{ \
		return (_member).operator _type(); \
	}

#define DefPredicate(_name, _member) \
	bool _yJOIN(Is, _name)() const ythrow() \
	{ \
		return (_member); \
	}
#define DefPredicateBase(_name, _base) \
	bool _yJOIN(Is, _name)() const ythrow() \
	{ \
		return _base::_yJOIN(Is, _name)(); \
	}
#define DefPredicateMember(_name, _member) \
	bool _yJOIN(Is, _name)() const ythrow() \
	{ \
		return (_member)._yJOIN(Is, _name)(); \
	}

#define DefMutablePredicate(_name, _member) \
	bool _yJOIN(Is, _name)() ythrow() \
	{ \
		return (_member); \
	}
#define DefMutablePredicateBase(_name, _base) \
	bool _yJOIN(Is, _name)() ythrow() \
	{ \
		return _base::_yJOIN(Is, _name)(); \
	}
#define DefMutablePredicateMember(_name, _member) \
	bool _yJOIN(Is, _name)() ythrow() \
	{ \
		return (_member)._yJOIN(Is, _name)(); \
	}

#define DefGetter(_type, _name, _member) \
	_type _yJOIN(Get, _name)() const ythrow() \
	{ \
		return (_member); \
	}
#define DefGetterBase(_type, _name, _base) \
	_type _yJOIN(Get, _name)() const ythrow() \
	{ \
		return _base::_yJOIN(Get, _name)(); \
	}
#define DefGetterMember(_type, _name, _member) \
	_type _yJOIN(Get, _name)() const ythrow() \
	{ \
		return (_member)._yJOIN(Get, _name)(); \
	}

#define DefMutableGetter(_type, _name, _member) \
	_type _yJOIN(Get, _name)() ythrow() \
	{ \
		return (_member); \
	}
#define DefMutableGetterBase(_type, _name, _base) \
	_type _yJOIN(Get, _name)() ythrow() \
	{ \
		return _base::_yJOIN(Get, _name)(); \
	}
#define DefMutableGetterMember(_type, _name, _member) \
	_type _yJOIN(Get, _name)() ythrow() \
	{ \
		return (_member)._yJOIN(Get, _name)(); \
	}

#define DefSetter(_type, _name, _member) \
	void _yJOIN(Set, _name)(_type _tempArgName) \
	{ \
		(_member) = _tempArgName; \
	}
#define DefSetterDe(_type, _name, _member, _defv) \
	void _yJOIN(Set, _name)(_type _tempArgName = _defv) \
	{ \
		(_member) = _tempArgName; \
	}
#define DefSetterBase(_type, _name, _base) \
	void _yJOIN(Set, _name)(_type _tempArgName) \
	{ \
		_base::_yJOIN(Set, _name)(_tempArgName); \
	}
#define DefSetterBaseDe(_type, _name, _base, _defv) \
	void _yJOIN(Set, _name)(_type _tempArgName = _defv) \
	{ \
		_base::_yJOIN(Set, _name)(_tempArgName); \
	}
#define DefSetterMember(_type, _name, _member) \
	void _yJOIN(Set, _name)(_type _tempArgName) \
	{ \
		(_member)._yJOIN(Set, _name)(_tempArgName); \
	}
#define DefSetterMemberDe(_type, _name, _member, _defv) \
	void _yJOIN(Set, _name)(_type _tempArgName = _defv) \
	{ \
		(_member)._yJOIN(Set, _name)(_tempArgName); \
	}
#define DefSetterEx(_type, _name, _member, _tempArgName, _expr) \
	void _yJOIN(Set, _name)(_type _tempArgName) \
	{ \
		(_member) = (_expr); \
	}
#define DefSetterDeEx(_type, _name, _member, _defv, _tempArgName, _expr) \
	void _yJOIN(Set, _name)(_type _tempArgName = _defv) \
	{ \
		(_member) = (_expr); \
	}

#endif

