# OFM-LogicModule – Agent Instructions

## Project

OFM-LogicModule ist ein OpenKNX Function Module (OFM), das einen Multi-Kanal-Logikprozessor für KNX-Geräte implementiert. Es ist kein eigenständiges Firmware-Projekt — es wird als Abhängigkeit in ein konkretes Geräteprojekt (z.B. OAM-RaumController) eingebunden zusammen mit OGM-Common und weiteren OFMs.

- Language: C++ (Arduino framework, `gnu++17`)
- Branch-Konvention: `v1` = stabil, `v1dev` = aktive Entwicklung
- ETS-Konfiguration: Parameterseiten in `src/Logikmodul.templ.xml` + `Logikmodul.share.xml` u.a.
- Dokumentation: `doc/Applikationsbeschreibung-Logik.md` (~250 KB, Anwenderdoku)

## Architektur

### Klassenübersicht

| Klasse | Datei | Rolle |
|---|---|---|
| `Logic` | `src/Logic.cpp/.h` | `OpenKNX::Module`-Subklasse, Singleton-Zugang via `openknxLogic` |
| `LogicChannel` | `src/LogicChannel.cpp/.h` | `OpenKNX::Channel`-Subklasse, ein Logikkanal |
| `LogicValue` | `src/LogicValue.cpp/.h` | Tagged-Union-Wertetyp (bool/int8–32/uint8–32/float/double/string) |
| `LogicFunction` | `src/LogicFunction.cpp/.h` | Rein statische Utility-Klasse, 42 Native Functions + 30 User Formulas |
| `Timer` | `src/Timer.cpp/.h` | Singleton, Sekundentakt, Sonnenauf-/-untergang, Feiertage |
| `TimerRestore` | `src/TimerRestore.cpp/.h` | Erbt `Timer`, läuft rückwärts durch bis zu 365 Tage zur State-Wiederherstellung |
| `TimerHoliday` | `src/TimerHoliday.cpp/.h` | Feiertag-Berechnung (Ostern, Advent), eingebettet in `Timer` |
| `LogicLed` | `src/LogicLed.cpp/.h` | Adapter von `OpenKNX::Led::Base` auf einen internen Kanaleingang |

### Lebenszyklus

`Logic::setup()` allokiert alle aktiven `LogicChannel`-Instanzen mit `new`. `loop()` iteriert Kanäle round-robin und respektiert `openknx.freeLoopTime()`. Kein `delete` — Kanäle leben für die gesamte Gerätelebensdauer.

### Pipeline-State-Machine (LogicChannel)

Jeder Kanal hat ein `uint32_t pCurrentPipeline`-Bitfield. `loop()` prüft die Bits der Reihe nach und ruft die entsprechenden `process*()`-Methoden auf. Reihenfolge:

```
processInput → startConvert → processConvertInput → startLogic → processLogic
→ startStairlight → startBlink → startOnDelay/startOffDelay
→ startOutputFilter → processOutputFilter → startOnOffRepeat → processLock → processOutput
```

`PIP_*`-Konstanten in `LogicChannel.h` definieren die Bit-Positionen.

### KNX Group-Object-Routing

- KO-Offset-Arithmetik: `LOG_KoOffset + channelIndex * LOG_KoBlockSize + IO_offset`
- 3 KOs pro Kanal: E1 (Eingang 1), E2 (Eingang 2), O (Ausgang)
- Externe KO-Zuordnungen werden in `mKoLookup[cCountKoLookups]` (flat array in `Logic`) verwaltet
- `Logic::processInputKo()` routet eingehende Telegramme sowohl via Lookup-Tabelle als auch per direkter ASAP-Arithmetik

### Interne Eingänge

`processAllInternalInputs(LogicChannel*, bool)` scannt beim Kanalausgang alle `LOG_ChannelCount` Kanäle auf passende interne Eingänge (I1/I2). Das ist O(n) pro Ausgang.

### User Formulas (tinyexpr)

30 Formel-Slots (IDs 201–230, Index 0–29). Kompilierte `te_expr*`-Zeiger werden in `sCompiledFormula[30]` gecacht (lazy, beim ersten Aufruf). `te_compile()` wird danach nicht mehr aufgerufen — `te_free()` darf für gecachete Ausdrücke **nicht** aufgerufen werden.

Die ETS-Diagnosepfade (`processFunctionPropertyFormula`) kompilieren One-Shot und gehen nicht in den Cache.

Rekursionsschutz: `sRecursionCounter`, Limit `NUM_RECURSION_DEPTH = 30`.

### Versionscheck

`ModuleVersionCheck.h` erzwingt zur Compile-Zeit, dass `LOG_ModuleVersion` (aus `knxprod.h`) mit `MODULE_LogicModule_ETS` (aus `versions.h`) übereinstimmt. Mismatch → harter Compiler-Fehler.

## KNX-Integration

- Parameter via Makros aus `knxprod.h`: `ParamLOG_*`, `KoLOG_*`
- Flash-Persistenz: `openknx.flash.read()`/`write()` (Streaming-API); `restoreEmpty()` liest 10 Byte um den Stream-Pointer zu bewegen
- Flash speichert pro Kanal: DPT + 4-Byte-Wert für E1 und E2; DPT-16-Strings (14 Byte) werden als `0x00000000` gespeichert

## Lastschutz

`pLoadCounter` zählt `startLogic`-Aufrufe pro Sekunde pro Kanal. Bei Erreichen von `LOAD_COUNTER_MAX` (50) wird der Kanal bis zum Reset gestoppt. Globale Statistik in `pLoadCounterMax` / `pLoadChannel` (statisch in `Logic`).

## Bekannte Besonderheiten

- `LogicValue::stringValue()` nutzt `openknxLogic.gBuffer[14]` — ein globaler 14-Byte-Puffer, nicht reentrant
- `uInputProcessing`-Union in `LogicChannel.h`: aliasiert Normal-Kanal-Felder (`repeatInput1/2Delay`) mit Timer-Kanal-Feldern (`activeTimer`/`processedTimer`) — Invariante wird nur zur Laufzeit durchgesetzt
- `Timer::mForceUpdate`: steuert den ersten sofortigen Update-Aufruf in `loop()`; ersetzt den alten `mTimeDelay == 0`-Sentinel

## Code-Konventionen

Entsprechen OGM-Common (`.clang-format` ist autoritativ):
- Allman-Style Braces, 4-Leerzeichen-Einrückung, keine Tabs
- Keine `delay()`-Aufrufe, alles non-blocking mit `millis()`
- Kein Heap-Churn in `loop()` (kein `new`/`delete`, kein `std::string`-Konstruieren)
- Kommentare auf Deutsch oder Englisch (gemischt OK)
- Platform-Switches: `#ifdef ARDUINO_ARCH_RP2040`, `#ifdef ARDUINO_ARCH_ESP32`

## Dokumentationspflege

Nach Code-Änderungen:
- Neue Parameter / KOs → `doc/Applikationsbeschreibung-Logik.md` aktualisieren
- Neue XML-Parameterseiten → entsprechende `.part.xml`/`.templ.xml` in `src/`
- Bugfix ohne API-Änderung → keine Doku-Anpassung nötig
