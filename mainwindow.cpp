#include "mainwindow.h"
#include "ui_mainwindow.h"

ostream & operator << (ostream & os, const QString & str)
{
	os << str.toStdString();
	return os;
}

std::vector<QString> readWordsFile(const QString & filePath)
{
	std::vector<QString> words;

	QFile fil(filePath);
	if(!fil.open(QIODevice::ReadOnly))
	{
		std::cout << "bad file" << std::endl;
		return {};
	}

	QString tmp;
	while(!fil.atEnd())
	{
		tmp = fil.readLine();
		tmp.resize(tmp.size() - 1);
		words.push_back(tmp);
	}
	return words;
}

QString mixWord(const QString & inWord)
{
	std::valarray<int> mixNum(inWord.size());
	std::iota(std::begin(mixNum),
			  std::end(mixNum),
			  0);

	int num = 0;

	do
	{
		std::shuffle(std::begin(mixNum),
					 std::end(mixNum),
					 std::default_random_engine(
						 std::chrono::system_clock::now().time_since_epoch().count()));
		auto nexts = mixNum.cshift(1) - mixNum; /// 1 if next letter is from right order
		auto prevs = mixNum - mixNum.cshift(-1); /// 1 if prev letter is from right order

		num = 0;
		for(int i = 0; i < mixNum.size() - 1; ++i)
		{
			if(nexts[i] == 1) { num += 1; }
			if(prevs[i + 1] == 1) { num += 1; }
		}

		if(mixNum[0] == 0 || mixNum[mixNum.size() - 1] == mixNum.size() - 1)
		{
			num += 10;
		}



	} while (num > 0);

	QString res = inWord;
	for(int i = 0; i < inWord.size(); ++i)
	{
		res[i] = inWord[mixNum[i]];
	}
	return res;
}

QPixmap drawWord(const QString & inWord)
{
	static const double FontSpace = 120.;
	static const QFont myFont = QFont("Courier", FontSpace, QFont::Normal);
	static const int wordWidth = QFontMetrics(myFont).width(inWord);
	static const int letterHeight = QFontMetrics(myFont).xHeight();

	static const int picWidth = 1280;
	static const int picHeight = 1024;

	static const int offsetX = picWidth / 2 - wordWidth / 2;
	static const int offsetY = picHeight / 2.0 + letterHeight / 2;

	QPixmap pic(picWidth, picHeight);
	pic.fill("black");
	QPainter pnt;
	pnt.begin(&pic);

	pnt.setFont(myFont);
	pnt.setPen("lightgray");

	pnt.drawText(offsetX, offsetY, inWord);
	pnt.end();
	return pic;
}




MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{    
	ui->setupUi(this);

	/// used for output file only
	dirPath = "/home/michael/Qt/Projects/TestAnagramms";

	/// read words
	answersArr = readWordsFile("/home/michael/Qt/Projects/TestAnagramms/answers.txt");

	/// shuffle words
	std::shuffle(std::begin(answersArr),
				 std::end(answersArr),
				 std::default_random_engine(
					 std::chrono::system_clock::now().time_since_epoch().count()));

	ui->nameLineEdit->setFocus();

	name.clear();
	counter = -1;

	QObject::connect(this->ui->startPushButton, SIGNAL(clicked()), this, SLOT(newTest()));
	QObject::connect(this->ui->answerLineEdit, SIGNAL(returnPressed()), this, SLOT(nextPic()));
	QObject::connect(this->ui->nameLineEdit, SIGNAL(returnPressed()), this, SLOT(setName()));
	QObject::connect(this->ui->stopPushButton, SIGNAL(clicked()), this, SLOT(stop()));
	QObject::connect(this->ui->skipPushButton, SIGNAL(clicked()), this, SLOT(skip()));
}

void MainWindow::setName()
{
	name = ui->nameLineEdit->text();
	ui->nameLineEdit->clear();
	ui->nameLineEdit->hide();
	ui->label->hide();
	ui->startPushButton->setFocus();
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
	switch(event->key())
	{
		case Qt::Key_Escape:
		{
			if(counter != -1) this->stop();
			this->close();
			break;
		}

		default:
		{
			break;
		}
	}
}

void MainWindow::newPic()
{
	mixedWord = mixWord(answersArr[counter]);

	this->ui->picLabel->setPixmap(drawWord(mixedWord).
			scaled(this->ui->picLabel->size()));

	time1.start();
	ui->answerLineEdit->clear();
	ui->answerLineEdit->setFocus();
}

void MainWindow::increment(const QString & message)
{
	++counter;
	if(counter == answersArr.size())
	{
		outStr.close();
		this->ui->picLabel->clear();
		QMessageBox::information((QWidget*)this,
								 tr("The End"),
								 message, QMessageBox::Ok);
		return;
	}
}

void MainWindow::newTest()
{
	if(name.isEmpty())
	{
		return;
	}

	QString helpString = dirPath + "/" + name + ".txt";

	outStr.open(helpString.toStdString());

	counter = 0;
	newPic();

}

void MainWindow::nextPic()
{
	if(ui->answerLineEdit->text() != answersArr[counter])
	{
		ui->answerLineEdit->setText("W R O N G");
		QTimer::singleShot(500, ui->answerLineEdit, SLOT(clear()));
		return;
	}

	outStr
			<< mixedWord << "\t"
			<< answersArr[counter] << "\t"
			<< time1.elapsed()/1000. << "\t"
			<< std::endl;

	increment("Thank you!");

	if(counter % 10 == 0)
	{
		QMessageBox::information(this, tr("Pause"), tr("You,ve solved 10 anagrams in a row\nIt's a pause time now\nPress OK when ready to continue."), QMessageBox::Ok );
	}

	newPic();
}

void MainWindow::stop()
{
	counter = -1;

	outStr.close();
	this->ui->picLabel->clear();

	QMessageBox::information((QWidget*)this, tr("The End"), tr("Stopped by user"), QMessageBox::Ok);

	ui->answerLineEdit->setFocus();
}

void MainWindow::mousePressEvent(QMouseEvent * ev)
{
	ui->answerLineEdit->setFocus();
}

void MainWindow::skip()
{
	outStr
			<< mixedWord << "\t"
			<< answersArr[counter] << "\t"
			<< time1.elapsed()/1000. << "\t"
			<< "SKIPPED"
			<< std::endl;

	increment("Congratulations!");
	newPic();
}


MainWindow::~MainWindow()
{
	delete ui;
}