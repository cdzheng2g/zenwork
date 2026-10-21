#pragma once
#include "application/operators/OperatorExecution.h"

void executeTemplateOperator(
    OperatorRunContext& context, const QJsonObject& parameters, OperatorOutcome& outcome);
