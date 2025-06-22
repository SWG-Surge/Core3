mission_direction_choice = ScreenPlay:new {
	numberOfActs = 1,

	directions = {
		{dirDesc = "Reset Mission Direction", dirSelect = 0},
		{dirDesc = "North", dirSelect = 90},
		{dirDesc = "North East", dirSelect = 45},
		{dirDesc = "East", dirSelect = 360}, 
		{dirDesc = "South East", dirSelect = 315}, 
		{dirDesc = "South", dirSelect = 270}, 
		{dirDesc = "South West", dirSelect = 225}, 
		{dirDesc = "West", dirSelect = 180}, 
		{dirDesc = "North West", dirSelect = 135}
	}
}

function mission_direction_choice:start()
end

function mission_direction_choice:openWindow(pPlayer)
	if (pPlayer == nil) then return end
	self:showDirections(pPlayer)
end

function mission_direction_choice:showDirections(pPlayer)
	local sui = SuiListBox.new("mission_direction_choice", "dirSelection")
	sui.setTargetNetworkId(SceneObject(pPlayer):getObjectID())
	sui.setTitle("Mission Direction Selection")

	local promptText = "Choose the direction you'd like your destroy missions to appear in.\n\nUse a mission terminal after selection.\n\nTo reset to random, choose 'Reset Mission Direction.'"
	sui.setPrompt(promptText)

	for i = 1, #self.directions do
		sui.add(self.directions[i].dirDesc, "")
	end

	sui.sendTo(pPlayer)
end

function mission_direction_choice:dirSelection(pPlayer, pSui, eventIndex, args)
	if (eventIndex == 1 or args == "-1") then
		CreatureObject(pPlayer):sendSystemMessage("No direction was selected.")
		return
	end

	local selectedIndex = tonumber(args) + 1
    local selectedDir = tonumber(self.directions[selectedIndex].dirSelect)
    local selectedDesc = self.directions[selectedIndex].dirDesc

    CreatureObject(pPlayer):sendSystemMessage("DIR DEBUG: SelectedDir = " .. selectedDir .. ", Desc = " .. selectedDesc)

writeScreenPlayData(pPlayer, "mission_direction_choice", "directionChoice", selectedDir)



	if selectedDir == 0 then
		CreatureObject(pPlayer):sendSystemMessage("Mission direction reset to default.")
	else
		CreatureObject(pPlayer):sendSystemMessage("Mission direction set to " .. selectedDesc .. ".")
	end
end
