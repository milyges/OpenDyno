#include "dynochartseriespen.h"

const Qt::PenStyle DynoChartSeriesPen::_lineStyles[3] = { Qt::SolidLine, Qt::SolidLine, Qt::DashLine };
const int DynoChartSeriesPen::_lineWidth[3] = { 2, 1, 1 };

QPen DynoChartSeriesPen::getPen(int runNumber) {
	QPen p;
	p.setStyle(_lineStyles[runNumber]);
	p.setWidth(_lineWidth[runNumber]);
	return p;
}
