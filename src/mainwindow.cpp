#include "mainwindow.h"
#include <iostream>
#include <QComboBox>
#include <QThread>
using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setupUi(this);
	
	version = "qdir2mod 0.1 ";
	setWindowTitle(version);
	QPalette p = listWidget->palette ( );
	p.setBrush ( QPalette::Base, QBrush ( QImage ( ":/fluxflux.png" )) );
	listWidget->setPalette ( p );
	initWindow();
}

void MainWindow::initWindow()
{
	QDesktopWidget *widget = QApplication::desktop();
	QRect dim = widget->screenGeometry();
	setGeometry((dim.width()-width())/2, (dim.height()-height())/2, width(), height());
	setWindowOpacity(qreal(100)/100);
	QStringList args = QApplication::arguments();
	args.removeDuplicates();
	if (args.count() > 1) {
	for (int i = 1; i < args.count(); ++i)
		listWidget->addItem(args[i]);
	}
	QFile configFile("/etc/qdir2mod.cfg");

	if (configFile.exists())
	{
	    if (configFile.open(QFile::ReadOnly))
	    {
	        while (!configFile.atEnd())
	        {
	            QString line = configFile.readLine(500).trimmed();
	            if (line.startsWith("LOGO="))
	            {
	                logo += line.remove(0, 5);
	            }
	            if (line.startsWith("THREADS="))
	            {
	                threads += line.remove(0, 8);
			threadBox->setValue(threads.toInt());
	            }
	            if (line.startsWith("BLOCKS="))
	            {
	                blocks += line.remove(0, 7);
			QString bString(blocks);  
			QStringList bStringList = bString.split(",");
		        blockSizeBox->clear();
		        blockSizeBox->addItems(bStringList);
		    } else {
			QStringList bStringList;
			bStringList.append("512");
			bStringList.append("128");
			bStringList.append("256");
			bStringList.append("1024");
		        blockSizeBox->clear();
		        blockSizeBox->addItems(bStringList);
		    }
	            if (line.startsWith("APPEND="))
	            {
	                append += line.remove(0, 7);
			if ((append.toInt() == 1)) {
				appendBox->setChecked(1);
			}
	            }
	            if (line.startsWith("HINTS="))
	            {
	                hints += line.remove(0, 6);
			if ((hints.toInt() == 1)) {
				hintBox->setChecked(1);
			}
	            }
	        }
	         configFile.close();
	    }
	}
     	
	tray = new QSystemTrayIcon(this);
	QFile file(logo);
	if (file.exists()) {
		this->setWindowIcon(QIcon(logo));
		tray->setIcon(QIcon(logo));
		distributorLabel->setPixmap(logo);
	} else {
    		tray->setIcon(QIcon(":/icons/qdir2mod.png"));
    		distributorLabel->setPixmap(QPixmap(":/icons/qdir2mod.png"));
	}
    	tray->show();
	lineEditStartFolder->setText(QDir::homePath());
}

void MainWindow:: on_toolButtonInputFile_clicked()
{
	QString startDir = lineEditStartFolder->text();
	QFileDialog* folderDialog = new QFileDialog(this);
	folderDialog->setDirectory(lineEditStartFolder->text());
	folderDialog->setFileMode(QFileDialog::Directory);
	folderDialog->setOption(QFileDialog::DontUseNativeDialog, true);
	folderDialog->setOption(QFileDialog::ShowDirsOnly, true);
	folderDialog->setOption(QFileDialog::DontResolveSymlinks, true);
	QListView *folderList = folderDialog->findChild<QListView*>("listView");
	if (folderList) {
	folderList->setSelectionMode(QAbstractItemView::MultiSelection);
	}
	QTreeView *folderTree = folderDialog->findChild<QTreeView*>();
	if (folderTree) {
	folderTree->setSelectionMode(QAbstractItemView::MultiSelection);
	}

	folderDialog->exec();
	QStringList folders = folderDialog->selectedFiles();
	if (!folders.isEmpty())
	    listWidget->addItems(folders);
}

void MainWindow::on_pushButtonCancelModule_clicked()
{
	listWidget->takeItem(listWidget->currentRow());
}

void MainWindow::on_toolButtonStartFolder_clicked()
{
	QFileDialog* startDialog = new QFileDialog(this);
	startDialog->setFileMode(QFileDialog::Directory);
	startDialog->setOption(QFileDialog::DontUseNativeDialog, true);
	startDialog->setOption(QFileDialog::ShowDirsOnly, true);
	startDialog->setOption(QFileDialog::DontResolveSymlinks, true);
	startDialog->setOption(QFileDialog::ReadOnly, true);

    	QString startFolder = startDialog->getExistingDirectory(this,
                        tr("Choose a folder"),
                        QDir::homePath(),QFileDialog::DontResolveSymlinks);
	lineEditStartFolder->setText(startFolder);
}

void MainWindow::on_toolButtonOutputFile_clicked()
{
    	QFileDialog fileDialog(this);
	QString filters(tr("FluxFlux/Porteus modules (*.xzm);;Slax7 bundles (*.sb)"));
        QString defaultFilter(tr("FluxFlux/Porteus modules (*.xzm)"));
	fileSave = fileDialog.getSaveFileName(0, tr("Save in destination file"), QDir::homePath(),
        filters, &defaultFilter);

	QFileInfo f( fileSave );
        if (f.suffix().isEmpty()) {
		fileSave += ".xzm";
        }

        lineEditOutputFile->setText(fileSave);
}

void MainWindow::on_pushButtonCreate_pressed() {
	int t = (QThread::idealThreadCount ());
	QString coreThreads = QString::number(t);
	int mt = threadBox ->value();	
	QString maxThreads = QString::number(mt);

	if (listWidget->count() == 0) {
		QMessageBox::warning(this, tr("Warning"), tr("Select input folder(s) first!"));
		return;
	} else if (fileSave == "") {
		QMessageBox::critical(this, tr("Error"), tr("Select destination file to save to!"));
		return;
	} else if ( mt > t) {
		QMessageBox::critical(this, tr("Warning"), (tr("Thread count <font color=\"red\"><b>") + maxThreads + tr("</b></font> is too high!<br><br>It is highly recommended<br>to decrease this value!<br><br><b>Available cores: <font color=\"green\">") + coreThreads) + "</font></b>");
		return ;
	} else {
        tray->showMessage(tr("Modulizing process"), tr("Please be patient while modulizing..."), QSystemTrayIcon::Information, 5000);
	}
}

void MainWindow::on_pushButtonCreate_released()
{
	QFile file(fileSave);
	if (appendBox->isChecked() == false) {
		file.remove();
	}

	QString script = "sudo mksquashfs ";	
	for (int i = 0; i < listWidget->count(); i++)
	{
		QString module = listWidget->item(i)->text();
		QDir dir(module);
		if (!dir.isReadable()) {
			QMessageBox::critical(this, tr("Error"), (tr("Folder <b><font color=\"red\">") + module + tr("</font></b> not found or readable!")));
			return;
		}
		script += module;
		script += " ";
	}
	
	script += " " + fileSave;
	QString blocksize = blockSizeBox ->currentText();
	script += " -comp xz"; 
	script += " -b ";
	script += blocksize;
	script += "K -Xbcj x86";
	if (appendBox->isChecked() == false) {
	script += " -noappend";
	}
	if (structureBox->isChecked() == true) {
	script += " -keep-as-directory";
	}
	script += " -processors ";
	int mt = threadBox ->value();	
	QString maxThreads = QString::number(mt);
	script += maxThreads;
	script += " -no-progress";

	modulizing = new QProcess(this);
	modulizing->execute(script);
	if (file.exists()) {
        	tray->showMessage(tr("Modulizing succeded"), (tr("Creating module ") + fileSave + tr(" succeeded!")), QSystemTrayIcon::Information, 3000);
	} else {
        	tray->showMessage(tr("Modulizing failed"), (tr("Creating module ") + fileSave + tr(" failed!")), QSystemTrayIcon::Critical, 5000);
	}
}

void MainWindow::on_actionExit_triggered()
{
	exit(1);
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::information(this, tr("About qdir2mod"), tr("<center><h1>Qdir2mod 0.1</h1>Copyright &copy;2014, Manfred Mueller - http://fluxflux.net.<br><br>Qdir2mod converts one or more folders to a Live module<br>and supports to be called with command-line parameters.<br>Thus you can integrate it into your file manager or script.<br><br>Qdir2mod is partly based on QPDFMerge by<br>Betti Sorbelli Francesco, montoya83@gmail.com</center>"));
}
