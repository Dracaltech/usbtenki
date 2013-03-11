#ifndef _graphview_h__
#define _graphview_h__

#include <QtGui>
#include <QWidget>

#include "ConfigCheckbox.h"
#include "TenkiSources.h"
#include "BigLabel.h"
#include "qcustomplot.h"
#include "GraphLegendPreference.h"

class GraphView : public QWidget, public TenkiSourceAddRemove
{
	Q_OBJECT

	public:
		GraphView();
		~GraphView(void);

		virtual void addTenkiSource(struct sourceDescription *sd);
		virtual void removeTenkiSource(struct sourceDescription *sd);

	public slots:
		void refreshView(void);
		void addSourceByName(QString src_name);
		void resetGraph(void);
		void saveGraph(void);
		void replot(void);
		void editTitle();
	
	private:
		QVBoxLayout *lay;
		QList<QString> sources;
		QList<QCPGraph*> src_graphs;
		ConfigCheckbox *graph_rescale;
		QCustomPlot *plt;
		GraphLegendPreference *graph_legend_pref;
		int x_count;
};

#endif 

