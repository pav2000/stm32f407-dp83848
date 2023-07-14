# stm32f407-dp83848 <br>
Simple web server on STM32F407 and LAN DP83848<br>
IP адрес статика 192.168.0.178 (настройка через Cube.MX свойства LwIP)<br>
Простейший однопоточный web сервер с использованием freeRTOS.<br>
Файлы в web сервера хранятся во флеше микроконтроллера (файл fsdata_custom.c).<br>
Этот файл создается специальной утилитой makeFSdata.exe (входит в поставку CUBE IDE).<br>
Для компиляции примера необходимо ИСКЛЮЧИТЬ файл fsdata_custom.c из компиляции.<br>
Этот файл включается в другой файл с помошью дерективы include, и если этого не сделать (исключить из компиляции) то будет двойное определение переменных - ошибка.<br>
Подробнее: <br>
Ошибка компиляции  fsdata.c:308: first defined here . . .<br>
Решение https://community.st.com/t5/stm32-mcu-products/how-to-add-fsdata-c-file-to-project/td-p/484237<br>
as you can see in fs.c file, fsdata.c is directly included at the begining of fs.c, so it means that the compiler physically copy the content of fsdata.c file at the place of #include 'fsdata.c' directive and then compile the whole file.<br>
<br>
So to fix multiple definition you need to exclude fsdata.c from your project build (for exemple if you use eclipse, right click on the file, select Properties and check 'Exclude from build')<br>
<br>


