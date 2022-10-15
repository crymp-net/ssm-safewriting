LoadExtension("SafeWritingAPIExample.dll");	--Load our DLL
ImportFunc("operations"); --Import our "operations" function
--Create server console command "example_test"
System.AddCCommand("example_test",[[
	print(operations(22,7));
]],"...");