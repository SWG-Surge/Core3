rebel_medic_poi = Creature:new {
	objectName = "@mob/creature_names:rebel_medic",
	randomNameType = NAME_GENERIC,
	randomNameTag = true,
	mobType = MOB_NPC,
	socialGroup = "rebel",
	faction = "rebel",
	level = 200,
	chanceHit = 0.85,
	damageMin = 1200,
	damageMax = 1800,
	baseXp = 15000,
	baseHAM = 45000,
	baseHAMmax = 55000,
	armor = 2,
	resists = {140,140,140,140,140,140,140,140,125},
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
	creatureBitmask = PACK + HEALER,
	optionsBitmask = AIENABLED,
	diet = HERBIVORE,

	templates = {
		"object/mobile/dressed_rebel_medic_bith_female_01.iff",
		"object/mobile/dressed_rebel_medic_bothan_male_01.iff",
		"object/mobile/dressed_rebel_medic_human_female_01.iff",
		"object/mobile/dressed_rebel_medic_moncal_male_01.iff",
		"object/mobile/dressed_rebel_medic_rodian_female_01.iff",
		"object/mobile/dressed_rebel_medic_sullustan_male_01.iff"},
	lootGroups = {
		{
			groups = {
				{group = "rebel_hideout_tier_1", chance = 10000000}
			}
		}
	},

	-- Primary and secondary weapon should be different types (rifle/carbine, carbine/pistol, rifle/unarmed, etc)
	-- Unarmed should be put on secondary unless the mobile doesn't use weapons, in which case "unarmed" should be put primary and "none" as secondary
	primaryWeapon = "rebel_pistol",
	secondaryWeapon = "unarmed",
	conversationTemplate = "",
	reactionStf = "@npc_reaction/military",
	personalityStf = "@hireling/hireling_military",

	-- primaryAttacks and secondaryAttacks should be separate skill groups specific to the weapon type listed in primaryWeapon and secondaryWeapon
	-- Use merge() to merge groups in creatureskills.lua together. If a weapon is set to "none", set the attacks variable to empty brackets
	primaryAttacks = marksmanmaster,
	secondaryAttacks = brawlermaster
}

CreatureTemplates:addCreatureTemplate(rebel_medic_poi, "rebel_medic_poi") 