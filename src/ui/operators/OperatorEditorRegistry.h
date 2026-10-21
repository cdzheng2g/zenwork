#pragma once
#include "OperatorEditor.h"
#include <functional>
using OperatorEditorFactory = std::function<OperatorEditor*(QWidget*)>;
class OperatorEditorRegistry {
public:
    static OperatorEditor* create(const QString& kind, QWidget* parent);
};
