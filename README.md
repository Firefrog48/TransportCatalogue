```markdown
# 🚍 TransportCatalogue

**Транспортный справочник** — это система, которая принимает данные в формате JSON,
визуализирует остановки и маршруты в виде SVG-файлов, и находит кратчайшие маршруты между остановками.
Для оптимизации вычислений база данных сериализуется через Google Protobuf.

## 📋 Основные возможности
- **Визуализация маршрутов и остановок**: Генерация SVG-файлов для отображения схемы маршрутов.
- **Поиск кратчайшего маршрута**: Алгоритм находит кратчайший путь между автобусными остановками.
- **Безопасная работа с JSON**: Конструктор JSON позволяет обнаруживать неправильную последовательность методов на этапе компиляции.

## 🛠️ Инструкция по сборке проекта 

git clone git@github.com:Firefrog48/TransportCatalogue.git

## 🚀 Запуск программы

любая IDE

## 📦 Формат входных данных

Входные данные подаются в формате JSON и содержат следующие ключи:

```json
{
    "base_requests": [ ... ],
    "render_settings": { ... },
    "routing_settings": { ... },
    "stat_requests": [ ... ]
}
```
порядок ключей не имеет значения.

### Структура ключей:
- `base_requests` — описание автобусных маршрутов и остановок.
- `render_settings` — настройки визуализации карты.
- `routing_settings` — параметры для расчета времени маршрутов.

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

🔍 Примеры запросов к базе данных
1. Информация о маршруте:
Запрос:

json
Copy code
{
    "id": 12345678,
    "type": "Bus",
    "name": "14"
}
Ответ:

json
Copy code
{
    "curvature": 2.18604,
    "request_id": 12345678,
    "route_length": 9300,
    "stop_count": 4,
    "unique_stop_count": 3
}
2. Информация об остановке:
Запрос:

json
Copy code
{
    "id": 12345,
    "type": "Stop",
    "name": "Улица Докучаева"
}
Ответ:

json
Copy code
{
    "buses": ["14", "22к"],
    "request_id": 12345
}
3. Построение маршрута:
Запрос:

json
Copy code
{
    "type": "Route",
    "from": "Biryulyovo Zapadnoye",
    "to": "Universam",
    "id": 4
}
Ответ:

json
Copy code
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
4. Получение карты:
Запрос:

json
Copy code
{
    "type": "Map",
    "id": 11111
}
Ответ:

json
Copy code
{
    "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">...\n</svg>",
    "request_id": 11111
}
✨ TransportCatalogue — это удобный и быстрый инструмент для управления транспортными данными, маршрутизации и их визуализации.

---

✨ **TransportCatalogue** — это мощный инструмент для работы с транспортными маршрутами и остановками, который делает их визуализацию и расчёт удобным и быстрым.
```
