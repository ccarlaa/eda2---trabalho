#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 1000000000
#define MAX_HEAP 10000
#define MAX_V 10005

// ---------------- Estruturas ----------------

typedef struct node {
    int vertex;
    int weight;
    int is_restaurant;
    int is_gasoline;
    int capacidade;
    struct node *next;
} *link;

typedef struct {
    int vertices;
    int edges;
    link *adj;
} *Graph;

typedef struct {
    int posicao_atual;
    int combustivel;
    int mochila_capacidade;
    int mochila_tamanho;
    int pedidos[100];
} Estado;

// ---------------- MinHeap p/ Dijkstra ----------------

typedef struct {
    int vertex;
    int distance;
} HeapNode;

typedef struct {
    HeapNode data[MAX_HEAP];
    int size;
} MinHeap;

void swap(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a; *a = *b; *b = temp;
}

void heapify_up(MinHeap* h, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (h->data[parent].distance > h->data[index].distance) {
            swap(&h->data[parent], &h->data[index]);
            index = parent;
        } else break;
    }
}

void heapify_down(MinHeap* h, int index) {
    int smallest = index;
    int left = 2*index+1, right = 2*index+2;
    if (left < h->size && h->data[left].distance < h->data[smallest].distance) smallest = left;
    if (right < h->size && h->data[right].distance < h->data[smallest].distance) smallest = right;
    if (smallest != index) {
        swap(&h->data[index], &h->data[smallest]);
        heapify_down(h, smallest);
    }
}

void insert_heap(MinHeap* h, int vertex, int distance) {
    h->data[h->size].vertex = vertex;
    h->data[h->size].distance = distance;
    heapify_up(h, h->size);
    h->size++;
    printf("[DEBUG] Inserido na heap do Dijkstra: vertice=%d dist=%d tamanho=%d\n", vertex, distance, h->size);
}

HeapNode extract_min(MinHeap* h) {
    if (h->size == 0) return (HeapNode){-1, INF};
    HeapNode min = h->data[0];
    h->data[0] = h->data[h->size-1];
    h->size--;
    heapify_down(h, 0);
    printf("[DEBUG] Extraído da heap do Dijkstra: vertice=%d dist=%d\n", min.vertex, min.distance);
    return min;
}

// ---------------- MinHeap p/ Pedidos ----------------

typedef struct {
    int destino;
    int distancia;
} PedidoNode;

typedef struct {
    PedidoNode data[MAX_HEAP];
    int size;
} PedidoHeap;

void swapPedido(PedidoNode* a, PedidoNode* b) {
    PedidoNode t = *a;
    *a = *b;
    *b = t;
}

void heapify_up_pedido(PedidoHeap* h, int index) {
    while (index > 0) {
        int parent = (index-1)/2;
        if (h->data[parent].distancia > h->data[index].distancia) {
            swapPedido(&h->data[parent], &h->data[index]);
            index = parent;
        } else break;
    }
}

void heapify_down_pedido(PedidoHeap* h, int index) {
    int smallest = index;
    int left = 2*index+1, right = 2*index+2;
    if (left < h->size && h->data[left].distancia < h->data[smallest].distancia) smallest = left;
    if (right < h->size && h->data[right].distancia < h->data[smallest].distancia) smallest = right;
    if (smallest != index) {
        swapPedido(&h->data[index], &h->data[smallest]);
        heapify_down_pedido(h, smallest);
    }
}

void insertPedidoHeap(PedidoHeap* h, int destino, int distancia) {
    h->data[h->size].destino = destino;
    h->data[h->size].distancia = distancia;
    heapify_up_pedido(h, h->size);
    h->size++;
    printf("[DEBUG] Inserido na Heap de Pedidos: destino=%d dist=%d tamanho=%d\n", destino, distancia, h->size);
}

PedidoNode extractPedidoMin(PedidoHeap* h) {
    if (h->size == 0) return (PedidoNode){-1, INF};
    PedidoNode min = h->data[0];
    h->data[0] = h->data[h->size-1];
    h->size--;
    heapify_down_pedido(h, 0);
    printf("[DEBUG] Extraído da Heap de Pedidos: destino=%d dist=%d\n", min.destino, min.distancia);
    return min;
}

// ---------------- Grafo ----------------

static link newNode(int v, int w, int is_restaurant, int is_gasoline) {
    link n = malloc(sizeof(struct node));
    n->vertex = v; n->weight = w; 
    n->is_restaurant = is_restaurant;
    n->is_gasoline = is_gasoline;
    n->capacidade = -1; // -1 indica que a capacidade ainda não foi definida
    n->next = NULL;
    return n;
}

Graph newGraph(int V) {
    Graph g = malloc(sizeof(*g));
    g->vertices = V; g->edges = 0;
    g->adj = malloc((V + 1) * sizeof(link));
    for(int i = 0; i <= V; i++) {
        g->adj[i] = NULL;
    }
    printf("[DEBUG] Grafo criado com %d vertices\n", V);
    return g;
}

void addEdge(Graph g, int u, int v, int w, int *is_restaurant, int *is_gasoline){
    link n = newNode(v, w, is_restaurant[v], is_gasoline[v]);
    n->next = g->adj[u];
    g->adj[u] = n;
    printf("[DEBUG] Aresta adicionada: %d -> %d (peso=%d)\n", u, v, w);
}

// ---------------- Dijkstra com caminho ----------------

int dijkstra(Graph g, int start, int target, int *dist, int *parent){
    int V = g->vertices;
    for(int i = 0; i <= V; i++){
        dist[i] = INF;
        parent[i]=-1;
    }

    dist[start] = 0;
    MinHeap h; 
    h.size = 0;
    insert_heap(&h, start, 0);
    int visited[V + 1];
    memset(visited, 0, sizeof(visited));
    
    if(target == -1) printf("[DEBUG] Executando Dijkstra a partir de %d procurando restaurante\n", start);
    else if(target == -2) printf("[DEBUG] Executando Dijkstra a partir de %d procurando posto\n", start);
    else if(target == -3) printf("[DEBUG] Executando Dijkstra a partir de %d para todos os vertices\n", start);
    else printf("[DEBUG] Executando Dijkstra a partir de %d para %d\n", start, target);

    while(h.size > 0){
        HeapNode mn = extract_min(&h);
        int u = mn.vertex;
        if(u == -1 || visited[u]) continue;

        visited[u] = 1;
        printf("[DEBUG] Visitando %d (dist=%d)\n", u, dist[u]);
        
        // Verifica se encontrou o alvo
        if(target >= 0 && u == target) {
            printf("[DEBUG] Alvo %d encontrado com distancia %d\n", target, dist[u]);
            return u;
        }
        
        // Verifica se o vértice atual é restaurante ou posto
        if(target == -1) {
            for(link p = g->adj[u]; p; p = p->next){
                if(p->vertex == u && p->is_restaurant) {
                    printf("[DEBUG] Restaurante encontrado no vertice %d com distancia %d\n", u, dist[u]);
                    return u;
                }
            }
        }
        if(target == -2) {
            for(link p = g->adj[u]; p; p = p->next){
                if(p->vertex == u && p->is_gasoline) {
                    printf("[DEBUG] Posto encontrado no vertice %d com distancia %d\n", u, dist[u]);
                    return u;
                }
            }
        }

        for(link p = g->adj[u]; p; p = p->next){
            int v = p->vertex, w = p->weight;
            if(dist[u] + w < dist[v]){
                dist[v] = dist[u] + w;
                parent[v] = u;
                insert_heap(&h, v, dist[v]);
                printf("[DEBUG] Atualizado: vertice=%d dist=%d pai=%d\n", v, dist[v], u);
            }
        }
    }
    
    if(target == -3) return 0; // Completou cálculo para todos
    return -1; // Não encontrou
}

// ---------------- Funções auxiliares ----------------


int temPedido(Estado *e, int destino){
    for(int i = 0; i < e->mochila_tamanho; i++) if(e->pedidos[i] == destino) return 1;
    return 0;
}

void pegarPedido(Estado *e, int destino){
    e->pedidos[e->mochila_tamanho++] = destino;
    printf("[DEBUG] Pegou pedido para %d | Mochila agora: %d\n", destino, e->mochila_tamanho);
}

void entregarPedido(Estado *e, int destino){
    for(int i = 0; i < e->mochila_tamanho; i++){
        if(e->pedidos[i] == destino){
            for(int j = i; j < e->mochila_tamanho-1; j++) e->pedidos[j] = e->pedidos[j+1];
            e->mochila_tamanho--;
            printf("[DEBUG] Entregou pedido para %d | Mochila agora: %d\n", destino, e->mochila_tamanho);
            return;
        }
    }
}

void atualizarCapacidadePosto(Graph g, int posto, int nova_capacidade) {
    // Procura o nó do posto no grafo e atualiza sua capacidade
    for(int i = 0; i <= g->vertices; i++) {
        for(link p = g->adj[i]; p; p = p->next) {
            if(p->vertex == posto && p->is_gasoline) {
                p->capacidade = nova_capacidade;
                printf("[DEBUG] Capacidade do posto %d atualizada para %d\n", posto, nova_capacidade);
                return;
            }
        }
    }
}

int temGasolinaDisponivel(Graph g, int posto) {
    // Verifica se o posto tem gasolina disponível (capacidade > 0)
    for(int i = 0; i <= g->vertices; i++) {
        for(link p = g->adj[i]; p; p = p->next) {
            if(p->vertex == posto && p->is_gasoline) {
                // Se capacidade é -1, ainda não sabemos (primeira vez)
                // Se capacidade é > 0, tem gasolina
                // Se capacidade é 0, não tem gasolina
                return (p->capacidade != 0);
            }
        }
    }
    return 0; // Não é posto ou não encontrado
}

// ---------------- Main ----------------

int main(){
    int N, M, H, T, I, C, P; 
    // vertices; arestas; casa; capacidade do tanque; combustivel inicial; capacidade maxima; quantidade de postos
    scanf("%d%d%d%d%d%d%d", &N, &M, &H, &T, &I, &C, &P);
    printf("[DEBUG] Parametros: N=%d M=%d H=%d T=%d I=%d C=%d P=%d\n", N,M,H,T,I,C,P);

    Graph g = newGraph(N); // Inicializa um grafo com N vertices
    
    // Arrays temporários para marcar restaurantes e postos
    int *is_restaurant = malloc((N + 1) * sizeof(int));
    int *is_gasoline = malloc((N + 1) * sizeof(int));
    for(int i = 0; i <= N; i++) {
        is_restaurant[i] = 0;
        is_gasoline[i] = 0;
    }

    // Salva os postos de gasolina
    for(int i = 0; i < P ; i++){
        int x;
        scanf("%d",&x);
        is_gasoline[x] = 1;
        printf("[DEBUG] Posto no vertice %d\n", x);
    };

    // Salva os vertices dos grafos
    for(int i = 0; i < M; i++){
        int u, v, w;
        scanf("%d%d%d", &u, &v, &w);
        addEdge(g, u, v, w, is_restaurant, is_gasoline);
    };

    // Salva os restaurantes e seus pedidos
    int Q;
    scanf("%d", &Q);
    printf("[DEBUG] Restaurantes: %d\n", Q);
    int **restaurant_orders = malloc((N + 1) * sizeof(int*));
    int *num_orders = malloc((N + 1) * sizeof(int));
    for(int i = 0; i <= N; i++) num_orders[i] = 0;
    
    for(int i = 0; i < Q; i++){
        int r, k;
        scanf("%d%d", &r, &k);
        is_restaurant[r] = 1;
        num_orders[r] = k;
        restaurant_orders[r] = malloc(k * sizeof(int));
        for(int j = 0; j < k; j++) scanf("%d", &restaurant_orders[r][j]);
        printf("[DEBUG] Restaurante %d com %d pedidos\n", r, k);
    }

    // Atualiza o estado do entregador
    Estado e = {.posicao_atual = H, .combustivel = I, .mochila_capacidade = C, .mochila_tamanho = 0};

    int *dist = malloc((N + 1) * sizeof(int));
    int *parent = malloc((N + 1) * sizeof(int));

    while(1){
        printf("[DEBUG] Estado: Pos=%d Comb=%d Mochila=%d\n", e.posicao_atual, e.combustivel, e.mochila_tamanho);
        
        // Calcular distâncias para todos os vértices
        dijkstra(g, e.posicao_atual, -3, dist, parent);

        // Verificar se terminou tudo e está em casa
        if(e.posicao_atual == H && e.mochila_tamanho == 0) {
            int restam_pedidos = 0;
            for(int i = 1; i <= N; i++) {
                if(is_restaurant[i] && num_orders[i] > 0) {
                    restam_pedidos = 1;
                    break;
                }
            }
            if(!restam_pedidos) {
                printf("x\n");
                fflush(stdout);
                break;
            }
        }

        // 1. Se está numa posição de entrega, entregar
        if(temPedido(&e, e.posicao_atual)) {
            printf("e %d\n", e.posicao_atual);
            fflush(stdout);
            char resp[10];
            scanf("%s", resp);
            entregarPedido(&e, e.posicao_atual);
            continue;
        }

        // 2. Se está num restaurante, pegar apenas um pedido (o mais próximo)
        if(is_restaurant[e.posicao_atual] && e.mochila_tamanho < e.mochila_capacidade && num_orders[e.posicao_atual] > 0) {
            // Calcular distâncias para todos os destinos dos pedidos deste restaurante
            int melhor_pedido = -1;
            int menor_dist_pedido = INF;
            int indice_melhor = -1;
            
            for(int i = 0; i < num_orders[e.posicao_atual]; i++) {
                int dest = restaurant_orders[e.posicao_atual][i];
                if(dist[dest] < menor_dist_pedido) {
                    menor_dist_pedido = dist[dest];
                    melhor_pedido = dest;
                    indice_melhor = i;
                }
            }
            
            // Pegar apenas o pedido mais próximo
            if(melhor_pedido != -1) {
                printf("p %d\n", melhor_pedido);
                fflush(stdout);
                char resp[10];
                scanf("%s", resp);
                pegarPedido(&e, melhor_pedido);
                
                // Remover o pedido da lista do restaurante
                for(int j = indice_melhor; j < num_orders[e.posicao_atual] - 1; j++) {
                    restaurant_orders[e.posicao_atual][j] = restaurant_orders[e.posicao_atual][j + 1];
                }
                num_orders[e.posicao_atual]--;
                printf("[DEBUG] Pedido %d removido do restaurante %d. Restam %d pedidos\n", 
                       melhor_pedido, e.posicao_atual, num_orders[e.posicao_atual]);
            }
            continue;
        }

        // 3. Se está num posto, abastecer se necessário
        if(is_gasoline[e.posicao_atual] && e.combustivel < T && temGasolinaDisponivel(g, e.posicao_atual)) {
            printf("a\n");
            fflush(stdout);
            int capacidade_retornada;
            scanf("%d", &capacidade_retornada);
            
            // Atualizar a capacidade do posto no grafo
            atualizarCapacidadePosto(g, e.posicao_atual, capacidade_retornada);
            
            e.combustivel = T;
            continue;
        }

        // 4. Decidir próximo destino
        int proximo_destino = -1;

        // Se tem pedidos na mochila, entregar o mais próximo
        if(e.mochila_tamanho > 0) {
            int melhor_entrega = -1;
            int menor_dist = INF;
            for(int i = 0; i < e.mochila_tamanho; i++) {
                if(dist[e.pedidos[i]] < menor_dist) {
                    menor_dist = dist[e.pedidos[i]];
                    melhor_entrega = e.pedidos[i];
                }
            }
            
            // Verificar se tem combustível para a entrega
            if(menor_dist <= e.combustivel) {
                proximo_destino = melhor_entrega;
            } else {
                // Precisa abastecer antes
                int melhor_posto = -1;
                int menor_dist_posto = INF;
                for(int i = 1; i <= N; i++) {
                    if(is_gasoline[i] && temGasolinaDisponivel(g, i) && dist[i] < menor_dist_posto) {
                        menor_dist_posto = dist[i];
                        melhor_posto = i;
                    }
                }
                if(melhor_posto == -1) {
                    // Nenhum posto tem gasolina disponível, voltar para casa
                    printf("[DEBUG] Nenhum posto com gasolina disponível, voltando para casa\n");
                    proximo_destino = H;
                } else {
                    proximo_destino = melhor_posto;
                }
            }
        } else {
            // Mochila vazia, ir para restaurante ou casa
            int melhor_restaurante = -1;
            int menor_dist_rest = INF;
            
            for(int i = 1; i <= N; i++) {
                if(is_restaurant[i] && num_orders[i] > 0 && dist[i] < menor_dist_rest) {
                    menor_dist_rest = dist[i];
                    melhor_restaurante = i;
                }
            }
            
            if(melhor_restaurante != -1) {
                // Verificar se tem combustível para ir ao restaurante
                if(menor_dist_rest <= e.combustivel) {
                    proximo_destino = melhor_restaurante;
                } else {
                    // Precisa abastecer antes
                    int melhor_posto = -1;
                    int menor_dist_posto = INF;
                    for(int i = 1; i <= N; i++) {
                        if(is_gasoline[i] && temGasolinaDisponivel(g, i) && dist[i] < menor_dist_posto) {
                            menor_dist_posto = dist[i];
                            melhor_posto = i;
                        }
                    }
                    if(melhor_posto == -1) {
                        // Nenhum posto tem gasolina disponível, voltar para casa
                        printf("[DEBUG] Nenhum posto com gasolina disponível, voltando para casa\n");
                        proximo_destino = H;
                    } else {
                        proximo_destino = melhor_posto;
                    }
                }
            } else {
                // Não há mais restaurantes, voltar para casa
                proximo_destino = H;
            }
        }

        // 5. Mover para o destino
        if(proximo_destino == -1 || dist[proximo_destino] == INF) {
            printf("x\n");
            fflush(stdout);
            break;
        }

        // Reconstruir e seguir o caminho
        int path[MAX_V];
        int path_len = 0;
        for(int v = proximo_destino; v != -1; v = parent[v]) {
            path[path_len++] = v;
        }
        
        for(int i = path_len - 2; i >= 0; i--) {
            int nxt = path[i];
            int prev = (i == path_len - 1) ? e.posicao_atual : path[i + 1];
            int custo = dist[nxt] - dist[prev];
            
            printf("m %d\n", nxt);
            fflush(stdout);
            char resp[10];
            scanf("%s", resp);
            
            e.combustivel -= custo;
            e.posicao_atual = nxt;
        }
    }
    return 0;
}