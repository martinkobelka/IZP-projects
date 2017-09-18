/*
 * Název vsouboru:    proj2.c
 * Datum vytvoření:   27.11.2016
 * Autor projektu:    Martin Kobelka (xkobel02)
 * 
 * 
 * Projekt: Projekt č.2 do předmětu IZP, Iterační výpočty
 * Popis:   Program počítá přirozený logaritmus z daného čísla, nebo 
 *          mocninu o reálném základu a exponentu na zadaný počet iterací.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>

// Parametry příkazové řádky
#define PARAMETR_LOG "--log"
#define PARAMETR_EXP "--pow"

// Způsoby výpočtu
#define TAYLORUV_POLYNOM 0
#define ZRETEZENE_ZLOMKY 1

// Chybové kódy
#define CHYBNE_ARGUMENTY 0
#define CHYBNY_SPECIFIKATOR 1

// Konstanty pro premiový obsah
#define EULEROVO_CISLO 2.71828182845904523536028747135266249775724709369995
#define DOLNI_MEZ 0.60653066
#define HORNI_MEZ 1.64872127

// Chybové zprávy
const char *chyboveZpravy[] = {
    "Chybné argumenty příkazové řádky!",
    "Chybný specifikátor pro daný argument!"
};

/* ## Prototypy funkcí ## */

// Funkce pro výpočet logaritmu
double taylor_log(double x, unsigned int n);
double cfrac_log(double x, unsigned int n);

// Funkce pro výpočet mocniny 
double taylor_pow(double x, double y, unsigned int n);
double taylorcf_pow(double x, double y, unsigned int n);
double pocitadloPow(double x, double y, unsigned int n, int zposobVypoctu);
// Funkce pro vypsání statických informací
void vypisChybu(int chybovyKod);

// Funkce pro výpis dynamických informací
void vypisLogarimus(double zaklad, unsigned int pocetIteraci);
void vypisExponencialniFunkce(double x, double y, unsigned int n);

// Funkce pro načítání argumentů
int nactiArgumentyLog(char **argv, double *hodnotaLogarimu, 
        long int *pocetIteraci);
int nactiArgumentyExp(char **argv, double *zakladMocniny, 
        double *exponentMocniny, long int *pocetIteraci);

// Funkce tvořící alternativy k funkcím z matematické knihovny
// ## Za prémiové body ##
double mylog(double x);
double mypow(double x, double y);


/**
 * Hlavní funkce, spustí se ihned po spuštění programu
 * 
 * @param argc Počet argumentů příkazové řádky
 * @param argv Pole argumentů příkazové řádky
 * @return V případě úspěšného průběhu programu vrací 0, v opačném případě -1
 */
int main(int argc, char **argv) {
    // Podle počtu argumentů v argc se budeme rozhodovat jakou akci provedeme.
    switch(argc) {
        case 4:
            // Při čtyřech argumentech počítáme přirozený logaritmus
            if(strcmp(argv[1], PARAMETR_LOG) == 0) {
                // První parametr je --log
                double hodnotaLogarimu;
                long int pocetIteraci;
                
                /* Načteme argumenty pro výpočet logaritmu. V případě neúspěchu
                 * program s chybou ukončíme. */
                if(!nactiArgumentyLog(argv, &hodnotaLogarimu, &pocetIteraci)) {
                    vypisChybu(CHYBNY_SPECIFIKATOR);
                    return (EXIT_FAILURE);
                }
                
                // V případě úspěšného načtení argumentů vypisujeme logaritmus.
                vypisLogarimus(hodnotaLogarimu, pocetIteraci);
            }
            else {
                // První parametr není --log, vypíše chybu
                vypisChybu(CHYBNE_ARGUMENTY);
                return (EXIT_FAILURE);
            }
            break;
        case 5:
            // Při pěti argumentech počítáme mocninu o reálném základu
            if(strcmp(argv[1], PARAMETR_EXP) == 0) {
                // První parametr je --pow
                double zakladMocniny;
                double exponentMocniny;
                long int pocetIteraci;
                
                // Načteme argumenty pro výpočet mocniny
                if(!nactiArgumentyExp(argv, &zakladMocniny, &exponentMocniny,
                        &pocetIteraci)) {
                    vypisChybu(CHYBNY_SPECIFIKATOR);
                    return (EXIT_FAILURE);
                }
                
                // V případě úspěchu vypíšeme
                vypisExponencialniFunkce(zakladMocniny, exponentMocniny,
                        pocetIteraci);
                
            }
            else {
                // První parametr není --pow, vypíše chybu
                vypisChybu(CHYBNE_ARGUMENTY);
                return (EXIT_FAILURE);
            }
            break;
        default:
            /* Program lze úspěšně spustit jen se třemi nebo čtyřmi argumenty. 
             * V opačném případě vrací chybu */
            
            vypisChybu(CHYBNE_ARGUMENTY);
            return (EXIT_FAILURE);
            break;
    }
    
    // Pokud program nebyl ukončen s chybou, ukončíme jej úspěšně.
    return (EXIT_SUCCESS);
}

/**
 * Vypočítá přirozený logaritmus za pomoci taylorova polynomu. Používá dva
 * vzorce pro x <= 1 nebo x > 1
 * 
 * @param x Základ logarimu
 * @param n Počet iterací
 * @return Hodnota logaritmu
 */
double taylor_log(double x, unsigned int n) {
    
    // Extrémní a krizové stavy jsou ošetřeny přímou podmínkou.
    if(x < 0)
        return NAN;
    if(x == 0)
        return -INFINITY;
    if(isinf(x))
        return INFINITY;
    
    double vysledek = 0;
    
    // Pokud je i <= 1, použijeme první vzorec pro taylorův polynom.
    if(x <= 1) {
    
        double zakladMocniny = 1 - x;
        double citatel = zakladMocniny;
        /* Počítáme výsledek za pomocí taylorova polynomu 
         * o zadaném počtu iterací */
        for(unsigned int i = 1; i <= n; i++) {
            // Následující člen se vypočítá na základdě členu předchozího.
            vysledek -= citatel / i;
            citatel *= zakladMocniny;
        }

        return vysledek;
    }
    // Pokud je x > 1, použijeme druhý vzorec pro taylorův polynom.
    else {
        double zakladMocniny = (x - 1)/x;
        double citatel = zakladMocniny;
        /* Opakujeme tolikrát, kolik iterací nám bylo zadáno. */
        for(unsigned int i = 1; i <= n; i++) {
            // Následující člen se vypočítá odvozením od členu předcozího.
            vysledek += citatel / i;
            citatel *= zakladMocniny;
        }
        
        return vysledek;
    }
}

/**
 * Počítá hodnotu logaritmu za pomoci zřetězeného zlomku
 * 
 * @param x Číslo ze kterého se počítá logaritmus
 * @param n Počet iterací
 * @return hodnota logarimu
 */
double cfrac_log(double x, unsigned int n) {
    
    // Extrémní a krizové stavy jsou ošetřeny podmínkou.
    if(x < 0)
        return NAN;
    if(x == 0)
        return -INFINITY;
    if(isinf(x))
        return INFINITY;
    
    // hodnota z je vyjádřena ze vzorce.
    double z = (x - 1) / (1 + x);
    
    double druhaMocnina = z*z;
    double cf = 0.0;
    
    int i = n - 1;
    while(i >= 1) {
        cf = (i*i*druhaMocnina)/(1+i*2 - cf);
        i = i - 1;
    }
    
    return  2*z/(1-cf);
}

/**
 * Funkce vypíše chybový kod na standartní chybový výstup
 * 
 * @param chybovyKod Kod chyby
 */
void vypisChybu(int chybovyKod) {
    fprintf(stderr, "%s\n", chyboveZpravy[chybovyKod]);
}

/**
 * Funkce vypíše logarimus ve správném tvaru pro OVĚŘENÉ vstupní hodnoty.
 * Funkce neprovádí vnitřní kontrolu
 * 
 * @param zaklad Základ logarimu
 * @param pocetIteraci Počet iterací
 */
void vypisLogarimus(double zaklad, unsigned int pocetIteraci) {
    printf("       log(%g) = %.12g\n"
           " cfrac_log(%g) = %.12g\n"
           "taylor_log(%g) = %.12g\n", 
            zaklad,
            log(zaklad), 
            zaklad,
            cfrac_log(zaklad, pocetIteraci),
            zaklad,
            taylor_log(zaklad, pocetIteraci)
            );
}

/**
 *  Funkce vypíše hdontu exponenciální funkce ve správném tvaru pro OVĚŘENÉ 
 *  vstupní hodnoty. Funkce neprovádí vnitřní kontrolu
 * 
 * @param x Základ mocniny
 * @param y Exponent mocniny
 * @param n počet iterací
 */
void vypisExponencialniFunkce(double x, double y, unsigned int n) {
    
    printf("         pow(%g,%g) = %.12g\n"
           "  taylor_pow(%g,%g) = %.12g\n"
           "taylorcf_pow(%g,%g) = %.12g\n",
            x, y, pow(x, y),
            x, y, taylor_pow(x, y, n), 
            x, y, taylorcf_pow(x, y, n)
            );
}

/**
 * Funkce načte argumenty příkazové řádky pro logarimus. V případně chybného 
 * argumentů přeruší svou činnost a okamžitě vrací false
 * 
 * @param argv Argumenty pčíkazové řádky
 * @param hodnotaLogarimu Ukazatel na hodnotu logarimu
 * @param pocetIteraci Ukazatel na počet iterací
 * @return  Vrací 0 pokud bylo vše načteno, v opačném případě vrací -1 pro
 *          chybný specifikátor.
 */
int nactiArgumentyLog(char **argv, double *hodnotaLogarimu, 
    long int*pocetIteraci) {
    
    char *chyba;
    *hodnotaLogarimu = strtod(argv[2], &chyba);
    
    // V případě že hodnotaLogarimu není číslo, vrátíme chybu
    if(*chyba != 0) {
        return 0;
    }
    
    // V případě že pocetIteraci není číslo, vrátíme chybu
    *pocetIteraci = strtol(argv[3], &chyba, 10);
    if(*chyba != 0) {
        return 0;
    }
          
    /* V případě že pocetIteraci neodpovídá rozsahuje datového typu 
     * unsigned int, vrátíme chybu */
    if(*pocetIteraci < 1 || *pocetIteraci > UINT_MAX) {
        return 0;
    }
    
    // V případě korektních hodnot vracíme 0
    return 1;
}

/**
 * Funkce načte argumenty pro výpočet e
 * 
 * @param argv argumenty příkazové řádky
 * @param zakladMocniny Ukazatel na základ mocniny
 * @param exponentMocniny Ukazatel na exponent mocniny
 * @param pocetIteraci Ukazatel na počet iterací
 * @return 0 v případě úspěchu, -1 v případě chybného specifiktoru
 */
int nactiArgumentyExp(char **argv, double *zakladMocniny, 
        double *exponentMocniny, long int *pocetIteraci) {
    
    char *chyba;
    
    // V případě že základ mocniny není číslo, vrátíme chybu
    *zakladMocniny = strtod(argv[2], &chyba);
    if(*chyba != 0) {
        return 0;
    }
    
    // V případě že exponent mocniny není číslo, vrátíme chybu
    *exponentMocniny = strtod(argv[3], &chyba);
    if(*chyba != 0) {
        return 0;
    }
    
    // V případě že počet iterací není číslo, vrátíme chybu
    *pocetIteraci = strtol(argv[4], &chyba, 10);
    if(*chyba != 0) {
        return 0;
    }
    
    /* V případě, kdy počet iterací je hodnotami 
     * mimo svůj datový typ, skončí s chybou */
    if(*pocetIteraci < 1 || *pocetIteraci > UINT_MAX) {
        return 0;
    }
    
    return 1;               
}

/**
 * Rozhraní pro výpočet exponenciální funkce s obecným základem.
 * Pro výpočet ln(a) bude použita metoda taylorova polynomu.
 * 
 * @param x Základ mocniny
 * @param y Exponent mocniny
 * @param n Počet iterací
 * @return Hodnota mocniny s obecným základem a exponentem
 */
double taylor_pow(double x, double y, unsigned int n) {
    return pocitadloPow(x, y, n, TAYLORUV_POLYNOM);
}

/**
 * Rozhraní pro výpočet exponenciální funkce s obecným základem.
 * Pro výpočet ln(a) bude ppoužita metoda zřetězeného zlomku.
 * 
 * @param x Základ mocniny
 * @param y Exponent mocninys
 * @param n Počet iterací
 * @return Hodnota mocniny s obecným základem a exponentem
 */
double taylorcf_pow(double x, double y, unsigned int n) {
    return pocitadloPow(x, y, n, ZRETEZENE_ZLOMKY);
}

/**
 * Funkce počítá hodntotu exponenciální funkce s obecným základem. 
 * 
 * @param x Základ mocniny
 * @param y Exponent mocniny
 * @param n Počet iterací
 * @param zposobVypoctu Způsob kterým bude vypočtena hodnota ln(a)
 *        0 - Tayloruv polynom, 1 - Zřetězený zlomek
 * 
 * @return Hodnota exponenciální funkce
 */
double pocitadloPow(double x, double y, unsigned int n, int zposobVypoctu) {
    if(x < 0)
        return NAN;
    if(y == 0)
        return 1;
    if(x == 1)
        return 1;
    if(x < 1 && isinf(y) == -1) 
        return INFINITY;
    if(x < 1 && isinf(y))
        return 0;
    if(isnan(x) == 1 || isnan(y))
        return NAN;
    if(x == 0 && y < 0)
        return INFINITY;
    if(x == 0 && isinf(y) == -1)
        return INFINITY;
    if(x == 0)
        return 0;
    if(isinf(y) == 1 && y < 0)
        return 0;
    if(y < 0) {
        return 1 / pocitadloPow(x, -y, n, zposobVypoctu);
    }
    
    double vysledek = 1.0;
    double lna;
    // Na základě způsobu býpočtu umístíme do proměnné lnA hodnotu log(a)
    switch(zposobVypoctu) {
        case TAYLORUV_POLYNOM:
            lna = taylor_log(x, n);
            break;
        case ZRETEZENE_ZLOMKY:
            lna = cfrac_log(x, n);
    }
    
    double aktualniClen = 1;
    for(unsigned int i = 1; i <= n; i++) {
        
        aktualniClen *= lna;
        aktualniClen /= i;
        aktualniClen *= y;
        vysledek += aktualniClen;
    }
    return vysledek;
    
}

/**
 * Funkce vrací přirozený logaritmus z daného čísla na i významných číslic
 * 
 * @param x Základ logaritmu
 * @return Prizozený logaritmus o daném základu
 */
double mylog(double x) {
    // Extrémní a krizové stavy jsou ošetřeny podmínkou.
    if(x < 0)
        return NAN;
    if(x == 0)
        return -INFINITY;
    if(isinf(x))
        return INFINITY;
    
    // Počet jedniček, které je třeba k výsledku přičíst nebo naopak odečíst.
    int pricist = 0;
    int odecist = 0;
    
    /* Pomocí dvou while smyček upravíme vstupní hodnotu na hodnotu v intervalu
     * <(e^(1/2)/e, e^(1/2)> */
    while(x < DOLNI_MEZ) {
        x *= EULEROVO_CISLO;
        odecist++;
    }
    
    while (x > HORNI_MEZ) {
        x /= EULEROVO_CISLO;
        pricist++;
    }
    
    // Pomocí čtyř iterací lze v upraveném intervalu vždy dosáhnout požadované přesnosti
    int i = 4;
    // hodnota z je vyjádřena ze vzorce.
    double z = (x - 1) / (1 + x);
    
    double druhaMocnina = z*z;
    double cf = 0.0;
    
    while(i >= 1) {
        cf = (i*i*druhaMocnina)/(1+i*2 - cf);
        i = i - 1;
    }
    
    return  2*z/(1-cf) - odecist + pricist;
    
}

/**
 * Funkce vrací hodnotu mocniny o přirozeném základu a exponentu na 8 
 * významných číslic
 * 
 * @param x Základ mocniny
 * @param y Exponent mocniny
 * @return Mocnina o daném základu a exponentu.
 */
double mypow(double x, double y) {
    // Krizové stavy ošetřeny přímo
    if(x < 0)
        return NAN;
    if(y == 0)
        return 1;
    if(x == 1)
        return 1;
    if(x < 1 && isinf(y) == -1) 
        return INFINITY;
    if(x < 1 && isinf(y))
        return 0;
    if(isnan(x) == 1 || isnan(y))
        return NAN;
    if(x == 0 && y < 0)
        return INFINITY;
    if(x == 0 && isinf(y) == -1)
        return INFINITY;
    if(x == 0)
        return 0;
    if(isinf(y) == 1 && y < 0)
        return 0;
    if(y < 0) {
        return 1 / mypow(x, -y);
    }
            
    double vysledek = 1.0;
    double lnA;
    
    double presnost = 0.0000001;
    
    // Na základě způsobu býpočtu umístíme do proměnné lnA hodnotu log(a)
    lnA = mylog(x);
    
    double aktualniClen = 1;
    for(unsigned int i = 1; aktualniClen > presnost; i++) {
        aktualniClen *= y;
        aktualniClen /= i;
        aktualniClen *= lnA;
        vysledek += aktualniClen;
    }
    
    return vysledek;
}
