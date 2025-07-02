-- GCW Trainer Creature Template
trainer_gcw = Creature:new {
	objectName = "GCW Trainer",
	randomNameType = NO_RANDOM_NAME,
	customName = "GCW Trainer",
	mobType = MOB_NPC,
	faction = "",
	level = 100,
	chanceHit = 0.390000,
	damageMin = 290,
	damageMax = 300,
	baseXp = 2914,
	baseHAM = 8400,
	baseHAMmax = 10200,
	armor = 0,
	resists = {-1,-1,-1,-1,-1,-1,-1,-1,-1},
	meatType = "",
	meatAmount = 0,
	hideType = "",
	hideAmount = 0,
	boneType = "",
	boneAmount = 0,
	milk = 0,
	tamingChance = 0.000000,
	ferocity = 0,
	pvpBitmask = NONE,
	creatureBitmask = NONE,
	optionsBitmask = INVULNERABLE + CONVERSABLE,
	diet = HERBIVORE,

	templates = {
		"object/mobile/dressed_trainer_bountyhunter_01.iff"
	},
	lootGroups = {},

	-- Primary and secondary weapon should be different types (rifle/carbine, carbine/pistol, rifle/unarmed, etc)
	-- Unarmed should be put on secondary unless the mobile doesn't use weapons, in which case "unarmed" should be put primary and "none" as secondary
	primaryWeapon = "unarmed",
	secondaryWeapon = "none",
	conversationTemplate = "gcw_trainer_convo",
	
	-- primaryAttacks and secondaryAttacks should be separate skill groups specific to the weapon type listed in primaryWeapon and secondaryWeapon
	-- Use merge() to merge groups in creatureskills.lua together. If a weapon is set to "none", set the attacks variable to empty brackets
	primaryAttacks = {},
	secondaryAttacks = { }
}
CreatureTemplates:addCreatureTemplate(trainer_gcw,"trainer_gcw")

-- GCW Trainer Screenplay
GcwTrainerScreenplay = ScreenPlay:new {
	numberOfActs = 1,
	screenplayName = "GcwTrainerScreenplay",
}

registerScreenPlay("GcwTrainerScreenplay", true)

function GcwTrainerScreenplay:start()
	-- This function can be used to spawn the trainer if needed
end

return GcwTrainerScreenplay 