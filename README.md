# Fogãozinho Educacional — Firmware ESP32

Firmware do **Fogãozinho Educacional**, primeiro módulo físico do projeto de iniciação científica **“Elaboração de Kits Educacionais de Robótica para Promoção do Letramento Digital em Contextos de Vulnerabilidade Social”**.

O protótipo combina **ESP32, display OLED, botões, potenciômetro, LEDs RGB e buzzer** com um livrinho interativo e puzzles físicos. A proposta é transformar conceitos de programação e automação em uma experiência lúdica, tangível e progressiva.

> **Brincar primeiro, aprender enquanto brinca.**

---

## Sobre o projeto

O Fogãozinho funciona como um brinquedo educacional interativo.

A criança encontra receitas bloqueadas, resolve desafios físicos no livrinho **_O Gatinho Engenheiro e as Receitas Perdidas_**, descobre códigos e utiliza esses códigos no fogão para recuperar e executar receitas interativas.

O firmware foi estruturado para separar três momentos:

1. **Puzzle físico** — a criança resolve o desafio no livrinho e descobre o código.
2. **Desbloqueio** — o código correto recupera a receita no fogão.
3. **Receita jogável** — a criança executa a receita usando botões, controle giratório, LEDs, tela e feedback sonoro.

A receita desbloqueada permanece salva mesmo após o desligamento do ESP32.

---

## Objetivos educacionais

O sistema foi projetado para introduzir conceitos de pensamento computacional utilizando situações cotidianas.

| Receita | Conceito principal | Código |
|---|---|---|
| Panqueca | Sequência / algoritmo | `92846` |
| Brigadeiro | Repetição / loop | `317056` |
| Sopa | Condição `SE / SENÃO` | `274196` |
| Bolo dos Números | Quantidades / variáveis | `4231` |
| Omelete Colorido | Combinação RGB | `121323` |
| Mingau do Bug | Depuração / debugging | `51827` |

Os códigos fazem parte dos puzzles físicos e são utilizados pelo firmware para validar e desbloquear cada receita.

---

## Fluxo de uso

```text
LIVRINHO
   ↓
PUZZLE FÍSICO
   ↓
CÓDIGO DESCOBERTO
   ↓
FOGÃOZINHO
   ↓
RECEITA RECUPERADA
   ↓
RECEITA JOGÁVEL
   ↓
CONCLUSÃO
```

O puzzle não é substituído pelo software.

Ele continua sendo responsável pela descoberta do código, enquanto o fogão transforma essa descoberta em uma experiência física e interativa.

---

# Funcionalidades

O firmware atualmente possui:

- menu principal;
- seleção de receitas;
- seis receitas diferentes;
- estados de receita bloqueada, desbloqueada e concluída;
- entrada de códigos com diferentes quantidades de dígitos;
- armazenamento de progresso com `Preferences`;
- interface gráfica em OLED;
- personagem do Gatinho Engenheiro;
- diferentes sprites de reação;
- controle de chama por potenciômetro;
- LEDs RGB para representar o fogo;
- animação de tremulação da chama;
- alertas de fogo alto;
- modo de perigo;
- buzzer para feedback sonoro;
- minigames específicos para cada receita;
- receitas executadas passo a passo;
- Modo Livre;
- reset manual do progresso;
- funcionamento sem necessidade de internet ou celular.

---

# Receitas jogáveis

## Panqueca

A criança recupera a sequência da receita pelo puzzle físico e depois executa a preparação no fogão.

Fluxo aproximado:

```text
Preparar a massa
↓
Ligar o fogo
↓
Aquecer a frigideira
↓
Colocar a massa
↓
Esperar aparecerem bolhas
↓
Virar a panqueca
↓
Dourar o outro lado
↓
Diminuir o fogo
↓
Retirar a panqueca
↓
Desligar o fogo
```

Durante as etapas de cocção, o sistema verifica continuamente a intensidade da chama.

Se a criança sair da faixa necessária, o fogão fornece feedback imediatamente.

---

## Brigadeiro

A receita trabalha principalmente o conceito de **repetição**.

Fluxo:

```text
Pegar a panela
↓
Adicionar os ingredientes
↓
Ligar em fogo baixo
↓
REPETIR
    mexer
↓
Continuar até engrossar
↓
Verificar o ponto
↓
Desligar
↓
Deixar esfriar
```

Durante o jogo, a criança precisa realizar ciclos de mexer enquanto mantém o fogo adequado.

---

## Sopa

A Sopa trabalha o conceito de **condição**.

Durante a receita, o fogão apresenta diferentes situações.

Exemplo:

```text
SE estiver fervendo demais
    DIMINUIR O FOGO

SENÃO
    CONTINUAR COZINHANDO
```

A criança precisa observar o estado apresentado e tomar a decisão adequada.

---

## Bolo dos Números

O Bolo trabalha conceitos de **quantidade, matemática e variáveis**.

Durante essa receita, o potenciômetro deixa temporariamente de controlar a chama e passa a representar valores.

A criança:

```text
recebe um problema
↓
calcula a resposta
↓
gira o controle
↓
seleciona o valor
↓
confirma
```

Depois disso, a preparação da receita continua normalmente.

---

## Omelete Colorido

O Omelete Colorido trabalha combinações de luz utilizando os canais RGB.

A receita possui duas partes principais:

```text
DESAFIO RGB
↓
PREPARAÇÃO DO OMELETE
```

A criança combina:

```text
R = Vermelho
G = Verde
B = Azul
```

para formar cores como:

```text
Amarelo = R + G
Magenta = R + B
Ciano = G + B
```

Depois do desafio de cores, a criança continua a preparação do omelete utilizando o controle de chama.

---

## Mingau do Bug

O Mingau do Bug trabalha o conceito de **depuração**.

Primeiro, a criança analisa diferentes comandos para identificar comportamentos incorretos.

Depois de encontrar o erro, executa a receita corrigida.

Fluxo aproximado:

```text
Adicionar ingredientes
↓
Aquecer em fogo baixo
↓
Mexer
↓
Esperar engrossar
↓
Desligar
↓
Deixar esfriar
```

---

# Arquitetura pedagógica

A proposta utiliza uma separação intencional entre:

```text
PENSAR
Puzzle físico

        ↓

VALIDAR
Código no fogão

        ↓

EXPERIMENTAR
Receita interativa
```

Dessa forma, a interface digital não entrega a resposta do puzzle.

A criança precisa primeiro organizar, interpretar, calcular ou depurar fisicamente para depois acessar a experiência correspondente no brinquedo.

---

# Livro interativo

O firmware foi concebido para funcionar em conjunto com o livro:

## **O Gatinho Engenheiro e as Receitas Perdidas**

O livro reúne:

- narrativa do Gatinho Engenheiro;
- apresentação da casa automatizada;
- Manual do Fogãozinho e Receitas;
- envelopes com peças físicas;
- puzzles de programação;
- dicas contextualizadas;
- campos para registrar códigos;
- acompanhamento das receitas recuperadas.

O livro amplia a experiência pedagógica, mas o hardware foi projetado para continuar sendo funcional de maneira independente.

---

# Hardware

O protótipo atual utiliza:

- ESP32 DevKit / WROOM;
- display OLED 0,96" 128×64 I²C;
- potenciômetro B10K;
- 3 push buttons;
- 4 LEDs RGB de 4 terminais;
- LEDs RGB de cátodo comum;
- 12 resistores de 330 Ω;
- buzzer ativo de 3 pinos;
- conversor DC-DC LM2596;
- suporte para 4 pilhas AA;
- capacitor eletrolítico de 470 µF / 16 V;
- chave liga/desliga;
- protoboard;
- jumpers.

---

# Pinagem

| Componente | GPIO |
|---|---|
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| Potenciômetro | GPIO 34 |
| Botão esquerdo / voltar | GPIO 25 |
| Botão central / OK | GPIO 26 |
| Botão direito / avançar | GPIO 27 |
| Buzzer | GPIO 14 |
| LED RGB — Vermelho | GPIO 18 |
| LED RGB — Azul | GPIO 19 |
| LED RGB — Verde | GPIO 23 |

O display OLED utiliza o endereço I²C:

```text
0x3C
```

Os quatro LEDs RGB compartilham os mesmos sinais de controle.

Cada canal de cada LED possui seu próprio resistor.

---

# Controles

## Botão esquerdo

Utilizado para:

- voltar;
- apagar dígitos;
- cancelar determinadas ações;
- sair de algumas telas.

---

## Botão central — OK

Utilizado para:

- selecionar;
- confirmar;
- registrar ações;
- interagir durante as receitas.

---

## Botão direito

Utilizado para:

- avançar;
- confirmar códigos;
- responder a determinados desafios.

---

## Controle giratório

O potenciômetro possui comportamento **contextual**.

Sua função depende da etapa atual do jogo.

Ele pode ser utilizado para:

```text
selecionar números
controlar a chama
selecionar valores
selecionar canais RGB
```

---

# Sistema de chama

Durante as etapas apropriadas, o potenciômetro controla a intensidade da chama.

O firmware utiliza os seguintes estados:

```text
DESLIGADO
BAIXO
MÉDIO
ALTO
MUITO ALTO
PERIGO
```

O sistema combina:

- intensidade dos LEDs;
- mudança de cor;
- tremulação;
- mensagens na OLED;
- reações do Gatinho;
- alarmes sonoros.

---

## Feedback da chama

Exemplos:

```text
FOGO APAGOU
```

```text
AUMENTE +
```

```text
PERFEITO!
```

```text
DIMINUA -
```

```text
MUITO ALTO!
```

```text
PERIGO!
```

Quando uma receita exige que o fogo permaneça em determinada faixa, sair da faixa pode interromper ou reiniciar o progresso da etapa.

---

# Gatinho Engenheiro

O Gatinho funciona como parte da interface visual do sistema.

Ele possui diferentes estados:

```text
Dormindo
Atento
Assustado
Muito assustado
```

Esses estados ajudam a representar visualmente a situação atual do fogão.

Exemplo:

```text
Fogo apagado
→ Gatinho dormindo
```

```text
Fogo correto
→ Gatinho atento
```

```text
Fogo alto
→ Gatinho assustado
```

```text
Perigo
→ Gatinho muito assustado + buzzer
```

---

# LEDs RGB

Os quatro LEDs representam visualmente a chama.

A progressão utilizada no protótipo busca simular:

```text
BAIXO
amarelo

↓

MÉDIO
amarelo / laranja

↓

ALTO
laranja / vermelho

↓

PERIGO
vermelho intenso
```

Uma pequena quantidade de azul pode aparecer no nível máximo para aumentar a sensação visual de chama.

Os LEDs também apresentam uma pequena variação de intensidade para simular tremulação.

---

# Persistência de progresso

O firmware utiliza:

```cpp
#include <Preferences.h>
```

A biblioteca `Preferences` permite armazenar dados na memória não volátil do ESP32.

São armazenados estados como:

```text
receita desbloqueada
receita concluída
progresso necessário para o Modo Livre
```

Dessa forma, desligar o brinquedo não remove as receitas já conquistadas.

---

# Estados das receitas

Cada receita pode possuir três estados principais:

```text
BLOQUEADA
```

A criança ainda não descobriu o código.

```text
LIBERADA
```

O código correto já foi informado.

```text
CONCLUÍDA
```

A criança terminou a gameplay da receita.

Assim:

```text
desbloquear != concluir
```

O código apenas permite acessar a receita.

A conclusão exige que a criança realmente execute a atividade.

---

# Modo Livre

O Modo Livre permanece bloqueado inicialmente.

Ele é liberado após a conclusão das seis receitas.

Nesse modo, a criança pode utilizar livremente o controle de chama.

O sistema continua oferecendo:

- animação dos LEDs;
- diferentes intensidades;
- reação do Gatinho;
- alertas;
- buzzer em situações de perigo.

---

# Reset do progresso

Para apagar o progresso salvo:

1. desligue o fogão;
2. mantenha pressionados os botões esquerdo e direito;
3. ligue o ESP32;
4. continue pressionando por aproximadamente 2 segundos.

No hardware atual:

```text
GPIO 25 + GPIO 27
```

O sistema exibirá uma confirmação de reset.

---

# Bibliotecas utilizadas

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
```

---

## Dependências externas

Instale pela **Library Manager** da Arduino IDE:

```text
Adafruit GFX Library
Adafruit SSD1306
```

As bibliotecas:

```text
Arduino
Wire
Preferences
```

fazem parte do ambiente ESP32/Arduino.

---

# Configuração na Arduino IDE

1. Instale o suporte para placas ESP32.
2. Selecione a placa correspondente ao seu ESP32.
3. Instale as bibliotecas necessárias.
4. Abra o arquivo `.ino`.
5. Selecione a porta serial correta.
6. Compile o firmware.
7. Faça o upload.

No ambiente utilizado durante o desenvolvimento deste protótipo, a velocidade de upload utilizada foi:

```text
921600
```

Essa opção pode variar conforme a versão do pacote ESP32, placa e Arduino IDE.

---

# Alimentação

O protótipo utiliza quatro pilhas AA.

Fluxo de alimentação:

```text
4× AA
   ↓
Chave liga/desliga
   ↓
LM2596
   ↓
aproximadamente 3,3 V
   ↓
ESP32 + circuito
```

Antes de conectar o ESP32 ao circuito, a saída do LM2596 deve ser ajustada utilizando um multímetro.

Valor utilizado no protótipo:

```text
aproximadamente 3,30 V
```

---

## Atenção durante o upload

Durante a gravação do firmware por USB, a alimentação externa aplicada ao pino `3V3` deve ser desconectada.

No protótipo atual:

```text
UPLOAD

USB
 ↓
ESP32
```

e:

```text
USO COM BATERIA

4xAA
 ↓
LM2596
 ↓
ESP32
```

Evite manter simultaneamente a alimentação externa em `3V3` e o USB conectado durante a gravação.

---

# Estrutura sugerida do repositório

```text
fogaozinho-educacional-pibic/
│
├── firmware/
│   └── Fogaozinho_PIBIC.ino
│
├── docs/
│   ├── circuito/
│   ├── imagens/
│   └── modelagem-3d/
│
├── README.md
└── LICENSE
```

Outros materiais podem ser adicionados posteriormente.

---

# Status do projeto

**Protótipo funcional em desenvolvimento.**

Atualmente:

- [x] Menu principal
- [x] Menu de receitas
- [x] Seis códigos
- [x] Seis puzzles físicos definidos
- [x] Desbloqueio persistente
- [x] Receitas jogáveis
- [x] Controle de chama
- [x] Feedback por OLED
- [x] LEDs RGB
- [x] Buzzer
- [x] Sprites do Gatinho Engenheiro
- [x] Modo Livre
- [x] Reset de progresso
- [x] Integração conceitual com o livrinho
- [ ] Finalização física do livrinho
- [ ] Testes com público-alvo
- [ ] Refinamentos após testes de usabilidade
- [ ] Desenvolvimento dos demais módulos do kit

---

# Contexto acadêmico

Este firmware integra o projeto de iniciação científica:

## **Elaboração de Kits Educacionais de Robótica para Promoção do Letramento Digital em Contextos de Vulnerabilidade Social**

A pesquisa investiga o uso de:

- robótica educacional;
- cultura maker;
- pensamento computacional;
- objetos cotidianos;
- fabricação digital;
- atividades físico-digitais;

como ferramentas para aproximar crianças e pré-adolescentes de conceitos tecnológicos de maneira acessível, lúdica e inclusiva.

---

# Autoria

Projeto desenvolvido no contexto de iniciação científica no **Centro Universitário UDF**.

**Sofia Sarmento Soares Prado**  
**Victória Resende Vieira**  
**Karla Roberto Sartin**

---

# Licença

A licença do projeto ainda deve ser definida antes da distribuição pública do código.

Caso o repositório seja disponibilizado como software livre, deverá ser adicionado um arquivo:

```text
LICENSE
```

correspondente à licença escolhida.

---

# Observação

Este repositório documenta um **protótipo educacional em desenvolvimento**.

O firmware, a arquitetura eletrônica, os modelos físicos e as atividades pedagógicas podem sofrer alterações durante os ciclos de desenvolvimento, teste e validação.
