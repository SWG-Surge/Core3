/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef POLEARMLUNGE2COMMAND_H_
#define POLEARMLUNGE2COMMAND_H_

#include "CombatQueueCommand.h"
#include "server/zone/managers/combat/CombatManager.h"
#include "server/zone/objects/creature/BuffAttribute.h"
#include "server/zone/objects/creature/buffs/Buff.h"
#include <system/thread/Locker.h>

class PolearmLunge2Command : public CombatQueueCommand {
public:

	PolearmLunge2Command(const String& name, ZoneProcessServer* server)
		: CombatQueueCommand(name, server) {
	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		// -------------------------------------------------------------------------------------
		// Beginning of Stripping ForceRun from a target
		Reference<SceneObject*> object = server->getZoneServer()->getObject(target);
		ManagedReference<CreatureObject*> creatureTarget = cast<CreatureObject*>( object.get());

		if (creatureTarget == nullptr)
			return GENERALERROR;

		if (creature->getDistanceTo(object) > 20.f){
			creature->sendSystemMessage("You are out of range.");
			return GENERALERROR;
		}

		PlayerManager* playerManager = server->getPlayerManager();

		if (creature != creatureTarget && !CollisionManager::checkLineOfSight(creature, creatureTarget)) {
			return GENERALERROR;
		}

		// Make sure they can't spam!
		// Also skip FR stripping if the target has Master Enhancer
		if (creature->isAttackableBy(creatureTarget) && creatureTarget->checkCooldownRecovery("fr_strip_resistant")) {

			// Do not strip FR if they're not within 20meters, but still do the attack.
			// Technically this should never happen as the attack itself is set to 20m, but this is just so FR strip cannot be used outside of 20m.
			// We also want to check to see if they even have novice enhancer to even check if they could have FR on them!
			if (!creature->isInRange(creatureTarget, 20) || !creatureTarget->hasSkill("force_discipline_enhancements_novice")) {
				return doCombatAction(creature, target);
			}

			// Add Strip code here
			// Don't strip Fr1
			// const bool hasFr1 = creatureTarget->hasBuff(BuffCRC::JEDI_FORCE_RUN_1);
			const bool hasFr2 = creatureTarget->hasBuff(BuffCRC::JEDI_FORCE_RUN_2);
			const bool hasFr3 = creatureTarget->hasBuff(BuffCRC::JEDI_FORCE_RUN_3);

			if (hasFr2 || hasFr3) {
				Locker clocker(creatureTarget, creature);

				if (hasFr2) {
					creatureTarget->removeBuff(BuffCRC::JEDI_FORCE_RUN_2);
				}
				if (hasFr3) {
					creatureTarget->removeBuff(BuffCRC::JEDI_FORCE_RUN_3);
				}

				creatureTarget->addCooldown("fr_strip_resistant", 30000); // 30 second cooldown
				creature->sendSystemMessage("You have stripped Force Run from your target!");
				creatureTarget->sendSystemMessage("Your Force Run has been stripped!");
			}
		}
		// -------------------------------------------------------------------------------------

		return doCombatAction(creature, target);
	}

};

#endif //POLEARMLUNGE2COMMAND_H_
