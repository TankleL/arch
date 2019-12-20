echo run win32 post build commnads --- param(%1), param(%2)

rem : param 1 - archserver include path
set archsvr_inc=%1
set archsvr_inc=%archsvr_inc:/=\%
echo archserver inc path: %archsvr_inc%

rem : param 2 - libam include path
set libam_inc=%2
set libam_inc=%libam_inc:/=\%
echo libam inc path: %libam_inc%


rem copy shared header files from archserver to libam
xcopy /D /Y %archsvr_inc%\acquirable.hpp    %libam_inc%\acquirable.hpp
xcopy /D /Y %archsvr_inc%\archmessage.hpp   %libam_inc%\archmessage.hpp
xcopy /D /Y %archsvr_inc%\disposable.hpp    %libam_inc%\disposable.hpp
xcopy /D /Y %archsvr_inc%\protocols.hpp     %libam_inc%\protocols.hpp


