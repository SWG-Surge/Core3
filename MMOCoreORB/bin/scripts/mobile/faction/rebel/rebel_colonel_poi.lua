rebel_colonel_poi = Creature:new {
	objectName = "@mob/creature_names:rebel_colonel",
	randomNameType = NAME_GENERIC,
	randomNameTag = true,
	mobType = MOB_NPC,
	socialGroup = "rebel",
	faction = "rebel",
	level = 300,
	chanceHit = 0.95,
	damageMin = 2000,
	damageMax = 3000,
	baseXp = 30000,
	baseHAM = 80000,
	baseHAMmax = 100000,
	armor = 3,
	resists = {180,180,180,180,180,180,180,180,135},
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
		"object/mobile/dressed_rebel_colonel_bith_female_01.iff",
		"object/mobile/dressed_rebel_colonel_bothan_male_01.iff",
		"object/mobile/dressed_rebel_colonel_human_female_01.iff",
		"object/mobile/dressed_rebel_colonel_moncal_male_01.iff",
		"object/mobile/dressed_rebel_colonel_rodian_female_01.iff",
		"object/mobile/dressed_rebel_colonel_sullustan_male_01.iff"},
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

CreatureTemplates:addCreatureTemplate(rebel_colonel_poi, "rebel_colonel_poi") 