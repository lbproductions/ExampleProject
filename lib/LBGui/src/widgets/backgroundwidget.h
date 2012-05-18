#ifndef LBGUI_BACKGROUNDWIDGET_H
#define LBGUI_BACKGROUNDWIDGET_H

#include <QFrame>

namespace LBGui {

class BackgroundWidget : public QFrame
{
    Q_OBJECT

public:
    enum Style {
        Gray
    };

    explicit BackgroundWidget(QWidget *parent = 0);
    
    Style style() const;

    void setStyle(Style style);

private:
    Style m_style;

};

} // namespace LBGui

#endif // LBGUI_BACKGROUNDWIDGET_H
