#ifndef DYNOCHARTVIEW_H
#define DYNOCHARTVIEW_H

#include <QChartView>
#include <QObject>
#include "dynochartviewinfo.h"
#include "dynochartviewlegend.h"
#include "dynorun.h"

class DynoChartViewRunInfo {
	private:
		QChart * _chart;
		QGraphicsTextItem * _text;
		QGraphicsLineItem * _line;

	public:
		DynoChartViewRunInfo(QChart * chart, DynoRun * run, int number);
		~DynoChartViewRunInfo();

		void setPos(qreal x, qreal y);
		qreal height(void);
};

class DynoChartView : public QChartView {
		Q_OBJECT
	public:
		DynoChartView(QChart * chart, QWidget * parent = nullptr);
		~DynoChartView();

		void updateUserInfo(QString text);
		void addRunInfo(DynoRun * run, int number);
		void removeRunInfo(void);
		void hideInfoBox(void);

	protected:
		void mouseMoveEvent(QMouseEvent *event);
		void resizeEvent(QResizeEvent *event);

	private:
		QChart * _chart;
		DynoChartViewInfo * _infoBox;
		DynoChartViewLegend * _legend;

		QGraphicsTextItem * _userInfoLabel;
		QList<DynoChartViewRunInfo *> _runInfo;

	signals:
		void hoveredAtX(float x);
};


#endif // DYNOCHARTVIEW_H
