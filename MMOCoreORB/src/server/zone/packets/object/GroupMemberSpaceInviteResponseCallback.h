/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef GROUPMEMBERSPACEINVITERESPONSECALLBACK_H_
#define GROUPMEMBERSPACEINVITERESPONSECALLBACK_H_

#include "ObjectControllerMessageCallback.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/packets/object/GroupMemberSpaceInviteResponseMessage.h"

class GroupMemberSpaceInviteResponseCallback : public MessageCallback {
	ObjectControllerMessageCallback* objectControllerMain;
	uint64 pilotID;
	byte decision;

public:
	GroupMemberSpaceInviteResponseCallback(ObjectControllerMessageCallback* objectControllerCallback) : MessageCallback(objectControllerCallback->getClient(), objectControllerCallback->getServer()),
		objectControllerMain(objectControllerCallback), pilotID(0), decision(0) {
	}

	void parse(Message* message) {
		message->shiftOffset(4);
		pilotID = message->parseLong();
		decision = message->parseByte();
	}

	void run() {
		ManagedReference<CreatureObject*> player = client->getPlayer();

		if (player == nullptr)
			return;

		auto zoneServer = player->getZoneServer();

		if (zoneServer == nullptr)
			return;

		ManagedReference<SceneObject*> pilotScno = zoneServer->getObject(pilotID);

		if (pilotScno == nullptr || !pilotScno->isPlayerCreature())
			return;

		if ((player->getParentID() != pilotScno->getParentID()) || !pilotScno->isInRange(player, 7.f))
			return;

		auto pilot = pilotScno.castTo<CreatureObject*>();

		if (pilot == nullptr)
			return;

		GroupMemberSpaceInviteResponseMessage* responseMsg = new GroupMemberSpaceInviteResponseMessage(player, pilot, decision);
		pilot->sendMessage(responseMsg);
	}
};

#endif /* GROUPMEMBERSPACEINVITERESPONSECALLBACK_H_ */
