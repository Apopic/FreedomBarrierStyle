#pragma once

namespace std {
	inline bool signbit(int _X) {
		if (_X <= 0) {
			return true;
		}
		return false;
	}
	template<class T>
	inline T digit(T Num) {
		if (Num == 0) {
			return 1;
		}
		T digit = 0;
		while (Num != 0) {
			++digit;
			Num /= 10;
		}
		return digit;
	}
	template<class T>
	inline T log_a(T a, T x) {
		return log10(x) / log10(a);
	}
}