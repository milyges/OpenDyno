#ifndef DYNOCHARTVIEW_H
#define DYNOCHARTVIEW_H

#include <QChartView>
#include <QObject>
#include "dynochartviewinfo.h"
#include "dynochartviewlegend.h"

class DynoChartView : public QChartView {
		Q_OBJECT
	public:
		DynoChartView(QChart * chart, QWidget * parent = nullptr);
		~DynoChartView();

	protected:
		void mouseMoveEvent(QMouseEvent *event);
		void resizeEvent(QResizeEvent *event);

	private:
		QChart * _chart;
		DynoChartViewInfo * _infoBox;
		DynoChartViewLegend * _legend;
	signals:
		void hoveredAtX(float x);
};


#endif // DYNOCHARTVIEW_H
