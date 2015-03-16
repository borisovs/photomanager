#include "messagebox.h"
#include <QPixmap>
#include <QIcon>

MessageBox::MessageBox(QWidget *parent) :
    QDialog(parent){
    setupUi(this);
    tbYes->setIcon(QIcon(":/icons/dialog-ok.png"));
    tbNo->setIcon(QIcon(":/icons/list-remove.png"));
}

void MessageBox::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void MessageBox::setMsgPixmap(const QPixmap &pixmap)
{
    lbIcon->setPixmap(pixmap);
}


void MessageBox::setMsgText(const QString &m_caption, const QString &m_text)
{
    setWindowTitle(m_caption);
    lbText->setText(m_text);
}
