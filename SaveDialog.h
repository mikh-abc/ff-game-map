#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>
#include "GameMapChanger.h"

namespace Ui {
class SaveDialog;
}

class SaveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveDialog(QWidget *parent = nullptr);
    ~SaveDialog();

    GameMapChanger::Options options();
    void addInfo(const QString& text);

private:
    Ui::SaveDialog *ui;
};

#endif // SAVEDIALOG_H
