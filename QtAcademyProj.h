// Copyright 2023 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef QTACADEMYPROJ_H
#define QTACADEMYPROJ_H

namespace Esri::ArcGISRuntime {
class Map;
class MapQuickView;
class OfflineMapTask;
class ExportVectorTilesTask;
class ArcGISVectorTiledLayer;
} // namespace Esri::ArcGISRuntime

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h")

class QtAcademyProj : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  //Q_PROPERTY(bool isOffline READ isOffline WRITE setIsOffline NOTIFY isOfflineChanged)

public:
  explicit QtAcademyProj(QObject* parent = nullptr);
  ~QtAcademyProj() override;

  Q_INVOKABLE void toggleLocationDisplay();
  Q_INVOKABLE void createOfflineAreaFromExtent();
  Q_INVOKABLE void toggleOffline(bool offline);

signals:
  void mapViewChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void loadBasemaps();
  void exportVectorTiles(Esri::ArcGISRuntime::ArcGISVectorTiledLayer* vectorTileLayer);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::OfflineMapTask* m_offlineMapTask = nullptr;
  Esri::ArcGISRuntime::ExportVectorTilesTask* m_exportVectorTilesTask = nullptr;
};

#endif // QTACADEMYPROJ_H
