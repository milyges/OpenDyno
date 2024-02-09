#ifndef DYNOCHARTSERIESPEN_H
#define DYNOCHARTSERIESPEN_H

#include <QPen>

class DynoChartSeriesPen
{
	private:
		static const Qt::PenStyle _lineStyles[3];
		static const int _lineWidth[3];
	public:
		static QPen getPen(int runNumber);
};

#endif // DYNOCHARTSERIESPEN_H
