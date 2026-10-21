#pragma once
#include <QApplication>
#include <QElapsedTimer>
#include <QEvent>
#include <QFrame>
#include <QFutureWatcher>
#include <QLabel>
#include <QPainter>
#include <QPointer>
#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QtConcurrent/QtConcurrentRun>
#include <functional>
#include <exception>

// Worker functions capture only value snapshots, never widgets or mutable managers.
// One file transaction per window, with a small floating busy indicator.
namespace FileOperation {
inline bool busy(QWidget* owner)
{
    return owner->window()->property("fileOperationBusy").toBool();
}
class Spinner final : public QWidget {
public:
    explicit Spinner(QWidget* parent)
        : QWidget(parent)
    {
        setFixedSize(28, 28);
        auto* timer = new QTimer(this);
        QObject::connect(timer, &QTimer::timeout, this, [this] {
            _angle = (_angle + 24) % 360;
            update();
        });
        timer->start(70);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(QColor("#e5d5d8"), 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawEllipse(QRectF(4, 4, 20, 20));
        painter.setPen(QPen(QColor("#a91427"), 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(QRectF(4, 4, 20, 20), _angle * 16, 110 * 16);
    }

private:
    int _angle { 0 };
};
class Progress final : public QFrame {
public:
    Progress(QWidget* host, const QString& title)
        : QFrame(host)
    {
        setObjectName("fileOperationOverlay");
        setAttribute(Qt::WA_TranslucentBackground);
        setStyleSheet(
            "QFrame#fileOperationOverlay {background:transparent;border:0;}"
            "QFrame#fileProgressCard {background:#ffffff;border:1px solid #d8dcdf;border-radius:8px;}"
            "QFrame#fileProgressCard QLabel {background:transparent;border:0;color:#263445;}");
        _card = new QFrame(this);
        _card->setObjectName("fileProgressCard");
        auto* row = new QHBoxLayout(_card);
        row->setContentsMargins(14, 10, 14, 10);
        row->setSpacing(11);
        row->addWidget(new Spinner(_card));
        auto* labels = new QVBoxLayout();
        labels->setSpacing(1);
        auto* heading = new QLabel(title, _card);
        heading->setStyleSheet("font-size:13px;font-weight:600;");
        heading->setWordWrap(false);
        labels->addWidget(heading);
        auto* label = new QLabel(QStringLiteral("处理中 · 已用 0 秒"), _card);
        label->setStyleSheet("font-size:11px;color:#71808c;");
        labels->addWidget(label);
        row->addLayout(labels, 1);
        auto* timer = new QTimer(this);
        _elapsed.start();
        QObject::connect(timer, &QTimer::timeout, this, [this, label] {
            label->setText(QStringLiteral("处理中 · 已用 %1 秒").arg(_elapsed.elapsed() / 1000));
        });
        timer->start(250);
        host->installEventFilter(this);
        setGeometry(host->rect());
        placeCard();
        show();
        raise();
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (watched == parentWidget() && event->type() == QEvent::Resize) {
            setGeometry(parentWidget()->rect());
            placeCard();
        }
        return QFrame::eventFilter(watched, event);
    }

private:
    void placeCard()
    {
        const int width = qMin(270, qMax(190, this->width() - 24));
        _card->setGeometry(
            qMax(12, this->width() - width - 18), qMin(142, qMax(16, this->height() - 76)), width, 66);
    }
    QFrame* _card { nullptr };
    QElapsedTimer _elapsed;
};
inline bool run(QWidget* owner, const QString& title, std::function<QString()> work,
    std::function<void(const QString&)> finished)
{
    if (busy(owner))
        return false;
    QPointer<QWidget> host = owner->window(), target = owner;
    host->setProperty("fileOperationBusy", true);
    auto* progress = new Progress(host, title);
    auto* watcher = new QFutureWatcher<QString>(host);
    // Closing the application must not truncate a running write transaction.
    QObject::connect(
        qApp, &QCoreApplication::aboutToQuit, watcher, [watcher] { watcher->waitForFinished(); });
    QObject::connect(
        watcher, &QFutureWatcher<QString>::finished, watcher, [host, target, progress, watcher, finished] {
            const QString error = watcher->result();
            progress->hide();
            progress->deleteLater();
            if (host)
                host->setProperty("fileOperationBusy", false);
            if (target)
                finished(error);
            watcher->deleteLater();
        });
    watcher->setFuture(QtConcurrent::run([work, title] {
        QElapsedTimer timer;
        timer.start();
        QString error;
        try {
            error = work();
        } catch (const std::exception& e) {
            error = QString::fromUtf8(e.what());
        } catch (...) {
            error = QStringLiteral("文件操作失败：未知异常");
        }
        qInfo().noquote() << title << "elapsedMs=" << timer.elapsed()
                          << (error.isEmpty() ? QStringLiteral("成功") : error);
        return error;
    }));
    return true;
}
}
