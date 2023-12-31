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
class Basemap;
class Map;
class MapQuickView;
class OfflineMapTask;
class ExportVectorTilesTask;
class ArcGISVectorTiledLayer;
class Graphic;
class ExportVectorTilesJob;
class VectorTileCache;
class ItemResourceCache;
class PolylineBuilder;
} // namespace Esri::ArcGISRuntime

#include <QObject>

Q_MOC_INCLUDE("MapQuickView.h")

class QtAcademyProj : public QObject
{
  Q_OBJECT

  Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
  Q_PROPERTY(int downloadProgress READ downloadProgress NOTIFY downloadProgressChanged)
  Q_PROPERTY(bool isTracking READ isTracking WRITE setIsTracking NOTIFY isTrackingChanged)

public:
  explicit QtAcademyProj(QObject* parent = nullptr);
  ~QtAcademyProj() override;

  Q_INVOKABLE void createOfflineAreaFromExtent();
  Q_INVOKABLE void toggleOffline(bool offline);

signals:
  void mapViewChanged();
  void downloadProgressChanged();
  void isTrackingChanged();

private:
  Esri::ArcGISRuntime::MapQuickView* mapView() const;
  void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);
  void loadOfflineBasemaps();
  void exportVectorTiles(Esri::ArcGISRuntime::ArcGISVectorTiledLayer* vectorTileLayer);
  int downloadProgress();
  bool isTracking() const;
  void setIsTracking(bool value);

  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;

  Esri::ArcGISRuntime::OfflineMapTask* m_offlineMapTask = nullptr;
  Esri::ArcGISRuntime::ExportVectorTilesTask* m_exportVectorTilesTask = nullptr;
  Esri::ArcGISRuntime::Basemap* m_basemap = nullptr;

  Esri::ArcGISRuntime::Graphic* m_offlineMapExtentGraphic = nullptr;
  Esri::ArcGISRuntime::ExportVectorTilesJob* m_exportVectorTilesJob = nullptr;
  Esri::ArcGISRuntime::VectorTileCache* m_vectorTileCache = nullptr;
  Esri::ArcGISRuntime::ItemResourceCache* m_itemResourceCache = nullptr;
  Esri::ArcGISRuntime::ArcGISVectorTiledLayer* m_offlineLayer = nullptr;
  Esri::ArcGISRuntime::PolylineBuilder* m_lineBuilder = nullptr;
  int m_downloadProgress = 0;
  bool m_isTracking = false;
  std::unique_ptr<QObject> m_tempObject;
};

#endif // QTACADEMYPROJ_H
