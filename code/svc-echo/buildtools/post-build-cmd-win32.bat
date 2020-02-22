echo ">>>>>>>>>>>>>>>>>>>> POST BUILD <<<<<<<<<<<<<<<<<<<<"
echo run win32 post build commnads --- param(%1), param(%2)

rem : param 1 - project binary dir
set proj_bin_dir=%1
set proj_bin_dir=%proj_bin_dir:/=\%
echo project binary dir: %proj_bin_dir%

rem : param 2 - libam include path
set gsvr_root=%2
set gsvr_root=%gsvr_root:/=\%
echo game server root: %gsvr_root%

rem : copy this service to game server root
xcopy /D /Y %proj_bin_dir%\svc-echo\Debug\svc_echo.exe  %gsvr_root%\services\echo\svc_echo.exe*


