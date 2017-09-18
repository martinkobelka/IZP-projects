/*
 * Název vsouboru:    proj1.c
 * Datum vytvoření:   6.11.2016
 * Autor projektu:    Martin Kobelka (xkobel02)
 * 
 * 
 * Projekt: Projekt č.1 do předmětu IZP, Práce s textem
 * Popis:   Program buď binární data formátuje do textové podoby 
 *          nebo textovou podobu dat převádí do binární podoby.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

// Parametry příkazové řádky
#define VYPIS_NA_JEDEN_RADEK "-x"
#define BINARNI_NA_TEXT "-r"
#define TISK_TEXTOVYCH_RETEZCU "-S"
#define ZACATEK_VYPISU "-s"
#define DELKA_VYPISU "-n"

// Definice chybových kodu
#define ARGUMENT_VYZADUJE_SPECIFIKATOR 0
#define SPECIFIKATOR_NENI_CISLO 1
#define SPECIFIKATOR_NENI_V_ROZSAHU 2
#define SPATNE_ZADANY_ZNAK 3
#define CHYBNY_ARGUMENT_PRIKAZOVE_RADKY 4

// Počet bytů na jeden řádek
#define DELKA_VYPSANEHO_RETEZCE 16

// ## Prototypy funkcí ##

//Funkce pro práci s řetězci
int delkaRetezce(char *retezec);
int porovnejRetezce(char *prvniRetezec, char *druhyRetezec);
void vyplnRetezecMezerami(unsigned char (*retezec)[DELKA_VYPSANEHO_RETEZCE + 1], 
        int delkaRetezce);
// Funkce pro výpis statických informací
void vypisNapovedy();
void vypisChybu(int chybovyKod);
// Funkce vyvolány zadáním konkrétního paraemtru.
void vypisTextoveRetezce(int minimalniDelka);
void spustBezParametru();
void vypisNaJedenRadek();
void spustSParametry(int pocatek, int pocet);
int prevedNaText();
//Funkce pro výpis dynamických informací
void vypisAdresuPrvnihoBajtu(int cisloAdresy);
void vypisMezeryZaPoslednimZnakem(int pocetPrazdnychZnaku);

int main(int argc, char **argv) {
    
    /*
     * Program se na základě počtu argumentů příkazové řádky rozhoduje, 
     * jakou akci spustí.
     */
    
    switch(argc) {
        char *neniCislo;

        case 1:
            spustBezParametru();
            break;
        case 2:
            // Podle zvoleného argumentu vyvoláme danou akci
            if(porovnejRetezce(argv[1], VYPIS_NA_JEDEN_RADEK)) {
                vypisNaJedenRadek();
            }
            else if(porovnejRetezce(argv[1], BINARNI_NA_TEXT)) {
                if(!prevedNaText())
                    return (EXIT_FAILURE);
            }
            else if(porovnejRetezce(argv[1], TISK_TEXTOVYCH_RETEZCU)) {
                vypisChybu(ARGUMENT_VYZADUJE_SPECIFIKATOR);
                return (EXIT_FAILURE);
            }
            else if(porovnejRetezce(argv[1], ZACATEK_VYPISU)) {
                vypisChybu(ARGUMENT_VYZADUJE_SPECIFIKATOR);
                return (EXIT_FAILURE);
            } 
            else if(porovnejRetezce(argv[1], DELKA_VYPISU)) {
                vypisChybu(ARGUMENT_VYZADUJE_SPECIFIKATOR);
                return (EXIT_FAILURE);
            } 
            else {
                // V případě neznámého argumentu vyvoláme výpis nápovědy
                vypisNapovedy();
            }
            break;
        case 3:
            
            if(porovnejRetezce(argv[1], TISK_TEXTOVYCH_RETEZCU)) {
                char *chyba;
                long specifikator = strtol(argv[2], &chyba, 10);
                
                if(*chyba != 0) {
                    vypisChybu(SPECIFIKATOR_NENI_CISLO);
                    return (EXIT_FAILURE);
                }
                else {
                    if(specifikator > 0 && specifikator < 200)
                        vypisTextoveRetezce(specifikator);
                    else {
                        vypisChybu(SPECIFIKATOR_NENI_V_ROZSAHU);
                        return (EXIT_FAILURE);
                    }
                }
            }
            else if(porovnejRetezce(argv[1], ZACATEK_VYPISU)) {
                char *chyba;
                long specifikator = strtol(argv[2], &chyba, 10);
                if(*chyba != 0) {
                    vypisChybu(SPECIFIKATOR_NENI_CISLO);
                    return (EXIT_FAILURE);
                }
                else {
                    spustSParametry(specifikator, INT_MAX);
                }
            }
            else if(porovnejRetezce(argv[1], DELKA_VYPISU)) {
                char *chyba;
                long specifikator = strtol(argv[2], &chyba, 10);
                if(*chyba != 0) {
                    vypisChybu(SPECIFIKATOR_NENI_CISLO);
                    return (EXIT_FAILURE);
                }
                else {
                    spustSParametry(0, specifikator);
                }
            }
            else {
                vypisNapovedy();
            }
            break;
        case 4:
            for(int i = 1; i < argc; i++) {
                
                strtol(argv[i], &neniCislo, 10);
                if(!porovnejRetezce(VYPIS_NA_JEDEN_RADEK, argv[i]) && 
                        !porovnejRetezce(VYPIS_NA_JEDEN_RADEK, argv[i]) &&
                        !porovnejRetezce(BINARNI_NA_TEXT, argv[i])&&
                        !porovnejRetezce(TISK_TEXTOVYCH_RETEZCU, argv[i])&&
                        !porovnejRetezce(ZACATEK_VYPISU, argv[i])&&
                        !porovnejRetezce(DELKA_VYPISU, argv[i])&&
                        *neniCislo != 0) {
                    vypisNapovedy();
                    return(EXIT_SUCCESS);
                }
                
            }
            vypisChybu(CHYBNY_ARGUMENT_PRIKAZOVE_RADKY);
            return(EXIT_FAILURE);
            break;
        case 5: 
            if(porovnejRetezce(argv[1], ZACATEK_VYPISU) && 
                    porovnejRetezce(argv[3], DELKA_VYPISU)) {
                char *chyba;
                long parametr = strtol(argv[2], &chyba, 10);
                if(*chyba != 0) {
                    vypisChybu(SPECIFIKATOR_NENI_CISLO);
                    return (EXIT_FAILURE);
                }
                long druhyParametr = strtol(argv[4], &chyba, 10);
                if(chyba == NULL) {
                    vypisChybu(SPECIFIKATOR_NENI_CISLO);
                    return (EXIT_FAILURE);
                }
                
                spustSParametry(parametr, druhyParametr);
                
            }
            else if(porovnejRetezce(argv[3], ZACATEK_VYPISU) && 
                    porovnejRetezce(argv[1], DELKA_VYPISU)) {
                char *chyba;
                long parametr = strtol(argv[2], &chyba, 10);
                if(*chyba != 0) {
                    vypisChybu(SPECIFIKATOR_NENI_CISLO);
                    return (EXIT_FAILURE);
                }
                long druhyParametr = strtol(argv[4], &chyba, 10);
                if(*chyba != 0L) {
                    vypisChybu(SPECIFIKATOR_NENI_CISLO);
                    return (EXIT_FAILURE);
                }
                
                spustSParametry(druhyParametr, parametr);
            }
            else {
                vypisChybu(CHYBNY_ARGUMENT_PRIKAZOVE_RADKY);
                return (EXIT_FAILURE);
            }
            break;
        default:
            //argc > 5 tak vypíšeme nápovědu a ukončíme
            vypisNapovedy();
            break;
    }
    return (EXIT_SUCCESS);
}

/**
 * Funkce vypíše nápovědu na standatní výstup.
 */
void vypisNapovedy() {
    printf(
      "Program buďto provádí formátování binárních dat do textové podoby, nebo\n"
      "provádí převod textových dat do binarní podoby.\n"
      "Program lze spustit v následujících podobách:\n"
      "\t./proj1\tProgram převede textový vstup na binarmí a vytiskne 16"
      "bajtů na řádek\n"
      "\t./proj1 [-s M] [-n N] Program převede textový vstup na binarní."
      "Bude převádět znak začínající na N o délce M\n"
      "\t./proj1 -x Veškerá vstupní data budou převedena do "
      "hexadecimální podoby na jeden řádek\n"
      "\t./proj1 -S N Program bude tisknout pouze takové posloupnosti v"
      "binárním stupu, které vypadají jako textový řetězec.\n"
      "\t./proj1 -r Program očekává na vstup sekvenci hexadecimálních číslic. "
      "Program převádí tato čísla do binárního formátu.\n"
   );
}

/**
 * Funkce se spustí se zadáním argumentu -x. Vezme veškerá vstupní data a 
 * převede je do hexadecimální podoby na 1 jeden řádek.
 */
void vypisNaJedenRadek() {
    int c;
    while((c = getchar()) != EOF) {
        printf("%02x", c);
    }
    putchar('\n');
}

/**
 * Funkce se spustí po zadání argumentu -S. Bude ve stupu hledat takové
 * posloupnosti, které vypadají jako textové řetězce a tisknout je.
 * 
 * @param minimalniDelka Minimální délka vypisovaného řetězce
 */
void vypisTextoveRetezce(int minimalniDelka) {
    
    /* Používá se buffer o minimální délce vypisované posloupnosti + 1 znak. 
     * V případě, že dosáhneme požadované délky, vypíšeme buffer,
     * A vypisujeme každý další znak. */
    
    unsigned char buffer[minimalniDelka + 1];
    buffer[minimalniDelka] = '\0';
    
    int jeNecoVypsano = false;
    
    bool prvni = true;
    int ctenyZnak;
    int pocetZnaku = 1;
    while((ctenyZnak = getchar()) != EOF) {
        if(!isblank(ctenyZnak) && !isprint(ctenyZnak)) {
            pocetZnaku = 0;
        }
        if(pocetZnaku > minimalniDelka) {
            jeNecoVypsano = true;
            putchar(ctenyZnak);
        }        
        else if(pocetZnaku <= minimalniDelka) {
            buffer[pocetZnaku - 1] = ctenyZnak;
            if(pocetZnaku == minimalniDelka) {
                if(prvni != true) {
                    putchar('\n'); 
                }
                jeNecoVypsano = true;
                printf("%s", buffer);
                prvni = false;
            }
        }
        pocetZnaku++;
    }
    if(jeNecoVypsano)
        putchar('\n');
}

/**
 * Funkce vrátí délku řetězce
 * 
 * @param retezec Ukazatel na první znak řětězce
 * @return Délka řetězce
 */
int delkaRetezce(char *retezec) {
    char *pocatek = retezec;
    
    // V případě že nejsme na konci řetězce, tak zvýšíme hodnotu ukazatele o 1
    while(*retezec != '\0') {
        retezec++;
    }
    
    /* Odečteme od sebe ukazatel na první posldní znak řetězce a 
     * ukazatel na první znak. Výsledek je délka řetězce.
     */
    return retezec - pocatek;
}

/**
 * Porovná dva řetězce
 * 
 * @param prvniRetezec První řetězec k porovnání
 * @param druhyRetezec Druhý řetězec k porovnání
 * @return V případě rovností vrací 1. V opačném případě vrací 0
 */
int porovnejRetezce(char *prvniRetezec, char *druhyRetezec) {
    // V případě že se nerovají délky řetězců, nemohou se sobě řetězce rovnat
    int delkaRetezcu;
    if((delkaRetezcu = delkaRetezce(prvniRetezec)) 
            != delkaRetezce(druhyRetezec)) {
        return 0;
    }
    
    //Procházíme znak po znaku a kontrolujee
    for(int i = 0; i < delkaRetezcu; i++) {
        if(*prvniRetezec != *druhyRetezec)
            return 0;
        else {
            prvniRetezec++;
            druhyRetezec++;
        }
    }
    
    return 1;
}

/**
 * Funkce vypíše chybu na standartní chybový výstup.
 * 
 * @param chybovyKod Kod chyby
 */
void vypisChybu(int chybovyKod){
    switch(chybovyKod) {
        case ARGUMENT_VYZADUJE_SPECIFIKATOR:
            fprintf(stderr, "Tento argument vyžaduje specifikátor!");
            break;
        case SPECIFIKATOR_NENI_CISLO:
            fprintf(stderr, "Specifikátor musí být číslo!");
            break;
        case SPECIFIKATOR_NENI_V_ROZSAHU:
            fprintf(stderr, "Specifikátor musí být 0 < N < 200");
            break;
        case SPATNE_ZADANY_ZNAK:
            fprintf(stderr, "Chybný znak");
            break;
        case CHYBNY_ARGUMENT_PRIKAZOVE_RADKY:
            fprintf(stderr, "Chybné argumenty příkazové řádky");
            break;
    }
}

/**
 * Vyvolána ve chvíli, kdy je program spuštěn bez parametrů
 */
void spustBezParametru() {
    spustSParametry(0, INT_MAX);
}

/**
 * Funkce začne vypisovat  až od určité adresy a jen určený počet znaků.
 * 
 * @param pocatek adresa začátku výpisu
 * @param pocet maximální délka vstupních bajtů
 */
void spustSParametry(int pocatek, int pocet) {
    int aktualniZnak = pocatek;
    int pocetZnaku = 0;
    int c;
    unsigned char znaky[DELKA_VYPSANEHO_RETEZCE + 1];
    vyplnRetezecMezerami(&znaky, DELKA_VYPSANEHO_RETEZCE);
    
    for(int i = 0; i < pocatek; i++) {
        if(getchar() == EOF)
            break;
    }
    
    while((c = getchar()) != EOF && pocetZnaku < pocet) {
        if(pocetZnaku == 0 || (pocetZnaku % 16) == 0) {
            vypisAdresuPrvnihoBajtu(aktualniZnak);
        }
        
        printf("%02x ", c);
        if(isprint(c)) {
            znaky[pocetZnaku % DELKA_VYPSANEHO_RETEZCE] = c;
        }
        else
            znaky[pocetZnaku % DELKA_VYPSANEHO_RETEZCE] = '.';
        aktualniZnak++;        
        pocetZnaku++;
        if((pocetZnaku % 8) == 0)
            putchar(' ');
        
        if((pocetZnaku % DELKA_VYPSANEHO_RETEZCE) == 0) {
            printf("|%s|\n", znaky);
            vyplnRetezecMezerami(&znaky, DELKA_VYPSANEHO_RETEZCE);
        }
    }
    if((pocetZnaku % DELKA_VYPSANEHO_RETEZCE) != 0) {
        vypisMezeryZaPoslednimZnakem(DELKA_VYPSANEHO_RETEZCE - 
                (pocetZnaku % DELKA_VYPSANEHO_RETEZCE));
        printf("|%s|\n", znaky);
    }
}

/**
 * Spustí se se zadáním argumentu -r. Převádí sekvenci hexadecimálních číslic
 * které převádí do binárního výstupu.
 * 
 * @return v případě chyby vrací -1, v případě úspěchu vrací 1
 */
int prevedNaText() {
    
    char buffer[3];
    buffer[2] = '\0';
    
    char bufferJednomistny[2];
    buffer[1] = '\0';
    
    bool jednomistne = false;
    
    char *chybaCteni;
    long hodnotaCisla;
    
    int znak;
    while((znak = getchar()) != EOF) {
        if(!isspace(znak)) {
            jednomistne = !jednomistne;
            if((znak >= '0' && znak <= '9') ||
                    (znak >= 'a' && znak <= 'f') ||
                    (znak >= 'A' && znak <= 'F')) {
                if(jednomistne)
                    buffer[0] = znak;
                else{
                    buffer[1] = znak;
                    hodnotaCisla = strtol(buffer, &chybaCteni, 16);
                    if(chybaCteni != NULL)
                        printf("%c", (int)hodnotaCisla);
                    else
                        return -1;
                }
            }
            else{
                vypisChybu(SPATNE_ZADANY_ZNAK);
                return -1;
            }
        }
    }
    
    if(jednomistne) {
        bufferJednomistny[0] = buffer[0];
        hodnotaCisla = strtol(bufferJednomistny, &chybaCteni, 16);
        if(chybaCteni != NULL)
            printf("%c", (int)hodnotaCisla);
        else
            return -1;
    }
    
    return 1;
}

/**
 * Všechny znaky vstupního řetězce nahradí mezerami.
 * 
 * @param retezec Vstupní řetězec
 * @param delkaRetezce Délka vstupního řetězce
 */
void vyplnRetezecMezerami(unsigned char (*retezec)[DELKA_VYPSANEHO_RETEZCE + 1], 
        int delkaRetezce) {
    for(int i = 0; i < delkaRetezce; i++) {
        (*retezec)[i] = ' ';
    }
    (*retezec)[delkaRetezce] = '\0';
}

/**
 * Funkce vypíše adresu prvního bajtu v hexadecimálním tvau na 8 znaků.
 * V případě, že je adresa krátká, nahradí znaky nulami.
 * 
 * @param cisloAdresy Číslo adresy dekadicky
 */
void vypisAdresuPrvnihoBajtu(int cisloAdresy) {
    printf("%08x  ", cisloAdresy);
}

/**
 * Funkce vypisuje mezery za poslední bajt pro dodržení požadovaného formátu.
 * 
 * @param pocetPrazdnychZnaku
 */
void vypisMezeryZaPoslednimZnakem(int pocetChybejicichZnaku) {
    
    for(int i = 0; i < pocetChybejicichZnaku; i++) {
        printf("   ");
    }
    if(pocetChybejicichZnaku >= 8) {
        printf("  ");
    }
    else
        putchar(' ');
}
