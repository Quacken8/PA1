# hw06 - Osmisměrky

Úkolem je vytvořit program, který bude řešit hlavolam - osmisměrku.

Osmisměrka je zadaná jako 2D pole znaků a seznam slov. Cílem je v poli znaků nalézt slova ze seznamu a "vyškrtnout je". Zbylá (neškrtnutá) písmena tvoří tajenku. Slova se v osmisměrce mohou nacházet kdekoliv - horizontálně, vertikálně i diagonálně, čtená zleva doprava, shora dolů i naopak. Jedno písmeno může být použito ve více vyškrtnutých slovech, škrtaná slova se mohou překrývat, slovo v seznamu může být v poli obsaženo vícekrát (pak je potřeba vyškrtat všechny výskyty).

Vstupem programu je zadání osmisměrky. Nejprve je zadané obdélníkové 2D pole s písmeny. Může obsahovat znaky malé a až z nebo znak tečka (nepoužité políčko). Pole musí mít alespoň jeden řádek a alespoň jeden sloupec. Po zadání osmisměrky následuje prázdný řádek a za ním seznam příkazů. Každý příkaz je na novém řádku. Příkazy jsou 3 druhů:

- \- slovo: vyškrtnutí slova. Za znakem mínus může být nula, jeden nebo více bílých znaků a slovo k vyškrtnutí. Slovo je tvořeno písmeny malé abecedy. Program najde všechny výskyty zadaného slova v osmisměrce a označí si je za přeškrtnuté. Při hledání slov program uvažuje i již přeškrtnutá písmena, škrtnutí má význam pouze pro výpis tajenky. Odpovědí je počet nalezených výskytů daného slova.
- \# slovo: vyhledání zadaného slova. Příkaz pracuje stejně jako vyškrtávání slov, jen nic neškrtá, pouze slova vyhledá a vypíše počet nalezených výskytů slova.
- ?: zobrazí tajenku, tedy všechna dosud neškrtnutá písmena. Výpis může být dlouhý, program jej proto rozdělí na řádky délky 60 znaků + případný zbytek na poslední řádce. Pokud jsou všechna písmena vyškrtána, je tajenka prázdné slovo. Program v takovém případě nezobrazí nic (zobrazí pouze řádek s textem "Tajenka").

Program musí detekovat nesprávný vstup. Pokud je vstup nesprávný, program to zjistí, vypíše chybové hlášení a ukončí se. Za chybu je považováno:

- pole osmisměrky nemá velikost alespoň 1x1,
- pole osmisměrky není obdélníkového tvaru,
- pole osmisměrky obsahuje znaky jiné než malá písmena a tečku,
- chybí prázdný řádek za zadáním pole osmisměrky,
- znak pro příkaz (-, #, ?) chybí, není rozpoznaný nebo není první na řádce,
- hledané slovo není platné (musí obsahovat pouze znaky malé abecedy, musí být alespoň 2 znaky dlouhé),
- za příkazy - a # musí být hledané slovo, na řadce ještě mohou být bílé znaky mezi znakem pro příkaz a hledaným slovem, ale nic jiného,
- za příkazem ? nesmí být žádné další znaky.

Dodržte přesně formát všech výpisů. Výpis Vašeho programu musí přesně odpovídat ukázkám. Testování provádí stroj, který kontroluje výpis na přesnou shodu. Pokud se výpis Vašeho programu liší od referenčního výstupu, je Vaše odpověď považovaná za nesprávnou. Záleží i na mezerách, i na odřádkování.

Váš program bude spouštěn v omezeném testovacím prostředí. Je omezen dobou běhu a dále je omezena i velikost dostupné paměti. Počítejte s tím, že šířka/délka obdélníku může mít velké rozpětí. Není rozumné paměť pro osmisměrku alokovat staticky. Paměti je ale obecně dostatek pro případné předzpracování osmisměrky nebo hledaných slov. Hodnocení úlohy záleží na efektivitě použitého algoritmu:

- Naivní algoritmus projde povinnými a nepovinnými testy, nezvládne však test bonusový. Bude hodnocen nejvýše nominálním počtem bodů.
- Vylepšený algoritmus musí rychle zpracovat velké zadané osmisměrky. První bonus zadává velké osmisměrky, ze kterých vyškrtává relativně malý počet slov. Druhý rovněž bonus zadává velké osmisměrky a vyhledává v nich velké množství slov. Druhý bonus používá zejména příkazy #, tedy je potřeba optimalizovat rychlost vyhledávání (není potřeba optimalizovat rychlost škrtání).

## Ukázkové běhy programu

```
Osmismerka:
test...x
progtest

?
Tajenka:
testxprogtest
#test
test: 2x

# test

test: 2x

# test

test: 2x

# progte

progte: 1x

# est

est: 2x
?
Tajenka:
testxprogtest

- test
  test: 2x
- program
  program: 0x
- test
  test: 2x
  ?
  Tajenka:
  xprog
```

Protože jsou ukázky poměrně dlouhé, jsou další ukázkové běhy pouze v přiloženém archivu.

## Poznámky:

- Ukázkové běhy zachycují očekávané výpisy Vašeho programu (tučné písmo) a vstupy zadané uživatelem (základní písmo). Zvýraznění tučným písmem je použité pouze zde na stránce zadání, aby byl výpis lépe čitelný. Váš program má za úkol pouze zobrazit text bez zvýrazňování (bez HTML markupu).
- Znak odřádkování (\n) je i za poslední řádkou výstupu (i za případným chybovým hlášením).
- Délky řetězců ani velikost osmisměrky není omezena. Alokujte je dynamicky podle aktuální potřeby.
- Správné řešení této úlohy, které zvládne povinné a nepovinné testy na 100%, lze použít pro code review (tj. pro code review nemusíte zvládnout bonusový test).
