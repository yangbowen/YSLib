﻿/*
	Copyright (C) by Franksoft 2011 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file iterator.hpp
\ingroup YStandardEx
\brief 通用迭代器。
\version r1849
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 189
\par 创建时间:
	2011-01-27 23:01:00 +0800
\par 修改时间:
	2012-11-06 15:06 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Iterator
*/


#ifndef YB_INC_YSTDEX_ITERATOR_HPP_
#define YB_INC_YSTDEX_ITERATOR_HPP_ 1

#include "type_op.hpp" // for std::remove_reference, ystdex::*_tag;
#include <iterator> // for std::make_move_iterator, std::iterator,
//	std::iterator_traits, std::iterator_category;
#include <utility> // for std::make_pair;
#include <memory> // for std::addressof;

namespace ystdex
{

/*!	\defgroup iterators Iterators
\brief 迭代器。
*/

/*!	\defgroup iterator_adaptors Iterator Adaptors
\ingroup iterators
\brief 迭代器适配器。
*/


/*!
\ingroup helper_functions
\brief 取转移迭代器对。
\since build 337
*/
template<typename _tIterator1, typename _tIterator2>
inline auto
make_move_iterator_pair(_tIterator1 it1, _tIterator2 it2) -> decltype(
	std::make_pair(std::make_move_iterator(it1), std::make_move_iterator(it2)))
{
	return std::make_pair(std::make_move_iterator(it1),
		std::make_move_iterator(it2));
}
/*!
\ingroup helper_functions
\brief 取容器的转移迭代器对。
\since build 337
*/
template<typename _tContainer>
inline auto
make_move_iterator_pair(_tContainer& c)
	-> decltype(ystdex::make_move_iterator_pair(c.begin(), c.end()))
{
	return ystdex::make_move_iterator_pair(c.begin(), c.end());
}


/*!
\ingroup iterator_adaptors
\brief 指针迭代器。
\since build 290

转换指针为类类型的随机迭代器。
*/
template<typename _type>
class pointer_iterator
	: private std::iterator<typename
	std::iterator_traits<_type*>::iterator_category, _type>
{
protected:
	typedef typename std::iterator<typename
		std::iterator_traits<_type*>::iterator_category, _type> base_type;

public:
	typedef typename base_type::difference_type difference_type;
	typedef typename base_type::value_type value_type;
	typedef typename base_type::pointer pointer;
	typedef typename base_type::reference reference;
	typedef typename base_type::iterator_category iterator_category;

protected:
	mutable pointer current;

public:
	yconstfn
	pointer_iterator(std::nullptr_t = {})
		: current()
	{}
	//! \since build 347
	template<typename _tPointer>
	explicit yconstfn
	pointer_iterator(_tPointer&& ptr)
		: current(yforward(ptr))
	{}
	inline
	pointer_iterator(const pointer_iterator&) = default;
	inline
	pointer_iterator(pointer_iterator&&) = default;

	inline pointer_iterator&
	operator+=(const difference_type& n)
	{
		current += n;
		return *this;
	}

	inline pointer_iterator&
	operator-=(const difference_type& n)
	{
		current -= n;
		return *this;
	}

	yconstfn reference
	operator*() const
	{
		return *current;
	}

	yconstfn pointer
	operator->() const
	{
		return current;
	}

	inline pointer_iterator&
	operator++()
	{
		++current;
		return *this;
	}
	yconstfn pointer_iterator
	operator++(int)
	{
		return current++;
	}

	inline pointer_iterator&
	operator--()
	{
		--current;
		return *this;
	}
	yconstfn pointer_iterator
	operator--(int)
	{
		return current--;
	}

	yconstfn reference
	operator[](const difference_type& n) const
	{
		return current[n];
	}

	yconstfn pointer_iterator
	operator+(const difference_type& n) const
	{
		return pointer_iterator(current + n);
	}

	yconstfn pointer_iterator
	operator-(const difference_type& n) const
	{
		return pointer_iterator(current - n);
	}

	yconstfn
	operator pointer() const
	{
		return current;
	}
};


/*!
\ingroup meta_operations
\brief 指针包装为类类型迭代器。
\since build 290

若参数是指针类型则包装为 pointer_iterator 。
*/
//@{
template<typename _type>
struct pointer_classify
{
	typedef _type type;
};

template<typename _type>
struct pointer_classify<_type*>
{
	typedef pointer_iterator<_type> type;
};
//@}


/*!
\ingroup iterator_adaptors
\brief 伪迭代器。

总是返回单一值的迭代器适配器。
*/
template<typename _type, typename _tIterator = _type*,
	typename _tTraits = std::iterator_traits<_tIterator>>
class pseudo_iterator
{
protected:
	typedef _tTraits traits_type;

public:
	typedef _tIterator iterator_type;
	typedef typename traits_type::iterator_category iterator_category;
	typedef typename traits_type::value_type value_type;
	typedef typename traits_type::difference_type difference_type;
	typedef typename traits_type::reference reference;
	typedef typename traits_type::pointer pointer;

	value_type value;

	yconstfn
	pseudo_iterator()
		: value()
	{}
	explicit yconstfn
	pseudo_iterator(value_type v)
		: value(v)
	{}
	yconstfn inline
	pseudo_iterator(const pseudo_iterator&) = default;
	yconstfn inline
	pseudo_iterator(pseudo_iterator&&) = default;

	pseudo_iterator&
	operator=(const pseudo_iterator&) = default;
	pseudo_iterator&
	operator=(pseudo_iterator&&) = default;

	pseudo_iterator&
	operator+=(const difference_type&)
	{
		return *this;
	}

	pseudo_iterator&
	operator-=(const difference_type&)
	{
		return *this;
	}

	//前向迭代器需求。
	yconstfn reference
	operator*() const
	{
		return value;
	}

	yconstfn pointer
	operator->() const
	{
		return this;
	}

	pseudo_iterator&
	operator++()
	{
		return *this;
	}

	pseudo_iterator
	operator++(int)
	{
		return *this;
	}

	//双向迭代器需求。
	yconstfn pseudo_iterator&
	operator--()
	{
		return *this;
	}

	yconstfn pseudo_iterator
	operator--(int)
	{
		return *this;
	}

	//随机访问迭代器需求。
	yconstfn reference
	operator[](const difference_type& _n) const
	{
		return this[_n];
	}

	yconstfn pseudo_iterator
	operator+(const difference_type&) const
	{
		return *this;
	}

	yconstfn pseudo_iterator
	operator-(const difference_type&) const
	{
		return *this;
	}
};


/*!
\ingroup iterator_adaptors
\brief 转换迭代器。
\since build 288

使用指定参数隐藏指定迭代器的间接操作的迭代器适配器。
*/
template<typename _tIterator, typename _fTransformer>
class transformed_iterator : public pointer_classify<_tIterator>::type
{
public:
	/*!
	\brief 原迭代器类型。
	\since build 290
	*/
	typedef typename pointer_classify<typename
		remove_reference<_tIterator>::type>::type iterator_type;
	typedef _fTransformer transformer_type;
	typedef decltype(std::declval<_fTransformer>()(
		std::declval<_tIterator&>())) result;
	typedef decltype(std::declval<_fTransformer>()(std::declval<const
		_tIterator&>())) const_result;

protected:
	mutable transformer_type transformer;

public:
	//! \since build 347
	template<typename _tIter, typename _tTran>
	explicit yconstfn
	transformed_iterator(_tIter&& i, _tTran&& f = {})
		: iterator_type(yforward(i)), transformer(f)
	{}

	inline result
	operator*()
	{
		return transformer(get());
	}
	inline const_result
	operator*() const
	{
		return transformer(get());
	}

	inline result*
	operator->()
	{
		return std::addressof(operator*());
	}
	inline const_result*
	operator->() const
	{
		return std::addressof(operator*());
	}

	/*!
	\brief 转换为原迭代器引用。
	\since build 290
	*/
	inline
	operator iterator_type&()
	{
		return *this;
	}

	/*!
	\brief 转换为原迭代器 const 引用。
	\since build 290
	*/
	yconstfn
	operator const iterator_type&() const
	{
		return *this;
	}

	/*!
	\brief 取原迭代器引用。
	\since build 290
	*/
	inline iterator_type&
	get()
	{
		return *this;
	}

	/*!
	\brief 取原迭代器 const 引用。
	\since build 290
	*/
	yconstfn const iterator_type&
	get() const
	{
		return *this;
	}
};


/*!
\ingroup helper_functions
\brief 创建转换迭代器。
\note 使用 ADL 。
\since build 288
*/
template<typename _tIterator, typename _fTransformer>
inline transformed_iterator<typename array_ref_decay<_tIterator>::type,
	_fTransformer>
make_transform(_tIterator&& i, _fTransformer&& f)
{
	return transformed_iterator<typename array_ref_decay<_tIterator>::type,
		_fTransformer>(yforward(i), f);
}


/*!
\brief 成对迭代操作。
\since build 288
*/
template<typename _tIterator>
struct pair_iterate
{
	typedef _tIterator iterator_type;
	typedef decltype(*std::declval<_tIterator>()) reference;
	typedef typename remove_reference<reference>::type pair_type;
	typedef typename pair_type::first_type first_type;
	typedef typename pair_type::second_type second_type;

	static yconstfn auto
	first(const _tIterator& i) -> decltype(i->first)
	{
		return i->first;
	}
	static yconstfn auto
	second(const _tIterator& i) -> decltype(i->second)
	{
		return i->second;
	}
};


/*!
\brief 操纵子。
\since build 288
*/
//@{
yconstexpr first_tag get_first = {}, get_key = {};
yconstexpr second_tag get_second = {}, get_value = {};
//@}


/*!
\brief 管道匹配操作符。
\since build 288
*/
template<typename _tIterator>
inline auto
operator|(_tIterator&& i, first_tag)
	-> decltype(make_transform(yforward(i), pair_iterate<typename
	decay<_tIterator>::type>::first))
{
	return make_transform(yforward(i), pair_iterate<typename
		decay<_tIterator>::type>::first);
}
template<typename _tIterator>
inline auto
operator|(_tIterator&& i, second_tag)
	-> decltype(make_transform(yforward(i), pair_iterate<typename
	decay<_tIterator>::type>::second))
{
	return make_transform(yforward(i), pair_iterate<typename
		decay<_tIterator>::type>::second);
}


/*!
\ingroup iterator_adaptors
\brief 成对迭代器。

拼接两个迭代器对得到的迭代器适配器，以第一个为主迭代器的迭代器适配器。
*/
template<typename _tMaster, typename _tSlave,
	class _tTraits = std::iterator_traits<_tMaster>>
class pair_iterator : private std::pair<_tMaster, _tSlave>
{
protected:
	typedef _tTraits traits_type;

public:
	typedef std::pair<_tMaster, _tSlave> pair_type;
	typedef _tMaster iterator_type;
	typedef typename traits_type::iterator_category iterator_category;
	typedef typename traits_type::value_type value_type;
	typedef typename traits_type::difference_type difference_type;
	typedef typename traits_type::reference reference;
	typedef typename traits_type::pointer pointer;

	yconstfn
	pair_iterator()
		: std::pair<_tMaster, _tSlave>(_tMaster(), _tSlave())
	{}
	explicit yconstfn
	pair_iterator(const _tMaster& _i)
		: std::pair<_tMaster, _tSlave>(_i, _tSlave())
	{}
	yconstfn
	pair_iterator(const _tMaster& _i, const _tSlave& _s)
		: std::pair<_tMaster, _tSlave>(_i, _s)
	{}
	yconstfn
	pair_iterator(const pair_iterator&) = default;
	yconstfn
	pair_iterator(pair_iterator&& _r)
		: std::pair<_tMaster, _tSlave>(std::move(_r))
	{}

	inline pair_iterator&
	operator=(const pair_iterator&) = default;
	inline pair_iterator&
	operator=(pair_iterator&&) = default;
	// TODO: Allow iterator to const_iterator conversion.

	pair_iterator&
	operator+=(const difference_type& _n)
	{
		yunseq(this->first += _n, this->second += _n);
		return *this;
	}

	pair_iterator&
	operator-=(const difference_type& _n)
	{
		yunseq(this->first -= _n, this->second -= _n);
		return *this;
	}

	//前向迭代器需求。
	yconstfn reference
	operator*() const
	{
		return *this->first;
	}

	yconstfn pointer
	operator->() const
	{
		return this->first;
	}

	pair_iterator&
	operator++()
	{
		yunseq(++this->first, ++this->second);
		return *this;
	}

	pair_iterator
	operator++(int)
	{
		const auto i(*this);

		++*this;
		return i;
	}

	//双向迭代器需求。
	pair_iterator&
	operator--()
	{
		yunseq(--this->first, --this->second);
		return *this;
	}

	pair_iterator
	operator--(int)
	{
		const auto i(*this);

		--*this;
		return i;
	}

	//随机访问迭代器需求。
	yconstfn reference
	operator[](const difference_type& _n) const
	{
		return this->first[_n];
	}

	yconstfn pair_iterator
	operator+(const difference_type& _n) const
	{
		return pair_iterator(this->first + _n, this->second + _n);
	}

	yconstfn pair_iterator
	operator-(const difference_type& _n) const
	{
		return pair_iterator(this->first - _n, this->second - _n);
	}

	yconstfn const pair_type&
	base() const
	{
		return *this;
	}
};

} // namespace ystdex;

#endif

