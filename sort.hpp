#ifndef SORT_HPP
#define SORT_HPP


#include <functional>

#include "sem2_lab3/avl_tree.hpp"


template <typename Iter>
void quick_sort(Iter begin, Iter end) {
	return quick_sort(begin, end, [](auto a, auto b){
		return a < b;
	});
}

template <typename Iter, typename Func>
void quick_sort(Iter begin, Iter end, Func less) {
	Iter l = begin;
	Iter r = end-1;
	if(l >= r)
		return;

	Iter m = l + (r - l) / 2 + 1;
	do {
		while(less(*l, *m))
			++l;
		while(less(*m, *r))
			--r;

		if(l <= r) {
			std::swap(*l, *r);
			++l;
			if(r > 0)
				--r;
			else
				break;
		}
	} while(l <= r);

	quick_sort(begin, r+1, less);
	quick_sort(l, end, less);
}


template <typename Iter>
void counting_sort(Iter begin, Iter end) {
	return counting_sort(begin, end, [](auto val){return val;}, [](auto val){return val;});
}

template <typename Iter, typename Func1, typename Func2>
void counting_sort(Iter begin, Iter end, Func1 to_integer, Func2 from_integer) {
	auto mn = to_integer(*begin);
	auto mx = to_integer(*begin);
	for(auto it = begin; it != end; ++it){
		mx = std::max(mx, to_integer(*it));
		mn = std::min(mn, to_integer(*it));
	}

	size_t n = mx - mn + 1;
	auto *buff = new size_t[n];

	for(size_t i = 0; i < n; ++i){
		buff[i] = 0;
	}

	for(auto it = begin; it != end; ++it){
		++buff[to_integer(*it) - mn];
	}

	auto it = begin;
	for(size_t i = 0; i < n; ++i){
		for(size_t j = 0; j < buff[i]; ++j){
			*it = from_integer(i + mn);
			++it;
		}
	}
}



template <typename Iter>
void tree_sort(Iter begin, Iter end) {
	using T = typeof(*begin);
	AVL_Tree<T, size_t> tree;
	for(auto it = begin; it != end; ++it){
		tree[*it] += 1;
	}

	auto it = begin;
	tree.const_traversal(tree.LRtR, [&it](const T& key, const size_t& value){
		for(auto i = 0; i < value; ++i, ++it)
			*it = key;
	});
}



#endif //SORT_HPP
