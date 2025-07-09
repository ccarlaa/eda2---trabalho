# [cite_start]FCTE Delivery [cite: 1]

[cite_start]A FCTE (Federação das Comidas Transportadas com Eficiência) é uma empresa do ramo de delivery em plena expansão. [cite: 2] [cite_start]Diante da crescente demanda por novos colaboradores, você foi convidado a fazer parte dessa equipe (e ainda garantir uma graninha extra). [cite: 3] [cite_start]Após receber seu kit completo de trabalho, você está pronto para iniciar sua jornada de entregas. [cite: 4]

[cite_start]Partindo da sua residência, é preciso planejar uma rota eficiente para coletar e entregar pedidos de diversos restaurantes, sempre retornando ao ponto de partida ao final do trajeto. [cite: 5] [cite_start]As ruas possuem um custo fixo de deslocamento, e alguns pontos oferecem postos de combustível. [cite: 6] [cite_start]Seu veículo possui um tanque com capacidade limitada, e há também uma quantidade máxima de pedidos que podem ser transportados simultaneamente. [cite: 7] [cite_start]Cada pedido, uma vez coletado, deve obrigatoriamente ser entregue. [cite: 8]

[cite_start]Seu programa será interativo, se comunicando com um árbitro via entrada e saída padrão. [cite: 9] [cite_start]A cada ação, você imprime um comando e imediatamente lê a resposta do árbitro. [cite: 10]

## [cite_start]Ações Possíveis [cite: 11]

[cite_start]As ações são realizadas imprimindo uma linha na saída padrão. [cite: 12] As opções são:

* [cite_start]`m X` → mover-se para o local X, se for um vizinho imediato. [cite: 13]
* [cite_start]`a` → abastecer no local atual (caso haja posto de combustível). [cite: 14]
* [cite_start]`p D` → pegar pedido com destino ao local D. [cite: 15]
* [cite_start]`e D` → entregar pedido com destino ao local D. [cite: 16]
* [cite_start]`x` → encerrar a entrega (caso o entregador esteja em casa ou não seja possível continuar). [cite: 17]

[cite_start]**Importante:** Após cada ação, seu programa deve obrigatoriamente ler a entrada padrão para receber a resposta do árbitro. [cite: 18, 19] [cite_start]Qualquer ação inválida resultará em WA (Wrong Answer). [cite: 20]

## [cite_start]Respostas do Árbitro [cite: 21]

### [cite_start]m X [cite: 22]
[cite_start]Mover-se para o local X. [cite: 23] [cite_start]Se há uma rua válida até X e combustível suficiente, o árbitro retorna 1 caso a ação seja possível. [cite: 24, 25]
Caso contrário:
* [cite_start]Rua inexistente → WA [cite: 27]
* [cite_start]Combustível insuficiente → WA [cite: 28]

### a
[cite_start]Abastecer. [cite: 31] [cite_start]Se há posto no local atual: [cite: 32]
* Na primeira vez, o tanque será preenchido até a capacidade máxima. [cite_start]O árbitro retornará a quantidade de combustível restante no posto. [cite: 33]
* [cite_start]Se o posto tiver pouco combustível, abastece o máximo possível. [cite: 34]
* [cite_start]Caso não haja posto → WA. [cite: 35]

### [cite_start]p D [cite: 36]
[cite_start]Pegar pedido com destino D. [cite: 38]
* [cite_start]Adiciona o pedido à mochila. [cite: 39]
* [cite_start]Se estiver na capacidade máxima, o árbitro retorna -1 (pedido perdido). [cite: 40, 41]
* [cite_start]O entregador deve estar no local do restaurante. [cite: 42]

### [cite_start]e D [cite: 37]
[cite_start]Entregar pedido com destino D. [cite: 43]
* [cite_start]Se houver pedido com destino D, o pedido é removido e o árbitro retorna 1. [cite: 44]
* [cite_start]Caso contrário → WA. [cite: 44]

### [cite_start]x [cite: 45]
[cite_start]Encerrar entrega. [cite: 46]
* [cite_start]Encerra o programa imediatamente. [cite: 47]
* [cite_start]Só pode ser usado quando estiver na casa do entregador. [cite: 48]
* [cite_start]Se usado fora da casa, a pontuação para esse mapa será zerada. [cite: 49]

## [cite_start]Pontuação [cite: 50]

[cite_start]A pontuação por mapa será calculada seguindo a seguinte fórmula: [cite: 51]

[cite_start]**Fórmula:** [cite: 52]
[cite_start]$$PF = \frac{((F \cdot SP) + 1) \cdot \max(1, K)}{PB}$$ [cite: 53]

[cite_start]**Legenda:** [cite: 54]

| Símbolo | Significado |
| :--- | :--- |
| (F) | Entregas faltantes |
| (SP) | Soma dos pesos de todas as arestas |
| (K) | Quilometragem |
| (PB) | Pontuação base |
[cite_start][cite: 55]

## [cite_start]Entrada [cite: 57]

[cite_start]A primeira linha contém sete inteiros: [cite: 58]
[cite_start]`N M H T I C P` [cite: 59]

Onde:
* [cite_start]**(N):** número de locais (vértices), numerados de 1 a N. [cite: 61]
* [cite_start]**(M):** número de ruas (arestas). [cite: 62]
* [cite_start]**(H):** índice do local onde o entregador começa e termina (casa). [cite: 63]
* [cite_start]**(T):** capacidade máxima do tanque (em km). [cite: 64]
* [cite_start]**(I):** combustível inicial no tanque (em km). [cite: 65]
* [cite_start]**(C):** capacidade máxima da mochila (número máximo de pedidos). [cite: 66]
* [cite_start]**(P):** quantidade de locais que possuem posto de gasolina. [cite: 67]

[cite_start]A segunda linha contém `P` inteiros indicando os locais que possuem posto de gasolina: [cite: 68]
[cite_start]`P1 P2 ... PP` [cite: 69, 70]

[cite_start]Em seguida, `M` linhas descrevem as ruas, cada uma contendo três inteiros: [cite: 71]
[cite_start]`U V W` [cite: 72, 73, 74]

* [cite_start]**(u) e (v):** locais conectados (rua dirigida de u para v). [cite: 75]
* [cite_start]**(w):** custo (distância em km) da travessia. [cite: 76]

[cite_start]Depois, uma linha com um inteiro `Q`, a quantidade de restaurantes. [cite: 77]
[cite_start]Seguem `Q` linhas, cada uma contendo: [cite: 78]
[cite_start]`r k d1 d2 ... dk` [cite: 79, 80, 81]

* [cite_start]**(r):** local do restaurante. [cite: 82]
* [cite_start]**(k):** número de pedidos disponíveis. [cite: 83]
* [cite_start]**(d_i):** destino do i-ésimo pedido. [cite: 84]

### [cite_start]Exemplo de Entrada [cite: 85]
