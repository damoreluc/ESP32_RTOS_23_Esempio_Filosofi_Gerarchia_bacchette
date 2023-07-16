# ESP32_RTOS_23_Esempio_Filosofi_Gerarchia_bacchette

## FreeRTOS Esercizio 23: cinque filosofi, con gerarchia delle bacchette.

Una possibile soluzione del problema dei cinque filosofi mediante bacchette con gerarchia (priorità) crescente.

I filosofi sono denominati $F_1$, $F_2$, $F_3$, $F_4$ e $F_5$, mentre le bacchette alla loro sinistra sono rispettivamente
$b_1$, $b_2$, $b_3$, $b_4$ e $b_5$.

Il filosofo $F_1$ dovrà prendere la prima bacchetta $b_1$ prima di poter prendere la seconda bacchetta $b_2$.

I filosofi $F_2$, $F_3$ e $F_4$ si comporteranno in modo analogo, prendendo sempre la bacchetta $b_i$ prima della bacchetta $b_{i+1}$.

Rispettando l'ordine numerico ma invertendo l'ordine delle mani, il filosofo $F_5$ prenderà prima la bacchetta $b_1$ e poi la bacchetta $b_5$. 
Si crea così un'asimmetria che serve ad evitare i deadlock: se tutti i filosofi $F_2$, $F_3$ e $F_4$ hanno già preso la bacchetta $b_i$, 
il filosofo $F_5$ non può procedere con $b_1$ perché è già presa dal filosofo $F_1$; invece può procedere il filosofo $F_4$, che dopo aver preso
la bacchetta $b_4$ può prendere la $b_5$.
