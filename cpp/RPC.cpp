#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define USE_LIB_EVENT
#ifdef USE_LIB_EVENT
#include <event.h>
#endif
#include <string>
#include <map>
#include "RPC.h"
#include "CPPAPI.h"

#pragma comment(lib, "Ws2_32")
#ifdef USE_LIB_EVENT
#pragma comment(lib, "event")
#pragma comment(lib, "event_core")
#endif
using namespace std;

#define BUFFER_SIZE 32768
#define MAX_NAME 40
#define ID_LEN 32

Mutex rpcMutex;

struct Client;
std::map<std::string, Client*> *clientMapping;
std::map<SOCKET, Client*> *clients;
SOCKET server;

unsigned long rpc_mem_usage = 0;
unsigned long rpc_active_instances = 0;

struct Client {
#ifdef USE_LIB_EVENT
	event ev_read;
	event ev_write;
#endif
	bool closeFlag;
	int channelId;
	char *name;
	sockaddr_in ca;
	SOCKET sock;
	int expect;
	int received;
	bool head;
	unsigned char *buffer;
	char *id;
	void execute() {
		std::vector<std::string> args;
		std::string arg = "";
		for (int i = 0; i < received; i++) {
			char n = (char)((buffer[i] ^ 0x5A) & 0xFF);
			if (n == 0) {
				args.push_back(arg);
				arg = "";
			} else arg += n;
		}
		RPCEvent *evt = new RPCEvent(id, args);
		evt->callAsync();
	}
	void send(const char *data, int len) {
		::send(sock, (char*)&len, 4, 0);
		::send(sock, data, len, 0);
	}
	Client(sockaddr_in sai, SOCKET client) : buffer(0), name(0) {
		rpc_active_instances++;
		closeFlag = false;
		rpc_mem_usage += sizeof(Client);
		ca = sai;
		sock = client;
		buffer = new unsigned char[BUFFER_SIZE];
		name = new char[MAX_NAME];
		id = new char[ID_LEN];
		rpc_mem_usage += BUFFER_SIZE + MAX_NAME + ID_LEN;
		expect = 4;
		received = 0;
		head = true;
		snprintf(name, MAX_NAME, "%s:%d", inet_ntoa(ca.sin_addr), ntohs(ca.sin_port));
#ifdef USE_LIB_EVENT
		memset(&ev_read, 0, sizeof(ev_read));
		memset(&ev_write, 0, sizeof(ev_write));
#endif
		memset(id, 0, ID_LEN);
		for (int i = 0; i < 20; i++) {
			id[i] = 33 + rand() % 90;
		}
		(*clientMapping)[id] = this;
		std::vector<const char*> args = { id };
		sendMessage("id", args);
	}
	Client() :buffer(0), name(0) {
		rpc_active_instances++;
		rpc_mem_usage += sizeof(Client);
		closeFlag = false;
	}
	~Client() {
		rpc_active_instances--;
		std::map<std::string, Client*>::iterator it = clientMapping->find(id);
		std::map<SOCKET, Client*>::iterator it2 = clients->find(sock);
		if (it != clientMapping->end()) {
			clientMapping->erase(it);
		}
		if (it2 != clients->end()) {
			clients->erase(it2);
		}
		closesocket(sock);
		if (buffer) {
			delete[] buffer;
			buffer = 0;
			rpc_mem_usage -= BUFFER_SIZE;
		}
		if (name) {
			delete[] name;
			name = 0;
			rpc_mem_usage -= MAX_NAME;
		}
		if (id) {
			delete[] id;
			id = 0;
			rpc_mem_usage -= ID_LEN;
		}
		rpc_mem_usage -= sizeof(Client);
	}
	void sendMessage(const char *method, std::vector<const char*>&args) {
		std::string str = method;
		str += '\0';
		for (auto& arg : args) {
			str += arg; str += '\0';
		}
		for (size_t i = 0; i < str.size(); i++) {
			str[i] = (str[i] ^ 0x5A) & 0xFF;
		}
		send(str.c_str(), str.size());
	}
	int onRecv(int n) {
		if (closeFlag) return 1;
		expect -= n; received += n;
		if (expect == 0) {
			if (head) {
				received = 0;
				int len = 0;
				len |= buffer[3]; len <<= 8;
				len |= buffer[2]; len <<= 8;
				len |= buffer[1]; len <<= 8;
				len |= buffer[0];
				if (len >= BUFFER_SIZE - 4) {
					return 1;
				}
				expect = len;
				head = false;
				memset(buffer, 0, BUFFER_SIZE);
			}
			else {
				execute();
				head = true;
				expect = 4;
				received = 0;
			}
		}
		return 0;
	}
};


void RPCOnRead(int fd, short ev, void *arg) {
#ifdef USE_LIB_EVENT
	SimpleLock lock(rpcMutex);
	Client *cl = (Client*)arg;
	if (!cl) return;
	int n = recv(fd, (char*)cl->buffer + cl->received, cl->expect, 0);
	if (n <= 0) {
		event_del(&cl->ev_read);
		delete cl;
		return;
	} else cl->onRecv(n);
#endif
}

void RPCOnAccept(int fd, short ev, void *arg) {
#ifdef USE_LIB_EVENT
	SimpleLock lock(rpcMutex);
	sockaddr_in ca;
	int sl = sizeof(ca);
	SOCKET client = accept(fd, (sockaddr*)&ca, &sl);
	if (client != -1) {
		unsigned long mode = 1;
		ioctlsocket(server, FIONBIO, &mode);
		Client *cl = new Client(ca, client);
		(*clients)[client] = cl;
		event_set(&cl->ev_read, client, EV_READ | EV_PERSIST, RPCOnRead, cl);
		event_add(&cl->ev_read, 0);
	}
#endif
}

void IterateClients() {
	extern IScriptSystem *pScriptSystem;
	SimpleLock lock(rpcMutex);
	for (std::map<std::string, Client*>::iterator it = clientMapping->begin(); it != clientMapping->end();it++) {
		Client* cl = it->second;
		pScriptSystem->BeginCall("printf");
		pScriptSystem->PushFuncParam(" %s (%s), socket: %d, exp/recv: %d/%d, close: %d");
		pScriptSystem->PushFuncParam(cl->name);
		pScriptSystem->PushFuncParam(cl->id);
		pScriptSystem->PushFuncParam((int)cl->sock);
		pScriptSystem->PushFuncParam((int)cl->expect);
		pScriptSystem->PushFuncParam((int)cl->received);
		pScriptSystem->PushFuncParam((int)cl->closeFlag);
		pScriptSystem->EndCall();
	}
}
int SendMessageToClient(const char *clientId, const char *method, std::vector<const char*>& args) {
	SimpleLock lock(rpcMutex);
	std::map<std::string, Client*>::iterator it = clientMapping->find(clientId);
	if (it == clientMapping->end()) {
		return 0;
	}
	it->second->sendMessage(method, args);
	return 1;
}
bool CheckClientID(const char *id, int channelId) {
	SimpleLock lock(rpcMutex);
	if (!clientMapping) return false;
	std::map<std::string, Client*>::iterator it = clientMapping->find(id);
	if (it == clientMapping->end()) {
		return false;
	}
	it->second->channelId = channelId;
	return true;
}
bool CloseClientID(const char *id) {
	SimpleLock lock(rpcMutex);
	if (!clientMapping) return false;
	std::map<std::string, Client*>::iterator it = clientMapping->find(id);
	if (it == clientMapping->end()) return false;
	it->second->closeFlag = true;
	closesocket(it->second->sock);
	return true;
}
int DeployRPCServer(int port) {
	srand((unsigned int)time(0));
	clients = new std::map<SOCKET, Client*>();
	clientMapping = new std::map<std::string, Client*>();
	WSADATA wsa;
	WSAStartup(0x202, &wsa);

#ifdef USE_LIB_EVENT
	event_init();
#endif

	server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in sa;
	ZeroMemory(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = 0;
	if (bind(server, (const sockaddr*)&sa, sizeof(sa))) {
		return 1;
	}
	listen(server, 80);
	unsigned long mode = 1;
	ioctlsocket(server, FIONBIO, &mode);

#ifdef USE_LIB_EVENT
	struct event ev_accept;

	event_set(&ev_accept, server, EV_READ | EV_PERSIST, RPCOnAccept, 0);
	event_add(&ev_accept, 0);

	event_dispatch();
#else

	fd_set master, read_fds;
	FD_ZERO(&master); FD_ZERO(&read_fds);
	FD_SET(server, &master);
	SOCKET fdmax = server;

	while (true) {
		read_fds = master;
		if (select((int)(fdmax + 1), &read_fds, 0, 0, 0) == -1) {
			return 1;
		}
		for (SOCKET i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == server) {
					int cl = sizeof(sockaddr_in);
					sockaddr_in ca;
					SOCKET client = accept(server, (sockaddr*)&ca, &cl);
					if (client != -1){
						FD_SET(client, &master);
						if (client > fdmax) {
							fdmax = client;
						}
						Client *cl = new Client(ca, client);
						(*clients)[client] = cl;
					}
				} else {
					std::map<SOCKET, Client*>::iterator& it = clients->find(i);
					if (it != clients->end()) {
						auto& cl = it->second;
						int n = 0;
						if(cl->closeFlag)
							recv(i, (char*)cl->buffer + cl->received, cl->expect, 0);
						if (n <= 0 || cl->onRecv(n)) {
							closesocket(i);
							FD_CLR(i, &master);
							clients->erase(it);
							delete cl;
						}
					} else {
						closesocket(i);
						FD_CLR(i, &master);
					}
				}
			}
		}
	}
	
#endif
	return 0;
}