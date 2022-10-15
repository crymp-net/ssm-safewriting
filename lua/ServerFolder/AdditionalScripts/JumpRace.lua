-------------------------------
-- Jump race with containers --
-------------------------------

if (not g_jumpRace) then
	g_jumpRace = {};
	g_jumpRace.size = 0;
	g_jumpRace.isReady = false;
	g_jumpRace.scoreTable = {};
	g_jumpRace.startPos = {};
	g_jumpRace.startDir = {};
	g_jumpRace.spawnPos = {};
	g_jumpRace.endContainer = nil;
	g_jumpRace.buildProgress = 0;
	g_jumpRace.spawnCounter = 0;
end

function JumpRaceGetRandomContainerModel()
	local colors = { "red", "green", "blue" };
	local num = math.random(3);
	return "objects/library/storage/crates/container/container_" .. colors[num] .. ".cgf";
end

function JumpRaceHandlePlayerFinish(player)
	local finishTime = _time - player.jumpRaceJoinTime;
	player.jumpRaceFinished = true;

	g_gameRules:CreateExplosion(player.id, weaponId, 0, player:GetWorldPos(), g_Vectors.up, 1, 1, 1, 1, "explosions.flare.night_time", 1.6, 1, 1, 1);

	g_jumpRace.scoreTable[#g_jumpRace.scoreTable + 1] = { player:GetName(), finishTime };
	table.sort(g_jumpRace.scoreTable, function(a, b)
		return a[2] < b[2];
	end);

	Chat:SendToAll("[JumpRace] " .. player:GetName() .. " finished the race in " .. string.format("%.1f", finishTime) .. " seconds");
end

function JumpRaceSpawnBox(spawnParams)
	spawnParams.name = "JumpRaceBox" .. tostring(g_jumpRace.spawnCounter);
	local entity = System.SpawnEntity(spawnParams);
	g_jumpRace.spawnCounter = g_jumpRace.spawnCounter + 1;
	return entity;
end

function JumpRaceBuild()
	if g_jumpRace.size <= 0 then
		Chat:SendToAll("[JumpRace] Generation stopped");
		return;
	end

	local step = 10;  -- max number of blocks spawned at once

	if (g_jumpRace.buildProgress + step) > g_jumpRace.size then
		step = g_jumpRace.size - g_jumpRace.buildProgress;
	end

	local spawnParams = {};
	spawnParams.class = "CustomAmmoPickup";
	spawnParams.position = g_jumpRace.spawnPos;
	spawnParams.orientation = { x = 1; y = 0; z = 0 };
	spawnParams.properties = {};
	spawnParams.properties.bUsable = 0;
	spawnParams.properties.bPickable = 1;
	spawnParams.properties.bPhysics = 0;

	for i = 1, step, 1 do
		spawnParams.properties.objModel = JumpRaceGetRandomContainerModel();
		spawnParams.position.x = spawnParams.position.x + math.random() + math.random(-6, 6);
		spawnParams.position.y = spawnParams.position.y + math.random() + math.random(-6, 6);
		spawnParams.position.z = spawnParams.position.z + math.random() + math.random(-4, 6);
		spawnParams.orientation.y = math.random() + math.random(-1, 1);
		local entity = JumpRaceSpawnBox(spawnParams);
		local posZ = spawnParams.position.z;

		local num = math.random(-1, 2);
		for j = 1, num, 1 do
			spawnParams.position.z = spawnParams.position.z + 2.82;
			entity = JumpRaceSpawnBox(spawnParams);
		end

		if g_jumpRace.endContainer then
			local endPos = g_jumpRace.endContainer:GetPos();
			if spawnParams.position.z >= endPos.z then
				g_jumpRace.endContainer = entity;
			end
		else
			g_jumpRace.endContainer = entity;
		end

		spawnParams.position.z = posZ;

		num = math.random(-2, 2);
		for j = 1, num, 1 do
			spawnParams.position.z = spawnParams.position.z - 2.82;
			entity = JumpRaceSpawnBox(spawnParams);
		end

		spawnParams.position.z = posZ;
	end

	g_jumpRace.spawnPos = spawnParams.position;
	g_jumpRace.buildProgress = g_jumpRace.buildProgress + step;

	if g_jumpRace.buildProgress < g_jumpRace.size then
		local delay = 200;  -- number of milliseconds between steps
		Script.SetTimer(delay, function()
			JumpRaceBuild();
		end);
		return;
	end

	local endPos = g_jumpRace.endContainer:GetPos();
	local endDir = g_jumpRace.endContainer:GetDirectionVector();
	endPos.z = endPos.z + 6;

	local flag = {
		class = "Flag";
		position = endPos;
		orientation = endDir;
		name = "JumpRaceEndFlag";
	};
	System.SpawnEntity(flag);

	local trigger = {
		class = "ServerTrigger";
		position = endPos;
		orientation = endDir;
		name = "JumpRaceEndTrigger";
		properties = {
			DimX = 5;
			DimY = 5;
			DimZ = 10;
			EnterCallback = function(entity, areaId)
				if entity.jumpRaceJoined and not entity.jumpRaceFinished and entity.actor:GetSpectatorMode() == 0 then
					JumpRaceHandlePlayerFinish(entity);
				end
			end;
		};
	};
	System.SpawnEntity(trigger);

	Chat:SendToAll("[JumpRace] New race generated");
end

function JumpRaceDestroy()
	local wasReady = g_jumpRace.isReady;
	g_jumpRace.isReady = false;
	g_jumpRace.size = 0;
	g_jumpRace.scoreTable = {};
	g_jumpRace.endContainer = nil;

	local players = System.GetEntitiesByClass("Player");
	if players then
		for i, v in pairs(players) do
			v.jumpRaceJoined = false;
			v.jumpRaceFinished = false;
			--v.savedPos = nil;
			--v.savedDir = nil;
		end
	end

	local entities = System.GetEntitiesByClass("CustomAmmoPickup");
	local counter = 0;
	if entities then
		for i, v in pairs(entities) do
			if v:GetName():match("^JumpRace") then
				System.RemoveEntity(v.id);
				counter = counter + 1;
			end
		end
	end

	entities = System.GetEntitiesByClass("Flag");
	if entities then
		for i, v in pairs(entities) do
			if v:GetName():match("^JumpRace") then
				System.RemoveEntity(v.id);
			end
		end
	end

	entities = System.GetEntitiesByClass("ServerTrigger");
	if entities then
		for i, v in pairs(entities) do
			if v:GetName():match("^JumpRace") then
				System.RemoveEntity(v.id);
			end
		end
	end

	if wasReady then
		Chat:SendToAll("[JumpRace] Race has been destroyed");
	end

	return counter;
end

AddChatCommand("jumpbuild", function(self, player, msg, size)

	if size == nil or size <= 0 then
		Chat:SendToTarget(nil, player, "Enter valid number of blocks");
		return;
	end

	local minSize = 10;  -- min number of blocks

	if size < minSize then
		Chat:SendToTarget(nil, player, "Size must be at least " .. minSize .. " blocks");
		return;
	end

	JumpRaceDestroy();

	g_jumpRace.size = size;
	g_jumpRace.spawnPos = player:GetPos();
	g_jumpRace.buildProgress = 0;
	g_jumpRace.spawnCounter = 0;

	Chat:SendToAll("[JumpRace] Generating new race...");

	JumpRaceBuild();

end, {NUMBER}, {AdminOnly=true;}, "Generates new jump race");

AddChatCommand("jumpclear", function(self, player, msg)

	if g_jumpRace.size <= 0 then
		Chat:SendToTarget(nil, player, "No jump race exists");
		return;
	end

	local num = JumpRaceDestroy();

	Chat:SendToTarget(nil, player, "Removed " .. num .. " containers");

end, nil, {AdminOnly=true;}, "Removes generated jump race");

AddChatCommand("jumpsetstart", function(self, player, msg)

	if g_jumpRace.size <= 0 then
		Chat:SendToTarget(nil, player, "No jump race exists");
		return;
	end

	if g_jumpRace.size > g_jumpRace.buildProgress then
		Chat:SendToTarget(nil, player, "The race is still under construction");
		return;
	end

	if g_jumpRace.isReady then
		g_jumpRace.scoreTable = {};
	end

	g_jumpRace.startPos = player:GetPos();
	g_jumpRace.startDir = player:GetWorldAngles();
	g_jumpRace.isReady = true;

	Chat:SendToAll("[JumpRace] Race is prepared, use !jumpjoin to join it");

end, nil, {AdminOnly=true;}, "Sets start position of the generated jump race");

AddChatCommand("jumpstatus", function(self, player, msg)

	if g_jumpRace.size <= 0 then
		Chat:SendToTarget(nil, player, "No jump race exists");
		return;
	end

	if g_jumpRace.size > g_jumpRace.buildProgress then
		local num = g_jumpRace.buildProgress;
		local percentage = 100.0 * num / g_jumpRace.size;
		Chat:SendToTarget(nil, player, num .. " blocks out of " .. g_jumpRace.size .. " generated (" .. string.format("%.1f", percentage) .. "% completed)");
		return;
	end

	Chat:SendToTarget(nil, player, "Jump race is generated (" .. g_jumpRace.size .. " blocks, " .. g_jumpRace.spawnCounter .. " containers)");

end, nil, nil, "Shows status of current jump race");

AddChatCommand("jumpjoin", function(self, player, msg)

	if g_jumpRace.size <= 0 then
		Chat:SendToTarget(nil, player, "No jump race exists");
		return;
	end

	if g_jumpRace.size > g_jumpRace.buildProgress then
		Chat:SendToTarget(nil, player, "The race is still under construction");
		return;
	end

	if (not g_jumpRace.isReady) then
		Chat:SendToTarget(nil, player, "Start position of the race is not set");
		return;
	end

	if (not player.jumpRaceJoined) then
		player.jumpRaceJoinPos = player:GetPos();
		player.jumpRaceJoinDir = player:GetWorldAngles();
	end

	--player.savedPos = nil;
	--player.savedDir = nil;
	player.jumpRaceJoined = true;
	player.jumpRaceFinished = false;
	player.jumpRaceJoinTime = _time;

	TeleportPlayer(player, g_jumpRace.startPos, g_jumpRace.startDir);  -- SafeWriting specific function

	Chat:SendToAll("[JumpRace] " .. player:GetName() .. " teleported to start of the race");

end, nil, nil, "Teleports you to start of jump race");

AddChatCommand("jumpleave", function(self, player, msg)

	if (not player.jumpRaceJoined) then
		Chat:SendToTarget(nil, player, "You are not in jump race");
		return;
	end

	TeleportPlayer(player, player.jumpRaceJoinPos, player.jumpRaceJoinDir);  -- SafeWriting specific function

	player.jumpRaceJoined = false;
	player.jumpRaceJoinPos = nil;
	player.jumpRaceJoinDir = nil;
	--player.savedPos = nil;
	--player.savedDir = nil;

	if g_jumpRace.isReady then
		Chat:SendToAll("[JumpRace] " .. player:GetName() .. " left the race");
	end

end, nil, nil, "Stops timed race for you");

AddChatCommand("jumptop", function(self, player, msg)

	if (not g_jumpRace.isReady) then
		Chat:SendToTarget(nil, player, "No jump race is prepared");
		return;
	end

	if #g_jumpRace.scoreTable == 0 then
		Chat:SendToTarget(nil, player, "Nobody has finished the race yet");
		return;
	end

	Console:SendToTarget(player, "$9%4s $8%-20s $9%s", "Rank", "Name", "Time [s]");
	for i, v in ipairs(g_jumpRace.scoreTable) do
		Console:SendToTarget(player, "$3%4d $6%-20s $8%.1f", i, v[1], v[2]);
	end
	self:OpenConsole(player);  -- SafeWriting specific function

end, nil, nil, "Shows score table of current jump race");
