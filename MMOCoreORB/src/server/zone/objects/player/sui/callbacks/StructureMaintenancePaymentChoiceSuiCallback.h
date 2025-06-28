#ifndef STRUCTUREMAINTENANCEPAYMENTCHOICESUICALLBACK_H_
#define STRUCTUREMAINTENANCEPAYMENTCHOICESUICALLBACK_H_

#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/managers/structure/StructureManager.h"

class StructureMaintenancePaymentChoiceSuiCallback : public SuiCallback {
public:
	StructureMaintenancePaymentChoiceSuiCallback(ZoneServer* serv) : SuiCallback(serv) {}

	void run(CreatureObject* creature, SuiBox* suiBox, uint32 eventIndex, Vector<String>& args) override {
		if (creature == nullptr || suiBox == nullptr)
			return;

		ManagedReference<SceneObject*> usingObject = suiBox->getUsingObject();
		if (usingObject == nullptr || !usingObject->isStructureObject())
			return;

		StructureObject* structure = cast<StructureObject*>(usingObject.get());

		// Only handle OK (selection made)
		if (eventIndex != 1)
			return;

		// eventIndex is OK, menu selection is stored in args[0]
		if (args.size() < 1)
			return;

		int selection = Integer::parseInt(args.get(0));
		bool useBank = (selection == 1);

		StructureManager::instance()->openMaintenancePaymentTransfer(structure, creature, useBank);
	}

	void destroy() override {
		delete this;
	}
};

#endif /* STRUCTUREMAINTENANCEPAYMENTCHOICESUICALLBACK_H_ */
