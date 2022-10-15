NightLightsPlugin={Enabled=true; LightsEnabled=false; };
function NightLightsPlugin_Do()
	--if(IsDllLoaded())then
		local t=tonumber(System.GetCVar("e_time_of_day"));
		if(t>18.5 or t<6.5)then
			if(not NightLightsPlugin.LightsEnabled)then
				ForceSet("v_lights_enable_always","1");
				NightLightsPlugin.LightsEnabled=true;
			end
		else
			if(NightLightsPlugin.LightsEnabled)then
				ForceSet("v_lights_enable_always","0");
				NightLightsPlugin.LightsEnabled=false;
			end
		end
	--end
end
if(NightLightsPlugin.Enabled)then
	SafeWriting.FuncContainer:AddFunc(NightLightsPlugin_Do,"PrepareAll");
	SafeWriting.FuncContainer:AddFunc(NightLightsPlugin_Do,"OnTimerTick");
end