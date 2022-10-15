SafeWritingAPI provides you way how to get your own C++ stuff work in mod (Lua).
The only limit is, that you cannot work with CryEngine stuff - players,...
You can return to/accept from Lua only these 4 types to Lua.
So if you feel, that something is missing in mod and you need it (for example you want UploadFile(file,website) ), you can do it with this API.

Here is one example of how-to. See Main.cpp.

Generated .dll goes to ServerFolder/Extensions/Bin32/
You load it using LoadExtension(dllName), i.e. LoadExtension("Example.dll")
To import some your function to Lua, you need first to ->RegisterLuaFunc() it and then ImportFunc(funcName) in Lua, i.e. ImportFunc("helloworld")
If you build 64bit DLL, put it to /Bin64/. LoadExtension recognizes whether server is 64 or 32bit and loads DLL which is same -bit. So if server is 32bit, LoadExtension loads DLL in /Bin32/
