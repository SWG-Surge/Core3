--Should all created players start with God Mode? 1 = yes, 0 = no
freeGodMode = 0;
--How many cash credits new characters start with after creating a character (changed during test phase, normal value is 100)
startingCash = 100
--startingCash = 100000
--How many bank credits new characters start with after creating a character (changed during test phase, normal value is 1000)
startingBank = 1000
--startingBank = 100000
--How many skill points a new characters start with
skillPoints = 250

professions = {
	"combat_brawler",
	"combat_marksman",
	"crafting_artisan",
	"jedi",
	"outdoors_scout",
	"science_medic",
	"social_entertainer"
}

marksmanPistol = "object/weapon/ranged/pistol/pistol_cdef.iff"
	
marksmanRifle = "object/weapon/ranged/rifle/rifle_cdef.iff"

marksmanCarbine = "object/weapon/ranged/carbine/carbine_cdef.iff"

brawlerOneHander = "object/weapon/melee/knife/knife_stone.iff"

brawlerTwoHander = "object/weapon/melee/axe/axe_heavy_duty.iff"

brawlerPolearm = "object/weapon/melee/polearm/lance_staff_wood_s1.iff"

survivalKnife = "object/weapon/melee/knife/knife_survival.iff"

genericTool = "object/tangible/crafting/station/generic_tool.iff"

foodTool = "object/tangible/crafting/station/food_tool.iff"

mineralTool = "object/tangible/survey_tool/survey_tool_mineral.iff"

chemicalTool = "object/tangible/survey_tool/survey_tool_liquid.iff"

slitherhorn = "object/tangible/instrument/slitherhorn.iff"

marojMelon = "object/tangible/food/foraged/foraged_fruit_s1.iff"

x31Speeder = "object/tangible/deed/vehicle_deed/landspeeder_x31_deed.iff"

placeholderDatapad = "object/tangible/mission/mission_datadisk.iff"

professionSpecificItems = {
	combat_brawler = { brawlerOneHander, brawlerTwoHander, brawlerPolearm },
	combat_marksman = { marksmanPistol, marksmanCarbine, marksmanRifle },
	crafting_artisan = { genericTool, mineralTool, chemicalTool },
	jedi = { },
	outdoors_scout = { genericTool },
	science_medic = { foodTool },
	social_entertainer = { slitherhorn }
}

commonStartingItems = { marojMelon, survivalKnife, x31Speeder, placeholderDatapad }

-- Function to set up Force-sensitive state for new characters
function setupForceSensitiveState(pPlayer)
	if (pPlayer == nil) then
		print("setupForceSensitiveState: pPlayer is nil")
		return
	end

	local pGhost = CreatureObject(pPlayer):getPlayerObject()
	
	if (pGhost == nil) then
		print("setupForceSensitiveState: pGhost is nil")
		return
	end

	print("setupForceSensitiveState: Setting up Force-sensitive state for " .. CreatureObject(pPlayer):getFirstName())

	-- Set Jedi state to 1 (Force sensitive)
	if (not PlayerObject(pGhost):isJedi()) then
		PlayerObject(pGhost):setJediState(1)
		print("setupForceSensitiveState: Set Jedi state to 1")
	else
		print("setupForceSensitiveState: Player already has Jedi state")
	end

	-- Try to set Village progression states
	local success, result = pcall(function()
		-- Try to require the module
		local VillageJediManagerCommon = require("managers.jedi.village_jedi_manager_common")
		
		-- Set progression states as if intro is completed
		VillageJediManagerCommon.setJediProgressionScreenPlayState(pPlayer, 1) -- VILLAGE_JEDI_PROGRESSION_GLOWING
		VillageJediManagerCommon.setJediProgressionScreenPlayState(pPlayer, 2) -- VILLAGE_JEDI_PROGRESSION_HAS_CRYSTAL
		VillageJediManagerCommon.setJediProgressionScreenPlayState(pPlayer, 4) -- VILLAGE_JEDI_PROGRESSION_HAS_VILLAGE_ACCESS
		
		print("setupForceSensitiveState: Set Village progression states")
		return true
	end)
	
	if not success then
		print("setupForceSensitiveState: Could not set Village progression states: " .. tostring(result))
	end

	-- Give Force crystal
	local pInventory = SceneObject(pPlayer):getSlottedObject("inventory")
	if (pInventory ~= nil) then
		giveItem(pInventory, "object/tangible/loot/quest/force_sensitive/force_crystal.iff", -1)
		print("setupForceSensitiveState: Gave Force crystal")
	else
		print("setupForceSensitiveState: Could not get inventory")
	end

	print("setupForceSensitiveState: Basic setup complete for " .. CreatureObject(pPlayer):getFirstName())
end