# hw01a: Obsah obdélníků

Úkolem je realizovat program, který bude rozhodovat o obsahu zadaných obdélníků.

Předpokládáme, že v rovině jsou dva obdélníky, každý je určen svoji výškou a šířkou. Výška a šířka jsou desetinná čísla větší než 0.

Program dostane na vstupu 4 čísla, tato reprezentují výšku a šířku prvního obdélníku a výšku a šířku druhého obdélníku.

Výstupem programu je rozhodnutí, zda mají zadané obdélníky stejný obsah. Výstupem je odpověď, který z obdélníků má menší obsah, případně informace o shodném obsahu. Přesný formát odpovědi je vidět v ukázkách.

Pokud vstup není platný (na vstupu jsou nečíselné nebo nesmyslné hodnoty), program tuto situaci detekuje a vypíše chybové hlášení. Formát chybového hlášení je opět uveden v ukázkách níže. Za chybu je považováno, pokud je na vstupu:

- nečíselná hodnota nebo
- nesmyslná hodnota (záporná hodnota, nula, zadání chybí).

Pokud program detekuje chybu, přestane se dotazovat na další vstupní hodnoty, vypíše chybové hlášení a ukončí se. Chybu je tedy potřeba detekovat okamžitě po načtení hodnoty (neodkládejte kontrolu vstupních údajů až za načtení celého vstupu). Chybové hlášení vypisujte na standardní výstup (nevypisujte jej na standardní chybový výstup).

Dodržte přesně formát všech výpisů. Výpis Vašeho programu musí přesně odpovídat ukázkám. Testování provádí stroj, který kontroluje výpis na přesnou shodu. Pokud se výpis Vašeho programu liší od referenčního výstupu, je Vaše odpověď považovaná za nesprávnou. Záleží i na mezerách, i na odřádkování. Nezapomeňte na odřádkování za posledním řádkem výstupu (a za případným chybovým hlášením). Využijte přiložený archiv s testovacími vstupy a usnadněte si testování Vašeho programu.

Váš program bude spouštěn v omezeném testovacím prostředí. Je omezen dobou běhu (limit je vidět v logu referenčního řešení) a dále je omezena i velikost dostupné paměti (ale tato úloha by ani s jedním omezením neměla mít problém).

## Ukázka práce programu:

```
Obdelnik #1:
7.5 12
Obdelnik #2:
6 15
Obdelniky maji stejny obsah.
```

```
Obdelnik #1:
22 18
Obdelnik #2:
13 9
Obdelnik #2 ma mensi obsah.
```

```
Obdelnik #1:
 133e4   4.25
Obdelnik #2:
4e4 5.28e3
Obdelnik #1 ma mensi obsah.
```

```
Obdelnik #1:
0.2 0.1731
Obdelnik #2:
0.15 0.2308
Obdelniky maji stejny obsah.
```

```
Obdelnik #1:
12 4
Obdelnik #2:
-2 5
Nespravny vstup.
```

```
Obdelnik #1:
4 abcd
Nespravny vstup.
```

## Poznámky:

- Ukázkové běhy zachycují očekávané výpisy Vašeho programu (tučné písmo) a vstupy zadané uživatelem (základní písmo). Zvýraznění tučným písmem je použité pouze zde na stránce zadání, aby byl výpis lépe čitelný. Váš program má za úkol pouze zobrazit text bez zvýrazňování (bez HTML markupu).
- Znak odřádkování (\n) je i za poslední řádkou výstupu (i za případným chybovým hlášením).
