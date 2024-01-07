#include "kalmanfilter.h"
#include <cmath>

KalmanFilter::KalmanFilter() {

}

void KalmanFilter::init(double stdDev, double A, double C, double power, double dt) {
	_A = A;
	_C = C;

	_V = power * stdDev * dt;
	_W = stdDev * stdDev;
}

void KalmanFilter::reset(double initialVal) {
	_P0 = 1;
	_xpri = initialVal;
	_Ppri = _P0;
	_xpost = initialVal;
	_Ppost = _P0;
}

double KalmanFilter::getFiltered(double value) {
	double eps, s, k;

	_xpri = _A * _xpost;
	_Ppri = _A * _Ppost + _V;

	eps = value - _C * _xpri;
	s = _C * _Ppri * _C + _W;
	k = _Ppri * _C * pow(s, -1);
	_xpost = _xpri + k * eps;
	_Ppost = _Ppri - k * s * k;

	return _xpost;
}
