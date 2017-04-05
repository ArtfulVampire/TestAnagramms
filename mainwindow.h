#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QTextCodec>
#include <QPixmap>
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QEvent>
#include <QKeyEvent>
#include <QTextStream>

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <chrono>
#include <vector>
#include <valarray>
#include "answers.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

public slots:
	void nextPic();
	void newTest();
	void stop();
	void skip();
	void setName();

private:
	void newPic();
	void increment(const QString & message);

protected:
	void mousePressEvent(QMouseEvent *);
	void keyPressEvent(QKeyEvent *);

private:
	Ui::MainWindow *ui;
	std::vector<QString> answersArr;
	QString mixedWord;
    QString dirPath;
	QTime time1;

	int counter;
	std::ofstream outStr;
    QFile outFile;
    QTextStream outStream;
};

#endif // MAINWINDOW_H
