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

/////////////////////////////////////////////////
//  Admin Chat Commands
/////////////////////////////////////////////////

#include "StdAfx.h"
#include "ObjectMgr.h"
#include "Master.h"

bool ChatHandler::HandleWorldPortCommand(const char* args, WorldSession *m_session)
{
	float x, y, z;
	uint32 mapid;
	if(sscanf(args, "%u %f %f %f", (unsigned int*)&mapid, &x, &y, &z) != 4)
		return false;

	if(x >= _maxX || x <= _minX || y <= _minY || y >= _maxY)
		return false;

	LocationVector vec(x, y, z);
	if(m_session->GetPlayer()->GetMapId() == mapid || !m_session->CheckTeleportPrerequisites(NULL, m_session, m_session->GetPlayer(), mapid))
		m_session->GetPlayer()->SafeTeleport(mapid, 0, vec);
	else
	{
		WorldPacket data(SMSG_AREA_TRIGGER_MESSAGE, 50);
		data << uint32(0);
		data << "You do not reach the requirements to teleport here.";
		data << uint8(0);
		m_session->SendPacket(&data);
	}
	return true;
}


bool ChatHandler::HandleClearCooldownsCommand(const char *args, WorldSession *m_session)
{
	uint32 guid = (uint32)m_session->GetPlayer()->GetSelection();
	Player* plr = getSelectedChar(m_session, true);

	if(!plr)
	{
		plr = m_session->GetPlayer();
		SystemMessage(m_session, "Auto-targeting self.");
	}
	if(!plr) return false;

	if(plr->getClass()==WARRIOR)
	{
		plr->ClearCooldownsOnLine(26, guid);
		plr->ClearCooldownsOnLine(256, guid);
		plr->ClearCooldownsOnLine(257 , guid);
		BlueSystemMessage(m_session, "Cleared all Warrior cooldowns.");
		return true;
	}
	if(plr->getClass()==PALADIN)
	{
		plr->ClearCooldownsOnLine(594, guid);
		plr->ClearCooldownsOnLine(267, guid);
		plr->ClearCooldownsOnLine(184, guid);
		BlueSystemMessage(m_session, "Cleared all Paladin cooldowns.");
		return true;
	}
	if(plr->getClass()==HUNTER)
	{
		plr->ClearCooldownsOnLine(50, guid);
		plr->ClearCooldownsOnLine(51, guid);
		plr->ClearCooldownsOnLine(163, guid);
		BlueSystemMessage(m_session, "Cleared all Hunter cooldowns.");
		return true;
	}
	if(plr->getClass()==ROGUE)
	{
		plr->ClearCooldownsOnLine(253, guid);
		plr->ClearCooldownsOnLine(38, guid);
		plr->ClearCooldownsOnLine(39, guid);
		BlueSystemMessage(m_session, "Cleared all Rogue cooldowns.");
		return true;
	}
	if(plr->getClass()==PRIEST)
	{
		plr->ClearCooldownsOnLine(56, guid);
		plr->ClearCooldownsOnLine(78, guid);
		plr->ClearCooldownsOnLine(613, guid);
		BlueSystemMessage(m_session, "Cleared all Priest cooldowns.");
		return true;
	}
	if(plr->getClass()==SHAMAN)
	{
		plr->ClearCooldownsOnLine(373, guid);
		plr->ClearCooldownsOnLine(374, guid);
		plr->ClearCooldownsOnLine(375, guid);
		BlueSystemMessage(m_session, "Cleared all Shaman cooldowns.");
		return true;
	}
	if(plr->getClass()==DEATHKNIGHT)
	{
		plr->ClearCooldownsOnLine(770, guid);
		plr->ClearCooldownsOnLine(771, guid);
		plr->ClearCooldownsOnLine(772, guid);
		BlueSystemMessage(m_session, "Cleared all Death Knight cooldowns.");
		return true;
	}
	if(plr->getClass()==MAGE)
	{
		plr->ClearCooldownsOnLine(6, guid);
		plr->ClearCooldownsOnLine(8, guid);
		plr->ClearCooldownsOnLine(237, guid);
		BlueSystemMessage(m_session, "Cleared all Mage cooldowns.");
		return true;
	}
	if(plr->getClass()==WARLOCK)
	{
		plr->ClearCooldownsOnLine(355, guid);
		plr->ClearCooldownsOnLine(354, guid);
		plr->ClearCooldownsOnLine(593, guid);
		BlueSystemMessage(m_session, "Cleared all Warlock cooldowns.");
		return true;
	}
	if(plr->getClass()==DRUID)
	{
		plr->ClearCooldownsOnLine(573, guid);
		plr->ClearCooldownsOnLine(574, guid);
		plr->ClearCooldownsOnLine(134, guid);
		BlueSystemMessage(m_session, "Cleared all Druid cooldowns.");
		return true;
	}
	return true;
}

bool ChatHandler::HandleLearnCommand(const char* args, WorldSession *m_session)
{
	if( !*args )
		return false;

	Player* plr = getSelectedChar( m_session, true );
	if( !plr )
	{
		plr = m_session->GetPlayer();
		SystemMessage(m_session, "Auto-targeting self.");
	}
	if(	!plr )
		return false;

	if( stricmp( args, "all" ) == 0 )
	{
		sGMLog.writefromsession(m_session, "taught %s all spells.", plr->GetName());
#include "ClassSpells.h"

		uint32 c = plr->getClass();
		for( uint32 i = 0; spellarray[c][i] != 0; ++i )
		{
			plr->addSpell(spellarray[c][i]);
		}

		static uint32 paladinspellarray[RACE_DRAENEI + 1][2] = {
			{ 0 },		// RACE 0
			{ 13819, 0 },		// HUMAN
			{ 0 },		// ORC
			{ 13819, 0 },		// DWARF
			{ 0 },		// NIGHTELF
			{ 0 },		// UNDEAD
			{ 0 },		// TAUREN
			{ 0 },		// GNOME
			{ 0 },		// TROLL
			{ 34769, 0 },		// BLOODELF
			{ 13819, 0 },		// DRAENEI
		};

		static uint32 priestspellarray[RACE_DRAENEI+1][16] = {
			{ 0 },		// RACE 0
			{ 13896, 13908, 19236, 19238, 19240, 19241, 19242, 19243, 19271, 19273, 19274, 19275, 25437, 25441, 0 },		// HUMAN
			{ 0 },		// ORC
			{ 13908, 19236, 19238, 19240, 44041, 19241, 19242, 19243, 25437, 44043, 44044, 44045, 44046, 44047, 0 },		// DWARF
			{ 2651, 10797, 19296, 19299, 19302, 19303, 19304, 19305, 25446, 0 },		// NIGHTELF
			{ 2652, 2944, 19261, 19262, 19264, 19265, 19266, 19276, 19277, 19278, 19279, 19280, 25461, 25467, 0 },		// UNDEAD
			{ 0 },		// TAUREN
			{ 0 },		// GNOME
			{ 9035, 18137, 19281, 19282, 19283, 19284, 19285, 19308, 19309, 19310, 19311, 19312, 25470, 25477, 0 },		// TROLL
			{ 2652, 19261, 19262, 19264, 19265, 19266, 25461, 32676, 0 },		// BLOODELF
			{ 32548, 44041, 44043, 44044, 44045, 44046, 44047, 0 },		// DRAENEI
		};

		static uint32 shamanspellarray[RACE_DRAENEI+1][2] = {
			{ 0 },		// RACE 0
			{ 0 },		// HUMAN
			{ 2825, 0 },		// ORC
			{ 0 },		// DWARF
			{ 0 },		// NIGHTELF
			{ 0 },		// UNDEAD
			{ 2825, 0 },		// TAUREN
			{ 0 },		// GNOME
			{ 2825, 0 },		// TROLL
			{ 0 },		// BLOODELF
			{ 32182, 0 },		// DRAENEI
		};

		static uint32 magespellarray[RACE_DRAENEI+1][11] = {
			{ 0 },		// RACE 0
			{ 3561, 3562, 3565, 10059, 11416, 11419, 32266, 32271, 33690, 33691, 0 },		// HUMAN
			{ 3563, 3566, 3567, 11417, 11418, 11420, 32267, 32272, 35715, 35717, 0 },		// ORC
			{ 3561, 3562, 3565, 10059, 11416, 11419, 32266, 32271, 33690, 33691, 0 },		// DWARF
			{ 3561, 3562, 3565, 10059, 11416, 11419, 32266, 32271, 33690, 33691, 0 },		// NIGHTELF
			{ 3563, 3566, 3567, 11417, 11418, 11420, 32267, 32272, 35715, 35717, 0 },		// UNDEAD
			{ 3563, 3566, 3567, 11417, 11418, 11420, 32267, 32272, 35715, 35717, 0 },		// TAUREN
			{ 3561, 3562, 3565, 10059, 11416, 11419, 32266, 32271, 33690, 33691, 0 },		// GNOME
			{ 3563, 3566, 3567, 11417, 11418, 11420, 32267, 32272, 35715, 35717, 0 },		// TROLL
			{ 3563, 3566, 3567, 11417, 11418, 11420, 32267, 32272, 35715, 35717, 0 },		// BLOODELF
			{ 3561, 3562, 3565, 10059, 11416, 11419, 32266, 32271, 33690, 33691, 0 },		// DRAENEI
		};

		uint32 r = plr->getRace();
		switch(c)
		{
		case PALADIN:
			for( uint32 i = 0; paladinspellarray[r][i] != 0; ++i )
			{
				plr->addSpell( paladinspellarray[r][i] );
			}
			break;
		case PRIEST:
			for( uint32 i = 0; priestspellarray[r][i] != 0; ++i )
			{
				plr->addSpell( priestspellarray[r][i] );
			}
			break;
		case MAGE:
			for( uint32 i = 0; magespellarray[r][i] != 0; ++i )
			{
				plr->addSpell( magespellarray[r][i] );
			}
			break;
		case SHAMAN:
			for( uint32 i = 0; shamanspellarray[r][i] != 0; ++i )
			{
				plr->addSpell( shamanspellarray[r][i] );
			}
			break;
		}
		return true;
	}

	uint32 spell = atol((char*)args);
	sGMLog.writefromsession(m_session, "taught %s spell %u", plr->GetName(), spell);

	SpellEntry * sp = dbcSpell.LookupEntry(spell);
	if(!plr->GetSession()->HasGMPermissions() && (sp->Effect[0]==SPELL_EFFECT_INSTANT_KILL||sp->Effect[1]==SPELL_EFFECT_INSTANT_KILL||sp->Effect[2]==SPELL_EFFECT_INSTANT_KILL))
	{
		SystemMessage(m_session, "don't be an idiot and teach players instakill spells. this action has been logged.");
		return true;
	}

	if( plr->HasSpell( spell ) ) // check to see if char already knows
	{
		std::string OutStr = plr->GetName();
		OutStr += " already knows that spell.";

		SystemMessage(m_session, OutStr.c_str());
		return true;
	}

	plr->addSpell(spell);
	BlueSystemMessageToPlr(plr, "%s taught you Spell %d", m_session->GetPlayer()->GetName(), spell);

	return true;
}

bool ChatHandler::HandleReviveCommand(const char* args, WorldSession *m_session)
{
	Player* SelectedPlayer = getSelectedChar(m_session, true);
	if(!SelectedPlayer)
		return true;

	if(SelectedPlayer->m_currentMovement == MOVE_ROOT)
		SelectedPlayer->SetMovement(MOVE_UNROOT, 1);
	if(SelectedPlayer->getDeathState())
		SelectedPlayer->ResurrectPlayer();

	SelectedPlayer->SetUInt32Value(UNIT_FIELD_HEALTH, SelectedPlayer->GetUInt32Value(UNIT_FIELD_MAXHEALTH) );
	SelectedPlayer->SetUInt32Value(UNIT_FIELD_POWER1, SelectedPlayer->GetUInt32Value(UNIT_FIELD_MAXPOWER1) );
	SelectedPlayer->SetUInt32Value(UNIT_FIELD_POWER4, SelectedPlayer->GetUInt32Value(UNIT_FIELD_MAXPOWER4) );
	return true;
}

bool ChatHandler::HandleExploreCheatCommand(const char* args, WorldSession *m_session)
{
	if (!*args)
		return false;

	int flag = atoi((char*)args);

	Player* chr = getSelectedChar(m_session);
	if (chr == NULL)
	{
		SystemMessage(m_session, "No character selected.");
		return true;
	}

	char buf[256];

	// send message to user
	if (flag != 0)
	{
		snprintf((char*)buf,256,"%s has explored all zones now.", chr->GetName());
	}
	else
	{
		snprintf((char*)buf,256,"%s has no more explored zones.", chr->GetName());
	}
	SystemMessage(m_session, buf);

	// send message to player
	if (flag != 0)
	{
		snprintf((char*)buf,256,"%s has explored all zones for you.",
			m_session->GetPlayer()->GetName());
	}
	else
	{
		snprintf((char*)buf,256,"%s has hidden all zones from you.", 
			m_session->GetPlayer()->GetName());
	}
	SystemMessage(m_session,  buf);


	for (uint8 i = 0; i < 128; ++i)
	{
		if (flag != 0)
		{
			chr->SetFlag(PLAYER_EXPLORED_ZONES_1+i,0xFFFFFFFF);
		}
		else
		{
			chr->SetFlag(PLAYER_EXPLORED_ZONES_1+i,0);
		}
	}

	return true;
}

bool ChatHandler::HandleBanCharacterCommand(const char* args, WorldSession *m_session)
{
	if(!*args)
		return false;

	// this is rather complicated due to ban reasons being able to have spaces. so we'll have to some c string magic
	// rather than just sscanf'ing it.
	char * pCharacter = (char*)args;
	char * pBanDuration = strchr(pCharacter, ' ');
	PlayerInfo * pInfo = NULL;
	if(pBanDuration == NULL)
		return false;

	char * pReason = strchr(pBanDuration+1, ' ');
	if(pReason == NULL)
		return false;

	// zero them out to create sepearate strings.
	*pBanDuration = 0;
	++pBanDuration;
	*pReason = 0;
	++pReason;

	int32 BanTime = GetTimePeriodFromString(pBanDuration);
	if(BanTime < 1)
		return false;

	Player* pPlayer = objmgr.GetPlayer(pCharacter, false);
	if(pPlayer == NULL)
	{
		pInfo = objmgr.GetPlayerInfoByName(pCharacter);
		if(pInfo == NULL)
		{
			SystemMessage(m_session, "Player not found.");
			return true;
		}

		SystemMessage(m_session, "Banning player '%s' in database for '%s'.", pCharacter, pReason);
		string escaped_reason = CharacterDatabase.EscapeString(string(pReason));
		
		CharacterDatabase.Execute("UPDATE characters SET banned = %u, banReason = '%s' WHERE guid = %u",
			BanTime ? BanTime+(uint32)UNIXTIME : 1, escaped_reason.c_str(), pInfo->guid);
	}
	else
	{
		SystemMessage(m_session, "Banning player '%s' ingame for '%s'.", pCharacter, pReason);
		string sReason = string(pReason);
		uint32 uBanTime = BanTime ? BanTime+(uint32)UNIXTIME : 1;
		pPlayer->SetBanned(uBanTime, sReason);
		pInfo = pPlayer->m_playerInfo;
	}

	SystemMessage(m_session, "This ban is due to expire %s%s.", BanTime ? "on " : "", BanTime ? ConvertTimeStampToDataTime(BanTime+(uint32)UNIXTIME).c_str() : "Never");
	if(pPlayer)
	{
		SystemMessage(m_session, "Kicking %s.", pPlayer->GetName());
		pPlayer->Kick();
	}

	sGMLog.writefromsession(m_session, "used ban character on %s reason %s for %s", pCharacter, pReason, BanTime ? ConvertTimeStampToString(BanTime).c_str() : "ever");
	if( sWorld.m_banTable && pInfo )
	{
		CharacterDatabase.Execute("INSERT INTO %s VALUES('%s', '%s', %u, %u, '%s')", sWorld.m_banTable,
			m_session->GetPlayer()->GetName(), pInfo->name, (uint32)UNIXTIME, (uint32)UNIXTIME + BanTime, CharacterDatabase.EscapeString(string(pReason)).c_str() );
	}
	return true;
}

bool ChatHandler::HandleUnBanCharacterCommand(const char* args, WorldSession *m_session)
{
	if(!*args)
		return false;

	char Character[255];
	if(sscanf(args, "%s", Character) == 0)
	{
		RedSystemMessage(m_session, "A character name and reason is required.");
		return true;
	}

	// Check if player is in world.
	Player* pPlayer = ObjectMgr::getSingleton( ).GetPlayer(Character, false);
	if(pPlayer != 0)
	{
		GreenSystemMessage(m_session, "Unbanned player %s ingame.",pPlayer->GetName());
		pPlayer->UnSetBanned();
	}
	else
	{
		GreenSystemMessage(m_session, "Player %s not found ingame.", Character);
	}

	// Ban in database
	CharacterDatabase.Execute("UPDATE characters SET banned = 0 WHERE name = '%s'", CharacterDatabase.EscapeString(string(Character)).c_str());

	SystemMessage(m_session, "Unbanned character %s in database.", Character);
	sGMLog.writefromsession(m_session, "used unban character on %s", Character);
	return true;
}

bool ChatHandler::HandleAddSkillCommand(const char* args, WorldSession *m_session)
{
	char buf[256];
	Player* target = objmgr.GetPlayer((uint32)m_session->GetPlayer()->GetSelection());

	if(!target) {
		SystemMessage(m_session, "Select A Player first.");
		return true;
	}

	uint32 skillline;
	uint16 cur, max;

	char* pSkillline = strtok((char*)args, " ");
	if (!pSkillline)
		return false;

	char* pCurrent = strtok(NULL, " ");
	if (!pCurrent)
		return false;

	char* pMax = strtok(NULL, " ");
	if (!pMax)
		return false;

	skillline = (uint32)atol(pSkillline);
	cur = (uint16)atol(pCurrent);
	max = (uint16)atol(pMax);

	target->_AddSkillLine(skillline,cur,max);

	snprintf(buf,256,"SkillLine: %u CurrentValue %u Max Value %u Added.",(unsigned int)skillline,(unsigned int)cur,(unsigned int)max);
	sGMLog.writefromsession(m_session, "added skill line %u (%u/%u) to %s", skillline, cur, max, target->GetName());
	SystemMessage(m_session, buf);

	return true;
}

bool ChatHandler::HandleNpcInfoCommand(const char *args, WorldSession *m_session)
{
	char msg[512];
	uint32 guid = GUID_LOPART(m_session->GetPlayer()->GetSelection());
	Creature* crt = getSelectedCreature(m_session);
	if(!crt) return false;
	if(crt->GetCreatureInfo())
		BlueSystemMessage(m_session, "Showing creature info for %s", crt->GetCreatureInfo()->Name);
	snprintf(msg,512,"GUID: %d\nFaction: %d\nNPCFlags: %d\nDisplayID: %d\n Scale %f", (int)guid, (int)crt->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE), (int)crt->GetUInt32Value(UNIT_NPC_FLAGS), (int)crt->GetUInt32Value(UNIT_FIELD_DISPLAYID), crt->proto->Scale);
	SystemMessage(m_session, msg);
	if(crt->m_faction)
		GreenSystemMessage(m_session, "Combat Support: 0x%.3X", crt->m_faction->FriendlyMask);
	GreenSystemMessage(m_session, "Base Health: %d", crt->GetUInt32Value(UNIT_FIELD_BASE_HEALTH));
	GreenSystemMessage(m_session, "Base Armor: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES));
	GreenSystemMessage(m_session, "Base Mana: %d", crt->GetUInt32Value(UNIT_FIELD_MAXPOWER1));
	GreenSystemMessage(m_session, "Base Holy: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_1));
	GreenSystemMessage(m_session, "Base Fire: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_2));
	GreenSystemMessage(m_session, "Base Nature: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_3));
	GreenSystemMessage(m_session, "Base Frost: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_4));
	GreenSystemMessage(m_session, "Base Shadow: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_5));
	GreenSystemMessage(m_session, "Base Arcane: %d", crt->GetUInt32Value(UNIT_FIELD_RESISTANCES_6));
	GreenSystemMessage(m_session, "Damage min/max: %f/%f", crt->GetFloatValue(UNIT_FIELD_MINDAMAGE),crt->GetFloatValue(UNIT_FIELD_MAXDAMAGE));
	
	ColorSystemMessage(m_session, MSG_COLOR_RED, "Entry ID: %d", crt->GetUInt32Value(OBJECT_FIELD_ENTRY));
	ColorSystemMessage(m_session, MSG_COLOR_RED, "SQL Entry ID: %d", crt->GetSQL_id());
	// show byte
	std::stringstream sstext;
	uint32 theBytes = crt->GetUInt32Value(UNIT_FIELD_BYTES_0);
	sstext << "UNIT_FIELD_BYTES_0 are " << uint16((uint8)theBytes & 0xFF) << " " << uint16((uint8)(theBytes >> 8) & 0xFF) << " ";
	sstext << uint16((uint8)(theBytes >> 16) & 0xFF) << " " << uint16((uint8)(theBytes >> 24) & 0xFF) << '\0';
	BlueSystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleIncreaseWeaponSkill(const char *args, WorldSession *m_session)
{
	char *pMin = strtok((char*)args, " ");
	uint32 cnt = 0;
	if(!pMin)
		cnt = 1;
	else
		cnt = atol(pMin);

	Player* pr = getSelectedChar(m_session, true);
	
	uint32 SubClassSkill = 0;
	if(!pr) pr = m_session->GetPlayer();
	if(!pr) return false;
	Item* it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
	ItemPrototype* proto = NULL;
	if (!it)
		it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
	if (it)
		proto = it->GetProto();
	if (proto)
	{
		switch(proto->SubClass)
		{
			// Weapons
		case 0:	// 1 handed axes
			SubClassSkill = SKILL_AXES;
			break;
		case 1:	// 2 handed axes
			SubClassSkill = SKILL_2H_AXES;
			break;
		case 2:	// bows
			SubClassSkill = SKILL_BOWS;
			break;
		case 3:	// guns
			SubClassSkill = SKILL_GUNS;
			break;
		case 4:	// 1 handed mace
			SubClassSkill = SKILL_MACES;
			break;
		case 5:	// 2 handed mace
			SubClassSkill = SKILL_2H_MACES;
			break;
		case 6:	// polearms
			SubClassSkill = SKILL_POLEARMS;
			break;
		case 7: // 1 handed sword
			SubClassSkill = SKILL_SWORDS;
			break;
		case 8: // 2 handed sword
			SubClassSkill = SKILL_2H_SWORDS;
			break;
		case 9: // obsolete
			SubClassSkill = 136;
			break;
		case 10: //1 handed exotic
			SubClassSkill = 136;
			break;
		case 11: // 2 handed exotic
			SubClassSkill = 0;
			break;
		case 12: // fist
			SubClassSkill = SKILL_FIST_WEAPONS;
			break;
		case 13: // misc
			SubClassSkill = 0;
			break;
		case 15: // daggers
			SubClassSkill = SKILL_DAGGERS;
			break;
		case 16: // thrown
			SubClassSkill = SKILL_THROWN;
			break;
		case 17: // spears
			SubClassSkill = SKILL_SPEARS;
			break;
		case 18: // crossbows
			SubClassSkill = SKILL_CROSSBOWS;
			break;
		case 19: // wands
			SubClassSkill = SKILL_WANDS;
			break;
		case 20: // fishing
			SubClassSkill = SKILL_FISHING;
			break;
		}
	}
	else
	{
		SubClassSkill = 162;
	}

	if(!SubClassSkill)
	{
		RedSystemMessage(m_session, "Can't find skill ID :-/");
		return false;
	}

	uint32 skill = SubClassSkill;

	BlueSystemMessage(m_session, "Modifying skill line %d. Advancing %d times.", skill, cnt);
	sGMLog.writefromsession(m_session, "increased weapon skill of %s by %u", pr->GetName(), cnt);

	if(!pr->_HasSkillLine(skill))
	{
		SystemMessage(m_session, "Does not have skill line, adding.");
		pr->_AddSkillLine(skill, 1, 300);
	}
	else
	{
		pr->_AdvanceSkillLine(skill,cnt);
	}
	return true;
}


bool ChatHandler::HandleResetTalentsCommand(const char* args, WorldSession *m_session)
{
	Player* plr = this->getSelectedChar(m_session);
	if(!plr) return true;

	plr->Reset_Talents();

	SystemMessage(m_session, "Reset talents of %s.", plr->GetName());
	BlueSystemMessageToPlr(plr, "%s reset all your talents.", m_session->GetPlayer()->GetName());
	sGMLog.writefromsession(m_session, "reset talents of %s", plr->GetName());
	return true;
}

bool ChatHandler::HandleResetSpellsCommand(const char* args, WorldSession *m_session)
{
	Player* plr = this->getSelectedChar(m_session);
	if(!plr) return true;

	plr->Reset_Spells();

	SystemMessage(m_session, "Reset spells of %s to level 1.", plr->GetName());
	BlueSystemMessage(m_session, "%s reset all your spells to starting values.", m_session->GetPlayer()->GetName());
	sGMLog.writefromsession(m_session, "reset spells of %s", plr->GetName());
	return true;
}

bool ChatHandler::HandleAccountLevelCommand(const char * args, WorldSession * m_session)
{
    if(!*args) return false;

	char account[100];
	char gmlevel[100];
	int argc = sscanf(args, "%s %s", account, gmlevel);
	if(argc != 2)
		return false;

	sLogonCommHandler.Account_SetGM( account, gmlevel );

	GreenSystemMessage(m_session, "Account '%s' level has been updated to '%s'. The change will be effective immediately.", account, gmlevel);
	sGMLog.writefromsession(m_session, "set account %s flags to %s", account, gmlevel);

	return true;
}

bool ChatHandler::HandleAccountUnbanCommand(const char * args, WorldSession * m_session)
{
	if(!*args) return false;
	char * pAccount = (char*)args;

	sLogonCommHandler.Account_SetBanned( pAccount, 0, "" );
	GreenSystemMessage(m_session, "Account '%s' has been unbanned. This change will be effective immediately.", pAccount);

	sGMLog.writefromsession(m_session, "unbanned account %s", pAccount);
	return true;
}

bool ChatHandler::HandleAccountBannedCommand(const char * args, WorldSession * m_session)
{
	if(!*args)
		return false;

	char * pAccount = (char*)args;
	char * pDuration = strchr(pAccount, ' ');
	if( pDuration == NULL )
		return false;
	*pDuration = 0;
	++pDuration;

	char * pReason = strchr(pDuration, ' ');
	if( pReason == NULL )
		return false;

	*pReason = 0;
	++pReason;

	int32 timeperiod = GetTimePeriodFromString(pDuration);
	if( timeperiod < 0 )
		return false;

	uint32 banned = (timeperiod ? (uint32)UNIXTIME+timeperiod : 1);

	/*stringstream my_sql;
	my_sql << "UPDATE accounts SET banned = " << banned << " WHERE login = '" << CharacterDatabase.EscapeString(string(pAccount)) << "'";

	sLogonCommHandler.LogonDatabaseSQLExecute(my_sql.str().c_str());
	sLogonCommHandler.LogonDatabaseReloadAccounts();*/
	sLogonCommHandler.Account_SetBanned(pAccount, banned, pReason);

	GreenSystemMessage(m_session, "Account '%s' has been banned %s%s. The change will be effective immediately.", pAccount,
		timeperiod ? "until " : "forever", timeperiod ? ConvertTimeStampToDataTime(timeperiod+(uint32)UNIXTIME).c_str() : "");

	sWorld.DisconnectUsersWithAccount(pAccount, m_session);
	sGMLog.writefromsession(m_session, "banned account %s until %s for %s", pAccount, timeperiod ? ConvertTimeStampToDataTime(timeperiod+(uint32)UNIXTIME).c_str() : "permanant", pReason);
	return true;
}

bool ChatHandler::HandleAccountMuteCommand(const char * args, WorldSession * m_session)
{
	if(!*args) return false;

	char * pAccount = (char*)args;
	char * pDuration = strchr(pAccount, ' ');
	if(pDuration == NULL)
		return false;
	*pDuration = 0;
	++pDuration;

	int32 timeperiod = GetTimePeriodFromString(pDuration);
	if(timeperiod <= 0)
		return false;

	uint32 banned = (uint32)UNIXTIME+timeperiod;

	sLogonCommHandler.Account_SetMute( pAccount, banned );

	string tsstr = ConvertTimeStampToDataTime(timeperiod+(uint32)UNIXTIME);
	GreenSystemMessage(m_session, "Account '%s' has been muted until %s. The change will be effective immediately.", pAccount, 
		tsstr.c_str());

	sGMLog.writefromsession(m_session, "mutex account %s until %s", pAccount, ConvertTimeStampToDataTime(timeperiod+(uint32)UNIXTIME).c_str());

	WorldSession * pSession = sWorld.FindSessionByName(pAccount);
	if( pSession != NULL )
	{
		pSession->m_muted = banned;
		pSession->SystemMessage("Your voice has been muted until %s by a GM. Until this time, you will not be able to speak in any form.", tsstr.c_str());
	}

	return true;
}

bool ChatHandler::HandleAccountUnmuteCommand(const char * args, WorldSession * m_session)
{
	sLogonCommHandler.Account_SetMute( args, 0 );

	GreenSystemMessage(m_session, "Account '%s' has been unmuted.", args);
	WorldSession * pSession = sWorld.FindSessionByName(args);
	if( pSession != NULL )
	{
		pSession->m_muted = 0;
		pSession->SystemMessage("Your voice has restored. You may speak again.");
	}

	return true;
}

bool ChatHandler::HandleGetTransporterTime(const char* args, WorldSession* m_session)
{
	//Player* plyr = m_session->GetPlayer();
	Creature* crt = getSelectedCreature(m_session, false);
	if( crt == NULL )
		return false;

	WorldPacket data(SMSG_ATTACKERSTATEUPDATE, 1000);
	data << uint32(0x00000102);
	data << crt->GetNewGUID();
	data << m_session->GetPlayer()->GetNewGUID();

	data << uint32(6);
	data << uint8(1);
	data << uint32(1);
	data << uint32(0x40c00000);
	data << uint32(6);
	data << uint32(0);
	data << uint32(0);
	data << uint32(1);
	data << uint32(0x000003e8);
	data << uint32(0);
	data << uint32(0);
	m_session->SendPacket(&data);
	return true;
}

bool ChatHandler::HandleRemoveAurasCommand(const char *args, WorldSession *m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr) return false;

	BlueSystemMessage(m_session, "Removing all auras...");
	for(uint32 i = 0; i < MAX_AURAS; ++i)
	{
		if(plr->m_auras[i] != NULL) 
			plr->RemoveAuraBySlot(i);
	}
	return true;
}

bool ChatHandler::HandleRemoveRessurectionSickessAuraCommand(const char *args, WorldSession *m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr) return false;

	BlueSystemMessage(m_session, "Removing ressurection sickness...");
	plr->RemoveAura( 15007 );
	return true;
}

bool ChatHandler::HandleParalyzeCommand(const char* args, WorldSession *m_session)
{
	//Player* plr = getSelectedChar(m_session, true);
	//if(!plr) return false;
	Unit* plr = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!plr || plr->GetTypeId() != TYPEID_PLAYER)
	{
		RedSystemMessage(m_session, "Invalid target.");
		return true;
	}

	BlueSystemMessage(m_session, "Rooting target.");
	BlueSystemMessageToPlr( TO_PLAYER( plr ), "You have been rooted by %s.", m_session->GetPlayer()->GetName() );
	WorldPacket data;
	data.Initialize(SMSG_FORCE_MOVE_ROOT);
	data << plr->GetNewGUID();
	data << uint32(1);

	plr->SendMessageToSet(&data, true);
	return true;
}

bool ChatHandler::HandleUnParalyzeCommand(const char* args, WorldSession *m_session)
{
	//Player* plr = getSelectedChar(m_session, true);
	//if(!plr) return false;
	Unit* plr = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!plr || plr->GetTypeId() != TYPEID_PLAYER)
	{
		RedSystemMessage(m_session, "Invalid target.");
		return true;
	}
	
	BlueSystemMessage(m_session, "Unrooting target.");
	BlueSystemMessageToPlr( TO_PLAYER( plr ), "You have been unrooted by %s.", m_session->GetPlayer()->GetName() );
	WorldPacket data;
	data.Initialize(SMSG_FORCE_MOVE_UNROOT);
	data << plr->GetNewGUID();
	data << uint32(5);

	plr->SendMessageToSet(&data, true);
	return true;
}

bool ChatHandler::HandleSetMotdCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "You must specify a message.");
		return true;
	}

	GreenSystemMessage(m_session, "Motd has been set to: %s", args);
	World::getSingleton().SetMotd(args);
	sGMLog.writefromsession(m_session, "Set MOTD to %s", args);
	return true;
}

bool ChatHandler::HandleAddItemSetCommand(const char* args, WorldSession* m_session)
{
	uint32 setid = (args ? atoi(args) : 0);
	if(!setid)
	{
		RedSystemMessage(m_session, "You must specify a setid.");
		return true;
	}

	Player* chr = getSelectedChar(m_session);
	if (chr == NULL) {
	RedSystemMessage(m_session, "Unable to select character.");
	return true;
	}

	ItemSetEntry *entry = dbcItemSet.LookupEntry(setid);
	std::list<ItemPrototype*>* l = objmgr.GetListForItemSet(setid);
	if(!entry || !l)
	{
		RedSystemMessage(m_session, "Invalid item set.");
		return true;
	}
	//const char* setname = sItemSetStore.LookupString(entry->name);
	BlueSystemMessage(m_session, "Searching item set %u...", setid);
	uint32 start = getMSTime();
	sGMLog.writefromsession(m_session, "used add item set command, set %u, target %s", setid, chr->GetName());
	for(std::list<ItemPrototype*>::iterator itr = l->begin(); itr != l->end(); ++itr)
	{
		Item* itm = objmgr.CreateItem((*itr)->ItemId, m_session->GetPlayer());
		if(!itm) continue;
		if(itm->GetProto()->Bonding == ITEM_BIND_ON_PICKUP)
			itm->SoulBind();

		if(!chr->GetItemInterface()->AddItemToFreeSlot(itm))
		{
			m_session->SendNotification("No free slots left!");
			itm->DeleteMe();
			itm = NULLITEM;
			return true;
		} else {
			//SystemMessage(m_session, "Added item: %s [%u]", (*itr)->Name1, (*itr)->ItemId);
			SlotResult * le = chr->GetItemInterface()->LastSearchResult();
			chr->GetSession()->SendItemPushResult(itm,false,true,false,true,le->ContainerSlot,le->Slot,1);
		}
	}
	GreenSystemMessage(m_session, "Added set to inventory complete. Time: %u ms", getMSTime() - start);
	return true;
}

bool ChatHandler::HandleExitInstanceCommand(const char* args, WorldSession* m_session)
{
	BlueSystemMessage(m_session, "Attempting to exit from instance...");
	bool result = m_session->GetPlayer()->ExitInstance();
	if(!result)
	{
		RedSystemMessage(m_session, "Entry points not found.");
		return true;
	} else {
		GreenSystemMessage(m_session, "Removal successful.");
		return true;
	}
}

bool ChatHandler::HandleCastTimeCheatCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr) return true;

	bool val = plyr->CastTimeCheat;
	BlueSystemMessage(m_session, "%s cast time cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
	GreenSystemMessageToPlr(plyr, "%s %s a cast time cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plyr->CastTimeCheat = !val;
	sGMLog.writefromsession(m_session, "%s cast time cheat on %s", val ? "disabled" : "enabled", plyr->GetName());
	return true;
}

bool ChatHandler::HandleCooldownCheatCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr) return true;

	bool val = plyr->CooldownCheat;
	BlueSystemMessage(m_session, "%s cooldown cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
	GreenSystemMessageToPlr(plyr, "%s %s a cooldown cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plyr->CooldownCheat = !val;
	sGMLog.writefromsession(m_session, "%s cooldown cheat on %s", val ? "disabled" : "enabled", plyr->GetName());

	return true;
}

bool ChatHandler::HandleGodModeCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr)
		return true;

	bool val = plyr->GodModeCheat;
	BlueSystemMessage(m_session, "%s godmode cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
	GreenSystemMessageToPlr(plyr, "%s %s a godmode cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plyr->GodModeCheat = !val;
	sGMLog.writefromsession(m_session, "%s godmode cheat on %s", val ? "disabled" : "enabled", plyr->GetName());
	return true;
}

bool ChatHandler::HandlePowerCheatCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr)
		return true;

	bool val = plyr->PowerCheat;
	BlueSystemMessage(m_session, "%s power cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
	GreenSystemMessageToPlr(plyr, "%s %s a power cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plyr->PowerCheat = !val;
	sGMLog.writefromsession(m_session, "%s powertime cheat on %s", val ? "disabled" : "enabled", plyr->GetName());
	return true;
}

bool ChatHandler::HandleShowCheatsCommand(const char* args, WorldSession* m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr) return true;

	uint32 active = 0, inactive = 0;
#define print_cheat_status(CheatName, CheatVariable) SystemMessage(m_session, "%s%s: %s%s", MSG_COLOR_LIGHTBLUE, CheatName, \
		CheatVariable ? MSG_COLOR_LIGHTRED : MSG_COLOR_GREEN, CheatVariable ? "Active" : "Inactive");  \
		if(CheatVariable) \
		active++; \
		else \
		inactive++;

	GreenSystemMessage(m_session, "Showing cheat status for: %s", plyr->GetName());
	print_cheat_status("Cooldown", plyr->CooldownCheat);
	print_cheat_status("CastTime", plyr->CastTimeCheat);
	print_cheat_status("GodMode", plyr->GodModeCheat);
	print_cheat_status("Power", plyr->PowerCheat);
	print_cheat_status("Fly", plyr->FlyCheat);
	print_cheat_status("AuraStack", plyr->stack_cheat);
	SystemMessage(m_session, "%u cheats active, %u inactive.", active, inactive);

#undef print_cheat_status

	return true;
}

bool ChatHandler::HandleFlyCommand(const char* args, WorldSession* m_session)
{
	WorldPacket fly(SMSG_MOVE_SET_CAN_FLY, 13);
	
	Player* chr = getSelectedChar(m_session);
	
	if(!chr)
		chr = m_session->GetPlayer();
	
	chr->m_setflycheat = true;
	fly << chr->GetNewGUID();
	fly << uint32(2);
	chr->SendMessageToSet(&fly, true);
	BlueSystemMessage(chr->GetSession(), "Flying mode enabled.");
	return 1;
}

bool ChatHandler::HandleLandCommand(const char* args, WorldSession* m_session)
{
	WorldPacket fly(SMSG_MOVE_UNSET_CAN_FLY, 13);
	
	Player* chr = getSelectedChar(m_session);
	
	if(!chr)
		chr = m_session->GetPlayer();
	
	chr->m_setflycheat = false;
	fly << chr->GetNewGUID();
	fly << uint32(5);
	chr->SendMessageToSet(&fly, true);
	BlueSystemMessage(chr->GetSession(), "Flying mode disabled.");
	return 1;
}

bool ChatHandler::HandleDBReloadCommand(const char* args, WorldSession* m_session)
{
	char str[256];
	int ret = 0;

	if(!*args || strlen(args) < 3)
		return false;

	uint32 mstime = getMSTime();
	snprintf(str, 200, "%s%s initiated server-side reload of table `%s`. The server may experience some lag while this occurs.",
		MSG_COLOR_LIGHTRED, m_session->GetPlayer()->GetName(), args);
	sWorld.SendWorldText(str, 0);

	if (0 == stricmp(args, "spell_disable"))
	{
		objmgr.ReloadDisabledSpells();
		ret = 1;
	} else
	if (0 == stricmp(args, "spellfixes"))
	{
		objmgr.LoadSpellFixes();
		ret = 1;
	} else
	if (0 == stricmp(args, "vendors"))
	{
		objmgr.ReloadVendors();
		ret = 1;
	}
	else
	{
		ret = Storage_ReloadTable(args);
	}

	if (ret == 0)
		snprintf(str, 256, "%sDatabase reload failed.", MSG_COLOR_LIGHTRED);
	else
		snprintf(str, 256, "%sDatabase reload completed in %u ms.", MSG_COLOR_LIGHTBLUE, (unsigned int)(getMSTime() - mstime));
	sWorld.SendWorldText(str, 0);
	sGMLog.writefromsession(m_session, "reloaded table %s", args);
	return true;
}

bool ChatHandler::HandleFlySpeedCheatCommand(const char* args, WorldSession* m_session)
{
	float Speed = (float)atof(args);
	if(Speed == 0)
		Speed = 20;

	Player* plr = getSelectedChar(m_session);
	if(plr == 0)
		return true;

	BlueSystemMessage(m_session, "Setting the fly speed of %s to %f.", plr->GetName(), Speed);
	GreenSystemMessage(plr->GetSession(), "%s set your fly speed to %f.", m_session->GetPlayer()->GetName(), Speed);
	
	WorldPacket data(SMSG_FORCE_FLIGHT_SPEED_CHANGE, 16);
	data << plr->GetNewGUID();
	data << uint32(0) << Speed;
	plr->SendMessageToSet(&data, true);
	
	plr->m_flySpeed = Speed;
	
	return true;
}

bool ChatHandler::HandleModifyLevelCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(plr == 0) return true;

	uint32 Level = args ? atol(args) : 0;
	if(Level == 0 || Level > sWorld.GetMaxLevel(plr))
	{
		RedSystemMessage(m_session, "A level (numeric) is required to be specified after this command.");
		return true;
	}

	// Set level message
	BlueSystemMessage(m_session, "Setting the level of %s to %u.", plr->GetName(), Level);
	GreenSystemMessageToPlr(plr, "%s set your level to %u.", m_session->GetPlayer()->GetName(), Level);

	sGMLog.writefromsession(m_session, "used modify level on %s, level %u", plr->GetName(), Level);

	// lookup level information
	LevelInfo * Info = objmgr.GetLevelInfo(plr->getRace(), plr->getClass(), Level);
	if(Info == 0)
	{
		RedSystemMessage(m_session, "Levelup information not found.");
		return true;
	}

	plr->ApplyLevelInfo(Info, Level);
	return true;
}

bool ChatHandler::HandleAddTitleCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		RedSystemMessage(m_session, "This command requires selecting a player.");
		return true;
	}

	uint32 title = args ? atoi(args) : 0;
	if(title == 0 || title > TITLE_END)
	{
		RedSystemMessage(m_session, "A title number (numeric) is required to be specified after this command.");
		return true;
	}
	BlueSystemMessage(m_session, "Adding title number %u to %s.", title, plr->GetName());
	GreenSystemMessageToPlr(plr, "%s added title number %u to you.", m_session->GetPlayer()->GetName(), title);

	sGMLog.writefromsession(m_session, "added title number %u to %s", title, plr->GetName());
	plr->SetKnownTitle(title, true);
	return true;
}
bool ChatHandler::HandleRemoveTitleCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr){
		RedSystemMessage(m_session, "This command requires selecting a player.");
		return true;
	}
	uint32 title = args ? atoi(args) : 0;
	if(title == 0 || title > TITLE_END)
	{
		RedSystemMessage(m_session, "A title number (numeric) is required to be specified after this command.");
		return true;
	}
	BlueSystemMessage(m_session, "Removing title number %u from %s.", title, plr->GetName());
	GreenSystemMessageToPlr(plr, "%s removed title number %u from you.", m_session->GetPlayer()->GetName(), title);

	sGMLog.writefromsession(m_session, "removed title number %u from %s", title, plr->GetName());
	plr->SetKnownTitle(title, false);
	return true;
}
bool ChatHandler::HandleGetKnownTitlesCommand(const char* args, WorldSession* m_session){
	Player* plr = getSelectedChar(m_session, true);
	if(!plr){
		RedSystemMessage(m_session, "This command requires selecting a player.");
		return true;
	}
	std::stringstream ss;
	for(uint32 i=1;i<=TITLE_END;++i){
		if(plr->HasKnownTitle(i)){
			ss << i << " ";
		}
	}
	BlueSystemMessage(m_session, ss.str().c_str());
	return true;
}
bool ChatHandler::HandleSetChosenTitleCommand(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr){
		RedSystemMessage(m_session, "This command requires selecting a player.");
		return true;
	}
	uint32 title = args ? atoi(args) : 0;
	if(title == 0 || title > TITLE_END)
	{
		RedSystemMessage(m_session, "A title number (numeric) is required to be specified after this command.");
		return true;
	}
	BlueSystemMessage(m_session, "Setting title number %u for %s.", title, plr->GetName());
	GreenSystemMessageToPlr(plr, "%s set title number %u for you.", m_session->GetPlayer()->GetName(), title);

	sGMLog.writefromsession(m_session, "set title number %u for %s", title, plr->GetName());
	if(!plr->HasKnownTitle(title)){
		RedSystemMessage(m_session, "Selected player doesn't know this title.");
		return true;
	}
	plr->SetUInt32Value(PLAYER_CHOSEN_TITLE,title);
	return true;
}

bool ChatHandler::HandleCreatePetCommand(const char* args, WorldSession* m_session)
{

	if(!args || strlen(args) < 2)
	return false;

	uint32 Entry = atol(args);
	if(!Entry)
	return false;

	Player * player = m_session->GetPlayer();
	if(!player)
	return false;

	CreatureProto * pTemplate = CreatureProtoStorage.LookupEntry(Entry);
	CreatureInfo * pCreatureInfo = CreatureNameStorage.LookupEntry(Entry);
	if(!pTemplate || !pCreatureInfo)
	{
		RedSystemMessage(m_session, "Invalid creature spawn template: %u", Entry);
		return true;
	}

	if(player->GeneratePetNumber() == 0)
	return false;

	else if(player->GetSummon() || player->GetUnstabledPetNumber())
	return false;

	CreatureFamilyEntry *cf = dbcCreatureFamily.LookupEntry(pCreatureInfo->Family);
	if(cf && !cf->tameable)
		return false;

	Pet* pPet = objmgr.CreatePet();
	pPet->SetInstanceID(player->GetInstanceID());
	pPet->CreateAsSummon(Entry, pCreatureInfo, ((Creature*)pPet), TO_UNIT(player), NULL, 2, 0);

	sGMLog.writefromsession(m_session, "used create pet entry %u", Entry);
	return true;
}


#ifdef USE_SPECIFIC_AIAGENTS
//this is custom stuff !
bool ChatHandler::HandlePetSpawnAIBot(const char* args, WorldSession *m_session)
{
	if (!*args)
		return false;

	if( !m_session->GetPlayer() )
		return false; //wtf ?

	uint32 botprice = m_session->GetPlayer()->GetUInt32Value(UNIT_FIELD_LEVEL)*10000; //1 gold per level ?

	if( m_session->GetPlayer()->GetUInt32Value(PLAYER_FIELD_COINAGE) < botprice )
	{
		GreenSystemMessage(m_session, "You need a total of %u coins to afford a bot", botprice);
		return false;
	}

	uint8 botType = (uint8)atof((char*)args);

	if ( botType!=0 )
	{
		RedSystemMessage(m_session, "Incorrect value. Accepting value 0 only = healbot :)");
		return true;
	}

	uint32 Entry;
	char name[50];
	uint8 race = m_session->GetPlayer()->getRace();

	if( race == RACE_HUMAN || race == RACE_DWARF || race == RACE_NIGHTELF || race == RACE_GNOME || race == RACE_DRAENEI )
	{
		Entry = 1826;
		strcpy( name, "|cffff6060A_HealBot" );
	}
	else
	{
		Entry = 5473;
		strcpy( name, "|cffff6060H_HealBot" );
	}

	CreatureProto * pTemplate = CreatureProtoStorage.LookupEntry(Entry);
	CreatureInfo * pCreatureInfo = CreatureNameStorage.LookupEntry(Entry);
	if(!pTemplate || !pCreatureInfo)
	{
		RedSystemMessage(m_session, "Invalid creature spawn template: %u", Entry);
		return true;
	}
	Player* plr = m_session->GetPlayer();

	// spawn a creature of this id to create from
	Creature* pCreature = new Creature(HIGHGUID_UNIT ,1);//no need in guid
	CreatureSpawn * sp = new CreatureSpawn;
	sp->id = 1;
	sp->bytes = 0;
	sp->bytes2 = 0;
	sp->displayid = pCreatureInfo->Male_DisplayID;
	sp->emote_state = 0;
	sp->MountedDisplayID = 0;
	sp->entry = pCreatureInfo->Id;
	sp->factionid = pTemplate->Faction;
	sp->flags = 0;
	sp->form = 0;
	sp->movetype = 0;
	sp->o = plr->GetOrientation();
	sp->x = plr->GetPositionX();
	sp->y = plr->GetPositionY();
	sp->respawnNpcLink = 0;
	sp->channel_spell=sp->channel_target_creature=sp->channel_target_go=0;
	pCreature->Load(sp, (uint32)NULL, NULL);

	Pet *old_tame = plr->GetSummon();
	if(old_tame != NULL)
	{
		old_tame->Dismiss(true);
	}

	// create a pet from this creature
	Pet * pPet = objmgr.CreatePet();
	pPet->SetInstanceID(plr->GetInstanceID());
	pPet->SetMapId(plr->GetMapId());

	pPet->SetFloatValue ( OBJECT_FIELD_SCALE_X, pTemplate->Scale / 2); //we do not wish to block visualy other players
	AiAgentHealSupport *new_interface = new AiAgentHealSupport;
	pPet->ReplaceAIInterface( (AIInterface *) new_interface );
//	new_interface->Init(pPet,AITYPE_PET,MOVEMENTTYPE_NONE,plr); // i think this will get called automatically for pet

	pPet->CreateAsSummon(Entry, pCreatureInfo, pCreature, plr, NULL, 0x2, 0);

	pPet->Rename(name);

	//healer bot should not have any specific ations
	pPet->SetActionBarSlot(0,PET_SPELL_FOLLOW);
	pPet->SetActionBarSlot(1,PET_SPELL_STAY);
	pPet->SetActionBarSlot(2,0);
	pPet->SetActionBarSlot(3,0);
	pPet->SetActionBarSlot(4,0);
	pPet->SetActionBarSlot(5,0);
	pPet->SetActionBarSlot(6,0);
	pPet->SetActionBarSlot(7,0);
	pPet->SetActionBarSlot(8,0);
	pPet->SetActionBarSlot(9,0);
	pPet->SendSpellsToOwner();

	// remove the temp creature
	delete sp;
	delete pCreature;

	sGMLog.writefromsession(m_session, "used create an AI bot");
	return true;
}
#endif

bool ChatHandler::HandleAddPetSpellCommand(const char* args, WorldSession* m_session)
{
	Player* plr = m_session->GetPlayer();
	Pet* pPet = plr->GetSummon();
	if(pPet == 0)
	{
		RedSystemMessage(m_session, "You have no pet.");
		return true;
	}

	uint32 SpellId = atol(args);
	SpellEntry * spell = dbcSpell.LookupEntry(SpellId);
	if(!SpellId || !spell)
	{
		RedSystemMessage(m_session, "Invalid spell id requested.");
		return true;
	}

	pPet->AddSpell(spell, true);
	GreenSystemMessage(m_session, "Added spell %u to your pet.", SpellId);
	return true;
}

bool ChatHandler::HandleRemovePetSpellCommand(const char* args, WorldSession* m_session)
{
	Player* plr = m_session->GetPlayer();
	Pet* pPet = plr->GetSummon();
	if(pPet == 0)
	{
		RedSystemMessage(m_session, "You have no pet.");
		return true;
	}

	uint32 SpellId = atol(args);
	SpellEntry * spell = dbcSpell.LookupEntry(SpellId);
	if(!SpellId || !spell)
	{
		RedSystemMessage(m_session, "Invalid spell id requested.");
		return true;
	}

	pPet->RemoveSpell(SpellId);
	GreenSystemMessage(m_session, "Added spell %u to your pet.", SpellId);
	return true;
}

bool ChatHandler::HandleAddPetTalentPoints(const char* args, WorldSession* m_session)
{
	if(!args)
		return false;

	uint32 talentPointsToAdd = atol(args);

	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		plr = m_session->GetPlayer();
		if(!plr)
			return true;
	}
	Pet * pPet = plr->GetSummon();
	if(!pPet)
		return true;

	pPet->SetUnspentPetTalentPoints(pPet->GetUnspentPetTalentPoints() + talentPointsToAdd);
	plr->smsg_TalentsInfo(true);

	GreenSystemMessage(m_session, "Added %u talent points to your pet.", talentPointsToAdd);
	return true;
}

bool ChatHandler::HandleResetPetTalents(const char* args, WorldSession* m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
	{
		plr = m_session->GetPlayer();
		if(!plr)
			return true;
	}
	Pet * pPet = plr->GetSummon();
	if(!pPet)
		return true;

	pPet->ResetTalents(true);
	GreenSystemMessage(m_session, "Reset your pets talents.");
	return true;
}

bool ChatHandler::HandleRenamePetCommand(const char* args, WorldSession* m_session)
{
	Player* plr = m_session->GetPlayer();
	Pet* pPet = plr->GetSummon();
	if(pPet == 0)
	{
		RedSystemMessage(m_session, "You have no pet.");
		return true;
	}

	if(strlen(args) < 1)
	{
		RedSystemMessage(m_session, "You must specify a name.");
		return true;
	}

	GreenSystemMessage(m_session, "Renamed your pet to %s.", args);
	pPet->Rename(args);
	return true;
}

bool ChatHandler::HandleShutdownCommand(const char* args, WorldSession* m_session)
{
	uint32 shutdowntime = atol(args);
	if( !args )
		shutdowntime = 5;
	
	sGMLog.writefromsession(m_session, "initiated server shutdown timer %u sec", shutdowntime);
	sWorld.QueueShutdown(shutdowntime, SERVER_SHUTDOWN_TYPE_SHUTDOWN);
	return true;
}

bool ChatHandler::HandleAllowWhispersCommand(const char* args, WorldSession* m_session)
{
	if(args == 0 || strlen(args) < 2) return false;
	Player* plr = objmgr.GetPlayer(args, false);
	if(!plr)
	{
		RedSystemMessage(m_session, "Player not found.");
		return true;
	}

	m_session->GetPlayer()->gmTargets.insert(plr);
	BlueSystemMessage(m_session, "Now accepting whispers from %s.", plr->GetName());
	return true;
}

bool ChatHandler::HandleBlockWhispersCommand(const char* args, WorldSession* m_session)
{
	if(args == 0 || strlen(args) < 2) return false;
	Player* plr = objmgr.GetPlayer(args, false);
	if(!plr)
	{
		RedSystemMessage(m_session, "Player not found.");
		return true;
	}

	m_session->GetPlayer()->gmTargets.erase(plr);
	BlueSystemMessage(m_session, "Now blocking whispers from %s.", plr->GetName());
	return true;
}

bool ChatHandler::HandleAdvanceAllSkillsCommand(const char* args, WorldSession* m_session)
{
	uint32 amt = args ? atol(args) : 0;
	if(!amt)
	{
		RedSystemMessage(m_session, "An amount to increment is required.");
		return true;
	}

	Player* plr = getSelectedChar(m_session);
	if(!plr)
		return true;


	plr->_AdvanceAllSkills(amt);
	GreenSystemMessageToPlr(plr, "Advanced all your skill lines by %u points.", amt);
	sGMLog.writefromsession(m_session, "advanced all skills by %u on %s", amt, plr->GetName());
	return true;
}

bool ChatHandler::HandleKillByPlayerCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "A player's name is required.");
		return true;
	}

	sWorld.DisconnectUsersWithPlayerName(args,m_session);
	return true;
}

bool ChatHandler::HandleKillBySessionCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "A player's name is required.");
		return true;
	}

	sWorld.DisconnectUsersWithAccount(args,m_session);
	return true;
}

bool ChatHandler::HandleMassSummonCommand(const char* args, WorldSession* m_session)
{
	if( !m_session->GetPlayer()->m_massSummonEnabled )
	{
		m_session->GetPlayer()->BroadcastMessage("Please re-enter the command to confirm you wish to summon all players online to your position.");
		m_session->GetPlayer()->BroadcastMessage("This will reset in 30 seconds.");
		m_session->GetPlayer()->m_massSummonEnabled = true;
		sEventMgr.AddEvent( m_session->GetPlayer(), &Player::EventMassSummonReset, EVENT_RESET_CHAT_COMMAND, 30000, 0, 0);
	}

	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	Player* summoner = m_session->GetPlayer();
	Player* plr;
	uint32 c=0;
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); ++itr)
	{
		plr = itr->second;
		if(plr->GetSession() && plr->IsInWorld())
		{
			//plr->SafeTeleport(summoner->GetMapId(), summoner->GetInstanceID(), summoner->GetPosition());
			/* let's do this the blizz way */
			plr->SummonRequest(summoner, summoner->GetZoneId(), summoner->GetMapId(), summoner->GetInstanceID(), summoner->GetPosition());
			++c;
		}
	}
	sGMLog.writefromsession(m_session, "requested a mass summon of %u players.", c);
	objmgr._playerslock.ReleaseReadLock();
	m_session->GetPlayer()->m_massSummonEnabled = false;
	sEventMgr.RemoveEvents(m_session->GetPlayer(), EVENT_RESET_CHAT_COMMAND);
	return true;
}

bool ChatHandler::HandleCastAllCommand(const char* args, WorldSession* m_session)
{
	if(!args || strlen(args) < 2)
	{
		RedSystemMessage(m_session, "No spellid specified.");
		return true;
	}
	Player* plr;
	uint32 spellid = atol(args);
	SpellEntry * info = dbcSpell.LookupEntry(spellid);
	if(!info)
	{
		RedSystemMessage(m_session, "Invalid spell specified.");
		return true;
	}

	// this makes sure no moron casts a learn spell on everybody and wrecks the server
	for (int i = 0; i < 3; ++i)
	{
		if (info->Effect[i] == 36) //SPELL_EFFECT_LEARN_SPELL - 36
		{
			sGMLog.writefromsession(m_session, "used wrong / learnall castall command, spellid %u", spellid);
			RedSystemMessage(m_session, "Learn spell specified.");
			return true;
		}
	}

	sGMLog.writefromsession(m_session, "used castall command, spellid %u", spellid);

	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); ++itr)
	{
		plr = itr->second;
		if(plr->GetSession() && plr->IsInWorld())
		{
			if(plr->GetMapMgr() != m_session->GetPlayer()->GetMapMgr())
			{
				sEventMgr.AddEvent( TO_UNIT(plr), &Unit::EventCastSpell, TO_UNIT(plr), info, EVENT_PLAYER_CHECKFORCHEATS, 100, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
			}
			else
			{
				Spell* sp(new Spell(plr, info, true, NULLAURA));
				SpellCastTargets targets(plr->GetGUID());
				sp->prepare(&targets);
			}
		}
	}
	objmgr._playerslock.ReleaseReadLock();

	BlueSystemMessage(m_session, "Casted spell %u on all players!", spellid);
	return true;
}

bool ChatHandler::HandleNpcReturnCommand(const char* args, WorldSession* m_session)
{
	Creature* creature = getSelectedCreature(m_session);
	if(!creature || !creature->m_spawn) return true;

	// return to respawn coords
	float x = creature->m_spawn->x;
	float y = creature->m_spawn->y;
	float z = creature->m_spawn->z;

	// restart movement
	creature->GetAIInterface()->SetAIState(STATE_IDLE);
	creature->GetAIInterface()->WipeHateList();
	creature->GetAIInterface()->WipeTargetList();
	creature->GetAIInterface()->MoveTo(x, y, z);

	return true;
}

bool ChatHandler::HandleModPeriodCommand(const char* args, WorldSession * m_session)
{
	Transporter* trans = m_session->GetPlayer()->m_CurrentTransporter;
	if(trans == 0)
	{
		RedSystemMessage(m_session, "You must be on a transporter.");
		return true;
	}

	uint32 np = args ? atol(args) : 0;
	if(np == 0)
	{
		RedSystemMessage(m_session, "A time in ms must be specified.");
		return true;
	}

	trans->SetPeriod(np);
	BlueSystemMessage(m_session, "Period of %s set to %u.", trans->GetInfo()->Name, np);
	return true;
}

bool ChatHandler::HandleFormationLink1Command(const char* args, WorldSession * m_session)
{
	// set formation "master"
	Creature* pCreature = getSelectedCreature(m_session, true);
	if(pCreature == 0) return true;

	m_session->GetPlayer()->linkTarget = pCreature;
	BlueSystemMessage(m_session, "Linkup \"master\" set to %s.", pCreature->GetCreatureInfo()->Name);
	return true;
}

bool ChatHandler::HandleFormationLink2Command(const char* args, WorldSession * m_session)
{
	// set formation "slave" with distance and angle
	float ang, dist;
	if(*args == 0 || sscanf(args, "%f %f", &dist, &ang) != 2)
	{
		RedSystemMessage(m_session, "You must specify a distance and angle.");
		return true;
	}

	if(m_session->GetPlayer()->linkTarget == 0 || m_session->GetPlayer()->linkTarget->GetTypeId() != TYPEID_UNIT)
	{
		RedSystemMessage(m_session, "Master not selected. select the master, and use formationlink1.");
		return true;
	}

	Creature* slave = getSelectedCreature(m_session, true);
	if(slave == 0) return true;

	if( slave->GetAIInterface()->m_formationLinkTarget->m_spawn == NULL || slave->m_spawn == NULL )
		return false;

	slave->GetAIInterface()->m_formationFollowDistance = dist;
	slave->GetAIInterface()->m_formationFollowAngle = ang;
	slave->GetAIInterface()->m_formationLinkTarget = TO_CREATURE( m_session->GetPlayer()->linkTarget );
	slave->GetAIInterface()->m_formationLinkSqlId = slave->GetAIInterface()->m_formationLinkTarget->GetSQL_id();
	slave->GetAIInterface()->SetUnitToFollowAngle(ang);

	// add to db
	WorldDatabase.Execute("INSERT INTO creature_formations VALUES(%u, %u, '%f', '%f')", 
		slave->GetSQL_id(), slave->GetAIInterface()->m_formationLinkSqlId, ang, dist);

	BlueSystemMessage(m_session, "%s linked up to %s with a distance of %f at %f radians.", slave->GetCreatureInfo()->Name, 
		TO_CREATURE( m_session->GetPlayer()->linkTarget )->GetCreatureInfo()->Name, dist, ang );

	return true;
}

bool ChatHandler::HandleNpcFollowCommand(const char* args, WorldSession * m_session)
{
	Creature* creature = getSelectedCreature(m_session, true);
	if(!creature) return true;

	creature->GetAIInterface()->SetUnitToFollow(m_session->GetPlayer());
	return true;
}

bool ChatHandler::HandleFormationClearCommand(const char* args, WorldSession * m_session)
{
	Creature* c = getSelectedCreature(m_session, true);
	if(!c || c->m_spawn == NULL) return true;

	c->GetAIInterface()->m_formationLinkSqlId = 0;
	c->GetAIInterface()->m_formationLinkTarget = NULLCREATURE;
	c->GetAIInterface()->m_formationFollowAngle = 0.0f;
	c->GetAIInterface()->m_formationFollowDistance = 0.0f;
	c->GetAIInterface()->SetUnitToFollow(NULLUNIT);
	
	WorldDatabase.Execute("DELETE FROM creature_formations WHERE spawn_id=%u", c->GetSQL_id());
	return true;
}

bool ChatHandler::HandleNullFollowCommand(const char* args, WorldSession * m_session)
{
	Creature* c = getSelectedCreature(m_session, true);
	if(!c) return true;

	// restart movement
	c->GetAIInterface()->SetAIState(STATE_IDLE);
	c->GetAIInterface()->SetUnitToFollow(NULLUNIT);
	return true;
}

bool ChatHandler::HandleStackCheatCommand(const char* args, WorldSession * m_session)
{
	Player* plyr = getSelectedChar(m_session, true);
	if(!plyr)
		return true;

	bool val = plyr->stack_cheat;
	BlueSystemMessage(m_session, "%s aura stack cheat on %s.", val ? "Deactivating" : "Activating", plyr->GetName());
	GreenSystemMessageToPlr(plyr, "%s %s an aura stack cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plyr->stack_cheat = !val;
	sGMLog.writefromsession(m_session, "used stack cheat on %s", plyr->GetName());
	return true;
}

bool ChatHandler::HandleTriggerpassCheatCommand(const char* args, WorldSession * m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
		return true;

	bool val = plr->triggerpass_cheat;
	BlueSystemMessage(m_session, "%s areatrigger prerequisites immunity cheat on %s.", val ? "Deactivated" : "Activated", plr->GetName());
	GreenSystemMessageToPlr(plr, "%s %s areatrigger prerequisites immunity cheat on you.", m_session->GetPlayer()->GetName(), val ? "deactivated" : "activated");

	plr->triggerpass_cheat = !val;
	sGMLog.writefromsession(m_session, "used areatrigger cheat on %s", plr->GetName());
	return true;
}

bool ChatHandler::HandleResetSkillsCommand(const char* args, WorldSession * m_session)
{
	skilllineentry * se;
	Player* plr = getSelectedChar(m_session, true);
	if(!plr) return true;

	plr->_RemoveAllSkills();

	// Load skills from create info.
	PlayerCreateInfo * info = objmgr.GetPlayerCreateInfo(plr->getRace(), plr->getClass());
	if(!info) return true;

	for(std::list<CreateInfo_SkillStruct>::iterator ss = info->skills.begin(); ss!=info->skills.end(); ss++)
	{
		se = dbcSkillLine.LookupEntry(ss->skillid);
		if(se->type != SKILL_TYPE_LANGUAGE && ss->skillid && ss->currentval && ss->maxval)
			plr->_AddSkillLine(ss->skillid, ss->currentval, ss->maxval);
	}
	//Chances depend on stats must be in this order!
	plr->UpdateStats();
	plr->UpdateChances();
	plr->_UpdateMaxSkillCounts();
	plr->_AddLanguages(false);
	BlueSystemMessage(m_session, "Reset skills to default.");
	sGMLog.writefromsession(m_session, "reset skills of %s to default", plr->GetName());
	return true;
}

bool ChatHandler::HandlePlayerInfo(const char* args, WorldSession * m_session)
{
	Player* plr;
	if(strlen(args) >= 2) // char name can be 2 letters
	{
		plr = objmgr.GetPlayer(args, false);
		if(!plr)
		{
			RedSystemMessage(m_session, "Unable to locate player %s.", args);
			return true;
		}
	}
	else
		plr = getSelectedChar(m_session, true);

	if(!plr)
		return true;

	if(!plr->GetSession())
	{
		RedSystemMessage(m_session, "ERROR: this player hasn't got any session !");
		return true;
	}
	if(!plr->GetSession()->GetSocket())
	{
		RedSystemMessage(m_session, "ERROR: this player hasn't got any socket !");
		return true;
	}
	WorldSession* sess = plr->GetSession();

//	char* infos = new char[128];
	static const char* classes[12] =
	{"None","Warrior", "Paladin", "Hunter", "Rogue", "Priest", "Death Knight", "Shaman", "Mage", "Warlock", "None", "Druid"};
	static const char* races[12] =
	{"None","Human","Orc","Dwarf","Night Elf","Undead","Tauren","Gnome","Troll","None","Blood Elf","Draenei"};

	char playedLevel[64];
	char playedTotal[64];

	int seconds = (plr->GetPlayedtime())[0];
	int mins= 0;
	int hours= 0;
	int days= 0;
	if(seconds >= 60)
	{
		mins = seconds / 60;
		if(mins)
		{
			seconds -= mins*60;
			if(mins >= 60)
			{
				hours = mins / 60;
				if(hours)
				{
					mins -= hours*60;
					if(hours >= 24)
					{
						days = hours/24;
						if(days)
							hours -= days*24;
					}
				}
			}
		}
	}
	snprintf(playedLevel, 64, "[%d days, %d hours, %d minutes, %d seconds]", days, hours, mins, seconds);

	seconds = (plr->GetPlayedtime())[1];
	mins= 0;
	hours= 0;
	days= 0;
	if(seconds >= 60)
	{
		mins = seconds / 60;
		if(mins)
		{
			seconds -= mins*60;
			if(mins >= 60)
			{
				hours = mins / 60;
				if(hours)
				{
					mins -= hours*60;
					if(hours >= 24)
					{
						days = hours/24;
						if(days)
							hours -= days*24;
					}
				}
			}
		}
	}
	snprintf(playedTotal, 64, "[%d days, %d hours, %d minutes, %d seconds]", days, hours, mins, seconds);

	GreenSystemMessage(m_session, "%s[%u] is a %s %s %s", plr->GetName(), plr->GetLowGUID(),
		(plr->getGender()?"Female":"Male"), races[plr->getRace()], classes[plr->getClass()]);

	BlueSystemMessage(m_session, "%s has played %s at this level",(plr->getGender()?"She":"He"), playedLevel);
	BlueSystemMessage(m_session, "and %s overall", playedTotal);

	BlueSystemMessage(m_session, "%s is connecting from account '%s'[%u] with permissions '%s'",
		(plr->getGender()?"She":"He"), sess->GetAccountName().c_str(), sess->GetAccountId(), sess->GetPermissions());

	const char *client;
	if(sess->HasFlag(ACCOUNT_FLAG_XPACK_02))
		client = "Wrath of the Lich King";
	else if(sess->HasFlag(ACCOUNT_FLAG_XPACK_01))
		client = "The Burning Crusade";
	else
		client = "World of Warcraft";
 	BlueSystemMessage(m_session, "%s uses %s (build %u)", (plr->getGender()?"She":"He"),
		client, sess->GetClientBuild());

	BlueSystemMessage(m_session, "%s IP is '%s', and has a latency of %ums", (plr->getGender()?"Her":"His"),
		sess->GetSocket()->GetRemoteIP().c_str(), sess->GetLatency());

	return true;
}

bool ChatHandler::HandleGlobalPlaySoundCommand(const char* args, WorldSession * m_session)
{
	if(!*args) return false;
	uint32 sound = atoi(args);
	if(!sound) return false;

	WorldPacket data(SMSG_PLAY_SOUND, 4);
	data << sound;
	sWorld.SendGlobalMessage(&data, 0);
	BlueSystemMessage(m_session, "Broadcasted sound %u to server.", sound);
	sGMLog.writefromsession(m_session, "used play all command soundid %u", sound);
	return true;
}

bool ChatHandler::HandleIPBanCommand(const char * args, WorldSession * m_session)
{
	char * pIp = (char*)args;
	char * pDuration = strchr(pIp, ' ');
	if(pDuration == NULL)
		return false;

	string sip = string(pIp);

	string::size_type i = sip.find("/");
	if( i == string::npos )
		return false;

	*pDuration = 0;
	++pDuration;

	int32 timeperiod = GetTimePeriodFromString(pDuration);
	if(timeperiod < 1)
		return false;


	uint32 o1, o2, o3, o4;
	if ( sscanf(pIp, "%3u.%3u.%3u.%3u", (unsigned int*)&o1, (unsigned int*)&o2, (unsigned int*)&o3, (unsigned int*)&o4) != 4
			|| o1 > 255 || o2 > 255 || o3 > 255 || o4 > 255)
	{
		RedSystemMessage(m_session, "Invalid IPv4 address [%s]", pIp);
		return true;	// error in syntax, but we wont remind client of command usage
	}

	time_t expire_time;
	if ( timeperiod == 0)		// permanent ban
		expire_time = 0;
	else
		expire_time = UNIXTIME + (time_t)timeperiod;
	
	SystemMessage(m_session, "Adding [%s] to IP ban table, expires %s", pIp, (expire_time == 0)? "Never" : ctime( &expire_time ));
	sLogonCommHandler.IPBan_Add( pIp, (uint32)expire_time );
	sWorld.DisconnectUsersWithIP(pIp, m_session);
	sGMLog.writefromsession(m_session, "banned ip address %s, expires %s", pIp, (expire_time == 0)? "Never" : ctime( &expire_time ));
	return true;
}

bool ChatHandler::HandleIPUnBanCommand(const char * args, WorldSession * m_session)
{
	char ip[16] = {0};		// IPv4 address

	// we require at least one argument, the network address to unban
	if ( sscanf(args, "%15s", ip) < 1)
		return false;

	/**
	 * We can afford to be less fussy with the validty of the IP address given since
	 * we are only attempting to remove it.
	 * Sadly, we can only blindly execute SQL statements on the logonserver so we have
	 * no idea if the address existed and so the account/IPBanner cache requires reloading.
	 */

	SystemMessage(m_session, "Removing [%s] from IP ban table if it exists", ip);
	sLogonCommHandler.IPBan_Remove( ip );
	sGMLog.writefromsession(m_session, "unbanned ip address %s", ip);
	return true;
}

bool ChatHandler::HandleCreatureSpawnCommand(const char *args, WorldSession *m_session)
{
	if(m_session == NULL || m_session->GetPlayer() == NULL)
		return true;

	Player* plr = m_session->GetPlayer();

	uint32 entry, save;
	if( sscanf(args, "%u %u", &entry, &save) != 2 )
	{
		if( sscanf(args, "%u", &entry) != 1 )
			return false;

		save = (uint32)m_session->CanUseCommand('z');
	}

	if( save && !m_session->CanUseCommand('z') )
	{
		SystemMessage(m_session, "You are not allowed to save spawns.");
		return true;
	}

	CreatureProto * proto = CreatureProtoStorage.LookupEntry(entry);
	CreatureInfo * info = CreatureNameStorage.LookupEntry(entry);
	if(proto == NULL || info == NULL || (objmgr.SQLCheckExists("creature_names", "entry", entry) == NULL)
		|| (objmgr.SQLCheckExists("creature_proto", "entry", entry) == NULL))
	{
		RedSystemMessage(m_session, "Invalid entry id(%u).", entry);
		return true;
	}

	//Are we on a transporter?
	if(m_session->GetPlayer()->m_TransporterGUID != 0)
	{
		Player* pl = m_session->GetPlayer();
		Transporter* t = objmgr.GetTransporter(GUID_LOPART(pl->m_TransporterGUID));
		if(t)
		{
			WorldDatabase.Execute("INSERT INTO transport_creatures VALUES(%u, %u, '%f', '%f', '%f', '%f')", GUID_LOPART(pl->m_TransporterGUID), entry, pl->m_transportPosition->x, pl->m_transportPosition->y, pl->m_transportPosition->z, pl->GetOrientation());
			t->AddNPC(entry, pl->m_transportPosition->x, pl->m_transportPosition->y, pl->m_transportPosition->z, pl->GetOrientation());
			BlueSystemMessage(m_session, "Spawned crew-member %u on transport %u. You might need to relog.", entry, GUID_LOPART(pl->m_TransporterGUID));
			sGMLog.writefromsession(m_session, "spawned crew-member %u on transport %u.", entry, GUID_LOPART(pl->m_TransporterGUID));
			return true;
		}
		else
		{
			BlueSystemMessage(m_session, "Incorrect transportguid %u. Spawn has been denied and transport guid has been reset.", pl->m_TransporterGUID);
			pl->m_TransporterGUID = uint64(NULL);
			return true;
		}
	}
//	VehicleEntry * ve = dbcVehicle.LookupEntry( proto->vehicle_entry );
//	bool spVehicle = (ve && proto->vehicle_entry > 0) ? true : false;
	bool spVehicle = proto->vehicle_entry > 0 ? true : false;

	Creature* p = NULLCREATURE;
	p = spVehicle ? TO_CREATURE(m_session->GetPlayer()->GetMapMgr()->CreateVehicle(entry)) : m_session->GetPlayer()->GetMapMgr()->CreateCreature(entry);
	if(p == NULLCREATURE)
	{
		RedSystemMessage(m_session, "Could not create spawn.");
		return true;
	}

	CreatureSpawn * sp = NULL;
	ASSERT(p);
	if( save )
	{
		sp = new CreatureSpawn;
		info->GenerateModelId(&sp->displayid);
		sp->entry = entry;
		sp->form = 0;
		sp->id = objmgr.GenerateCreatureSpawnID();
		sp->movetype = 0;
		sp->x = plr->GetPositionX();
		sp->y = plr->GetPositionY();
		sp->z = plr->GetPositionZ();
		sp->o = plr->GetOrientation();
		sp->factionid = proto->Faction;
		sp->bytes = 0;
		sp->bytes1 = 0;
		sp->bytes2 = 0;
		sp->channel_spell = sp->channel_target_creature = sp->channel_target_go = 0;
		sp->phase = 1;
		sp->vehicle = proto->vehicle_entry;

		CreatureInfoExtra* extrainfo = CreatureInfoExtraStorage.LookupEntry(entry);
		if(extrainfo != NULL)
		{
			sp->emote_state = extrainfo->default_emote_state;
			sp->flags = extrainfo->default_flags;
			sp->stand_state = extrainfo->default_stand_state;
			sp->MountedDisplayID = extrainfo->default_MountedDisplayID;
		}
		else
		{
			sp->emote_state = 0;
			sp->flags = 0;
			sp->stand_state = 0;
			sp->MountedDisplayID = 0;
		}

		MapEntry* mapinfo = dbcMap.LookupEntry(m_session->GetPlayer()->GetMapId());
		bool raid = mapinfo->israid();
		p->Load(sp, (plr->IsInInstance() ? (raid ? plr->iInstanceType : plr->iRaidType) : MODE_5PLAYER_NORMAL), NULL);
	}
	else
	{
		p->Load(proto, m_session->GetPlayer()->GetPositionX(), m_session->GetPlayer()->GetPositionY(), m_session->GetPlayer()->GetPositionZ(), 0.0f);
	}
	
	p->PushToWorld(m_session->GetPlayer()->GetMapMgr());

	BlueSystemMessage(m_session, "Spawned a creature `%s` with entry %u at %f %f %f on map %u", info->Name, 
		entry, m_session->GetPlayer()->GetPositionX(), m_session->GetPlayer()->GetPositionY(), m_session->GetPlayer()->GetPositionZ(), m_session->GetPlayer()->GetMapId());

	// Save it to the database.
	if( save )
	{
		uint32 x = m_session->GetPlayer()->GetMapMgr()->GetPosX(m_session->GetPlayer()->GetPositionX());
		uint32 y = m_session->GetPlayer()->GetMapMgr()->GetPosY(m_session->GetPlayer()->GetPositionY());

		// Add spawn to map
		m_session->GetPlayer()->GetMapMgr()->GetBaseMap()->GetSpawnsListAndCreate(
			x,
			y)->CreatureSpawns.push_back(sp);

		p->SaveToDB();
	}

	sGMLog.writefromsession(m_session, "spawned a %s at %u %f %f %f", info->Name, m_session->GetPlayer()->GetMapId(),m_session->GetPlayer()->GetPositionX(), m_session->GetPlayer()->GetPositionY(), m_session->GetPlayer()->GetPositionZ() );

	return true;
}

bool ChatHandler::HandleCreatureRespawnCommand(const char *args, WorldSession *m_session)
{
	Creature * cCorpse = getSelectedCreature( m_session, false );

	if( cCorpse != NULL && cCorpse->IsCreature() && cCorpse->getDeathState() == CORPSE && cCorpse->GetSQL_id() != 0 )
	{
		sEventMgr.RemoveEvents( cCorpse, EVENT_CREATURE_RESPAWN );
		BlueSystemMessage( m_session, "Respawning a Creature: `%s` with entry: %u on map: %u sqlid: %u", cCorpse->GetCreatureInfo()->Name, cCorpse->GetEntry(), cCorpse->GetMapMgr()->GetMapId(), cCorpse->GetSQL_id() );
		sGMLog.writefromsession(m_session, "Respawned a Creature: `%s` with entry: %u on map: %u sqlid: %u", cCorpse->GetCreatureInfo()->Name, cCorpse->GetEntry(), cCorpse->GetMapMgr()->GetMapId(), cCorpse->GetSQL_id() );

		cCorpse->Despawn(0, 1);
		return true;
	}

	RedSystemMessage( m_session, "You must select a creature's corpse with a valid CreatureSpawn point." );
	return true;
}

bool ChatHandler::HandleRemoveItemCommand(const char * args, WorldSession * m_session)
{
	uint32 item_id;
	int32 count, ocount;
	int argc = sscanf(args, "%u %u", (unsigned int*)&item_id, (unsigned int*)&count);
	if(argc == 1)
		count = 1;
	else if(argc != 2 || !count)
		return false;

	ocount = count;
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
		return true;

	// loop until they're all gone.
	int32 loop_count = 0;
	int32 start_count = plr->GetItemInterface()->GetItemCount(item_id, true);
	int32 start_count2 = start_count;
	if(count > start_count)
		count = start_count;

	while(start_count >= count && (count > 0) && loop_count < 20)	 // Prevent a loop here.
	{
		plr->GetItemInterface()->RemoveItemAmt(item_id, count);
		start_count2 = plr->GetItemInterface()->GetItemCount(item_id, true);
		count -= (start_count - start_count2);
		start_count = start_count2;
		++loop_count;
	}

	sGMLog.writefromsession(m_session, "used remove item id %u count %u from %s", item_id, ocount, plr->GetName());
	BlueSystemMessage(m_session, "Removing %u copies of item %u from %s's inventory.", ocount, item_id, plr->GetName());
	BlueSystemMessage(plr->GetSession(), "%s removed %u copies of item %u from your inventory.", m_session->GetPlayer()->GetName(), ocount, item_id);
	return true;
}

bool ChatHandler::HandleForceRenameCommand(const char * args, WorldSession * m_session)
{
	// prevent buffer overflow
	if(strlen(args) > 100)
		return false;

	string tmp = string(args);
	PlayerInfo * pi = objmgr.GetPlayerInfoByName(tmp.c_str());
	if(pi == NULL)
	{
		RedSystemMessage(m_session, "Player with that name not found.");
		return true;
	}

	Player* plr = objmgr.GetPlayer((uint32)pi->guid);
	if(plr == NULLPLR)
	{
		CharacterDatabase.Execute("UPDATE characters SET forced_rename_pending = 1 WHERE guid = %u", (uint32)pi->guid);
	}
	else
	{
		plr->rename_pending = true;
		plr->SaveToDB(false);
		BlueSystemMessageToPlr(plr, "%s forced your character to be renamed next logon.", m_session->GetPlayer()->GetName());
	}

	CharacterDatabase.Execute("INSERT INTO banned_names VALUES('%s')", CharacterDatabase.EscapeString(string(pi->name)).c_str());
	GreenSystemMessage(m_session, "Forcing %s to rename his character next logon.", args);
	sGMLog.writefromsession(m_session, "forced %s to rename his charater (%u)", pi->name, pi->guid);
	return true;
}

bool ChatHandler::HandleRecustomizeCharCommand(const char * args, WorldSession * m_session)
{
	// prevent buffer overflow
	if(strlen(args) > 100)
		return false;

	string tmp = string(args);
	PlayerInfo * pi = objmgr.GetPlayerInfoByName(tmp.c_str());
	if(pi == NULL)
	{
		RedSystemMessage(m_session, "Player with that name not found.");
		return true;
	}

	Player* plr = objmgr.GetPlayer((uint32)pi->guid);
	if(plr == NULLPLR)
	{
		CharacterDatabase.Execute("UPDATE characters SET customizable = 1 WHERE guid = %u", (uint32)pi->guid);
	}
	else
	{
		plr->customizable = true;
		plr->SaveToDB(false);
		BlueSystemMessageToPlr(plr, "%s granted you a character recustomization, please relog.", m_session->GetPlayer()->GetName());
	}

	GreenSystemMessage(m_session, "Granting %s a character recustomization on his/her next character logon.", args);
	sGMLog.writefromsession(m_session, "Granted %s a character recustomization (%u)", pi->name, pi->guid);
	return true;
}

bool ChatHandler::HandleGetStandingCommand(const char * args, WorldSession * m_session)
{
	uint32 faction = atoi(args);
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
		return true;

	int32 standing = plr->GetStanding(faction);
	int32 bstanding = plr->GetBaseStanding(faction);

	GreenSystemMessage(m_session, "Reputation for faction %u:", faction);
	SystemMessage(m_session, "Base Standing: %d", bstanding);
	BlueSystemMessage(m_session, "Standing: %d", standing);
	return true;
}

bool ChatHandler::HandleSetStandingCommand(const char * args, WorldSession * m_session)
{
	uint32 faction;
	int32 standing;
	if(sscanf(args, "%u %d", (unsigned int*)&faction, (unsigned int*)&standing) != 2)
		return false;
	Player* plr = getSelectedChar(m_session, true);
	if(!plr)
		return true;

	BlueSystemMessage(m_session, "Setting standing of %u to %d on %s.", faction, standing, plr->GetName());
	plr->SetStanding(faction, standing);
	GreenSystemMessageToPlr(plr, "%s set your standing of faction %u to %d.", m_session->GetPlayer()->GetName(), faction, standing);
	sGMLog.writefromsession(m_session, "set standing of faction %u to %u for %s", faction,standing,plr->GetName());
	return true;
}

void SendHighlightedName(WorldSession * m_session, char* full_name, string& lowercase_name, string& highlight, uint32 id, bool item)
{
	char message[1024];
	char start[50];
	start[0] = message[0] = 0;

	snprintf(start, 50, "%s %u: %s", item ? "Item" : "Creature", (unsigned int)id, MSG_COLOR_WHITE);

	string::size_type hlen = highlight.length();
	string fullname = string(full_name);
	string::size_type offset = lowercase_name.find(highlight);
	string::size_type remaining = fullname.size() - offset - hlen;
	strcat(message, start);
	strncat(message, fullname.c_str(), offset);
	if(remaining > 0)
	{
		strcat(message, MSG_COLOR_LIGHTRED);
		strncat(message, (fullname.c_str() + offset), hlen);
		strcat(message, MSG_COLOR_WHITE);
		strncat(message, (fullname.c_str() + offset + hlen), remaining);
	}

	sChatHandler.SystemMessage(m_session, message);
}

bool ChatHandler::HandleLookupItemCommand(const char * args, WorldSession * m_session)
{
	if(!*args) return false;

	string x = string(args);
	HEARTHSTONE_TOLOWER(x);
	if(x.length() < 4)
	{
		RedSystemMessage(m_session, "Your search string must be at least 5 characters long.");
		return true;
	}

	StorageContainerIterator<ItemPrototype> * itr = ItemPrototypeStorage.MakeIterator();

	BlueSystemMessage(m_session, "Starting search of item `%s`...", x.c_str());
	uint32 t = getMSTime();
	ItemPrototype * it;
	uint32 count = 0;
	while(!itr->AtEnd())
	{
		it = itr->Get();
		if(FindXinYString(x, it->lowercase_name))
		{
			// Print out the name in a cool highlighted fashion
			char messagetext[500];
			string itemlink = it->ConstructItemLink(it->RandomPropId, it->RandomSuffixId, 1);
			snprintf(messagetext, 500, "Item %u %s",it->ItemId,itemlink.c_str());
			SystemMessage(m_session, messagetext);
			++count;
			if(count == 25)
			{
				RedSystemMessage(m_session, "More than 25 results returned. aborting.");
				break;
			}
		}
		
		if(!itr->Inc())
			break;
	}
	itr->Destruct();

	BlueSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

bool ChatHandler::HandleLookupObjectCommand(const char * args, WorldSession * m_session)
{
	if(!*args) return false;

	string x = string(args);
	HEARTHSTONE_TOLOWER(x);

	StorageContainerIterator<GameObjectInfo> * itr = GameObjectNameStorage.MakeIterator();

	GreenSystemMessage(m_session, "Starting search of object `%s`...", x.c_str());
	uint32 t = getMSTime();
	GameObjectInfo * i;
	uint32 count = 0;
	string y;
	string recout;
	while(!itr->AtEnd())
	{
		i = itr->Get();
		y = string(i->Name);
		HEARTHSTONE_TOLOWER(y);
		if(FindXinYString(x,y))
		{
			string Name;
			std::stringstream strm;
			strm<<i->ID;
			const char*objectName=i->Name;
			recout="|cfffff000Object ";
			recout+=strm.str();
			recout+="|cffFFFFFF: ";
			recout+=objectName;
			recout = recout + Name;
			SendMultilineMessage(m_session,recout.c_str());
			
			++count;
			if(count==25 || count > 25)
			{
				RedSystemMessage(m_session,"More than 25 results returned. aborting.");
				break;
			}
		}
		if(!itr->Inc()) break;
	}
	itr->Destruct();
	if (count== 0)
	{
		recout="|cff00ccffNo matches found.";
		SendMultilineMessage(m_session,recout.c_str());
	}
	BlueSystemMessage(m_session,"Search completed in %u ms.",getMSTime()-t);
	return true;
}

bool ChatHandler::HandleLookupSpellCommand(const char * args, WorldSession * m_session)
{
	if(!*args) return false;

	string x = string(args);
	HEARTHSTONE_TOLOWER(x);
	if(x.length() < 4)
	{
		RedSystemMessage(m_session, "Your search string must be at least 4 characters long.");
		return true;
	}

	GreenSystemMessage(m_session, "Starting search of spell `%s`...", x.c_str());
	uint32 t = getMSTime();
	uint32 count = 0;
	string recout;
	char itoabuf[12];
	for (uint32 index = 0; index < dbcSpell.GetNumRows(); ++index)
	{
		SpellEntry* spell = dbcSpell.LookupRow(index);
		string y = string(spell->Name);
		HEARTHSTONE_TOLOWER(y);
		if(FindXinYString(x, y))
 		{
 			sprintf((char*)itoabuf,"Spell %u ",spell->Id);
			recout = (const char*)itoabuf;
			recout += "|cff71d5ff|Hspell:";
			recout += (const char*)itoabuf;
			recout += "|h[";
			recout += spell->Name;
			recout += "]|h|r";
			SendMultilineMessage(m_session, recout.c_str());

			++count;
			if(count == 25)
			{
				RedSystemMessage(m_session, "More than 25 results returned. aborting.");
				break;
			}
		}
	}

	GreenSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

bool ChatHandler::HandleLookupCreatureCommand(const char * args, WorldSession * m_session)
{
	if(!*args) return false;

	string x = string(args);
	HEARTHSTONE_TOLOWER(x);
	if(x.length() < 4)
	{
		RedSystemMessage(m_session, "Your search string must be at least 4 characters long.");
		return true;
	}

	StorageContainerIterator<CreatureInfo> * itr = CreatureNameStorage.MakeIterator();

	GreenSystemMessage(m_session, "Starting search of creature `%s`...", x.c_str());
	uint32 t = getMSTime();
	CreatureInfo * i;
	uint32 count = 0;
	while(!itr->AtEnd())
	{
		i = itr->Get();
		if(FindXinYString(x, i->lowercase_name))
		{
			// Print out the name in a cool highlighted fashion
			SendHighlightedName(m_session, i->Name, i->lowercase_name, x, i->Id, false);

			++count;
			if(count == 25)
			{
				RedSystemMessage(m_session, "More than 25 results returned. aborting.");
				break;
			}
		}
		if(!itr->Inc())
			break;
	}
	itr->Destruct();

	GreenSystemMessage(m_session, "Search completed in %u ms.", getMSTime() - t);
	return true;
}

bool ChatHandler::HandleLookupTitleCommand(const char *args, WorldSession *m_session)
{
	if(!*args)
		return false;

	// Create a fatty array for title names.
	std::string TitleNames[TITLE_END] =
	{
		"",
		"Private", "Corporal", "Sergeant", "Master Sergeant", "Sergeant Major", "Knight", "Knight-Lieutenant", "Knight-Captain", "Knight-Champion", "Lieutenant Commander", "Commander", "Marshal", "Field Marshal", "Grand Marshal", "Scout", "Grunt",
		"Sergeant", "Senior Sergeant", "First Sergeant", "Stone Guard", "Blood Guard", "Legionnaire", "Centurion", "Champion", "Lieutenant General", "General", "Warlord", "High Warlord", "Gladiator", "Duelist", "Rival", "Challenger", "Scarab Lord",
		"Conqueror", "Justicar", "Champion of the Naaru", "Merciless Gladiator", "of the Shattered Sun", "Hand of A'dal", "Vengeful Gladiator", "Battlemaster", "the Seeker", "Elder", "Flame Warden", "Flame Keeper", "the Exalted", " the Explorer",
		"the Diplomat", "Brutal Gladiator", "Arena Master", "Salty", "Chef", "the Supreme", "of the Ten Storms", "of the Emerald Dream", "Deadly Gladiator", "Prophet", "the Malefic", "Stalker", "of the Ebon Blade", "Archmage", "Warbringer",
		"Assassin", "Grand Master Alchemist", "Grand Master Blacksmith", "Iron Chef", "Grand Master Enchanter", "Grand Master Engineer", "Doctor", "Grand Master Angler", "Grand Master Herbalist", "Grand Master Scribe", "Grand Master Jewelcrafter",
		"Grand Master Leatherworker", "Grand Master Miner", "Grand Master Skinner", "Grand Master Tailor", "of Quel'Thalas", "of Argus", "of Khaz Modan", "of Gnomergan", "the Lion Hearted", "Champion of Elune", "Hero of Orgrimmar", "Plainsrunner",
		"of the Darkspear", "the Forsaken", "the Magic Seeker", "Twilight Vanquisher", "Conqueror of Naxxramas", "Hero of Northrend", "the Hallowed", "Loremaster", "of the Alliance", "of the Horde", "the Flawless Victor", "Champion of the Frozen Wastes",
		"Ambassador", "the Argent Champion", "Guardian of Cenarius", "Brewmaster", "Merrymaker", "the Love Fool", "Matron", "Patron", "Obsidian Slayer", "of the Nightfall", "the Immortal", "the Undying", "Jenkins", "Bloodsail Admiral", "the Insane",
		"of the Exodar", "of Darnassus", "of Ironforge", "of Stormwind", "of Orgrimmar", "of Sen'jin", "of Silvermoon", "of Thunder Bluff", "of the Undercity", "the Noble", "Crusader", "Death's Demise", "the Celestial Defender", "Conqueror of Ulduar",
		"Champion of Ulduar", "Vanquisher", "Starcaller", "the Astral Walker", "Herald of the Titans", "Furious Gladiator", "the Pilgrim", "Relentless Gladiator", "Grand Crusader", "the Argent Defender", "the Patient", "the Light of Dawn",
		"Bane of the Fallen King", "the Kingslayer", "of the Ashen Verdict", "Wrathful Gladiator"
	};

	string x = string(args);
	HEARTHSTONE_TOLOWER(x);

	if(x.length() < 3)
	{
		RedSystemMessage(m_session, "Your search string must be at least 3 characters long.");
		return true;
	}
	GreenSystemMessage(m_session, "Initializing title finder.");
	for(uint16 i = 1; i < TITLE_END; ++i)
	{
		std::string lowercase_name = HEARTHSTONE_TOLOWER_RETURN(TitleNames[i]);
		if(FindXinYString(x, lowercase_name))
		{
			BlueSystemMessage(m_session, "Title %03u: %s", i, TitleNames[i].c_str());
		}
	}

	return true;
}

bool ChatHandler::HandleGORotate(const char * args, WorldSession * m_session)
{
	GameObject* go = m_session->GetPlayer()->m_GM_SelectedGO;
	if( !go )
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}

	/*float deg = (float)atof(args);
	if(deg == 0.0f)
		return false;*/

	// Convert the argument to radians
	//float rad = deg * (float(M_PI) / 180.0f);
	//uint32 rad = (float)atoi(args);
	float r1, r2, r3;
	sscanf(args, "%f %f %f", &r1, &r2, &r3);
//	uint32 rad = low & (high << 21);
	go->SetFloatValue(GAMEOBJECT_ROTATION_01, r1);
	go->SetFloatValue(GAMEOBJECT_ROTATION_02, r2);
	go->SetFloatValue(GAMEOBJECT_ROTATION_03, r3);
	go->UpdateRotation();


	//go->ModFloatValue(GAMEOBJECT_ROTATION, rad);
	//go->ModFloatValue(GAMEOBJECT_ROTATION_01, rad);
	//go->ModFloatValue(GAMEOBJECT_ROTATION_02, rad);
	//go->ModFloatValue(GAMEOBJECT_ROTATION_03, rad);
	//go->SetRotation(rad);
	/*go->SetUInt32Value(GAMEOBJECT_ROTATION, rad);
	go->SetUInt32Value(GAMEOBJECT_ROTATION_01, rad);
	go->SetUInt32Value(GAMEOBJECT_ROTATION_02, rad);
	go->SetUInt32Value(GAMEOBJECT_ROTATION_03, rad);*/
	/*float rad=(float)atof(args);
	go->SetFloatValue(GAMEOBJECT_ROTATION, rad);
	go->SetFloatValue(GAMEOBJECT_ROTATION_01, rad);
	go->SetFloatValue(GAMEOBJECT_ROTATION_02, rad);
	go->SetFloatValue(GAMEOBJECT_ROTATION_03, rad);*/
	//go->SaveToDB();

	// despawn and respawn
	//go->Despawn(1000);
	go->RemoveFromWorld(true);
	go->SetNewGuid(m_session->GetPlayer()->GetMapMgr()->GenerateGameobjectGuid());
	go->PushToWorld(m_session->GetPlayer()->GetMapMgr());
	uint32 ak = 51;
	//go->SetUInt32Value(GAMEOBJECT_ARTKIT, ak);
	return true;
}

bool ChatHandler::HandleGOMove(const char * args, WorldSession * m_session)
{
	// move the go to player's coordinates
	GameObject* go = m_session->GetPlayer()->m_GM_SelectedGO;
	if( !go )
	{
		RedSystemMessage(m_session, "No selected GameObject...");
		return true;
	}

	go->RemoveFromWorld(true);
	go->SetPosition(m_session->GetPlayer()->GetPosition());
	go->SetNewGuid(m_session->GetPlayer()->GetMapMgr()->GenerateGameobjectGuid());
	go->SaveToDB();
	go->PushToWorld(m_session->GetPlayer()->GetMapMgr());
	return true;
}

bool ChatHandler::HandleNpcPossessCommand(const char * args, WorldSession * m_session)
{
	Unit* pTarget = getSelectedChar(m_session, false);
	if(!pTarget)
	{
		pTarget = getSelectedCreature(m_session, false);
		if(pTarget && (pTarget->IsPet() || pTarget->GetUInt32Value(UNIT_FIELD_CREATEDBY) != 0))
			return false;
	}
		
	if(!pTarget)
	{
		RedSystemMessage(m_session, "You must select a player/creature.");
		return true;
	}

	m_session->GetPlayer()->Possess(pTarget);
	BlueSystemMessage(m_session, "Possessed "I64FMT, pTarget->GetGUID());
	return true;
}

bool ChatHandler::HandleNpcUnPossessCommand(const char * args, WorldSession * m_session)
{
	Creature* creature = getSelectedCreature(m_session);
	m_session->GetPlayer()->UnPossess();

	if( creature != NULL )
	{
			// restart movement
			creature->GetAIInterface()->SetAIState(STATE_IDLE);
			creature->GetAIInterface()->WipeHateList();
			creature->GetAIInterface()->WipeTargetList();

			if(creature->m_spawn)
			{
				// return to respawn coords
				float x = creature->m_spawn->x;
				float y = creature->m_spawn->y;
				float z = creature->m_spawn->z;
				creature->GetAIInterface()->MoveTo(x, y, z);
			}
	}
	GreenSystemMessage(m_session, "Removed any possessed targets.");
	return true;
}

bool ChatHandler::HandleRehashCommand(const char * args, WorldSession * m_session)
{
	/* rehashes */
	char msg[250];
	snprintf(msg, 250, "%s is rehashing config file.", m_session->GetPlayer()->GetName());
	sWorld.SendWorldWideScreenText(msg, 0);
	sWorld.SendWorldText(msg, 0);
	sWorld.Rehash(true);
	return true;
}

struct spell_thingo
{
	uint32 type;
	uint32 target;
};

list<SpellEntry*> aiagent_spells;
map<uint32, spell_thingo> aiagent_extra;

bool ChatHandler::HandleAIAgentDebugBegin(const char * args, WorldSession * m_session)
{
	QueryResult * result = WorldDatabase.Query("SELECT DISTINCT spell FROM ai_agents");
	if(!result) return false;

	do
	{
		SpellEntry * se = dbcSpell.LookupEntryForced(result->Fetch()[0].GetUInt32());
		if(se)
			aiagent_spells.push_back(se);
	} while(result->NextRow());
	delete result;

	for(list<SpellEntry*>::iterator itr = aiagent_spells.begin(); itr != aiagent_spells.end(); ++itr)
	{
		result = WorldDatabase.Query("SELECT * FROM ai_agents WHERE spell = %u", (*itr)->Id);
		ASSERT(result);
		spell_thingo t;
		t.type = result->Fetch()[6].GetUInt32();
		t.target = result->Fetch()[7].GetUInt32();
		delete result;
		aiagent_extra[(*itr)->Id] = t;
	}

	GreenSystemMessage(m_session, "Loaded %u spells for testing.", aiagent_spells.size());
	return true;
}

SpellCastTargets SetTargets(SpellEntry * sp, uint32 type, uint32 targettype, Unit* dst, Creature* src)
{
	SpellCastTargets targets;
	targets.m_unitTarget = 0;
	targets.m_itemTarget = 0;
	targets.m_srcX = 0;
	targets.m_srcY = 0;
	targets.m_srcZ = 0;
	targets.m_destX = 0;
	targets.m_destY = 0;
	targets.m_destZ = 0;

	if(targettype == TTYPE_SINGLETARGET)
	{
		targets.m_targetMask = 2;
		targets.m_unitTarget = dst->GetGUID();
	}
	else if(targettype == TTYPE_SOURCE)
	{
		targets.m_targetMask = 32;
		targets.m_srcX = src->GetPositionX();
		targets.m_srcY = src->GetPositionY();
		targets.m_srcZ = src->GetPositionZ();
	}
	else if(targettype == TTYPE_DESTINATION)
	{
		targets.m_targetMask = 64;
		targets.m_destX = dst->GetPositionX();
		targets.m_destY = dst->GetPositionY();
		targets.m_destZ = dst->GetPositionZ();
	}

	return targets;
};

bool ChatHandler::HandleAIAgentDebugContinue(const char * args, WorldSession * m_session)
{
	uint32 count = atoi(args);
	if(!count) return false;

	Creature* pCreature = getSelectedCreature(m_session, true);
	if(!pCreature) return true;

	Player* pPlayer = m_session->GetPlayer();

	for(uint32 i = 0; i < count; ++i)
	{
		if(!aiagent_spells.size())
			break;

		SpellEntry * sp = *aiagent_spells.begin();
		aiagent_spells.erase(aiagent_spells.begin());
		BlueSystemMessage(m_session, "Casting %u, "MSG_COLOR_SUBWHITE"%u remaining.", sp->Id, aiagent_spells.size());

		map<uint32, spell_thingo>::iterator it = aiagent_extra.find(sp->Id);
		ASSERT( it != aiagent_extra.end() );

		SpellCastTargets targets;
		if(it->second.type == STYPE_BUFF)
			targets = SetTargets(sp, it->second.type, it->second.type, pCreature, pCreature );
		else
			targets = SetTargets(sp, it->second.type, it->second.type, pPlayer, pCreature );

		pCreature->GetAIInterface()->CastSpell(pCreature, sp, targets);
	}

	if(!aiagent_spells.size())
		RedSystemMessage(m_session, "Finished.");
	/*else
		BlueSystemMessage(m_session, "Got %u remaining.", aiagent_spells.size());*/
	return true;
}

bool ChatHandler::HandleAIAgentDebugSkip(const char * args, WorldSession * m_session)
{
	uint32 count = atoi(args);
	if(!count)
		return false;

	for(uint32 i = 0; i < count; ++i)
	{
		if(!aiagent_spells.size())
			break;

		aiagent_spells.erase(aiagent_spells.begin());
	}
	BlueSystemMessage(m_session, "Erased %u spells.", count);
	return true;
}

bool ChatHandler::HandleRenameGuildCommand(const char* args, WorldSession *m_session)
{
	Player* ptarget = getSelectedChar(m_session);
	
	if(!*args || !ptarget)
		return false;
		
	char* newname = (char*)args;

	Guild* guild = ptarget->GetGuild();
	if(guild == NULL)
	{
		RedSystemMessage(m_session, "Target is not in a guild.");
		return true;
	}

	guild->ChangeGuildName(newname);
	BlueSystemMessage(ptarget->GetSession(), "The Name of your Guild has been changed to %s, please relog.", newname);
	return true;	
}

//People seem to get stuck in guilds from time to time. This should be helpful. -DGM
bool ChatHandler::HandleGuildRemovePlayerCommand(const char* args, WorldSession *m_session)
{
	Player* plr = getSelectedChar(m_session);
	if(!plr || (plr && (plr->GetGuild() == NULL))) 
		return false;

	plr->GetGuild()->RemoveGuildMember(plr->m_playerInfo,m_session);
	GreenSystemMessage(m_session, "Member removed succesfully.");
	return true;
}

//-DGM
bool ChatHandler::HandleGuildDisbandCommand(const char* args, WorldSession *m_session)
{
	Player* plr = getSelectedChar(m_session);
	if(!plr || (plr && (plr->GetGuild() == NULL))) 
		return false;
	
	plr->GetGuild()->Disband();
	GreenSystemMessage(m_session,"Guild disbanded succesfully.");
	return true;
}

//-DGM
bool ChatHandler::HandleGuildMembersCommand(const char* args, WorldSession *m_session)
{
	Player* plr = getSelectedChar(m_session);
	if(!plr || (plr && (plr->GetGuild() == NULL))) 
		return false;
	
	plr->GetGuild()->ListGuildMembers(m_session);	
	return true;
}

bool ChatHandler::HandleCreateArenaTeamCommands(const char * args, WorldSession * m_session)
{
	uint32 arena_team_type;
	char name[1000];
	uint32 real_type;
	Player* plr = getSelectedChar(m_session, true);
	if(sscanf(args, "%u %s", &arena_team_type, name) != 2)
	{
		SystemMessage(m_session, "Invalid syntax.");
		return true;
	}

	switch(arena_team_type)
	{
	case 2:
		real_type= 0;
		break;
	case 3:
		real_type=1;
		break;
	case 5:
		real_type=2;
		break;
	default:
		SystemMessage(m_session, "Invalid arena team type specified.");
		return true;
	}

	if(!plr)
		return true;

	if(plr->m_playerInfo->arenaTeam[real_type] != NULL)
	{
		SystemMessage(m_session, "Already in an arena team of that type.");
		return true;
	}

	ArenaTeam * t = new ArenaTeam(real_type,objmgr.GenerateArenaTeamId());
	t->m_emblemStyle=22;
	t->m_emblemColour=4292133532UL;
	t->m_borderColour=4294931722UL;
	t->m_borderStyle=1;
	t->m_backgroundColour=4284906803UL;
	t->m_leader=plr->GetLowGUID();
	t->m_name = string(name);
	t->AddMember(plr->m_playerInfo);
	objmgr.AddArenaTeam(t);
	SystemMessage(m_session, "created arena team.");
	return true;
}

bool ChatHandler::HandleWhisperBlockCommand(const char * args, WorldSession * m_session)
{
	if(m_session->GetPlayer()->bGMTagOn)
		return false;

	m_session->GetPlayer()->bGMTagOn = true;
	return true;
}

bool ChatHandler::HandleDispelAllCommand(const char * args, WorldSession * m_session)
{
	uint32 pos=0;
	if(*args)
		pos=atoi(args);

	Player* plr;

	sGMLog.writefromsession(m_session, "used dispelall command, pos %u", pos);

	PlayerStorageMap::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); ++itr)
	{
		plr = itr->second;
		if(plr->GetSession() && plr->IsInWorld())
		{
			if(plr->GetMapMgr() != m_session->GetPlayer()->GetMapMgr())
			{
				sEventMgr.AddEvent( TO_UNIT( plr ), &Unit::DispelAll, pos ? true : false, EVENT_PLAYER_CHECKFORCHEATS, 100, 1,EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT );
			}
			else
			{
				plr->DispelAll(pos?true:false);
			}
		}
	}
	sGMLog.writefromsession(m_session, "used mass dispel");
	objmgr._playerslock.ReleaseReadLock();

	BlueSystemMessage(m_session, "Dispel action done.");
	return true;
}

bool ChatHandler::HandleShowItems(const char * args, WorldSession * m_session)
{
	Player* plr = getSelectedChar(m_session, true);
	if(!plr) return true;

	ItemIterator itr(plr->GetItemInterface());
	itr.BeginSearch();
	for(; !itr.End(); itr.Increment())
	{
		SystemMessage(m_session, "Item %s count %u", (*itr)->GetProto()->Name1, (*itr)->GetUInt32Value(ITEM_FIELD_STACK_COUNT));
	}
	itr.EndSearch();

	SkillIterator itr2(plr);
	itr2.BeginSearch();
	for(; !itr2.End(); itr2.Increment())
	{
		SystemMessage(m_session, "Skill %u %u/%u", itr2->Skill->id, itr2->CurrentValue, itr2->MaximumValue);
	}
	itr2.EndSearch();

	return true;
}

bool ChatHandler::HandleCollisionTestIndoor(const char * args, WorldSession * m_session)
{
	Player* plr = m_session->GetPlayer();
	if(plr == NULL)
	{
		SystemMessage(m_session, "Invalid target.");
		return true;
	}

	if (plr->GetMapMgr()->IsCollisionEnabled())
	{
		const LocationVector & loc = plr->GetPosition();
		bool res = CollideInterface.IsIndoor(plr->GetMapId(), loc.x, loc.y, loc.z + 2.0f);
		SystemMessage(m_session, "Result was: %s.", res ? "indoors" : "outside");
		return true;
	}
	else
	{
		SystemMessage(m_session, "Hearthstone was not compiled with collision support.");
		return true;
	}
}

bool ChatHandler::HandleCollisionTestLOS(const char * args, WorldSession * m_session)
{
	if (sWorld.Collision)
	{
		Object* pObj = NULLOBJ;
		Creature* pCreature = getSelectedCreature(m_session, false);
		Player* pPlayer = getSelectedChar(m_session, false);
		if(pCreature)
			pObj = pCreature;
		else if(pPlayer)
			pObj = pPlayer;

		if(pObj == NULL)
		{
			SystemMessage(m_session, "Invalid target.");
			return true;
		}

		const LocationVector & loc2 = pObj->GetPosition();
		const LocationVector & loc1 = m_session->GetPlayer()->GetPosition();
		bool res = CollideInterface.CheckLOS(pObj->GetMapId(), loc1.x, loc1.y, loc1.z, loc2.x, loc2.y, loc2.z);
		bool res2 = CollideInterface.CheckLOS(pObj->GetMapId(), loc1.x, loc1.y, loc1.z+2.0f, loc2.x, loc2.y, loc2.z+2.0f);
		bool res3 = CollideInterface.CheckLOS(pObj->GetMapId(), loc1.x, loc1.y, loc1.z+5.0f, loc2.x, loc2.y, loc2.z+5.0f);
		SystemMessage(m_session, "Result was: %s %s %s.", res ? "in LOS" : "not in LOS", res2 ? "in LOS" : "not in LOS", res3 ? "in LOS" : "not in LOS");
		return true;
	}
	else
	{
		SystemMessage(m_session, "Hearthstone was not compiled with collision support.");
		return true;
	}
}

bool ChatHandler::HandleCollisionGetHeight(const char * args, WorldSession * m_session)
{
	Player* plr = m_session->GetPlayer();
	if(plr == NULL)
	{
		SystemMessage(m_session, "Invalid target.");
		return true;
	}

	if (plr->GetMapMgr()->IsCollisionEnabled())
	{
		float z = CollideInterface.GetHeight(plr->GetMapId(), plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ() + 2.0f);
		float z2 = CollideInterface.GetHeight(plr->GetMapId(), plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ() + 5.0f);
		float z3 = CollideInterface.GetHeight(plr->GetMapId(), plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ());

		SystemMessage(m_session, "Results were: %f %f %f", z, z2, z3);
		return true;
	}
	else
	{
		SystemMessage(m_session, "Hearthstone was not compiled with collision support.");
		return true;
	}
}

bool ChatHandler::HandleFixScaleCommand(const char * args, WorldSession * m_session)
{
	Creature * pCreature = getSelectedCreature(m_session, true);
	if( pCreature == NULL )
		return true;

	float sc = (float)atof(args);
	uint32 model;
	uint32 gender = pCreature->creature_info->GenerateModelId(&model);
	pCreature->setGender(gender);

	if(sc < 0.1f)
	{
		sc = GetScale( dbcCreatureDisplayInfo.LookupEntry( model ));
		SystemMessage(m_session, "Using scale %f from DBCDisplayInfo.",sc);
	}
	else
		SystemMessage(m_session, "Scale override set to %f (DBCDisplayInfo = %f).",sc,GetScale( dbcCreatureDisplayInfo.LookupEntry( model )));


	pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, sc);
	pCreature->proto->Scale = sc;
	WorldDatabase.Execute("UPDATE creature_proto SET scale = '%f' WHERE entry = %u", sc, pCreature->GetEntry());
	return true;
}

bool ChatHandler::HandleAddTrainerSpellCommand( const char * args, WorldSession * m_session )
{
	Creature * pCreature = getSelectedCreature(m_session, true);
	if( pCreature == NULL )
		return true;

	uint32 spellid, cost, reqspell, reqlevel, delspell;
	if( sscanf(args, "%u %u %u %u %u", &spellid, &cost, &reqspell, &reqlevel, &delspell) != 5 )
		return false;

	Trainer * pTrainer =  pCreature->GetTrainer();
	if( pTrainer == NULL )
	{
		RedSystemMessage(m_session, "Target is not a trainer.");
		return true;
	}

	SpellEntry* pSpell = dbcSpell.LookupEntryForced(spellid);
	if(pSpell== NULL)
	{
		RedSystemMessage(m_session, "Invalid spell.");
		return true;
	}

	if( pSpell->Effect[0] == SPELL_EFFECT_INSTANT_KILL || pSpell->Effect[1] == SPELL_EFFECT_INSTANT_KILL || pSpell->Effect[2] == SPELL_EFFECT_INSTANT_KILL )
	{
		RedSystemMessage(m_session, "No. You're not doing that.");
		return true;
	}

	TrainerSpell sp;
	sp.Cost = cost;
	sp.IsProfession = false;
	sp.pLearnSpell = pSpell;
	sp.pCastRealSpell = NULL;
	sp.pCastSpell = NULL;
	sp.RequiredLevel = reqlevel;
	sp.RequiredSpell = reqspell;
	sp.DeleteSpell = delspell;

	pTrainer->Spells.push_back(sp);
	pTrainer->SpellCount++;

	SystemMessage(m_session, "Added spell %u (%s) to trainer.", pSpell->Id, pSpell->Name);
	sGMLog.writefromsession(m_session, "added spell %u to trainer %u", spellid, pCreature->GetEntry());
	WorldDatabase.Execute("INSERT INTO trainer_spells VALUES(%u, %u, %u, %u, %u, %u, %u, %u, %u, %u)", 
		pCreature->GetEntry(), (int)0, pSpell->Id, cost, reqspell, (int)0, (int)0, reqlevel, delspell, (int)0);

	return true;
}

bool ChatHandler::HandleClearBonesCommand(const char *args, WorldSession *m_session)
{
	Player* p = m_session->GetPlayer();
	sGMLog.writefromsession(m_session, "cleared bones on map %u at %f %f %f", p->GetMapId(), p->GetPositionX(), p->GetPositionY(), p->GetPositionZ());

	Object::InRangeSet::iterator itr;
	Object* obj;

	for( itr = p->GetInRangeSetBegin(); itr != p->GetInRangeSetEnd(); ++itr)
	{
		obj = *itr;
		if(!obj)
			continue;

		if( obj->GetTypeId() == TYPEID_CORPSE && TO_CORPSE(obj)->GetCorpseState() == CORPSE_STATE_BONES )
			TO_CORPSE(obj)->Despawn();
	}

	SystemMessage(m_session, "Completed.");
	return true;
}

bool ChatHandler::HandleClearCorpsesCommand(const char *args, WorldSession *m_session)
{
	Player* p = m_session->GetPlayer();
	sGMLog.writefromsession(m_session, "cleared corpses on map %u at %f %f %f", p->GetMapId(), p->GetPositionX(), p->GetPositionY(), p->GetPositionZ());

	Object::InRangeSet::iterator itr;
	Object* obj;

	for( itr = p->GetInRangeSetBegin(); itr != p->GetInRangeSetEnd(); ++itr)
	{
		obj = *itr;
		if(!obj)
			continue;

		if( obj->GetTypeId() == TYPEID_CORPSE && TO_CORPSE(obj)->GetCorpseState() == CORPSE_STATE_BODY )
			TO_CORPSE(obj)->Despawn();
	}

	SystemMessage(m_session, "Completed.");
	return true;
}

bool ChatHandler::HandleMultiMuteCommand(const char *args, WorldSession *m_session)
{
	vector<string> real_args = StrSplit(string(args), " ");
	if( real_args.size() < 3 )
		return false;

	const char *reason = real_args[0].c_str();
	int32 timespan = GetTimePeriodFromString(real_args[1].c_str());
	if( timespan <= 0 )
		return false;

	string tsstr = ConvertTimeStampToDataTime((uint32)timespan+(uint32)UNIXTIME);
	uint32 i;
	char msg[200];

	for(i = 2; i < real_args.size(); ++i)
	{
		Player* pPlayer = objmgr.GetPlayer(real_args[i].c_str(), false);
		if( pPlayer == NULL )
		{
			SystemMessage(m_session, "Could not find player, %s.\n", real_args[i].c_str());
			continue;
		}

		pPlayer->GetSession()->SystemMessage("Your voice has been muted until %s by a GM. Until this time, you will not be able to speak in any form. Reason: %s", tsstr.c_str(), reason);
		sLogonCommHandler.Account_SetMute(pPlayer->GetSession()->GetAccountNameS(), (uint32)timespan + (uint32)UNIXTIME);
		sGMLog.writefromsession(m_session, "muted account %s until %s", pPlayer->GetSession()->GetAccountNameS(), ConvertTimeStampToDataTime((uint32)timespan+(uint32)UNIXTIME).c_str());

		snprintf(msg, 200, "%s%s was muted by %s (%s)", MSG_COLOR_WHITE, pPlayer->GetName(), m_session->GetPlayer()->GetName(), reason);
		sWorld.SendWorldText(msg, NULL);		
	}

	return true;
}

bool ChatHandler::HandleMultiKickCommand(const char *args, WorldSession *m_session)
{
	vector<string> real_args = StrSplit(string(args), " ");
	if( real_args.size() < 2 )
		return false;

	const char *reason = real_args[0].c_str();
	uint32 i;
	char msg[200];

	for(i = 1; i < real_args.size(); ++i)
	{
		Player* pPlayer = objmgr.GetPlayer(real_args[i].c_str(), false);
		if( pPlayer == NULL )
		{
			SystemMessage(m_session, "Could not find player, %s.\n", real_args[i].c_str());
			continue;
		}

		snprintf(msg, 200, "%s%s was kicked by %s (%s)", MSG_COLOR_WHITE, pPlayer->GetName(), m_session->GetPlayer()->GetName(), reason);
		sWorld.SendWorldText(msg, NULL);		
		pPlayer->Kick(6000);
	}

	return true;
}

bool ChatHandler::HandleMultiBanCommand(const char *args, WorldSession *m_session)
{
	vector<string> real_args = StrSplit(string(args), " ");
	if( real_args.size() < 3 )
		return false;

	const char *reason = real_args[0].c_str();
	int32 timespan = GetTimePeriodFromString(real_args[1].c_str());
	if( timespan <= 0 )
		return false;

	string tsstr = ConvertTimeStampToDataTime((uint32)timespan+(uint32)UNIXTIME);
	uint32 i;
	char msg[200];

	for(i = 2; i < real_args.size(); ++i)
	{
		Player* pPlayer = objmgr.GetPlayer(real_args[i].c_str(), false);
		if( pPlayer == NULL )
		{
			SystemMessage(m_session, "Could not find player, %s.\n", real_args[i].c_str());
			continue;
		}

		pPlayer->GetSession()->SystemMessage("Your have been character banned until %s by a GM. Until this time, you will not be able login on this character. Reason was: %s", tsstr.c_str(), reason);
		pPlayer->SetBanned((uint32)timespan+(uint32)UNIXTIME, real_args[0]);
		pPlayer->Kick(15000);
		sGMLog.writefromsession(m_session, "banned player %s until %s for %s", pPlayer->GetName(), ConvertTimeStampToDataTime((uint32)timespan+(uint32)UNIXTIME).c_str(), reason);

		snprintf(msg, 200, "%s%s was banned by %s (%s)", MSG_COLOR_WHITE, pPlayer->GetName(), m_session->GetPlayer()->GetName(), reason);
		sWorld.SendWorldText(msg, NULL);		
	}

	return true;
}

bool ChatHandler::HandleMultiAccountBanCommand(const char *args, WorldSession *m_session)
{
	vector<string> real_args = StrSplit(string(args), " ");
	if( real_args.size() < 3 )
		return false;

	const char *reason = real_args[0].c_str();
	int32 timespan = GetTimePeriodFromString(real_args[1].c_str());
	if( timespan <= 0 )
		return false;

	string tsstr = ConvertTimeStampToDataTime((uint32)timespan+(uint32)UNIXTIME);
	uint32 i;
	char msg[200];

	for(i = 2; i < real_args.size(); ++i)
	{
		Player* pPlayer = objmgr.GetPlayer(real_args[i].c_str(), false);
		if( pPlayer == NULL )
		{
			SystemMessage(m_session, "Could not find player, %s.\n", real_args[i].c_str());
			continue;
		}

		pPlayer->GetSession()->SystemMessage("Your have been account banned until %s by a GM. Until this time, you will not be able to log in on this account. Reason: %s", tsstr.c_str(), reason);
		sLogonCommHandler.Account_SetBanned(pPlayer->GetSession()->GetAccountNameS(), (uint32)timespan + (uint32)UNIXTIME, reason);
		sGMLog.writefromsession(m_session, "banned account %s until %s", pPlayer->GetSession()->GetAccountNameS(), ConvertTimeStampToDataTime((uint32)timespan+(uint32)UNIXTIME).c_str());

		snprintf(msg, 200, "%s%s was account banned by %s (%s)", MSG_COLOR_WHITE, pPlayer->GetName(), m_session->GetPlayer()->GetName(), reason);
		sWorld.SendWorldText(msg, NULL);	
		pPlayer->Kick(15000);
	}

	return true;
}

bool ChatHandler::HandleEnableAH(const char *args, WorldSession *m_session)
{
	BlueSystemMessage(m_session, "Auction House Enabled, staff has been alerted.");
	sWorld.SendMessageToGMs(m_session, "%s has disabled the auction house", (m_session->GetPlayer() ? m_session->GetPlayer()->GetName() : m_session->GetAccountNameS()));
	sWorld.AHEnabled = true;
	return true;
}

bool ChatHandler::HandleDisableAH(const char *args, WorldSession *m_session)
{
	BlueSystemMessage(m_session, "Auction house Disabled, staff has been alerted.");
	sWorld.SendMessageToGMs(m_session, "%s has enabled the auction house", (m_session->GetPlayer() ? m_session->GetPlayer()->GetName() : m_session->GetAccountNameS()));
	sWorld.AHEnabled = false;
	return true;
}

bool ChatHandler::HandleForceStartWintergrasp(const char *args, WorldSession *m_session)
{
	if(sWorld.wg_enabled == false)
		return false;

	if(sWintergraspI.GetWintergrasp() == NULL)
	{
		BlueSystemMessage(m_session, "Wintergrasp Forced to start, staff has been alerted.");
		sWorld.SendMessageToGMs(m_session, "%s has force started Wintergrasp", (m_session->GetPlayer() ? m_session->GetPlayer()->GetName() : m_session->GetAccountNameS()));
		sWintergraspI.forcestart_WG = true;
		return true;
	}
	return false;
}

bool ChatHandler::HandleForceEndWintergrasp(const char *args, WorldSession *m_session)
{
	if(sWorld.wg_enabled == false)
		return false;

	if(sWintergraspI.GetWintergrasp() != NULL)
	{
		BlueSystemMessage(m_session, "Wintergrasp ended, staff has been alerted.");
		sWorld.SendMessageToGMs(m_session, "%s has forced wintergrasp to end.", (m_session->GetPlayer() ? m_session->GetPlayer()->GetName() : m_session->GetAccountNameS()));
		sWintergraspI.GetWintergrasp()->End();
		return true;
	}
	return false;
}