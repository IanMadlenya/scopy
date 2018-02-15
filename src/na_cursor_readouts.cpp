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

#include "na_cursor_readouts.h"
#include "ui_na_cursor_readouts.h"

#include <qwt_plot.h>

NaCursorReadouts::NaCursorReadouts(QwtPlot *plot) :
	QWidget(plot),
	ui(new Ui::NaCursorReadouts)
{
	ui->setupUi(this);
}

NaCursorReadouts::~NaCursorReadouts()
{
	delete ui;
}

void NaCursorReadouts::setFreqCursor1Text(const QString& text)
{
	ui->frequency1->setText(text);
}

QString NaCursorReadouts::FreqCursor1Text()
{
	return ui->frequency1->text();
}
void NaCursorReadouts::setFreqCursor2Text(const QString& text)
{
	ui->frequency2->setText(text);
}

QString NaCursorReadouts::FreqCursor2Text()
{
	return ui->frequency2->text();
}

void NaCursorReadouts::setCustomLabel1(const QString& text)
{
	ui->customLabel1->setText(text + "= ");
}

QString NaCursorReadouts::customLabel1()
{
	return ui->customLabel1->text();
}

void NaCursorReadouts::setCustomLabel2(const QString& text)
{
	ui->customLabel2->setText(text + "= ");
}

QString NaCursorReadouts::customLabel2()
{
	return ui->customLabel2->text();
}

void NaCursorReadouts::setCustomValue1(const QString& value)
{
	ui->customValue1->setText(value);
}

QString NaCursorReadouts::customValue1()
{
	return ui->customValue1->text();
}

void NaCursorReadouts::setCustomValue2(const QString& value)
{
	ui->customValue2->setText(value);
}

QString NaCursorReadouts::customValue2()
{
	return ui->customValue2->text();
}

void NaCursorReadouts::setDeltaValue(const QString& text)
{
	ui->deltaValue->setText(text);
}

QString NaCursorReadouts::NaCursorReadouts::deltaValue()
{
	return ui->deltaValue->text();
}

void NaCursorReadouts::setDeltaLabel(const QString& text)
{
	ui->deltaLabel->setText(text + "= ");
}

QString NaCursorReadouts::deltaLabel()
{
	return ui->deltaLabel->text();
}
