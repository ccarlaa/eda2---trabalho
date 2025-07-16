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
    struct node *next;
} *link;

typedef struct {
    int vertices;
    int edges;
    link *adj;
    int *is_restaurant;
    int *is_gasoline;
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

static link newNode(int v, int w) {
    link n = malloc(sizeof(struct node));
    n->vertex = v; n->weight = w; n->next = NULL;
    return n;
}

Graph newGraph(int V) {
    Graph g = malloc(sizeof(*g));
    g->vertices = V; g->edges = 0;
    g->adj = malloc((V+1)*sizeof(link));
    g->is_restaurant = calloc(V+1,sizeof(int));
    g->is_gasoline = calloc(V+1,sizeof(int));
    for (int i=0;i<=V;i++) g->adj[i]=NULL;
    printf("[DEBUG] Grafo criado com %d vertices\n", V);
    return g;
}

void addEdge(Graph g,int u,int v,int w){
    link n = newNode(v,w);
    n->next=g->adj[u];
    g->adj[u]=n;
    printf("[DEBUG] Aresta adicionada: %d -> %d (peso=%d)\n", u, v, w);
}

// ---------------- Dijkstra com caminho ----------------

void dijkstra(Graph g,int start,int *dist,int *parent){
    int V=g->vertices;
    for(int i=0;i<=V;i++){ dist[i]=INF; parent[i]=-1; }
    dist[start]=0;
    MinHeap h; h.size=0;
    insert_heap(&h,start,0);
    int visited[V+1]; memset(visited,0,sizeof(visited));
    printf("[DEBUG] Executando Dijkstra a partir de %d\n", start);
    while(h.size>0){
        HeapNode mn=extract_min(&h);
        int u=mn.vertex;
        if(u==-1||visited[u]) continue;
        visited[u]=1;
        printf("[DEBUG] Visitando %d (dist=%d)\n", u, dist[u]);
        for(link p=g->adj[u];p;p=p->next){
            int v=p->vertex,w=p->weight;
            if(dist[u]+w<dist[v]){
                dist[v]=dist[u]+w;
                parent[v]=u;
                insert_heap(&h,v,dist[v]);
                printf("[DEBUG] Atualizado: vertice=%d dist=%d pai=%d\n", v, dist[v], u);
            }
        }
    }
}

// ---------------- Funções auxiliares ----------------

void reconstruirCaminho(int alvo,int *parent,int atual,int *caminho,int *len){
    *len=0;
    for(int v=alvo;v!=-1;v=parent[v]) caminho[(*len)++]=v;
    for(int i=0;i<*len/2;i++){ int tmp=caminho[i]; caminho[i]=caminho[*len-1-i]; caminho[*len-1-i]=tmp; }
    printf("[DEBUG] Caminho até %d: ", alvo);
    for(int i=0;i<*len;i++) printf("%d ", caminho[i]);
    printf("\n");
}

int temPedido(Estado *e,int destino){
    for(int i=0;i<e->mochila_tamanho;i++) if(e->pedidos[i]==destino) return 1;
    return 0;
}

void pegarPedido(Estado *e,int destino){
    e->pedidos[e->mochila_tamanho++]=destino;
    printf("[DEBUG] Pegou pedido para %d | Mochila agora: %d\n", destino, e->mochila_tamanho);
}

void entregarPedido(Estado *e,int destino){
    for(int i=0;i<e->mochila_tamanho;i++){
        if(e->pedidos[i]==destino){
            for(int j=i;j<e->mochila_tamanho-1;j++) e->pedidos[j]=e->pedidos[j+1];
            e->mochila_tamanho--;
            printf("[DEBUG] Entregou pedido para %d | Mochila agora: %d\n", destino, e->mochila_tamanho);
            return;
        }
    }
}

// ---------------- Main ----------------

int main(){
    int N,M,H,T,I,C,P;
    scanf("%d%d%d%d%d%d%d",&N,&M,&H,&T,&I,&C,&P);
    printf("[DEBUG] Parametros: N=%d M=%d H=%d T=%d I=%d C=%d P=%d\n", N,M,H,T,I,C,P);

    Graph g=newGraph(N);
    for(int i=0;i<P;i++){ int x; scanf("%d",&x); g->is_gasoline[x]=1; printf("[DEBUG] Posto no vertice %d\n", x); }
    for(int i=0;i<M;i++){ int u,v,w; scanf("%d%d%d",&u,&v,&w); addEdge(g,u,v,w); }
    int Q; scanf("%d",&Q);
    printf("[DEBUG] Restaurantes: %d\n", Q);
    int **restaurant_orders=malloc((N+1)*sizeof(int*));
    int *num_orders=calloc(N+1,sizeof(int));
    for(int i=0;i<Q;i++){
        int r,k; scanf("%d%d",&r,&k);
        g->is_restaurant[r]=1;
        num_orders[r]=k;
        restaurant_orders[r]=malloc(k*sizeof(int));
        for(int j=0;j<k;j++) scanf("%d",&restaurant_orders[r][j]);
        printf("[DEBUG] Restaurante %d com %d pedidos\n", r, k);
    }

    Estado e={.posicao_atual=H,.combustivel=I,.mochila_capacidade=C,.mochila_tamanho=0};
    PedidoHeap heapPedidos; heapPedidos.size=0;
    int *dist=malloc((N+1)*sizeof(int));
    int *parent=malloc((N+1)*sizeof(int));
    int caminho[MAX_V]; int len;

    while(1){
        printf("[DEBUG] Estado: Pos=%d Comb=%d Mochila=%d\n", e.posicao_atual,e.combustivel,e.mochila_tamanho);
        dijkstra(g,e.posicao_atual,dist,parent);

        // 1. Entregar pedido
        if(temPedido(&e,e.posicao_atual)){
            printf("[DEBUG] Entregando pedido em %d\n", e.posicao_atual);
            printf("e %d\n",e.posicao_atual); fflush(stdout);
            char resp[10]; scanf("%s",resp);
            entregarPedido(&e,e.posicao_atual);
            continue;
        }

        // 2. Pegar pedidos
        if(g->is_restaurant[e.posicao_atual] && e.mochila_tamanho<e.mochila_capacidade && num_orders[e.posicao_atual]>0){
            printf("[DEBUG] Pegando pedidos no restaurante %d\n", e.posicao_atual);
            int num=num_orders[e.posicao_atual];
            for(int i=0;i<num && e.mochila_tamanho<e.mochila_capacidade;i++){
                int dest=restaurant_orders[e.posicao_atual][i];
                printf("p %d\n",dest); fflush(stdout);
                char resp[10]; scanf("%s",resp);
                pegarPedido(&e,dest);
                insertPedidoHeap(&heapPedidos,dest,dist[dest]);
            }
            num_orders[e.posicao_atual]=0;
            continue;
        }

        // 3. Abastecer
        if(g->is_gasoline[e.posicao_atual] && e.combustivel<T){
            printf("[DEBUG] Abastecendo no posto %d\n", e.posicao_atual);
            printf("a\n"); fflush(stdout);
            char resp[10]; scanf("%s",resp);
            e.combustivel=T;
            continue;
        }

        // 4. Próximo alvo
        PedidoNode prox=extractPedidoMin(&heapPedidos);
        int alvo=-1;
        if(prox.destino!=-1) alvo=prox.destino;
        else {
            if(e.posicao_atual==H && e.mochila_tamanho==0){ printf("[DEBUG] Todos pedidos entregues. Encerrando.\n"); printf("x\n"); fflush(stdout); break; }
            else alvo=H;
        }

        if(dist[alvo]==INF || dist[alvo]>e.combustivel){
            printf("[DEBUG] Nao e possivel chegar ao alvo (distancia ou combustivel insuficiente). Encerrando.\n");
            printf("x\n"); fflush(stdout);
            break;
        }

        reconstruirCaminho(alvo,parent,e.posicao_atual,caminho,&len);
        for(int i=1;i<len;i++){
            int nxt=caminho[i];
            int custo=dist[nxt]-dist[caminho[i-1]];
            printf("[DEBUG] Movendo para %d (custo=%d)\n", nxt, custo);
            printf("m %d\n",nxt); fflush(stdout);
            char resp[10]; scanf("%s",resp);
            e.combustivel-=custo;
            e.posicao_atual=nxt;
        }
    }
    return 0;
}