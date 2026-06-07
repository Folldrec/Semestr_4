# Lab 3a

## Опис

Реалізація паралельного сортування злиттям (Merge Sort) з використанням
`std::async` / `std::future`. Порівняння з послідовною версією.


## ООП дизайн

- **Патерн Strategy** — `Sorter` є абстрактним інтерфейсом; клієнтський код
  залежить тільки від нього, не від конкретних реалізацій.
- **Інкапсуляція** — внутрішні методи (`mergeSort`, `parallelSort`) є
  `private`; публічний API мінімальний.
- **Поліморфізм** — обидва сортувальники взаємозамінні через `Sorter*`.

## Паралелізм

`ParallelMergeSort` рекурсивно ділить масив надвоє та сортує ліву половину
на окремому потоці (`std::async(std::launch::async, ...)`), а праву — на
поточному потоці.  Коли глибина рекурсії досягає `log2(numThreads)` або
розмір підмасиву ≤ `sequentialThreshold`, відбувається перемикання на
`SequentialMergeSort` щоб уникнути надлишкових потоків.

## Збірка

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel
```

## Запуск тестів

```bash
./run_tests
```

## Запуск бенчмарку

```bash
./benchmark
# Результати зберігаються у results.txt
```

## Результати бенчмарку

Середовище: Windows 11, GCC 13.2.0, 32 ядер.

# Benchmark Results – Parallel vs Sequential Merge Sort

size      sorter                                                       time_s   speedup
------------------------------------------------------------------------------------------
1000      Sequential Merge Sort                                        0.0002     1.00x
1000      Parallel Merge Sort (threads=1, threshold=1000)              0.0001     1.20x
1000      Parallel Merge Sort (threads=2, threshold=1000)              0.0001     1.22x
1000      Parallel Merge Sort (threads=4, threshold=1000)              0.0001     1.24x
1000      Parallel Merge Sort (threads=8, threshold=1000)              0.0001     1.26x
5000      Sequential Merge Sort                                        0.0008     1.00x
5000      Parallel Merge Sort (threads=1, threshold=1000)              0.0008     1.02x
5000      Parallel Merge Sort (threads=2, threshold=1000)              0.0009     0.93x
5000      Parallel Merge Sort (threads=4, threshold=1000)              0.0009     0.84x
5000      Parallel Merge Sort (threads=8, threshold=1000)              0.0011     0.70x
10000     Sequential Merge Sort                                        0.0018     1.00x
10000     Parallel Merge Sort (threads=1, threshold=1000)              0.0018     0.97x
10000     Parallel Merge Sort (threads=2, threshold=1000)              0.0018     1.01x
10000     Parallel Merge Sort (threads=4, threshold=1000)              0.0019     0.94x
10000     Parallel Merge Sort (threads=8, threshold=1000)              0.0020     0.90x
50000     Sequential Merge Sort                                        0.0093     1.00x
50000     Parallel Merge Sort (threads=1, threshold=1000)              0.0093     1.00x
50000     Parallel Merge Sort (threads=2, threshold=1000)              0.0102     0.91x
50000     Parallel Merge Sort (threads=4, threshold=1000)              0.0093     1.00x
50000     Parallel Merge Sort (threads=8, threshold=1000)              0.0099     0.94x
100000    Sequential Merge Sort                                        0.0193     1.00x
100000    Parallel Merge Sort (threads=1, threshold=1000)              0.0188     1.02x
100000    Parallel Merge Sort (threads=2, threshold=1000)              0.0193     1.00x
100000    Parallel Merge Sort (threads=4, threshold=1000)              0.0201     0.96x
100000    Parallel Merge Sort (threads=8, threshold=1000)              0.0199     0.97x

**Висновок:** На даних вхідних розмірах (до 100 000 елементів) паралельний merge sort не дає практичного прискорення порівняно з послідовним. Це очікувана поведінка: merge sort має відносно невисоку обчислювальну складність і значний overhead на злиття, що обмежує ефективний паралелізм. Прискорення понад 2× реалістично досягнути лише на масивах від ~500 000 елементів і при правильно підібраному порозі перемикання на послідовну версію.