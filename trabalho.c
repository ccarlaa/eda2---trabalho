#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 1000000000
#define MAX_HEAP 10000

// ------------------ Estruturas do Grafo ------------------

struct node {
    int vertex;
    int weight;
    struct node *next;
};

typedef struct node *link;

struct graph {
    int vertices;
    int edges;
    link *adj;
    int *is_restaurant;
    int *is_gasoline;
};

typedef struct graph *Graph;

static link newNode(int vertex, int weight) {
    link new = malloc(sizeof(struct node));
    if (new == NULL) {
        perror("Erro ao alocar novo nó");
        exit(EXIT_FAILURE);
    }
    new->vertex = vertex;
    new->weight = weight;
    new->next = NULL;
    return new;
}

static Graph newGraph(int vertices) {
    Graph g = malloc(sizeof(struct graph));
    if (g == NULL) {
        perror("Erro ao alocar grafo");
        exit(EXIT_FAILURE);
    }
    g->vertices = vertices;
    g->edges = 0;
    g->adj = malloc((vertices + 1) * sizeof(link)); 
    if (g->adj == NULL) {
        perror("Erro ao alocar lista de adj");
        free(g);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i <= vertices; i++) {
        g->adj[i] = NULL;
    }

    g->is_restaurant = calloc((vertices + 1), sizeof(int));
    g->is_gasoline = calloc((vertices + 1), sizeof(int));
    if (g->is_restaurant == NULL || g->is_gasoline == NULL) {
        perror("Erro ao alocar g ou r");
        free(g->adj);
        free(g->is_restaurant);
        free(g->is_gasoline);
        free(g);
        exit(EXIT_FAILURE);
    }
    printf("[DEBUG] Grafo criado com %d vértices\n", vertices);
    return g;
}

static void addEdge(Graph g, int src, int dest, int weight) {
    link new = newNode(dest, weight);
    new->next = g->adj[src];
    g->adj[src] = new;
    g->edges++;
    printf("[DEBUG] Aresta add: %d -> %d (peso %d)\n", src, dest, weight);
}

// ------------------ Heap p/ Dijkstra ------------------

typedef struct {
    int vertex;
    int distance;
} HeapNode;

typedef struct {
    HeapNode data[MAX_HEAP];
    int size;
} MinHeap;

void swap(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(MinHeap* h, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (h->data[parent].distance > h->data[idx].distance) {
            swap(&h->data[parent], &h->data[idx]);
            idx = parent;
        } else break;
    }
}

void heapify_down(MinHeap* h, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < h->size && h->data[left].distance < h->data[smallest].distance)
        smallest = left;
    if (right < h->size && h->data[right].distance < h->data[smallest].distance)
        smallest = right;

    if (smallest != idx) {
        swap(&h->data[idx], &h->data[smallest]);
        heapify_down(h, smallest);
    }
}

void insert_heap(MinHeap* h, int vertex, int distance) {
    if (h->size >= MAX_HEAP) {
        fprintf(stderr, "[DEBUG] Erro: Heap cheia. Não é possível inserir mais elementos\n");
        return;
    }
    h->data[h->size].vertex = vertex;
    h->data[h->size].distance = distance;
    heapify_up(h, h->size);
    h->size++;
    printf("[DEBUG] Heap: Inserido vértice %d, distância %d Tamanho: %d\n", vertex, distance, h->size);
}

HeapNode extract_min(MinHeap* h) {
    if (h->size == 0) {
        fprintf(stderr, "[DEBUG] Erro: Heap vazia\n");
        return (HeapNode){-1, INF};
    }
    HeapNode min = h->data[0];
    h->data[0] = h->data[h->size - 1];
    h->size--;
    heapify_down(h, 0);
    printf("[DEBUG] Heap: Extraído vértice %d, distância %d Novo tamanho: %d\n", min.vertex, min.distance, h->size);
    return min;
}

void dijkstra_heap(Graph g, int start, int* dist) {
    int V = g->vertices;
    int visited[V + 1];
    printf("[DEBUG] Dijkstra: Iniciando de %d\n", start);
    for (int i = 0; i <= V; i++) {
        dist[i] = INF;
        visited[i] = 0;
    }

    dist[start] = 0;

    MinHeap heap;
    heap.size = 0;
    insert_heap(&heap, start, 0);

    while (heap.size > 0) {
        HeapNode min_node = extract_min(&heap);
        int u = min_node.vertex;
        int d_u = min_node.distance;

        if (u == -1) break;

        if (visited[u]) {
            printf("[DEBUG] Dijkstra: Vértice %d já visitado\n", u);
            continue;
        }
        visited[u] = 1;
        printf("[DEBUG] Dijkstra: Visitando vértice %d (distância atual %d)\n", u, d_u);

        link p = g->adj[u];
        while (p != NULL) {
            int v = p->vertex;
            int w = p->weight;

            if (!visited[v] && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                insert_heap(&heap, v, dist[v]);
                printf("[DEBUG] Dijkstra: Distância p/ %d atualizada p/ %d (via %d)\n", v, dist[v], u);
            }
            p = p->next;
        }
    }
    printf("[DEBUG] Dijkstra: Concluído %d\n", start);
}

// ------------------ Entregador ------------------

typedef struct {
    int posicao_atual;
    int combustivel;
    int mochila_capacidade;
    int mochila_tamanho;
    int pedidos[100]; // Tô supondo que são 100 pedidos
} State;

void pegarPedido(State *estado, int destino) {
    if (estado->mochila_tamanho < estado->mochila_capacidade) {
        estado->pedidos[estado->mochila_tamanho++] = destino;
        printf("[DEBUG] Pegou pedido para %d. Mochila agora tem %d pedidos\n", destino, estado->mochila_tamanho);
    } else {
        printf("[DEBUG] Mochila cheia. Não pegou pedido para %d\n", destino);
    }
}

int entregarPedido(State *estado, int destino) {
    for (int i = 0; i < estado->mochila_tamanho; i++) {
        if (estado->pedidos[i] == destino) {
            for (int j = i; j < estado->mochila_tamanho - 1; j++)
                estado->pedidos[j] = estado->pedidos[j + 1];
            estado->mochila_tamanho--;
            printf("[DEBUG] Entregou pedido para %d Mochila agora tem %d pedidos\n", destino, estado->mochila_tamanho);
            return 1;
        }
    }
    printf("[DEBUG] Tentou entregar para %d, mas não tinha na mochila\n", destino);
    return 0;
}

int temPedidoPara(State *estado, int destino) {
    for (int i = 0; i < estado->mochila_tamanho; i++) {
        if (estado->pedidos[i] == destino) {
            printf("[DEBUG] Tem pedido para %d na mochila\n", destino);
            return 1;
        }
    }
    printf("[DEBUG] Não tem pedido para %d na mochila\n", destino);
    return 0;
}

// ------------------ Liberando pedidos ------------------
void freeGraph(Graph g) {
    if (g == NULL) return;
    printf("[DEBUG] Liberando memória do grafo\n");
    for (int i = 0; i <= g->vertices; i++) {
        link current = g->adj[i];
        while (current != NULL) {
            link next = current->next;
            free(current);
            current = next;
        }
    }
    free(g->adj);
    free(g->is_restaurant);
    free(g->is_gasoline);
    free(g);
    printf("[DEBUG] Memória do grafo liberada\n");
}

int main() {
    int N, M, H, T, I, C, P;
    scanf("%d %d %d %d %d %d %d", &N, &M, &H, &T, &I, &C, &P);
    printf("[DEBUG] Parâmetros lidos: N=%d, M=%d, H=%d, T=%d, I=%d, C=%d, P=%d\n", N, M, H, T, I, C, P);

    Graph g = newGraph(N);

    // Marca os postos de gasolina no array
    for (int i = 0; i < P; i++) {
        int posto_id;
        scanf("%d", &posto_id);
        if (posto_id >= 1 && posto_id <= N) {
            g->is_gasoline[posto_id] = 1;
            printf("[DEBUG] Vértice %d marcado como Posto de Gasolina\n", posto_id);
        }
    }

    for (int i = 0; i < M; i++) {
        int u, v, w;
        scanf("%d %d %d", &u, &v, &w);
        addEdge(g, u, v, w);
    }

    int Q;
    scanf("%d", &Q);
    printf("[DEBUG] Lendo %d restaurantes\n", Q);

    int **restaurant_orders = malloc((N + 1) * sizeof(int*));
    if (restaurant_orders == NULL) {
        perror("Erro ao alocar restaurant_orders");
        freeGraph(g);
        exit(EXIT_FAILURE);
    }
    int *num_orders_per_restaurant = calloc((N + 1), sizeof(int));
    if (num_orders_per_restaurant == NULL) {
        perror("Erro ao alocar num_orders_per_restaurant");
        free(restaurant_orders);
        freeGraph(g);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < Q; i++) {
        int r, k;
        scanf("%d %d", &r, &k);
        if (r >= 1 && r <= N) {
            g->is_restaurant[r] = 1;
            num_orders_per_restaurant[r] = k;
            printf("[DEBUG] Vértice %d marcado como Restaurante com %d pedidos\n", r, k);
            restaurant_orders[r] = malloc(k * sizeof(int));
            if (restaurant_orders[r] == NULL) {
                perror("Erro ao alocar pedidos para restaurante");
                for(int j = 0; j <= N; j++) {
                    if (restaurant_orders[j] != NULL) free(restaurant_orders[j]);
                }
                free(restaurant_orders);
                free(num_orders_per_restaurant);
                freeGraph(g);
                exit(EXIT_FAILURE);
            }
            printf("[DEBUG] Pedidos para o restaurante %d: ", r);
            for (int j = 0; j < k; j++) {
                scanf("%d", &restaurant_orders[r][j]);
                printf("%d ", restaurant_orders[r][j]);
            }
            printf("\n");
        }
    }

    State estado = { .posicao_atual = H, .combustivel = I, .mochila_capacidade = C, .mochila_tamanho = 0 };
    printf("[DEBUG] Estado inicial do entregador: Pos=%d, Comb=%d, CapMochila=%d, TamMochila=%d\n",
        estado.posicao_atual, estado.combustivel, estado.mochila_capacidade, estado.mochila_tamanho);

    while (1) {
        printf("\n Novo ciclo de decisão (Pos: %d, Comb: %d, Mochila: %d) ---\n",
            estado.posicao_atual, estado.combustivel, estado.mochila_tamanho);
        int *distancias = malloc((N + 1) * sizeof(int));
        if (distancias == NULL) {
            perror("Erro ao alocar distancias");
            break;
        }
        dijkstra_heap(g, estado.posicao_atual, distancias);
        printf("[DEBUG] Distâncias a partir de %d: ", estado.posicao_atual);
        for(int i = 1; i <= N; i++) {
            if (distancias[i] == INF) printf("%d:INF ", i);
            else printf("%d:%d ", i, distancias[i]);
        }
        printf("\n");


        // 1. Entregar pedido (Prioridade 1)
        if (temPedidoPara(&estado, estado.posicao_atual)) {
            printf("[DEBUG] PRIORIDADE 1: Entregar pedido em %d\n", estado.posicao_atual);
            printf("e %d\n", estado.posicao_atual);
            fflush(stdout);
            char resp[10]; scanf("%s", resp);
            entregarPedido(&estado, estado.posicao_atual);
            free(distancias);
            continue;
        }

        // 2. Pegar pedidos (Prioridade 2)
        if (g->is_restaurant[estado.posicao_atual] && estado.mochila_tamanho < estado.mochila_capacidade && num_orders_per_restaurant[estado.posicao_atual] > 0) {
            printf("[DEBUG] PRIORIDADE 2: Pegar pedidos em Restaurante %d\n", estado.posicao_atual);
            int num = num_orders_per_restaurant[estado.posicao_atual];
            for (int i = 0; i < num && estado.mochila_tamanho < estado.mochila_capacidade; i++) {
                printf("p %d\n", restaurant_orders[estado.posicao_atual][i]);
                fflush(stdout);
                char resp[10]; scanf("%s", resp);
                pegarPedido(&estado, restaurant_orders[estado.posicao_atual][i]);
                restaurant_orders[estado.posicao_atual][i] = -1;
            }
            num_orders_per_restaurant[estado.posicao_atual] = 0; // Pega todos os possíveis
            free(distancias);
            continue;
        }

        // 3. Abastecer (Prioridade 3)
        if (g->is_gasoline[estado.posicao_atual] && estado.combustivel < T) {
            printf("[DEBUG] PRIORIDADE 3: Abastecer em Posto %d\n", estado.posicao_atual);
            printf("a\n");
            fflush(stdout);
            char resp[10]; scanf("%s", resp);
            estado.combustivel = T;
            printf("[DEBUG] Combustível abastecido para %d\n", estado.combustivel);
            free(distancias);
            continue;
        }

        // 4. Escolher próximo destino (Prioridade 4: Mover)
        int alvo = -1, menor_distancia_alvo = INF;
        printf("[DEBUG] PRIORIDADE 4: Escolhendo próximo alvo para mover\n");
        for (int i = 0; i < estado.mochila_tamanho; i++) {
            int dest = estado.pedidos[i];
            if (dest != -1 && distancias[dest] != INF && distancias[dest] <= estado.combustivel && distancias[dest] < menor_distancia_alvo) {
                alvo = dest;
                menor_distancia_alvo = distancias[dest];
                printf("[DEBUG] Alvo potencial (entrega): %d (dist %d)\n", alvo, menor_distancia_alvo);
            }
        }
        if (alvo == -1) {
            printf("[DEBUG] Nenhum pedido para entregar Buscando restaurante\n");
            for (int i = 1; i <= N; i++) {
                if (g->is_restaurant[i] && num_orders_per_restaurant[i] > 0 &&
                    estado.mochila_tamanho < estado.mochila_capacidade &&
                    distancias[i] != INF && distancias[i] <= estado.combustivel &&
                    distancias[i] < menor_distancia_alvo) {
                    alvo = i;
                    menor_distancia_alvo = distancias[i];
                    printf("[DEBUG] Alvo potencial (pegar pedido): %d (dist %d)\n", alvo, menor_distancia_alvo);
                }
            }
        }
        if (alvo == -1) {
            printf("[DEBUG] Nenhum alvo de entrega ou pegar pedido, verificando postos e casa\n");
            int all_orders_collected = 1;
            for(int i = 1; i <= N; i++) {
                if (g->is_restaurant[i] && num_orders_per_restaurant[i] > 0) {
                    all_orders_collected = 0;
                    break;
                }
            }

            if (estado.posicao_atual == H && estado.mochila_tamanho == 0 && all_orders_collected) {
                printf("[DEBUG] Dia finalizadooo\n");
                printf("x\n");
                fflush(stdout);
                char resp[10]; scanf("%s", resp);
                free(distancias);
                break;
            }

            // Vai pro posto mais próximo se precisar
            int posto_mais_proximo = -1;
            int dist_posto = INF;
            if (estado.combustivel < T) {
                for (int i = 1; i <= N; i++) {
                    if (g->is_gasoline[i] && distancias[i] != INF && distancias[i] <= estado.combustivel && distancias[i] < dist_posto) {
                        posto_mais_proximo = i;
                        dist_posto = distancias[i];
                    }
                }
            }
            if (posto_mais_proximo != -1) {
                alvo = posto_mais_proximo;
                menor_distancia_alvo = dist_posto;
                printf("[DEBUG] posto (posto de gasolina): %d (dist %d)\n", alvo, menor_distancia_alvo);
            } else {
                if (estado.posicao_atual != H && distancias[H] != INF && distancias[H] <= estado.combustivel) {
                    alvo = H;
                    menor_distancia_alvo = distancias[H];
                    printf("[DEBUG] casa (voltar para casa): %d (dist %d)\n", alvo, menor_distancia_alvo);
                } else {
                    printf("[DEBUG] Não tem mais nada, dia finalizadoo\n");
                    printf("x\n");
                    fflush(stdout);
                    char resp[10]; scanf("%s", resp);
                    free(distancias);
                    break;
                }
            }
        }
        
        printf("[DEBUG] next %d (distância %d)\n", alvo, menor_distancia_alvo);

        int prox = -1;
        int custo_passo = INF;

        link p = g->adj[estado.posicao_atual];
        while (p != NULL) {
            int v_vizinho = p->vertex;
            int peso_aresta = p->weight;

            // Vê qual o caminho mais curto do vizinho e se tem gasolina 
            if (distancias[alvo] == distancias[v_vizinho] + peso_aresta && peso_aresta <= estado.combustivel) {
                if (prox == -1 || peso_aresta < custo_passo) {
                    prox = v_vizinho;
                    custo_passo = peso_aresta;
                    printf("[DEBUG] Candidato a próximo passo: %d (custo %d) Distancia destino: %d, Distancia viz: %d\n", prox, custo_passo, distancias[alvo], distancias[v_vizinho]);
                }
            }
            p = p->next;
        }
        
        if (prox != -1) {
            printf("[DEBUG] MOVENDO: Próximo passo para %d com custo %d\n", prox, custo_passo);
            printf("m %d\n", prox);
            fflush(stdout);
            char resp[10]; scanf("%s", resp);
            estado.posicao_atual = prox;
            estado.combustivel -= custo_passo;
            printf("[DEBUG] Nova posição: %d, Combustível restante: %d.\n", estado.posicao_atual, estado.combustivel);
        } else {
            printf("[DEBUG] Não foi possível encontrar um próximo passo válido para o destino %d\n", alvo);
            printf("x\n");
            fflush(stdout);
            char resp[10]; scanf("%s", resp);
            free(distancias);
            break;
        }

        free(distancias);
    }

    // Finalização
    printf("[DEBUG] Liberação da memória\n");
    for (int i = 0; i <= N; i++) {
        if (restaurant_orders[i]) free(restaurant_orders[i]);
    }
    free(restaurant_orders);
    free(num_orders_per_restaurant);
    freeGraph(g);
    printf("[DEBUG] Fim do programa\n");

    return 0;
}