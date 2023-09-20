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

#include "QtAcademyProj.h"

#include "Map.h"
#include "MapTypes.h"
#include "MapQuickView.h"
#include "Viewpoint.h"
#include "TaskWatcher.h"
#include "LocationDisplay.h"
#include "Polygon.h"
#include "Envelope.h"
#include "Point.h"
#include "OfflineMapTask.h"
#include "GenerateOfflineMapJob.h"
#include "GenerateOfflineMapParameters.h"
#include "TaskTypes.h"
#include "Error.h"
#include "ExportVectorTilesTask.h"
#include "ExportVectorTilesJob.h"
#include "ExportVectorTilesParameters.h"
#include "ExportVectorTilesResult.h"
#include "ArcGISVectorTiledLayer.h"
#include "LayerListModel.h"
#include "Basemap.h"
#include "PortalItem.h"
#include "VectorTileCache.h"
#include "ItemResourceCache.h"
#include "ExportTileCacheTask.h"
#include "VectorTileSourceInfo.h"
#include "LevelOfDetail.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QDateTime>
#include <QNetworkInformation>

#include <QFuture>
#include <QStandardPaths>

using namespace Esri::ArcGISRuntime;

const QString vtpkPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+"/vtpks";

QtAcademyProj::QtAcademyProj(QObject* parent /* = nullptr */):
  QObject(parent),
  m_map(new Map(this))
{
  toggleOffline(false);
}

QtAcademyProj::~QtAcademyProj()
{
}

void QtAcademyProj::toggleOffline(bool offline)
{
  if (offline)
  {
    qDebug() << "going offline";
    m_map->setBasemap(new Basemap(this));
    loadBasemaps();
  }
  else
  {
    qDebug() << "going online";
    m_map->setBasemap(new Basemap(BasemapStyle::OsmStandard, this));
  }
}

void QtAcademyProj::loadBasemaps()
{
  QDirIterator dirIterator(vtpkPath, QDir::Dirs | QDir::NoDotAndDotDot);
  while (dirIterator.hasNext())
  {
    QString folderPath = dirIterator.next();

    if (QFile::exists(folderPath+"/vectorTiles.vtpk"))
    {
      VectorTileCache* vectorTileCache = new VectorTileCache(folderPath+"/vectorTiles.vtpk", this);
      ArcGISVectorTiledLayer* vLayer = nullptr;

      if (QFile::exists(folderPath+"/itemResources"))
      {
        ItemResourceCache* itemResourceChache = new ItemResourceCache(folderPath+"/itemResources", this);
        vLayer = new ArcGISVectorTiledLayer(vectorTileCache, itemResourceChache, this);
      }
      else
      {
        vLayer = new ArcGISVectorTiledLayer(vectorTileCache, this);
      }

      connect(vLayer, &ArcGISVectorTiledLayer::doneLoading, this, [=](const Error& loadError)
      {
        for (auto l : vLayer->sourceInfo().levelsOfDetail())
        {
          qDebug() << l.level() << l.resolution() << l.scale();
        }
      });

      m_map->basemap()->baseLayers()->append(vLayer);
    }
  }
}

void QtAcademyProj::toggleLocationDisplay()
{
  if (!m_mapView)
    return;

  if (m_mapView->locationDisplay()->isStarted())
    m_mapView->locationDisplay()->stop();
  else
    m_mapView->locationDisplay()->start();
}

void QtAcademyProj::createOfflineAreaFromExtent()
{
  for (Layer* layer : *m_map->basemap()->baseLayers())
  {
    if (ArcGISVectorTiledLayer* vectorTileLayer = dynamic_cast<ArcGISVectorTiledLayer*>(layer))
      exportVectorTiles(vectorTileLayer);
  }
}

void QtAcademyProj::exportVectorTiles(ArcGISVectorTiledLayer* vectorTileLayer)
{
  qDebug() << vectorTileLayer->url();
  m_exportVectorTilesTask = new ExportVectorTilesTask(vectorTileLayer->url(), this);

  connect(m_exportVectorTilesTask, &ExportVectorTilesTask::errorOccurred, this, [=](Error e)
  {
    qDebug() << e.message() << e.additionalMessage();
  });

  m_exportVectorTilesTask->createDefaultExportVectorTilesParametersAsync(m_mapView->visibleArea(), m_mapView->mapScale()*0.1)
      .then(this, [=](ExportVectorTilesParameters defaultParams)
  {
    qDebug() << defaultParams.maxLevel();
    const QString newVtpkPath = vtpkPath+"/"+QString::number(QDateTime::currentSecsSinceEpoch());

    QDir().mkdir(newVtpkPath);

    const QString vtpkFileName = newVtpkPath+"/vectorTiles.vtpk";
    const QString itemResourcesPath = newVtpkPath+"/itemResources";

    ExportVectorTilesJob* exportJob = m_exportVectorTilesTask->exportVectorTiles(defaultParams, vtpkFileName, itemResourcesPath);

    connect(exportJob, &Job::progressChanged, this, [exportJob, vectorTileLayer]()
    {
      qDebug() << vectorTileLayer->name() << exportJob->progress();
    });

    connect(exportJob, &Job::statusChanged, this, [exportJob, vectorTileLayer](JobStatus s)
    {
      qDebug() << vectorTileLayer->name() << s;
      qDebug() << exportJob->error().message() << exportJob->error().additionalMessage();
    });

    connect(exportJob, &Job::errorOccurred, this, [](const Error& e){qDebug() << e.message() << e.additionalMessage();});

    exportJob->start();
  });
}

MapQuickView* QtAcademyProj::mapView() const
{
  return m_mapView;
}

// Set the view (created in QML)
void QtAcademyProj::setMapView(MapQuickView* mapView)
{
  if (!mapView || mapView == m_mapView)
  {
    return;
  }

  m_mapView = mapView;

  m_mapView->setMap(m_map);

  //m_mapView->setViewpoint(Viewpoint(34.108, -116.918, 50'000));

  connect(m_mapView, &MapQuickView::mouseClicked, this, [=](const QMouseEvent& e)
  {
    qDebug() << this->children().size();
  });

  m_map->setMaxExtent(Envelope{});
  m_map->setMaxScale(0);
  m_map->setMinScale(0);

  emit mapViewChanged();
}

