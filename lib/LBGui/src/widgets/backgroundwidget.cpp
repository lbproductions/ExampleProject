#include "backgroundwidget.h"

#include <QFile>

#include <QDebug>

namespace LBGui {

BackgroundWidget::BackgroundWidget(QWidget *parent) :
    QFrame(parent)
{
}

BackgroundWidget::Style BackgroundWidget::style() const
{
    return m_style;
}

void BackgroundWidget::setStyle(Style style)
{
    QFile stylesheet;
    m_style = style;

    switch(m_style) {
    case Gray:
        stylesheet.setFileName(QLatin1String(":/backgroundwidget/gray/stylesheet"));
        break;
    }

    if(!stylesheet.exists()) {
        qWarning() << "BackgroundWidget::setStyle: The stylesheet file for the style does not exist.";
    }

    stylesheet.open(QFile::ReadOnly);
    setStyleSheet(stylesheet.readAll());
    stylesheet.close();
}

} // namespace LBGui
