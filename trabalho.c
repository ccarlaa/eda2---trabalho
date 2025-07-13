#include <stdio.h>
#include <stdlib.h>

// Estrutura do nó
struct node {
    int vertex;
    int weight;
    struct node *next;
    int is_restaurant;
    int is_gasoline;
};

// Estrutura do nó
typedef struct node *link;

// Estrutura do grafo
struct graph {
    int vertices;
    int edges;
    link *adj;
};

// Estrutura do grafo
typedef struct graph *Graph;

// Cria um novo nó
static link newNode(int vertex, int weight) {
    link new = malloc(sizeof(struct node));
    new->vertex = vertex;
    new->weight = weight;
    new->next = NULL;
    new->is_restaurant = 0;
    new->is_gasoline = 0;
    return new;
}

// Cria um novo grafo
static Graph newGraph(int vertices) {
    Graph g = malloc(sizeof(struct graph));
    g->vertices = vertices;
    g->edges = 0;
    g->adj = malloc(vertices * sizeof(link));
    for (int i = 0; i < vertices; i++) {
        g->adj[i] = NULL;
        // Removido acesso a NULL pointer
    }
    return g;
}

// Adiciona uma aresta ao grafo
static void addEdge(Graph g, int src, int dest, int weight) {
    link new = newNode(dest, weight);
    new->next = g->adj[src];
    g->adj[src] = new;
    g->edges++;
}

// Imprime o grafo
static void printGraph(Graph g) {
    for (int i = 0; i < g->vertices; i++) {
        printf("%d: ", i);
        link p = g->adj[i];
        while (p != NULL) {
            printf("%d ", p->vertex);
            p = p->next;
        }
        printf("\n");
    }
}

// Insere uma ordem ordenada
static void insertOrdenedOrder(int* orders, int order_weight, int num_orders) {
    for (int i = 0; i < num_orders; i++) {
        if (orders[i] > order_weight) {
            for (int j = num_orders; j > i; j--) {
                orders[j] = orders[j - 1];
            }
            orders[i] = order_weight;
            return;
        }
    }
    orders[num_orders] = order_weight;
}

// Libera o grafo
static void freeGraph(Graph g) {
    for (int i = 0; i < g->vertices; i++) {
        link p = g->adj[i];
        while (p != NULL) {
            link q = p;
            p = p->next;
            free(q);
        }
    }
    free(g->adj);
    free(g);
}

int main() {
    while(1) {
        int vertices_num = 0;
        scanf("%d", &vertices_num);
        Graph g = newGraph(vertices_num);

        int edges_num = 0;

        int home = 0;
        scanf("%d", &home);

        int max_gasoline = 0;
        scanf("%d", &max_gasoline);

        int num_gasoline = 0;
        scanf("%d", &num_gasoline);

        int max_bag = 0;
        scanf("%d", &max_bag);

        int num_bag = 0;
        scanf("%d", &num_bag);

        // Lê as estações de gasolina
        int num_gasoline_stations = 0;
        scanf("%d", &num_gasoline_stations);
        printf("\n[DEBUG] Número de postos de gasolina: %d\n", num_gasoline_stations);

        for (int i = 0; i < num_gasoline_stations; i++) {
            int gasoline_station = 0;
            scanf("%d", &gasoline_station);
            // Verificar se o nó existe antes de acessar
            if (g->adj[gasoline_station] != NULL) {
                g->adj[gasoline_station]->is_gasoline = 1;
            } else {
                // Criar um nó para a estação de gasolina se não existir
                g->adj[gasoline_station] = newNode(gasoline_station, 0);
                g->adj[gasoline_station]->is_gasoline = 1;
            }
            printf("[DEBUG] Posto de gasolina %d: vértice %d\n", i+1, gasoline_station);
        }

        // Lê as arestas
        scanf("%d", &edges_num);
        for (int i = 0; i < edges_num; i++) {
            int src = 0, dest = 0, weight = 0;
            scanf("%d %d %d", &src, &dest, &weight);
            addEdge(g, src, dest, weight);
        }

        for (int i = 0; i < vertices_num; i++) {
            if (g->adj[i] != NULL) {
                printf("%d: ", i);
                link p = g->adj[i];
                while (p != NULL) {
                    printf("%d ", p->vertex);
                    p = p->next;
                }
                printf("\n");
            }
        }

        // Lê os restaurantes
        int num_restaurants = 0;
        scanf("%d", &num_restaurants);

        // Array de ponteiros para arrays de inteiros indexado pelo número do vértice
        // Cada posição guarda um array com as distâncias das entregas daquele restaurante
        int **restaurant_orders = malloc(vertices_num * sizeof(int*));
        int *num_orders_per_restaurant = malloc(vertices_num * sizeof(int));
        
        // Inicializa todos como NULL (não são restaurantes)
        for (int i = 0; i < vertices_num; i++) {
            restaurant_orders[i] = NULL;
            num_orders_per_restaurant[i] = 0;
        }
        
        // Lê os dados dos restaurantes
        printf("\n[DEBUG] Número de restaurantes: %d\n", num_restaurants);
        for (int i = 0; i < num_restaurants; i++) {
            int restaurant_vertex = 0;
            scanf("%d", &restaurant_vertex);
            
            // Marca este vértice como restaurante
            link p = g->adj[restaurant_vertex];
            if (p != NULL) {
                p->is_restaurant = 1;
            }
            
            int num_orders = 0;
            scanf("%d", &num_orders);
            num_orders_per_restaurant[restaurant_vertex] = num_orders;
            printf("[DEBUG] Restaurante %d: vértice %d, número de pedidos: %d\n", i+1, restaurant_vertex, num_orders);
            
            // Aloca o array para os pedidos deste restaurante
            restaurant_orders[restaurant_vertex] = malloc(num_orders * sizeof(int));

            for (int j = 0; j < num_orders; j++) {
                int order_weight = 0;
                scanf("%d", &order_weight);
                // Insere o pedido na ordem crescente
                insertOrdenedOrder(restaurant_orders[restaurant_vertex], order_weight, j);
            }
            
            // Imprime os pedidos do restaurante após inserção
            printf("[DEBUG] Pedidos do restaurante %d (vértice %d): ", i+1, restaurant_vertex);
            for (int j = 0; j < num_orders; j++) {
                printf("%d ", restaurant_orders[restaurant_vertex][j]);
            }
            printf("\n");
        }


        printf("\n[DEBUG] Estrutura do grafo:\n");
        printGraph(g);
        
        // Libera a memória dos arrays de pedidos dos restaurantes
        for (int i = 0; i < vertices_num; i++) {
            if (restaurant_orders[i] != NULL) {
                free(restaurant_orders[i]);
            }
        }
        free(restaurant_orders);
        free(num_orders_per_restaurant);
        
        freeGraph(g);
    }
    return 0;
}