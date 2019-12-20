echo run win32 post build commnads ...
rem : check input parameters 
echo proj_bin root = %1
echo module-archd root = %2
echo test-env root = %3

rem : covert and store input parameters
set proj_bin_root=%1
set proj_bin_root=%proj_bin_root:/=\%

set module_archd_root=%2
set module_archd_root=%module_archd_root:/=\%

set test_env_root=%3
set test_env_root=%test_env_root:/=\%

rem make local vars
set module_archd_bin_root=%proj_bin_root%\module-archd
echo module_archd_bin_root = %module_demo_bin_root%

rem copy dll file to test environment
xcopy /D /Y %module_archd_bin_root%\Debug\mdlarchd.dll %test_env_root%\modules\Module-Archd\


