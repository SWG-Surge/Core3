/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef INVITECOMMAND_H_
#define INVITECOMMAND_H_


#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/managers/group/GroupManager.h"
#include "server/zone/ZoneServer.h"

class InviteCommand : public QueueCommand {
public:

	InviteCommand(const String& name, ZoneProcessServer* server) : QueueCommand(name, server) {
	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		auto ghost = creature->getPlayerObject();
		bool godMode = false;

		if (ghost != nullptr && ghost->isPrivileged()) {
			godMode = true;
		}

		auto zoneServer = server->getZoneServer();

		if (zoneServer == nullptr) {
			creature->sendSystemMessage("Error: ZoneServer is null");
			return GENERALERROR;
		}

		auto object = zoneServer->getObject(target);
		bool galaxyWide = ConfigManager::instance()->getBool("Core3.PlayerManager.GalaxyWideGrouping", false);

		// If galaxy-wide grouping is enabled and we have arguments, try to get player by name
		if (galaxyWide && arguments.toString().trim().length() > 0) {
			StringTokenizer args(arguments.toString());
			String firstName;

			if (args.hasMoreTokens()) {
				args.getStringToken(firstName);

				auto chatManager = zoneServer->getChatManager();

				if (chatManager != nullptr) {
					auto playerByName = chatManager->getPlayer(firstName);
					if (playerByName != nullptr) {
						object = playerByName;
					}
				}
			}
		}

		// If galaxy-wide grouping is enabled and we don't have a valid object, try to get player by name from arguments
		if (galaxyWide && (object == nullptr || (!object->isPlayerCreature() && !object->isShipObject()))) {
			StringTokenizer args(arguments.toString());
			String firstName;

			if (args.hasMoreTokens()) {
				args.getStringToken(firstName);

				auto chatManager = zoneServer->getChatManager();

				if (chatManager != nullptr) {
					auto playerByName = chatManager->getPlayer(firstName);
					if (playerByName != nullptr) {
						object = playerByName;
					} else {
						// Player not found by name
						StringIdChatParameter stringId;
						stringId.setStringId("group", "no_target");
						stringId.setTT(firstName);
						creature->sendSystemMessage(stringId);
						return GENERALERROR;
					}
				} else {
					creature->sendSystemMessage("Error: ChatManager is null");
					return GENERALERROR;
				}
			} else {
				// No arguments provided
				creature->sendSystemMessage("Usage: /invite <playerName> or target a player");
				return GENERALERROR;
			}
		}

		auto groupManager = GroupManager::instance();

		if (object == nullptr) {
			creature->sendSystemMessage("Error: Target object is null");
			return GENERALERROR;
		}

		if (groupManager == nullptr) {
			creature->sendSystemMessage("Error: GroupManager is null");
			return GENERALERROR;
		}

		if (!object->isPlayerCreature() && !object->isShipObject()) {
			creature->sendSystemMessage("Error: Target is not a player or ship");
			return GENERALERROR;
		}

		CreatureObject* player = nullptr;

		if (object->isShipObject()) {
			auto ship = object->asShipObject();

			if (ship != nullptr) {
				player = ship->getOwner().get();
			}
		} else {
			player = object->asCreatureObject();
		}

		if (player == nullptr) {
			creature->sendSystemMessage("Error: Could not get player from object");
			return GENERALERROR;
		}

		auto invitedGhost = player->getPlayerObject();

		if (invitedGhost == nullptr) {
			creature->sendSystemMessage("Error: Target player has no PlayerObject");
			return GENERALERROR;
		}

		// Cannot be invite by a player that they ignore, does not apply to privileged players
		if (!godMode && invitedGhost->isIgnoring(creature->getFirstName())) {
			creature->sendSystemMessage("Error: Target player is ignoring you");
			return GENERALERROR;
		}

		groupManager->inviteToGroup(creature, player);

		return SUCCESS;
	}

};

#endif //INVITECOMMAND_H_
