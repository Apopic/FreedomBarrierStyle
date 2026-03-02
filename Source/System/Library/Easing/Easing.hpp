#pragma once
#include <cmath>
#define PI (3.1415926535897932384626433832795028841971)

namespace ease {
	enum Base {
		In,
		Out,
		InOut,
		OutIn
	};

	enum Line {
		Linear,
		Sine,
		Quad,
		Cubic,
		Quart,
		Quint,
		Expo,
		Cric,
		Back,
		Elastic,
		Bounce
	};

	inline double GetEasingRate(double x, const Base ebt, const Line elt) {
		if (x <= 0.0) { return 0.0; }
		if (x >= 1.0) { return 1.0; }
		switch (ebt) {
			case Base::InOut:
				return x < 0.5 ?
					GetEasingRate(x / 0.5, Base::In, elt) / 2 :
					0.5 + (GetEasingRate((x - 0.5) / 0.5, Base::Out, elt) / 2);
				break;
			case Base::OutIn:
				return x < 0.5 ?
					GetEasingRate(x / 0.5, Base::Out, elt) / 2 :
					0.5 + (GetEasingRate((x - 0.5) / 0.5, Base::In, elt) / 2);
				break;
		}

		switch (elt) {
			case Line::Linear:
				return x;
				break;
			case Line::Sine:
				switch (ebt) {
					case Base::In:
						return 1 - cos(x * PI / 2);
						break;
					case Base::Out:
						return sin(x * PI / 2);
						break;
				}
				break;
			case Line::Quad:
				switch (ebt) {
					case Base::In:
						return x * x;
						break;
					case Base::Out:
						return 1 - (1 - x) * (1 - x);
						break;
				}
				break;
			case Line::Cubic:
				switch (ebt) {
					case Base::In:
						return x * x * x;
						break;
					case Base::Out:
						return 1 - (1 - x) * (1 - x) * (1 - x);
						break;
				}
				break;
			case Line::Quart:
				switch (ebt) {
					case Base::In:
						return x * x * x * x;
						break;
					case Base::Out:
						return 1 - (1 - x) * (1 - x) * (1 - x) * (1 - x);
						break;
				}
				break;
			case Line::Quint:
				switch (ebt) {
					case Base::In:
						return x * x * x * x * x;
						break;
					case Base::Out:
						return 1 - (1 - x) * (1 - x) * (1 - x) * (1 - x) * (1 - x);
						break;
				}
				break;
			case Line::Expo:
				switch (ebt) {
					case Base::In:
						return x == 0 ? 0 : pow(2, 10 * x - 10);
						break;
					case Base::Out:
						return x == 1 ? 1 : 1 - pow(2, -10 * x);
						break;
				}
				break;
			case Line::Cric:
				switch (ebt) {
					case Base::In:
						return 1 - sqrt(1 - pow(x, 2));
						break;
					case Base::Out:
						return sqrt(1 - pow(x - 1, 2));
						break;
				}
				break;
			case Line::Back:
			{
				const double c1 = 1.70158;
				const double c3 = c1 + 1;
				switch (ebt) {
					case Base::In:
						return c3 * x * x * x - c1 * x * x;
						break;
					case Base::Out:
						return 1 + c3 * pow(x - 1, 3) + c1 * pow(x - 1, 2);
						break;
				}
			}	break;
			case Line::Elastic:
			{
				const double c4 = 2 * PI / 3;
				switch (ebt) {
					case Base::In:
						return
							x == 0 ?
							0 :
							x == 1 ?
							1 : -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4);
						break;
					case Base::Out:
						return
							x == 0 ?
							0 :
							x == 1 ?
							1 :
							pow(2, -10 * x) * sin((x * 10 - 0.75) * c4) + 1;
						break;
				}
			} break;
			case Line::Bounce:
				switch (ebt) {
					case Base::In:
						return 1 - GetEasingRate(1 - x, Base::Out, elt);
						break;
					case Base::Out: {
						const double n1 = 7.5625;
						const double d1 = 2.75;

						if (x < 1 / d1) {
							return n1 * x * x;
						}
						else if (x < 2 / d1) {
							return n1 * (x -= 1.5 / d1) * x + 0.75;
						}
						else if (x < 2.5 / d1) {
							return n1 * (x -= 2.25 / d1) * x + 0.9375;
						}
						else {
							return n1 * (x -= 2.625 / d1) * x + 0.984375;
						}
					}
					break;
				}
				break;
		}
		return 0;
	}
}

#undef PI
