#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include "QStringListModel"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void OnLineEditEditingFinished();
    void OnComboBoxChanged(int index);
    void OnActionOpenTriggerd();
    void OnActionSaveTriggerd();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void Select_imgFile(QModelIndex index);
    void Get_Params(QString &path);
    void Sync_slideAndEdit(int min,int max);
    void initValues(int fileIndex=0);
    Ui::MainWindow *ui;
    bool m_isFileOpen;
    QStringListModel m_model;
    QStringList m_pathList;
};
#endif // MAINWINDOW_H
