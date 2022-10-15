#include "CryEngine/IGameFramework.h"

#include "maxminddb.h"
#include "SafeWritingAPI.h"

#include <windows.h>

#define DLL_EXPORT __declspec(dllexport)

#define GEOIP_DB_COUNTRY_FILENAME "GeoLite2-Country.mmdb"
#define GEOIP_DB_ASN_FILENAME     "GeoLite2-ASN.mmdb"

#ifdef _WIN64
#define NET_CHANNEL_IP_ADDRESS_OFFSET 0xd0  // 64-bit
#else
#define NET_CHANNEL_IP_ADDRESS_OFFSET 0x78  // 32-bit
#endif

SafeWritingAPI *g_API = NULL;

MMDB_s g_dbCountry;
MMDB_s g_dbASN;
bool g_hasCountry;
bool g_hasASN;

static void GeoIPQueryChannelFunc()
{
	int channelID;
	g_API->GetArg(1, channelID);

	INetChannel *pNetChannel = g_API->GetIGameFramework()->GetNetChannel(channelID);

	const char *countryCode = "ZZ";
	const char *countryName = "N/A";
	int countryNameLength = 3;
	const char *asnOrgName = "N/A";
	int asnOrgNameLength = 3;
	int asnNetmask = 0;
	int asn = 0;

	if (pNetChannel)
	{
		const unsigned char *rawIP = (unsigned char*) pNetChannel + NET_CHANNEL_IP_ADDRESS_OFFSET;  // dirty haxs
		sockaddr_in addr;
		memset(&addr, 0, sizeof addr);
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = (rawIP[3] << 0) | (rawIP[2] << 8) | (rawIP[1] << 16) | (rawIP[0] << 24);
		const sockaddr *pAddr = (const sockaddr*) &addr;

		int status;

		if (g_hasCountry)
		{
			MMDB_lookup_result_s data = MMDB_lookup_sockaddr(&g_dbCountry, pAddr, &status);
			if (status == MMDB_SUCCESS)
			{
				if (data.found_entry)
				{
					MMDB_entry_data_s entry;

					// get country code
					status = MMDB_get_value(&data.entry, &entry, "country", "iso_code", NULL);
					if (status == MMDB_SUCCESS)
					{
						if (entry.has_data && entry.type == MMDB_DATA_TYPE_UTF8_STRING)
						{
							countryCode = entry.utf8_string;
						}
					}

					// get country name
					status = MMDB_get_value(&data.entry, &entry, "country", "names", "en", NULL);
					if (status == MMDB_SUCCESS)
					{
						if (entry.has_data && entry.type == MMDB_DATA_TYPE_UTF8_STRING)
						{
							countryName = entry.utf8_string;
							countryNameLength = entry.data_size;
						}
					}
				}
				else
				{
					countryName = "?";
					countryNameLength = 1;
				}
			}
		}

		if (g_hasASN)
		{
			MMDB_lookup_result_s data = MMDB_lookup_sockaddr(&g_dbASN, pAddr, &status);
			if (status == MMDB_SUCCESS)
			{
				if (data.found_entry)
				{
					MMDB_entry_data_s entry;

					// IPv4 addresses are stored as IPv4-mapped IPv6 addresses
					asnNetmask = data.netmask - 96;

					// get AS number
					status = MMDB_get_value(&data.entry, &entry, "autonomous_system_number", NULL);
					if (status == MMDB_SUCCESS)
					{
						if (entry.has_data && entry.type == MMDB_DATA_TYPE_UINT32)
						{
							asn = entry.uint32;
						}
					}

					// get AS organization name
					status = MMDB_get_value(&data.entry, &entry, "autonomous_system_organization", NULL);
					if (status == MMDB_SUCCESS)
					{
						if (entry.has_data && entry.type == MMDB_DATA_TYPE_UTF8_STRING)
						{
							asnOrgName = entry.utf8_string;
							asnOrgNameLength = entry.data_size;
						}
					}
				}
				else
				{
					asnOrgName = "?";
					asnOrgNameLength = 1;
				}
			}
		}
	}

	g_API->StartReturn();
	g_API->ReturnToLua(countryCode);
	g_API->ReturnToLua(countryName);
	g_API->ReturnToLua(countryNameLength);
	g_API->ReturnToLua(asnOrgName);
	g_API->ReturnToLua(asnOrgNameLength);
	g_API->ReturnToLua(asnNetmask);
	g_API->ReturnToLua(asn);
	g_API->EndReturn();
}

extern "C"
{
	DLL_EXPORT void Init(SafeWritingAPI *pAPI)
	{
		if (g_API)
		{
			return;
		}

		g_API = pAPI;

		g_hasCountry = false;
		g_hasASN = false;

		// try to load country database
		if (MMDB_open(GEOIP_DB_COUNTRY_FILENAME, MMDB_MODE_MMAP, &g_dbCountry) == MMDB_SUCCESS)
		{
			g_hasCountry = true;
		}

		// try to load ASN database
		if (MMDB_open(GEOIP_DB_ASN_FILENAME, MMDB_MODE_MMAP, &g_dbASN) == MMDB_SUCCESS)
		{
			g_hasASN = true;
		}

		g_API->RegisterLuaFunc("GeoIPQueryChannel", GeoIPQueryChannelFunc);
	}
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}
