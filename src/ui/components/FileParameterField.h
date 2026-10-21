#pragma once
#include <QWidget>
#include <QString>
class QLineEdit;

class FileParameterField final : public QWidget {
    Q_OBJECT
public:
    explicit FileParameterField(const QString& filter, QWidget* parent = nullptr);
    QString value() const;
    void setValue(const QString& path);
signals:
    void changed();
private:
    QLineEdit* _path;
};
