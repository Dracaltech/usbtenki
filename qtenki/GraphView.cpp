#include <iostream>
#include <QGroupBox>
#include <QFileDialog>
#include "GraphView.h"
#include "ConfigCheckbox.h"
#include "qcustomplot.h"
#include "globals.h"

GraphView::GraphView()
{
	lay = new QVBoxLayout();
	this->setLayout(lay);
	

	x_count = 0;
	plt = new QCustomPlot(this);
	
	plt->xAxis->setLabel("Samples");
	plt->yAxis->setLabel("Value");
	plt->setRangeDrag(Qt::Horizontal | Qt::Vertical);
	plt->setRangeZoom(Qt::Horizontal | Qt::Vertical);
	plt->setInteractions(QCustomPlot::iRangeZoom | QCustomPlot::iRangeDrag);

	connect(plt, SIGNAL(titleClick(QMouseEvent*)), this, SLOT(editTitle()));
	
	QFont legendFont = font();
	legendFont.setPointSize(9);
	plt->legend->setVisible(true);
	plt->legend->setFont(legendFont);
	plt->legend->setPositionStyle(QCPLegend::psBottomRight);
	plt->setAutoAddPlottableToLegend(true);
	plt->setTitle(tr("Untitled graph"));

	g_tenkisources->addSourcesTo(this);
	

	QGroupBox *graph_opts = new QGroupBox(tr("Graph options"));
	graph_opts->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	QHBoxLayout *graph_opts_lay = new QHBoxLayout();
	graph_opts->setLayout(graph_opts_lay);

	QPushButton *btn_reset = new QPushButton(tr("Reset graph"));
	connect(btn_reset, SIGNAL(clicked()), this, SLOT(resetGraph()));
	
	QPushButton *btn_save = new QPushButton(tr("Save graph to file..."));
	connect(btn_save, SIGNAL(clicked()), this, SLOT(saveGraph()));
	
	graph_rescale = new ConfigCheckbox(tr("Auto-scale axes"), "graph/autoscale");

	connect(graph_rescale, SIGNAL(changed()), this, SLOT(replot()));

	graph_legend_pref = new GraphLegendPreference();

	connect(graph_legend_pref, SIGNAL(changed()), this, SLOT(replot()));
	
	graph_opts_lay->addWidget(btn_reset);
	graph_opts_lay->addWidget(btn_save);
	graph_opts_lay->addWidget(graph_rescale);
	graph_opts_lay->addWidget(new QLabel(tr("Graph legend:")));
	graph_opts_lay->addWidget(graph_legend_pref);

	graph_opts_lay->addStretch();
	
	lay->addWidget(plt);
	lay->addWidget(graph_opts);

	plt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	graph_opts->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

	plt->legend->setPositionStyle(graph_legend_pref->getStyle());
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
		
		d.sprintf("%.3f",  chndata.converted_data );
		qDebug() << d;

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
			p.setWidth(2);

			gr->setPen(p);
			src_graphs.replace(i, gr);
		}
		
		gr->addData(x_count, chndata.converted_data);

	}

	if (graph_rescale->isChecked()) {
		plt->rescaleAxes();
	}

	replot();
	
	x_count++;

}

void GraphView::replot(void)
{
	plt->legend->setPositionStyle(graph_legend_pref->getStyle());
	plt->replot();
}
