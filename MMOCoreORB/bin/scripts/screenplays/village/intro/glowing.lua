local ObjectManager = require("managers.object.object_manager")
local Logger = require("utils.logger")

Glowing = ScreenPlay:new {
	requiredBadges = {
		{ type = "master", amount = 1 },
	},
}

function Glowing:getCompletedBadgeTypeCount(pPlayer)
	local pGhost = CreatureObject(pPlayer):getPlayerObject()

	if (pGhost == nil) then
		return 0
	end

	local typesCompleted = 0

	for i = 1, #self.requiredBadges, 1 do
		local type = self.requiredBadges[i].type
		local requiredAmount = self.requiredBadges[i].amount

		local badgeListByType = getBadgeListByType(type)
		local badgeCount = 0

		for j = 1, #badgeListByType, 1 do
			if PlayerObject(pGhost):hasBadge(badgeListByType[j]) then
				badgeCount = badgeCount + 1
			end
		end

		if badgeCount >= requiredAmount then
			typesCompleted = typesCompleted + 1
		end
	end

	return typesCompleted
end

function Glowing:hasRequiredBadgeCount(pPlayer)
	return self:getCompletedBadgeTypeCount(pPlayer) == #self.requiredBadges
end

-- Check if the player is glowing or not.
-- @param pPlayer pointer to the creature object of the player.
function Glowing:isGlowing(pPlayer)
	return VillageJediManagerCommon.hasJediProgressionScreenPlayState(pPlayer, VILLAGE_JEDI_PROGRESSION_GLOWING)
end

-- Event handler for the BADGEAWARDED event.
-- @param pPlayer pointer to the creature object of the player who was awarded with a badge.
-- @param pPlayer2 pointer to the creature object of the player who was awarded with a badge.
-- @param badgeNumber the badge number that was awarded.
-- @return 0 to keep the observer active.
function Glowing:badgeAwardedEventHandler(pPlayer, pPlayer2, badgeNumber)
	if (pPlayer == nil) then
		return 0
	end

	if self:hasRequiredBadgeCount(pPlayer) and not CreatureObject(pPlayer):hasSkill("force_title_jedi_novice") then
		VillageJediManagerCommon.setJediProgressionScreenPlayState(pPlayer, VILLAGE_JEDI_PROGRESSION_GLOWING)
		self:completeVillageIntro(pPlayer)
		return 1
	end

	return 0
end

-- Function to automatically complete the Village intro sequence
function Glowing:completeVillageIntro(pPlayer)
	if (pPlayer == nil) then
		return
	end

	print("Glowing: Auto-completing Village intro for " .. CreatureObject(pPlayer):getFirstName())

	-- Give Force crystal
	local pInventory = SceneObject(pPlayer):getSlottedObject("inventory")
	if (pInventory ~= nil) then
		giveItem(pInventory, "object/tangible/loot/quest/force_sensitive/force_crystal.iff", -1)
		print("Glowing: Gave Force crystal")
	else
		print("Glowing: Could not get inventory")
	end
	
	-- Complete all intro quests
	local QuestManager = require("managers.quest.quest_manager")
	QuestManager.completeQuest(pPlayer, QuestManager.quests.OLD_MAN_INITIAL)
	QuestManager.completeQuest(pPlayer, QuestManager.quests.OLD_MAN_FORCE_CRYSTAL)
	QuestManager.completeQuest(pPlayer, QuestManager.quests.TWO_MILITARY)
	QuestManager.completeQuest(pPlayer, QuestManager.quests.LOOT_DATAPAD_1)
	QuestManager.completeQuest(pPlayer, QuestManager.quests.GOT_DATAPAD)
	QuestManager.completeQuest(pPlayer, QuestManager.quests.FS_THEATER_CAMP)
	QuestManager.completeQuest(pPlayer, QuestManager.quests.GOT_DATAPAD_2)
	QuestManager.completeQuest(pPlayer, QuestManager.quests.LOOT_DATAPAD_2)
	QuestManager.completeQuest(pPlayer, QuestManager.quests.FS_VILLAGE_ELDER)
	
	-- Set progression states
	VillageJediManagerCommon.setJediProgressionScreenPlayState(pPlayer, VILLAGE_JEDI_PROGRESSION_HAS_CRYSTAL)
	VillageJediManagerCommon.setJediProgressionScreenPlayState(pPlayer, VILLAGE_JEDI_PROGRESSION_HAS_VILLAGE_ACCESS)
	
	-- Set Jedi state and award skill
	local pGhost = CreatureObject(pPlayer):getPlayerObject()
	if (pGhost ~= nil and not PlayerObject(pGhost):isJedi()) then
		PlayerObject(pGhost):setJediState(1)
		print("Glowing: Set Jedi state to 1")
	else
		print("Glowing: Player already has Jedi state")
	end
	
	awardSkill(pPlayer, "force_title_jedi_novice")
	print("Glowing: Awarded force_title_jedi_novice skill")
	
	-- Show popup notification to the player
	self:showVillageAccessPopup(pPlayer)
	
	print("Glowing: Village intro auto-completed for " .. CreatureObject(pPlayer):getFirstName())
end

-- Function to show popup notification about Village access
function Glowing:showVillageAccessPopup(pPlayer)
	if (pPlayer == nil) then
		return
	end
	
	-- Try immediate popup first, then delayed as backup
	self:showVillagePopupImmediate(pPlayer)
	
	-- Also create a delayed event as backup
	createEvent(3000, "Glowing", "showVillagePopupDelayed", pPlayer)
	
	-- Also create a waypoint to the Village immediately
	local pGhost = CreatureObject(pPlayer):getPlayerObject()
	if (pGhost ~= nil) then
		PlayerObject(pGhost):addWaypoint("dathomir", "Village of Aurilia", "Village of Aurilia - Jedi Training", 5306, 0, -4145, WAYPOINTGREEN, true, true, 0)
	end
end

-- Immediate popup function
function Glowing:showVillagePopupImmediate(pPlayer)
	if (pPlayer == nil) then
		return
	end
	
	-- Create popup message using a simpler approach
	local sui = SuiMessageBox.new("Glowing", "popupCallback")
	sui.setTitle("Force Sensitivity Awakened")
	sui.setPrompt("You feel the Force awaken within you! Your mastery of your profession has revealed your connection to the Force.\n\nYou now have access to the Village of Aurilia on Dathomir, where you can begin your Jedi training. Seek out the Village Elder to continue your journey.\n\nLocation: Dathomir (5306, -4145)")
	sui.sendTo(pPlayer)
end

-- Delayed function to show the popup
function Glowing:showVillagePopupDelayed(pPlayer)
	if (pPlayer == nil) then
		return
	end

	-- Create popup message using a simpler approach
	local sui = SuiMessageBox.new("Glowing", "popupCallback")
	sui.setTitle("Force Sensitivity Awakened")
	sui.setPrompt("You feel the Force awaken within you! Your mastery of your profession has revealed your connection to the Force.\n\nYou now have access to the Village of Aurilia on Dathomir, where you can begin your Jedi training. Seek out the Village Elder to continue your journey.\n\nLocation: Dathomir (5306, -4145)")
	sui.sendTo(pPlayer)
end

-- Callback function for the Village popup
function Glowing:popupCallback(pPlayer, pSui, eventIndex, args)
	-- Do nothing, just close the popup
end

-- Register observer on the player for observing badge awards.
-- @param pPlayer pointer to the creature object of the player to register observers on.
function Glowing:registerObservers(pPlayer)
	dropObserver(BADGEAWARDED, "Glowing", "badgeAwardedEventHandler", pPlayer)
	createObserver(BADGEAWARDED, "Glowing", "badgeAwardedEventHandler", pPlayer)
end

-- Handling of the onPlayerLoggedIn event. The progression of the player will be checked and observers will be registered.
-- @param pPlayer pointer to the creature object of the player who logged in.
function Glowing:onPlayerLoggedIn(pPlayer)
	if not self:isGlowing(pPlayer) then
		if self:hasRequiredBadgeCount(pPlayer) then
			VillageJediManagerCommon.setJediProgressionScreenPlayState(pPlayer, VILLAGE_JEDI_PROGRESSION_GLOWING)
			FsIntro:startPlayerOnIntro(pPlayer)
		else
			self:registerObservers(pPlayer)
		end
	end
end

-- Handling of the checkForceStatus command.
-- @param pPlayer pointer to the creature object of the player who performed the command
function Glowing:checkForceStatusCommand(pPlayer)
	local progress = "@jedi_spam:fs_progress_" .. self:getCompletedBadgeTypeCount(pPlayer)

	CreatureObject(pPlayer):sendSystemMessage(progress)
end

-- Register the screenplay
registerScreenPlay("Glowing", true)

return Glowing
