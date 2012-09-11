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

#define SPELL_SNAPKICK                15618
#define SPELL_CLEAVE             15579  
#define SPELL_UPPERCUT            10966
#define SPELL_MORTALSTRIKE                 16856
#define SPELL_PUMMEL              15615       
#define SPELL_THROWAXE         16075
#define SPELL_DAZED         1604


struct MANGOS_DLL_DECL boss_warmastervooneAI : public ScriptedAI
{
    boss_warmastervooneAI(Creature *c) : ScriptedAI(c) {EnterEvadeMode();}

    uint32 Snapkick_Timer;
    uint32 Cleave_Timer;
    uint32 Uppercut_Timer;
    uint32 MortalStrike_Timer;
    uint32 Pummel_Timer;
    uint32 ThrowAxe_Timer;
    uint32 Dazed_Timer;
    bool InCombat;

    void EnterEvadeMode()
    {       
        Snapkick_Timer = 8000;
        Cleave_Timer = 14000;
        Uppercut_Timer = 20000;
        MortalStrike_Timer = 12000;
        Pummel_Timer = 32000;
        ThrowAxe_Timer = 1000;
        Dazed_Timer = 25000;
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

            //Snapkick_Timer
            if (Snapkick_Timer < diff)
            {
                //Cast
                DoCast(m_creature->getVictim(),SPELL_SNAPKICK);


                //6 seconds
                Snapkick_Timer = 6000;
            }else Snapkick_Timer -= diff;

            //Cleave_Timer
            if (Cleave_Timer < diff)
            {
                //Cast
                DoCast(m_creature->getVictim(),SPELL_CLEAVE);

                //12 seconds until we should cast this agian
                Cleave_Timer = 12000;
            }else Cleave_Timer -= diff;

            //Uppercut_Timer
            if (Uppercut_Timer < diff)
            {
                //Cast
                DoCast(m_creature->getVictim(),SPELL_UPPERCUT);

                //20 seconds until we should cast this agian
                Uppercut_Timer = 14000;
            }else Uppercut_Timer -= diff;

            //MortalStrike_Timer
            if (MortalStrike_Timer < diff)
            {

                DoCast(m_creature->getVictim(),SPELL_MORTALSTRIKE);

                //8 seconds until we should cast this agian
                MortalStrike_Timer = 10000;
            }else MortalStrike_Timer -= diff;

            //Pummel_Timer
            if (Pummel_Timer < diff)
            {
                //Cast
                DoCast(m_creature->getVictim(),SPELL_PUMMEL);

                //30 seconds until we should cast this agian
                Pummel_Timer = 16000;
            }else Pummel_Timer -= diff;

            //ThrowAxe_Timer
            if (ThrowAxe_Timer < diff)
            {
                //Cast
                DoCast(m_creature->getVictim(),SPELL_THROWAXE);

                //30 seconds until we should cast this agian
                ThrowAxe_Timer = 8000;
            }else ThrowAxe_Timer -= diff;

            //Dazed_Timer
            if (Dazed_Timer < diff)
            {
                //Cast
                DoCast(m_creature->getVictim(),SPELL_DAZED);

                //24 seconds until we should cast this agian
                Dazed_Timer = 24000;
            }else Dazed_Timer -= diff;

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
CreatureAI* GetAI_boss_warmastervoone(Creature *_Creature)
{
    return new boss_warmastervooneAI (_Creature);
}


void AddSC_boss_warmastervoone()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_warmaster_voone";
    newscript->GetAI = GetAI_boss_warmastervoone;
    m_scripts[nrscripts++] = newscript;
}