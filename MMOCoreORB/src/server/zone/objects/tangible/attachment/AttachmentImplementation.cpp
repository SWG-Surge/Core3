/*
 * AttachmentImplementation.cpp
 *
 *  Created on: Mar 10, 2011
 *      Author: polonel
 */

#include "server/zone/objects/tangible/attachment/Attachment.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/ZoneProcessServer.h"
#include "server/zone/packets/scene/AttributeListMessage.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/managers/loot/LootManager.h"
#include "server/zone/managers/loot/LootValues.h"
#include "server/zone/managers/auction/AuctionManager.h"
#include "server/zone/managers/auction/AuctionsMap.h"

void AttachmentImplementation::initializeMembers() {
	if (gameObjectType == SceneObjectType::CLOTHINGATTACHMENT) {
		setOptionsBitmask(32, true);
		attachmentType = CLOTHINGTYPE;

	} else if (gameObjectType == SceneObjectType::ARMORATTACHMENT) {
		setOptionsBitmask(32, true);
		attachmentType = ARMORTYPE;
	}
}

void AttachmentImplementation::initializeTransientMembers() {
	TangibleObjectImplementation::initializeTransientMembers();

	setLoggingName("AttachmentObject");

	// This is here to convert the existing HashTable to a VectorMap
	if (skillModifiers.size() < 1) {
		HashTableIterator<String, int> iterator = skillModMap.iterator();

		String key = "";
		int value = 0;

		for (int i = 0; i < skillModMap.size(); ++i) {
			iterator.getNextKeyAndValue(key, value);

			skillModifiers.put(key, value);
		}
	}
}

void AttachmentImplementation::updateCraftingValues(CraftingValues* values, bool firstUpdate) {
	auto zoneServer = getZoneServer();

	if (zoneServer == nullptr) {
		return;
	}

	auto lootManager = zoneServer->getLootManager();

	if (lootManager == nullptr) {
		return;
	}

	float level = values->hasExperimentalAttribute("creatureLevel") ? values->getCurrentValue("creatureLevel") : 1;
	float bonus = values->hasExperimentalAttribute("modifier") ? values->getCurrentValue("modifier") : 1;
	float rank = LootValues::getLevelRankValue(level, 0.2f, 0.9f);

	int chance = rank * bonus * 100.f;
	int roll = System::random(1000);
	int modCount = 1;

	int pivot = chance - roll;

	if (pivot < 40) {
		modCount = 1;
	} else if (pivot < 70) {
		modCount = System::random(1) + 1;
	} else if (pivot < 100) {
		modCount = System::random(2) + 1;
	} else {
		modCount = System::random(1) + 2;
	}

	for (int i = 0; i < modCount; ++i) {
		float step = 1.f - ((i / (float)modCount) * 0.5f);
		int min = Math::clamp(-1, (int)round(0.075f * level) - 1, 25) * step;
		int max = Math::clamp(-1, (int)round(0.125f * level) + 1, 25);
		int mod = System::random(max - min) + min;

		String modName = lootManager->getRandomLootableMod(gameObjectType);

		skillModifiers.put(modName, ((mod <= 0) ? 1 : mod));
	}
}

void AttachmentImplementation::fillAttributeList(AttributeListMessage* msg, CreatureObject* object) {
	TangibleObjectImplementation::fillAttributeList(msg, object);

	// Check if this attachment is currently in the auction system
	bool isInAuction = false;
	ZoneServer* zoneServer = object->getZoneServer();
	if (zoneServer != nullptr) {
		AuctionManager* auctionManager = zoneServer->getAuctionManager();
		if (auctionManager != nullptr) {
			AuctionsMap* auctionMap = auctionManager->getAuctionMap();
			if (auctionMap != nullptr) {
				isInAuction = auctionMap->containsItem(getObjectID());
			}
		}
	}

	StringBuffer name;

	for (int i = 0; i < skillModifiers.size(); i++) {
		auto key = skillModifiers.elementAt(i).getKey();
		auto value = skillModifiers.elementAt(i).getValue();

		name << "cat_skill_mod_bonus.@stat_n:" << key;

		if (isInAuction) {
			// When examining from auction, show only the skill mod name without the value
			msg->insertAttribute(name.toString(), "");
		} else {
			// Normal examination shows the value
			msg->insertAttribute(name.toString(), value);
		}

		name.deleteAll();
	}
}
