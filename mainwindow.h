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
#include <QThread>
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
#include <thread>
#include <set>

#include "answers.h"

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
	void startTest();
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
	static double hashFunc(const QString & answer,
						   const QString & word,
						   double time,
						   int wordNum, int prevSiz);

public:
	static void testFileHash(const QString & filePath);

private:
	Ui::MainWindow *ui;
	std::vector<QString> answersArr;
	std::set<int> alreadyAnswered;
	const QString answeredFileName{"answered.txt"};
	const QString outFileName{"out.txt"};
	std::vector<int> mixNum;
	QString mixedWord;
	QString dirPath{"."};
	QTime time1;
	std::thread timerThread{};

	int counter;
	std::ofstream outStr;
    QFile outFile;
    QTextStream outStream;
};

#endif // MAINWINDOW_H
