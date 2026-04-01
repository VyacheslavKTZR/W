# W

Минималистичный интерпретируемый язык программирования на C++.

## Сборка
```bash
g++ -o w w.cpp cli.cpp
```

## Запуск
```bash
./w
```

## Язык

### Переменные
```
var.create x
var.set x 10
mov x 10

var.createStr name
var.setStr name Hello
```

### Арифметика
```
add x 5
sub x 3
mul x 2
div x 4
```

### Ввод/вывод
```
input name "Введите имя: "
!cr name
```

### Условия
```
if x > 10
!cr x
else
!cr y
endif
```

### Циклы
```
while x < 10
add x 1
endwhile
```

### Функции
```
func myFunc
!cr x
endfunc

call myFunc
```

### Служебные
```
end
```

## CLI редактор

| Команда | Описание |
|---|---|
| :help | Помощь/Справка |
| `:new <file>` | Новый файл |
| `:open <file>` | Открыть файл |
| `:save` | Сохранить |
| `:saveas <file>` | Сохранить как |
| `:list` | Показать буфер |
| `:insert <n> <text>` | Вставить строку |
| `:edit <n> <text>` | Изменить строку |
| `:delete <n>` | Удалить строку |
| `:clear` | Очистить буфер |
| `:run [fresh]` | Запустить |
| `:runfile <file>` | Запустить файл |
| `:exit` | Выход |

## Лицензия

BSD 3-Clause — Copyright (c) 2026, Vyacheslav Pronin
