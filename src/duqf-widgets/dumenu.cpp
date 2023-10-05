#include "dumenu.h"
#include "duqf-app/dusettingsmanager.h"
#include "duqf-widgets/dushadow.h"

DuMenu::DuMenu(const QString &title, QWidget *parent):
    QMenu(title, parent)
{
    setupUi();
}

DuMenu::DuMenu(QWidget *parent):
    QMenu(parent)
{
    setupUi();
}

void DuMenu::setShadowWidth(int w) {
    m_shadowWidth = w;
    //setContentsMargins(w, w, w, w);
}

int DuMenu::shadowWidth() const {
    return m_shadowWidth;
}

void DuMenu::ensureLoaded() {
    if (m_cache.isNull()) {
        m_cache = QPixmap::fromImage(DuShadow::paint(this, this->rect(), m_shadowWidth, m_shadowWidth, QColor(1,1,1), DuShadow::Around));
    }
}

void DuMenu::paintEvent(QPaintEvent *e) {

    QPainter painter(this);
    if (!m_cache.isNull()) painter.drawPixmap(QPoint(), m_cache);

    painter.setBrush(palette().window());
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    auto m = QMargins(m_shadowWidth, m_shadowWidth, m_shadowWidth, m_shadowWidth);
    painter.drawRoundedRect(rect().marginsRemoved(m), m_shadowWidth, m_shadowWidth);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QMenu::paintEvent(e);
}

void DuMenu::resizeEvent(QResizeEvent *e) {
    ensureLoaded();
    m_cache = m_cache.scaled(e->size());
    QWidget::resizeEvent(e);
}

void DuMenu::setupUi()
{
    setWindowFlag(Qt::FramelessWindowHint, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);
    setStyleSheet("QMenu::item { border-radius: 5px; padding-left: 5px; padding-right: 5px; padding-top: 3px; padding-bottom: 3px; }"
                  "QMenu::item:selected {"
                  "background-color: " + DuSettingsManager::instance()->uiFocusColor(DuSettingsManager::DarkerColor).name() + "; }"
                                                                                                         "QMenu::icon { padding-left: 5px; }" );
}
