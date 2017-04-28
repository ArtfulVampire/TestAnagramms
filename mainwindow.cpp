#include "mainwindow.h"
#include "ui_mainwindow.h"

std::ostream & operator << (std::ostream & os, const QString & str)
{
	os << str.toStdString();
	return os;
}



std::set<int> readAnsweredFile(const QString & filePath)
{
	if(!QFile::exists(filePath)) return {};

	std::set<int> res;
	std::ifstream fil;
	fil.open(filePath.toStdString());
	int tmp;
	while(fil >> tmp)
	{
		res.emplace(tmp);
	}
	fil.close();
	return res;
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

		/// 1 if next letter is from right order
		/// -1 if prev
		decltype(mixNum) nexts = mixNum.cshift(1) - mixNum;

		num = 0;
		for(int i = 0; i < mixNum.size() - 1; ++i)
		{
			if(std::abs(nexts[i]) == 1) { num += 1; }
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
//	dirPath = "/home/michael/Qt/Projects/TestAnagramms";
	dirPath = ".";

	/// read words
//	answersArr = readWordsFile("/home/michael/Qt/Projects/TestAnagramms/answers.txt");
	answersArr = answers;

	/// make indices array - allow for already answered
	alreadyAnswered = readAnsweredFile(dirPath + "/" + answeredFileName);

	mixNum.clear();
	for(int i = 0; i < answersArr.size(); ++i)
	{
		if(alreadyAnswered.count(i) == 0)
		{
			mixNum.push_back(i);
		}
	}


	/// shuffle words
	std::shuffle(std::begin(mixNum),
				 std::end(mixNum),
				 std::default_random_engine(
					 std::chrono::system_clock::now().time_since_epoch().count()));

//	ui->nameLineEdit->setFocus();
	ui->progressBar->setValue(0);

	ui->currentLabel->setFont(QFont("Helvetica", 24));
	ui->currentLabel->setText("0/" + QString::number(mixNum.size()));

    time1.start();
	counter = -1;

	QObject::connect(this->ui->startPushButton, SIGNAL(clicked()), this, SLOT(startTest()));
	QObject::connect(this->ui->answerLineEdit, SIGNAL(returnPressed()), this, SLOT(nextPic()));
//	QObject::connect(this->ui->nameLineEdit, SIGNAL(returnPressed()), this, SLOT(startTest()));
	QObject::connect(this->ui->stopPushButton, SIGNAL(clicked()), this, SLOT(stop()));
	QObject::connect(this->ui->skipPushButton, SIGNAL(clicked()), this, SLOT(skip()));
	QObject::connect(this->ui->timerCheckBox, &QCheckBox::clicked,
					 [this](bool in)
	{
		ui->progressBar->setVisible(in);
	});
	ui->timerCheckBox->setChecked(true);
}

void MainWindow::testFileHash(const QString & filePath)
{
	if(!QFile::exists(filePath))
	{
		std::cout << "File doesn't exist" << std::endl;
		return;
	}

	QFile fil(filePath);
	fil.open(QIODevice::ReadOnly);
	QTextStream str(&fil);

	QString answer;
	QString word;
	double time;
	int num;
	double hash;
	int lineNum = 0;

	while(1)
	{
		str >> answer >> word >> time >> num >> hash;


		if(!str.atEnd())
		{
			double shouldHash = hashFunc(answer, word, time, num, 0);
			std::cout << answer << "\t"
					  << word << "\t"
					  << time << "\t"
					  << num << "\t"
					  << hash << "\t"
					  << shouldHash
					  << std::endl;
			if(std::abs(hash - shouldHash) > 0.001)
			{
				std::cout << "hash error in line " << lineNum << std::endl;
			}
		}
		else { break; }

		if(answer != "WRONG") { ++lineNum; }
	}
	fil.close();
}

double MainWindow::hashFunc(const QString & answer,
							const QString & word,
							double time,
							int wordNum,
							int prevSiz)
{
	static int num = 0;
	double res = int(answer[0].toLower().toLatin1())
			+ int(word.toStdString()[1])
			+ std::pow(time, 2.5) / 3.5
			+ (wordNum % 7) * (wordNum % 5)
			+ num + prevSiz;
	if(answer != "WRONG") { ++num; }
	return res;
}

void MainWindow::setName()
{
//    name = ui->nameLineEdit->text();
//	ui->nameLineEdit->clear();
//	ui->nameLineEdit->hide();
//	ui->label->hide();
//	ui->startPushButton->setFocus();
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
	mixedWord = mixWord(answersArr[mixNum[counter]]);
	ui->currentLabel->setText(QString::number(counter) + "/" + QString::number(mixNum.size()));

	this->ui->picLabel->setPixmap(drawWord(mixedWord).
			scaled(this->ui->picLabel->size()));

    time1.restart();
	ui->progressBar->setValue(0);
	ui->answerLineEdit->clear();
	ui->answerLineEdit->setFocus();


	timerThread = std::thread([this]()
	{
		int counterBC = counter;
		double val = 0;
		double solveTime = 50.;
		while(counter == counterBC)
		{
			std::this_thread::sleep_for(std::chrono::seconds{1});
			val += 100. / solveTime;
			if(counter == counterBC)
			{
				this->ui->progressBar->setValue(val);
			}
		}
	}
	);
	timerThread.detach();
}

void MainWindow::increment(const QString & message)
{
	/// write number into alreadyAnsweredFile
	std::ofstream fil;
	fil.open((dirPath + "/" + answeredFileName).toStdString(), std::ios_base::app);
	fil << mixNum[counter] << "\r\n"; fil.flush();
	fil.close();


	++counter;
	if(counter == mixNum.size())
	{
        outFile.close();
		this->ui->picLabel->clear();
		QMessageBox::information((QWidget*)this,
								 tr("The End"),
                                 message, QMessageBox::Ok);
        this->close();
        return;
	}
    else if(counter % 5 == 0)
	{
		ui->progressBar->setValue(0);
		QMessageBox::information(this,
								 tr("Pause"),
								 tr("You,ve passed 5 anagrams in a row\n"
									"It's a pause time now\n"
									"Press OK when ready to continue."),
								 QMessageBox::Ok);
    }
    newPic();
}

void MainWindow::startTest()
{
//    QString guyName = ui->nameLineEdit->text();
//    if(guyName.isEmpty())
//	{
//        ui->nameLineEdit->setText("Enter your name!");
//        QTimer::singleShot(1000, ui->nameLineEdit, SLOT(clear()));
//		return;
//	}

//    ui->nameLineEdit->hide();
//    ui->label->hide();
//    ui->startPushButton->setFocus();

	QString helpString = dirPath + "/" + outFileName;
    outFile.setFileName(helpString);
	outFile.open(QIODevice::WriteOnly|QIODevice::Append);
	outStream.setDevice(&outFile);

	counter = 0;
    newPic();
}

void MainWindow::nextPic()
{
	QString toProcess = ui->answerLineEdit->text();
	toProcess.remove(QRegExp("\\s"));
	if(toProcess != answersArr[mixNum[counter]])
	{
		if(toProcess.isEmpty() || toProcess == "WRONG")
		{
			toProcess = "нетотв";
		}
		double tim = time1.elapsed() / 1000.;
		outStream
				<< "WRONG" << "\t"
				<< toProcess << "\t"
				<< tim << "\t"
				<< mixNum[counter] << "\t"
				<< hashFunc("WRONG", toProcess, tim,
							mixNum[counter], alreadyAnswered.size()) << "\t"
                << "\r\n";
		outStream.flush();

		ui->answerLineEdit->setText("W R O N G");
		QTimer::singleShot(300, ui->answerLineEdit, SLOT(clear()));
	}
	else
	{

		double tim = time1.elapsed() / 1000.;
		outStream
				<< "RIGHT" << "\t"
				<< toProcess << "\t"
				<< tim << "\t"
				<< mixNum[counter] << "\t"
				<< hashFunc("RIGHT", toProcess, tim,
							mixNum[counter], alreadyAnswered.size()) << "\t"
				<< "\r\n";
		outStream.flush();

		increment("Thank you!");
	}
}

void MainWindow::skip()
{
	double tim = time1.elapsed()/1000.;
	outStream
			<< "SKIPD" << "\t"
			<< answersArr[mixNum[counter]] << "\t"
			<< tim << "\t"
			<< mixNum[counter] << "\t"
			<< hashFunc("SKIPD", answersArr[mixNum[counter]], tim,
						mixNum[counter], alreadyAnswered.size()) << "\t"
			<< "\r\n";
	outStream.flush();

	increment("Congratulations!");
}

void MainWindow::stop()
{
	counter = -1;

//	outStr.close();
    outFile.close();
	this->ui->picLabel->clear();

	QMessageBox::information((QWidget*)this, tr("The End"), tr("Stopped by user"), QMessageBox::Ok);

	ui->answerLineEdit->setFocus();
	this->close();
}

void MainWindow::mousePressEvent(QMouseEvent * ev)
{
	ui->answerLineEdit->setFocus();
}

MainWindow::~MainWindow()
{
	delete ui;
}
