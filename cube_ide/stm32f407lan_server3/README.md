# stm32f407-dp83848 <br>
Simple web server on STM32F407 and LAN DP83848<br>
IP адрес статика 192.168.0.178 (настройка через Cube.MX свойства LwIP)<br>
Простейший однопоточный web сервер с использованием freeRTOS + ajax.<br>
Файлы в web сервера хранятся на sd карте.<br>
Файлы (из директории fs) надо скинуть на sd карту.<br>
Led2 при работе толжен мигать раз в секунду (индикация что система не упала).<br>
Достаточно долго бился что бы карта работала стабильно 09.08.23 - вроде получилось рабоатет более суток и сайт грузиться.
Ранее была ошибка 1 fatfs что значить ошибка ввода вывода нижнего уровня при открытии файла.<br>
Кеализована работа с ajax запросами.<br>
Подробне по настройке sd карты под freeRTOS <br>
http://ipasoft.info/index.php/9-blog/13-stm32f4-rabota-s-flash-sdio-microsd-pod-freertos <br>
https://blog.csdn.net/dxc8865de2/article/details/107914150 <br>
https://blog.csdn.net/xuan530482366/article/details/123713602 <br>

<br>


