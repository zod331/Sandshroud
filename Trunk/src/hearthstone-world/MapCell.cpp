/*
 * Sandshroud Hearthstone
 * Copyright (C) 2010 - 2011 Sandshroud <http://www.sandshroud.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//
// MapCell.cpp
//
#include "StdAfx.h"
MapCell::MapCell()
{
	_forcedActive = false;
}

MapCell::~MapCell()
{
	RemoveObjects();
}

void MapCell::Init(uint32 x, uint32 y, uint32 mapid, MapMgr* mapmgr)
{
	_mapmgr = mapmgr;
	_active = false;
	_loaded = false;
	_playerCount = 0;
	_x=x;
	_y=y;
	_unloadpending=false;
	_objects.clear();
}

void MapCell::AddObject(Object* obj)
{
	if(obj->IsPlayer())
		++_playerCount;

	_objects.insert(obj);
}

void MapCell::RemoveObject(Object* obj)
{
	if(obj->IsPlayer())
		--_playerCount;

	_objects.erase(obj);
}

void MapCell::SetActivity(bool state)
{
	uint32 x, y;
	x = _x/8;
	y = _y/8;
	if(!_active && state)
	{
		// Move all objects to active set.
		for(ObjectSet::iterator itr = _objects.begin(); itr != _objects.end(); itr++)
		{
			if(!(*itr)->Active && (*itr)->CanActivate())
				(*itr)->Activate(_mapmgr);
		}

		if(_unloadpending)
			CancelPendingUnload();

		if (_mapmgr->IsCollisionEnabled())
		{
			CollideInterface.ActivateTile(_mapmgr->GetMapId(), x, y);

			if(sWorld.PathFinding)
				NavMeshInterface.LoadNavMesh(_mapmgr->GetMapId(), x, y);
		}
	}
	else if(_active && !state)
	{
		// Move all objects from active set.
		for(ObjectSet::iterator itr = _objects.begin(); itr != _objects.end(); itr++)
		{
			if((*itr)->Active)
				(*itr)->Deactivate(_mapmgr);
		}

		if(sWorld.map_unload_time && !_unloadpending)
			QueueUnloadPending();

		if (_mapmgr->IsCollisionEnabled())
		{
			CollideInterface.DeactivateTile(_mapmgr->GetMapId(), x, y);

			if(sWorld.PathFinding)
				NavMeshInterface.UnloadNavMesh(_mapmgr->GetMapId(), x, y);
		}
	}

	_active = state;

}
void MapCell::RemoveObjects()
{
	if(_loaded == false)
		return;

	_loaded = false;
	ObjectSet::iterator itr;

	/* delete objects in pending respawn state */
	Object* pObject;
	for(itr = _respawnObjects.begin(); itr != _respawnObjects.end(); itr++)
	{
		pObject = *itr;
		if(!pObject)
			continue;

		switch(pObject->GetTypeId())
		{
		case TYPEID_UNIT:
			{
				if( pObject->IsVehicle() )
				{
					_mapmgr->_reusable_guids_vehicle.push_back( pObject->GetUIdFromGUID() );
					TO_VEHICLE(pObject)->m_respawnCell=NULL;
					delete TO_VEHICLE(pObject);
					pObject = NULLOBJ;
				}
				else if( !pObject->IsPet() )
				{
					_mapmgr->_reusable_guids_creature.push_back( pObject->GetUIdFromGUID() );
					TO_CREATURE(pObject)->m_respawnCell=NULL;
					delete TO_CREATURE(pObject);
					pObject = NULLOBJ;
				}
			}break;

		case TYPEID_GAMEOBJECT:
			{
				TO_GAMEOBJECT(pObject)->m_respawnCell = NULL;
				delete TO_GAMEOBJECT(pObject);
				pObject = NULLOBJ;
			}break;
		default:
			delete pObject;
			pObject = NULLOBJ;
			break;

		}
	}
	_respawnObjects.clear();

	//This time it's simpler! We just remove everything :)
	Object* obj; //do this outside the loop!
	for(itr = _objects.begin(); itr != _objects.end();)
	{
		obj = (*itr);
		++itr;

		if(!obj)
			continue;

		if( _unloadpending )
		{
			if(obj->GetTypeFromGUID() == HIGHGUID_TYPE_TRANSPORTER)
				continue;

			if(obj->GetTypeId()==TYPEID_CORPSE && obj->GetUInt32Value(CORPSE_FIELD_OWNER) != 0)
				continue;

			if(!obj->m_loadedFromDB)
				continue;
		}

		if( obj->Active )
			obj->Deactivate( _mapmgr );

		if( obj->IsInWorld())
			obj->RemoveFromWorld( true );

		delete obj;
		obj = NULLOBJ;
	}
	_objects.clear();

	_playerCount = 0;
}

void MapCell::LoadObjects(CellSpawns * sp)
{
	if(_loaded == true)
		return;

	_loaded = true;
	Instance * pInstance = _mapmgr->pInstance;
	if(sp->CreatureSpawns.size())//got creatures
	{
		Vehicle* v = NULLVEHICLE;
		Creature* c = NULLCREATURE;
		for(CreatureSpawnList::iterator i=sp->CreatureSpawns.begin();i!=sp->CreatureSpawns.end();++i)
		{
			if(pInstance)
			{
				if(pInstance->m_killedNpcs.find((*i)->id) != pInstance->m_killedNpcs.end())
					continue;
			}

			if((*i)->vehicle > 0)
			{
				v =_mapmgr->CreateVehicle((*i)->entry);
				if(v == NULLVEHICLE)
					continue;

				v->SetMapId(_mapmgr->GetMapId());
				v->SetInstanceID(_mapmgr->GetInstanceID());
				v->m_loadedFromDB = true;

				if(v->Load(*i, _mapmgr->iInstanceMode, _mapmgr->GetMapInfo()))
				{
					if(!v->CanAddToWorld())
					{
						delete v;
						v = NULLVEHICLE;
						continue;
					}

					v->PushToWorld(_mapmgr);
				}
				else
				{
					delete v;
					v = NULLVEHICLE;
				}
			}
			else
			{
				c = _mapmgr->CreateCreature((*i)->entry);
				if(c == NULLCREATURE)
					continue;

				c->SetMapId(_mapmgr->GetMapId());
				c->SetInstanceID(_mapmgr->GetInstanceID());
				c->m_loadedFromDB = true;

				if(c->Load(*i, _mapmgr->iInstanceMode, _mapmgr->GetMapInfo()))
				{
					if(!c->CanAddToWorld())
					{
						delete c;
						c = NULLCREATURE;
						continue;
					}

					c->PushToWorld(_mapmgr);
				}
				else
				{
					delete c;
					c = NULLCREATURE;
				}
			}
		}
	}

	if(sp->GOSpawns.size())//got GOs
	{
		GameObject* go;
		for(GOSpawnList::iterator i = sp->GOSpawns.begin(); i != sp->GOSpawns.end(); i++)
		{
			go = _mapmgr->CreateGameObject((*i)->entry);
			if(go == NULL)
				continue;

			if(go->Load(*i))
			{
				go->PushToWorld(_mapmgr);
				CALL_GO_SCRIPT_EVENT(go, OnSpawn)();
			}
			else
			{
				delete go;
				go = NULLOBJ;
			}
		}
	}
}


void MapCell::QueueUnloadPending()
{
	if(_unloadpending)
		return;

	_unloadpending = true;
	DEBUG_LOG("MapCell", "Queueing pending unload of cell %u %u", _x, _y);
	sEventMgr.AddEvent(_mapmgr, &MapMgr::UnloadCell,(uint32)_x,(uint32)_y,MAKE_CELL_EVENT(_x,_y),sWorld.map_unload_time * 1000,1,0);
}

void MapCell::CancelPendingUnload()
{
	DEBUG_LOG("MapCell", "Cancelling pending unload of cell %u %u", _x, _y);
	if(!_unloadpending)
		return;

	sEventMgr.RemoveEvents(_mapmgr,MAKE_CELL_EVENT(_x,_y));
}

void MapCell::Unload()
{
	DEBUG_LOG("MapCell", "Unloading cell %u %u", _x, _y);
	ASSERT(_unloadpending);
	if(_active)
		return;

	RemoveObjects();
	_unloadpending = false;
}
