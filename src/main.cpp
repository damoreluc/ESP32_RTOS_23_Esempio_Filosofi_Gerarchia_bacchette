/*
 * FreeRTOS Esercizio 23: cinque filosofi, con gerarchia delle bacchette, N mangiate del filosofo.
 * 
 * Modificare a piacere il numero di mangiate assengnado un differente valore al parametro NUM_CICLI
 *
 * Possibile soluzione del problema dei cinque filosofi mediante bacchette con gerarchia (priorità) crescente.
 *
 * I filosofi sono denominati F1, F2, F3, F4 e F5, mentre le bacchette alla loro sinistra sono
 * rispettivamente b1, b2, b3, b4 e b5.
 * Il filosofo F1 dovrà prendere la prima bacchetta b1 prima di poter prendere la seconda bacchetta b2.
 * I filosofi F2, F3 e F4 si comporteranno in modo analogo, prendendo sempre la bacchetta bi prima
 * della bacchetta bi+1.
 *
 * Rispettando l'ordine numerico ma invertendo l'ordine delle mani, il filosofo F5 prenderà prima
 * la bacchetta b1 e poi la bacchetta b5. Si crea così un'asimmetria che serve ad evitare i deadlock:
 * se tutti i filosofi F1, F2, F3, F4 hanno già preso la bacchetta bi, il filosofo F5 non può procedere
 * con b1 perché è già presa dal filosofo F1; invece può procedere il filosofo F4, che dopo aver preso
 * la bacchetta b4 può prendere la b5.
 *
 *
 * Nota: nel file soc.h sono definiti i riferimenti ai due core della ESP32:
 *   #define PRO_CPU_NUM (0)
 *   #define APP_CPU_NUM (1)
 *
 * Qui viene adoperata la APP_CPU
 *
 */

#include <Arduino.h>

// Impostazioni **********************************************************************
// Numero di tasks (filosofi)
const uint16_t NUM_TASKS = 5;
// Dimensione in byte dello stack del task
const uint16_t TASK_STACK_SIZE = 2048;
// numero di cicli del filosofo
const uint16_t NUM_CICLI = 10;

// Variabili Globali *****************************************************************
// attesa sulla lettura del parametro da parte dei cinque task
static SemaphoreHandle_t bin_sem;
// Notifica verso il main che tutti i task sono terminati
static SemaphoreHandle_t done_sem;
// Le bacchette: il loro ordine corrisponde al loro indice nell'array
static SemaphoreHandle_t chopstick[NUM_TASKS];

//************************************************************************************
// Tasks

// Il filosofo: Un unico task da istanziare cinque volte;
// il task accetta un parametro in ingresso, il proprio numero d'ordine al tavolo
void filosofo(void *parameters)
{
  // numero di cicli compiuti dal filosofo
  uint16_t cicli = 0;

  // numero d'ordine del filosofo
  int num_ord;

  char buf[100];
  // indice della prima bacchetta
  int idx_1;
  // indice della seconda bacchetta
  int idx_2;

  // Copia localmente il valore del parametro e incrementa il semaforo a conteggio
  num_ord = *(int *)parameters;
  xSemaphoreGive(bin_sem);

  // Calcola l'indice della prima e della seconda bacchetta per ciascun filosofo
  // per tutti i filosofi tranne l'ultimo, la prima bacchetta corrisponde al
  // numero d'ordine del filosofo; la seconda bacchetta è quella successiva
  if (num_ord < (num_ord + 1) % NUM_TASKS)
  {
    idx_1 = num_ord;
    idx_2 = (num_ord + 1) % NUM_TASKS;
  }
  else
  {
    // per l'ultimo filosofo, l'algoritmo deve invertire l'ordine delle bacchette
    //  quindi se num_ord = 4 dovrà essere
    //   idx_1 = 0
    //   idx_2 = 4
    idx_1 = (num_ord + 1) % NUM_TASKS;
    idx_2 = num_ord;
  }

  // il filosofo mangia e pensa un certo numero di volte
  for(cicli = 0; cicli < NUM_CICLI; cicli++)
  {
    // Prende la prima bacchetta
    xSemaphoreTake(chopstick[idx_1], portMAX_DELAY);
    sprintf(buf, "Il filosofo %i prende la bacchetta %i", num_ord, idx_1);
    Serial.println(buf);

    // Introduce un po' di ritardo per dare luogo al deadlock
    vTaskDelay(pdMS_TO_TICKS(3));

    // Prende la seconda bacchetta
    xSemaphoreTake(chopstick[idx_2], portMAX_DELAY);
    sprintf(buf, "Il filosofo %i prende la bacchetta %i", num_ord, idx_2);
    Serial.println(buf);

    // Il filosofo mangia
    sprintf(buf, "Il filosofo %i sta mangiando, ciclo %i", num_ord, cicli);
    Serial.println(buf);
    vTaskDelay(pdMS_TO_TICKS(100));

    // Ripone la seconda bacchetta
    xSemaphoreGive(chopstick[idx_2]);
    sprintf(buf, "Il filosofo %i ripone la bacchetta %i", num_ord, idx_2);
    Serial.println(buf);

    // Ripone la prima bacchetta
    xSemaphoreGive(chopstick[idx_1]);
    sprintf(buf, "Il filosofo %i ripone la bacchetta %i", num_ord, idx_1);
    Serial.println(buf);

    // Il filosofo pensa
    sprintf(buf, "Il filosofo %i pensa", num_ord);
    Serial.println(buf);
    vTaskDelay(pdMS_TO_TICKS(200));
  }

  // Notifica il main che questo task è terminato e rimuove se stesso
  xSemaphoreGive(done_sem);
  vTaskDelete(NULL);

}

//************************************************************************************
// Main (sul core 1, con priorità 1)

// configurazione del sistema
void setup()
{
  // buffer per creare il nome di ciascun task (filosofo)
  char task_name[20];

  // Configurazione della seriale
  Serial.begin(115200);

  // breve pausa
  vTaskDelay(pdMS_TO_TICKS(1000));
  Serial.println();
  Serial.println("FreeRTOS deadlock: Problema dei cinque filosofi, gerarchia delle bacchette");

  // crea il semaforo per il passaggio del numero d'ordine tra main e task
  bin_sem = xSemaphoreCreateBinary();

  // crea il semaforo a conteggio per gestire la conclusione dei cinque task
  done_sem = xSemaphoreCreateCounting(NUM_TASKS, 0);

  // crea l'array delle bacchette
  for (int i = 0; i < NUM_TASKS; i++)
  {
    chopstick[i] = xSemaphoreCreateMutex();
  }

  // Crea e avvia i cinque task filosofi, ciascuno col suo numero d'ordine
  for (int i = 0; i < NUM_TASKS; i++)
  {
    sprintf(task_name, "Filosofo %i", i);
    xTaskCreatePinnedToCore(filosofo,
                            task_name,
                            TASK_STACK_SIZE,
                            (void *)&i,
                            1,
                            NULL,
                            APP_CPU_NUM);
    // attendi che il task abbia acquisito il proprio numero d'ordine
    xSemaphoreTake(bin_sem, portMAX_DELAY);
  }

  // Attende la conclusione dei cinque task
  for (int i = 0; i < NUM_TASKS; i++)
  {
    xSemaphoreTake(done_sem, portMAX_DELAY);
  }

  // Messaggio di uscita, compito concluso senza deadlock
  Serial.println("Terminato senza deadlock!");
}

void loop()
{
  // lasciare vuoto
}
