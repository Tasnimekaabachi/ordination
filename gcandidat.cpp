#include "gcandidat.h"
#include "ui_gcandidat.h"
#include "connection.h"
#include <QTableWidgetItem>
#include <QDebug>
#include <QDate>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QRegExp>

GCANDIDAT::GCANDIDAT(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GCANDIDAT)
{
    ui->setupUi(this);
    
    connection c;
    bool test=c.createconnection();
    if(test)
    {
        setupTable();
        refreshTable();
        ui->dateEdit_naissance->setCalendarPopup(true);
        ui->dateEdit_naissance->setDate(QDate::currentDate());
    }
    else
        QMessageBox::critical(nullptr, QObject::tr("Database is not open"),
                    QObject::tr("Connection failed.\n"
                              "Click Cancel to exit."), QMessageBox::Cancel);
}

GCANDIDAT::~GCANDIDAT()
{
    delete ui;
}

void GCANDIDAT::setupTable()
{
    ui->tableWidget->setColumnCount(7);
    QStringList headers;
    headers << "Identifiant" << "Date Naissance" << "Prénom" << "Nom" << "Email" << "Sexe" << "Niveau";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void GCANDIDAT::refreshTable()
{
    QSqlQuery query;
    query.prepare("SELECT IDENTIFIANT, TO_CHAR(DATE_NAISSANCE, 'YYYY-MM-DD'), PRENOM, NOM, EMAIL, SEXE, NIVEAU_SCOLAIRE FROM CANDIDAT");
    
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la récupération des données : " + query.lastError().text());
        return;
    }

    ui->tableWidget->setRowCount(0);
    while (query.next()) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        
        for(int i = 0; i < 7; i++) {
            QTableWidgetItem *item = new QTableWidgetItem(query.value(i).toString());
            ui->tableWidget->setItem(row, i, item);
        }
    }
}

bool GCANDIDAT::validateInputs()
{
    // Vérification des champs vides
    if (ui->lineEdit_id->text().isEmpty() ||
        ui->lineEdit_nom->text().isEmpty() ||
        ui->lineEdit_prenom->text().isEmpty() ||
        ui->lineEdit_email->text().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Veuillez remplir tous les champs obligatoires");
        return false;
    }

    // Validation de l'identifiant selon le sexe
    QString id = ui->lineEdit_id->text();
    QString prefixeAttendu = ui->radioButton_homme->isChecked() ? "JMT" : "JFT";
    if (!id.startsWith(prefixeAttendu)) {
        QMessageBox::warning(this, "Validation", 
            "L'identifiant doit commencer par " + prefixeAttendu + " pour le sexe sélectionné");
        return false;
    }

    // Validation du nom et prénom (lettres et espaces uniquement)
    QRegExp nomPrenomRegex("^[A-Za-z ]+$");
    if (!nomPrenomRegex.exactMatch(ui->lineEdit_nom->text())) {
        QMessageBox::warning(this, "Validation", 
            "Le nom doit contenir uniquement des lettres et des espaces");
        return false;
    }
    if (!nomPrenomRegex.exactMatch(ui->lineEdit_prenom->text())) {
        QMessageBox::warning(this, "Validation", 
            "Le prénom doit contenir uniquement des lettres et des espaces");
        return false;
    }

    // Validation de l'email
    QRegExp emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!emailRegex.exactMatch(ui->lineEdit_email->text())) {
        QMessageBox::warning(this, "Validation", 
            "Veuillez entrer une adresse email valide");
        return false;
    }

    return true;
}

void GCANDIDAT::clearInputs()
{
    ui->lineEdit_id->clear();
    ui->lineEdit_nom->clear();
    ui->lineEdit_prenom->clear();
    ui->lineEdit_email->clear();
    ui->dateEdit_naissance->setDate(QDate::currentDate());
    ui->comboBox_niveau->setCurrentIndex(0);
    ui->radioButton_homme->setChecked(true);
}

bool GCANDIDAT::ajouter()
{
    QSqlQuery query;
    QString id = ui->lineEdit_id->text();
    
    query.prepare("INSERT INTO CANDIDAT (IDENTIFIANT, DATE_NAISSANCE, PRENOM, NOM, EMAIL, SEXE, NIVEAU_SCOLAIRE) "
                 "VALUES (:id, TO_DATE(:date, 'YYYY-MM-DD'), :prenom, :nom, :email, :sexe, :niveau)");

    query.bindValue(":id", id);
    query.bindValue(":date", ui->dateEdit_naissance->date().toString("yyyy-MM-dd"));
    query.bindValue(":prenom", ui->lineEdit_prenom->text());
    query.bindValue(":nom", ui->lineEdit_nom->text());
    query.bindValue(":email", ui->lineEdit_email->text());
    query.bindValue(":sexe", ui->radioButton_homme->isChecked() ? "Homme" : "Femme");
    query.bindValue(":niveau", ui->comboBox_niveau->currentText());

    return query.exec();
}

bool GCANDIDAT::supprimer(QString id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM CANDIDAT WHERE IDENTIFIANT = :id");
    query.bindValue(":id", id);
    return query.exec();
}

bool GCANDIDAT::modifier()
{
    if (!validateInputs()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs obligatoires");
        return false;
    }

    QSqlQuery query;
    QString id = ui->lineEdit_id->text();
    
    query.prepare("UPDATE CANDIDAT SET "
                 "DATE_NAISSANCE = TO_DATE(:date, 'YYYY-MM-DD'), "
                 "PRENOM = :prenom, "
                 "NOM = :nom, "
                 "EMAIL = :email, "
                 "SEXE = :sexe, "
                 "NIVEAU_SCOLAIRE = :niveau "
                 "WHERE IDENTIFIANT = :id");

    query.bindValue(":id", id);
    query.bindValue(":date", ui->dateEdit_naissance->date().toString("yyyy-MM-dd"));
    query.bindValue(":prenom", ui->lineEdit_prenom->text());
    query.bindValue(":nom", ui->lineEdit_nom->text());
    query.bindValue(":email", ui->lineEdit_email->text());
    query.bindValue(":sexe", ui->radioButton_homme->isChecked() ? "Homme" : "Femme");
    query.bindValue(":niveau", ui->comboBox_niveau->currentText());

    bool success = query.exec();

    if (success) {
        QMessageBox::information(this, "Succès", "Candidat modifié avec succès");
        refreshTable();
    } else {
        QMessageBox::critical(this, "Erreur", 
            "Erreur lors de la modification : " + query.lastError().text());
        qDebug() << "Erreur SQL : " << query.lastError().text();
    }

    return success;
}

void GCANDIDAT::on_pushButton_ajouter_clicked()
{
    if (!validateInputs()) return;

    if (ajouter()) {
        QMessageBox::information(this, "Succès", "Candidat ajouté avec succès");
        clearInputs();
        refreshTable();
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout du candidat");
    }
}

void GCANDIDAT::on_pushButton_supprimer_clicked()
{
    QString id = ui->lineEdit_id->text();
    if (id.isEmpty()) {
        QMessageBox::warning(this, "Validation", "Veuillez sélectionner un candidat à supprimer");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmation",
        "Voulez-vous vraiment supprimer ce candidat ?",
        QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (supprimer(id)) {
            QMessageBox::information(this, "Succès", "Candidat supprimé avec succès");
            clearInputs();
            refreshTable();
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression du candidat");
        }
    }
}

void GCANDIDAT::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    int row = item->row();
    
    ui->lineEdit_id->setText(ui->tableWidget->item(row, 0)->text());
    ui->dateEdit_naissance->setDate(QDate::fromString(ui->tableWidget->item(row, 1)->text(), "yyyy-MM-dd"));
    ui->lineEdit_prenom->setText(ui->tableWidget->item(row, 2)->text());
    ui->lineEdit_nom->setText(ui->tableWidget->item(row, 3)->text());
    ui->lineEdit_email->setText(ui->tableWidget->item(row, 4)->text());
    
    QString sexe = ui->tableWidget->item(row, 5)->text();
    ui->radioButton_homme->setChecked(sexe == "Homme");
    ui->radioButton_femme->setChecked(sexe == "Femme");
    
    int index = ui->comboBox_niveau->findText(ui->tableWidget->item(row, 6)->text());
    ui->comboBox_niveau->setCurrentIndex(index != -1 ? index : 0);
}

void GCANDIDAT::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
        if(!ui->lineEdit_id->text().isEmpty())
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this, 
                "Confirmation de modification",
                "Voulez-vous modifier ce candidat ?",
                QMessageBox::Yes|QMessageBox::No);

            if(reply == QMessageBox::Yes)
            {
                if(modifier()) {
                    clearInputs();
                }
            }
        }
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}


