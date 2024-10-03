# Aufgabenstellung: Rafael Reip
# Entwicklung eines einfachen Bildbearbeitungsprogramms für ein x86-basiertes Betriebssystem
# Development of a Simple Image Editing Program for an x86-based Operating System

## Zielsetzung
Ziel dieser Bachelorarbeit ist die Konzeption und Implementierung eines einfachen, aber funktionalen Bildbearbeitungsprogramms für das vom Lehrstuhl entwickeltes Betriebssystem hhuOS.
Das Programm soll grundlegende Bildbearbeitungsfunktionen bieten und dabei die spezifischen Eigenschaften und Einschränkungen des Zielbetriebssystems berücksichtigen.

## Kernfunktionalitäten
1. **Bildverwaltung**
   - Import von Bildern (von stb_image unterstütze Formate)
   - Verwaltung mehrerer Bilder
   - Export bearbeiteter Bilder (von stb_image unterstütze Formate)

2. **Grundlegende Bildbearbeitung**
   - Verschieben, Rotieren und Skalieren von Bildern
   - Zuschneiden von Bildern
   - Farbauswahl mittels Farbpicker
   - Bildfilter und Effekte

3. **Zeichenwerkzeuge (mindestens)**
   - Pinsel mit verschiedenen Größen und Farben
   - Radierer-Funktion (Setzen des Alpha-Werts auf 0 auf der jeweiligen Ebene)

4. **Benutzeroberfläche**
   - Dynamische Anpassung des Arbeitsbereiches an die verfügbare Bildschirmfläche
   - simple GUI, welche sowohl Maus- als auch Tastaturbedienung unterstützt
     - muss nicht zwingend wiederverwendbar für andere Programme sein
   - Unterstützung mehrerer Ebenen (Bild- und Zeichenebenen)
   - Verschieben und Bearbeiten von Ebenen

## Optionale Erweiterungen (je nach Zeitrahmen)
- mehr Bearbeitungswerkzeuge (z.B. verschiedene Pinselformen)
- Speichern und Laden von Projektdateien (als eigenes Format)
- Historie mit Implementierung von Undo/Redo-Funktionalität

## Technische Anforderungen
- Entwicklung in der vom Betriebssystem unterstützten Programmiersprache
- Berücksichtigung der Speicher- und Leistungseinschränkungen des Zielsystems
   - insofern optimiert, dass es flüssig genug läuft
- Implementierung einer intuitiven grafischen Benutzeroberfläche
