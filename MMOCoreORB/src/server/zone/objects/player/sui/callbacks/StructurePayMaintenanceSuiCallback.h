/*
 * StructurePayMaintenanceSuiCallback.h
 *
 *  Created on: Aug 16, 2011
 *      Author: cRush
 */

#ifndef STRUCTUREPAYMAINTENANCESUICALLBACK_H_
#define STRUCTUREPAYMAINTENANCESUICALLBACK_H_

#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/structure/StructureObject.h"
#include "server/zone/managers/structure/StructureManager.h"


class StructurePayMaintenanceSuiCallback : public SuiCallback {
public:
	bool useBank;
	StructurePayMaintenanceSuiCallback(ZoneServer* serv, bool useBank = false) : SuiCallback(serv), useBank(useBank) {
	}


	void run(CreatureObject* creature, SuiBox* sui, uint32 eventIndex, Vector<UnicodeString>* args) {
		bool cancelPressed = (eventIndex == 1);

		if (!sui->isTransferBox() || cancelPressed || args->size() < 2)
			return;

		int amount = Integer::valueOf(args->get(1).toString());

		if (amount < 0)
			return;

		ManagedReference<SceneObject*> obj = sui->getUsingObject().get();

		if (obj == nullptr || !obj->isStructureObject()) {
			creature->sendSystemMessage("@player_structure:invalid_target"); // "Your original structure target is no longer valid. Aborting..."
			return;
		}

		//Deposit/Withdraw the maintenance
		StructureObject* structure = cast<StructureObject*>(obj.get());

		ManagedReference<Zone*> zone = structure->getZone();

		if (zone == nullptr)
			return;

		//Creature is already locked (done in handleSuiEventNotification in SuiManager).
		Locker _lock(structure, creature);

		if (amount > 0) {
	if (useBank) {
		if (creature->getBankCredits() < amount) {
			creature->sendSystemMessage("@player_structure:insufficient_funds");
			return;
		}

		TransactionLog trx(creature, structure, TrxCode::STRUCTUREMAINTANENCE, amount, true);
		creature->subtractBankCredits(amount);
	} else {
		if (creature->getCashCredits() < amount) {
			creature->sendSystemMessage("@player_structure:insufficient_funds");
			return;
		}

		TransactionLog trx(creature, structure, TrxCode::STRUCTUREMAINTANENCE, amount, true);
		creature->subtractCashCredits(amount);
	}

	structure->addMaintenance(amount);

	StringIdChatParameter msg("base_player", "prose_pay_success"); // "You successfully make a payment of %DI credits to %TT."
	msg.setDI(amount);
	msg.setTT(structure->getDisplayedName());

	creature->sendSystemMessage(msg);
	}
};

#endif /* STRUCTUREPAYMAINTENANCESUICALLBACK_H_ */
