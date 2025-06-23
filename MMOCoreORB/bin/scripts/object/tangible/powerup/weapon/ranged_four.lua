object_tangible_powerup_weapon_ranged_four = object_tangible_powerup_weapon_shared_ranged_four:new {

	templateType = POWERUP,

	pupType = "Ranged",

	baseName = "Ranged Four Powerup",
	uses = 500,

	primary = {
		// (list only the remaining stats)
	},

	secondary = {
		// (list only the remaining stats)
	},

	numberExperimentalProperties = {1, 1, 1, 1},
	experimentalProperties = {"XX", "XX", "XX", "OQ"},
	experimentalWeights = {1, 1, 1, 1},
	experimentalGroupTitles = {"null", "null", "null", "exp_effectiveness"},
	experimentalSubGroupTitles = {"null", "null", "hitpoints", "effect"},
	experimentalMin = {0, 0, 1000, 1},
	experimentalMax = {0, 0, 1000, 100},
	experimentalPrecision = {0, 0, 0, 0},
	experimentalCombineType = {0, 0, 4, 1},
}

ObjectTemplates:addTemplate(object_tangible_powerup_weapon_ranged_four, "object/tangible/powerup/weapon/ranged_four.iff") 