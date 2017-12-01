#ifndef WYSZUKIWANIE_H
#define WYSZUKIWANIE_H

#include <QMainWindow>
#include <QWidget>
#include <QDir>
#include <QFile>
#include <QTextStream>



namespace Ui {
class wyszukiwanie;
}

class wyszukiwanie : public QMainWindow
{
    Q_OBJECT

public:
    explicit wyszukiwanie(QWidget *parent = 0);
    void showFiles(const QStringList &list);
    ~wyszukiwanie();

private slots:
    void browse();
    void find();
    void saveFiles();

private:
    Ui::wyszukiwanie *ui;

    QDir currentDir;




};

#endif // WYSZUKIWANIE_H
