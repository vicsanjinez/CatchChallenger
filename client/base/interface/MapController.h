#ifndef CATCHCHALLENGER_MAPCONTROLLER_H
#define CATCHCHALLENGER_MAPCONTROLLER_H

#include "MapControllerMP.h"
#include "../Api_protocol.h"

#include <QString>
#include <QList>
#include <QStringList>
#include <QHash>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QColor>

class MapController : public MapControllerMP
{
    Q_OBJECT
public:
    explicit MapController(const bool &centerOnPlayer=true, const bool &debugTags=false, const bool &useCache=true, const bool &OpenGL=false);
    ~MapController();
    virtual void connectAllSignals(CatchChallenger::Api_protocol * client);
    virtual void resetAll();
    QString mapIdToString(const uint32_t &mapId) const;
    void remove_plant_full(const QString &map,const uint8_t &x,const uint8_t &y);
    void insert_plant_full(const QString &map,const uint8_t &x,const uint8_t &y,const uint8_t &plant_id,const uint16_t &seconds_to_mature);
    void setColor(const QColor &color, const uint32_t &timeInMS=0);
    virtual bool asyncMapLoaded(const QString &fileName,MapVisualiserThread::Map_full * tempMapObject);
private:
    //the delayed action
    struct DelayedPlantInsert
    {
        uint32_t mapId;
        uint8_t x,y;
        uint8_t plant_id;
        uint16_t seconds_to_mature;
    };
    QList<DelayedPlantInsert> delayedPlantInsert;
    QMultiHash<QString,DelayedPlantInsert> delayedPlantInsertOnMap;
    struct PlantTimer
    {
        Tiled::MapObject * mapObject;
        uint8_t plant_id;
        uint16_t seconds_to_mature;
    };
    Tiled::Tileset *botFlags;
    QGraphicsPixmapItem *imageOver;
    bool imageOverAdded;
    QColor actualColor,tempColor,newColor;
    QTimer updateColorTimer;
    int updateColorIntervale;
    QTimer updateBotTimer;
protected slots:
    //plant
    void getPlantTimerEvent();
    bool updatePlantGrowing(CatchChallenger::ClientPlantWithTimer *plant);//return true if is growing
    void insert_plant(const uint32_t &mapId,const uint8_t &x,const uint8_t &y,const uint8_t &plant_id,const uint16_t &seconds_to_mature);
    void remove_plant(const uint32_t &mapId,const uint8_t &x,const uint8_t &y);
    void seed_planted(const bool &ok);
    void plant_collected(const CatchChallenger::Plant_collect &stat);
    virtual bool canGoTo(const CatchChallenger::Direction &direction,CatchChallenger::CommonMap map,COORD_TYPE x,COORD_TYPE y,const bool &checkCollision);
    void tryLoadPlantOnMapDisplayed(const QString &fileName);
    void updateGrowing();
    void updateColor();
    void loadPlayerFromCurrentMap();
    void updateBot();
public slots:
    virtual void datapackParsed();
    virtual void datapackParsedMainSub();
    virtual void reinject_signals();
private slots:
    void loadBotOnTheMap(MapVisualiserThread::Map_full *parsedMap, const uint32_t &botId, const uint8_t &x, const uint8_t &y, const QString &lookAt, const QString &skin);
protected:
    static QString text_random;
    static QString text_loop;
    static QString text_move;
    static QString text_left;
    static QString text_right;
    static QString text_top;
    static QString text_bottom;
    static QString text_slash;
    static QString text_type;
    static QString text_fightRange;
    static QString text_fight;
    static QString text_fightid;
    static QString text_bot;
    static QString text_slashtrainerpng;
    static QString text_DATAPACK_BASE_PATH_SKIN;
};

#endif // MAPCONTROLLER_H
