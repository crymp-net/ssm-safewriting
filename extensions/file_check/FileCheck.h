#pragma once

struct INetChannel;

typedef void (*TFileCheckCallback)(INetChannel *pNetChannel, const char *file, void *param);

bool FileCheckInit(TFileCheckCallback callback, void *param);
