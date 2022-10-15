#pragma once
int FileDecrypt(const char *name, char **out);
void FileEncrypt(const char *name, const char *out);
#define GetGameFolder getGameFolder
bool PostInitScripts(bool force = false);