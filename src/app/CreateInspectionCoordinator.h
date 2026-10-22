#pragma once
class QObject;
class InspectionService;
class InspectionCoordinator;
class SchemeCatalog;
class QString;
InspectionCoordinator* createInspectionCoordinator(InspectionService& service, QObject* parent);
bool activateCatalogScheme(InspectionCoordinator& coordinator, const SchemeCatalog& catalog, QString* error);
