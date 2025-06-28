#ifndef STRUCTUREMAINTENANCEPAYMENTCHOICESUICALLBACK_H_
#define STRUCTUREMAINTENANCEPAYMENTCHOICESUICALLBACK_H_

#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/managers/structure/StructureManager.h"

class StructureMaintenancePaymentChoiceSuiCallback : public SuiCallback {
public:
	StructureMaintenancePaymentChoiceSuiCallback(ZoneServer* serv) : SuiCallback(serv) {}

	void run(CreatureObject* creature, SuiBox* suiBox, uint32 eventIndex, Vector<UnicodeString>* args) override {
		if (creature == nullptr || suiBox == nullptr || eventIndex == 0)
			return;

		ManagedReference<SceneObject*> usingObject = suiBox->getUsingObject();
		if (usingObject == nullptr || !usingObject->isStructureObject())
			return;

		StructureObject* structure = cast<StructureObject*>(usingObject.get());

		bool useBank = (eventIndex == 2); // 1 = OK = cash, 2 = Cancel = bank

		StructureManager::instance()->openMaintenancePaymentTransfer(structure, creature, useBank);
	}

	void destroy() override {
		delete this;
	}
};

#endif /* STRUCTUREMAINTENANCEPAYMENTCHOICESUICALLBACK_H_ */
