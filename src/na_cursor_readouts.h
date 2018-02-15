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

#ifndef NA_CURSOR_READOUTS_H
#define NA_CURSOR_READOUTS_H

#include <QWidget>

class QwtPlot;

namespace Ui {
class NaCursorReadouts;
}

class NaCursorReadouts : public QWidget
{
	Q_OBJECT

public:
	NaCursorReadouts(QwtPlot *plot);
	~NaCursorReadouts();

	void setFreqCursor1Text(const QString&);
	QString FreqCursor1Text();
	void setFreqCursor2Text(const QString&);
	QString FreqCursor2Text();
	void setCustomLabel1(const QString&);
	QString customLabel1();
	void setCustomLabel2(const QString&);
	QString customLabel2();
	void setCustomValue1(const QString&);
	QString customValue1();
	void setCustomValue2(const QString&);
	QString customValue2();
	void setDeltaValue(const QString&);
	QString deltaValue();
	void setDeltaLabel(const QString&);
	QString deltaLabel();

private:
	Ui::NaCursorReadouts *ui;
};

#endif // NA_CURSOR_READOUTS_H
