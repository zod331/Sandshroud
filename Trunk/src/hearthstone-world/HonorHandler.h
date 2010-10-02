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

#ifndef HONORHANDLER_H
#define HONORHANDLER_H

class HonorHandler
{
public:
    static int32 CalculateHonorPointsForKill(Player* pPlayer, Unit* pVictim);
    static void RecalculateHonorFields(Player* pPlayer);
    static void AddHonorPointsToPlayer(Player* pPlayer, uint32 uAmount);
    static void OnPlayerKilledUnit(Player* pPlayer, Unit* pVictim);
    static int32 CalculateHonorPointsFormula(uint32 AttackerLevel,uint32 VictimLevel);
};


#endif
