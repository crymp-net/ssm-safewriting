SafeWriting.FuncContainer:AddFunc(function()

	LoadExtension("GeoIP.dll")
	ImportFunc("GeoIPQueryChannel")

	g_gameRules.game:SetSynchedGlobalValue(1000, "who knows :)")

end, "PrepareAll")

SafeWriting.FuncContainer:AddFunc(function(player, name, reset, channelId)

	if channelId then
		local countryCode, countryName, countryNameLength, asnName, asnNameLength, asnNetmask, asn = GeoIPQueryChannel(channelId)
		player.countryCode = string.sub(countryCode, 1, 2);
		player.countryName = string.sub(countryName, 1, countryNameLength);
		player.asnName = string.sub(asnName, 1, asnNameLength);
		player.asnNetmask = asnNetmask;
		player.asn = asn;
	end

	if player then
		local playerName = player:GetName():gsub("%$%d", "")
		if player.countryName and player.asnName then
			Chat:SendToAll(playerName .. " has connected (" .. player.countryName .. " | " .. player.asnName .. ")")
		else
			Chat:SendToAll(playerName .. " has connected")
		end
	end

end, "OnClientConnect")

SafeWriting.FuncContainer:AddFunc(function(player, channelId)

	if player then
		local playerName = player:GetName():gsub("%$%d", "")
		Chat:SendToAll(playerName .. " has disconnected")
	end

end, "OnClientDisconnect")
