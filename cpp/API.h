#ifndef MASTER_API
#define MASTER_API

#include <map>
#include <string>

#define ABI __cdecl

#ifndef OPCODES
#define OPCODES
enum MessageID{
	NOP = 0,
	DISCONNECT,
	SERVER_LIST,
	SERVER_INFO,
	SERVER_UPDATE,
	RESPONSE
};
enum ReturnCodes{
	NO_ERROR,
	UNKNOWN_ERROR
};
#define EXTRA_MASK (0x80)
#endif

struct Pair{
	const char *First;
	const char *Second;
};
struct Pairs{
	Pair *values;
	int count;
};

typedef void(ABI *PFNRESPONSECALLBACK)(int,Pairs*);
typedef void(ABI *PFNSETRESPONSECALLBACK)(PFNRESPONSECALLBACK);
typedef void(ABI *PFNSENDPACKET)(int, Pairs*);
typedef bool(ABI *PFNINIT)(const char*, int);

struct Response : std::map < std::string, std::string > {
	int Code;
	Response(Pairs *pairs, int code = NOP) : Code(code){
		for (int i = 0; i < pairs->count; i++){
			this->insert(std::make_pair(std::string(pairs->values[i].First), std::string(pairs->values[i].Second)));
		}
	}
	Response(int code, std::map<std::string, std::string> Data) :Code(code){
		for (std::map<std::string, std::string>::iterator& it = Data.begin(); it != Data.end(); it++){
			this->insert(std::make_pair(it->first,it->second));
		}
	}
	void send(PFNSENDPACKET helper,Pairs *out=0){
		bool own = false;
		if (out == 0){
			out = new Pairs;
			own = true;
		}
		out->count = (int)size();
		out->values = new Pair[out->count+1];
		int i = 0;
		for (iterator& it = begin(); it != end(); it++){
			out->values[i].First = it->first.c_str();
			out->values[i].Second = it->second.c_str();
			i++;
		}
		helper(Code, out);
		if (out->values)
			delete[] out->values;
		if (own)
			delete out;
	}
};
#endif