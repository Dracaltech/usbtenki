#include <iostream>
#include <QSpinBox>
#include <QGroupBox>
#include <QFileDialog>
#include "GraphView.h"
#include "ConfigCheckbox.h"
#include "qcustomplot.h"
#include "globals.h"

GraphView::GraphView()
{
	QSettings settings;

	lay = new QVBoxLayout();
	this->setLayout(lay);
	

	x_count = 0;
	plt = new QCustomPlot(this);
	
	plt->xAxis->setLabel("Samples");
	plt->yAxis->setLabel("Value");
	plt->setRangeDrag(Qt::Horizontal);
	plt->setRangeZoom(Qt::Horizontal);
	plt->setInteractions(QCustomPlot::iRangeZoom | QCustomPlot::iRangeDrag);

	connect(plt, SIGNAL(titleClick(QMouseEvent*)), this, SLOT(editTitle()));
	
	QFont legendFont = font();
	legendFont.setPointSize(9);
	plt->legend->setVisible(true);
	plt->legend->setFont(legendFont);
	plt->legend->setPositionStyle(QCPLegend::psBottomRight);

	plt->setAutoAddPlottableToLegend(true);
	plt->setTitle(tr("Untitled graph"));
	plt->setNoAntialiasingOnDrag(true);
	plt->setNotAntialiasedElements(QCP::aeAll);

	g_tenkisources->addSourcesTo(this);
	

	//////////////////////////
	QGroupBox *graph_opts = new QGroupBox(tr("Operations"));
	graph_opts->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	QHBoxLayout *graph_opts_lay = new QHBoxLayout();
	graph_opts->setLayout(graph_opts_lay);

	btn_pause_continue = new QPushButton(tr("Pause"));
	connect(btn_pause_continue, SIGNAL(clicked()), this, SLOT(pause_unpause()));

	QPushButton *btn_reset = new QPushButton(tr("Reset graph"));
	connect(btn_reset, SIGNAL(clicked()), this, SLOT(resetGraph()));
	
	QPushButton *btn_save = new QPushButton(tr("Save graph to file..."));
	connect(btn_save, SIGNAL(clicked()), this, SLOT(saveGraph()));
	

	// Sample interval
	QSpinBox *sample_interval = new QSpinBox();
	sample_interval->setMinimum(100);
	sample_interval->setMaximum(60000);
	sample_interval->setValue(settings.value("graph/sample_interval_ms", 1000).toInt());
	connect(sample_interval, SIGNAL(valueChanged(int)), this, SLOT(intervalChanged(int)));

	graph_opts_lay->addWidget(btn_pause_continue);
	graph_opts_lay->addWidget(btn_reset);
	graph_opts_lay->addWidget(btn_save);
	graph_opts_lay->addStretch();
	
	////////////////////////// OPTIONS
	QGroupBox *graph_opts2 = new QGroupBox(tr("Options"));
	graph_opts2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	QHBoxLayout *graph_opts_lay2 = new QHBoxLayout();
	graph_opts2->setLayout(graph_opts_lay2);
	

	graph_rescale_x = new ConfigCheckbox(tr("Auto-scale X axis"), "graph/autoscale_x");
	graph_rescale_y = new ConfigCheckbox(tr("Auto-scale Y axis"), "graph/autoscale_y");

	connect(graph_rescale_x, SIGNAL(changed()), this, SLOT(replot()));
	connect(graph_rescale_y, SIGNAL(changed()), this, SLOT(replot()));

	// Graph legend
	graph_legend_pref = new GraphLegendPreference();
	connect(graph_legend_pref, SIGNAL(changed()), this, SLOT(replot()));

	graph_opts_lay2->addWidget(new QLabel(tr("Graph legend:")));
	graph_opts_lay2->addWidget(graph_legend_pref);

	graph_opts_lay2->addWidget(graph_rescale_x);
	graph_opts_lay2->addWidget(graph_rescale_y);

	graph_opts_lay2->addWidget(new QLabel(tr("Sample interval (ms):")));
	graph_opts_lay2->addWidget(sample_interval);


	
	
	lay->addWidget(plt);
	lay->addWidget(graph_opts2);
	lay->addWidget(graph_opts);

	plt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	graph_opts->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	
	replot();

	sample_timer = new QTimer();
	sample_timer->setInterval(sample_interval->value());
	connect(sample_timer, SIGNAL(timeout()), this, SLOT(refreshView()));
	sample_timer->start();
}

GraphView::~GraphView(void)
{
}

void GraphView::editTitle()
{
	QString new_title;
	bool ok = false;

	new_title = QInputDialog::getText(this, tr("Enter graph title"), tr("Title:"), 
									QLineEdit::Normal, plt->title() ,  &ok);

	new_title = new_title.trimmed();
	if (ok && !new_title.isEmpty()) {
		plt->setTitle(new_title);
		replot();
	}
}

void GraphView::saveGraph(void)
{
	QString filename;
	QString default_dir = QDir::homePath();

	filename = QFileDialog::getSaveFileName(this, tr("Save graph to file"), default_dir,
		"PNG (*.png);; JPEG (*.jpg *.jpeg);; BMP (*.bmp);; PDF (*.pdf)");

	if (filename.size()) {
		qDebug() << filename;

		if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) {
			plt->saveJpg(filename);
		}
		if (filename.endsWith(".png")) {
			plt->savePng(filename);
		}
		if (filename.endsWith(".bmp")) {
			plt->saveBmp(filename);
		}
		if (filename.endsWith(".pdf")) {
			plt->savePdf(filename, true);
		}
	}
}

void GraphView::resetGraph(void)
{
	plt->clearGraphs();
	src_graphs.clear();
	for (int i=0; i<sources.size(); i++) {
		src_graphs.append(NULL);
	}
	x_count = 0;
}

void GraphView::addTenkiSource(struct sourceDescription *sd)
{
	addSourceByName(sd->q_name);
}

void GraphView::removeTenkiSource(struct sourceDescription *sd)
{
}

void GraphView::addSourceByName(QString sname)
{
	sources.append(sname);
	src_graphs.append(NULL);
}

void GraphView::refreshView()
{
	struct sourceDescription *sd;
	struct USBTenki_channel chndata;
	static int first=1;
	QSettings settings;
	QCPGraph *gr;
	QColor colors[10] = {
		Qt::green,
		Qt::blue,
		QColor(177,92,0),
		Qt::darkBlue,
		Qt::darkMagenta,
		Qt::darkRed,
		Qt::darkYellow,
		Qt::darkGray,
		Qt::cyan,
		Qt::black,
	};
	
	for (int i=0; i<sources.size(); i++)
	{
		if (!settings.value("graphChecked/"+sources.at(i)).toBool())
			continue;

		sd = g_tenkisources->getSourceByName(sources.at(i));
		QSettings settings;

		if (!sd) {
			return;
		}

		g_tenkisources->convertToUnits(sd->chn_data, &chndata);
		QString alias = sd->q_alias;
		QString units = QString::fromUtf8(unitToString(chndata.converted_unit, 0));
		QString d;
		
//		d.sprintf("%.3f",  chndata.converted_data );
//		qDebug() << d;

		// Ok, now we have our value.
		// Find if there is a pre-existing graph
		if (src_graphs.at(i)) {
			gr = src_graphs.at(i);
			if (alias.compare(gr->name()) != 0) {
				// Name changed
				gr->setName(alias);
			}
		} else {
			// Create it on the fly
			gr = plt->addGraph();
			gr->setName(alias);
			
			QPen p(colors[i%10]);
//			p.setWidth(2);

			gr->setPen(p);
			src_graphs.replace(i, gr);
		}
	
	/*	
		if (first)
		{
			int j;
			printf("Load test\n");
			for (j=0; j<5000; j++) {
				gr->addData(x_count++, chndata.converted_data + (j%10)*0.01);
			}
		}
		*/
		gr->addData(x_count, chndata.converted_data);
		if (x_count >= 100) {
			gr->removeData(x_count - 100);
			printf("Sliding window\n");
		}

	}

	Qt::Orientations orient = 0;

	if (graph_rescale_x->isChecked()) {
		for (int i=0; i<plt->graphCount(); i++) {
			plt->graph(i)->rescaleKeyAxis(i==0 ? false : true);
		}
	} else {
		orient |= Qt::Horizontal;
	}


	if (graph_rescale_y->isChecked()) {
		for (int i=0; i<plt->graphCount(); i++) {
			plt->graph(i)->rescaleValueAxis(i==0? false : true);
		}
	} else {
		orient |= Qt::Vertical;
	}

	plt->setRangeZoom(orient);
	plt->setRangeDrag(orient);

//		d.sprintf("%.3f",  chndata.converted_data );
//		qDebug() << d;
	QString lbl;
	lbl.sprintf("Samples (%d ms interval)", sample_timer->interval());
	plt->xAxis->setLabel(lbl);

	replot();
	
	x_count++;

}

void GraphView::replot(void)
{
	if (graph_legend_pref->getStyle() == QCPLegend::psManual) {
		plt->legend->setVisible(false);
	} else {
		plt->legend->setVisible(true);
		plt->legend->setPositionStyle(graph_legend_pref->getStyle());
	}
	plt->replot();
}

void GraphView::intervalChanged(int i)
{
	QSettings settings;
	settings.setValue("graph/sample_interval_ms", i);
	sample_timer->setInterval(i);
}

void GraphView::pause_unpause(void)
{
	is_paused = !is_paused;

	if (is_paused) {
		sample_timer->stop();
		btn_pause_continue->setText(tr("Unpause"));
	} else {
		sample_timer->start();
		btn_pause_continue->setText(tr("Pause"));
	}

}
