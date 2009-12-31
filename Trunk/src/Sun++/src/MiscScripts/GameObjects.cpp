/*
 * Moon++ Scripts for Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2007-2008 Moon++ Team <http://www.moonplusplus.info/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see <http://www.gnu.org/licenses/>.
 */

#include "StdAfx.h"
#include "Setup.h"
#include "../Common/EasyFunctions.h"

class OrbOfCommand : public GameObjectAIScript
{
public:
	OrbOfCommand(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new OrbOfCommand(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(pPlayer->HasFinishedQuest(7761) && pPlayer->getLevel() >= 58 && pPlayer->InGroup() == true)
		{
			pPlayer->SafeTeleport(469, 0, -7672.939941f, -1107.307617f, 396.649994f, 0.616532f);
		}
		else if(pPlayer->getLevel() <= 57 || pPlayer->HasFinishedQuest(7761) == false)
		{
			pPlayer->BroadcastMessage("You need to be level 58 and have completed the quest : Blackhand's Command");
		}
		else if(pPlayer->HasFinishedQuest(7761) == true && pPlayer->getLevel() >= 58 && pPlayer->InGroup() == false)
		{
			pPlayer->BroadcastMessage("You need to be in a raid group to be able to enter this instance");
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class OrbOfTransLocUC : public GameObjectAIScript
{
public:
	OrbOfTransLocUC(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new OrbOfTransLocUC(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(pPlayer->getRace() == RACE_TAUREN || pPlayer->getRace() == RACE_TROLL || pPlayer->getRace() == RACE_ORC || pPlayer->getRace() == RACE_UNDEAD || pPlayer->getRace() == RACE_BLOODELF )
		{
			pPlayer->SafeTeleport(530, 0, 10021.254883f, -7014.893066f, 49.717690f, 4.00532f);		 // teleports to Silvermoon
		}
		else
		{
			pPlayer->BroadcastMessage("You are not alowed to use this orb");
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class OrbOfTransLocSILVM : public GameObjectAIScript
{
public:
	OrbOfTransLocSILVM(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new OrbOfTransLocSILVM(GO); }

	void OnActivate(Player *pPlayer)
	{
		if( pPlayer->getRace() == RACE_TAUREN || pPlayer->getRace() == RACE_TROLL || pPlayer->getRace() == RACE_ORC || pPlayer->getRace() == RACE_UNDEAD || pPlayer->getRace() == RACE_BLOODELF )
		{
			pPlayer->SafeTeleport(0, 0, 1805.823975f, 334.009796f, 70.397469f, 4.724170f);		 // teleports to UC
		}
		else
		{
			pPlayer->BroadcastMessage("You are not alowed to use this orb");
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class FalconTowerUpper : public GameObjectAIScript
{
public:
	FalconTowerUpper(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new FalconTowerUpper(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->SafeTeleport(530, 0, -588.9f, 4070.8f, 93.8f, 4.724170f);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class FalconTowerLower : public GameObjectAIScript
{
public:
	FalconTowerLower(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new FalconTowerLower(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->SafeTeleport(530, 0, -592.2f, 4070.2f, 143.3f, 4.724170f);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class FirewingPointOrb1 : public GameObjectAIScript
{
public:
	FirewingPointOrb1(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new FirewingPointOrb1(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->SafeTeleport(530, 0, -2260.6f, 3186.5f, 23.6f, 4.724170f);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class FirewingPointOrb2 : public GameObjectAIScript
{
public:
	FirewingPointOrb2(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new FirewingPointOrb2(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->SafeTeleport(530, 0, -2250.4f, 3120.7f, 27.3f, 4.724170f);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class duskwitherspirebottom : public GameObjectAIScript
{
public:
	duskwitherspirebottom(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new duskwitherspirebottom(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->SafeTeleport(530, 0, 9334.352f, -7880.743f, 74.910f, 4.724170f);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class duskwitherspiretop : public GameObjectAIScript
{
public:
	duskwitherspiretop(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new duskwitherspiretop(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->SafeTeleport(530, 0, 9291.155f, -7913.556f, 176.130f, 4.724170f);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class Blacksmithing_Plans_Use : public GameObjectAIScript
{
public:
	Blacksmithing_Plans_Use(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new Blacksmithing_Plans_Use(GO); }

	void OnLootTaken(Player *pLooter, ItemPrototype *pItemInfo)
	{
		float SSX = pLooter->GetPositionX();
		float SSY = pLooter->GetPositionY();
		float SSZ = pLooter->GetPositionZ();
		float SSO = pLooter->GetOrientation();

		Creature *NewCreature = pLooter->GetMapMgr()->GetInterface()->SpawnCreature(11120, SSX, SSY, SSZ, SSO, true, false, 0, 0);
		if ( NewCreature != NULL )
			NewCreature->Despawn(600000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class GongOfBethekk : public GameObjectAIScript
{
public:
	GongOfBethekk(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new GongOfBethekk(GO); }

	void OnActivate(Player *pPlayer)
	{
		Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(14515, -11556.3f, -1628.32f, 41.299f, 4.1f, true, false, 0, 0);
		if ( NewCreature != NULL )
			NewCreature->Despawn(1200000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class TerokksDownfall : public GameObjectAIScript
{
public:
	TerokksDownfall(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new TerokksDownfall(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(pPlayer->GetQuestLogForEntry(11073) && pPlayer->GetItemInterface()->GetItemCount(32720, 1))
		{
			float SSX = pPlayer->GetPositionX();
			float SSY = pPlayer->GetPositionY();
			float SSZ = pPlayer->GetPositionZ();
			float SSO = pPlayer->GetOrientation();

			Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(21838, SSX, SSY, SSZ, SSO, true, false, 0, 0);
			if ( NewCreature != NULL )
				NewCreature->Despawn(600000, 0);
		}
		else
		{
			pPlayer->BroadcastMessage("You need to have item : Time-Lost Offering and to have quest : Terokk's Downfall");
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class VilebranchKidnapper : public GameObjectAIScript
{
public:
	VilebranchKidnapper(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new VilebranchKidnapper(GO); }

	void OnActivate(Player *pPlayer)
	{
		float SSX = pPlayer->GetPositionX();
		float SSY = pPlayer->GetPositionY();
		float SSZ = pPlayer->GetPositionZ();
		float SSO = pPlayer->GetOrientation();

		Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(14748, SSX+1, SSY, SSZ, SSO, true, false, 0, 0);
		if ( NewCreature != NULL )
			NewCreature->Despawn(600000, 0);

		NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(14748, SSX, SSY, SSZ, SSO, true, false, 0, 0);
		if ( NewCreature != NULL )
			NewCreature->Despawn(600000, 0);

		NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(14748, SSX-1, SSY, SSZ, SSO, true, false, 0, 0);
		if ( NewCreature != NULL )
			NewCreature->Despawn(600000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class GongOfZulFarrak : public GameObjectAIScript
{
public:
	GongOfZulFarrak(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new GongOfZulFarrak(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(pPlayer->GetItemInterface()->GetItemCount(9240, 1))
		{
			float SSX = pPlayer->GetPositionX();
			float SSY = pPlayer->GetPositionY();
			float SSZ = pPlayer->GetPositionZ();
			float SSO = pPlayer->GetOrientation();

			Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(7273, SSX+1, SSY, SSZ, SSO, true, false, 0, 0);
			if ( NewCreature != NULL )
				NewCreature->Despawn(1800000, 0);
		}
		else
		{
			pPlayer->BroadcastMessage("Missing required item : Mallet of Zul'Farrak");
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class Obsidias_Egg : public GameObjectAIScript
{
public:
	Obsidias_Egg(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new Obsidias_Egg(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(pPlayer->GetQuestLogForEntry(11078))
		{
			float SSX = pPlayer->GetPositionX();
			float SSY = pPlayer->GetPositionY();
			float SSZ = pPlayer->GetPositionZ();
			float SSO = pPlayer->GetOrientation();

			Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(23282, SSX, SSY+1, SSZ, SSO, true, false, 0, 0);
			if ( NewCreature != NULL )
				NewCreature->Despawn(600000, 0);
		}
		else
		{
			pPlayer->BroadcastMessage("Missing required quest : To Rule The Skies");
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class Rivendarks_Egg : public GameObjectAIScript
{
public:
	Rivendarks_Egg(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new Rivendarks_Egg(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(pPlayer->GetQuestLogForEntry(11078))
		{
			float SSX = pPlayer->GetPositionX();
			float SSY = pPlayer->GetPositionY();
			float SSZ = pPlayer->GetPositionZ();
			float SSO = pPlayer->GetOrientation();

			Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(23061, SSX, SSY+1, SSZ, SSO, true, false, 0, 0);
			if ( NewCreature != NULL )
				NewCreature->Despawn(600000, 0);
		}
		else
		{
			pPlayer->BroadcastMessage("Missing required quest : To Rule The Skies");
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class Furywings_Egg : public GameObjectAIScript
{
public:
	Furywings_Egg(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new Furywings_Egg(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(pPlayer->GetQuestLogForEntry(11078))
		{
			float SSX = pPlayer->GetPositionX();
			float SSY = pPlayer->GetPositionY();
			float SSZ = pPlayer->GetPositionZ();
			float SSO = pPlayer->GetOrientation();

			Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(23261, SSX, SSY+1, SSZ, SSO, true, false, 0, 0);
			if ( NewCreature != NULL )
				NewCreature->Despawn(600000, 0);
		}
		else
		{
			pPlayer->BroadcastMessage("Missing required quest : To Rule The Skies");
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class Insidions_Egg : public GameObjectAIScript
{
public:
	Insidions_Egg(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new Insidions_Egg(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(pPlayer->GetQuestLogForEntry(11078))
		{
			float SSX = pPlayer->GetPositionX();
			float SSY = pPlayer->GetPositionY();
			float SSZ = pPlayer->GetPositionZ();
			float SSO = pPlayer->GetOrientation();

			Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(23281, SSX, SSY+1, SSZ, SSO, true, false, 0, 0);
			if ( NewCreature != NULL )
				NewCreature->Despawn(600000, 0);
		}
		else
		{
			pPlayer->BroadcastMessage("Missing required quest : To Rule The Skies");
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class Corrupt_Minor_Manifestation_Water_Object : public GameObjectAIScript
{
public:
	Corrupt_Minor_Manifestation_Water_Object(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new Corrupt_Minor_Manifestation_Water_Object(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(pPlayer->GetQuestLogForEntry(63))
		{
			float SSX = pPlayer->GetPositionX();
			float SSY = pPlayer->GetPositionY();
			float SSZ = pPlayer->GetPositionZ();
			float SSO = pPlayer->GetOrientation();

			Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(5894, SSX, SSY+1, SSZ, SSO, true, false, 0, 0);
			if ( NewCreature != NULL )
				NewCreature->Despawn(600000, 0);
		}
		else
		{
			pPlayer->BroadcastMessage("Missing required quest : Call of Water");
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class Telathion_the_Impure_Object : public GameObjectAIScript
{
public:
	Telathion_the_Impure_Object(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new Telathion_the_Impure_Object(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(pPlayer->GetQuestLogForEntry(9508))
		{
			float SSX = pPlayer->GetPositionX();
			float SSY = pPlayer->GetPositionY();
			float SSZ = pPlayer->GetPositionZ();
			float SSO = pPlayer->GetOrientation();

			Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(17359, SSX, SSY+1, SSZ, SSO, true, false, 0, 0);
			if ( NewCreature != NULL )
				NewCreature->Despawn(600000, 0);
		}
		else
		{
			pPlayer->BroadcastMessage("Missing required quest : Call of Water");
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class UlagTheCleaver : public GameObjectAIScript
{
public:
	UlagTheCleaver(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new UlagTheCleaver(GO); }

	void OnActivate(Player *pPlayer)
	{

		Creature *Ulag = NULLCREATURE;
		GameObject *pDoor = NULLGOB;
		Ulag = pPlayer->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(2390.101807f, 336.676788f, 40.015614f, 6390);
		pDoor = pPlayer->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(2388.480029f, 338.3901f, 40.092899f, 176594);
		QuestLogEntry * en = pPlayer->GetQuestLogForEntry(1819);
		if(!en || pDoor==NULL || Ulag)
		return;

		Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(6390, 2390.101807f, 336.676788f, 40.015614f, 2.259590f, true, false, 0, 0);
		if ( NewCreature != NULL )
		{
			NewCreature->Despawn(180000, 0);
			NewCreature->GetAIInterface()->SetNextTarget(pPlayer);
			NewCreature->GetAIInterface()->AttackReaction(pPlayer, 1);
			pDoor->SetUInt32Value(GAMEOBJECT_FLAGS, 33);
			pDoor->SetByte(GAMEOBJECT_BYTES_1, 0, 0);
		};
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class DustySpellbooks : public GameObjectAIScript
{
public:
	DustySpellbooks(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new DustySpellbooks(GO); }

	void OnLootTaken(Player *pLooter, ItemPrototype *pItemInfo)
	{
		QuestLogEntry * en = pLooter->GetQuestLogForEntry(422);
		if(!en)
		return;

		float SSX = pLooter->GetPositionX();
		float SSY = pLooter->GetPositionY();
		float SSZ = pLooter->GetPositionZ();
		float SSO = pLooter->GetOrientation();

		Creature *NewCreature = pLooter->GetMapMgr()->GetInterface()->SpawnCreature(1770, SSX, SSY, SSZ, SSO, true, false, 0, 0);
		if ( NewCreature != NULL )
			NewCreature->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "The Sons of Arugal will rise against all who challenge the power of the Moonrage!");
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class CatFigurine : public GameObjectAIScript
{
public:
	CatFigurine(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new CatFigurine(GO); }

	void OnActivate(Player *pPlayer)
	{
		uint32 Chance = RandomUInt(100);

		if(Chance <= 10)
		{
			float SSX = pPlayer->GetPositionX();
			float SSY = pPlayer->GetPositionY();
			float SSZ = pPlayer->GetPositionZ();
			float SSO = pPlayer->GetOrientation();

			Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(3619, SSX, SSY+1, SSZ, SSO, true, false, 0, 0);
			if ( NewCreature != NULL )
				NewCreature->Despawn(600000, 0);
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class EthereumTransponderZeta : public GameObjectAIScript
{
public:
	EthereumTransponderZeta(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new EthereumTransponderZeta(GO); }

	void OnActivate(Player *pPlayer)
	{
		Creature *commander = pPlayer->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 20482);
		if(commander)
			return;

		if(pPlayer->GetQuestLogForEntry(10339))
		{
			float SSX = 4017.96f;
			float SSY = 2315.91f;
			float SSZ = 116.418f;
			float SSO = pPlayer->GetOrientation();
			Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(20482, SSX, SSY, SSZ, SSO, true, false, 0, 0);
			if ( NewCreature != NULL )
				NewCreature->Despawn(1*60*1000, 0);
		}
	}

};

/*--------------------------------------------------------------------------------------------------------*/

class BringMetheEgg : public GameObjectAIScript
{
public:
	BringMetheEgg(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new BringMetheEgg(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;
		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(10111);
		if(qle == NULL)
			return;

		if(!pPlayer->HasAura(33382))
			pPlayer->CastSpell(pPlayer, 33382, true);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class MysteriousEgg : public GameObjectAIScript
{
public:
	MysteriousEgg(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new MysteriousEgg(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(10111);
		if(qle == NULL)
			return;

		if(qle->GetMobCount(0) < qle->GetQuest()->required_mobcount[0])
		{
			qle->SetMobCount(0, qle->GetMobCount(0)+1);
			qle->SendUpdateAddKill(0);
			qle->UpdatePlayerFields();
		}

		Creature *bird = pPlayer->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 19055);
		if(bird)
			return;

		bird = sEAS.SpawnCreature(pPlayer, 19055, pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), 0);
		if ( bird != NULL )
			bird->Despawn(5*60*1000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class AlterofTidalMastery : public GameObjectAIScript
{
public:
	AlterofTidalMastery(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new AlterofTidalMastery(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(16292, 7934.343750f, -7637.020996f, 112.694130f, 3.098388f, true, false, 0, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class ShrineOfDathRemar : public GameObjectAIScript
{
public:
	ShrineOfDathRemar(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new ShrineOfDathRemar(GO); }

	void OnActivate(Player *pPlayer)
	{
		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(8345);
		if(qle)
			qle->SendQuestComplete();
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class FirewingPointUpper : public GameObjectAIScript
{
public:
	FirewingPointUpper(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new FirewingPointUpper(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->SafeTeleport(530, 0, -2260, 3223.16f, 5.534f, 1.60f);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class FirewingPointLower : public GameObjectAIScript
{
public:
	FirewingPointLower(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new FirewingPointLower(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->SafeTeleport(530, 0, -2263, 3111.45f, 136.35f, 3.65f);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class APlagueUponThee : public GameObjectAIScript
{
public:
	APlagueUponThee(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new APlagueUponThee(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		GameObject *barel = sEAS.SpawnGameobject(pPlayer, 177491, pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), 1, 0, 0, 0, 0);
		sEAS.GameobjectDelete(barel, 2*60*1000);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class SerpentStatue : public GameObjectAIScript
{
public:
	SerpentStatue(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new SerpentStatue(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(6027);
		if(qle == NULL)
			  return;

		Creature *naga = sEAS.SpawnCreature(pPlayer, 12369, 246.741, 2953.3, 5.8631, 1.078, 0);
		if ( naga != NULL )
			naga->Despawn(6*60*1000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class CuregosGold : public GameObjectAIScript
{
public:
	CuregosGold(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new CuregosGold(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(2882);
		if(qle == NULL)
			return;

		Creature *pirate = sEAS.SpawnCreature(pPlayer, 7899, pPlayer->GetPositionX()+RandomFloat(5.0f), pPlayer->GetPositionY()+RandomFloat(5.0f), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), 0);
		if ( pirate != NULL )
			pirate->Despawn(6*60*1000, 0);

		pirate = sEAS.SpawnCreature(pPlayer, 7899, pPlayer->GetPositionX()-RandomFloat(5.0f), pPlayer->GetPositionY()+RandomFloat(5.0f), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), 0);
		if ( pirate != NULL )
			pirate->Despawn(6*60*1000, 0);

		pirate = sEAS.SpawnCreature(pPlayer, 7901, pPlayer->GetPositionX()+RandomFloat(5.0f), pPlayer->GetPositionY()-RandomFloat(5.0f), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), 0);
		if ( pirate != NULL )
			pirate->Despawn(6*60*1000, 0);

		pirate = sEAS.SpawnCreature(pPlayer, 7901, pPlayer->GetPositionX()+RandomFloat(5.0f), pPlayer->GetPositionY()+RandomFloat(5.0f), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), 0);
		if ( pirate != NULL )
			pirate->Despawn(6*60*1000, 0);

		pirate = sEAS.SpawnCreature(pPlayer, 7902, pPlayer->GetPositionX()-RandomFloat(5.0f), pPlayer->GetPositionY()-RandomFloat(5.0f), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), 0);
		if ( pirate != NULL )
			pirate->Despawn(6*60*1000, 0);

		GameObject *gobj = sEAS.SpawnGameobject(pPlayer, 142194, pPlayer->GetPositionX()+5, pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), 1, 0, 0, 0, 0);
		sEAS.GameobjectDelete(gobj, 10*60*1000);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class DreadmaulRock : public GameObjectAIScript
{
public:
	DreadmaulRock(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new DreadmaulRock(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(3821);
		if(qle == NULL)
			return;

		Creature *shaghost = pPlayer->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 9136);
		if(shaghost)
			return;

		Creature *shaghostspawn = sEAS.SpawnCreature(pPlayer, 9136, pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), 0);
		if ( shaghostspawn != NULL )
			shaghostspawn->Despawn(2*60*1000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class HandofIruxos : public GameObjectAIScript
{
public:
	HandofIruxos(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new HandofIruxos(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(5381);
		if(qle == NULL)
			return;

		Creature *demon = sEAS.SpawnCreature(pPlayer, 11876, -348.231, 1763.85, 138.371, 4.42728, 0);
		if ( demon != NULL )
			demon->Despawn(6*60*1000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class LegionPortals : public GameObjectAIScript
{
public:
	LegionPortals(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new LegionPortals(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(5581);
		if(qle == NULL)
			return;

		if(qle->GetMobCount(0) < qle->GetQuest()->required_mobcount[0])
		{
			qle->SetMobCount(0, qle->GetMobCount(0)+1);
			qle->SendUpdateAddKill(0);
			qle->UpdatePlayerFields();
		}
		return;
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class ProphecyofAkida : public GameObjectAIScript
{
public:
	ProphecyofAkida(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new ProphecyofAkida(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(9544);
		if(qle == NULL)
			return;

		if(qle->GetMobCount(0) < qle->GetQuest()->required_mobcount[0])
		{
			qle->SetMobCount(0, qle->GetMobCount(0)+1);
			qle->SendUpdateAddKill(0);
			qle->UpdatePlayerFields();
		}

		Creature *prisoner = pPlayer->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 17375);
		if(!prisoner)
			return;

		prisoner->Despawn(1, 6*60*1000);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class Razormaw : public GameObjectAIScript
{
public:
	Razormaw(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new Razormaw(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(9689);
		if(qle == NULL)
			return;

		Creature *razormaw = sEAS.SpawnCreature(pPlayer, 17592, -1203.8, -12424.7, 95.36, 4.7, 0);
		if ( razormaw != NULL )
			razormaw->Despawn(6*60*1000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class TabletoftheSeven : public GameObjectAIScript
{
public:
	TabletoftheSeven(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new TabletoftheSeven(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(4296);
		if(qle == NULL)
			return;

		if(pPlayer->GetItemInterface()->GetItemCount(11470, 0) < 1)
			sEAS.AddItem(11470, pPlayer);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class TestofEndurance : public GameObjectAIScript
{
public:
	TestofEndurance(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new TestofEndurance(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(1150);
		if(qle == NULL)
			return;

		Creature *grenka = pPlayer->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 4490);
		if(grenka)
		{
			if(!grenka->isAlive())
				grenka->Despawn( 5000, 120000 );
			else
				return;
		}

		Creature *grenkaspawn = sEAS.SpawnCreature(pPlayer, 4490, pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), 0);
		if ( grenkaspawn != NULL )
			grenkaspawn->Despawn(6*60*1000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class TheFallenExarch : public GameObjectAIScript
{
public:
	TheFallenExarch(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new TheFallenExarch(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(10915);
		if(qle == NULL)
			return;

		Creature *exarch = sEAS.SpawnCreature(pPlayer, 22452, -3365.9, 5143.19, -9.00132, 3.05, 0);
		if ( exarch != NULL )
			exarch->Despawn(6*60*1000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class TheFinalCode : public GameObjectAIScript
{
public:
	TheFinalCode(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new TheFinalCode(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(10447);
		if(qle == NULL)
			return;

		if(qle->GetMobCount(1) < qle->GetQuest()->required_mobcount[1])
		{
			qle->SetMobCount(1, qle->GetMobCount(1)+1);
			qle->SendUpdateAddKill(1);
			qle->UpdatePlayerFields();
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class TheRootofAllEvil : public GameObjectAIScript
{
public:
	TheRootofAllEvil(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new TheRootofAllEvil(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(8481);
		if(qle == NULL)
			return;

		Creature *xandivious = sEAS.SpawnCreature(pPlayer, 15623, pPlayer->GetPositionX()+5, pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetOrientation(), 0);
		if ( xandivious != NULL )
			xandivious->Despawn(6*60*1000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class TheThunderspike : public GameObjectAIScript
{
public:
	TheThunderspike(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new TheThunderspike(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		if(!pPlayer->GetQuestLogForEntry(10526))
			return;

		// Wth is that ? To remove ?
		GameObject *gobj = pPlayer->GetMapMgr()->GetInterface()->GetGameObjectNearestCoords(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 184729);
		if(!gobj)
			return;

		gobj->Despawn(6*60*1000);

		Creature *spike = sEAS.SpawnCreature(pPlayer, 21319, 1315.54, 6688.33, -18, 0.001, 0);
		spike->Despawn(5*60*1000, 0);
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class StrengthofOne : public GameObjectAIScript
{
public:
	StrengthofOne(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new StrengthofOne(GO); }

	void OnActivate(Player *pPlayer)
	{
		if(!pPlayer)
			return;

		QuestLogEntry *qle = pPlayer->GetQuestLogForEntry(9582);
		if(qle == NULL)
			return;

		// What is this ? :O To remove ?
		Creature *reaver = pPlayer->GetMapMgr()->GetInterface()->GetCreatureNearestCoords(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), 17556);
		if(reaver)
		{
			Creature *reaver2 = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(17556, reaver->GetPositionX(), reaver->GetPositionY(), reaver->GetPositionZ(), reaver->GetOrientation(), true, false, 0, 0);
			reaver2->Despawn(5*60*1000, 0);
			reaver->Despawn(1, 5*60*1000);
		}

	}
};

/*--------------------------------------------------------------------------------------------------------*/

class HealingTheLake : public GameObjectAIScript
{
public:
	HealingTheLake(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new HealingTheLake(GO); }

	void OnActivate(Player *pPlayer)
	{
		QuestLogEntry *pQuest = pPlayer->GetQuestLogForEntry(181433);
		if(pQuest && pQuest->GetMobCount(0) < pQuest->GetQuest()->required_mobcount[0])
		{
			pQuest->SetMobCount(0, 1);
			pQuest->SendUpdateAddKill(0);
			pQuest->UpdatePlayerFields();
		}
	}
};

/*--------------------------------------------------------------------------------------------------------*/

class OrbOfTransLocQuelLower : public GameObjectAIScript
{
public:
	OrbOfTransLocQuelLower(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new OrbOfTransLocQuelLower(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->SafeTeleport(530, 4080, 12791.350586f, -6891.479004f, 31.322657f, 5.336779f);		 // teleports to the upper level
	}
};


/*********************************************************************************************************************/

class OrbOfTransLocQuelTop : public GameObjectAIScript
{
public:
	OrbOfTransLocQuelTop(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new OrbOfTransLocQuelTop(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->SafeTeleport(530, 4080, 12782.415039f, -6879.196289f, 23.106073f, 2.222675f);		 // teleports to the lower level
	}
};


/**********************************************************************************************************************/

class OrbSunwell : public GameObjectAIScript
{
public:
	OrbSunwell(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new OrbSunwell(GO); }

	void OnActivate(Player *pPlayer)
	{
		pPlayer->SafeTeleport(530, 4080, 12558.393555f, -6774.535645f, 15.027822f, 0.035100f);		 // Sunwell quit portal
	}
};

/**********************************************************************************************************************/

class TheRavensClaw : public GameObjectAIScript
{
public:
	TheRavensClaw(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new TheRavensClaw(GO); }

	void OnActivate(Player *pPlayer)
	{
		if ( pPlayer->GetMapMgr()->iInstanceMode == MODE_HEROIC )
			sEAS.SpawnCreature(pPlayer, 23035, -87.3546, 288.006, 26.4832, 0, 0);
	}
};

/**********************************************************************************************************************/

class DeathGate1 : public GameObjectAIScript
{
public:
	DeathGate1(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new DeathGate1(GO); }

	void OnSpawn()
	{
		RegisterAIUpdateEvent( 500 );
	}

	void AIUpdate()
	{
		Player *plr = _gameobject->GetMapMgr()->GetInterface()->GetPlayerNearestCoords( _gameobject->GetPositionX(), _gameobject->GetPositionY(), _gameobject->GetPositionZ() );
		if( !plr )
			return;

		if( _gameobject->CalcDistance( _gameobject, plr ) <= 1.5f && plr->GetMapId() == 609)
		{
			plr->SafeTeleport( 609, 4298, 2390.18, -5640.28, 420.84, 0.57 );
		}

		else if( _gameobject->CalcDistance( _gameobject, plr ) <= 1.5f && plr->GetMapId() == 0)
		{
			plr->SafeTeleport( 0, plr->GetInstanceID(), 2387.468994f, -5642.544922f, 420.809937f, 0.620461f);
		}
	}
};

/**********************************************************************************************************************/
// Should it work like that ?
class DeathGate2 : public GameObjectAIScript
{
public:
	DeathGate2(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new DeathGate2(GO); }

	void OnSpawn()
	{
		RegisterAIUpdateEvent( 500 );
	}

	void AIUpdate()
	{
		Player *plr = _gameobject->GetMapMgr()->GetInterface()->GetPlayerNearestCoords( _gameobject->GetPositionX(), _gameobject->GetPositionY(), _gameobject->GetPositionZ() );
		if( !plr )
			return;

		if( _gameobject->CalcDistance( _gameobject, plr ) <= 1.5f && plr->GetMapId() == 609)
		{
			plr->SafeTeleport( 609, 4298, 2397.57, -5636.03, 377.05, 0.57 );
		}

		else if( _gameobject->CalcDistance( _gameobject, plr ) <= 1.5f && plr->GetMapId() == 0)
		{
			plr->SafeTeleport( 0, plr->GetInstanceID(), 2395.233154f, -5637.875488f, 377.086884f, 0.520721f);
		}
	}
};

/**********************************************************************************************************************/
class DeathGate3 : public GameObjectAIScript
{
public:
	DeathGate3(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new DeathGate3(GO); }

    void OnActivate(Player *pPlayer)
    {
       if (pPlayer->getClass() == 6)
       {
			if(pPlayer->GetMapId() == 609){
				pPlayer->SafeTeleport(609, pPlayer->GetInstanceID(), 2354.496826f, -5661.948730f, 389.454865f, 0.365210f);
			}else{
				pPlayer->SafeTeleport(0, pPlayer->GetInstanceID(), 2354.496826f, -5661.948730f, 389.454865f, 0.365210f);
			};
       }
    }
 };

 class SacredFireofLife : public GameObjectAIScript
{
public:
	SacredFireofLife(GameObject *goinstance) : GameObjectAIScript(goinstance) {}
	static GameObjectAIScript *Create(GameObject *GO) { return new SacredFireofLife(GO); }

	void OnActivate(Player *pPlayer)
	{
		float SSX = pPlayer->GetPositionX();
		float SSY = pPlayer->GetPositionY();
		float SSZ = pPlayer->GetPositionZ();
		float SSO = pPlayer->GetOrientation();

		Creature *NewCreature = pPlayer->GetMapMgr()->GetInterface()->SpawnCreature(10882, SSX, SSY, SSZ, SSO, true, false, 0, 0);
		if ( NewCreature != NULL )
			NewCreature->Despawn(600000, 0);
	}
};

void SetupGoHandlers(ScriptMgr * mgr)
{
	mgr->register_gameobject_script(179879, &OrbOfCommand::Create);
	mgr->register_gameobject_script(184503, &OrbOfTransLocUC::Create);
	mgr->register_gameobject_script(184502, &OrbOfTransLocSILVM::Create);
	mgr->register_gameobject_script(184500, &FalconTowerUpper::Create);
	mgr->register_gameobject_script(184501, &FalconTowerLower::Create);
	mgr->register_gameobject_script(182543, &FirewingPointOrb1::Create);
	mgr->register_gameobject_script(182548, &FirewingPointOrb2::Create);
	mgr->register_gameobject_script(180912, &duskwitherspirebottom::Create);
	mgr->register_gameobject_script(180911, &duskwitherspiretop::Create);
	mgr->register_gameobject_script(173232, &Blacksmithing_Plans_Use::Create);
	mgr->register_gameobject_script(180526, &GongOfBethekk::Create);
	mgr->register_gameobject_script(185928, &TerokksDownfall::Create);
	mgr->register_gameobject_script(179910, &VilebranchKidnapper::Create);
	mgr->register_gameobject_script(141832, &GongOfZulFarrak::Create);
	mgr->register_gameobject_script(185932, &Obsidias_Egg::Create);
	mgr->register_gameobject_script(185936, &Rivendarks_Egg::Create);
	mgr->register_gameobject_script(185937, &Furywings_Egg::Create);
	mgr->register_gameobject_script(185938, &Insidions_Egg::Create);
	mgr->register_gameobject_script(113791, &Corrupt_Minor_Manifestation_Water_Object::Create);
	mgr->register_gameobject_script(181699, &Telathion_the_Impure_Object::Create);
	mgr->register_gameobject_script(104593, &UlagTheCleaver::Create);
	mgr->register_gameobject_script(1571, &DustySpellbooks::Create);

	mgr->register_gameobject_script(13873, &CatFigurine::Create);
	mgr->register_gameobject_script(184383, &EthereumTransponderZeta::Create);
	mgr->register_gameobject_script(183146, &BringMetheEgg::Create);
	mgr->register_gameobject_script(183147, &MysteriousEgg::Create);
	mgr->register_gameobject_script(180516, &ShrineOfDathRemar::Create);
	mgr->register_gameobject_script(182546, &FirewingPointUpper::Create);
	mgr->register_gameobject_script(182547, &FirewingPointLower::Create);
	mgr->register_gameobject_script(177490, &APlagueUponThee::Create);
	mgr->register_gameobject_script(177673, &SerpentStatue::Create);
	mgr->register_gameobject_script(142189, &CuregosGold::Create);
	mgr->register_gameobject_script(160445, &DreadmaulRock::Create);
	mgr->register_gameobject_script(176581, &HandofIruxos::Create);
	mgr->register_gameobject_script(177400, &LegionPortals::Create);
	mgr->register_gameobject_script(181730, &ProphecyofAkida::Create);
	mgr->register_gameobject_script(181988, &Razormaw::Create);
	mgr->register_gameobject_script(169294, &TabletoftheSeven::Create);
	mgr->register_gameobject_script(20447, &TestofEndurance::Create);
	mgr->register_gameobject_script(184999, &TheFallenExarch::Create);
	mgr->register_gameobject_script(184725, &TheFinalCode::Create);
	mgr->register_gameobject_script(180672, &TheRootofAllEvil::Create);
	mgr->register_gameobject_script(184729, &TheThunderspike::Create);
	mgr->register_gameobject_script(181849, &StrengthofOne::Create);
	mgr->register_gameobject_script(187428, &OrbOfTransLocQuelLower::Create);
	mgr->register_gameobject_script(187431, &OrbOfTransLocQuelTop::Create);
	mgr->register_gameobject_script(188173, &OrbSunwell::Create);
	mgr->register_gameobject_script(181433, &HealingTheLake::Create);
	mgr->register_gameobject_script(185554, &TheRavensClaw::Create);
	mgr->register_gameobject_script(191538, &DeathGate1::Create);
	mgr->register_gameobject_script(191539, &DeathGate2::Create);
	mgr->register_gameobject_script(190942, &DeathGate3::Create);
	mgr->register_gameobject_script(175944, &SacredFireofLife::Create);
}
