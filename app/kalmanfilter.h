#ifndef KALMANFILTER_H
#define KALMANFILTER_H


class KalmanFilter
{
	private:
		double _A;
		double _C;
		double _V;
		double _W;
		double _P0;
		double _xpri;
		double _Ppri;
		double _xpost;
		double _Ppost;

	public:
		KalmanFilter();
		void init(double stdDev, double A, double C, double power, double dt);
		void reset(double initialVal);
		double getFiltered(double value);
};

#endif // KALMANFILTER_H
