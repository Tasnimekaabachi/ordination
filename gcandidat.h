#ifndef GCANDIDAT_H
#define GCANDIDAT_H

#include <QMainWindow>
#include <QObject>
#include <QKeyEvent>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class GCANDIDAT; }
QT_END_NAMESPACE

class GCANDIDAT : public QMainWindow
{
    Q_OBJECT

public:
    explicit GCANDIDAT(QWidget *parent = nullptr);
    ~GCANDIDAT();

    bool ajouter();
    bool supprimer(QString id);
    bool modifier();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_pushButton_ajouter_clicked();
    void on_pushButton_supprimer_clicked();
    void on_tableWidget_itemClicked(QTableWidgetItem *item);

    void on_tableWidget_cellActivated(int row, int column); tt

private:
    Ui::GCANDIDAT *ui;
    void setupTable();
    void refreshTable();
    bool validateInputs();
    void clearInputs();
};

#endif // GCANDIDAT_H
