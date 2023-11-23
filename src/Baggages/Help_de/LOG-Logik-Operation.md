### Logik-Operation

Mittels der Auswahlliste kann eine Operation und damit die Art der Verknüpfung der Eingänge dieses Logikkanals ausgewählt werden. Es stehen folgende Operationen zur Verfügung:

#### aus

Dieser Logikkanal nicht definiert und nicht aktiv. Es stehen keine Eingänge und kein Ausgang zur Verfügung. Alle entsprechenden KO sind ausgeblendet.

#### UND

Alle Eingänge werden über ein logisches UND verknüpft. Das Ergebnis der Verknüpfung ist EIN, wenn alle Eingänge des Funktionsblock EIN sind. Das Ergebnis ist AUS, wenn auch nur ein Eingang AUS ist.

#### ODER

Alle Eingänge werden über ein logisches ODER verknüpft. Das Ergebnis der Verknüpfung ist EIN, sobald nur ein Eingang des Funktionsblock EIN ist. Das Ergebnis ist AUS, wenn alle Eingänge AUS sind.

#### EXKLUSIV-ODER

Alle Eingänge werden über ein logisches Exklusiv-ODER verknüpft. Das Ergebnis der Verknüpfung ist EIN, wenn eine ungerade Anzahl von Eingängen des Funktionsblock EIN sind. Das Ergebnis ist AUS, wenn eine gerade Anzahl von Eingängen EIN sind.

#### TOR

Ein Tor hat normalerweise einen Dateneingang, Datenausgang und einen Toreingang. Wird das Tor über ein Signal am Toreingang geöffnet, können Daten vom Dateneingang zum Datenausgang fließen. Wird das Tor geschlossen, dann fließen keine Daten zwischen Dateneingang und Datenausgang.

Wird das Signal am Toreingang invertiert (negiert), dann sprechen wir von einer Sperre.

Da ein Logikkanal 4 Eingänge hat, ist bei einem Tor

    Dateneingang = Eingang 1 ODER Kanalausgang X
    Toreingang = Eingang 2 ODER Kanalausgang Y

(in Worten: Jeweils ein externer und ein interner Eingang werden über ein ODER verknüpft und bilden den entsprechenden Eingang der TOR-Verknüpfung).

#### ZEITSCHALTUHR

Dieser Logikkanal hat keine Eingänge, sondern repräsentiert eine Zeitschaltuhr. Der Ausgang wird somit durch entsprechende Zeitschaltpunkte geschaltet. Der Ausgang kann immer noch passende Funktionsmodule enthalten.

#### SCHALTER (RS-FLIP_FLOP)

Dieser Logikkanal hat den Eingang 1, der bei einem EIN-Signal den Ausgang EINschaltet. Der Eingang 2 schaltet bei einem EIN-Signal den Ausgang AUS. Ein AUS-Signal auf einem der beiden Eingänge bewirkt nichts. 

Mit dem Schalter lassen sich viele Funktionen, die vorher nur mit 2 Kanälen zu lösen waren, jetzt mit nur einem Kanal lösen, was die Übersichtlichkeit der Logiken erhöht. 

Der Schalter ermöglicht die Realisierung eines einfachen Szenen-Controllers (siehe Beispiele). Ferner können damit Hysterese-Logiken aufgebaut werden, bei denen die Ein- und Ausschaltbedingung aus 2 verschiedenen Quellen kommt (z.B. verschiedene DPT).

