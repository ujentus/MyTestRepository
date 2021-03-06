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

#define SPELL_CURSEOFBLOOD             24673
#define SPELL_HEX             16708       
#define SPELL_CLEAVE            20691

struct MANGOS_DLL_DECL boss_shadowvoshAI : public ScriptedAI
{
    boss_shadowvoshAI(Creature *c) : ScriptedAI(c) {EnterEvadeMode();}

    uint32 CurseOfBlood_Timer;
    uint32 Hex_Timer;
    uint32 Cleave_Timer;
    bool InCombat;

    void EnterEvadeMode()
    {       
        CurseOfBlood_Timer = 2000;
        Hex_Timer = 8000;
        Cleave_Timer = 14000;
        InCombat = false;

        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop();
        DoGoHome();
        //m_creature->CastSpell(m_creature,SPELL_ICEARMOR,true);
    }

    void AttackStart(Unit *who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && who!= m_creature)
        {
            //Begin melee attack if we are within range
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

            //CurseOfBlood_Timer
            if (CurseOfBlood_Timer < diff)
            {
                //Cast
                DoCast(m_creature->getVictim(),SPELL_CURSEOFBLOOD);

                //45 seconds
                CurseOfBlood_Timer = 45000;
            }else CurseOfBlood_Timer -= diff;

            //Hex_Timer
            if (Hex_Timer < diff)
            {
                //Cast HEX on a Random target
                Unit* target = NULL;

                target = SelectUnit(SELECT_TARGET_RANDOM,0);
                if (target)DoCast(target,SPELL_HEX);

                //15 seconds until we should cast this agian
                Hex_Timer = 15000;
            }else Hex_Timer -= diff;

            //Cleave_Timer
            if (Cleave_Timer < diff)
            {
                //Cast
                DoCast(m_creature->getVictim(),SPELL_CLEAVE);

                //7 seconds until we should cast this agian
                Cleave_Timer = 7000;
            }else Cleave_Timer -= diff;

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
CreatureAI* GetAI_boss_shadowvosh(Creature *_Creature)
{
    return new boss_shadowvoshAI (_Creature);
}


void AddSC_boss_shadowvosh()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_shadow_hunter_voshgajin";
    newscript->GetAI = GetAI_boss_shadowvosh;
    m_scripts[nrscripts++] = newscript;
}
