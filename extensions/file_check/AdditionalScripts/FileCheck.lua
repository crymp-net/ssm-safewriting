SafeWriting.FuncContainer:AddFunc(function()

	LoadExtension("FileCheck.dll")
	ImportFunc("FileCheckInit")

	FileCheckInit("OnFileMismatch")

end, "PrepareAll")

function OnFileMismatch(channelId, file)
	local player = g_gameRules.game:GetPlayerByChannelId(channelId)

	if not player
	 or file == "game/scripts/entities/items/xml/weapons/multiplayer/law.xml"  -- sfwcl #1
	 or file == "game/scripts/entities/items/xml/offhand.xml"  -- sfwcl #2
	then
		return
	end

	if not player.modifiedFiles
	then
		player.modifiedFiles = {}
	end

	if not player.modifiedFiles[file]
	then
		player.modifiedFiles[file] = true

		local maxCount = 5
		local playerName = player:GetName():gsub("%$%d", "")

		local count = 0
		for _ in pairs(player.modifiedFiles)
		do
			count = count + 1
		end

		Chat:SendToAll(playerName .. " uses modified file (" .. count .. "/" .. maxCount .. "): " .. file)

		if count > maxCount
		then
			KickPlayer(player, "Too many modified files")
			Chat:SendToAll(playerName .. " has been kicked for too many modified files")
		end
	end
end
