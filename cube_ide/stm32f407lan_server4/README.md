# stm32f407-dp83848 <br>
Simple web server on STM32F407 and LAN DP83848<br>
IP адрес статика 192.168.0.178 (настройка через Cube.MX свойства LwIP)<br>
Простейший однопоточный web сервер с использованием freeRTOS + ajax.<br>
Файлы в web сервера хранятся на sd карте.<br>
Файлы (из директории fs) надо скинуть на sd карту.<br>
Led2 при работе толжен мигать раз в секунду (индикация что система не упала).<br>
Достаточно долго бился что бы карта работала стабильно 09.08.23 - вроде получилось рабоатет более суток и сайт грузиться.
Ранее была ошибка 1 fatfs что значить ошибка ввода вывода нижнего уровня при открытии файла.<br>
Реализована работа с ajax запросами +xml.При запросе серевер отдает xml файл с данными.<br>
Преимущество использования Ajax с XML заключается в том, что отдельные значения могут быть легко получены (извлечены) с
помощью JavaScript, вместо того чтобы писать специальный JavaScript код для извлечения этих значений из текстового файла.<br>
XML файл использует теги, такие как в HTML или XHTML. Файл содержит основной тег, который идентифицирует его как XML файл.
Далее следует определяемый пользователем тег, который содержит все остальные теги передаваемых значений.<br>
JavaScript код, содержащийся на веб-странице, инициирует отправку Ajax запроса на сервер stm32 (и продолжает отправлять такие запросы каждую секунду).<br>
Получив запрос XML файла, stm32 отвечает стандартным HTTP заголовком, за которым следует сам XML файл, содержащий нужные значения состояния входов Arduino.<br>
Подробне по настройке sd карты под freeRTOS <br>
http://ipasoft.info/index.php/9-blog/13-stm32f4-rabota-s-flash-sdio-microsd-pod-freertos <br>
https://blog.csdn.net/dxc8865de2/article/details/107914150 <br>
https://blog.csdn.net/xuan530482366/article/details/123713602 <br>
<img src="https://github.com/pav2000/stm32f407-dp83848/blob/master/board/web04.jpg" width="500" /> <br>  
<br>

