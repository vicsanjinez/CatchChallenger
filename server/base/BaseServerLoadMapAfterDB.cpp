#include "BaseServer.h"
#include "DictionaryServer.h"
#include "GlobalServerData.h"

using namespace CatchChallenger;

void BaseServer::preload_map_semi_after_db_id()
{
    if(DictionaryServer::dictionary_map_database_to_internal.size()==0)
    {
        std::cerr << "Need be called after preload_dictionary_map()" << std::endl;
        abort();
    }

    DictionaryServer::datapack_index_temp_for_item=0;
    DictionaryServer::datapack_index_temp_for_plant=0;
    unsigned int indexMapSemi=0;
    while(indexMapSemi<semi_loaded_map.size())
    {
        const Map_semi &map_semi=semi_loaded_map.at(indexMapSemi);
        MapServer * const mapServer=static_cast<MapServer *>(map_semi.map);
        const std::string &sortFileName=mapServer->map_file;
        //dirt/plant
        {
            unsigned int index=0;
            while(index<map_semi.old_map.dirts.size())
            {
                const Map_to_send::DirtOnMap_Semi &dirt=map_semi.old_map.dirts.at(index);

                uint16_t pointOnMapDbCode;
                std::pair<uint8_t/*x*/,uint8_t/*y*/> pair(dirt.point.x,dirt.point.y);
                bool found=false;
                if(DictionaryServer::dictionary_pointOnMap_internal_to_database.find(sortFileName)!=DictionaryServer::dictionary_pointOnMap_internal_to_database.end())
                {
                    const std::map<std::pair<uint8_t/*x*/,uint8_t/*y*/>,uint16_t/*db code*/> &subItem=DictionaryServer::dictionary_pointOnMap_internal_to_database.at(sortFileName);
                    if(subItem.find(pair)!=subItem.end())
                        found=true;
                }
                if(found)
                    pointOnMapDbCode=DictionaryServer::dictionary_pointOnMap_internal_to_database.at(sortFileName).at(pair);
                else
                {
                    dictionary_pointOnMap_maxId++;
                    std::string queryText;
                    switch(GlobalServerData::serverPrivateVariables.db_server->databaseType())
                    {
                        default:
                        case DatabaseBase::DatabaseType::Mysql:
                            queryText="INSERT INTO `dictionary_pointonmap`(`id`,`map`,`x`,`y`) VALUES("+
                                    std::to_string(dictionary_pointOnMap_maxId)+","+
                                    std::to_string(mapServer->reverse_db_id)+","+
                                    std::to_string(dirt.point.x)+","+
                                    std::to_string(dirt.point.y)+");"
                                    ;
                        break;
                        case DatabaseBase::DatabaseType::SQLite:
                            queryText="INSERT INTO dictionary_pointonmap(id,map,x,y) VALUES("+
                                    std::to_string(dictionary_pointOnMap_maxId)+","+
                                    std::to_string(mapServer->reverse_db_id)+","+
                                    std::to_string(dirt.point.x)+","+
                                    std::to_string(dirt.point.y)+");"
                                    ;
                        break;
                        case DatabaseBase::DatabaseType::PostgreSQL:
                            queryText="INSERT INTO dictionary_pointonmap(id,map,x,y) VALUES("+
                                    std::to_string(dictionary_pointOnMap_maxId)+","+
                                    std::to_string(mapServer->reverse_db_id)+","+
                                    std::to_string(dirt.point.x)+","+
                                    std::to_string(dirt.point.y)+");"
                                    ;
                        break;
                    }
                    if(!GlobalServerData::serverPrivateVariables.db_server->asyncWrite(queryText))
                    {
                        std::cerr << "Sql error for: " << queryText << ", error: " << GlobalServerData::serverPrivateVariables.db_server->errorMessage() << std::endl;
                        criticalDatabaseQueryFailed();abort();//stop because can't resolv the name
                    }
                    DictionaryServer::dictionary_pointOnMap_internal_to_database[sortFileName][std::pair<uint8_t,uint8_t>(dirt.point.x,dirt.point.y)]=dictionary_pointOnMap_maxId;
                    pointOnMapDbCode=dictionary_pointOnMap_maxId;
                }

                MapServer::PlantOnMap plantOnMap;
                #ifndef CATCHCHALLENGER_GAMESERVER_PLANTBYPLAYER
                plantOnMap.plant=0;//plant id
                plantOnMap.character=0;//player id
                plantOnMap.mature_at=0;//timestamp when is mature
                plantOnMap.player_owned_expire_at=0;//timestamp when is mature
                #endif
                plantOnMap.pointOnMapDbCode=pointOnMapDbCode;
                mapServer->plants[pair]=plantOnMap;

                {
                    while((uint32_t)DictionaryServer::dictionary_pointOnMap_database_to_internal.size()<=pointOnMapDbCode)
                    {
                        DictionaryServer::MapAndPoint mapAndPoint;
                        mapAndPoint.map=NULL;
                        mapAndPoint.x=0;
                        mapAndPoint.y=0;
                        mapAndPoint.datapack_index_item=0;
                        mapAndPoint.datapack_index_plant=0;
                        DictionaryServer::dictionary_pointOnMap_database_to_internal.push_back(mapAndPoint);
                    }
                    DictionaryServer::MapAndPoint &mapAndPoint=DictionaryServer::dictionary_pointOnMap_database_to_internal[pointOnMapDbCode];
                    mapAndPoint.map=mapServer;
                    mapAndPoint.x=dirt.point.x;
                    mapAndPoint.y=dirt.point.y;
                    mapAndPoint.datapack_index_plant=DictionaryServer::datapack_index_temp_for_plant;
                    DictionaryServer::datapack_index_temp_for_plant++;
                }
                index++;
            }
        }

        //item on map
        {
            unsigned int index=0;
            while(index<map_semi.old_map.items.size())
            {
                const Map_to_send::ItemOnMap_Semi &item=map_semi.old_map.items.at(index);

                const std::pair<uint8_t/*x*/,uint8_t/*y*/> pair(item.point.x,item.point.y);
                uint16_t pointOnMapDbCode;
                bool found=false;
                if(DictionaryServer::dictionary_pointOnMap_internal_to_database.find(sortFileName)!=DictionaryServer::dictionary_pointOnMap_internal_to_database.end())
                {
                    const std::map<std::pair<uint8_t/*x*/,uint8_t/*y*/>,uint16_t/*db code*/> &subItem=DictionaryServer::dictionary_pointOnMap_internal_to_database.at(sortFileName);
                    if(subItem.find(pair)!=subItem.end())
                        found=true;
                }
                if(found)
                    pointOnMapDbCode=DictionaryServer::dictionary_pointOnMap_internal_to_database.at(sortFileName).at(pair);
                else
                {
                    dictionary_pointOnMap_maxId++;
                    std::string queryText;
                    switch(GlobalServerData::serverPrivateVariables.db_server->databaseType())
                    {
                        default:
                        case DatabaseBase::DatabaseType::Mysql:
                            queryText="INSERT INTO `dictionary_pointonmap`(`id`,`map`,`x`,`y`) VALUES("+
                                    std::to_string(dictionary_pointOnMap_maxId)+","+
                                    std::to_string(mapServer->reverse_db_id)+","+
                                    std::to_string(item.point.x)+","+
                                    std::to_string(item.point.y)+");"
                                    ;
                        break;
                        case DatabaseBase::DatabaseType::SQLite:
                            queryText="INSERT INTO dictionary_pointonmap(id,map,x,y) VALUES("+
                                    std::to_string(dictionary_pointOnMap_maxId)+","+
                                    std::to_string(mapServer->reverse_db_id)+","+
                                    std::to_string(item.point.x)+","+
                                    std::to_string(item.point.y)+");"
                                    ;
                        break;
                        case DatabaseBase::DatabaseType::PostgreSQL:
                            queryText="INSERT INTO dictionary_pointonmap(id,map,x,y) VALUES("+
                                    std::to_string(dictionary_pointOnMap_maxId)+","+
                                    std::to_string(mapServer->reverse_db_id)+","+
                                    std::to_string(item.point.x)+","+
                                    std::to_string(item.point.y)+");"
                                    ;
                        break;
                    }
                    if(!GlobalServerData::serverPrivateVariables.db_server->asyncWrite(queryText))
                    {
                        std::cerr << "Sql error for: " << queryText << ", error: " << GlobalServerData::serverPrivateVariables.db_server->errorMessage() << std::endl;
                        criticalDatabaseQueryFailed();abort();//stop because can't resolv the name
                    }
                    DictionaryServer::dictionary_pointOnMap_internal_to_database[sortFileName][std::pair<uint8_t,uint8_t>(item.point.x,item.point.y)]=dictionary_pointOnMap_maxId;
                    pointOnMapDbCode=dictionary_pointOnMap_maxId;
                }

                MapServer::ItemOnMap itemOnMap;
                itemOnMap.infinite=item.infinite;
                itemOnMap.item=item.item;
                itemOnMap.pointOnMapDbCode=pointOnMapDbCode;
                mapServer->pointOnMap_Item[pair]=itemOnMap;

                {
                    while((uint32_t)DictionaryServer::dictionary_pointOnMap_database_to_internal.size()<=pointOnMapDbCode)
                    {
                        DictionaryServer::MapAndPoint mapAndPoint;
                        mapAndPoint.map=NULL;
                        mapAndPoint.x=0;
                        mapAndPoint.y=0;
                        mapAndPoint.datapack_index_item=0;
                        mapAndPoint.datapack_index_plant=0;
                        DictionaryServer::dictionary_pointOnMap_database_to_internal.push_back(mapAndPoint);
                    }
                    DictionaryServer::MapAndPoint &mapAndPoint=DictionaryServer::dictionary_pointOnMap_database_to_internal[pointOnMapDbCode];
                    mapAndPoint.map=mapServer;
                    mapAndPoint.x=item.point.x;
                    mapAndPoint.y=item.point.y;
                    mapAndPoint.datapack_index_item=DictionaryServer::datapack_index_temp_for_item;
                    DictionaryServer::datapack_index_temp_for_item++;
                }
                index++;
            }
        }
        indexMapSemi++;
    }

    semi_loaded_map.clear();
    plant_on_the_map=0;
    #ifndef EPOLLCATCHCHALLENGERSERVER
    preload_zone_sql();
    #else
    preload_market_monsters_sql();
    #endif
}