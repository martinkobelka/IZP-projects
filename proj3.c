/**
 * @file proj3.c
 * 
 * @date 30.11.2016
 * @author Martin Kobelka (xkobel02)
 * 
 * 
 * @brief Projekt č.3 do předmětu IZP, Jednoduchá shluková analýza
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // Kvůli funkci sqrt
#include <limits.h> // Kvůli INT_MAX

#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

/**
 * @defgroup ladeni Ladící makra
 * @{
 * 
 * @brief Makra sloužící k ladění aplikace
 * 
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */

/**
 * @brief Vypíše ladící řetězec
 * @param s Řetězec k vypsání
 */
#define debug(s) printf("- %s\n", s)

/**
 * @brief vypise formatovany ladici vystupc
 * @param s Formátovaný ladící řetězec
 * 
 * vypise formatovany ladici vystup - pouziti podobne jako printf
 */
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

/**
 * @brief vypise ladici informaci o promenne
 * @param i identifikátor proměnné
 * 
 * vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
 */
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

/**
 * @brief vypise ladici informaci o promenne typu float
 * @param f identifikátor proměnné
 * 
 * vypise ladici informaci o promenne typu float - pouziti
 * dfloat(identifikator_promenne)
 */
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

/**@}*/
#endif

// Deklarace chybových kodů
#define CHYBNE_ARGUMENTY 0 /**< @brief Chybné argumenty */
#define CHYBNY_SOUBOR 1 /**< @brief Soubor je chybný */
#define CHYBNY_SPECIFIKATOR 2 /**<@brief Specifikátor pro argument je chybný */

/**
 * @brief Deklarace chybových hlášek
 * 
 * Konstantní pole obsahující chybové hlášky. Výsledný argument je vypsán jen na
 * základě indexu definovaného v makrech výše.
 */
const char *chybove_hlasky[] = {
    "Chybné argumenty příkazové řádky",
    "Nepodařilo se načíst soubor s daty",
    "Chybný specifikátor pro daný argument"
};

/**
 * @defgroup struct Struktury
 * @brief Struktury určené pro uchování dat
 * 
 * struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
*/

/**
 * @ingroup struct
 * @brief Objekt ve shluku
 * 
 * Jeden objekt ve shluku. Je tvořen identifikátorem a souřadnicemi
 * v euklidovském prostoru.
 */
struct obj_t {
    /** @brief Identifikační číslo objektu */
    int id;
    /** @brief X souřadnice  */
    float x;
    /** @brief Y souřadnice */
    float y;
};

/**
 * @ingroup struct
 * @brief Shluk objektů 
 * 
 * Dynamické pole obsahující jeden nebo více shkluků. Známe údaj o jeho
 * velikosti, kapacitě a ukazatel na pole objektů.
 */
struct cluster_t {
    /** @brief Počet položek ve shluku */
    int size;
    /** @brief Maximální kapacita shluku */
    int capacity;
    /** @brief ukazatel na prní položku ve shluku */
    struct obj_t *obj;
};

/**
 * @defgroup vypis Výpis na výstup
 * @brief Funkce vypisující údaje
 * 
 * Funkce vypisující údaje na standartní nebo chyboý výstup výstup 
 */

/**
 * @ingroup vypis
 * @brief Vypíše chybu
 * 
 * Vypíše chybovou hlášku na standarní chybový výstup
 * 
 * @param chybovyKod Číselný kod označující chybu
 */
void vypis_chybu(int chybovyKod) {
    fprintf(stderr ,"%s\n", chybove_hlasky[chybovyKod]);
}

/**
 * @defgroup clusterF Operující s clustery
 * @brief Funkce operující s clustery
 * 
 * Funkce operující s clustery.
 * 
 */

/**
 * @ingroup clusterF
 * @brief Inicializace shluku
 * 
 * Inicializace shluku c s počáteční kapacitou cap
 * 
 * @param c Ukazatel na shluk, který chceme inicializovat
 * @param cap Počáteční kapacita shluku
 */
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->capacity = cap;
    c->size = 0;
    c->obj = malloc(sizeof(struct obj_t)*cap);

}

/**
 * @ingroup clusterF
 * @brief Odstranění objektů ve shluku
 * 
 * Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 * 
 * @param c Shluk, který chceme vyčistit
 */
void clear_cluster(struct cluster_t *c)
{
    c->capacity = 0;
    c->size = 0;
    free(c->obj);
}

/**
 * @brief Počet objektů rozšiřující daný shluk
 * 
 * Shluk bude v případě nutnosti rozšířen o právě tolik shluků, kolik obsahuje 
 * tato konstanta.
 */
const int CLUSTER_CHUNK = 10;

/**
 * @ingroup clusterF
 * @brief Změna velikosti clusteru
 * 
 * Změní velikost clusteru který přijimá jako parametr na novou kapacitu. Budou
 * aktualizovány všechny metadata uvnitř struktur.
 * 
 * @param c Cluster, který chceme rozšířit
 * @param new_cap Nová kapacita
 * 
 * @return Vrací ukazatel na reallocovaný cluster. Pokud se operace nezdařila, 
 * vrací NULL.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/**
 * @ingroup clusterF
 * Přidá do clusteru objekt. ROzšíří jeho kapacitu, pokud se do něj další prvek 
 * nevejde
 * 
 * @param c Cluster
 * @param obj Objekt
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if(c->capacity == c->size)
        c = resize_cluster(c, c->capacity + CLUSTER_CHUNK);
    c->obj[c->size] = obj;
    c->size++;
}

/**
 * @ingroup clusterF
 * @brief Seřadí objekty ve shluku
 * 
 * Seřadí objekty ve shluku implementací algoritmu podle ID implementací 
 * algoritmu quick sort
 * 
 * @param c Ukazatel na pole shluků určené k seřazení
 */
void sort_cluster(struct cluster_t *c);


/**
 * @ingroup clusterF
 * @brief Spojí shluky c1 a c2 do shluku c2
 * 
 * Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 * Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacnihuo cisla.
 * Shluk 'c2' bude nezmenen.
 * 
 * @param c1 Zhluk, do kterého bude kopírováno
 * @param c2 Zhluk, z kterého bude kopírováno
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    for(int i = 0; i < c2->size; i++) {
        append_cluster(c1, c2->obj[i]);
    }
    
    sort_cluster(c1);
}

/**
 * @ingroup clusterF
 * @brief Odstaní shluk z pole
 * 
 * Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 * (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 * pocet shluku v poli.
 * 
 * @param carr Pole shluků, které má být odstraněno
 * @param narr Počet položek v odstraňovaném shluku
 * @param idx index shluku, který se odstrení
 * @return Nový počet shluků v poli
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);
    clear_cluster(&carr[idx]);
    
    for(int i = idx; i < narr - 1; i++) {
        // TODO
        // Memory leak !!!! Dodělat 
        carr[i].capacity = carr[i + 1].capacity;
        carr[i].size = carr[i + 1].size;
        carr[i].obj = carr[i + 1].obj;
        // 
    }
    
    return narr - 1;
}

/**
 * Pocita Euklidovskou vzdalenost mezi dvema objekty.
 * 
 * @param o1 První objekt 
 * @param o2 Druhý objekt
 * @return Euklidovská vzdálenost
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    return sqrtf(pow(o1->x - o2->x, 2) + pow(o1->y - o2->y, 2)); 
}

/**
 * @brief Počítá vzdálenost dvou shluků
 * 
 * Počítá vzdálenost dvou shluků metodou nejvzdálenějšího souseda. Porovná každý
 * objekt s každým, a vrací nejdelší nalezenou vzdálenost
 * 
 * @param c1 První shluk
 * @param c2 Druhý shluk
 * @return Nejdelší nalezená vzdálenost
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float vzdalenost = 0;
    float nova_vzdalenost;
    for(int i = 0; i < c1->size; i++) {
        for(int r = 0; r < c2->size; r++) {
            nova_vzdalenost = obj_distance(&c1->obj[i], &c2->obj[r]);
            if(nova_vzdalenost > vzdalenost)
                vzdalenost = nova_vzdalenost;
        }
    }
    
    return vzdalenost;
}

/**
 * @ingroup clusterF
 * @brief Najde dva nejbližší shluky
 * 
 * Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 * hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 * 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 * adresu 'c1' resp. 'c2'.
 * 
 * @param carr Pole shluků
 * @param narr Velikost pole shluků
 * @param c1 Ukazatel na místo, kam bude uložen první nalezený shluk
 * @param c2 Ukazatel na místo, kam bude uložen druhý naleený shluk
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);
    float nejkratsi_vzdalenost = INT_MAX;
    float aktualniVzdalenost;
    int index_c1, index_c2;
    for(int i = 0; i < narr; i++) {
        for(int r = i + 1; r < narr; r++) {
            aktualniVzdalenost = cluster_distance(&carr[i], &carr[r]);
            if(aktualniVzdalenost < nejkratsi_vzdalenost){
                nejkratsi_vzdalenost = aktualniVzdalenost;
                index_c1 = i;
                index_c2 = r;
            }
        }
    }
    
    *c1 = index_c1;
    *c2 = index_c2;
}

/**
 * @brief Porovná dva shluky podle ID
 * 
 * Pomocná funkce pro řazení shluků. Porovnává dva shluky podle jejich ID
 * 
 * @param a První shluk
 * @param b Druhý shluk
 * @return -1 pokud ID prvního shluku menší, 1 pokud je ID prvního shluku větší.
 */
static int obj_sort_compar(const void *a, const void *b)
{
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

void sort_cluster(struct cluster_t *c)
{
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/**
 * @ingroup vypis
 * @brief Vytiskne shluk nía stdout
 * 
 * Funkce vytiskne jeden shluk na standartní výstup
 * 
 * @param c Shluk k vytisknutí
 */
void print_cluster(struct cluster_t *c)
{
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/**
 * Funkce načítá ze vstupního souboru clustery, a ukládá je do dynamického pole
 * 
 * @param filename Jméno vstupního souboru
 * @param arr Pole clusterů, do kterého načítáme clustery, v případě neúspěchu 
 *        nabude hodnoty NULL
 * @return Vrací počet objektů, které funkce načetla ze vstupního souboru
 */
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    FILE *soubor;
    char konec_radku;
    
    /* Pokoušíme se otevřít soubor. Pokud se tak nepovede, tak *arr = NULL, a 
     * vracíme -1 */
    if((soubor = fopen(filename, "r")) == NULL) {
        *arr = NULL;
        return -1;
    }
    
    /* Z prvního řádku přečteme počet objektů a vytvoříme dynamicky pole pro 
     * právě tolik objektů, kolik jich je ve vstupním souboru. */
    int pocet_objektu;
    
    // V případě že se nám nepodaří najít počet objektů, skončíme a vrátíme -1
    if(fscanf(soubor, "count=%d%c", &pocet_objektu, &konec_radku) != 2 && 
            konec_radku != '\n') {
        fclose(soubor);
        return -1;
    }    
    
    // Alokujeme pole pro právě takový poččet objektů, jaký je potřeba
    *arr = malloc(sizeof(struct cluster_t)*pocet_objektu);
    
    // Budeme opakovat tolikrát, kolik máme objektů v souboru.
    int i;
    for(i = 0; i < pocet_objektu; i++) {
        int id, x, y;
        
        /* V případě že narazíme na konec souboru, znamená to, že je zadán 
         * větší počet objektů než soubor obsahuje. Skončíme proto s chybou.*/
        int hodnota;
        if((hodnota = fscanf(soubor, "%d %d %d%c", &id, &x, &y, &konec_radku)) == EOF || 
                (hodnota != 3 && hodnota != 4)) {
            for(int s = 0; s < i; s++) {
                clear_cluster(&(*arr)[s]);
            }
            fclose(soubor);
            return -1;
        }
        
        if(i != pocet_objektu - 1 && konec_radku != '\n') {
            fclose(soubor);
            return -1;
        } 
        
        /* V případě nekorektních hodnot x nebo y ve vstupním souboru vrátíme -1
         * čímž vynutíme ukončení programu. */
        if(x < 0 || x > 1000 || y < 0 || y > 1000) {
            for(int s = 0; s < i; s++) {
                clear_cluster(&(*arr)[s]);
            }
            fclose(soubor);
            return -1;
        }
        
        // Zjišťujeme, jestli je ID jedinečné
        for(int r = 0; r < i; r++) {
            if((*arr)[r].obj->id == id) {
                for(int s = 0; s < i; s++) {
                clear_cluster(&(*arr)[s]);
            }
            fclose(soubor);
            return -1;
            }
        }
        
        struct cluster_t cluster;
        init_cluster(&cluster, 1);
        
        
        struct obj_t objekt;
        objekt.id = id;
        objekt.x = x;
        objekt.y = y;
        append_cluster(&cluster, objekt);
        
        (*arr)[i] = cluster;
    }
    
    // Uzavřeme soubor a vracíme počet objektů
    fclose(soubor);
    return pocet_objektu;

}

/**
 * @ingroup vypis
 * @brief Vytiskne pole shluků
 * 
 * Funkce slouží k vytisknutí pole shluků na standartní výstup
 * 
 * @param carr Ukazatel na první položku (shluk)
 * @param narr počet clusterů
 */
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

/**
 * Hlavní funkce, spustí se ihned po spuštění aplikace
 * 
 * @param argc Počet argumentů příkazové řádky
 * @param argv Pole argumentů příkazové řádky
 * 
 * @return Vrací 0 v případě úspěšného ukončení aplikace, 
 * nebo 1 v případě chyby.
 */
int main(int argc, char *argv[])
{
    // Pole slusterů
    struct cluster_t *clusters;
    
    
    int pocet_objektu = 0;
    long vysledny_pocet_clusteru = 1;
    
    // Podle počtu argumentů se rozhoduji, kterou akci vykonám
    switch(argc) {
        char *neni_cislo;
        // Program funguje pouze ze dvěma nebo třemi vstupními argumenty
        case 2:
            vysledny_pocet_clusteru = 1;
            break;
        case 3: 
            // Zjišťujeme počet clusterů z argumentu
            vysledny_pocet_clusteru = strtol(argv[2], &neni_cislo, 10);
            if(*neni_cislo != 0) {
                vypis_chybu(CHYBNY_SPECIFIKATOR);
                return (EXIT_FAILURE);
            }
            if(vysledny_pocet_clusteru <= 0) {
                vypis_chybu(CHYBNY_SPECIFIKATOR);
                return (EXIT_FAILURE);
            }
            
            break;
        default:
            /* Pokud byl zadán špatný počet argumentů, tak ukončí program s 
             * chybou */
            vypis_chybu(CHYBNE_ARGUMENTY);
            return (EXIT_FAILURE);
            break;
    }
    
    // Načte clustery ze vstupního souboru
    pocet_objektu = load_clusters(argv[1], &clusters);
    if(pocet_objektu == -1){
        vypis_chybu(CHYBNY_SOUBOR);
        free(clusters);
        return (EXIT_FAILURE);
    }
    
    if(pocet_objektu < vysledny_pocet_clusteru) {
        vypis_chybu(CHYBNY_SPECIFIKATOR);
        // Vyčistí všechny clustery před ukončením aplikace
        for(int i = 0; i < pocet_objektu; i++) {
            clear_cluster(&clusters[i]);
        }

        // Vyčistí i samotné pole
        free(clusters);
        return (EXIT_FAILURE);
    }
            
    /* V případě, kdy se nepovedlo načíst vstupní soubor, 
     * vypíše chybu a ukončí aplikaci */
    if(clusters == NULL) {
        vypis_chybu(CHYBNY_SOUBOR);
        return (EXIT_FAILURE);
    }
    
    // Zde probíhá výpočet samotného algoritmu amen modlím se aby to fungovalo
    
    int hodnota = pocet_objektu;
    
    for(long i = hodnota; i > vysledny_pocet_clusteru; i--) {
        int i1, i2;
        find_neighbours(clusters, pocet_objektu, &i1, &i2);
        merge_clusters(&clusters[i1], &clusters[i2]);
        pocet_objektu = remove_cluster(clusters, pocet_objektu, i2);
    }
      
    print_clusters(clusters, pocet_objektu);
    
    // Vyčistí všechny clustery před ukončením aplikace
    for(int i = 0; i < pocet_objektu; i++) {
        clear_cluster(&clusters[i]);
    }
    
    // Vyčistí i samotné pole
    free(clusters);
    
    // Ukončí aplikaci úspěšně
    return (EXIT_SUCCESS);
}

