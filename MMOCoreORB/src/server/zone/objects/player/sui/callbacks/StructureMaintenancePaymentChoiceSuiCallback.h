#ifndef STRUCTUREMAINTENANCEPAYMENTCHOICESUICALLBACK_H_
#define STRUCTUREMAINTENANCEPAYMENTCHOICESUICALLBACK_H_

#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/managers/structure/StructureManager.h"

class StructureMaintenancePaymentChoiceSuiCallback : public SuiCallback {
public:
	StructureMaintenancePaymentChoiceSuiCallback(ZoneServer* serv) : SuiCallback(serv) {}

	void run(CreatureObject* creature, SuiBox* suiBox, uint32 eventIndex, Vector<UnicodeString>* args) override {
		Logger::console.info("DEBUG: StructureMaintenancePaymentChoiceSuiCallback::run called with eventIndex = " + String::valueOf(eventIndex));

		if (creature == nullptr || suiBox == nullptr || eventIndex != 1)
			return;

		ManagedReference<SceneObject*> usingObject = suiBox->getUsingObject();
		if (usingObject == nullptr || !usingObject->isStructureObject())
			return;

		if (args == nullptr || args->size() == 0)
			return;

		int selection = Integer::valueOf(args->get(0).toString());
		bool useBank = (selection == 1); // 0 = cash, 1 = bank

		StructureObject* structure = cast<StructureObject*>(usingObject.get());
		StructureManager::instance()->openMaintenancePaymentTransfer(structure, creature, useBank);
	}

	void destroy() override {
		delete this;
	}
};

#endif /* STRUCTUREMAINTENANCEPAYMENTCHOICESUICALLBACK_H_ */
