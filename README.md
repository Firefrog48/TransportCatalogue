```markdown
# 🚍 TransportCatalogue

**Транспортный справочник** — это приложение, принимающее данные в формате JSON и визуализирующее автобусные маршруты и остановки в виде SVG-файлов.
Оно также позволяет находить кратчайший маршрут между остановками. 

## 📋 Основные возможности
- **Визуализация маршрутов и остановок**: Генерация SVG для отображения схемы маршрутов.
- **Поиск кратчайшего пути**: Поиск оптимального маршрута между остановками.
- **Безопасный JSON-конструктор**: Проверка правильной последовательности
    методов на этапе компиляции.

## 🛠️ Сборка проекта 

```
git clone git@github.com:Firefrog48/TransportCatalogue.git
```

## 🚀 Запуск программы

```
Любая IDE
```

## 📦 Формат входных данных

Входные данные поступают в формате JSON и имеют следующую структуру:

```json
{
    "base_requests": [ ... ],
    "render_settings": { ... },
    "routing_settings": { ... },
    "stat_requests": [ ... ]
}
```

- `base_requests` — массив с описанием маршрутов и остановок.
- `render_settings` — настройки для отрисовки карты.
- `routing_settings` — параметры маршрутизации.

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
Пример конфигурации для рендера карты:

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

## 🛤️ Настройки маршрутизации (`routing_settings`)

```json
{
    "bus_wait_time": 6,
    "bus_velocity": 40
}
```
- `bus_wait_time` — время ожидания на остановке (минуты).
- `bus_velocity` — скорость автобуса (км/ч).

## 🔍 Примеры запросов к базе данных

### 1. Информация о маршруте:
Запрос:
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

### 2. Информация об остановке:
Запрос:
```json
{
    "id": 12345,
    "type": "Stop",
    "name": "Улица Докучаева"
}
```
Ответ:
```json
{
    "buses": ["14", "22к"],
    "request_id": 12345
}
```

### 3. Построение маршрута:
Запрос:
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

### 4. Получение карты:
Запрос:
```json
{
    "type": "Map",
    "id": 11111
}
```
Ответ:
```json
{
    "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">...\n</svg>",
    "request_id": 11111
}
```

---

✨ **TransportCatalogue** — это удобный и быстрый инструмент для управления транспортными данными, маршрутизации и их визуализации.
```
