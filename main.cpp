#include <iostream>
#include "sort.hpp"
#include <iterator>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>

#include "sem2_lab2/dynamic_array.hpp"
#include "sem2_lab3/avl_tree.hpp"


template <typename Func, typename ...Args>
long long measure_time(Func f, Args&& ...args)
{
	auto start = std::chrono::high_resolution_clock::now();
	f(std::forward<Args>(args)...);
	auto stop = std::chrono::high_resolution_clock::now();

	return std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
}


template <typename T>
class Main {
public:
	Main(std::istream &is, std::ostream &os);

	void start();
	void stop();
private:
	void _loop();

	void _input();
	void _clear();
	void _output();
	void _random();
	void _random_impl(T min, T max, size_t n);

	void _sort();

	unsigned long long _sort_count();
private:
	std::istream &_is;
	std::ostream &_os;
	DynamicArray<T> _arr;
	AVL_Tree<const char*, std::function<void()>> _cmds;
	bool _running;
};


template <typename T>
Main<T>::Main(std::istream &is, std::ostream &os):
	_is(is), _os(os), _running(false)
{
	_cmds = {
		std::make_pair("clear", std::bind(&Main<T>::_clear, this)),
		std::make_pair("input", std::bind(&Main<T>::_input, this)),
		std::make_pair("output", std::bind(&Main<T>::_output, this)),
		std::make_pair("random", std::bind(&Main<T>::_random, this)),
		std::make_pair("sort", std::bind(&Main<T>::_sort, this)),
		std::make_pair("stop", std::bind(&Main<T>::stop, this))
	};
}


template <typename T>
void Main<T>::_input() {
	size_t n;
	_os << "Number of items: ";
	_is >> n;

	_os << "Items: ";
	for(size_t i = 0; i < n; ++i){
		T t;
		_is >> t;
		_arr.push_back(t);
	}
}

template <typename T>
void Main<T>::_clear() {
	_arr.clear();
}

template <typename T>
void Main<T>::_output() {
	_os << "Items (" << _arr.size() << "): ";
	for(size_t i = 0; i < _arr.size(); ++i){
		_os << _arr[i] << ' ';
	}
	_os << '\n';
}

template <typename T>
void Main<T>::_random() {
	size_t n;
	T min, max;
	_os << "Number of items: ";
	_is >> n;
	_os << "Min and max: ";
	_is >> min >> max;

	_random_impl(min, max, n);
}


template <typename T>
void Main<T>::_random_impl(T min, T max, size_t n) {
	std::random_device rnd_device;
	std::mt19937 mersenne_engine {rnd_device()};
	std::uniform_int_distribution<T> dist{min, max};
	for(size_t i = 0; i < n; ++i){
		_arr.push_back(dist(mersenne_engine));
	}
}

template <>
void Main<double>::_random_impl(double min, double max, size_t n) {
	std::random_device rnd_device;
	std::mt19937 mersenne_engine {rnd_device()};
	std::uniform_real_distribution<double> dist{min, max};
	for(size_t i = 0; i < n; ++i){
		_arr.push_back(dist(mersenne_engine));
	}
}


template <typename T>
void Main<T>::_sort() {
	std::string sort_method;
	_os << "Select sort method: ";
	_is >> sort_method;
	unsigned long long time = 0;

	if(sort_method == "quick"){
		time = measure_time(quick_sort<typename DynamicArray<T>::iterator>, _arr.begin(), _arr.end());
	}
	else if(sort_method == "quick_std"){
		time = measure_time(std::sort<typename DynamicArray<T>::iterator>, _arr.begin(), _arr.end());
	}
	else if(sort_method == "count"){
		time = _sort_count();
	}
	else if(sort_method == "tree"){
		time = measure_time(tree_sort<typename DynamicArray<T>::iterator>, _arr.begin(), _arr.end());
	}
	else{
		_os << "Incorrect method\n";
		return;
	}

	_os << "Sorted in " << time << "ms\n";
}


template <typename T>
unsigned long long Main<T>::_sort_count() {
	return measure_time(counting_sort<typename DynamicArray<T>::iterator>, _arr.begin(), _arr.end());
}

template <>
unsigned long long Main<double>::_sort_count() {
	int precision;
	_os << "Select precision: ";
	_is >> precision;

	auto to_int = [precision](double val){
		return static_cast<long long>(precision * val);
	};
	auto from_int = [precision](long long val){
		return static_cast<double>(val) / precision;
	};

	auto sort_function = counting_sort< typename DynamicArray<double>::iterator,
										std::function<long long(double)>, std::function<double(long long)>>;
	return measure_time(sort_function, _arr.begin(), _arr.end(), to_int, from_int);
}

template<typename T>
void Main<T>::_loop() {
	_os << "Available sections: ";
	_cmds.const_traversal(_cmds.LRtR, [this](auto key, auto val){
		_os << key << ' ';
	});
	_os << '\n';

	while(_running){
		std::string cmd;
		_os << "Select section: ";
		_is >> cmd;
		if(_cmds.find(cmd)){
			_cmds.get(cmd)();
		}
		else{
			_os << "Incorrect section\n";
		}
	}
}

template<typename T>
void Main<T>::start() {
	_running = true;
	_loop();
}

template<typename T>
void Main<T>::stop() {
	_running = false;
	_os << "Goodbye!";
}


int main() {
	std::string datatype;
	std::cout << "Select datatype (int, double): ";
	std::cin >> datatype;

	if(datatype == "int") {
		Main<int> main(std::cin, std::cout);
		main.start();
	}
	else if(datatype == "double") {
		Main<double> main(std::cin, std::cout);
		main.start();
	}
	else {
		std::cout << "Incorrect datatype\n";
	}
}