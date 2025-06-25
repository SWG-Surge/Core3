object_tangible_gem_clothing = object_tangible_gem_shared_clothing:new {

}

function object_tangible_gem_clothing:onObjectReady(player)
    local mods = self:getSkillMods()
    for modName, _ in pairs(mods) do
        local label = modName:gsub("_", " "):gsub("(%l)(%w*)", function(a,b) return a:upper()..b end)
        self:setCustomObjectName(label)
        self:setCustomName(label)
        break
    end
end

ObjectTemplates:addTemplate(object_tangible_gem_clothing, "object/tangible/gem/clothing.iff")
