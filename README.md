# l_mem_alloc

`l_mem_alloc` ist ein kleines Lernprojekt zur Implementierung einer eigenen Memory-Allocation-Library in C.

Ziel des Projekts ist es, grundlegende Konzepte hinter dynamischer Speicherverwaltung besser zu verstehen. Dafür werden eigene Versionen von `malloc`, `calloc`, `realloc` und `free` bereitgestellt. Aktuell ist hauptsächlich `l_malloc` implementiert. `l_free` existiert derzeit nur als einfache, vorläufige Implementierung.

## Aktueller Stand

Implementiert sind momentan:

* `l_malloc`
* eine einfache Version von `l_free`
* grundlegendes Heap-Management mit `sbrk`
* Alignment-Behandlung
* Overflow-Prüfungen
* Wiederverwendung freier Blöcke
* Splitten größerer freier Speicherblöcke

`l_calloc` und `l_realloc` sind aktuell noch nicht implementiert.

## Funktionsweise

Beim ersten Aufruf von `l_malloc` wird der Heap initialisiert. Dabei wird die aktuelle Heap-Startadresse ermittelt, korrekt aligned und gespeichert. Zusätzlich wird die Größe der Verwaltungsstruktur berechnet und ebenfalls aligned gespeichert.

Jeder Speicherblock besitzt einen eigenen Header. Dieser Header wird durch die im Header-File definierte Struktur `alloc_data_t` beschrieben:

```c
typedef struct {
    size_t capacity;
    size_t expected_size;
    bool is_free;
} alloc_data_t;
```

Die Struktur speichert:

* `capacity`: die tatsächlich verfügbare, alignte Größe des Blocks
* `expected_size`: die ursprünglich angefragte Größe ohne Alignment
* `is_free`: ob der Block aktuell frei oder belegt ist

Bei einer Speicheranforderung wird zuerst über den Heap iteriert. Dabei wird nach dem ersten freien Block gesucht, der groß genug ist. Dieses Verfahren entspricht einer einfachen First-Fit-Strategie.

Wird ein passender freier Block gefunden, wird geprüft, ob dieser sinnvoll gesplittet werden kann. Falls ja, wird der Block geteilt und der übrige Speicher als neuer freier Block markiert. Falls kein passender Block gefunden wird, wird der Heap mit `sbrk` erweitert und ein neuer Block angelegt.

## `l_free`

Die aktuelle Implementierung von `l_free` markiert einen Block lediglich als frei:

```c
void l_free(void *ptr) {
    if(ptr == NULL) return;
    alloc_data_t *h = (alloc_data_t *)((char *)ptr - ALIGNED_HEADER_SIZE);
    h->is_free = true;
}
```

Coalescing, also das Zusammenführen benachbarter freier Blöcke, ist aktuell noch nicht implementiert. Die Funktion dient momentan vor allem dazu, im Testprogramm die Speicherfreigabe und das Wiederverwenden freier Blöcke zu demonstrieren.

## Build und Ausführung

Ein `Makefile` ist enthalten.

Kompilieren:

```bash
make
```

Programm ausführen:

```bash
./runable
```

Aufräumen:

```bash
make clean
```

## Hinweis

Dieses Projekt dient ausschließlich Lernzwecken. Die Implementierung ist nicht für den produktiven Einsatz gedacht.
