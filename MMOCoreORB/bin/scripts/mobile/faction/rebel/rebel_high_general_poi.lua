rebel_high_general_poi = Creature:new {
	objectName = "@mob/creature_names:rebel_high_general",
	randomNameType = NAME_GENERIC,
	randomNameTag = true,
	mobType = MOB_NPC,
	socialGroup = "rebel",
	faction = "rebel",
	level = 320,
	chanceHit = 0.96,
	damageMin = 2200,
	damageMax = 3200,
	baseXp = 35000,
	baseHAM = 90000,
	baseHAMmax = 110000,
	armor = 3,
	resists = {185,185,185,185,185,185,185,185,135},
	meatType = "",
	meatAmount = 0,
	hideType = "",
	hideAmount = 0,
	boneType = "",
	boneAmount = 0,
	milk = 0,
	tamingChance = 0,
	ferocity = 0,
	pvpBitmask = ATTACKABLE,
	creatureBitmask = PACK + KILLER,
	optionsBitmask = AIENABLED,
	diet = HERBIVORE,

	templates = {
		"object/mobile/dressed_rebel_high_general_bith_female_01.iff",
		"object/mobile/dressed_rebel_high_general_bothan_male_01.iff",
		"object/mobile/dressed_rebel_high_general_human_female_01.iff",
		"object/mobile/dressed_rebel_high_general_moncal_male_01.iff",
		"object/mobile/dressed_rebel_high_general_rodian_female_01.iff",
		"object/mobile/dressed_rebel_high_general_sullustan_male_01.iff"},
	lootGroups = {
		{
			groups = {
				{group = "rebel_hideout_boss", chance = 10000000}
			}
		}
	},

	-- Primary and secondary weapon should be different types (rifle/carbine, carbine/pistol, rifle/unarmed, etc)
	-- Unarmed should be put on secondary unless the mobile doesn't use weapons, in which case "unarmed" should be put primary and "none" as secondary
	primaryWeapon = "rebel_rifle",
	secondaryWeapon = "rebel_carbine",
	conversationTemplate = "",
	reactionStf = "@npc_reaction/military",
	personalityStf = "@hireling/hireling_military",

	-- primaryAttacks and secondaryAttacks should be separate skill groups specific to the weapon type listed in primaryWeapon and secondaryWeapon
	-- Use merge() to merge groups in creatureskills.lua together. If a weapon is set to "none", set the attacks variable to empty brackets
	primaryAttacks = marksmanmaster,
	secondaryAttacks = marksmanmaster
}

CreatureTemplates:addCreatureTemplate(rebel_high_general_poi, "rebel_high_general_poi") 