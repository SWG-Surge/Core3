#ifndef STRUCTUREPAYMAINTENANCESUICALLBACK_H_
#define STRUCTUREPAYMAINTENANCESUICALLBACK_H_

#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/structure/StructureObject.h"
#include "server/zone/managers/structure/StructureManager.h"
#include "server/zone/ZoneServer.h"

class StructurePayMaintenanceSuiCallback : public SuiCallback {
public:
	bool useBank;

	StructurePayMaintenanceSuiCallback(ZoneServer* serv, bool useBank = false) : SuiCallback(serv), useBank(useBank) {}

	void run(CreatureObject* creature, SuiBox* sui, uint32 eventIndex, Vector<UnicodeString>* args) override {
		if (!sui->isTransferBox() || args == nullptr || args->size() < 2)
			return;

		int amount = Integer::valueOf(args->get(1).toString());
		if (amount <= 0)
			return;

		ManagedReference<SceneObject*> obj = sui->getUsingObject().get();
		if (obj == nullptr || !obj->isStructureObject()) {
			creature->sendSystemMessage("@player_structure:invalid_target");
			return;
		}

		StructureObject* structure = cast<StructureObject*>(obj.get());
		Locker _lock(structure, creature);

		if (useBank) {
			if (creature->getBankCredits() < amount) {
				creature->sendSystemMessage("@player_structure:insufficient_funds");
				return;
			}
			creature->subtractBankCredits(amount);
		} else {
			if (creature->getCashCredits() < amount) {
				creature->sendSystemMessage("@player_structure:insufficient_funds");
				return;
			}
			creature->subtractCashCredits(amount);
		}

		structure->addMaintenance(amount);

		StringIdChatParameter msg("base_player", "prose_pay_success");
		msg.setDI(amount);
		msg.setTT(structure->getDisplayedName());

		creature->sendSystemMessage(msg);
	}

	void destroy() override {
		delete this;
	}
};

#endif /* STRUCTUREPAYMAINTENANCESUICALLBACK_H_ */
