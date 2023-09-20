# Низкоуровневое программирование 
# Лабораторная работа №1
## О проекте
Лабораторная работа посвящена написанию программе, которая способна работать с одним файлом весом 10 гб. 

По варианту реализован файл содержащий данные в виде документного дерева.

## Сборка
### linux
Сборка исполняемого файла

    make clean
    make build

Запуск

    ./main [-g <generator_data>] <filename>

### Windows
Для сборки использовался gcc и make MinGW.
Скачать можно тут: https://sourceforge.net/projects/mingw/files/Installer/mingw-get-setup.exe/download

Сборка исполняемого файла

    mingw32-make --file=Makefile_win clean
    mingw32-make --file=Makefile_win build

Запуск

    main [-g <generator_data>] <filename>

## Генерация данных для файла
Для генерации случайных данных используется скрипт

    data_generator.py

Для генерации запустите скрипт

    python data_generator.py <count> [<name of field>=<type : String, Integer ...>]

в таком случае программу необходимо запускать с флагом -g и указанием имени сгенерированного файла