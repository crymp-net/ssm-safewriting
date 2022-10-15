@echo off
set GameFolder=..\
set ServerFolder=%CD%
title My Awesome Server

goto OS32

:OS32
cd %GameFolder%
echo Server is starting (32 bit)
Bin32\CrysisDedicatedServer.exe -root %ServerFolder% +exec "server.cfg" -mod SafeWriting
echo Server was closed

::Uncomment following line if you want server to auto reboot after crash
::goto OS32

goto END
:END
cd %ServerFolder%