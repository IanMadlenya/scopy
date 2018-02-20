/*
 * Copyright 2017 Analog Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file LICENSE.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "dbgraph.hpp"
#include "DisplayPlot.h"
#include "osc_scale_engine.h"
#include "osc_scale_zoomer.h"

#include "plotpickerwrapper.h"

#include <qwt_plot_layout.h>

using namespace adiscope;

dBgraph::dBgraph(QWidget *parent) : QwtPlot(parent),
	curve("data")
{
	enableAxis(QwtPlot::xBottom, false);
	enableAxis(QwtPlot::xTop, true);

	setAxisAutoScale(QwtPlot::yLeft, false);
	setAxisAutoScale(QwtPlot::xTop, false);

	EdgelessPlotGrid *grid = new EdgelessPlotGrid;
	grid->setMajorPen(QColor("#353537"), 1.0, Qt::DashLine);
	grid->setXAxis(QwtPlot::xTop);
	grid->attach(this);

	plotLayout()->setAlignCanvasToScales(true);

	curve.attach(this);
	curve.setXAxis(QwtPlot::xTop);
	curve.setYAxis(QwtPlot::yLeft);

	useLogFreq(false);

	OscScaleEngine *scaleLeft = new OscScaleEngine;
	scaleLeft->setMajorTicksCount(6);
	this->setAxisScaleEngine(QwtPlot::yLeft,
	                         static_cast<QwtScaleEngine *>(scaleLeft));

	/* draw_x / draw_y: Outmost X / Y scales. Only draw the labels */
	formatter = static_cast<PrefixFormatter *>(new MetricPrefixFormatter);
	draw_x = new OscScaleDraw(formatter, "Hz");
	draw_x->setFloatPrecision(2);
	draw_x->enableComponent(QwtAbstractScaleDraw::Ticks, false);
	draw_x->enableComponent(QwtAbstractScaleDraw::Backbone, false);
	setAxisScaleDraw(QwtPlot::xTop, draw_x);

	draw_y = new OscScaleDraw("dB");
	draw_y->setFloatPrecision(0);
	draw_y->enableComponent(QwtAbstractScaleDraw::Ticks, false);
	draw_y->enableComponent(QwtAbstractScaleDraw::Backbone, false);
	setAxisScaleDraw(QwtPlot::yLeft, draw_y);

	/* Create 4 scales within the plot itself. Only draw the ticks */
	for (unsigned int i = 0; i < 4; i++) {
		EdgelessPlotScaleItem *scaleItem = new EdgelessPlotScaleItem(
		        static_cast<QwtScaleDraw::Alignment>(i));

		/* Top/bottom scales must be sync'd to xTop; left/right scales
		 * must be sync'd to yLeft */
		if (i < 2) {
			scaleItem->setXAxis(QwtPlot::xTop);
		} else {
			scaleItem->setYAxis(QwtPlot::yLeft);
		}

		scaleItem->scaleDraw()->enableComponent(
		        QwtAbstractScaleDraw::Backbone, false);
		scaleItem->scaleDraw()->enableComponent(
		        QwtAbstractScaleDraw::Labels, false);

		QPalette palette = scaleItem->palette();
		palette.setBrush(QPalette::Foreground, QColor("#6E6E6F"));
		palette.setBrush(QPalette::Text, QColor("#6E6E6F"));
		scaleItem->setPalette(palette);
		scaleItem->setBorderDistance(0);
		scaleItem->attach(this);
	}

	zoomer = new OscScaleZoomer(canvas());
	zoomer->setMousePattern(QwtEventPattern::MouseSelect3,
	                        Qt::RightButton);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2,
	                        Qt::RightButton, Qt::ControlModifier);

	static_cast<QFrame *>(canvas())->setLineWidth(0);
	setContentsMargins(10, 10, 24, 20);

	d_bottomHandlesArea = new HorizHandlesArea(this->canvas());
	d_bottomHandlesArea->setLeftPadding(80);
	d_bottomHandlesArea->setRightPadding(20);
	d_bottomHandlesArea->setMinimumHeight(50);

	d_symbolCtrl = new SymbolController(this);

	d_hCursorHandle1 = new PlotLineHandleH(
	        QPixmap(":/icons/h_cursor_handle.svg"),
	        d_bottomHandlesArea);
	d_hCursorHandle2 = new PlotLineHandleH(
	        QPixmap(":/icons/h_cursor_handle.svg"),
	        d_bottomHandlesArea);
	d_vBar1 = new VertBar(this,true);
	d_vBar2 = new VertBar(this,true);
	d_symbolCtrl->attachSymbol(d_vBar1);
	d_symbolCtrl->attachSymbol(d_vBar2);

	QPen cursorsLinePen = QPen(QColor(155,155,155),1,Qt::DashLine);

	d_vBar1->setPen(cursorsLinePen);
	d_vBar2->setPen(cursorsLinePen);
	d_hCursorHandle1->setPen(cursorsLinePen);
	d_hCursorHandle2->setPen(cursorsLinePen);
	d_vBar1->setVisible(false);
	d_vBar2->setVisible(false);
	d_hCursorHandle1->setVisible(false);
	d_hCursorHandle2->setVisible(false);

	connect(d_hCursorHandle1, SIGNAL(positionChanged(int)),
	        d_vBar1, SLOT(setPixelPosition(int)));
	connect(d_vBar1, SIGNAL(pixelPositionChanged(int)),
	        SLOT(onVbar1PixelPosChanged(int)));

	connect(d_hCursorHandle2, SIGNAL(positionChanged(int)),
	        d_vBar2, SLOT(setPixelPosition(int)));
	connect(d_vBar2, SIGNAL(pixelPositionChanged(int)),
	        SLOT(onVbar2PixelPosChanged(int)));

	connect(d_vBar1, SIGNAL(pixelPositionChanged(int)),
	        SLOT(onCursor1Moved(int)));
	connect(d_vBar2, SIGNAL(pixelPositionChanged(int)),
	        SLOT(onCursor2Moved(int)));


	d_naCursorReadouts = new NaCursorReadouts(this);
	d_naCursorReadouts->setVisible(false);

	picker = new PlotPickerWrapper(QwtPlot::xTop,QwtPlot::yLeft,this->canvas());

}

dBgraph::~dBgraph()
{
	delete formatter;
	delete picker;
}

QWidget *dBgraph::bottomHandlesArea()
{
	return d_bottomHandlesArea;
}

void dBgraph::setAxesScales(double xmin, double xmax, double ymin, double ymax)
{
	setAxisScale(QwtPlot::xTop, xmin, xmax);
	setAxisScale(QwtPlot::yLeft, ymin, ymax);
}

void dBgraph::setAxesTitles(const QString& x, const QString& y)
{
	setAxisTitle(QwtPlot::xTop, x);
	setAxisTitle(QwtPlot::yLeft, y);
}

void dBgraph::plot(double x, double y)
{
	if (xdata.size() == numSamples + 1) {
		return;
	}

	xdata.push(x);
	ydata.push(y);

	curve.setRawSamples(xdata.data(), ydata.data(), xdata.size());
	replot();
}

int dBgraph::getNumSamples() const
{
	return numSamples;
}

void dBgraph::setNumSamples(int num)
{
	numSamples = (unsigned int) num;

	reset();
	ydata.reserve(numSamples + 1);
	xdata.reserve(numSamples + 1);

	replot();
}

void dBgraph::reset()
{
	xdata.clear();
	ydata.clear();
}

void dBgraph::setColor(const QColor& color)
{
	this->color = color;
	curve.setPen(QPen(color));
}

const QColor& dBgraph::getColor() const
{
	return this->color;
}

QString dBgraph::xTitle() const
{
	return axisTitle(QwtPlot::xTop).text();
}

QString dBgraph::yTitle() const
{
	return axisTitle(QwtPlot::yLeft).text();
}

void dBgraph::setXTitle(const QString& title)
{
	setAxisTitle(QwtPlot::xTop, title);
}

void dBgraph::setYTitle(const QString& title)
{
	setAxisTitle(QwtPlot::yLeft, title);
	d_naCursorReadouts->setCustomLabel1("Cur"+title.mid(0,3)+"1");
	d_naCursorReadouts->setCustomLabel2("Cur"+title.mid(0,3)+"2");
	d_naCursorReadouts->setDeltaLabel("Δ"+title.mid(0,3));
}

void dBgraph::setXMin(double val)
{
	zoomer->resetZoom();
	setAxisScale(QwtPlot::xTop, val, xmax);
	xmin = val;
	draw_x->invalidateCache();

	zoomer->setZoomBase();
	replot();
}

void dBgraph::setXMax(double val)
{
	zoomer->resetZoom();
	setAxisScale(QwtPlot::xTop, xmin, val);
	xmax = val;
	draw_x->invalidateCache();

	zoomer->setZoomBase();
	replot();
}

void dBgraph::setYMin(double val)
{
	zoomer->resetZoom();
	setAxisScale(QwtPlot::yLeft, val, ymax);
	ymin = val;

	zoomer->setZoomBase();
	replot();
}

void dBgraph::setYMax(double val)
{
	zoomer->resetZoom();
	setAxisScale(QwtPlot::yLeft, ymin, val);
	ymax = val;

	zoomer->setZoomBase();
	replot();
}

QString dBgraph::xUnit() const
{
	return draw_x->getUnitType();
}

QString dBgraph::yUnit() const
{
	return draw_y->getUnitType();
}

void dBgraph::setXUnit(const QString& unit)
{
	draw_x->setUnitType(unit);
}

void dBgraph::setYUnit(const QString& unit)
{
	draw_y->setUnitType(unit);
}

void dBgraph::useLogFreq(bool use_log_freq)
{
	if (use_log_freq) {
		this->setAxisScaleEngine(QwtPlot::xTop, new QwtLogScaleEngine);
	} else {
		auto scaleTop = new OscScaleEngine;
		scaleTop->setMajorTicksCount(8);
		this->setAxisScaleEngine(QwtPlot::xTop, scaleTop);
	}

	this->log_freq = use_log_freq;
	replot();
}

void dBgraph::onVbar1PixelPosChanged(int pos)
{
	d_hCursorHandle1->setPositionSilenty(pos);
}

void dBgraph::onVbar2PixelPosChanged(int pos)
{
	d_hCursorHandle2->setPositionSilenty(pos);
}

void dBgraph::toggleCursors(bool en)
{
	if (d_cursorsEnabled != en) {
		d_cursorsEnabled = en;
		d_vBar1->setVisible(en);
		d_vBar2->setVisible(en);
		d_hCursorHandle1->setVisible(en);
		d_hCursorHandle2->setVisible(en);

		d_naCursorReadouts->setVisible(en);
	}
}

void dBgraph::onCursor1Moved(int value)
{
	QString text;

	auto point = picker->pointCoordinates(QPoint(value,0));
	text = draw_x->label(point.x()).text();

	d_naCursorReadouts->setFreqCursor1Text(text);
	d_naCursorReadouts->setCustomValue1(cursorIntersection(point.x()));

	int d1 = d_naCursorReadouts->customValue1().split(" ")[0].toInt();
	int d2 = d_naCursorReadouts->customValue2().split(" ")[0].toInt();

	d_naCursorReadouts->setDeltaValue(QString::number(d2-d1) +" "+
	                                  draw_y->getUnitType());
}

void dBgraph::onCursor2Moved(int value)
{
	QString text;

	auto point = picker->pointCoordinates(QPoint(value,0));
	text = draw_x->label(point.x()).text();

	d_naCursorReadouts->setFreqCursor2Text(text);
	d_naCursorReadouts->setCustomValue2(cursorIntersection(point.x()));

	int d1 = d_naCursorReadouts->customValue1().split(" ")[0].toInt();
	int d2 = d_naCursorReadouts->customValue2().split(" ")[0].toInt();

	d_naCursorReadouts->setDeltaValue(QString::number(d2-d1)+" "+
	                                  draw_y->getUnitType());
}

QString dBgraph::cursorIntersection(qreal freq)
{
	if (xdata.size() == 0 || xdata.data()[xdata.size()-1] < freq) {
		return QString("-");//for the case when there is no plot
	} else {
		double leftFreq,rightFreq,leftCustom,rightCustom;
		int rightIndex = -1;
		int leftIndex = -1;

		for (int i=1; i<xdata.size(); i++) {
			if (xdata.data()[i-1] <= freq && freq <= xdata.data()[i]) {
				leftIndex=i-1;
				rightIndex=i;
			}
		}

		if (leftIndex == -1 || rightIndex == -1) {
			return QString("-");
		}

		if (!log_freq) {
			leftFreq = xdata.data()[leftIndex];
			rightFreq = xdata.data()[rightIndex];
		} else {
			freq = log10(freq);
			leftFreq = log10(xdata.data()[leftIndex]);
			rightFreq = log10(xdata.data()[rightIndex]);
		}

		leftCustom = ydata.data()[leftIndex];
		rightCustom = ydata.data()[rightIndex];

		double val = (rightCustom - leftCustom)/(rightFreq - leftFreq)*
		             (freq-leftFreq)+leftCustom;

		return QString::number(val,'f',0) +" "+ draw_y->getUnitType();
	}
}
