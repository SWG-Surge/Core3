/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

		#ifndef THROWGRENADECOMMAND_H_
		#define THROWGRENADECOMMAND_H_
		
		#include "server/zone/objects/creature/commands/CombatQueueCommand.h"
		#include "server/zone/objects/tangible/weapon/WeaponObject.h"
		#include "server/zone/objects/tangible/TangibleObject.h"
		#include "templates/tangible/SharedWeaponObjectTemplate.h"
		#include "templates/SharedObjectTemplate.h"
		#include "templates/manager/TemplateManager.h"
		#include "server/ServerCore.h"
		
		class ThrowGrenadeCommand : public CombatQueueCommand {
		public:
			ThrowGrenadeCommand(const String& name, ZoneProcessServer* server) : CombatQueueCommand(name, server) {}
		
			Reference<WeaponObject*> findGrenade(CreatureObject* creature) const {
				SceneObject* inventory = creature->getSlottedObject("inventory");
				if (inventory == nullptr) return nullptr;
		
				for (int i = 0; i < inventory->getContainerObjectsSize(); ++i) {
					SceneObject* item = inventory->getContainerObject(i);
					if (!item->isWeaponObject()) continue;
		
					WeaponObject* weapon = cast<WeaponObject*>(item);
					if (weapon != nullptr && weapon->isThrownWeapon()) {
						return weapon;
					}
				}
				return nullptr;
			}
		
			int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const override {
				if (!checkStateMask(creature)) return INVALIDSTATE;
				if (!checkInvalidLocomotions(creature)) return INVALIDLOCOMOTION;
		
				ManagedReference<TangibleObject*> targetObject = server->getZoneServer()->getObject(target).castTo<TangibleObject*>();
				if (targetObject == nullptr) return INVALIDTARGET;
		
				Reference<WeaponObject*> grenade = findGrenade(creature);
				if (grenade == nullptr) {
					creature->sendSystemMessage("You do not have a usable grenade.");
					return GENERALERROR;
				}
		
				if (!grenade->isASubChildOf(creature)) return GENERALERROR;
		
				SharedWeaponObjectTemplate* grenadeData = cast<SharedWeaponObjectTemplate*>(grenade->getObjectTemplate());
				if (grenadeData == nullptr) return GENERALERROR;
		
				UnicodeString args = "combatSpam=" + grenadeData->getCombatSpam() + ";";
				int result = doCombatAction(creature, target, args, grenade);
		
				if (result == SUCCESS) {
					Core::getTaskManager()->scheduleTask([grenade] {
						Locker lock(grenade);
						grenade->decreaseUseCount();
					}, "ThrowGrenadeTanoDecrementTask", 100);
		
				// 	creature->setNextAllowedMoveTime(3000); // prevent animation break
				}
		
				return result;
			}
		
			String getAnimation(TangibleObject* attacker, TangibleObject* defender, WeaponObject* weapon, uint8 hitLocation, int damage) const override {
				SharedWeaponObjectTemplate* weaponData = cast<SharedWeaponObjectTemplate*>(weapon->getObjectTemplate());
				if (weaponData == nullptr) return "throw_grenade";
		
				String type = weaponData->getAnimationType();
				int range = attacker->getWorldPosition().distanceTo(defender->getWorldPosition());
		
				String distance = (range < 10) ? "_near_" : (range < 20) ? "_medium_" : "_far_";
				return "throw_grenade" + distance + type;
			}
		
			float getCommandDuration(CreatureObject* object, const UnicodeString& arguments) const override {
				Reference<WeaponObject*> grenade = findGrenade(object);
				if (grenade == nullptr) return 10.f;
		
				return CombatManager::instance()->calculateWeaponAttackSpeed(object, grenade, speedMultiplier);
			}
		};
		
		#endif // THROWGRENADECOMMAND_H_