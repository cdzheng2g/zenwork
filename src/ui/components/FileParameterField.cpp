#include "FileParameterField.h"
#include "ui/theme/AppTheme.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

FileParameterField::FileParameterField(const QString& filter, QWidget* parent) : QWidget(parent)
{
    auto* row = new QHBoxLayout(this);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(8);
    _path = new QLineEdit(this);
    _path->setMinimumHeight(AppTheme::ControlHeight);
    auto* browse = new QPushButton("选择", this);
    browse->setMinimumHeight(AppTheme::ControlHeight);
    row->addWidget(_path, 1);
    row->addWidget(browse);
    connect(_path, &QLineEdit::textEdited, this, &FileParameterField::changed);
    connect(browse, &QPushButton::clicked, this, [this, filter] {
        const auto path = QFileDialog::getOpenFileName(this, "选择资源文件", _path->text(), filter);
        if (!path.isEmpty() && path != _path->text()) {
            _path->setText(path);
            emit changed();
        }
    });
}
QString FileParameterField::value() const { return _path->text(); }
void FileParameterField::setValue(const QString& path) { _path->setText(path); }
