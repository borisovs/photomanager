#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "ui_messagebox.h"
class QPixmap;
class QString;

class MessageBox : public QDialog, private Ui::MessageBox {
    Q_OBJECT
public:
    MessageBox(QWidget *parent = 0);
    void setMsgPixmap(const QPixmap &);
    void setMsgText(const QString &, const QString &);
protected:
    void changeEvent(QEvent *e);


};

#endif // MESSAGEBOX_H
