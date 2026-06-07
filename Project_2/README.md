# Profile Analyzer

## Підтримувані платформи

| Провайдер | Що збирається | Ключ |
|-----------|--------------|------|
| **Steam** | Бібліотека, час, жанри, досягнення | [steamcommunity.com/dev/apikey](D83A8B40B5C979F0876040B67E256B13) |

## Структура проекту

```
steam_analyzer/
├── config.h                    ← ВСІ ключі API та ліміти
├── types.h                     ← Achievement, Game, UserProfile
├── http.h / http.cpp           ← HTTP-клієнт (libcurl)
├── steam_api.h / steam_api.cpp ← Steam Web API
├── report.h / report.cpp       ← Термінальний вивід (Steam)
├── main.cpp                    ← Точка входу + registerProviders()
├── CMakeLists.txt
│
└── providers/
    ├── i_provider.h            ← Абстрактний інтерфейс IProvider
    ├── provider_registry.h/cpp ← Singleton-реєстр провайдерів
    ├── provider_report.h/cpp   ← Звіти для зовнішніх провайдерів
    ├── riot_provider.h/cpp     ← Riot Games API
    └── epic_provider.h/cpp     ← Epic Games Store API
```

## Налаштування ключів (config.h)

```cpp
// Steam
inline const std::string API_KEY = "ВАШ_STEAM_КЛЮЧ";
```

## Збірка

```bash
cd steam_analyzer
cmake -B build
cmake --build build
./build/game_analyzer
```