# kabeltester-sw

## Projektbeschreibung

Dieses Repository enthält die Firmware für den automatisierten **Kabeltester**, der im Rahmen der Projektaufgabe an der **Hochschule München** (Fakultät für angewandte Naturwissenschaften und Mechatronik, Prof. Dr.-Ing. Alexander Steinkogler) entwickelt wird.

Ziel des Projekts ist die Entwicklung eines kompakten Messgeräts zur schnellen und zuverlässigen Überprüfung von Kabeln mit bis zu 32 Polen an beiden Enden. Es löst das Problem zeitaufwendiger manueller Messungen und veralteter Kabeldokumentationen im Sondermaschinenbau und bei Studierendenprojekten.

### Hauptfunktionen der Software

* **32-Pol-Messung & Brückenerkennung:** Steuerung des relaisbasierten Routings zur Messung von bis zu 32 Kontakten sowie Identifikation gewollter und ungewollter Brücken an den Steckverbindern[cite: 1].
* **4-Zustände-Bewertung:** Automatische Auswertung und Zuordnung der gemessenen Widerstände in vier Kategorien[cite: 1]:
  * **Leitend:** $0\ \Omega$ bis $1\ \Omega$[cite: 1]
  * **Schluss (Niederohmig):** $1\ \Omega$ bis $1\text{ k}\Omega$[cite: 1]
  * **Hochohmig:** $1\text{ k}\Omega$ bis $100\text{ k}\Omega$[cite: 1]
  * **Isolierend:** $> 100\text{ k}\Omega$[cite: 1]
* **Dateiverarbeitung (SD-Karte):** 
  * Einlesen von Soll-Konfigurationen aus Textdateien zur automatischen Prüfung auf Korrektheit[cite: 1].
  * Speichern von Ist-Messwerten in einem standardisierten Textformat zur Dokumentation[cite: 1].
* **Benutzeroberfläche (UI):** Vor-Ort-Bedienung über das integrierte Display und Eingabeelemente[cite: 1].

### Rechtliches & Lizenzierung

* **Open-Source:** Der Quellcode ist unter der [Name deiner Lizenz, z. B. MIT-Lizenz] veröffentlicht.
* **Drittanbieter-Software:** Alle verwendeten Bibliotheken und deren Lizenzen sind in der Dokumentation aufgeführt. Es kommen keine geschützten Unternehmens-IPs zum Einsatz[cite: 1].