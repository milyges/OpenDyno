#ifndef DYNOCHARTVIEW_H
#define DYNOCHARTVIEW_H

#include <QChartView>
#include <QObject>
#include "dynochartviewinfo.h"
#include "dynochartviewlegend.h"
#include "dynorun.h"

class DynoChartView : public QChartView {
		Q_OBJECT
	public:
		DynoChartView(QChart * chart, QWidget * parent = nullptr);
		~DynoChartView();

		void updateUserInfo(QString text);

	protected:
		void mouseMoveEvent(QMouseEvent *event);
		void resizeEvent(QResizeEvent *event);

	private:
		QChart * _chart;
		DynoChartViewInfo * _infoBox;
		DynoChartViewLegend * _legend;

		QGraphicsTextItem * _userInfoLabel;

	signals:
		void hoveredAtX(float x);
};


#endif // DYNOCHARTVIEW_H
