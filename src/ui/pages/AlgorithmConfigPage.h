#ifndef ALGORITHMCONFIGPAGE_H
#define ALGORITHMCONFIGPAGE_H

#include <QWidget>
#include <QMap>
#include "../../application/scheme/SchemeCatalog.h"

class ImageCanvasWidget;
class QListWidget;
class QLabel;
class OperatorEditor;
class RuleSetEditor;
class QPushButton;
class QStackedWidget;

class AlgorithmConfigPage : public QWidget {
    Q_OBJECT
public:
    explicit AlgorithmConfigPage(QWidget* parent = nullptr);
    QList<SchemeTool> configuredTools() const;

public Q_SLOTS:
    void setConfiguredTools(const QList<SchemeTool>& tools);

Q_SIGNALS:
    void configuredToolsChanged(const QList<SchemeTool>& tools);

protected:
    void showEvent(QShowEvent* event) override;

private:
    void initContent();
    void showToolEditor(int row);
    void showLibrary();
    void addTool(const QString& kind);
    void removeSelectedTool();
    void refreshReferencePreview();
    QWidget* createLibraryCard(const QString& title, const QString& symbol, const QString& kind);

    QListWidget* _configuredToolList { nullptr };
    QLabel* _toolCount { nullptr };
    QLabel* _editorTitle { nullptr };
    QStackedWidget* _contentStack { nullptr };
    QStackedWidget* _operatorStack { nullptr };
    QMap<QString, QPushButton*> _addButtons;
    QMap<QString, int> _editorIndexes;
    QMap<QString, OperatorEditor*> _editors;
    QMap<QString, RuleSetEditor*> _ruleEditors;
    QList<SchemeTool> _configuredTools;
};

#endif // ALGORITHMCONFIGPAGE_H
