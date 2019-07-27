echo run win32 post build commnads ...
rem : check input parameters 
echo proj_bin root = %1
echo module-demo root = %2
echo test-env root = %3

rem : covert and store input parameters
set proj_bin_root=%1
set proj_bin_root=%proj_bin_root:/=\%

set module_demo_root=%2
set module_demo_root=%module_demo_root:/=\%

set test_env_root=%3
set test_env_root=%test_env_root:/=\%

rem make local vars
set module_demo_bin_root=%proj_bin_root%\module-demo
echo module_demo_bin_root = %module_demo_bin_root%

rem copy dll file to test environment
xcopy /D /Y %module_demo_bin_root%\Debug\mdldemo.dll %test_env_root%\modules\Module-Demo\


