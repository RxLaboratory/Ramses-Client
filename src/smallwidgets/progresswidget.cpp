#include "progresswidget.h"

ProgressWidget::ProgressWidget(QWidget *parent) : QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum );

    m_abyss = QColor(28,28,28);
    m_dark = QColor(51,51,51);
    m_medium = QColor(109,109,109);
    m_lessLight = QColor(157,157,157);
    m_light = QColor(227,227,227);

    m_padding = 10;

    this->setMinimumHeight( 8 );
}

void ProgressWidget::setCompletionRatio(int completionRatio)
{
    m_completionRatio = completionRatio;
    update();
}

void ProgressWidget::setLatenessRatio(float latenessRatio)
{
    m_latenessRatio = latenessRatio;
    update();
}

void ProgressWidget::setTimeSpent(qint64 timeSpent)
{
    m_timeSpent = timeSpent;
    update();
}

void ProgressWidget::setEstimation(float newEstimation)
{
    m_estimation = newEstimation;
    update();
}

void ProgressWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // BG Bar
    QBrush statusBrush( m_abyss );
    int statusWidth = this->width() - 20 ;
    QRect statusRect( m_padding, 0, statusWidth, this->height() );
    QPainterPath path;
    path.addRoundedRect(statusRect, 5, 5);
    painter.fillPath(path, statusBrush);

    if (!m_timeTracking && !m_showCompletionRatio) return;

    if (m_showCompletionRatio)
    {
        // Set a color according to the completion
        float completionRatio = m_completionRatio / 100.0;
        QColor completionColor;
        if (completionRatio < 0.12) completionColor = QColor( 197, 0, 0);
        else if (completionRatio < 0.25) completionColor = QColor( 197, 98, 17);
        else if (completionRatio < 0.5) completionColor = QColor( 197, 179, 40);
        else if (completionRatio < 0.75) completionColor = QColor( 128, 197, 37);
        else if (completionRatio < 0.88) completionColor = QColor( 100, 172, 69);
        else if (completionRatio < 0.98) completionColor = QColor( 55, 172, 23);
        else completionColor = QColor( 6, 116, 24);

        // Draw completion ratio
        statusBrush.setColor( completionColor );
        statusRect.setWidth(statusWidth * completionRatio);
        QPainterPath completionPath;
        completionPath.addRoundedRect(statusRect, 5, 5);
        painter.fillPath(completionPath, statusBrush);
    }//*/
}


