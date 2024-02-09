#include "dynorunresult.h"
#include <QDebug>
#include "dynosettings.h"

void DynoRunResult::_calculateForceAndPower(DynoRunResultItem * item) {
	item->force = item->accFiltered * _weight;
	item->powerKw = item->force * item->speedMs / 1000 * _correctionFactor;
	item->powerKwFiltered = _pwrFilter.getFiltered(item->powerKw);
}

/* Funkcja ogarnia obliczenia, średniego przyśpieszenia, filtowanego przyśpieszenia, mocy, mocy po filtrze, itp.
 * WAŻNE: z racji że korzysta z filtrowania musi być wywoływana pokolei, tj (0), (1).... */
void DynoRunResult::_recalcutateItem(int idx) {
	DynoRunResultItem * item;
	DynoRunResultItem * prevItem = nullptr;
	DynoRunResultItem * nextItem = nullptr;
	if (idx < 0) {
		return;
	}

	item = _items[idx];
	if ((idx == 0) || (idx == _items.size() - 1)) {
		item->accFiltered = item->acc;
		item->accMean = item->acc;
	}
	else {
		item = _items[idx];
		prevItem = _items[idx - 1];
		nextItem = _items[idx + 1];

		/* Uśrednienie z 3 pomiarów przyśpieszenia */
		item->accMean = (prevItem->acc + item->acc + nextItem->acc) / 3;
		//item->accMean = item->acc;
		item->accFiltered = /*item->accMean; //*/_accFilter.getFiltered(item->accMean);
	}

	_calculateForceAndPower(item);

	if (item->powerKw < 0) { /* Początek strat? */
		_lossesCnt++;
		if ((_lossesCnt > 3) && (_lossesStartIndex < 0)) {
			_lossesStartIndex = _items.size() - _lossesCnt;
		}

		_recalculateLosses();
	}
	else if (_lossesStartIndex < 0) {
		_lossesCnt = 0;
	}

	//qDebug() << "idx:" << idx << "time:" << item->time << "speed:" << item->speed << "rpm:" << item->rpm << "accFiltered:" << item->accFiltered << "force:" << item->force << "powerKw:" << item->powerKw << "powerKwFiltered:" << item->powerKwFiltered;
}

void DynoRunResult::_recalculateLosses() {
	/* Regresja dla strat */
    if (_lossesCnt <= 10) {
		return; /* Nie pobrano wystarczającej ilości danych */
	}

	int i;
	DynoRunResultItem * item;
	int base = _lossesStartIndex + _lossesSkipNum;
	int cnt = _lossesCnt - _lossesSkipNum - 1;
	double x[cnt];
	double y[cnt];
	double lny[cnt];
	double xsum = 0, x2sum = 0, ysum = 0, xysum = 0;

	/* Ustalamy równanie krzywej regresji wykładniczej, do obliczania strat
	 * kod znaleziony w internecie, z drobnymi zmianami */
	for(i = 0; i < cnt; i++) {
		item = _items[base + i];
		x[i] = item->rpm;
		y[i] = -item->powerKw;
		lny[i] = log(y[i]);
	}

	for (i = 0; i < cnt; i++) {
		xsum += x[i];
		ysum += lny[i];
		x2sum += pow(x[i], 2);
		xysum += x[i] * lny[i];
	}

	_lossA = (cnt * xysum - xsum * ysum) / (cnt * x2sum - xsum * xsum);   //calculate slope(or the the power of exp)
	_lossB = (x2sum * ysum - xsum * xysum) / (x2sum * cnt - xsum * xsum); //calculate intercept
	_lossC = pow(2.71828, _lossB);                                        //since b=ln(c)
}

DynoRunResult::DynoRunResult() {
	_lossesStartIndex = -1;
	_lossesCnt = 0;
	_weight = 1;
	_rpmRatio = 1.0f;

	/* Wartości filtrów dobrane eksperymentalnie */
	//_correctionFactor = 1.04;
	_correctionFactor = DynoSettings::getInstance()->correctionFactor();
	_accFilter = KalmanFilter();
	//_accFilter.init(8, 1, 1, 5, 0.1);
	_accFilter.init(4, 1, 1, DynoSettings::getInstance()->filterPower(), 0.3);

	_pwrFilter = KalmanFilter();
	//_pwrFilter.init(10, 1, 1, 15, 0.1);
	_pwrFilter.init(10, 1, 1, 15, 0.3);
}

DynoRunResult::~DynoRunResult() {
	for(int i = 0; i < _items.size(); i++) {
		delete _items[i];
	}
	_items.clear();
}

void DynoRunResult::setParameters(float rpmRatio, int weight, int lossesSkipNum) {
	_rpmRatio = rpmRatio;
	_weight = weight;
	_lossesSkipNum = lossesSkipNum;

	/* Reset filtra kalmana dla mocy */
	_pwrFilter.reset(0);

	/* Rekalkukacja po zmianie parametrów: moc, rpm */
	for(int i = 0; i < _items.size(); i++) {
		_items[i]->rpm = round(_items[i]->speed * _rpmRatio);
		_calculateForceAndPower(_items[i]);
	}

	_recalculateLosses();
}

float DynoRunResult::rpmRatio() {
	return _rpmRatio;
}

int DynoRunResult::weight() {
	return _weight;
}

DynoRunResultItem * DynoRunResult::addData(double time, double speed) {
	DynoRunResultItem * item;
	DynoRunResultItem * prevItem = nullptr;

	int lastIdx = _items.size() - 1;
	if (_items.size() > 0) {
		prevItem = _items.last();
	}

	item = new DynoRunResultItem();
	item->time = time;
	item->speed = speed;
	item->speedMs = speed * 1000 / 3600;
	item->rpm = round(item->speed * _rpmRatio);

	if (prevItem) {
		item->acc = (item->speedMs - prevItem->speedMs) / (item->time - prevItem->time);
	}
	else {
		item->acc = 0;
	}

	_items.append(item);

	/* Przeliczamy ale poprzedni wpis */
	_recalcutateItem(lastIdx);

	return item;
}

DynoRunResultItem * DynoRunResult::item(int idx) {
	return _items[idx];
}

int DynoRunResult::resultsCount() {
	return _lossesStartIndex >= 0 ? _lossesStartIndex : _items.size() - 1;
}

int DynoRunResult::lossesCount() {
	return _lossesStartIndex >= 0 ? _items.size() - _lossesStartIndex - 1: 0;
}

int DynoRunResult::itemsCount() {
	return _items.size() - 1;
}

double DynoRunResult::lossAt(int rpm) {
    return _lossC * pow(2.71828, _lossA * rpm);
}
