--------------------------------------------------------------------------
--	Crytek Source File.
-- 	Copyright (C), Crytek Studios, 2001-2004.
--------------------------------------------------------------------------
--	$Id$
--	$DateTime$
--	Description: Spawn Point
--  
--------------------------------------------------------------------------
--  History:
--  - 24:9:2004   12:00 : Created by Mathieu Pinard
--
--------------------------------------------------------------------------

SpawnPoint = {
	Client = {},
	Server = {},

	Editor={
		Model="Editor/Objects/spawnpointhelper.cgf",
		--Icon="SpawnPoint.bmp",
		--DisplayArrow=1,
	},
	
	Properties=
	{
		teamName="",
		bEnabled=1,
	},
}

--------------------------------------------------------------------------
function SpawnPoint.Server:OnInit()
	--Log("Spawnpoint: init");
	if not g_gameRules.OnChatMessage then
		--System.LogAlways("$6[SSM SafeWriting] Replacing old game rules");
		Script.UnloadScript("scripts/modfiles/safewritinggamerules.lua");
		Script.LoadScript("scripts/modfiles/safewritinggamerules.lua");
		--PrepareAll();
		--_G.SafeWritingGameRulesLoaded=true;
	end
	g_gameRules.game:SetTeam(g_gameRules.game:GetTeamId(self.Properties.teamName) or 0, self.id);
	self:Enable(tonumber(self.Properties.bEnabled)~=0);	
end
----------------------------------------------------------------------------------------------------
function SpawnPoint:Enable(enable)
	if (enable) then
		g_gameRules.game:AddSpawnLocation(self.id);
	else
		g_gameRules.game:RemoveSpawnLocation(self.id);
	end
	self.enabled=enable;
end

--------------------------------------------------------------------------
function SpawnPoint.Server:OnShutDown()
	if (g_gameRules) then
		g_gameRules.game:RemoveSpawnLocation(self.id);
	end
end

--------------------------------------------------------------------------
function SpawnPoint:Spawned(entity)
	BroadcastEvent(self, "Spawn");
end

--------------------------------------------------------------------------
function SpawnPoint:IsEnabled()
	return self.enabled;
end

--------------------------------------------------------------------------
-- Event is generated when something is spawned using this spawnpoint
--------------------------------------------------------------------------
function SpawnPoint:Event_Spawn()
	local player = g_localActor;
	player:SetWorldPos(self:GetWorldPos(g_Vectors.temp_v1));		
	--set angles
	player:SetWorldAngles(self:GetAngles(g_Vectors.temp_v1));
				
	BroadcastEvent(self, "Spawn");
end

SpawnPoint.FlowEvents =
{
	Inputs =
	{
		Spawn = { SpawnPoint.Event_Spawn, "bool" },
	},
	Outputs =
	{
		Spawn = "bool",
	},
}
