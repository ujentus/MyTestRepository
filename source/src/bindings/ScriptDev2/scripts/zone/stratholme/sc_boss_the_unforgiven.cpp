/* Copyright (C) 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "../../sc_defines.h"

// **** This script is still under Developement ****

//Satus: Send to SVN

#define SPELL_FROSTBOLT    17503
#define SPELL_CHILLED    20005

struct MANGOS_DLL_DECL boss_the_unforgivenAI : public ScriptedAI
{
    boss_the_unforgivenAI(Creature *c) : ScriptedAI(c) {EnterEvadeMode();}

    uint32 Frostbolt_Timer;
    uint32 Chilled_Timer;
    bool InCombat;

    void EnterEvadeMode()
    {
        Frostbolt_Timer = 1000;
        Chilled_Timer = 7000;
        InCombat = false;

        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop();
        DoGoHome();
    }

    void AttackStart(Unit *who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && who!= m_creature)
        {
            DoStartMeleeAttack(who);
            InCombat = true;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (!who || m_creature->getVictim())
            return;

        if (who->isTargetableForAttack() && who->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(who))
        {
            float attackRadius = m_creature->GetAttackDistance(who);
            if (m_creature->IsWithinDistInMap(who, attackRadius) && m_creature->GetDistanceZ(who) <= CREATURE_Z_ATTACK_RANGE)
            {
                if(who->HasStealthAura())
                    who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                //Begin melee attack if we are within range
                DoStartMeleeAttack(who);
                InCombat = true;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget())
            return;

        //Check if we have a current target
        if( m_creature->getVictim() && m_creature->isAlive())
        {
            //Frostbolt
            if (Frostbolt_Timer < diff)
            {
                //Cast
                if (rand()%100 < 95) //95% chance to cast
                {
                    DoCast(m_creature->getVictim(),SPELL_FROSTBOLT);
                }
                //4 seconds until we should cast this again
                Frostbolt_Timer = 4000;
            }else Frostbolt_Timer -= diff;

            //Chilled
            if (Chilled_Timer < diff)
            {
                //Cast
                if (rand()%100 < 45) //45% chance to cast
                {
                    DoCast(m_creature->getVictim(),SPELL_CHILLED);
                }
                //14 seconds until we should cast this again
                Chilled_Timer = 14000;
            }else Chilled_Timer -= diff;

            //If we are within range melee the target
            if( m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE))
            {
                //Make sure our attack is ready and we arn't currently casting
                if( m_creature->isAttackReady() && !m_creature->m_currentSpell)
                {
                    m_creature->AttackerStateUpdate(m_creature->getVictim());
                    m_creature->resetAttackTimer();
                }
            }
        }
    }
}; 
CreatureAI* GetAI_boss_the_unforgiven(Creature *_Creature)
{
    return new boss_the_unforgivenAI (_Creature);
}


void AddSC_boss_the_unforgiven()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_the_unforgiven";
    newscript->GetAI = GetAI_boss_the_unforgiven;
    m_scripts[nrscripts++] = newscript;
}