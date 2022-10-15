--This script was created on 28.10.2012 by Zi;
AddChatCommand("addadmin",function(self,player,msg,target)
	if(not target) then
		return self:EnterValidPlayer(player);
	end
	SafeWriting.Settings.Admins[target.profile]=true;
	if(not SafeWriting.Settings.UseAuthentificationPassword)then
		target.IsAdminLogged=true;
	end
	Chat:SendToTarget(nil,player,"[[Hrac ___ bol pridany medzi]]",target:GetName(),__qt(player.lang,"adminov"));
end,{PLAYER},{AdminOnly=true;},"[[addadmin_info]]");
AddChatCommand("deladmin",function(self,player,msg,target)
	if(not target) then
		return self:EnterValidPlayer(player);
	end
	SafeWriting.Settings.Admins[target.profile]=false;
	target.IsAdminLogged=false;
	Chat:SendToTarget(nil,player,"[[Hrac ___ bol odobrany z]]",target:GetName(),__qt(player.lang,"adminov"));
end,{PLAYER},{AdminOnly=true;},"[[deladmin_info]]");
AddChatCommand("addmoderator",function(self,player,msg,target)
	if(not target) then
		return self:EnterValidPlayer(player);
	end
	SafeWriting.Settings.Moderators[target.profile]=true;
	if(not SafeWriting.Settings.UseAuthentificationPassword)then
		target.IsModeratorLogged=true;
	end
	Chat:SendToTarget(nil,player,"[[Hrac ___ bol pridany medzi]]",target:GetName(),__qt(player.lang,"moderatorov"));
end,{PLAYER},{AdminOnly=true;},"[[addmoderator_info]]");
AddChatCommand("delmoderator",function(self,player,msg,target)
	if(not target) then
		return self:EnterValidPlayer(player);
	end
	SafeWriting.Settings.Moderators[target.profile]=false;
	target.IsModeratorLogged=true;
	Chat:SendToTarget(nil,player,"[[Hrac ___ bol odobrany z]]",target:GetName(),__qt(player.lang,"moderatorov"));
end,{PLAYER},{AdminOnly=true;},"[[delmoderator_info]]");
AddChatCommand("addpremium",function(self,player,msg,target)
	if(not target) then
		return self:EnterValidPlayer(player);
	end
	SafeWriting.Settings.Premiums[target.profile]=true;
	if(not SafeWriting.Settings.UseAuthentificationPassword)then
		target.IsPremiumLogged=true;
	end
	Chat:SendToTarget(nil,player,"[[Hrac ___ bol pridany medzi]]",target:GetName(),__qt(player.lang,"premium hracov"));
end,{PLAYER},{AdminOnly=true;},"[[addpremium_info]]");
AddChatCommand("delpremium",function(self,player,msg,target)
	if(not target) then
		return self:EnterValidPlayer(player);
	end
	SafeWriting.Settings.Premiums[target.profile]=false;
	target.IsPremiumLogged=false;
	Chat:SendToTarget(nil,player,"[[Hrac ___ bol odobrany z]]",target:GetName(),__qt(player.lang,"premium hracov"));
end,{PLAYER},{AdminOnly=true;},"[[delpremium_info]]");