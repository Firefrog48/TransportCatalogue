```markdown
# 🚍 TransportCatalogue

**Транспортный справочник** — это система, которая принимает данные в формате JSON, визуализирует остановки и маршруты в виде SVG-файлов, и находит кратчайшие маршруты между остановками. Для оптимизации вычислений база данных сериализуется через Google Protobuf.

## 📋 Основные возможности
- **Визуализация маршрутов и остановок**: Генерация SVG-файлов для отображения схемы маршрутов.
- **Поиск кратчайшего маршрута**: Алгоритм находит кратчайший путь между автобусными остановками.
- **Сериализация данных**: Использование Google Protobuf для ускорения операций с базой данных.
- **Безопасная работа с JSON**: Конструктор JSON позволяет обнаруживать неправильную последовательность методов на этапе компиляции.

## 🛠️ Инструкция по сборке проекта (Visual Studio)

### 1. Установка Google Protobuf
1. Скачайте и распакуйте архив `protobuf-cpp` с [официального репозитория](https://github.com/protocolbuffers/protobuf).
2. Создайте папки `build-debug` и `build-release` для сборки двух конфигураций Protobuf. Также создайте папку `package` для установки собранного пакета Protobuf.

### 2. Сборка и установка Protobuf
Для сборки и установки Protobuf выполните следующие команды:

```bash
cmake path\protobuf-3.15.8\cmake -DCMAKE_SYSTEM_VERSION=10.0.17763.0 -DCMAKE_BUILD_TYPE=Debug \ 
-Dprotobuf_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=path\protobuf-3.15.8\package

cmake --build .
cmake --install .
```

После сборки в папке `package` появятся:
- `bin\protoc.exe` — для компиляции `.proto` файлов
- Статические библиотеки в папке `lib`.

### 3. Компиляция proto-файла
Для компиляции proto-файла используйте команду:

```bash
<path_to_protobuf_package>\bin\protoc --cpp_out . transport_catalogue.proto
```

### 4. Сборка проекта с CMake
Соберите проект с помощью CMake:

```bash
cmake . -DCMAKE_PREFIX_PATH=/path/to/protobuf/package
cmake --build .
```

Если необходимо, добавьте папки `include` и `lib` в зависимости проекта: `Additional Include Directories` и `Additional Dependencies`.

## 🚀 Запуск программы

### 1. Сериализация базы данных
Для создания базы транспортного справочника и её сериализации используйте команду:

```bash
transport_catalogue.exe make_base <base.json>
```

### 2. Выполнение запросов к базе данных
Для обработки запросов и десериализации базы используйте команду:

```bash
transport_catalogue.exe process_requests <req.json> >out.txt
```

## 📦 Формат входных данных

Входные данные подаются в формате JSON и содержат следующие ключи:

```json
{
    "base_requests": [ ... ],
    "render_settings": { ... },
    "routing_settings": { ... },
    "serialization_settings": { ... },
    "stat_requests": [ ... ]
}
```

### Структура ключей:
- `base_requests` — описание автобусных маршрутов и остановок.
- `render_settings` — настройки визуализации карты.
- `routing_settings` — параметры для расчета времени маршрутов.
- `serialization_settings` — настройки сериализации базы.

### Пример описания остановки:
```json
{
    "type": "Stop",
    "name": "Электросети",
    "latitude": 43.598701,
    "longitude": 39.730623,
    "road_distances": {
        "Улица Докучаева": 3000,
        "Улица Лизы Чайкиной": 4300
    }
}
```

### Пример описания маршрута:
```json
{
    "type": "Bus",
    "name": "14",
    "stops": [
        "Улица Лизы Чайкиной",
        "Электросети",
        "Улица Докучаева",
        "Улица Лизы Чайкиной"
    ],
    "is_roundtrip": true
}
```

## 🎨 Настройки визуализации (`render_settings`)
Пример настроек для рендера карты:

```json
{
    "width": 1200.0,
    "height": 1200.0,
    "padding": 50.0,
    "line_width": 14.0,
    "stop_radius": 5.0,
    "bus_label_font_size": 20,
    "bus_label_offset": [7.0, 15.0],
    "stop_label_font_size": 20,
    "stop_label_offset": [7.0, -3.0],
    "underlayer_color": [255, 255, 255, 0.85],
    "underlayer_width": 3.0,
    "color_palette": [
        "green",
        [255, 160, 0],
        "red"
    ]
}
```

## 🛤️ Настройки маршрутов (`routing_settings`)
Пример настроек маршрутизации:

```json
{
    "bus_wait_time": 6,
    "bus_velocity": 40
}
```

### Параметры:
- `bus_wait_time` — время ожидания автобуса на остановке (в минутах).
- `bus_velocity` — скорость автобуса (в км/ч).

## 🔍 Запросы к базе данных

### Запрос информации о маршруте:
```json
{
    "id": 12345678,
    "type": "Bus",
    "name": "14"
}
```
Ответ:
```json
{
    "curvature": 2.18604,
    "request_id": 12345678,
    "route_length": 9300,
    "stop_count": 4,
    "unique_stop_count": 3
}
```

### Запрос на построение маршрута между остановками:
```json
{
    "type": "Route",
    "from": "Biryulyovo Zapadnoye",
    "to": "Universam",
    "id": 4
}
```
Ответ:
```json
{
    "items": [
        {
            "stop_name": "Biryulyovo Zapadnoye",
            "time": 6,
            "type": "Wait"
        },
        {
            "bus": "297",
            "span_count": 2,
            "time": 5.235,
            "type": "Bus"
        },
        {
            "stop_name": "Universam",
            "time": 6,
            "type": "Wait"
        }
    ],
    "request_id": 5,
    "total_time": 24.21
}
```

---

✨ **TransportCatalogue** — это мощный инструмент для работы с транспортными маршрутами и остановками, который делает их визуализацию и расчёт удобным и быстрым.
```
