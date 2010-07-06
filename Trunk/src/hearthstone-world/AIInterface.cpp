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

#include "StdAfx.h"


AIInterface::AIInterface()
{
	m_ChainAgroSet = NULL;
	m_waypoints=NULL;
	m_canMove = true;
	m_destinationX = m_destinationY = m_destinationZ = 0;
	m_nextPosX = m_nextPosY = m_nextPosZ = 0;
	UnitToFollow = NULLCREATURE;
	FollowDistance = 0.0f;
	m_fallowAngle = float(M_PI/2);
	m_timeToMove = 0;
	m_timeMoved = 0;
	m_moveTimer = 0;
	m_WayPointsShowing = false;
	m_WayPointsShowBackwards = false;
	m_currentWaypoint = 0;
	m_moveBackward = false;
	m_moveType = 0;
	m_moveRun = false;
	m_moveSprint = false;
	m_moveFly = false;
	m_creatureState = STOPPED;
	m_canCallForHelp = false;
	m_hasCalledForHelp = false;
	m_fleeTimer = 0;
	m_FleeDuration = 0;
	m_canFlee = false;
	m_hasFled = false;
	m_canRangedAttack = false;
	m_FleeHealth = m_CallForHelpHealth = 0.0f;
	m_AIState = STATE_IDLE;

	m_updateAssist = false;
	m_updateTargets = false;
	m_updateAssistTimer = 1;
	m_updateTargetsTimer = TARGET_UPDATE_INTERVAL;

	m_nextSpell = NULL;
	m_nextTarget = NULLUNIT;
	totemspell = NULL;
	m_Unit = NULLUNIT;
	m_PetOwner = NULLUNIT;
	m_aiCurrentAgent = AGENT_NULL;
	m_runSpeed = 0.0f;
	m_flySpeed = 0.0f;
	UnitToFear = NULLUNIT;
	firstLeaveCombat = true;
	m_outOfCombatRange = 10000;

	tauntedBy = NULLUNIT;
	isTaunted = false;
	m_AllowedToEnterCombat = true;
	m_totalMoveTime = 0;
	m_lastFollowX = m_lastFollowY = 0;
	m_FearTimer = 0;
	m_WanderTimer = 0;
	m_totemspelltime = 0;
	m_totemspelltimer = 0;
	m_formationFollowAngle = 0.0f;
	m_formationFollowDistance = 0.0f;
	m_formationLinkTarget = NULLCREATURE;
	m_formationLinkSqlId = 0;
	m_currentHighestThreat = 0;

	disable_combat = false;

	disable_melee = false;
	disable_ranged = false;
	disable_spell = false;

	disable_targeting = false;

	next_spell_time = 0;

	waiting_for_cooldown = false;
	UnitToFollow_backup = NULLUNIT;
	m_isGuard = false;
	m_is_in_instance=false;
	skip_reset_hp=false;
	m_guardCallTimer = 0;

	m_aiTargets.clear();
	m_assistTargets.clear();
	m_spells.clear();

}

void AIInterface::Init(Unit* un, AIType at, MovementType mt)
{
	ASSERT(at != AITYPE_PET);

	m_AIType = at;
	m_MovementType = mt;

	m_AIState = STATE_IDLE;
	m_MovementState = MOVEMENTSTATE_STOP;

	m_Unit = un;

	m_walkSpeed = m_Unit->m_walkSpeed*0.001f;//move distance per ms time 
	m_runSpeed = m_Unit->m_runSpeed*0.001f;//move distance per ms time 
	m_flySpeed = m_Unit->m_flySpeed * 0.001f;
	/*if(m_DefaultMeleeSpell == NULL)
	{
		m_DefaultMeleeSpell = new AI_Spell;
		m_DefaultMeleeSpell->entryId = 0;
		m_DefaultMeleeSpell->spellType = 0;
		m_DefaultMeleeSpell->agent = AGENT_MELEE;
		m_DefaultSpell = m_DefaultMeleeSpell;
	}*/
	m_sourceX = un->GetPositionX();
	m_sourceY = un->GetPositionY();
	m_sourceZ = un->GetPositionZ();
	m_guardTimer = getMSTime();
}

AIInterface::~AIInterface()
{
	for(list<AI_Spell*>::iterator itr = m_spells.begin(); itr != m_spells.end(); itr++)
		if(*itr)
			delete (*itr);

	m_spells.clear();

	if( m_ChainAgroSet && m_Unit->IsCreature() )
	{
		m_ChainAgroSet->RemoveAggroEntity( TO_CREATURE(m_Unit) );
	}

	m_formationLinkTarget = NULLCREATURE;
	m_Unit = NULLUNIT;
	m_PetOwner = NULLUNIT;
	soullinkedWith = NULLUNIT;
	UnitToFollow_backup = NULLUNIT;
}

void AIInterface::Init(Unit* un, AIType at, MovementType mt, Unit* owner)
{
	ASSERT(at == AITYPE_PET || at == AITYPE_TOTEM);

	m_AIType = at;
	m_MovementType = mt;

	m_AIState = STATE_IDLE;
	m_MovementState = MOVEMENTSTATE_STOP;

	m_Unit = un;
	m_PetOwner = owner;

	m_walkSpeed = m_Unit->m_walkSpeed*0.001f;//move distance per ms time 
	m_runSpeed = m_Unit->m_runSpeed*0.001f;//move/ms
	m_flySpeed = m_Unit->m_flySpeed*0.001f;
	m_sourceX = un->GetPositionX();
	m_sourceY = un->GetPositionY();
	m_sourceZ = un->GetPositionZ();
}

void AIInterface::HandleEvent(uint32 event, Unit* pUnit, uint32 misc1)
{
	if( m_Unit == NULL )
		return;

	Creature* cr = NULLCREATURE;
	if( m_Unit->GetTypeId() == TYPEID_UNIT )
	{
		cr = TO_CREATURE( m_Unit );
		if(cr == NULL)
			return;

		if(isTargetDummy(cr->GetEntry()) && event != EVENT_LEAVECOMBAT)
			return;
	}

	if(m_AIState != STATE_EVADE)
	{
		switch(event)
		{
		case EVENT_ENTERCOMBAT:
			{
				if( pUnit == NULL ) 
					return;

				/* send the message */
				if( cr != NULL )
				{
					if( cr->has_combat_text )
						objmgr.HandleMonsterSayEvent( cr, MONSTER_SAY_EVENT_ENTER_COMBAT );

					if( cr->m_spawn && ( cr->m_spawn->channel_target_go || cr->m_spawn->channel_target_creature))
					{
						m_Unit->SetUInt32Value(UNIT_CHANNEL_SPELL, 0);
						m_Unit->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, 0);
					}
				}
				
				// Stop the emote
				m_Unit->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
				m_returnX = m_Unit->GetPositionX();
				m_returnY = m_Unit->GetPositionY();
				m_returnZ = m_Unit->GetPositionZ();

				m_moveRun = true; //run to the target

				// dismount if mounted
				m_Unit->Dismount();

				if(m_AIState != STATE_ATTACKING)
					StopMovement(0);

				m_AIState = STATE_ATTACKING;
				firstLeaveCombat = true;

				if(pUnit->GetInstanceID() == m_Unit->GetInstanceID())
					m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, pUnit->GetGUID());

				CALL_SCRIPT_EVENT(m_Unit, OnCombatStart)(pUnit);

				if( m_ChainAgroSet )
					m_ChainAgroSet->EventEnterCombat( pUnit );

				//Mark raid as combat in progress if it concerns a boss 
				if(pUnit->GetMapMgr() && pUnit->GetMapMgr()->GetMapInfo() && pUnit->GetMapMgr()->GetMapInfo()->type == INSTANCE_RAID)
					if(m_Unit->GetTypeId() == TYPEID_UNIT && m_Unit->m_loadedFromDB )
						if(cr->GetCreatureInfo() && cr->GetCreatureInfo()->Rank == ELITE_WORLDBOSS)
							 pUnit->GetMapMgr()->AddCombatInProgress(m_Unit->GetGUID());
			}break;
		case EVENT_LEAVECOMBAT:
			{
				if( pUnit == NULL ) 
					return;

				if( pUnit->GetTypeId() == TYPEID_UNIT && !pUnit->IsPet() && !pUnit->isAlive() )
					pUnit->RemoveAllNegativeAuras();

				//cancel spells that we are casting. Should remove bug where creatures cast a spell after they died
				if( pUnit->IsCreature() )
					CancelSpellCast();

				// restart emote
				if(cr != NULL)
				{
					cr->SetSheatheForAttackType(0); // sheathe yuor weapons!

					if( cr->has_combat_text )
						objmgr.HandleMonsterSayEvent( cr, MONSTER_SAY_EVENT_ON_COMBAT_STOP );

					if(cr->original_emotestate)
						m_Unit->SetUInt32Value(UNIT_NPC_EMOTESTATE, cr->original_emotestate);
					
					if(cr->m_spawn)
					{
						if(cr->m_spawn->channel_target_go || cr->m_spawn->channel_target_creature)
						{
							if(cr->m_spawn->channel_target_go)
								sEventMgr.AddEvent( cr, &Creature::ChannelLinkUpGO, cr->m_spawn->channel_target_go, EVENT_CREATURE_CHANNEL_LINKUP, 1000, 5, 0 );

							if(cr->m_spawn->channel_target_creature)
								sEventMgr.AddEvent( cr, &Creature::ChannelLinkUpCreature, cr->m_spawn->channel_target_creature, EVENT_CREATURE_CHANNEL_LINKUP, 1000, 5, 0 );
						}
						// Remount if mounted
						if( cr->m_spawn->MountedDisplayID )
							m_Unit->SetUInt32Value( UNIT_FIELD_MOUNTDISPLAYID, cr->m_spawn->MountedDisplayID );
					}
				}

				//reset ProcCount
				//ResetProcCounts();
				m_moveRun = true;
				m_aiTargets.clear();			
				m_fleeTimer = 0;
				m_hasFled = false;
				m_hasCalledForHelp = false;
				m_nextSpell = NULL;
				SetNextTarget(NULLUNIT);
				m_Unit->CombatStatus.Vanished();

				if(m_AIType == AITYPE_PET)
				{
					m_AIState = STATE_FOLLOWING;
					UnitToFollow = m_PetOwner;
					FollowDistance = 3.0f;
					m_lastFollowX = m_lastFollowY = 0;
					if(m_Unit->IsPet())
					{
						TO_PET(m_Unit)->SetPetAction(PET_ACTION_FOLLOW);
						if( m_Unit->GetEntry() == 416 && m_Unit->isAlive() && m_Unit->IsInWorld() )
						{
							TO_PET(m_Unit)->HandleAutoCastEvent(AUTOCAST_EVENT_LEAVE_COMBAT);
						}
					}
					HandleEvent(EVENT_FOLLOWOWNER, NULLUNIT, 0);
				}
				else
				{
					CALL_SCRIPT_EVENT(m_Unit, OnCombatStop)(UnitToFollow);
					m_AIState = STATE_EVADE;

					UnitToFollow = NULLUNIT;
					FollowDistance = 0.0f;
					m_lastFollowX = m_lastFollowY = 0;

					if(m_Unit->isAlive())
					{
						if(m_returnX != 0.0f && m_returnY != 0.0f && m_returnZ != 0.0f)
							MoveTo(m_returnX,m_returnY,m_returnZ);
						else
						{
							MoveTo(m_Unit->GetSpawnX(),m_Unit->GetSpawnY(),m_Unit->GetSpawnZ());
							m_returnX=m_Unit->GetSpawnX();
							m_returnY=m_Unit->GetSpawnY();
							m_returnZ=m_Unit->GetSpawnZ();
						}

						// clear tagger
						if(cr != NULL)
							cr->ClearTag();
					}
				}

				if( cr!= NULL && m_Unit->GetMapMgr() )
					m_Unit->GetMapMgr()->RemoveCombatInProgress(m_Unit->GetGUID());
			}break;
		case EVENT_DAMAGETAKEN:
			{
				if( pUnit == NULL ) return;

				if( cr!= NULL && cr->has_combat_text )
					objmgr.HandleMonsterSayEvent( TO_CREATURE( m_Unit ), MONSTER_SAY_EVENT_ON_DAMAGE_TAKEN );

				CALL_SCRIPT_EVENT(m_Unit, OnDamageTaken)(pUnit, float(misc1));
				if(!modThreatByPtr(pUnit, misc1))
				{
					if( pUnit->IsPlayer() )
					{
						if( pUnit->mThreatRTarget )
							m_aiTargets.insert(TargetMap::value_type(TO_PLAYER(pUnit->mThreatRTarget), misc1));
						else
							m_aiTargets.insert(TargetMap::value_type(pUnit, misc1));
					}
					else
						m_aiTargets.insert(TargetMap::value_type(pUnit, misc1));
				}
				m_Unit->CombatStatus.OnDamageDealt(pUnit, 1);
			}break;
		case EVENT_FOLLOWOWNER:
			{
				m_AIState = STATE_FOLLOWING;
				if(m_Unit->IsPet())
					TO_PET(m_Unit)->SetPetAction(PET_ACTION_FOLLOW);
				UnitToFollow = m_PetOwner;
				m_lastFollowX = m_lastFollowY = 0;
				FollowDistance = 4.0f;

				m_aiTargets.clear();
				m_fleeTimer = 0;
				m_hasFled = false;
				m_hasCalledForHelp = false;
				m_nextSpell = NULL;
				SetNextTarget(NULLUNIT);
				m_moveRun = true;
			}break;

		case EVENT_FEAR:
			{
				if( pUnit == NULL ) return;

				m_FearTimer = 0;
				SetUnitToFear(pUnit);

				CALL_SCRIPT_EVENT(m_Unit, OnFear)(pUnit, 0);
				m_AIState = STATE_FEAR;
				StopMovement(1);

				UnitToFollow_backup = UnitToFollow;
				UnitToFollow = NULLUNIT;
				m_lastFollowX = m_lastFollowY = 0;
				FollowDistance_backup = FollowDistance;
				FollowDistance = 0.0f;

				m_fleeTimer = 0;
				m_hasFled = false;
				m_hasCalledForHelp = false;

				// update speed
				m_moveRun = true;
				getMoveFlags();

				SetNextSpell(NULL);
				SetNextTarget(NULLUNIT);
			}break;

		case EVENT_UNFEAR:
			{
				UnitToFollow = UnitToFollow_backup;
				FollowDistance = FollowDistance_backup;
				m_AIState = STATE_IDLE; // we need this to prevent permanent fear, wander, and other problems

				SetUnitToFear(NULLUNIT);
				StopMovement(1);
			}break;

		case EVENT_WANDER:
			{
				if( pUnit == NULL ) return;

				m_WanderTimer = 0;

				//CALL_SCRIPT_EVENT(m_Unit, OnWander)(pUnit, 0); FIXME
				m_AIState = STATE_WANDER;
				StopMovement(1);

				m_aiTargets.clear(); // we'll get a new target after we are unwandered
				m_fleeTimer = 0;
				m_hasFled = false;
				m_hasCalledForHelp = false;

				// update speed
				m_moveRun = true;
				getMoveFlags();

				SetNextSpell(NULL);
				SetNextTarget(NULLUNIT);
			}break;

		case EVENT_UNWANDER:
			{
				UnitToFollow = UnitToFollow_backup;
				FollowDistance = FollowDistance_backup;
				m_AIState = STATE_IDLE; // we need this to prevent permanent fear, wander, and other problems

				StopMovement(1);
			}break;

		default:
			{
			}break;
		}
	}

	//Should be able to do this stuff even when evading
	switch(event)
	{
		case EVENT_UNITDIED:
		{
			if( pUnit == NULL ) 
				return;

			if( pUnit->m_CurrentVehicle )
				pUnit->m_CurrentVehicle->RemovePassenger(pUnit);

			if( cr != NULL )
			{
				if( cr->has_combat_text )
					objmgr.HandleMonsterSayEvent( cr, MONSTER_SAY_EVENT_ON_DIED );

				if(UnitToFollow != NULL)
					UnitToFollow = NULLUNIT;

				if(UnitToFollow_backup != NULL)
					UnitToFollow_backup = NULLUNIT;

				m_lastFollowX = m_lastFollowY = 0;
				FollowDistance = 0.0f;
			}

			CALL_SCRIPT_EVENT(m_Unit, OnDied)(pUnit);
			if ( m_Unit->IsCreature() )
				CALL_INSTANCE_SCRIPT_EVENT( m_Unit->GetMapMgr(), OnCreatureDeath )( TO_CREATURE( m_Unit ), pUnit );

			m_AIState = STATE_IDLE;

			StopMovement(0);
			m_aiTargets.clear();
			UnitToFear = NULLUNIT;
			m_fleeTimer = 0;
			m_hasFled = false;
			m_hasCalledForHelp = false;
			m_nextSpell = NULL;

			SetNextTarget(NULLUNIT);
		
			//reset waypoint to 1
			m_currentWaypoint = 1;
			
			// There isn't any need to do any attacker checks here, as
			// they should all be taken care of in DealDamage

			if(cr != NULL && !m_Unit->IsPet())
			{
				//only save creature which exist in db (don't want to save 0 values in db) 
				if( m_Unit->m_loadedFromDB && cr->m_spawn != NULL )
				{
					MapMgr* GMap = m_Unit->GetMapMgr();
					if(GMap != NULL)
					{
						if( GMap && GMap->pInstance && GMap->GetMapInfo()->type != INSTANCE_PVP )
						{
							GMap->pInstance->m_killedNpcs.insert( cr->GetSQL_id() );
							GMap->pInstance->SaveToDB();
						}
					}
				}
			}
		}break;

	case EVENT_UNITRESPAWN:
		{
			/* send the message */
			if( cr != NULL )
			{
				if( cr->has_combat_text )
					objmgr.HandleMonsterSayEvent( cr, MONSTER_SAY_EVENT_ON_SPAWN );
			}
		}break;
	}
}

void AIInterface::Update(uint32 p_time)
{
	float tdist;
	if(m_AIType == AITYPE_TOTEM)
	{
		assert(totemspell != 0);
		if(p_time >= m_totemspelltimer)
		{
			Spell* pSpell = NULLSPELL;
			pSpell = new Spell(m_Unit, totemspell, true, NULLAURA);

			SpellCastTargets targets(0);
			if(!m_nextTarget ||
				(m_nextTarget && 
					(!m_Unit->GetMapMgr()->GetUnit(m_nextTarget->GetGUID()) || 
					!m_nextTarget->isAlive() ||
					(m_nextTarget->GetTypeId() == TYPEID_UNIT && TO_CREATURE(m_nextTarget)->IsTotem()) ||
					!IsInrange(m_Unit,m_nextTarget,pSpell->m_spellInfo->base_range_or_radius_sqr) ||
					!isAttackable(m_Unit, m_nextTarget,!(pSpell->m_spellInfo->c_is_flags & SPELL_FLAG_IS_TARGETINGSTEALTHED))
					)
				)
				)
			{
				//we set no target and see if we managed to fid a new one
				m_nextTarget=NULLUNIT;
				//something happend to our target, pick another one
				pSpell->GenerateTargets(&targets);
				if(targets.m_targetMask & TARGET_FLAG_UNIT)
					m_nextTarget = m_Unit->GetMapMgr()->GetUnit(targets.m_unitTarget);
			}
			if(m_nextTarget)
			{
				SpellCastTargets targets(m_nextTarget->GetGUID());
				pSpell->prepare(&targets);
				// need proper cooldown time!
				m_totemspelltimer = m_totemspelltime;
			}
			else
			{
				delete pSpell;
				pSpell = NULL;
			}
			// these will *almost always* be AoE, so no need to find a target here.
//			SpellCastTargets targets(m_Unit->GetGUID());
//			Spell* pSpell = NULLSPELL
//			pSpell = new Spell(m_Unit, totemspell, true, NULLAURA);
//			pSpell->prepare(&targets);
			// need proper cooldown time!
//			m_totemspelltimer = m_totemspelltime;
		}
		else
		{
			m_totemspelltimer -= p_time;
		}
		return;
	}

	_UpdateTimer(p_time);
	_UpdateTargets();
	if(m_Unit->isAlive() && m_AIState != STATE_IDLE 
		&& m_AIState != STATE_FOLLOWING && m_AIState != STATE_FEAR 
		&& m_AIState != STATE_WANDER && m_AIState != STATE_SCRIPTMOVE)
	{
		if(m_AIType == AITYPE_PET )
		{
			if(!m_Unit->bInvincible && m_Unit->IsPet()) 
			{
				Pet* pPet = TO_PET(m_Unit);
	
				if(pPet->GetPetAction() == PET_ACTION_ATTACK || pPet->GetPetState() != PET_STATE_PASSIVE)
				{
					_UpdateCombat(p_time);
				}
			}
			//we just use any creature as a pet guardian
			else if(!m_Unit->IsPet())
			{
				_UpdateCombat(p_time);
			}
		}
		else
		{
			_UpdateCombat(p_time);
		}
	}

	_UpdateMovement(p_time);
	if(m_AIState==STATE_EVADE)
	{
		tdist = m_Unit->GetDistanceSq(m_returnX,m_returnY,m_returnZ);
		if(tdist <= 4.0f/*2.0*/)
		{
			m_AIState = STATE_IDLE;
			m_returnX = m_returnY = m_returnZ = 0.0f;
			m_moveRun = false;
			//remowed by zack : in scripted events if we keep reducing this it will bug the world out !
			//On Blizz it will return to previous wp but we can accept the fact that it will move on to next one
			/*
			if(hasWaypoints())
			{
				if(m_moveBackward)
				{
					if(m_currentWaypoint != GetWayPointsCount()-1)
						m_currentWaypoint++;
				}
				else
				{
					if(m_currentWaypoint != 0)
						m_currentWaypoint--;
				}
			}
			*/
			// Set health to full if they at there last location before attacking
			if(m_AIType != AITYPE_PET&&!skip_reset_hp)
				m_Unit->SetUInt32Value(UNIT_FIELD_HEALTH,m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH));
		}
		else
		{
			if( m_creatureState == STOPPED )
			{
				// return to the home
				if( m_returnX == 0.0f && m_returnY == 0.0f )
				{
					m_returnX = m_Unit->GetSpawnX();
					m_returnY = m_Unit->GetSpawnY();
					m_returnZ = m_Unit->GetSpawnZ();
				}

				MoveTo(m_returnX, m_returnY, m_returnZ);
			}
		}
	}

	if(m_fleeTimer)
	{
		if(m_fleeTimer > p_time)
		{
			m_fleeTimer -= p_time;
			if(!m_nextTarget) //something happened to our target, lets find another one
				SetNextTarget(FindTargetForSpell(m_nextSpell));
			if(m_nextTarget)
				_CalcDestinationAndMove(m_nextTarget, 5.0f);
		}
		else
		{
			m_fleeTimer = 0;
			SetNextTarget(FindTargetForSpell(m_nextSpell));
		}
	}

	//Pet Dismiss after a certian ditance away
	/*if(m_AIType == AITYPE_PET && m_PetOwner != NULL)
	{
		float dist = (m_Unit->GetInstanceID() == m_PetOwner->GetInstanceID()) ? 
			m_Unit->GetDistanceSq(m_PetOwner) : 99999.0f;

		if(dist > 8100.0f) //90 yard away we Dismissed
		{
			DismissPet();
			return;
		}
	}*/
}

void AIInterface::_UpdateTimer(uint32 p_time)
{
	if(m_updateAssistTimer > p_time)
	{
		m_updateAssistTimer -= p_time;
	}else
	{
		m_updateAssist = true;
		m_updateAssistTimer = TARGET_UPDATE_INTERVAL * 2 - m_updateAssistTimer - p_time;
	}

	if(m_updateTargetsTimer > p_time)
	{
		m_updateTargetsTimer -= p_time;
	}else
	{
		m_updateTargets = true;
		m_updateTargetsTimer = TARGET_UPDATE_INTERVAL * 2 - m_updateTargetsTimer - p_time;
	}
}

void AIInterface::_UpdateTargets()
{
	if( m_Unit->IsPlayer() || (m_AIType != AITYPE_PET && disable_targeting ))
		return;

	if( TO_CREATURE(m_Unit)->GetCreatureInfo() && TO_CREATURE(m_Unit)->GetCreatureInfo()->Type == CRITTER )
		return;

	AssistTargetSet::iterator i, i2;
	TargetMap::iterator itr, it2;

	// Find new Assist Targets and remove old ones
	if(m_AIState == STATE_FLEEING)
	{
		FindFriends(100.0f/*11.0*/);
	}
	else if(m_AIState != STATE_IDLE && m_AIState != STATE_SCRIPTIDLE)
	{
		FindFriends(16.0f/*4.0f*/);
	}

	if( m_updateAssist )
	{
		m_updateAssist = false;
	/*	deque< Unit > tokill;

		//modified for vs2005 compatibility
		for(i = m_assistTargets.begin(); i != m_assistTargets.end(); i++)
		{
			if(m_Unit->GetDistanceSq((*i)) > 2500.0f|| !(*i)->isAlive() || !(*i)->CombatStatus.IsInCombat())
			{
				tokill.push_back(*i);
			}
		}

		for(deque< Unit >::iterator i2 = tokill.begin(); i2 != tokill.end(); i2++)
			m_assistTargets.erase(*i2);*/

		for(i = m_assistTargets.begin(); i != m_assistTargets.end();)
		{
			i2 = i;
			++i;
			if((*i2) == NULL || (*i2)->event_GetCurrentInstanceId() != m_Unit->event_GetCurrentInstanceId() || !m_Unit->PhasedCanInteract(*i2) ||
				!(*i2)->isAlive() || m_Unit->GetDistanceSq((*i2)) >= 2500.0f || !(*i2)->CombatStatus.IsInCombat() )
			{
				m_assistTargets.erase( i2 );
			}
		}
	}

	if( m_updateTargets )
	{
		m_updateTargets = false;
		/*deque< Unit > tokill;

		//modified for vs2005 compatibility
		for(itr = m_aiTargets.begin(); itr != m_aiTargets.end();itr++)
		{
			if(!itr->first->isAlive() || m_Unit->GetDistanceSq(itr->first) >= 6400.0f)
			{
				tokill.push_back(itr->first);
			}
		}
		for(deque< Unit >::iterator itr = tokill.begin(); itr != tokill.end(); itr++)
			m_aiTargets.erase((*itr));
		tokill.clear();*/

		for(itr = m_aiTargets.begin(); itr != m_aiTargets.end();)
		{
			it2 = itr++;

			if( it2->first->event_GetCurrentInstanceId() != m_Unit->event_GetCurrentInstanceId() || !m_Unit->PhasedCanInteract(it2->first) ||
				!it2->first->isAlive() || m_Unit->GetDistanceSq(it2->first) >= 6400.0f )
			{
				m_aiTargets.erase( it2 );
			}
		}
		
		if(m_aiTargets.size() == 0 
			&& m_AIState != STATE_IDLE && m_AIState != STATE_FOLLOWING 
			&& m_AIState != STATE_EVADE && m_AIState != STATE_FEAR 
			&& m_AIState != STATE_WANDER && m_AIState != STATE_SCRIPTIDLE)
		{
			if(firstLeaveCombat)
			{
				Unit* target = FindTarget();
				if(target)
				{
					AttackReaction(target, 1, 0);
				}else
				{
					firstLeaveCombat = false;
				}
			}
			/*else
			{
				HandleEvent(EVENT_LEAVECOMBAT, m_Unit, 0);
			}*/
		}
		else if( m_aiTargets.size() == 0 && (m_AIType == AITYPE_PET && (m_Unit->IsPet() && TO_PET(m_Unit)->GetPetState() == PET_STATE_AGGRESSIVE) || (!m_Unit->IsPet() && disable_melee == false ) ) )
		{
			 Unit* target = FindTarget();
			 if( target )
			 {
				 AttackReaction(target, 1, 0);
			 }

		}
	}
	// Find new Targets when we are ooc
	if((m_AIState == STATE_IDLE || m_AIState == STATE_SCRIPTIDLE) && m_assistTargets.size() == 0)
	{
		Unit* target = FindTarget();
		if(target)
		{
			AttackReaction(target, 1, 0);
		}
	}
}

///====================================================================
///  Desc: Updates Combat Status of m_Unit
///====================================================================
void AIInterface::_UpdateCombat(uint32 p_time)
{
	if( m_AIType != AITYPE_PET && disable_combat )
		return;

	//just make sure we are not hitting self. This was reported as an exploit.Should never ocure anyway
	if( m_nextTarget == m_Unit )
		m_nextTarget = GetMostHated();

	uint16 agent = m_aiCurrentAgent;

	if(	m_AIType != AITYPE_PET 
			&& (m_outOfCombatRange && m_Unit->GetDistanceSq(m_returnX,m_returnY,m_returnZ) > m_outOfCombatRange) 
			&& m_AIState != STATE_EVADE
			&& m_AIState != STATE_SCRIPTMOVE
			&& !m_is_in_instance)
		HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0 );
	else if( m_nextTarget == NULL && m_AIState != STATE_FOLLOWING && m_AIState != STATE_SCRIPTMOVE )
	{
		m_nextTarget = GetMostHated();
		if( m_nextTarget == NULL )
			HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0 );
	}

	if(m_Unit->GetMapMgr() != NULL && GetNextTarget() != NULL)
	{
		bool fly = true;
		if(m_Unit->IsCreature())
			if(!(TO_CREATURE(m_Unit)->GetProto()->CanMove & LIMIT_AIR))
				fly = false;
				
		if(fly && !IS_INSTANCE(m_Unit->GetMapId()))
		{
			float target_land_z = 0.0f;
			if(m_Unit->GetMapMgr()->IsCollisionEnabled())
			{
				target_land_z = CollideInterface.GetHeight(m_Unit->GetMapId(), GetNextTarget()->GetPositionX(), GetNextTarget()->GetPositionY(), GetNextTarget()->GetPositionZ() + 2.0f);
				if(target_land_z == NO_WMO_HEIGHT)
					target_land_z = m_Unit->GetMapMgr()->GetLandHeight(GetNextTarget()->GetPositionX(), GetNextTarget()->GetPositionY());
			}
			else
			{
				target_land_z = m_Unit->GetMapMgr()->GetLandHeight(GetNextTarget()->GetPositionX(), GetNextTarget()->GetPositionY());
			}

			if(target_land_z)
			{
				if((fabs(GetNextTarget()->GetPositionZ() - target_land_z) > _CalcCombatRange(GetNextTarget(), false)) && fabs(GetNextTarget()->GetPositionZ() - target_land_z) < 100.0f)
				{
					if ( GetNextTarget()->GetTypeId() != TYPEID_PLAYER )
					{
						if ( target_land_z > m_Unit->GetMapMgr()->GetWaterHeight(GetNextTarget()->GetPositionX(), GetNextTarget()->GetPositionY()) )
							HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0);
					}
					else if (TO_PLAYER(GetNextTarget())->GetSession() != NULL)
					{
						MovementInfo* mi = TO_PLAYER(GetNextTarget())->GetMovementInfo();

						if ( mi != NULL && !(mi->flags & MOVEFLAG_REDIRECTED) && !(mi->flags & MOVEFLAG_FALLING) && !(mi->flags & MOVEFLAG_SWIMMING) && !(mi->flags & MOVEFLAG_LEVITATE))
							if(TO_PLAYER(GetNextTarget())->m_FlyingAura || TO_PLAYER(GetNextTarget())->m_setflycheat)
								HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0);
					}
				}
				else if(fabs(GetNextTarget()->GetPositionZ() - target_land_z) > 100.0f) // Cliff or Netherstorm breaks.
				{
					HandleEvent( EVENT_LEAVECOMBAT, m_Unit, 0);
				}
			}
		}
	}

	if( m_nextTarget != NULL && m_nextTarget->isAlive() && m_AIState != STATE_EVADE && !m_Unit->isCasting() )
	{
		if( agent == AGENT_NULL || ( m_AIType == AITYPE_PET && !m_nextSpell ) ) // allow pets autocast
		{
			if(m_canFlee && !m_hasFled && ( m_FleeHealth ? float(m_Unit->GetUInt32Value(UNIT_FIELD_HEALTH) / m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH)) < m_FleeHealth : 1))
				agent = AGENT_FLEE;
			else if(m_canCallForHelp && !m_hasCalledForHelp )
				agent = AGENT_CALLFORHELP;
			else //default to melee if no spells found
			{
				m_nextSpell = getSpell();
				if(m_nextSpell != NULL && m_nextSpell->agent != AGENT_NULL)
					agent = m_nextSpell->agent;
				else
					agent = AGENT_MELEE;
			}
		}
		//check if we can do range attacks
		if(agent == AGENT_RANGED || agent == AGENT_MELEE)
		{
			if(m_canRangedAttack)
			{
				float dist = m_Unit->GetDistanceSq(m_nextTarget);
				if(m_nextTarget->GetTypeId() == TYPEID_PLAYER)
				{
					if( TO_PLAYER( m_nextTarget )->m_currentMovement == MOVE_ROOT || dist >= 32.0f )
						agent = AGENT_RANGED;
				}
				else if( m_nextTarget->m_canMove == false || dist >= 32.0f )
				   agent = AGENT_RANGED;
			}
			else
			{
				agent = AGENT_MELEE;
			}
		}

		if( disable_melee && agent == AGENT_MELEE )
			agent = AGENT_NULL;
		  
		if( disable_ranged && agent == AGENT_RANGED )
			agent = AGENT_NULL;

		if( disable_spell && agent == AGENT_SPELL )
			agent = AGENT_NULL;

		switch(agent)
		{
			case AGENT_MELEE:
			{
				if( m_Unit->GetTypeId() == TYPEID_UNIT )
					TO_CREATURE(m_Unit)->SetSheatheForAttackType( 1 );
				float combatReach[2]; // Calculate Combat Reach
				float distance = m_Unit->CalcDistance(m_nextTarget);

				combatReach[0] = PLAYER_SIZE;
				combatReach[1] = _CalcCombatRange(m_nextTarget, false);

				if(	distance >= combatReach[0] && distance <= combatReach[1] + 0.75f) // Target is (alomst) in Range -> Attack
				{
					if(UnitToFollow != NULL)
					{
						UnitToFollow = NULLUNIT; //we shouldn't be following any one
						m_lastFollowX = m_lastFollowY = 0;
						FollowDistance = 0.0f;
					}
				
					if(m_Unit->isAttackReady(false) && !m_fleeTimer)
					{
						m_creatureState = ATTACKING;
						bool infront = m_Unit->isInFront(m_nextTarget);	

						if(!infront) // set InFront
						{
							//prevent mob from rotating while stunned
							if(!m_Unit->IsStunned ())
							{
								setInFront(m_nextTarget);
								infront = true;
							}							
						}
						if(infront)
						{
							m_Unit->setAttackTimer(0, false);
							//we require to know if strike was succesfull. If there was no dmg then target cannot be dazed by it
							uint32 health_before_strike=m_nextTarget->GetUInt32Value(UNIT_FIELD_HEALTH);
							if(m_nextTarget != NULL)
							{
								m_Unit->Strike( m_nextTarget, MELEE, NULL, 0, 0, 0, false, false, true );
								//now if the target is facing his back to us then we could just cast dazed on him :P
								//as far as i know dazed is casted by most of the creatures but feel free to remove this code if you think otherwise
								if(m_nextTarget != NULL && !(m_Unit->m_factionDBC->RepListId == -1 && m_Unit->m_faction->FriendlyMask==0 && m_Unit->m_faction->HostileMask==0) /* neutral creature */
										&& m_nextTarget->IsPlayer() && !m_Unit->IsPet() && health_before_strike>m_nextTarget->GetUInt32Value(UNIT_FIELD_HEALTH)
										&& Rand(m_Unit->CalculateDazeCastChance(m_nextTarget)))
								{
									float our_facing=m_Unit->calcRadAngle(m_Unit->GetPositionX(),m_Unit->GetPositionY(),m_nextTarget->GetPositionX(),m_nextTarget->GetPositionY());
									float his_facing=m_nextTarget->GetOrientation();
									if(fabs(our_facing-his_facing)<CREATURE_DAZE_TRIGGER_ANGLE && !m_nextTarget->HasNegativeAura(CREATURE_SPELL_TO_DAZE))
									{
										SpellEntry *info = dbcSpell.LookupEntry(CREATURE_SPELL_TO_DAZE);
										Spell* sp = NULLSPELL;
										sp = new Spell(m_Unit, info, false, NULLAURA);
										SpellCastTargets targets;
										targets.m_unitTarget = m_nextTarget->GetGUID();
										sp->prepare(&targets);
									}
								}
							}
						}
					}	
				}
				else // Target out of Range -> Run to it
				{
					//Make sure target can reach us.
					float dist = combatReach[1] - m_Unit->GetFloatValue( UNIT_FIELD_COMBATREACH ); 
					if(dist < PLAYER_SIZE)
						dist = PLAYER_SIZE; //unbelievable how this could happen

					m_moveRun = true;
					_CalcDestinationAndMove(m_nextTarget, dist);
				}
			}break;
		case AGENT_RANGED:
			{
				if( m_Unit->GetTypeId() == TYPEID_UNIT )
					TO_CREATURE(m_Unit)->SetSheatheForAttackType( 3 );

				float combatReach[4]; // Used Shooting Ranges
				float distance = m_Unit->CalcDistance(m_nextTarget);

				combatReach[0] = GetUnit()->GetFloatValue(UNIT_FIELD_COMBATREACH); //normal combat reach
				combatReach[1] = combatReach[0] + 8.0f; //  if distance <  combatReach[1], run towards target, and enter melee.
				combatReach[2] = combatReach[0] + 30.0f; // When shooting and distance > combatReach[2], 
				combatReach[3] = combatReach[0] + 26.0f; //        close in to combatReach[3] again, and continue shooting.

				if(distance >= combatReach[1] && distance <= combatReach[2]) // Target is in Range -> Shoot!!
				{
					if(UnitToFollow != NULL)
					{
						UnitToFollow = NULLUNIT; //we shouldn't be following any one
						m_lastFollowX = m_lastFollowY = 0;
						FollowDistance = 0.0f;
					}
					

					//FIXME: offhand shit
					if(m_Unit->isAttackReady(false) && !m_fleeTimer)
					{
						m_creatureState = ATTACKING;
						bool infront = m_Unit->isInFront(m_nextTarget);	

						if(!infront) // set InFront
						{
							//prevent mob from rotating while stunned
							if(!m_Unit->IsStunned ())
							{
								setInFront(m_nextTarget);
								infront = true;
							}							
						}

						if(infront)
						{
							m_Unit->setAttackTimer(0, false);
							SpellEntry *info = dbcSpell.LookupEntry(m_RangedAttackSpell);
							if(info)
							{
								Spell* sp = NULLSPELL;
								sp = new Spell(m_Unit, info, false, NULLAURA);
								SpellCastTargets targets(m_nextTarget->GetGUID());
								sp->prepare(&targets);

								//Did we give it a sound ID?	
								if(m_SpellSoundid)
									m_Unit->PlaySoundToSet(m_SpellSoundid);
							}
						}
					}
				}
				else // Target out of Range -> Run to/from it, depending on current distance
				{
					m_moveRun = true;
					float drun = (distance < combatReach[1] ? combatReach[0] : combatReach[3] );
					_CalcDestinationAndMove(m_nextTarget, drun);
				}
			}break;
		case AGENT_SPELL:
			{
				if( m_nextSpell == NULL || m_nextTarget == NULL )
					return;  // this shouldnt happen

				DEBUG_LOG("AiAgents","NextSpell %u by NPC %u",m_nextSpell->spell->Id, GetUnit()->GetGUID());

				if( m_Unit->GetTypeId() == TYPEID_UNIT )
					TO_CREATURE(m_Unit)->SetSheatheForAttackType( 0 );

				float distance = m_Unit->CalcDistance(m_nextTarget);
				bool los = true;
				if(m_Unit->GetMapMgr()->IsCollisionEnabled() && m_Unit->GetMapMgr())
				{
					los = CollideInterface.CheckLOS( m_Unit->GetMapId(), m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(),
						m_nextTarget->GetPositionX(), m_nextTarget->GetPositionY(), m_nextTarget->GetPositionZ() );
				}
				if(los && ((distance <= (m_nextSpell->maxrange*m_nextSpell->maxrange)  && distance >= (m_nextSpell->minrange*m_nextSpell->minrange)) || m_nextSpell->maxrange == 0)) // Target is in Range -> Attack
				{
					SpellEntry* spellInfo = m_nextSpell->spell;

					//If this is a healing spell, check health of self and surroundings.
					SpellCastTargets targets;
					if(m_nextSpell->spellType == STYPE_HEAL)
					{
						Unit* starget = FindHealTargetForSpell(m_nextSpell);
						if(starget)
							targets = setSpellTargets(spellInfo, starget);
					}
					else
					{
						targets = setSpellTargets(spellInfo, m_nextTarget);
					}

					//Let's find out on whom to cast the spell on
					uint32 	targettype = m_nextSpell->spelltargetType;
					targets = setSpellTargets(spellInfo, m_nextTarget);
					targettype = m_nextSpell->spelltargetType;
					uint8 ccr = 0;
					switch(targettype)
					{
						case TTYPE_CASTER:
						case TTYPE_SINGLETARGET:
						{
							ccr = CastSpell(m_Unit, spellInfo, targets);
							break;
						}
						case TTYPE_SOURCE:
						{
							ccr = m_Unit->CastSpellAoF(m_Unit->GetPositionX(),m_Unit->GetPositionY(),m_Unit->GetPositionZ(), spellInfo, true);
							break;
						}
						case TTYPE_DESTINATION:
						{
							ccr = m_Unit->CastSpellAoF(m_Unit->GetPositionX(),m_Unit->GetPositionY(),m_Unit->GetPositionZ(), spellInfo, false);
							break;
						}
						case TTYPE_OWNER:
						{
							if(m_Unit->IsPet() &&  m_PetOwner)
								ccr = CastSpell(m_PetOwner, spellInfo, targets);
							break;
						}
					}

					if( ccr == SPELL_CANCAST_OK )
					{
						//Did we give it a sound ID?	
						if( m_nextSpell && m_nextSpell->Misc2 )
							m_Unit->PlaySoundToSet(m_nextSpell->Misc2);

						//add pet spell after use to pet owner with some chance
						if( m_Unit->IsPet() &&  m_PetOwner && m_PetOwner->IsPlayer())
						{	
							Pet* pPet = TO_PET(m_Unit);
							if( pPet && Rand(5)) //5% change to gain spell from pet
								pPet->AddPetSpellToOwner(spellInfo->Id);
						}
						//This spell won't be cast until it cooldown cleared.
						if(m_nextSpell && m_nextSpell->cooldown)
							m_nextSpell->cooldowntime = getMSTime() + m_nextSpell->cooldown;

						//increase procCounter if we're counting
						if(m_nextSpell && m_nextSpell->procCount)
							m_nextSpell->procCounter++;
					}
					else
						DEBUG_LOG("AIAgents","Spell failed: Result %u, NPC %u, spell %u, TargetType %u", ccr, m_Unit->GetEntry() , spellInfo->Id, targettype );
				}
				else // Target out of Range/not in LOS -> Run to it
				{
					//calculate next move
					m_moveRun = true;
					_CalcDestinationAndMove( m_nextTarget, ( m_nextSpell->maxrange < 5.0f ? GetUnit()->GetFloatValue(UNIT_FIELD_COMBATREACH) : m_nextSpell->maxrange - 5.0f ));
				}
				//Null out the agent, so we force the lookup of the next spell.
				m_aiCurrentAgent = AGENT_NULL;
			}break;
		case AGENT_FLEE:
			{
				m_moveRun = false;
				if(m_fleeTimer == 0)
					m_fleeTimer = m_FleeDuration;

				_CalcDestinationAndMove(m_nextTarget, 5.0f);
				if(!m_hasFled)
					CALL_SCRIPT_EVENT(m_Unit, OnFlee)(m_nextTarget);

				m_AIState = STATE_FLEEING;
				SetNextTarget(NULLUNIT);

				WorldPacket data( SMSG_MESSAGECHAT, 100 );
				string msg = "%s attempts to run away in fear!";
				data << (uint8)CHAT_MSG_CHANNEL;
				data << (uint32)LANG_UNIVERSAL;
				data << (uint32)( strlen( TO_CREATURE( m_Unit )->GetCreatureInfo()->Name ) + 1 );
				data << TO_CREATURE( m_Unit )->GetCreatureInfo()->Name;
				data << (uint64)0;
				data << (uint32)(msg.size() + 1);
				data << msg;
				data << uint8(0);

				m_Unit->SendMessageToSet(&data, false);
				m_hasFled = true;
			}break;
		case AGENT_CALLFORHELP:
			{
				FindFriends( 50.0f /*7.0f*/ );
				m_hasCalledForHelp = true; // We only want to call for Help once in a Fight.
				if( m_Unit->GetTypeId() == TYPEID_UNIT )
						objmgr.HandleMonsterSayEvent( TO_CREATURE( m_Unit ), MONSTER_SAY_EVENT_CALL_HELP );
				CALL_SCRIPT_EVENT( m_Unit, OnCallForHelp )();
			}break;
		}
	}
	else if( !m_nextTarget || !m_Unit->isCasting() ||m_nextTarget->GetInstanceID() != m_Unit->GetInstanceID() || !m_nextTarget->isAlive() || !m_nextTarget->IsInWorld() )
	{
		// no more target
		SetNextTarget( NULLUNIT );
	}
}

void AIInterface::DismissPet()
{
	/*
	if(m_AIType != AITYPE_PET)
		return;

	if(!m_PetOwner)
		return;
	
	if(m_PetOwner->GetTypeId() != TYPEID_PLAYER)
		return;

	if(m_Unit->GetUInt32Value(UNIT_CREATED_BY_SPELL) == 0)
		TO_PLAYER( m_PetOwner )->SetFreePetNo(false, (int)m_Unit->GetUInt32Value(UNIT_FIELD_PETNUMBER));
	TO_PLAYER( m_PetOwner )->SetPet(NULL);
	TO_PLAYER( m_PetOwner )->SetPetName("");
	
	//FIXME:Check hunter pet or not
	//FIXME:Check enslaved creature
	m_PetOwner->SetUInt64Value(UNIT_FIELD_SUMMON, 0);
	
	WorldPacket data;
	data.Initialize(SMSG_PET_SPELLS);
	data << (uint64)0;
	TO_PLAYER( m_PetOwner )->GetSession()->SendPacket(&data);
	
	sEventMgr.RemoveEvents(TO_CREATURE(m_Unit));
	if(m_Unit->IsInWorld())
	{
		m_Unit->RemoveFromWorld();
	}
	//setup an event to delete the Creature
	sEventMgr.AddEvent(TO_CREATURE(this->m_Unit), &Creature::DeleteMe, EVENT_DELETE_TIMER, 1, EVENT_FLAG_DELETES_OBJECT);*/
}

void AIInterface::AttackReaction(Unit* pUnit, uint32 damage_dealt, uint32 spellId)
{
	if( m_AIState == STATE_EVADE || m_fleeTimer != 0 || !pUnit || !pUnit->isAlive() || !m_Unit->isAlive() )
	{
		return;
	}

	if( m_Unit == pUnit || m_Unit->IsVehicle() )
	{
		return;
	}

	if(pUnit->IsVehicle())
	{
		uint32 count = TO_VEHICLE(pUnit)->GetPassengerCount();
		if(!count) // No players.
			return;
	}

	if( m_AIState == STATE_IDLE || m_AIState == STATE_FOLLOWING )
	{
		WipeTargetList();
		
		HandleEvent(EVENT_ENTERCOMBAT, pUnit, 0);
	}

	HandleEvent(EVENT_DAMAGETAKEN, pUnit, _CalcThreat(damage_dealt, spellId ? dbcSpell.LookupEntryForced(spellId) : NULL, pUnit));
}

bool AIInterface::HealReaction(Unit* caster, Unit* victim, uint32 amount, SpellEntry * sp)
{
	if(!caster || !victim)
	{
		//printf("!!!BAD POINTER IN AIInterface::HealReaction!!!\n");
		return false;
	}

	// apply spell modifiers
	if (sp != NULL && sp->SpellGroupType)
	{
		SM_FIValue(caster->SM[SMT_THREAT_REDUCED][0],(int32*)&amount,sp->SpellGroupType);
		SM_PIValue(caster->SM[SMT_THREAT_REDUCED][1],(int32*)&amount,sp->SpellGroupType);
	}
	amount += (amount * caster->GetGeneratedThreatModifier() / 100);

	int casterInList = 0, victimInList = 0;

	if(m_aiTargets.find(caster) != m_aiTargets.end())
		casterInList = 1;

	if(m_aiTargets.find(victim) != m_aiTargets.end())
		victimInList = 1;

	/*for(i = m_aiTargets.begin(); i != m_aiTargets.end(); i++)
	{
		if(casterInList && victimInList)
		{ // no need to check the rest, just break that
			break;
		}
		if(i->target == victim)
		{
			victimInList = true;
		}
		if(i->target == caster)
		{
			casterInList = true;
		}
	}*/
	if(!victimInList && !casterInList) // none of the Casters is in the Creatures Threat list
	{
		return false;
	}
	if(!casterInList && victimInList) // caster is not yet in Combat but victim is
	{
		// get caster into combat if he's hostile
		if(isHostile(m_Unit, caster))
		{
			//AI_Target trgt;
			//trgt.target = caster;
			//trgt.threat = amount;
			//m_aiTargets.push_back(trgt);
			if( caster->IsPlayer() )
			{
				if( caster->mThreatRTarget )
					m_aiTargets.insert(TargetMap::value_type(TO_PLAYER(caster->mThreatRTarget), amount));
				else
					m_aiTargets.insert(TargetMap::value_type(caster, amount));
			}
			else
				m_aiTargets.insert(TargetMap::value_type(caster, amount));

			return true;
		}
		return false;
	}
	else if(casterInList && victimInList) // both are in combat already
	{
		// mod threat for caster
		modThreatByPtr(caster, amount);
		return true;
	}
	else // caster is in Combat already but victim is not
	{
		modThreatByPtr(caster, amount);
		// both are players so they might be in the same group
		if( caster->GetTypeId() == TYPEID_PLAYER && victim->GetTypeId() == TYPEID_PLAYER )
		{
			if( TO_PLAYER( caster )->GetGroup() == TO_PLAYER( victim )->GetGroup() )
			{
				// get victim into combat since they are both
				// in the same party
				if( isHostile( m_Unit, victim ) )
				{
					m_aiTargets.insert( TargetMap::value_type( victim, 1 ) );
					return true;
				}
				return false;
			}
		}
	}

	return false;
}

void AIInterface::OnDeath(Object* pKiller)
{
	if(pKiller->GetTypeId() == TYPEID_PLAYER || pKiller->GetTypeId() == TYPEID_UNIT)
		HandleEvent(EVENT_UNITDIED, TO_UNIT(pKiller), 0);
	else
		HandleEvent(EVENT_UNITDIED, m_Unit, 0);
}

void AIInterface::OnRespawn(Unit* unit)
{
	HandleEvent(EVENT_UNITRESPAWN, unit, 0);
}

Unit* AIInterface::FindTarget()
{// find nearest hostile Target to attack
	if( !m_AllowedToEnterCombat ) 
		return NULLUNIT;

	Unit* target = NULLUNIT;
	Unit* critterTarget = NULLUNIT;
	float distance = 999999.0f; // that should do it.. :p
	float crange;
	float z_diff;

	unordered_set<Object* >::iterator itr, it2;
	Object* pObj;
	Unit* pUnit;
	float dist;
	bool pvp=true;
	if(m_Unit->GetTypeId()==TYPEID_UNIT&&TO_CREATURE(m_Unit)->GetCreatureInfo()&&TO_CREATURE(m_Unit)->GetCreatureInfo()->Civilian)
		pvp=false;

	//target is immune to all form of attacks, cant attack either.
	if(m_Unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
	{
		return NULLUNIT;
	}

	for( itr = m_Unit->GetInRangeOppFactsSetBegin(); itr != m_Unit->GetInRangeOppFactsSetEnd(); )
	{
		it2 = itr++;
		pObj = (*it2);

		if( pObj->GetTypeId() == TYPEID_PLAYER )
		{
			if(TO_PLAYER( pObj )->GetTaxiState() )	  // skip players on taxi
				continue;
		}
		else if( pObj->GetTypeId() != TYPEID_UNIT )
				continue;

		pUnit = TO_UNIT(pObj);
		// Don't agro on invincibility
		if( pUnit->bInvincible )
			continue;

		// don't agro players on flying mounts
		/*if(pUnit->GetTypeId() == TYPEID_PLAYER && TO_PLAYER(pUnit)->FlyCheat)
			continue;*/

		//do not agro units that are faking death. Should this be based on chance ?
		if( pUnit->HasFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_FEIGN_DEATH ) )
			continue;

		//target is immune to unit attacks however can be targeted
		//as a part of AI we allow this mode to attack creatures as seen many times on oficial.
		if( m_Unit->HasFlag( UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9 ) )
		{
			if( pUnit->IsPlayer() || pUnit->IsPet() )
			{
				continue;
			}
		}

		/* is it a player? we have to check for our pvp flag. */
//		if(m_U)
		crange = _CalcCombatRange(pUnit,false);
		if(m_isGuard)
			crange *= 4;

		z_diff = fabs(m_Unit->GetPositionZ() - pUnit->GetPositionZ());
		if(z_diff > crange)
		{
			continue;
		}

		if( !m_Unit->PhasedCanInteract( pUnit ) )
			continue;

		if(pUnit->m_invisible) // skip invisible units
			continue;
		
		if(!pUnit->isAlive()
			|| m_Unit == pUnit /* wtf? */
			|| m_Unit->GetUInt64Value(UNIT_FIELD_CREATEDBY) == pUnit->GetGUID())
			continue;

		dist = m_Unit->GetDistanceSq(pUnit);
		if(!pUnit->m_faction || !pUnit->m_factionDBC)
			continue;

		if(pUnit->m_faction->Faction == 28)// only Attack a critter if there is no other Enemy in range
		{
			if(dist < 225.0f)	// was 10
				critterTarget = pUnit;
			continue;
		}

		if(dist > distance)	 // we want to find the CLOSEST target
			continue;
		
		if(dist <= _CalcAggroRange(pUnit) )
		{
            if( m_Unit->IsInLineOfSight(pUnit) )
			{
				distance = dist;
				target = pUnit;
			}
		}
	}

	if( !target )
	{
		target = critterTarget;
	}

	if( target )
	{
/*		if(m_isGuard)
		{
			m_Unit->m_runSpeed = m_Unit->m_base_runSpeed * 2.0f;
			m_fastMove = true;
		}*/

		AttackReaction(target, 1, 0);
		WorldPacket data(SMSG_AI_REACTION, 12);
		data << m_Unit->GetGUID() << uint32(2);		// Aggro sound
		m_Unit->SendMessageToSet(&data, false);

		if(target->GetUInt32Value(UNIT_FIELD_CREATEDBY) != 0)
		{
			Unit* target2 = m_Unit->GetMapMgr()->GetPlayer(target->GetUInt32Value(UNIT_FIELD_CREATEDBY));
			if(target2)
			{
				AttackReaction(target2, 1, 0);
			}
		}
	}
	return target;
}

Unit* AIInterface::FindTargetForSpell(AI_Spell *sp)
{
	/*if(!m_Unit) return NULL;*/

	/*if(!sp)
	{
		m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
		return NULL;
	}*/

	TargetMap::iterator itr, itr2;

	if(sp)
	{
		if(sp->spellType == STYPE_HEAL)
		{
			uint32 cur = m_Unit->GetUInt32Value(UNIT_FIELD_HEALTH) + 1;
			uint32 max = m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH) + 1;
			float healthPercent = float(cur) / float(max);
			if(healthPercent <= sp->floatMisc1) // Heal ourselves cause we got too low HP
			{
				m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
				return m_Unit;
			}
			for(AssistTargetSet::iterator i = m_assistTargets.begin(); i != m_assistTargets.end(); i++)
			{
				if(!(*i)->isAlive())
				{
					continue;
				}
				cur = (*i)->GetUInt32Value(UNIT_FIELD_HEALTH);
				max = (*i)->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
				healthPercent = float(cur) / float(max);
				if(healthPercent <= sp->floatMisc1) // Heal ourselves cause we got too low HP
				{
					m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, (*i)->GetGUID());
					return (*i); // heal Assist Target which has low HP
				}
			}
		}
		if(sp->spellType == STYPE_BUFF)
		{
			m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
			return m_Unit;
		}
	}

	return GetMostHated();
}

Unit* AIInterface::FindHealTargetForSpell(AI_Spell *sp)
{
	TargetMap::iterator itr, itr2;

	if(sp)
	{
		ASSERT(sp->spellType == STYPE_HEAL);

		uint32 cur = m_Unit->GetUInt32Value(UNIT_FIELD_HEALTH) + 1;
		uint32 max = m_Unit->GetUInt32Value(UNIT_FIELD_MAXHEALTH) + 1;
		float healthPercent = float(cur/max);
		if(sp->floatMisc1)
		{	// Heal ourselves cause we got too low HP
			if(healthPercent <= sp->floatMisc1 && !m_Unit->HasActiveAura(sp->spell->Id, m_Unit->GetGUID()))
			{
				sp->spelltargetType = TTYPE_CASTER;
				m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
				return m_Unit;
			}
		} // If we don't have a preset, then we can just use half health.
		else if(healthPercent <= 50 && !m_Unit->HasActiveAura(sp->spell->Id, m_Unit->GetGUID()))
		{
			sp->spelltargetType = TTYPE_CASTER;
			if(!sp->procCount) // Limit our self heals so we don't cast every time we go below 50.
				sp->procCount = 3;

			m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, 0);
			return m_Unit;
		}

		for(AssistTargetSet::iterator i = m_assistTargets.begin(); i != m_assistTargets.end(); i++)
		{
			if(!(*i)->isAlive())
				continue;

			cur = (*i)->GetUInt32Value(UNIT_FIELD_HEALTH);
			max = (*i)->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
			healthPercent = max ? float(cur/max) : 0;
			if(sp->floatMisc1)
			{
				if(healthPercent <= sp->floatMisc1 && !(*i)->HasActiveAura(sp->spell->Id,m_Unit->GetGUID()))
				{
					sp->spelltargetType = TTYPE_SINGLETARGET;
					m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, (*i)->GetGUID());
					return (*i); // heal Assist Target which has low HP
				}
			} // If we don't have a preset, then we can just use half health.
			else if(healthPercent <= 50 && !(*i)->HasActiveAura(sp->spell->Id,m_Unit->GetGUID()))
			{
				sp->spelltargetType = TTYPE_SINGLETARGET;
				if(!sp->procCount) // Limit our heals so we don't cast every time someone goes below 50.
				{
					if(sp->spell->RankNumber)
						sp->procCount = 40/sp->spell->RankNumber;
					else
						sp->procCount = 10;
				}

				m_Unit->SetUInt64Value(UNIT_FIELD_TARGET, (*i)->GetGUID());
				return (*i); // heal Assist Target which has low HP
			}
		}
	}

	return NULLUNIT;
}

bool AIInterface::FindFriends(float dist)
{
	if( m_Unit->IsPet() ) //pet's do not have friends; Players are exploiting this :-/
		return false;

	bool result = false;
	TargetMap::iterator it;

	unordered_set<Object* >::iterator itr;
	Unit* pUnit;

	for(itr = m_Unit->GetInRangeSetBegin(); itr != m_Unit->GetInRangeSetEnd(); itr++)
	{
		if((*itr) == NULL || !(*itr)->IsInWorld() || (*itr)->GetTypeId() != TYPEID_UNIT)
			continue;

		pUnit = TO_UNIT((*itr));
		if(!pUnit->isAlive())
			continue;

		if(pUnit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
		{
			continue;
		}
		if(pUnit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_9))
		{
			continue;
		}

		if( !isHostile(GetMostHated(), pUnit) )
		{
			continue;
		}

		if( !m_Unit->PhasedCanInteract(pUnit) )
			continue;

		if( isCombatSupport( m_Unit, pUnit ) && ( pUnit->GetAIInterface()->getAIState() == STATE_IDLE || pUnit->GetAIInterface()->getAIState() == STATE_SCRIPTIDLE ) )//Not sure
		{
			if( m_Unit->GetDistanceSq(pUnit) < dist)
			{
				if( m_assistTargets.count( pUnit ) > 0 ) // already have him
					break;

				result = true;
				m_assistTargets.insert(pUnit);
					
				for(it = m_aiTargets.begin(); it != m_aiTargets.end(); it++)
				{
					TO_UNIT( *itr )->GetAIInterface()->AttackReaction( it->first, 1, 0 );
				}
			}
		}
	}

	// check if we're a civillan, in which case summon guards on a despawn timer
	CreatureInfo * ci = TO_CREATURE(m_Unit)->GetCreatureInfo();
	if( ci && ci->Type == HUMANOID && ci->Civilian )
		CallGuards();	

	return result;
}

float AIInterface::_CalcAggroRange(Unit* target)
{
	const static float baseAR[17] = {19.0f, 18.5f, 18.0f, 17.5f, 17.0f, 16.5f, 16.0f, 15.5f, 15.0f, 14.5f, 12.0f, 10.5f, 8.5f,  7.5f,  6.5f,  6.5f, 5.0f};
	int8 lvlDiff = target->getLevel() - m_Unit->getLevel();
	uint8 realLvlDiff = lvlDiff;
	if(lvlDiff > 8)
	{
		lvlDiff = 8;
	}
	if(lvlDiff < -8)
	{
		lvlDiff = -8;
	}
	if(!TO_CREATURE(m_Unit)->CanSee(target))
		return 0;
	
	float AggroRange = baseAR[lvlDiff + 8];
	if(realLvlDiff > 8)
	{
		AggroRange += AggroRange * ((lvlDiff - 8) * 5 / 100);
	}

	// Multiply by elite value
	if(TO_CREATURE(m_Unit)->GetCreatureInfo() && TO_CREATURE(m_Unit)->GetCreatureInfo()->Rank > 0)
		AggroRange *= (TO_CREATURE(m_Unit)->GetCreatureInfo()->Rank) * 1.50f;

	if(AggroRange > 40.0f) // cap at 40.0f
	{
		AggroRange = 40.0f;
	}
  /*  //printf("aggro range: %f , stealthlvl: %d , detectlvl: %d\n",AggroRange,target->GetStealthLevel(),m_Unit->m_stealthDetectBonus);
	if(! TO_CREATURE(m_Unit)->CanSee(target))
	{
		AggroRange =0;
	//	AggroRange *= ( 100.0f - (target->m_stealthLevel - m_Unit->m_stealthDetectBonus)* 20.0f ) / 100.0f;
	}
*/
	// SPELL_AURA_MOD_DETECT_RANGE
	int32 modDetectRange = target->getDetectRangeMod(m_Unit->GetGUID());
	AggroRange += modDetectRange;
	if(target->IsPlayer())
		AggroRange += TO_PLAYER( target )->DetectedRange;
	if(AggroRange < 3.0f)
	{
		AggroRange = 3.0f;
	}
	if(AggroRange > 40.0f) // cap at 40.0f
	{
		AggroRange = 40.0f;
	}

	return (AggroRange*AggroRange);
}

void AIInterface::_CalcDestinationAndMove(Unit* target, float dist)
{
	if(!m_canMove || m_Unit->IsStunned())
	{
		StopMovement(0); //Just Stop
		return;
	}
	
	if(target && (target->GetTypeId() == TYPEID_UNIT || target->GetTypeId() == TYPEID_PLAYER))
	{
		float ResX = target->GetPositionX();
		float ResY = target->GetPositionY();
		float ResZ = target->GetPositionZ();

		float angle = m_Unit->calcAngle(m_Unit->GetPositionX(), m_Unit->GetPositionY(), ResX, ResY) * float(M_PI) / 180.0f;
		float x = dist * cosf(angle);
		float y = dist * sinf(angle);

		if(target->GetTypeId() == TYPEID_PLAYER && TO_PLAYER( target )->m_isMoving )
		{
			// cater for moving player vector based on orientation
			x -= cosf(target->GetOrientation());
			y -= sinf(target->GetOrientation());
		}

		m_nextPosX = ResX - x;
		m_nextPosY = ResY - y;
		m_nextPosZ = ResZ;
	}
	else
	{
		target = NULLUNIT;
		m_nextPosX = m_Unit->GetPositionX();
		m_nextPosY = m_Unit->GetPositionY();
		m_nextPosZ = m_Unit->GetPositionZ();
	}

	float dx = m_nextPosX - m_Unit->GetPositionX();
	float dy = m_nextPosY - m_Unit->GetPositionY();
	if(dy != 0.0f)
	{
		float angle = atan2(dx, dy);
		m_Unit->SetOrientation(angle);
	}

	if(m_creatureState != MOVING)
		UpdateMove();
}

float AIInterface::_CalcCombatRange(Unit* target, bool ranged)
{
	if(target == NULL)
		return 0.0f;

	float range = 0.0f;
	float rang = PLAYER_SIZE;
	if(ranged)
		rang = 5.0f;

	float selfreach = m_Unit->GetFloatValue(UNIT_FIELD_COMBATREACH);
	float targetradius = target->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS);
	float selfradius = m_Unit->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS);
	float targetscale = GetScale( dbcCreatureDisplayInfo.LookupEntry( target->GetUInt32Value(UNIT_FIELD_DISPLAYID)));
	float selfscale =  GetScale( dbcCreatureDisplayInfo.LookupEntry( m_Unit->GetUInt32Value(UNIT_FIELD_DISPLAYID)));

	range = ((((targetradius*targetradius)*targetscale) + selfreach) + ((selfradius*selfscale) + rang));
	if(range > 28.29f) range = 28.29f;
	return range < PLAYER_SIZE ? PLAYER_SIZE :range ;
}

float AIInterface::_CalcDistanceFromHome()
{
	if (m_AIType == AITYPE_PET)
	{
		return m_Unit->GetDistanceSq(m_PetOwner);
	}
	else if(m_Unit->GetTypeId() == TYPEID_UNIT)
	{

		if(m_returnX !=0.0f && m_returnY != 0.0f)
		{
			return m_Unit->GetDistanceSq(m_returnX,m_returnY,m_returnZ);
		}
	}

	return 0.0f;
}

/************************************************************************************************************
SendMoveToPacket:
Comments: Some comments on the SMSG_MONSTER_MOVE packet: 
	the uint8 field:
		0: Default															known
		1: Don't move														known
		2: there is an extra 3 floats, also known as a vector				unknown
		3: there is an extra uint64 most likely a guid.						unknown
		4: there is an extra float that causes the orientation to be set.	known
		
		note:	when this field is 1. 
			there is no need to send  the next 3 uint32's as they are'nt used by the client
	
	the MoveFlags:
	0x00000000 - Walk
	0x00001000 - Run
	0x00003000 - Fly
		some comments on that 0x00000300 - Fly = 0x00001000 | 0x00002000

	waypoints:
		TODO.... as they somehow seemed to be changed long time ago..
		
*************************************************************************************************************/

void AIInterface::SendMoveToPacket(float toX, float toY, float toZ, float toO, uint32 time, uint32 MoveFlags)
{
	//this should NEVER be called directly !!!!!!
	//use MoveTo()
	//WorldPacket data(SMSG_MONSTER_MOVE, 60);
	uint8 buffer[100];
	StackPacket data(SMSG_MONSTER_MOVE, buffer, 100);
	data << m_Unit->GetNewGUID();
	data << uint8(0);
	data << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
	data << getMSTime();
	
	// Check if we have an orientation
	if(toO != 0.0f)
	{
		data << uint8(4);
		data << toO;
	}
	else
	{
		data << uint8(0);
	}

	data << MoveFlags;
	data << time;
	if(MoveFlags & MOVEFLAG_FLYING)
	{
		data << float(float(time)/1000);
		data << uint32(0);
	}
	else if(MoveFlags & MOVEFLAG_SWIMMING)
	{
		data << uint8(0);
		data << uint32(0);
	}

	data << uint32(1);	// 1 waypoint
	data << toX << toY << toZ;

#ifndef ENABLE_COMPRESSED_MOVEMENT_FOR_CREATURES
	bool self = m_Unit->GetTypeId() == TYPEID_PLAYER;
	m_Unit->SendMessageToSet( &data, self );
#else
	if( m_Unit->GetTypeId() == TYPEID_PLAYER )
		TO_PLAYER(m_Unit)->GetSession()->SendPacket(&data);

	for(unordered_set<Player*  >::iterator itr = m_Unit->GetInRangePlayerSetBegin(); itr != m_Unit->GetInRangePlayerSetEnd(); itr++)
	{
		if( (*itr)->GetPositionNC().Distance2DSq( m_Unit->GetPosition() ) >= World::m_movementCompressThresholdCreatures )
			(*itr)->AppendMovementData( SMSG_MONSTER_MOVE, uint32(data.GetSize()), (const uint8*)data.GetBufferPointer() );
		else
			(*itr)->GetSession()->SendPacket(&data);
	}
#endif
}

void AIInterface::StopMovement(uint32 time)
{
	m_moveTimer = time; //set pause after stopping
	m_creatureState = STOPPED;

	if(m_destinationY && m_destinationX)
	{
		float orient = atan2(m_destinationY - m_Unit->GetPositionY(), m_destinationX - m_Unit->GetPositionX());
		m_Unit->SetOrientation(orient);
	}

	m_destinationX = m_destinationY = m_destinationZ = 0;
	m_nextPosX = m_nextPosY = m_nextPosZ = 0;
	m_timeMoved = 0;
	m_timeToMove = 0;

	SendMoveToPacket(m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(), m_Unit->GetOrientation(), 0, getMoveFlags());
}

void AIInterface::MoveTo(float x, float y, float z)
{
	m_sourceX = m_Unit->GetPositionX();
	m_sourceY = m_Unit->GetPositionY();
	m_sourceZ = m_Unit->GetPositionZ();

	if(!m_canMove || m_Unit->IsStunned()|| m_Unit->isCasting())
	{
		StopMovement(0); //Just Stop
		return;
	}

	m_nextPosX = x;
	m_nextPosY = y;
	m_nextPosZ = z;
	CheckHeight();

	if(m_creatureState != MOVING)
		UpdateMove();
}

bool AIInterface::IsFlying()
{
	if(m_moveFly)
		return true;

	if( m_Unit->GetTypeId() == TYPEID_PLAYER )
		return TO_PLAYER( m_Unit )->FlyCheat;

	return false;
}

uint32 AIInterface::getMoveFlags()
{
	uint32 MoveFlags = MONSTER_MOVE_FLAG_RUN;
	if(m_moveFly == true) //Fly
	{
		m_flySpeed = m_Unit->m_flySpeed*0.001f;
		MoveFlags = MONSTER_MOVE_FLAG_FLY;
	}
	else if(m_moveSprint == true) //Sprint
	{
		m_runSpeed = (m_Unit->m_runSpeed+5.0f)*0.001f;
	}
	else if(m_moveRun == true) //Run
	{
		m_runSpeed = m_Unit->m_runSpeed*0.001f;
	}
	m_walkSpeed = m_Unit->m_walkSpeed*0.001f;//move distance per ms time 
	return MoveFlags;
}

void AIInterface::UpdateMove()
{
	//this should NEVER be called directly !!!!!!
	//use MoveTo()
	float distance = m_Unit->CalcDistance(m_nextPosX,m_nextPosY,m_nextPosZ);

	m_destinationX = m_nextPosX;
	m_destinationY = m_nextPosY;
	m_destinationZ = m_nextPosZ;
	m_nextPosX = m_nextPosY = m_nextPosZ = 0;

	uint32 moveTime = 0;
	if(m_moveFly)
		moveTime = (uint32)(distance/m_flySpeed);
	else if(m_moveRun)
		moveTime = (uint32)(distance/m_runSpeed);
	else
		moveTime = (uint32)(distance/m_walkSpeed);

	m_totalMoveTime = moveTime;

	if(m_Unit->GetTypeId() == TYPEID_UNIT)
	{
		Creature* creature = TO_CREATURE(m_Unit);

		float angle = 0.0f;
		float c_reach =GetUnit()->GetFloatValue(UNIT_FIELD_COMBATREACH);

		//We don't want little movements here and there; 
		float DISTANCE_TO_SMALL_TO_WALK = c_reach - 1.0f <= 0.0f ? 1.0f : c_reach - 1.0f;

		// don't move if we're well within combat range; rooted can't move neither
		if( distance < DISTANCE_TO_SMALL_TO_WALK || (creature->proto && creature->proto->CanMove == LIMIT_ROOT ) )
			return; 

		// check if we're returning to our respawn location. if so, reset back to default
		// orientation
		if(creature->GetSpawnX() == m_destinationX && creature->GetSpawnY() == m_destinationY)
		{
			angle = creature->GetSpawnO();
			creature->SetOrientation(angle);
		}
		else
		{
			// Calculate the angle to our next position
			float dx = (float)m_destinationX - m_Unit->GetPositionX();
			float dy = (float)m_destinationY - m_Unit->GetPositionY();
			if(dy != 0.0f)
			{
				angle = atan2(dy, dx);
				m_Unit->SetOrientation(angle);
			}
		}
	}
	SendMoveToPacket(m_destinationX, m_destinationY, m_destinationZ, m_Unit->GetOrientation(), moveTime + UNIT_MOVEMENT_INTERPOLATE_INTERVAL, getMoveFlags());

	m_timeToMove = moveTime;
	m_timeMoved = 0;
	if(m_moveTimer == 0)
		m_moveTimer =  UNIT_MOVEMENT_INTERPOLATE_INTERVAL; // update every few msecs

	m_creatureState = MOVING;
}

void AIInterface::SendCurrentMove(Player* plyr/*uint64 guid*/)
{
	if(m_destinationX == 0.0f && m_destinationY == 0.0f && m_destinationZ == 0.0f) return; //invalid move 

	uint32 numpoints = 4;
	float pointDiffX = m_sourceX - m_destinationX / (float)numpoints;
	float pointDiffY = m_sourceY - m_destinationY / (float)numpoints;
	float pointDiffZ = m_sourceZ - m_destinationZ / (float)numpoints;

	if(pointDiffX < 1.0f || pointDiffY < 1.0f)
	{
		SendMoveToPacket(m_destinationX, m_destinationY, m_destinationZ, 0.0f, 0, getMoveFlags());
		return;
	}

	ByteBuffer *splineBuf = new ByteBuffer(34*4);
	uint32 flags = 0;
	*splineBuf << uint32(flags); // spline flags
	/*
	else if(flags & 0x8000)
		; // 3 unk floats
	if(flags & 0x10000)
		; // unknown int64
	if(flags & 0x20000)
		; // unknown float*/
	*splineBuf << uint32(0); //dont know if this is an uint32 but client seems to be trying to read 4 - 24 + 8 + 4
	*splineBuf << uint32((m_totalMoveTime - m_timeToMove)+m_moveTimer); //Time Passed (start Position) //should be generated/save 
	*splineBuf << uint32(m_totalMoveTime); //Total Time //should be generated/save
	*splineBuf << m_sourceX << m_sourceY << m_sourceZ;
	*splineBuf << uint32(0);

	*splineBuf << uint32(numpoints); //Spline Count	// lets try this

	for(uint8 i = 0 ; i < numpoints; i++)
	{
		*splineBuf << m_sourceX + (pointDiffX * i);
		*splineBuf << m_sourceY + (pointDiffY * i);
		*splineBuf << m_sourceZ + (pointDiffZ * i);
	}
	/*
	*splineBuf << m_sourceX << m_sourceY << m_sourceZ;
	*splineBuf << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
	*splineBuf << m_destinationX << m_destinationY << m_destinationZ;
	*splineBuf << m_destinationX << m_destinationY << m_destinationZ;
	*/
	*splineBuf << uint8(0); // Pguid?
	
	*splineBuf << m_destinationX << m_destinationY << m_destinationZ;	// 3 floats after all the spline points

	plyr->AddSplinePacket(m_Unit->GetGUID(), splineBuf);

	//This should only be called by Players AddInRangeObject() ONLY
	//using guid cuz when i atempted to use pointer the player was deleted when this event was called some times
	//Player* plyr = World::GetPlayer(guid);
	//if(!plyr) return;

	/*if(m_destinationX == 0.0f && m_destinationY == 0.0f && m_destinationZ == 0.0f) return; //invalid move 
	uint32 moveTime = m_timeToMove-m_timeMoved;
	//uint32 moveTime = (m_timeToMove-m_timeMoved)+m_moveTimer;
	WorldPacket data(50);
	data.SetOpcode( SMSG_MONSTER_MOVE );
	data << m_Unit->GetNewGUID();
	data << m_Unit->GetPositionX() << m_Unit->GetPositionY() << m_Unit->GetPositionZ();
	data << getMSTime();
	data << uint8(0);
	data << getMoveFlags();

	//float distance = m_Unit->CalcDistance(m_destinationX, m_destinationY, m_destinationZ);
	//uint32 moveTime = (uint32) (distance / m_runSpeed);

	data << moveTime;
	data << uint32(1); //Number of Waypoints
	data << m_destinationX << m_destinationY << m_destinationZ;
	plyr->GetSession()->SendPacket(&data);*/

}

bool AIInterface::setInFront(Unit* target) // not the best way to do it, though
{
	//angle the object has to face
	float angle = m_Unit->calcAngle(m_Unit->GetPositionX(), m_Unit->GetPositionY(), target->GetPositionX(), target->GetPositionY() ); 
	//Change angle slowly 2000ms to turn 180 deg around
	if(angle > 180) angle += 90;
	else angle -= 90; //angle < 180
	m_Unit->getEasyAngle(angle);
	//Convert from degrees to radians (180 deg = PI rad)
	float orientation = angle / float(180 / M_PI);
	//Update Orentation Server Side
	m_Unit->SetPosition(m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(), orientation);
	
	return m_Unit->isInFront(target);
}

bool AIInterface::addWayPoint(WayPoint* wp)
{
	if(m_waypoints == NULL)
		m_waypoints = new WayPointMap ;

	if(!wp) 
		return false;
	if(wp->id <= 0)
		return false; //not valid id

	if(m_waypoints->size() < wp->id)
		m_waypoints->resize(wp->id);

	if((*m_waypoints)[wp->id-1] == NULL)
	{
		(*m_waypoints)[wp->id-1] = wp;
		return true;
	}
	return false;
}

void AIInterface::changeWayPointID(uint32 oldwpid, uint32 newwpid)
{
	if(!m_waypoints)return;
	if(newwpid <= 0) 
		return; //not valid id
	if(newwpid > m_waypoints->size()) 
		return; //not valid id
	if(oldwpid > m_waypoints->size())
		return;

	if(newwpid == oldwpid) 
		return; //same spot

	//already wp with that id ?
	WayPoint* originalwp = getWayPoint(newwpid);
	if(!originalwp) 
		return;
	WayPoint* oldwp = getWayPoint(oldwpid);
	if(!oldwp) 
		return;

	oldwp->id = newwpid;
	originalwp->id = oldwpid;
	(*m_waypoints)[oldwp->id-1] = oldwp;
	(*m_waypoints)[originalwp->id-1] = originalwp;

	//SaveAll to db
	saveWayPoints();
}

void AIInterface::deleteWayPoint(uint32 wpid)
{
	if(!m_waypoints || wpid <= 0 || wpid > m_waypoints->size()) 
		return; //not valid id

	uint32 newpid = 1;
	WayPointMap new_waypoints;
	WayPointMap::iterator itr,it2;
	for(itr = m_waypoints->begin(); itr != m_waypoints->end();)
	{
		it2 = itr++;
		if((*it2) == NULL || (*it2)->id == wpid)
		{
			delete (*it2);
			continue;
		}
		new_waypoints.push_back(*it2);
	}

	m_waypoints->clear();
	for(WayPointMap::iterator itr = new_waypoints.begin(); itr != new_waypoints.end(); itr++)
	{
		(*itr)->id = newpid++;
		m_waypoints->push_back(*itr);
	}

	saveWayPoints();
}

bool AIInterface::showWayPoints(Player* pPlayer, bool Backwards)
{
	if(!m_waypoints)
		return false;

	//wpid of 0 == all
	WayPointMap::const_iterator itr;
	if(m_WayPointsShowing == true) 
		return false;

	m_WayPointsShowing = true;

	WayPoint* wp = NULL;
	for (itr = m_waypoints->begin(); itr != m_waypoints->end(); itr++)
	{
		if( (*itr) != NULL )
		{
			wp = *itr;

			//Create
			Creature* pWayPoint = NULLCREATURE;
			pWayPoint = new Creature((uint64)HIGHGUID_TYPE_WAYPOINT << 32 | wp->id);
			pWayPoint->Init();
			pWayPoint->CreateWayPoint(wp->id,pPlayer->GetMapId(),wp->x,wp->y,wp->z,0);
			pWayPoint->SetUInt32Value(OBJECT_FIELD_ENTRY, 300000);
			pWayPoint->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.5f);
			if(Backwards)
			{
				uint32 DisplayID = (wp->backwardskinid == 0)? GetUnit()->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID) : wp->backwardskinid;
				pWayPoint->SetUInt32Value(UNIT_FIELD_DISPLAYID, DisplayID);
				pWayPoint->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->backwardemoteid);
				pWayPoint->SetStandState(wp->backwardStandState);
			}
			else
			{
				uint32 DisplayID = (wp->forwardskinid == 0)? GetUnit()->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID) : wp->forwardskinid;
				pWayPoint->SetUInt32Value(UNIT_FIELD_DISPLAYID, DisplayID);
				pWayPoint->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->forwardemoteid);
				pWayPoint->SetStandState(wp->forwardStandState);
			}
			pWayPoint->SetUInt32Value(UNIT_FIELD_LEVEL, wp->id);
			pWayPoint->SetUInt32Value(UNIT_NPC_FLAGS, 0);
//			pWayPoint->SetUInt32Value(UNIT_FIELD_AURA+32, 8326); //invisable & deathworld look
			pWayPoint->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE , pPlayer->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE));
			pWayPoint->SetUInt32Value(UNIT_FIELD_HEALTH, 1);
			pWayPoint->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 1);
			pWayPoint->SetUInt32Value(UNIT_FIELD_STAT0, wp->flags);

			//Create on client
			ByteBuffer buf(2500);
			uint32 count = pWayPoint->BuildCreateUpdateBlockForPlayer(&buf, pPlayer);
			pPlayer->PushCreationData(&buf, count);

			//root the object
			WorldPacket data1;
			data1.Initialize(SMSG_FORCE_MOVE_ROOT);
			data1 << pWayPoint->GetNewGUID();
			pPlayer->GetSession()->SendPacket( &data1 );

			//Cleanup
			delete pWayPoint;
			pWayPoint = NULL;
		}
	}
	return true;
}

bool AIInterface::hideWayPoints(Player* pPlayer)
{
	if(!m_waypoints)
		return false;

	//wpid of 0 == all
	if(m_WayPointsShowing != true) return false;
	m_WayPointsShowing = false;
	WayPointMap::const_iterator itr;

	// slightly better way to do this
	uint64 guid;

	for (itr = m_waypoints->begin(); itr != m_waypoints->end(); itr++)
	{
		if( (*itr) != NULL )
		{
			// avoid C4293
			guid = ((uint64)HIGHGUID_TYPE_WAYPOINT << 32) | (*itr)->id;
			WoWGuid wowguid(guid);
			pPlayer->PushOutOfRange(wowguid);
		}
	}
	return true;
}

bool AIInterface::saveWayPoints()
{
	if(!m_waypoints)return false;

	if(!GetUnit()) return false;
	if(GetUnit()->GetTypeId() != TYPEID_UNIT ||
		TO_CREATURE(GetUnit())->m_spawn == NULL ) return false;

	WorldDatabase.Execute("DELETE FROM creature_waypoints WHERE spawnid = %u", TO_CREATURE(GetUnit())->GetSQL_id());
	WayPointMap::const_iterator itr;
	WayPoint* wp = NULL;
	std::stringstream ss;

	for (itr = m_waypoints->begin(); itr != m_waypoints->end();)
	{
		wp = (*itr);
		++itr;
		//Save
		ss.str("");
		ss << "REPLACE INTO creature_waypoints ";
		ss << "(spawnid,waypointid,position_x,position_y,position_z,orientation,waittime,flags,forwardemoteoneshot,forwardemoteid,backwardemoteoneshot,backwardemoteid,forwardskinid,backwardskinid,forwardStandState,backwardStandState,forwardSpellToCast,backwardSpellToCast,forwardSayText,backwardSayText) VALUES (";
		ss << TO_CREATURE(GetUnit())->GetSQL_id() << ", ";
		ss << wp->id << ", ";
		ss << wp->x << ", ";
		ss << wp->y << ", ";
		ss << wp->z << ", ";
		ss << wp->o << ", ";
		ss << wp->waittime << ", ";
		ss << wp->flags << ", ";
		ss << wp->forwardemoteoneshot << ", ";
		ss << wp->forwardemoteid << ", ";
		ss << wp->backwardemoteoneshot << ", ";
		ss << wp->backwardemoteid << ", ";
		ss << wp->forwardskinid << ", ";
		ss << wp->backwardskinid << ", ";
		ss << wp->forwardStandState << ", ";
		ss << wp->backwardStandState << ", ";
		ss << wp->forwardSpellToCast << ", ";
		ss << wp->backwardSpellToCast << ",'";
		ss << WorldDatabase.EscapeString(wp->forwardSayText) << "','";
		ss << WorldDatabase.EscapeString(wp->backwardSayText) << "')\0";
		WorldDatabase.Query( ss.str().c_str() );
	}
	return true;
}

void AIInterface::deleteWaypoints()
{
	if(!m_waypoints)
		return;

	for(WayPointMap::iterator itr = m_waypoints->begin(); itr != m_waypoints->end(); itr++)
	{
		if((*itr) != NULL)
			delete (*itr);
	}
	m_waypoints->clear();
}

WayPoint* AIInterface::getWayPoint(uint32 wpid)
{
	if(!m_waypoints)
		return NULL;

	//make sure wpid is valid, return 1st/last if out of boundaries.
	wpid = uint32( wpid > m_waypoints->size() ? m_waypoints->size() : ( !wpid?  1 : wpid ));

	return m_waypoints->at(wpid-1);
}

void AIInterface::_UpdateMovement(uint32 p_time)
{
	if(!m_Unit->isAlive())
	{
		StopMovement(0);
		return;
	}

	uint32 timediff = 0;

	if(m_moveTimer > 0)
	{
		if(p_time >= m_moveTimer)
		{
			timediff = p_time - m_moveTimer;
			m_moveTimer = 0;
		}
		else
			m_moveTimer -= p_time;
	}

	if(m_timeToMove > 0)
	{
		m_timeMoved = m_timeToMove <= p_time + m_timeMoved ? m_timeToMove : p_time + m_timeMoved;
	}

	if(m_creatureState == MOVING)
	{
		if(!m_moveTimer)
		{
			if(m_timeMoved == m_timeToMove) //reached destination
			{
				if(m_moveType == MOVEMENTTYPE_WANTEDWP)//We reached wanted wp stop now
					m_moveType = MOVEMENTTYPE_DONTMOVEWP;

				float wayO = 0.0f;

				if((GetWayPointsCount() != 0) && (m_AIState == STATE_IDLE || m_AIState == STATE_SCRIPTMOVE)) //if we attacking don't use wps
				{
					WayPoint* wp = getWayPoint(getCurrentWaypoint());
					if(wp)
					{
						CALL_SCRIPT_EVENT(m_Unit, OnReachWP)(wp->id, !m_moveBackward);
						if(TO_CREATURE(m_Unit)->has_waypoint_text)
							objmgr.HandleMonsterSayEvent(TO_CREATURE(m_Unit), MONSTER_SAY_EVENT_RANDOM_WAYPOINT);

						//Lets face to correct orientation
						wayO = wp->o;
						m_moveTimer = wp->waittime; //wait before next move
						if(!m_moveBackward)
						{
							if(wp->forwardemoteoneshot)
							{
								GetUnit()->Emote(EmoteType(wp->forwardemoteid));
							}
							else
							{
								if(GetUnit()->GetUInt32Value(UNIT_NPC_EMOTESTATE) != wp->forwardemoteid)
								{
									GetUnit()->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->forwardemoteid);
								}
							}
							if(GetUnit()->getStandState() != wp->forwardStandState )
								GetUnit()->SetStandState(wp->forwardStandState);
							if (wp->forwardSpellToCast)
								GetUnit()->CastSpell(GetUnit(),wp->forwardSpellToCast,false);
							if (wp->forwardSayText != "")
								GetUnit()->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, wp->forwardSayText.c_str() );
						}
						else
						{
							if(wp->backwardemoteoneshot)
							{
								GetUnit()->Emote(EmoteType(wp->backwardemoteid));
							}
							else
							{
								if(GetUnit()->GetUInt32Value(UNIT_NPC_EMOTESTATE) != wp->backwardemoteid)
								{
									GetUnit()->SetUInt32Value(UNIT_NPC_EMOTESTATE, wp->backwardemoteid);
								}
							}
							if(GetUnit()->getStandState() != wp->backwardStandState )
								GetUnit()->SetStandState(wp->backwardStandState);
							if (wp->backwardSpellToCast)
								GetUnit()->CastSpell(GetUnit(),wp->backwardSpellToCast,false);
							if (wp->backwardSayText != "")
								GetUnit()->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, wp->backwardSayText.c_str() );
						}
					}
					else
						m_moveTimer = RandomUInt(m_moveRun ? 5000 : 10000); // wait before next move
				}

				m_creatureState = STOPPED;
				m_moveSprint = false;

				if(m_MovementType == MOVEMENTTYPE_DONTMOVEWP)
					m_Unit->SetPosition(m_destinationX, m_destinationY, m_destinationZ, wayO, true);
				else
					m_Unit->SetPosition(m_destinationX, m_destinationY, m_destinationZ, m_Unit->GetOrientation(), true);

				m_destinationX = m_destinationY = m_destinationZ = 0;
				m_timeMoved = 0;
				m_timeToMove = 0;
			}
			else
			{
				//Move Server Side Update
				float q = (float)m_timeMoved / (float)m_timeToMove;
				float x = m_Unit->GetPositionX() + (m_destinationX - m_Unit->GetPositionX()) * q;
				float y = m_Unit->GetPositionY() + (m_destinationY - m_Unit->GetPositionY()) * q;
				float z = m_Unit->GetPositionZ() + (m_destinationZ - m_Unit->GetPositionZ()) * q;

				m_Unit->SetPosition(x, y, z, m_Unit->GetOrientation());
				
				m_timeToMove -= m_timeMoved;
				m_timeMoved = 0;
				m_moveTimer = (UNIT_MOVEMENT_INTERPOLATE_INTERVAL < m_timeToMove) ? UNIT_MOVEMENT_INTERPOLATE_INTERVAL : m_timeToMove;
			}
			//**** Movement related stuff that should be done after a move update (Keeps Client and Server Synced) ****//
			//**** Process the Pending Move ****//
			if(m_nextPosX != 0.0f && m_nextPosY != 0.0f)
			{
				UpdateMove();
			}
		}
	}
	else if(m_creatureState == STOPPED && (m_AIState == STATE_IDLE || m_AIState == STATE_SCRIPTMOVE) && !m_moveTimer && !m_timeToMove && UnitToFollow == NULL) //creature is stopped and out of Combat
	{
		if(sWorld.getAllowMovement() == false) //is creature movement enabled?
			return;

		if(m_Unit->GetUInt32Value(UNIT_FIELD_DISPLAYID) == 5233) //if Spirit Healer don't move
			return;

		// do we have a formation?
		if(m_formationLinkSqlId != 0)
		{
			if( m_formationLinkTarget == NULL)
			{
				// haven't found our target yet
				Creature* c = TO_CREATURE(m_Unit);
				if(!c->haslinkupevent)
				{
					// register linkup event
					c->haslinkupevent = true;
					sEventMgr.AddEvent(c, &Creature::FormationLinkUp, m_formationLinkSqlId, 
						EVENT_CREATURE_FORMATION_LINKUP, 1000, 0,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
				}
			}
			else
			{
				// we've got a formation target, set unittofollow to this
				UnitToFollow = m_formationLinkTarget;
				FollowDistance = m_formationFollowDistance;
				m_fallowAngle = m_formationFollowAngle;
			}
		}
		if(UnitToFollow == NULL)
		{
			// no formation, use waypoints
			int destpoint = -1;

			// If creature has no waypoints just wander aimlessly around spawnpoint
			if(GetWayPointsCount()==0) //no waypoints
			{
				/*	if(m_moveRandom)
				{
				if((rand()%10)==0)																																	
				{																																								  
				float wanderDistance = rand()%4 + 2;
				float wanderX = ((wanderDistance*rand()) / RAND_MAX) - wanderDistance / 2;																											   
				float wanderY = ((wanderDistance*rand()) / RAND_MAX) - wanderDistance / 2;																											   
				float wanderZ = 0; // FIX ME ( i dont know how to get apropriate Z coord, maybe use client height map data)																											 

				if(m_Unit->CalcDistance(m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(), TO_CREATURE(m_Unit)->respawn_cord[0], TO_CREATURE(m_Unit)->respawn_cord[1], TO_CREATURE(m_Unit)->respawn_cord[2])>15)																		   
				{   
				//return home																																				 
				MoveTo(TO_CREATURE(m_Unit)->respawn_cord[0],TO_CREATURE(m_Unit)->respawn_cord[1],TO_CREATURE(m_Unit)->respawn_cord[2],false);
				}   
				else
				{
				MoveTo(m_Unit->GetPositionX() + wanderX, m_Unit->GetPositionY() + wanderY, m_Unit->GetPositionZ() + wanderZ,false);
				}	
				}	
				}
				*/	
				return;																																				   
			}																																						  
			else //we do have waypoints
			{
				uint32 wpcount = uint32(GetWayPointsCount());
				if(m_moveType == MOVEMENTTYPE_RANDOMWP) //is random move on if so move to a random waypoint
				{
					if(wpcount > 1)
						destpoint = RandomUInt(wpcount-1)+1;
				}
				else if (m_moveType == MOVEMENTTYPE_CIRCLEWP) //random move is not on lets follow the path in circles
				{
					// 1 -> 10 then 1 -> 10
					m_currentWaypoint++;
					if (m_currentWaypoint > wpcount) 
						m_currentWaypoint = 1; //Happens when you delete last wp seems to continue ticking
					destpoint = m_currentWaypoint;
					m_moveBackward = false;
				}
				else if(m_moveType == MOVEMENTTYPE_WANTEDWP)//Move to wanted wp
				{
					if(m_currentWaypoint)
					{
						if( wpcount > 0)
						{
							destpoint = m_currentWaypoint;
						}
						else
							destpoint = -1;
					}
				}
				else if(m_moveType == MOVEMENTTYPE_FORWARDTHANSTOP)// move to end, then stop
				{
					++m_currentWaypoint;
					if(m_currentWaypoint > wpcount)
					{
						//hmm maybe we should stop being path walker since we are waiting here anyway
						destpoint = -1;
					}
					else
						destpoint = m_currentWaypoint;
				}
				else if(m_moveType != MOVEMENTTYPE_QUEST && m_moveType != MOVEMENTTYPE_DONTMOVEWP)//4 Unused
				{
					// 1 -> 10 then 10 -> 1
					if (m_currentWaypoint > wpcount)
						m_currentWaypoint = 1; //Happens when you delete last wp seems to continue ticking
					if (m_currentWaypoint == wpcount) // Are we on the last waypoint? if so walk back
						m_moveBackward = true;
					if (m_currentWaypoint == 1) // Are we on the first waypoint? if so lets goto the second waypoint
						m_moveBackward = false;
					if (!m_moveBackward) // going 1..n
						destpoint = ++m_currentWaypoint;
					else				// going n..1
						destpoint = --m_currentWaypoint;
				}

				if(destpoint != -1)
				{
					WayPoint* wp = getWayPoint(destpoint);
					if(wp)
					{
						if(!m_moveBackward)
						{
							if((wp->forwardskinid != 0) && (GetUnit()->GetUInt32Value(UNIT_FIELD_DISPLAYID) != wp->forwardskinid))
							{
								GetUnit()->SetUInt32Value(UNIT_FIELD_DISPLAYID, wp->forwardskinid);
							}
						}
						else
						{
							if((wp->backwardskinid != 0) && (GetUnit()->GetUInt32Value(UNIT_FIELD_DISPLAYID) != wp->backwardskinid))
							{
								GetUnit()->SetUInt32Value(UNIT_FIELD_DISPLAYID, wp->backwardskinid);
							}
						}
						m_moveFly = (wp->flags == 768) ? true : false;
						m_moveRun = (wp->flags == 256) ? 1 : 0;
						MoveTo(wp->x, wp->y, wp->z);
					}
				}
			}
		}
	}

	//Fear Code
	if(m_AIState == STATE_FEAR && UnitToFear != NULL && m_creatureState == STOPPED)
	{
		if(getMSTime() > m_FearTimer)   // Wait at point for x ms ;)
		{
			float Fx;
			float Fy;
			float Fz;
			// Calculate new angle to target.
			float Fo = m_Unit->calcRadAngle(UnitToFear->GetPositionX(), UnitToFear->GetPositionY(), m_Unit->GetPositionX(), m_Unit->GetPositionY());
			double fAngleAdd = RandomDouble(((M_PI/2) * 2)) - (M_PI/2);
			Fo += (float)fAngleAdd;
			
			float dist = m_Unit->CalcDistance(UnitToFear);
			if(dist > 30.0f || (Rand(25) && dist > 10.0f))	// not too far or too close
			{
				Fx = m_Unit->GetPositionX() - (RandomFloat(15.f)+5.0f)*cosf(Fo);
				Fy = m_Unit->GetPositionY() - (RandomFloat(15.f)+5.0f)*sinf(Fo);
			}
			else
			{
				Fx = m_Unit->GetPositionX() + (RandomFloat(20.f)+5.0f)*cosf(Fo);
				Fy = m_Unit->GetPositionY() + (RandomFloat(20.f)+5.0f)*sinf(Fo);
			}
			// Check if this point is in water.
			float wl = m_Unit->GetMapMgr()->GetWaterHeight(Fx, Fy);
//			uint8 wt = m_Unit->GetMapMgr()->GetWaterType(Fx, Fy);

			if (m_Unit->GetMapMgr() && m_Unit->GetMapMgr()->IsCollisionEnabled())
			{
				if( !CollideInterface.GetFirstPoint( m_Unit->GetMapId(), m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(), 
					Fx, Fy, m_Unit->GetPositionZ() + 1.5f, Fx, Fy, Fz, -3.5f ) )
				{
					// clear path?
					Fz = CollideInterface.GetHeight( m_Unit->GetMapId(), Fx, Fy, m_Unit->GetPositionZ() );
					if( Fz == NO_WMO_HEIGHT )
						Fz = m_Unit->GetMapMgr()->GetLandHeight(Fx, Fy);
				}
				else
				{
					// obstruction in the way.
					// the distmod will fuck up the Z, so get a new height.
					float fz2 = CollideInterface.GetHeight(m_Unit->GetMapId(), Fx, Fy, Fz);
					if( fz2 != NO_WMO_HEIGHT )
						Fz = fz2;
				}

				if( fabs( m_Unit->GetPositionZ() - Fz ) > 3.5f || ( wl != 0.0f && Fz < wl ) )		// in water
				{
					m_FearTimer=getMSTime() + 500;
				}
				else
				{
					MoveTo(Fx, Fy, Fz);
					m_FearTimer = m_totalMoveTime + getMSTime() + 400;
				}
			}
			else
			{
				Fz = m_Unit->GetMapMgr()->GetLandHeight(Fx, Fy);
				if(fabs(m_Unit->GetPositionZ()-Fz) > 4 || (Fz != 0.0f && Fz < (wl-2.0f)))
					m_FearTimer=getMSTime()+100;
				else
				{
					MoveTo(Fx, Fy, Fz);
					m_FearTimer = m_totalMoveTime + getMSTime() + 200;
				}
			}
		}
	}

	// Wander AI movement code
	if(m_AIState == STATE_WANDER && m_creatureState == STOPPED)
	{
		if(getMSTime() < m_WanderTimer) // is it time to move again?
			return;

		// calculate a random distance and angle to move
		float wanderD = RandomFloat(2.0f) + 2.0f;
		float wanderO = RandomFloat(6.283f);
		float Fx = m_Unit->GetPositionX() + wanderD * cosf(wanderO);
		float Fy = m_Unit->GetPositionY() + wanderD * sinf(wanderO);
		float Fz;

		if (m_Unit->GetMapMgr() && m_Unit->GetMapMgr()->IsCollisionEnabled())
		{
			if( !CollideInterface.GetFirstPoint( m_Unit->GetMapId(), m_Unit->GetPositionX(), m_Unit->GetPositionY(), m_Unit->GetPositionZ(), 
				Fx, Fy, m_Unit->GetPositionZ() + 1.5f, Fx, Fy, Fz, -3.5f ) )
			{
				// clear path?
				Fz = CollideInterface.GetHeight( m_Unit->GetMapId(), Fx, Fy, m_Unit->GetPositionZ() );
				if( Fz == NO_WMO_HEIGHT )
					Fz = m_Unit->GetMapMgr()->GetLandHeight(Fx, Fy);
			}
			else
			{
				// obstruction in the way.
				// the distmod will fuck up the Z, so get a new height.
				float fz2 = CollideInterface.GetHeight(m_Unit->GetMapId(), Fx, Fy, Fz);
				if( fz2 != NO_WMO_HEIGHT )
					Fz = fz2;
			}

			if( fabs( m_Unit->GetPositionZ() - Fz ) > 3.5f )
			{
				m_WanderTimer=getMSTime() + 1000;
			}
			else
			{
				m_Unit->SetOrientation(wanderO);
				MoveTo(Fx, Fy, Fz);
				m_WanderTimer = getMSTime() + m_totalMoveTime + 300; // time till next move (+ pause)
			}
		}
		else
		{
			Fz = m_Unit->GetMapMgr()->GetLandHeight(Fx, Fy);

			// without these next checks we could fall through the "ground" (WMO) and get stuck
			// wander won't work correctly in cities until we get some way to fix this and remove these checks
			float currentZ = m_Unit->GetPositionZ();
			float landZ = m_Unit->GetMapMgr()->GetLandHeight(m_Unit->GetPositionX(), m_Unit->GetPositionY());

			if( currentZ > landZ + 1.0f // are we more than 1yd above ground? (possible WMO)
			 || Fz < currentZ - 5.0f // is our destination land height too low? (possible WMO)
			 || Fz > currentZ + wanderD) // is our destination too high to climb?
			{
				m_WanderTimer = getMSTime() + 1000; // wait 1 second before we try again
				return;
			}

			m_Unit->SetOrientation(wanderO);
			MoveTo(Fx, Fy, Fz);
			m_WanderTimer = getMSTime() + m_totalMoveTime + 300; // time till next move (+ pause)
		}
	}

	//Unit Follow Code
	if(UnitToFollow != NULL)
	{
		if( UnitToFollow->event_GetCurrentInstanceId() != m_Unit->event_GetCurrentInstanceId() || !UnitToFollow->IsInWorld() )
			UnitToFollow = NULLUNIT;
		else
		{
			if(m_AIState == STATE_IDLE || m_AIState == STATE_FOLLOWING)
			{
				float dist = m_Unit->GetDistanceSq(UnitToFollow);

				// re-calculate orientation based on target's movement
				if(m_lastFollowX != UnitToFollow->GetPositionX() ||
					m_lastFollowY != UnitToFollow->GetPositionY())
				{
					float dx = UnitToFollow->GetPositionX() - m_Unit->GetPositionX();
					float dy = UnitToFollow->GetPositionY() - m_Unit->GetPositionY();
					if(dy != 0.0f)
					{
						float angle = atan2(dx,dy);
						m_Unit->SetOrientation(angle);
					}
					m_lastFollowX = UnitToFollow->GetPositionX();
					m_lastFollowY = UnitToFollow->GetPositionY();
				}

				if (dist > (FollowDistance*FollowDistance)) //if out of range
				{
					m_AIState = STATE_FOLLOWING;
						
					if(dist > 25.0f) //25 yard away lets run else we will loose the them
						m_moveRun = true;
					else 
						m_moveRun = false;

					if(m_AIType == AITYPE_PET || UnitToFollow == m_formationLinkTarget) //Unit is Pet/formation
					{
						if(dist > 900.0f/*30*/)
							m_moveSprint = true;

						float delta_x = UnitToFollow->GetPositionX();
						float delta_y = UnitToFollow->GetPositionY();
						float d = 3;
						if(m_formationLinkTarget != NULL)
							d = m_formationFollowDistance;

						MoveTo(delta_x+(d*(cosf(m_fallowAngle+UnitToFollow->GetOrientation()))),
							delta_y+(d*(sinf(m_fallowAngle+UnitToFollow->GetOrientation()))),
							UnitToFollow->GetPositionZ());				
					}
					else
					{
						_CalcDestinationAndMove(UnitToFollow, FollowDistance);
					}
				}
			}
		}
	}

	// This is fly checks, mostly because of object updates.
	CheckHeight();
}

uint8 AIInterface::CastSpell(Unit* caster, SpellEntry *spellInfo, SpellCastTargets targets)
{
	if( m_AIType != AITYPE_PET && disable_spell )
		return SPELL_FAILED_ERROR;

	//only stop for spells with casting time
	uint32 delay = GetCastTime( dbcSpellCastTime.LookupEntry(spellInfo->CastingTimeIndex));
	if( delay )
		StopMovement( delay + 500);

	// Stop AI while casting.
	m_AIState = STATE_CASTING;

	Spell* nspell = NULLSPELL;
	nspell = new Spell(caster, spellInfo, false, NULLAURA);
	return nspell->prepare(&targets);
}

void AIInterface::CancelSpellCast()
{
	m_nextSpell = NULL;
	if( m_Unit && m_Unit->m_currentSpell)
		m_Unit->m_currentSpell->cancel();
}

SpellEntry *AIInterface::getSpellEntry(uint32 spellId)
{
	SpellEntry *spellInfo = dbcSpell.LookupEntry(spellId );

	if(!spellInfo)
	{
		OUT_DEBUG("WORLD: unknown spell id %i\n", spellId);
		return NULL;
	}

	return spellInfo;
}

SpellCastTargets AIInterface::setSpellTargets(SpellEntry *spellInfo, Unit* target)
{
	SpellCastTargets targets;
	targets.m_unitTarget = target ? target->GetGUID() : 0;
	targets.m_itemTarget = 0;
	targets.m_srcX = 0;
	targets.m_srcY = 0;
	targets.m_srcZ = 0;
	targets.m_destX = 0;
	targets.m_destY = 0;
	targets.m_destZ = 0;

	if(m_nextSpell->spelltargetType == TTYPE_SINGLETARGET)
	{
		targets.m_targetMask = TARGET_FLAG_UNIT;
		targets.m_unitTarget = target->GetGUID();
	}
	else if(m_nextSpell->spelltargetType == TTYPE_SOURCE)
	{
		targets.m_targetMask = TARGET_FLAG_SOURCE_LOCATION;
		targets.m_srcX = m_Unit->GetPositionX();
		targets.m_srcY = m_Unit->GetPositionY();
		targets.m_srcZ = m_Unit->GetPositionZ();
	}
	else if(m_nextSpell->spelltargetType == TTYPE_DESTINATION)
	{
		targets.m_targetMask = TARGET_FLAG_DEST_LOCATION;
		targets.m_destX = target->GetPositionX();
		targets.m_destY = target->GetPositionY();
		targets.m_destZ = target->GetPositionZ();
	}
	else if(m_nextSpell->spelltargetType == TTYPE_CASTER)
	{
		targets.m_targetMask = TARGET_FLAG_UNIT;
		targets.m_unitTarget = m_Unit->GetGUID();
	}

	return targets;
}

AI_Spell *AIInterface::getSpell()
{
	// look at our spells
	AI_Spell *  sp = NULL;
	AI_Spell *  def_spell = NULL;
	uint32 cool_time = 0;
	uint32 nowtime = getMSTime();

	if( nowtime > next_spell_time )
	{
		if(m_Unit->IsPet())
		{
			Pet* pPet = TO_PET(m_Unit);
			def_spell = pPet->HandleAutoCastEvent();
		}
		else
		{
			//start searching the list for a suitable spell. 
			for(list<AI_Spell*>::iterator itr = m_spells.begin(); itr != m_spells.end(); itr++)
			{
				sp = (*itr);

				//don't waste our time if we already have spell in priority
				if (def_spell && def_spell->spellType > sp->spellType)
					continue;

				// Wtf?? There should be only spells on the list
				if(sp->agent != AGENT_SPELL)
				{
					//Log.Warning("AI_Agent","Agent entry %u is loaded, but not valid",sp->agent); 
					continue;
				}

				// skip when max proccount reached.	
				if((sp->procCount && sp->procCounter >= sp->procCount ))
					continue;

				//skip when still cooling down.
				if(sp->cooldown && sp->cooldowntime > nowtime )
					continue;

				//skip if proc change not met.
				if(sp->procChance < 100 && !Rand(sp->procChance))
					continue;

				//checks by spell type
				switch (sp->spellType)
				{
				case STYPE_ROOT:
					{
// #define AI_ROOT_OVERRIDING
#ifdef AI_ROOT_OVERRIDING
						if(sp->spell->c_is_flags & SPELL_FLAG_CASTED_ON_ENEMIES)
						{
							if(sp->spelltargetType == TTYPE_CASTER || sp->spelltargetType == TTYPE_OWNER)
							{
								sp->spelltargetType = TTYPE_SINGLETARGET;
								WorldDatabase.Execute("UPDATE ai_agents SET spelltargettype = \"RANDOMTARGET\" WHERE spell = %u;", sp->spell->Id);
								continue;
							}
						}
						else if(sp->spell->c_is_flags & SPELL_FLAG_IS_HEALING || sp->spell->c_is_flags & SPELL_FLAG_IS_HEALING_SPELL)
						{
							sp->spellType = STYPE_HEAL;
							WorldDatabase.Execute("UPDATE ai_agents SET spelltype = \"HEAL\" WHERE spell = %u;", sp->spell->Id);
							continue;
						}
						else if(sp->spell->c_is_flags & SPELL_FLAG_IS_DAMAGING)
						{
							if(sp->spell->base_range_or_radius)
								sp->spellType = STYPE_AOEDAMAGE;
							else
								sp->spellType = STYPE_DAMAGE;

							WorldDatabase.Execute("UPDATE ai_agents SET spelltype = \"%s\" WHERE spell = %u;", (sp->spellType == STYPE_AOEDAMAGE ? "AOEDAMAGE" : "DAMAGE"), sp->spell->Id);
							continue;
						}
#endif // AI_ROOT_OVERRIDING
					}break;
				case STYPE_DEBUFF:
					{
						if (!m_nextTarget || m_nextTarget->HasActiveAura(sp->spell->Id))
							continue;
					}break;
				case STYPE_BUFF:
					{
						if (m_Unit->HasActiveAura(sp->spell->Id))
							continue;
					}break;
				case STYPE_HEAL:
					{
						if (!FindHealTargetForSpell(sp))
							continue;
					}break;
				case STYPE_INTERRUPT:
					{
						if (!m_nextTarget || !m_nextTarget->isCasting())
							continue;
					}break;
				}
				//success
				def_spell = sp;
			}
		}

		//Lets update cooldown and return a spell.
		next_spell_time = nowtime + 500 + RandomUInt(1000);
		return def_spell;
	}

	return NULL;
}

void AIInterface::addSpellToList(AI_Spell *sp)
{
	if(sp->spell == NULL)
		return;

	if(sp->procCount || sp->cooldown)
	{
		AI_Spell * sp2 = NULLSPELL;
		sp2 = new AI_Spell;
		memcpy(sp2, sp, sizeof(AI_Spell));
		sp2->procCounter=0;
		sp2->cooldowntime = getMSTime() + sp->cooldown;
		sp2->custom_pointer = true;
		m_spells.push_back(sp2);
	}
	else
		m_spells.push_back(sp);

	m_Unit->m_SpellList.insert(sp->spell->Id); // add to list
}

uint32 AIInterface::getThreatByGUID(uint64 guid)
{
	Unit* obj = m_Unit->GetMapMgr()->GetUnit(guid);
	if(obj)
		return getThreatByPtr(obj);

	return 0;
}

uint32 AIInterface::getThreatByPtr(Unit* obj)
{
	TargetMap::iterator it = m_aiTargets.find(obj);
	if(it != m_aiTargets.end())
	{
		return it->second;
	}
	return 0;
}

//should return a valid target
Unit* AIInterface::GetMostHated()
{
	Unit* ResultUnit=NULLUNIT;

	//override mosthated with taunted target. Basic combat checks are made for it. 
	//What happens if we can't see tauntedby unit ?
	ResultUnit = getTauntedBy();
	if(ResultUnit)
		return ResultUnit;

	pair< Unit* , int32> currentTarget;
	currentTarget.first = NULLUNIT;
	currentTarget.second = -1;

	TargetMap::iterator it2 = m_aiTargets.begin();
	TargetMap::iterator itr;
	for(; it2 != m_aiTargets.end();)
	{
		itr = it2;
		++it2;

		/* check the target is valid */
		if(itr->first->event_GetCurrentInstanceId() != m_Unit->event_GetCurrentInstanceId() || !itr->first->isAlive() || !isAttackable(m_Unit, itr->first))
		{
			m_aiTargets.erase(itr);
			continue;
		}

		if((itr->second + itr->first->GetThreatModifier()) > currentTarget.second)
		{
			/* new target */
			currentTarget.first = itr->first;
			currentTarget.second = itr->second + itr->first->GetThreatModifier();
			m_currentHighestThreat = currentTarget.second;
		}

		/* there are no more checks needed here... the needed checks are done by CheckTarget() */
	}

	return currentTarget.first;
}
Unit* AIInterface::GetSecondHated()
{
	Unit* ResultUnit=GetMostHated();

	pair< Unit*, int32> currentTarget;
	currentTarget.first = NULLUNIT;
	currentTarget.second = -1;

	TargetMap::iterator it2 = m_aiTargets.begin();
	TargetMap::iterator itr;
	for(; it2 != m_aiTargets.end();)
	{
		itr = it2;
		++it2;

		/* check the target is valid */
		if(itr->first->GetInstanceID() != m_Unit->GetInstanceID() || !itr->first->isAlive() || !isAttackable(m_Unit, itr->first))
		{
			m_aiTargets.erase(itr);
			continue;
		}

		if((itr->second + itr->first->GetThreatModifier()) > currentTarget.second &&
			itr->first != ResultUnit)
		{
			/* new target */
			currentTarget.first = itr->first;
			currentTarget.second = itr->second + itr->first->GetThreatModifier();
			m_currentHighestThreat = currentTarget.second;
		}
	}

	return currentTarget.first;
}
bool AIInterface::modThreatByGUID(uint64 guid, int32 mod)
{
	if (!m_aiTargets.size())
		return false;

	Unit* obj = m_Unit->GetMapMgr()->GetUnit(guid);
	if(obj)
		return modThreatByPtr(obj, mod);

	return false;
}

bool AIInterface::modThreatByPtr(Unit* obj, int32 mod)
{
	if(obj == NULL)
		return false;

	if( obj->mThreatRTarget && mod > 0)
	{
		int32 partmod = float2int32(mod * obj->mThreatRAmount);
		mod -= partmod;
		Unit* robj = obj->mThreatRTarget;
		if( partmod && robj && robj->isAlive() && obj->GetDistanceSq(robj) < 1600 )
		{
			TargetMap::iterator it = m_aiTargets.find(robj);
			if(it != m_aiTargets.end())
			{
				it->second += partmod;
				if((it->second + robj->GetThreatModifier()) > m_currentHighestThreat)
				{
					// new target!
					if(!isTaunted)
					{
						m_currentHighestThreat = it->second + robj->GetThreatModifier();
						SetNextTarget(robj);
					}
				}
			}
			else
			{
				m_aiTargets.insert( make_pair( robj, partmod ) );
				if((partmod + robj->GetThreatModifier()) > m_currentHighestThreat)
				{
					if(!isTaunted)
					{
						m_currentHighestThreat = partmod + robj->GetThreatModifier();
						SetNextTarget(robj);
					}
				}
			}
		}
	}

	TargetMap::iterator it = m_aiTargets.find(obj);
	if( it != m_aiTargets.end() )
	{
		it->second += mod;
		if( (it->second + obj->GetThreatModifier()) > m_currentHighestThreat )
		{
			// new target!
			if( !isTaunted )
			{
				m_currentHighestThreat = it->second + obj->GetThreatModifier();
				SetNextTarget(obj);
			}
		}
	}
	else
	{
		m_aiTargets.insert( make_pair( obj, mod ) );
		if( (mod + obj->GetThreatModifier()) > m_currentHighestThreat )
		{
			if( !isTaunted )
			{
				m_currentHighestThreat = mod + obj->GetThreatModifier();
				SetNextTarget(obj);
			}
		}
	}

	if(obj == m_nextTarget)
	{
		// check for a possible decrease in threat.
		if(mod < 0)
		{
			m_nextTarget = GetMostHated();
			//if there is no more new targets then we can walk back home ?
			if(!m_nextTarget)
				HandleEvent(EVENT_LEAVECOMBAT, m_Unit, 0);
			SetNextTarget(m_nextTarget);
		}
	}
	return true;
}

void AIInterface::RemoveThreatByPtr(Unit* obj)
{
	if(!obj)
		return;

	TargetMap::iterator it = m_aiTargets.find(obj);
	if(it != m_aiTargets.end())
	{
		m_aiTargets.erase(it);
		//check if we are in combat and need a new target
		if(obj == m_nextTarget)
		{
			m_nextTarget = GetMostHated();
			//if there is no more new targets then we can walk back home ?
			if( !m_nextTarget )
				HandleEvent(EVENT_LEAVECOMBAT, m_Unit, 0);
			else
				SetNextTarget(m_nextTarget);
		}
	}
}

void AIInterface::addAssistTargets(Unit* Friend)
{
	// stop adding stuff that gives errors on linux!
	m_assistTargets.insert(Friend);
}

void AIInterface::WipeHateList()
{
	for(TargetMap::iterator itr = m_aiTargets.begin(); itr != m_aiTargets.end(); itr++)
		itr->second = 0;
	m_currentHighestThreat = 0;
}
void AIInterface::ClearHateList() //without leaving combat
{
	for(TargetMap::iterator itr = m_aiTargets.begin(); itr != m_aiTargets.end(); itr++)
		itr->second = 1;
	m_currentHighestThreat = 1;
}

void AIInterface::WipeTargetList()
{
	SetNextTarget(NULLUNIT);

	m_nextSpell = NULL;
	m_currentHighestThreat = 0;
	m_aiTargets.clear();
	m_Unit->CombatStatus.Vanished();
}

bool AIInterface::taunt(Unit* caster, bool apply)
{
	if(apply)
	{
		//wowwiki says that we cannot owerride this spell
		if(GetIsTaunted())
			return false;

		if(!caster)
		{
			isTaunted = false;
			return false;
		}

		//check if we can attack taunter. Maybe it's a hack or a bug if we fail this test
		if(isHostile(m_Unit, caster))
		{
			//check if we have to add him to our agro list
			//GetMostHated(); //update our most hated list/ Note that at this point we do not have a taunter yet. If we would have then this funtion will not give real mosthated
			int32 oldthreat = getThreatByPtr(caster);
			//make sure we rush the target anyway. Since we are not tauted yet, this will also set our target
			modThreatByPtr(caster,abs(m_currentHighestThreat-oldthreat)+1); //we need to be the most hated at this moment
//			SetNextTarget(caster);
		}
		isTaunted = true;
		tauntedBy = caster;
	}
	else
	{
		isTaunted = false;
		tauntedBy = NULLUNIT;
		//taunt is over, we should get a new target based on most hated list
		SetNextTarget(GetMostHated());
	}

	return true;
}

Unit* AIInterface::getTauntedBy()
{
	if(GetIsTaunted())
	{
		return tauntedBy;
	}
	else
	{
		return NULLUNIT;
	}
}

bool AIInterface::GetIsTaunted()
{
	if(isTaunted)
	{
		if(!tauntedBy || !tauntedBy->isAlive())
		{
			isTaunted = false;
			tauntedBy = NULLUNIT;
		}
	}
	return isTaunted;
}

void AIInterface::CheckTarget(Unit* target)
{
	if( target == NULL )
		return;

	if( target == UnitToFollow )		  // fix for crash here
	{
		UnitToFollow = NULLUNIT;
		m_lastFollowX = m_lastFollowY = 0;
		FollowDistance = 0;
	}

	if( target == UnitToFollow_backup )
	{
		UnitToFollow_backup = NULLUNIT;
	}

	AssistTargetSet::iterator  itr = m_assistTargets.find(target);
	if(itr != m_assistTargets.end())
		m_assistTargets.erase(itr);

	TargetMap::iterator it2 = m_aiTargets.find( target );
	if( it2 != m_aiTargets.end() || target == m_nextTarget )
	{
		if(it2 != m_aiTargets.end())
		{
			m_aiTargets.erase(it2);
		}

		if (target == m_nextTarget)	 // no need to cast on these.. mem addresses are still the same
		{
			SetNextTarget(NULLUNIT);
			m_nextSpell = NULL;

			// find the one with the next highest threat
			GetMostHated();
		}
	}

	if( target->GetTypeId() == TYPEID_UNIT )
	{
		it2 = target->GetAIInterface()->m_aiTargets.find( m_Unit );
		if( it2 != target->GetAIInterface()->m_aiTargets.end() )
			target->GetAIInterface()->m_aiTargets.erase( it2 );
        
		if( target->GetAIInterface()->m_nextTarget == m_Unit )
		{
			target->GetAIInterface()->m_nextTarget = NULLUNIT;
			target->GetAIInterface()->m_nextSpell = NULL;
			target->GetAIInterface()->GetMostHated();
		}

		if( target->GetAIInterface()->UnitToFollow == m_Unit )
			target->GetAIInterface()->UnitToFollow = NULLUNIT;
	}

	if(target == UnitToFear)
		UnitToFear = NULLUNIT;

	if(tauntedBy == target)
		tauntedBy = NULLUNIT;
}

uint32 AIInterface::_CalcThreat(uint32 damage, SpellEntry * sp, Unit* Attacker)
{
	if (isSameFaction(m_Unit,Attacker))
		return 0;

	int32 mod = 0;
	if( sp != NULL && sp->ThreatForSpell != 0 )
	{
		mod = sp->ThreatForSpell;
	}
	else
	{
		mod = damage;
	}

	if (sp != NULL && sp->SpellGroupType && Attacker)
	{
		SM_FIValue(Attacker->SM[SMT_THREAT_REDUCED][0],&mod,sp->SpellGroupType);
		SM_PIValue(Attacker->SM[SMT_THREAT_REDUCED][1],&mod,sp->SpellGroupType);
	}

	// modify mod by Affects
	mod += (mod * Attacker->GetGeneratedThreatModifier() / 100);

	return mod;
}

void AIInterface::WipeReferences()
{
	m_nextSpell = 0;
	m_currentHighestThreat = 0;
	m_aiTargets.clear();
	m_nextTarget = NULLUNIT;
	UnitToFear = NULLUNIT;
	UnitToFollow = NULLUNIT;
	tauntedBy = NULLUNIT;
}

void AIInterface::ResetProcCounts()
{
	for(list<AI_Spell*>::iterator itr = m_spells.begin(); itr != m_spells.end(); itr++)
		if((*itr)->procCount)
			(*itr)->procCounter=0;
}

//we only cast once a spell and we will set his health and resistances. Note that this can be made with db too !
void AIInterface::Event_Summon_Elemental(uint32 summon_duration, uint32 TotemEntry, int32 ResistanceType, uint8 Slot)
{
	//Event_Summon_EE_totem(GetDuration(), 15352, NATURE_DAMAGE);
	//Event_Summon_FE_totem(GetDuration(), 15438, FIRE_DAMAGE);

	if(m_Unit==NULL || Slot > 6)
		return;

	m_totemspelltimer = 0xEFFFFFFF;
	Creature * ourslave= NULLCREATURE;
	
	ourslave = TO_CREATURE(m_Unit->CreateTemporaryGuardian(TotemEntry,summon_duration,float(-M_PI*2), 0,Slot));
	if(ourslave != NULL)
	{
		//Just use slot 0, there shouldn't be any other spawns
		if(m_Unit->m_SummonSlots[Slot] != NULL)
			m_Unit->SummonExpireSlot(Slot);  // a previous summon?

		ourslave->ResistanceModPct[ResistanceType]=100;//we should be imune to nature dmg. This can be also set in db
		ourslave->SetUInt32Value(UNIT_FIELD_LEVEL, m_Unit->GetUInt32Value(UNIT_FIELD_LEVEL));
		ourslave->BaseDamage[0]=250;
		ourslave->BaseDamage[1]=450;
		ourslave->CalcDamage();
		ourslave->SetSummonOwnerSlot(m_Unit->GetGUID(),Slot);

		m_Unit->m_SummonSlots[Slot] = ourslave;

		/*
			Spells can be added to AI_Agents for every summoned elemental ;)
		*/
	}
}

void AIInterface::CallGuards()
{
	if( m_Unit->isDead() || !m_Unit->isAlive() || m_Unit->GetInRangePlayersCount() == 0 || m_Unit->GetMapMgr() == NULL )
		return;

	if(  getMSTime() > m_guardTimer && !IS_INSTANCE(m_Unit->GetMapId()))
	{
		m_guardTimer = getMSTime() + 15000;
		uint16 AreaId = m_Unit->GetMapMgr()->GetAreaID(m_Unit->GetPositionX(),m_Unit->GetPositionY(),m_Unit->GetPositionZ());
		AreaTable * at = dbcArea.LookupEntry(AreaId);
		if(!at)
			return;

		ZoneGuardEntry * zoneSpawn = ZoneGuardStorage.LookupEntry(at->ZoneId);
		if(!zoneSpawn) 
			return;

		uint32 team = isAlliance(m_Unit) ? 0 : 1; // Set team
		uint32 guardId = 0;
		guardId = team ? zoneSpawn->HordeEntry : zoneSpawn->AllianceEntry;
		guardId = guardId ? guardId : team ? 3296 : 68;

		CreatureProto * cp = CreatureProtoStorage.LookupEntry( guardId );
		if(!cp) 
			return;

		float x = m_Unit->GetPositionX() + (float(rand() % 150 + 100) / 1000.0f );
		float y = m_Unit->GetPositionY() + (float(rand() % 150 + 100) / 1000.0f );
		float z = 0.0f;

		if(m_Unit->GetMapMgr()->IsCollisionEnabled())
		{
			z = CollideInterface.GetHeight(m_Unit->GetMapId(), x, y, m_Unit->GetPositionZ() + 2.0f);
			if( z == NO_WMO_HEIGHT )
				z = m_Unit->GetMapMgr()->GetLandHeight(x, y);
		}
		else
			z = m_Unit->GetMapMgr()->GetLandHeight(x, y);

		if( fabs( z - m_Unit->GetPositionZ() ) > 10.0f )
			z = m_Unit->GetPositionZ();

		// "Guards!"
		m_Unit->SendChatMessage(CHAT_MSG_MONSTER_SAY, team ? LANG_ORCISH : LANG_COMMON, "Guards!");

		uint8 spawned = 0;
	
		unordered_set<Player*>::iterator hostileItr = m_Unit->GetInRangePlayerSetBegin();
		for(; hostileItr != m_Unit->GetInRangePlayerSetEnd(); ++hostileItr)
		{
			if(spawned >= 3)
				break;

			if(!isHostile(*hostileItr, m_Unit))
				continue;

			Creature* guard = NULLCREATURE;
			guard = m_Unit->GetMapMgr()->CreateCreature(guardId);
			if(guard == NULL)
				continue;

			guard->Load(cp, x, y, z);
			guard->SetInstanceID(m_Unit->GetInstanceID());
			guard->SetZoneId(m_Unit->GetZoneId());
			guard->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP); /* shitty DBs */
			guard->m_noRespawn=true;
		
			if(!guard->CanAddToWorld())
			{
				guard->SafeDelete();
				return;
			}

			uint32 t = spawned ? 0 : RandomUInt(8)*1000;
			if( t == 0 )
					guard->PushToWorld(m_Unit->GetMapMgr());
				else
					sEventMgr.AddEvent(guard,&Creature::AddToWorld, m_Unit->GetMapMgr(), EVENT_UNK, t, 1, 0);

			//despawn after 5 minutes.
			sEventMgr.AddEvent(guard, &Creature::SafeDelete, EVENT_CREATURE_SAFE_DELETE, 60*5*1000, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);
			//Start patrolling if nothing else to do.
			sEventMgr.AddEvent(guard, &Creature::SetGuardWaypoints, EVENT_UNK, 10000, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT);

			spawned++;
		}
	}
}

bool isGuard(uint32 id)
{
	switch(id)
	{
		/* stormwind guards */
	case 68:
	case 1423:
	case 1756:
	case 15858:
	case 15859:
	case 16864:
	case 20556:
	case 18948:
	case 18949:
	case 1642:
		/* ogrimmar guards */
	case 3296:
	case 15852:
	case 15853:
	case 15854:
	case 18950:
		/* undercity guards */
	case 5624:
	case 18971:
	case 16432:
		/* exodar */
	case 16733:
	case 18815:
		/* thunder bluff */
	case 3084:
		/* silvermoon */
	case 16221:
	case 17029:
	case 16222:
		/* ironforge */
	case 727:
	case 5595:
	case 12996:
		/* darnassus? */
		{
			return true;
		}break;
	}
	return false;
}

bool isTargetDummy(uint32 id)
{
	switch(id)
	{
		case 1921:
		case 2673:
		case 2674:
		case 4952:
		case 5202:
		case 5652:
		case 5723:
		case 11875:
		case 12385:
		case 12426:
		case 16211:
		case 16897:
		case 17059:
		case 17060:
		case 17578:
		case 18215:
		case 18504:
		case 19139:
		case 21157:
		case 24792:
		case 25225:
		case 25297:
		case 30527:
		case 31143:
		case 31144:
		case 31146:
		case 32541:
		case 32542:
		case 32543:
		case 32545:
		case 32546:
		case 32547:
		case 32666:
		case 32667:
		case 33272:
		case 33243:
		case 33229:
			{
				return true;
			}break;
		default:
			{
				CreatureInfo* info = CreatureNameStorage.LookupEntry(id);
				if(info != NULL)
					if(FindXinYString(string("training dummy"), info->lowercase_name))
						return true;
				return false;
			}
	}
}
void AIInterface::WipeCurrentTarget()
{
	TargetMap::iterator itr = m_aiTargets.find( m_nextTarget );
	if( itr != m_aiTargets.end() )
		m_aiTargets.erase( itr );

	m_nextTarget = NULLUNIT;

	if( m_nextTarget == UnitToFollow )
		UnitToFollow = NULLUNIT;

	if( m_nextTarget == UnitToFollow_backup )
		UnitToFollow_backup = NULLUNIT;
}

// Crow: THIS FUNCTION IS HEAVILY DEPENDENT ON THE CREATURE PROTO COLUMN!
void AIInterface::CheckHeight()
{
	if(m_Unit->GetMapMgr())
	{
		if(m_Unit->IsCreature())
		{
			if(!(TO_CREATURE(m_Unit)->GetProto()->CanMove & LIMIT_AIR))
			{
				m_moveFly = false;
				return;
			}
		}

		uint32 m = m_Unit->GetMapId();
		float x = m_Unit->GetPositionX();
		float y = m_Unit->GetPositionY();
		float z = m_Unit->GetPositionZ();
		if(m_nextPosX && m_nextPosY)
		{
			x = m_nextPosX;
			y = m_nextPosY;
			z = (z > m_nextPosZ ? z : m_nextPosZ); // Crow: Call it hacky, but it works.
		}

		float landheight_z = m_Unit->GetMapMgr()->GetLandHeight(x, y);
		if(m_Unit->GetMapMgr()->IsCollisionEnabled() && CollideInterface.GetHeight(m, x, y, z + 2.0f) != NO_WMO_HEIGHT /*&& CollideInterface.IsOutdoor(m, x, y, z)*/)
			landheight_z = CollideInterface.GetHeight(m, x, y, z + 2.0f);

		if(landheight_z)
		{
			if(landheight_z < (z-3.0f))
				m_moveFly = true;
			else
				m_moveFly = false;
		}
		m_Unit->UpdateVisibility();
	}
}
