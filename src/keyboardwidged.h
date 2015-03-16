#ifndef KEYBOARDWIDGED_H
#define KEYBOARDWIDGED_H

#include "ui_keyboardwidged.h"
#include <QDialog>


class KeyboardWidged : public QDialog, private Ui::KeyboardWidged{
    Q_OBJECT
public:
    KeyboardWidged(QDialog *parent = 0);

};

#endif // KEYBOARDWIDGED_H
