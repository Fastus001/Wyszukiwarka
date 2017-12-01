#include "wyszukiwanie.h"
#include "ui_wyszukiwanie.h"
#include <QtWidgets>

enum { absoluteFileNameRole = Qt::UserRole + 1 };

static inline QString fileNameOfItem(const QTableWidgetItem *item)
{
    return item->data(absoluteFileNameRole).toString();
}

wyszukiwanie::wyszukiwanie(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::wyszukiwanie)
{
    ui->setupUi(this);

    connect(ui->browse,SIGNAL(clicked(bool)),this,SLOT(browse())); //połaczenie przycisku brows z funkcją przegladania plików
    connect(ui->findFiles,SIGNAL(clicked(bool)),this,SLOT(find())); // połączenie przycisku wyszukaj z funkcją przeszukującą foldery
    connect(ui->saveFilesPathButton, SIGNAL(clicked(bool)),this,SLOT(saveFiles())); //polaczenie przycisku wyszukania lokalizacji do zapisanie wyszukanych zdjec

    ui->browseComboBox->addItem("F:/");
    ui->saveFilesPath->addItem("F:/Opisywanie zdjęć");
}

void wyszukiwanie::showFiles(const QStringList &list)
{


    for (int i = 0; i<list.size();++i)
    {
        const QString &fileName = list.at(i);
/*        const QString &toolTip = QDir::toNativeSeparators(fileName);
        const QString relativePath = QDir::toNativeSeparators(currentDir.relativeFilePath(fileName));

        //coś dopisane

        QTableWidgetItem *nazwaPliku = new QTableWidgetItem(relativePath);
        nazwaPliku->setData(absoluteFileNameRole, QVariant(fileName));
        nazwaPliku->setToolTip(toolTip);
        nazwaPliku->setFlags(nazwaPliku->flags()^ Qt::ItemIsEditable);
*/
        ui->listWidget->addItem(fileName);

    }

}

wyszukiwanie::~wyszukiwanie()
{
    delete ui;
}

void wyszukiwanie::browse()
{
    QString directory =
        QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,
                      tr("Wybierz lokalizację z którego mają być wyszukane pliki")));
    if (!directory.isEmpty())
    {
        if(ui->browseComboBox->findText(directory)==-1)
            ui->browseComboBox->addItem(directory);
        ui->browseComboBox->setCurrentIndex(ui->browseComboBox->findText(directory));

    }
}
//funkcja przeszukująca wszystkie podkatalogi w poszukiwaniu szukanej frazy
static void findRecursion(const QString &path, const QString &pattern, QStringList *result)
{
    QDir currentDir(path);
    QString prefix = path+QLatin1Char('/');
    foreach(const QString &match, currentDir.entryList(QStringList(pattern),QDir::Files | QDir::NoSymLinks))
       result->append(prefix+match);
    foreach(const QString &dir, currentDir.entryList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot))
        findRecursion(prefix+dir, pattern,result);

}


void wyszukiwanie::find()
{
    //otwarcie pliku z numerami do wyszukania
    QFile file("zdjecia.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
       return;

    QTextStream in(&file);
    QStringList lista;
    //zgranie z pliku txt symboli które mają być wyszukane/przekopiowane jak zostaną znalezione
    while(!in.atEnd())
    {
        lista << in.readLine();
    }
    file.close();

    //wypisanie szukanych numerów w tabeli
    /*for (int i = 0; i<lista.size();++i)
        ui->listWidget->addItem(lista.at(i));*/

    //przekazanie ścieżki katalogu w którym będziemy szukać
    QString path = QDir::cleanPath(ui->browseComboBox->currentText());

    //lista do przechowywania ścieżek do wyszukanych plików
    QStringList files, finalnaLista;


    //dialog box, który się pojawia jak wyszukuje wiecej pozycji
    QProgressDialog progresDialog(this);
    progresDialog.setCancelButtonText(tr("&Przerwij"));
    progresDialog.setRange(0,lista.size());
    progresDialog.setWindowTitle(tr("Szukanie plików"));


    //zaczytanie do listy stringów danych na temat wszystkich plików z *jpg z wybranego zakresu
    QString fileName = "*.jpg";
    //kursor zajęty!

    QApplication::setOverrideCursor(Qt::WaitCursor);
    //funkcja rekurencyjna robiąca pełną listę plików i folderów, zapisuje do listy stringów files.
    findRecursion(path,fileName, &files);

    QApplication::restoreOverrideCursor();

    for(int i = 0; i < lista.size();++i)
    {
        progresDialog.setValue(i);
        progresDialog.setLabelText(tr("Wyszukiwanie i kopiowanie pliku nr %1 z %n...",0,lista.size()).arg(i));
        QApplication::processEvents();

        if(progresDialog.wasCanceled())
            break;
        for (int x =0; x<files.size();++x)
        {
            QString test = files.at(x);
            if(test.contains(lista.at(i)))
            {
                ui->listWidget->addItem(lista.at(i));


                QString destPath = QDir::cleanPath(ui->saveFilesPath->currentText());
                destPath +="/";

                QFile plik(test);
                if(!plik.open(QIODevice::ReadWrite))
                   return;
                //do poprawienia że nie pokazuje skopiowanego pliku!!!!!!!!!!!!!
                if(plik.copy(test,destPath + "/" + lista.at(i) + ".jpg"))
                {
                    ui->listWidget->addItem(tr("skopiowano plik nr %1 z %n",0,lista.size()).arg(i+1));
                    plik.close();
                    break;
                }
            }
            if(x==files.size()-1)
            {
                ui->listWidget->addItem("nie znaleziono pliku symbol " + lista.at(i));
                QFile file("brak.txt");
                if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
                   return;
                QTextStream out(&file);
                out << lista.at(i) << "\n";
                file.close();
            }

        }

    }


    QString rozmiar;
    rozmiar.setNum(files.size());
    ui->listWidget->addItem(rozmiar + "plików");

    //showFiles(files);
}

void wyszukiwanie::saveFiles()
{
    QString directory =
        QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Wybierz lokalizację gdzie mają być zapisane pliki")));

    if (!directory.isEmpty())
    {
        if(ui->saveFilesPath->findText(directory)==-1)
            ui->saveFilesPath->addItem(directory);
        ui->saveFilesPath->setCurrentIndex(ui->saveFilesPath->findText(directory));

    }
}

