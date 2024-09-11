# cpp-transport-catalogue
Финальный проект: транспортный справочник

## Описание
Проект "Транспортный Каталог" представляет собой систему для управления и обработки данных о городском транспорте. Программа позволяет хранить информацию о маршрутах, остановках, и расстояниях между ними, а также предоставляет инструменты для визуализации данных на SVG-карте и для обработки пользовательских запросов. Этот проект был создан для изучения работы с JSON, работы с файловой системой и создания собственного парсера JSON без использования сторонних библиотек.

**Стек технологий:**
- C++
- Стандартная библиотека C++
- Собственные модули для работы с JSON, SVG и маршрутизацией

## Функциональность:
- Загрузка данных о транспорте из JSON.
- Обработка запросов на получение информации о маршрутах и остановках.
- Построение карты маршрутов в формате SVG.
- Построение маршрутов с учётом времени ожидания и скорости автобусов.

## Системные требования:
- **Версия языка:** C++17 и выше.
- **Сборщик:** CMake 3.10+.
- **Компилятор:** GCC/Clang/MSVC.
- **Необходимые библиотеки:** Проект не требует подключения сторонних библиотек, все необходимые модули, включая JSON-парсер, реализованы вручную.

## Инструкция по развёртыванию:
1. Клонируйте репозиторий:
   git clone https://github.com/username/transport_catalogue.git

2. Перейдите в папку проекта:
   cd transport_catalogue

3. Соберите проект с использованием CMake:
   mkdir build
   cd build
   cmake ..
   make
4. Запустите программу:
   ./transport_catalogue < input.json > output.json

Пример формата входных данных:
  {
      "base_requests": [
          {
              "is_roundtrip": true,
              "name": "297",
              "stops": [
                  "Biryulyovo Zapadnoye",
                  "Biryulyovo Tovarnaya",
                  "Universam",
                  "Biryulyovo Zapadnoye"
              ],
              "type": "Bus"
          },
          {
              "is_roundtrip": false,
              "name": "635",
              "stops": [
                  "Biryulyovo Tovarnaya",
                  "Universam",
                  "Prazhskaya"
              ],
              "type": "Bus"
          },
          {
              "latitude": 55.574371,
              "longitude": 37.6517,
              "name": "Biryulyovo Zapadnoye",
              "road_distances": {
                  "Biryulyovo Tovarnaya": 2600
              },
              "type": "Stop"
          },
          {
              "latitude": 55.587655,
              "longitude": 37.645687,
              "name": "Universam",
              "road_distances": {
                  "Biryulyovo Tovarnaya": 1380,
                  "Biryulyovo Zapadnoye": 2500,
                  "Prazhskaya": 4650
              },
              "type": "Stop"
          },
          {
              "latitude": 55.592028,
              "longitude": 37.653656,
              "name": "Biryulyovo Tovarnaya",
              "road_distances": {
                  "Universam": 890
              },
              "type": "Stop"
          },
          {
              "latitude": 55.611717,
              "longitude": 37.603938,
              "name": "Prazhskaya",
              "road_distances": {},
              "type": "Stop"
          }
      ],
      "render_settings": {
          "bus_label_font_size": 20,
          "bus_label_offset": [
              7,
              15
          ],
          "color_palette": [
              "green",
              [
                  255,
                  160,
                  0
              ],
              "red"
          ],
          "height": 200,
          "line_width": 14,
          "padding": 30,
          "stop_label_font_size": 20,
          "stop_label_offset": [
              7,
              -3
          ],
          "stop_radius": 5,
          "underlayer_color": [
              255,
              255,
              255,
              0.85
          ],
          "underlayer_width": 3,
          "width": 200
      },
      "routing_settings": {
          "bus_velocity": 40,
          "bus_wait_time": 6
      },
      "stat_requests": [
          {
              "id": 1,
              "name": "297",
              "type": "Bus"
          },
          {
              "id": 2,
              "name": "635",
              "type": "Bus"
          },
          {
              "id": 3,
              "name": "Universam",
              "type": "Stop"
          },
          {
              "from": "Biryulyovo Zapadnoye",
              "id": 4,
              "to": "Universam",
              "type": "Route"
          },
          {
              "from": "Biryulyovo Zapadnoye",
              "id": 5,
              "to": "Prazhskaya",
              "type": "Route"
          }
      ]
  }

## Структура файлов:
- `main.cpp`: Основной файл для запуска программы.
- `domain.h`, `domain.cpp`: Описание сущностей остановок и автобусов.
- `geo.h`, `geo.cpp`: Функции для работы с географическими координатами.
- `json.h`, `json.cpp`: Собственная реализация парсера JSON.
- `input_reader.h`, `input_reader.cpp`: Модуль для обработки пользовательских запросов.
- `map_renderer.h`, `map_renderer.cpp`: Модуль для рендеринга SVG-карты.
- `router.h`, `transport_router.cpp`: Маршрутизатор для расчёта оптимальных путей.
- `tests.h`: Модуль для тестирования компонентов.
  
## Планы по доработке:
- Добавить поддержку многопоточности для ускорения обработки запросов.
- Реализовать поддержку обновления данных в режиме реального времени.
- Оптимизировать алгоритмы маршрутизации с использованием сторонних библиотек (например, Boost.Graph).
- Добавить поддержку других форматов данных для входных и выходных файлов, таких как XML и CSV.

