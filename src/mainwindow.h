#include <QtGui>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
		
		void initWindow();
		QStringList files;
		QString folder;
		QFile file;
	
	public slots:
		void on_toolButtonInputFile_clicked();
		void on_pushButtonCancelModule_clicked();
		void on_toolButtonStartFolder_clicked();
		void on_toolButtonOutputFile_clicked();
		void on_pushButtonCreate_pressed();
		void on_pushButtonCreate_released();
		void on_actionExit_triggered();
		void on_actionAbout_triggered();

	private:
		QString version;
		QString fileSave;
                QSystemTrayIcon * tray;
	    	QPixmap* distributorImg;

	protected:
		QProcess *modulizing;
};
