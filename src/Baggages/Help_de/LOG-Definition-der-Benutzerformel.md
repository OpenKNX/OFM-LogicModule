### Formeldefinition

Hier wird die Benutzerformel eingegeben. Es wird normale C Formelsyntax unterstützt. Es stehen die unten aufgeführten Variablen, Konstanten, Operatoren und Funktionen zur Verfügung. Das Ergebnis der Formel wird immer dem Ausgang zugewiesen.

Benutzerformeln werden immer mit dem Datentyp double (Fließkommazahl) berechnet. Alle Eingangsgrößen werden in double umgewandelt, dann wird gerechnet, anschließend wird das Ergebnis von double im Ausgangskonverter in den Ziel-DPT gewandelt.

Eine Benutzerfunktion kann aus maximal 99 Zeichen bestehen. In einer Benutzerfunktion können auch andere Benutzerfunktionen aufgerufen werden.

Um die Lesbarkeit einer Benutzerfunktion zu erhöhen, können neue Zeilen eingefügt werden. Da die ETS keine mehrzeiligen Eingabefelder zulässt, wird das gleiche Verfahren wie bei Kommentarfeldern verwendet: Die Zeichenfolge *\n*, gefolgt von einem Klick auf die Taste "Neue Zeilen aus '\n' machen".

Eine Formel kann auch mit der Taste "Formel prüfen" auf ihre syntaktische Korrektheit geprüft werden. Dies passiert durch eine Online-Verbindung mit dem Gerät selbst. Damit die Taste "Formel prüfen" verfügbar ist, muss das Gerät der ETS bekannt sein. Dafür muss es mindestens einmal programmiert worden sein.

Groß- und Kleinschreibung ist für die Formelauswertung nicht relevant. Man sollte die Formeln so notieren, dass die Lesbarkeit für einen selbst am höchsten ist.

Wenn eine Formel zur Ausführungszeit einen Fehler enthält, liefert sie den Wert NaN (Not-a-Number). Das Ergebnis einer solchen Formel ändert nicht das Ausgangs-KO und der Wert wird nicht auf den Bus gesendet. Folgende Fehler sind möglich:

* Formel ist syntaktisch falsch (Formel prüfen wurde z.B nicht ausgeführt)
* Formel enthält eine Division durch 0
* Formel enthält die Quadratwurzel einer negativen Zahl
* Formel enthält den Logarithmus einer Zahl <= 0
* Formel ruft mehr als 10 weitere Formeln auf (z.B. durch Rekursion)

#### Variable E1 - Eingang 1

E1 wird in der Formel als der aktuelle Wert vom Eingang 1 interpretiert. Der Wert wird generisch in eine Fließkommazahl gewandelt (double in C++) und dann mit dem Wert weitergerechnet.

#### Variable E2 - Eingang 2

E2 wird in der Formel als der aktuelle Wert vom Eingang 2 interpretiert. Der Wert wird generisch in eine Fließkommazahl gewandelt (double in C++) und dann mit dem Wert weitergerechnet.

#### Variable A - Ausgang

A wird in der Formel als der aktuelle Wert vom Ausgang interpretiert. Der Wert wird generisch in eine Fließkommazahl gewandelt (double in C++) und dann mit dem Wert weitergerechnet.

#### Konstante e - Eulersche Zahl

Die Konstante e wird mit dem Wert der Eulerschen Zahl belegt.

#### Konstante pi - Kreiszahl

Die Konstante pi wird mit dem Wert der Kreiszahl belegt.

#### Operatoren

Folgende Operatoren sind verfügbar:

* '+' - Addition
* '-' - Subtraktion
* '*' - Multiplikation
* '/' - Division
* '*' - Multiplikation
* '^' - Potenz
* '%' - Modulo
* '==' - Gleich
* '!=' - Ungleich
* '<' - Kleiner
* '<=' - Kleiner gleich
* '>' - Größer
* '>=' - Größer gleich
* '!' - Logisches 'Nicht'
* '&&' - Logisches 'Und'
* '||' - Logisches 'Oder'

#### Funktionen

Folgende Funktionen sind verfügbar:

* 'if(c,p,q)' - Wenn c wahr ist, dann p sonst q
* 'if2(c1,p,c2,q,r)' - Wenn c1 wahr ist, dann p sonst wenn c2 wahr ist, dann q sonst r
* 'if3(c1,p,c2,q,c3,r,s)' - Wenn c1 wahr ist, dann p sonst wenn c2 wahr ist, dann q sonst wenn c3 wahr ist, dann  r sonst s
* 'b*n*(e1,e2,a)' - Benutzerfunktion *n* (für n=1 bis n=30)

* 'abs(x)' - Absolutwert
* 'sqrt(x)' - Quadratwurzel
* 'pow(x,y)' - Potenz (x^y)
* 'exp(x)' - Exponentialfunktion (e^x) 
* 'ln(x)' - natürlicher Logarithmus
* 'log(x)' - Logarithmus zur Basis 10
* 'fac(x)' - Fakultät
* 'ncr(x,y)' - Kombination (nCr)
* 'npr(x,y)' - Permutation (nPr)

* 'round(x,n)' - Runde x an n-ter Stelle hinter dem Komma
* 'ceil(x)' - nächstgrößere Ganzzahl
* 'floor(x)' - nächstkleinere Ganzzahl

* 'sin(x)' - Sinus
* 'sinh(x)' - Sinus hyperbolicus
* 'cos(x)' - Cosinus
* 'cosh(x)' - Cosinus hyperbolicus
* 'tan(x)' - Tangens
* 'tanh(x)' - Tangens hyperbolicus
* 'asin(x)' - Arcussinus
* 'acos(x)' - Arcuscosinus
* 'atan(x)' - Arcustangens

* 'nan()' - Not-A-Number (liefert ungültigen Funktionswert)
* 'rnd(x,y)' - Liefert eine ganzzahlige Zufallszahl im Intervall [x,y]. rnd(1,10) also Zufallszahlen von 1 bis 10 (einschließlich).

