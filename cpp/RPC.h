#pragma once
#include <vector>
int DeployRPCServer(int port);
bool CheckClientID(const char *id, int channelId);
bool CloseClientID(const char *id);
void IterateClients();
int SendMessageToClient(const char *clientId, const char *method, std::vector<const char*>& args);