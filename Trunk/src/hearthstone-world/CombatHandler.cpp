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

void WorldSession::HandleAttackSwingOpcode( WorldPacket & recv_data )
{
	CHECK_INWORLD_RETURN;
	CHECK_PACKET_SIZE(recv_data, 8);
	uint64 guid;
	recv_data >> guid;

	if(!guid)
	{
		// does this mean cancel combat?
		HandleAttackStopOpcode(recv_data);
		return;
	}

	// AttackSwing
	DEBUG_LOG( "WORLD"," Recvd CMSG_ATTACKSWING Message" );

	if(GetPlayer()->IsPacified() || GetPlayer()->IsStunned() || GetPlayer()->IsFeared())
		return;

//	printf("Got ATTACK SWING: %08X %08X\n", GUID_HIPART(guid), GUID_LOPART(guid));
	Unit* pEnemy = _player->GetMapMgr()->GetUnit(guid);
	//printf("Pointer: %08X\n", pEnemy);

	if(!pEnemy)
	{
		OUT_DEBUG("WORLD: "I64FMT" does not exist.", guid);
		return;
	}

	if(pEnemy->isDead() || _player->isDead())		// haxors :(
	{
		if(sLog.IsOutProccess())
			printf("Oh he dead.\n");
		HandleAttackStopOpcode(recv_data);
		return;
	}

	// Faction "Hack" by Deathshit
	// Implemented Hackfix for quest 1640
	if(sWorld.antihack_cheatengine)
	{
		if(!HasGMPermissions() || !sWorld.no_antihack_on_gm)
		{
			if(pEnemy->GetEntry() == 6090 && _player->HasQuest(1640))
			{
				GetPlayer()->smsg_AttackStart(pEnemy);
				GetPlayer()->EventAttackStart();
				return;
			}

			int attackablestatus = intisAttackable( GetPlayer(), pEnemy, false );
			if((attackablestatus < 1) && !pEnemy->IsInRangeOppFactSet(_player) &&
				!pEnemy->CombatStatus.DidDamageTo(_player->GetGUID()))
			{
				if(attackablestatus == -1)
					return;

				sWorld.LogCheater(this, "Faction exploit detected. Damagetype: Melee.");
				GetPlayer()->BroadcastMessage("Faction exploit detected. You will be disconnected in 5 seconds.");
				GetPlayer()->Kick(5000);
				return;
			}
		}
	}

	GetPlayer()->smsg_AttackStart(pEnemy);
	GetPlayer()->EventAttackStart();
}

void WorldSession::HandleAttackStopOpcode( WorldPacket & recv_data )
{
	CHECK_INWORLD_RETURN;
	uint64 guid = GetPlayer()->GetSelection();
	Unit* pEnemy = NULLUNIT;

	if(guid)
	{
		pEnemy = _player->GetMapMgr()->GetUnit(guid);
		if(pEnemy)
		{
			GetPlayer()->EventAttackStop();
			GetPlayer()->smsg_AttackStop(pEnemy);
		}
	}
}

