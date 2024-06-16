#ifndef DYNORUNRESULT_H
#define DYNORUNRESULT_H

#include <QList>
#include "kalmanfilter.h"

/* Klasa przechowująca dane w czasie */
class DynoRunResultItem {
	public:
		/* Dane zbierane z GPS */
		double time;       /* Czas w ms */
		double speed;      /* Prędkość w km/h */

		/* Dane obliczane */
		double speedMs;         /* Prędkośc w m/s */
		int rpm;                /* Obroty */
		double acc;             /* Przyśpieszenie */
		double accMean;         /* Przyśpieszenie po uśrednieniu */
		double accFiltered;     /* Przyśpieszenie po filtrze kalamana */
		double force;			/* Siła w N */
		double powerKw;         /* Moc w kW */
		double powerKwFiltered; /* Moc w kW po filtrze */
};

class DynoRunResult {
	private:
		float _rpmRatio;
		int _weight;
		int _lossesSkipNum;
		int _lossesStartIndex;
		int _lossesCnt;
		double _correctionFactor;

		QList<DynoRunResultItem *> _items;
		KalmanFilter _accFilter;
		KalmanFilter _pwrFilter;
		KalmanFilter _speedFilter;

		double _lossA;
		double _lossB;
		double _lossC;

		void _calculateForceAndPower(DynoRunResultItem * item);
		void _recalcutateItem(int idx);
		void _recalculateLosses(void);
	public:
		DynoRunResult();
		~DynoRunResult();

		void setParameters(float rpmRatio, int weight, int lossesSkipNum = 8);
		float rpmRatio(void);
		int weight(void);

		DynoRunResultItem * addData(double time, double speed);
		DynoRunResultItem * item(int idx);
		int resultsCount(void);
		int lossesCount(void);
		int itemsCount(void);

		double lossAt(int rpm);

};

#endif // DYNORUNRESULT_H
