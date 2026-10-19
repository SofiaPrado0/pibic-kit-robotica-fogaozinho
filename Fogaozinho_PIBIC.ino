// Fogaozinho Educacional - PIBIC/PIBIT
// Firmware v1.3 - receita + gameplay de fogo

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include <cstring>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LED_R 18
#define LED_B 19
#define LED_G 23

#define POT 34

#define BOTAO_1 25   // esquerda / voltar
#define BOTAO_2 26   // OK
#define BOTAO_3 27   // direita / confirmar

#define BUZZER 14    // modulo low-level trigger

enum TipoGato : uint8_t {
  GATO_DORMINDO,
  GATO_ATENTO,
  GATO_ASSUSTADO,
  GATO_MEGA
};

enum ZonaFogo : uint8_t {
  FOGO_OFF,
  FOGO_BAIXO,
  FOGO_MEDIO,
  FOGO_ALTO,
  FOGO_MUITO_ALTO,
  FOGO_PERIGO
};

struct DadosFogo {
  int pot;
  int potencia;
  int r;
  int g;
  int b;
  ZonaFogo zona;
};

enum EstadoSistema : uint8_t {
  MENU_PRINCIPAL,
  MENU_RECEITAS,
  DIGITAR_CODIGO,
  JOGAR_RECEITA,
  MODO_LIVRE
};

enum FaseReceita : uint8_t {
  RECEITA_RECAP,
  RECEITA_JOGO
};

enum EstadoPanqueca : uint8_t {
  PAN_ZERO,
  PAN_PREPARAR_MASSA,
  PAN_BAIXO_INICIAL,
  PAN_AQUECER,
  PAN_MASSA,
  PAN_COZINHAR_1,
  PAN_VIRAR,
  PAN_COZINHAR_2,
  PAN_REDUZIR,
  PAN_RETIRAR,
  PAN_DESLIGAR
};

enum EstadoBrigadeiro : uint8_t {
  BRI_ZERO,
  BRI_PANELA,
  BRI_INGREDIENTES,
  BRI_BAIXO,
  BRI_MEXER,
  BRI_ENGROSSAR,
  BRI_PONTO,
  BRI_DESLIGAR
};

enum EstadoSopa : uint8_t {
  SOP_ZERO,
  SOP_PANELA,
  SOP_INGREDIENTES,
  SOP_AQUECER,
  SOP_MEXER,
  SOP_CENARIOS,
  SOP_FINALIZAR,
  SOP_DESLIGAR
};

enum EstadoBolo : uint8_t {
  BOL_PERGUNTAS,
  BOL_JUNTAR,
  BOL_MISTURAR,
  BOL_FORMA,
  BOL_ZERO,
  BOL_PREAQUECER,
  BOL_REDUZIR,
  BOL_COLOCAR,
  BOL_ASSAR,
  BOL_VERIFICAR,
  BOL_DESLIGAR
};

enum EstadoOmelete : uint8_t {
  OME_PREPARAR,
  OME_TEMPERO,
  OME_CORES,
  OME_APAGAR,
  OME_ZERO_FOGO,
  OME_BAIXO,
  OME_COLOCAR,
  OME_SUBIR_MEDIO,
  OME_COZINHAR,
  OME_REDUZIR,
  OME_DOBRAR,
  OME_DESLIGAR
};

enum EstadoMingau : uint8_t {
  MIN_DEBUG,
  MIN_ZERO,
  MIN_INGREDIENTES,
  MIN_BAIXO,
  MIN_MEXER,
  MIN_ENGROSSAR,
  MIN_PONTO,
  MIN_DESLIGAR,
  MIN_ESFRIAR
};

void buzzerOff();
void buzzerOn();
void bip(int tempo);
void bipDuplo();
void bipTriplo();
void somVitoria();

void corRGB(int r, int g, int b);
void apagarFogo();
DadosFogo calcularFogo();
void aplicarFogoAnimado(DadosFogo fogo);
void atualizarAlarme(ZonaFogo zona);

bool clicou(int pino);

void desenharBitmapVertical(int x0, int y0, const unsigned char* bitmap, int largura, int altura);
void desenharGota(int x, int y);
void desenharGato(TipoGato tipo, bool gota);

void printLimitado(const char* texto, int maxChars);
void cabecalho(const char* texto);
void telaBase(const char* titulo, const char* l1, const char* l2, const char* l3, TipoGato gato, bool gota);
void telaComBarra(const char* titulo, const char* l1, const char* l2, const char* l3, TipoGato gato, bool gota, unsigned long inicio, unsigned long total);
void mostrarMensagem(const char* titulo, const char* l1, const char* l2, int tempo, TipoGato gato);
void telaAbertura();

bool fogoNaFaixa(DadosFogo fogo, ZonaFogo alvo);
void feedbackFogo(DadosFogo fogo, ZonaFogo alvo, const char*& texto, TipoGato& gato, bool& gota);
bool manterFaixa(DadosFogo fogo, ZonaFogo alvo, unsigned long tempoNecessario);

void carregarProgresso();
void salvarUnlock(int receita);
void salvarConclusao(int receita);
void resetarProgresso();
bool todasReceitasConcluidas();

void atualizarMenuPrincipal();
void atualizarMenuReceitas();
void atualizarTelaCodigo();
void iniciarReceita(int receita);
void entrarJogoReceita();
void atualizarRecapReceita();
void concluirReceita(int receita);
void atualizarReceitaAtual();
void atualizarModoLivre();
void verificarResetProgresso();

void jogoPanqueca();
void jogoBrigadeiro();
void jogoSopa();
void jogoBolo();
void jogoOmelete();
void jogoMingau();

const int gatoDormindo_width = 37;
const int gatoDormindo_height = 29;
const unsigned char gatoDormindo[] PROGMEM = {
0x00,0x00,0xc0,0xc0,0x60,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0xe0,0x60,
0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x60,0x60,0x60,0x03,0x03,0x03,0x03,
0x63,0x63,0x60,0x40,0x00,0xff,0xff,0x0c,0x0c,0x8c,0x8c,0x0c,0x0c,0x0c,0x0c,0x0c,
0x0c,0x0c,0x8c,0x10,0x10,0xe0,0xe0,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x3e,0xdc,
0xd8,0xd8,0xd8,0xf8,0xf8,0xd8,0xd8,0xd8,0xc6,0xc6,0xe1,0xf1,0xfe,0xfe,0xc1,0xdf,
0xde,0xdc,0xd8,0xd8,0xd8,0xd8,0xd8,0xc7,0x23,0x20,0x1f,0x1f,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00
};

const int gatoAtento_width = 30;
const int gatoAtento_height = 37;
const unsigned char gatoAtento[] PROGMEM = {
0x00,0x00,0x00,0x00,0x00,0xc0,0xe0,0x30,0xe0,0xc0,0x80,0x80,0x80,0x80,0x80,0x80,
0xc0,0xe0,0x30,0xe0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xff,0xff,0x00,0x20,0x60,0x01,0x01,0x01,0x01,0x61,0x21,0x00,0x00,
0x00,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x01,0x00,0xf2,0xcc,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0xf2,0xf3,0x01,
0x01,0xf8,0xfc,0x06,0x06,0xf6,0xfe,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7c,
0x7c,0x83,0xf8,0xf8,0x80,0x80,0xfc,0xfc,0x80,0x80,0xf8,0xf3,0x83,0xfc,0xfc,0x8f,
0x8f,0x60,0x60,0x1f,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,
0x00,0x00,0x00,0x00,0x00,0x00
};

const int gatoAssustado_width = 39;
const int gatoAssustado_height = 31;
const unsigned char gatoAssustado[] PROGMEM = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc0,0x30,0x30,
0x18,0x30,0xc0,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0xfc,0xec,0x0c,
0xcc,0x18,0x18,0x18,0xd8,0xd8,0x0c,0x0c,0xec,0xfc,0xdc,0x60,0x20,0x20,0x20,0x20,
0x20,0x20,0xc0,0xc0,0xe4,0x1b,0x1a,0x80,0x64,0x1b,0x03,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x03,0x03,0x0c,0x11,0xf0,0xf0,0x10,0xf1,0xf1,0x90,0xec,0x6f,
0x2f,0x20,0x20,0x20,0x20,0x20,0x60,0xe0,0x00,0xe0,0xe0,0x1e,0xe2,0x01,0x01,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,
0x06,0x07,0x06,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x02,0x06,0x07,0x06,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00
};

const int gatoMega_width = 34;
const int gatoMega_height = 28;
const unsigned char gatoMega[] PROGMEM = {
0x00,0x00,0x00,0xf0,0x10,0x60,0x80,0x80,0x80,0x80,0x80,0x70,0x10,0x10,0xe0,0x80,
0x80,0x80,0x80,0x00,0x00,0x00,0xc0,0x7c,0x04,0xfc,0xf8,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x14,0x2f,0x40,0x88,0x89,0xb1,0xb1,0x01,0x09,0x40,0xc0,0xc0,
0x3f,0x17,0x81,0x81,0x01,0x02,0x06,0x04,0x07,0x00,0x3e,0xe1,0xc1,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xc0,0xb9,0xdf,0xf7,0x71,0x88,0xc0,0x70,0x08,0x08,0x06,0x02,
0x01,0x01,0x01,0x06,0x06,0x0a,0x36,0xc6,0xb8,0xe1,0xc1,0x1e,0x38,0xc0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00
};

const int TOTAL_RECEITAS = 6;

const char* NOMES_RECEITAS[TOTAL_RECEITAS] = {
  "PANQUECA",
  "BRIGADEIRO",
  "SOPA",
  "BOLO NUMEROS",
  "OMELETE RGB",
  "MINGAU BUG"
};

const char* CODIGOS_RECEITAS[TOTAL_RECEITAS] = {
  "92846",   // Panqueca - sequencia / algoritmo
  "317056",  // Brigadeiro - repeticao / loop
  "274196",  // Sopa - SE / SENAO
  "4231",    // Bolo - quantidades / variaveis
  "121323",  // Omelete - RGB
  "51827"    // Mingau - depuracao
};

bool receitaDesbloqueada[TOTAL_RECEITAS] = {
  false, false, false, false, false, false
};

bool receitaConcluida[TOTAL_RECEITAS] = {
  false, false, false, false, false, false
};

Preferences prefs;

EstadoSistema estadoAtual = MENU_PRINCIPAL;

int opcaoMenuPrincipal = 0;
int receitaSelecionada = 0;
int receitaEmJogo = 0;

String codigoDigitado = "";

unsigned long inicioFaixa = 0;
unsigned long inicioDesligado = 0;
unsigned long ultimoAviso = 0;
unsigned long ultimoAlarme = 0;
unsigned long inicioAcerto = 0;

bool alarmeLigado = false;

unsigned long ultimaVariacaoChama = 0;
int brilhoChama = 100;

FaseReceita faseReceita = RECEITA_RECAP;
int passoRecap = 0;

const uint8_t RECAP_TOTAL[TOTAL_RECEITAS] = { 5, 6, 6, 4, 3, 5 };

const char* RECAP_L1[TOTAL_RECEITAS][6] = {
  { "LIGAR",      "AQUECER",     "COLOCAR",    "VIRAR",       "DESLIGAR", "" },
  { "PEGAR",      "COLOCAR",     "FOGO",       "REPETIR 3X",  "MEXER",    "DESLIGAR" },
  { "LIGAR",      "COLOCAR",     "SE FERVER +", "DIMINUIR",    "SENAO",    "DESLIGAR" },
  { "A: 6-2",     "B: 4/2",      "C: 1+2",     "D: 5-4",      "",         "" },
  { "AMARELO",    "MAGENTA",     "CIANO",      "",            "",         "" },
  { "PEGAR",      "ADICIONAR",   "FOGO",       "MEXER ATE",    "DESLIGAR", "" }
};

const char* RECAP_L2[TOTAL_RECEITAS][6] = {
  { "O FOGO",     "FRIGIDEIRA",  "A MASSA",    "PANQUECA",    "O FOGO",     "" },
  { "A PANELA",   "INGREDIENTES","BAIXO",      "",            "",           "O FOGO" },
  { "O FOGO",     "A PANELA",    "",           "O FOGO",      "CONTINUAR",   "O FOGO" },
  { "RESULTADO: 4","RESULTADO: 2","RESULTADO: 3","RESULTADO: 1","",          "" },
  { "R + G",      "R + B",       "G + B",      "",            "",           "" },
  { "A PANELA",   "LEITE",       "BAIXO",      "ENGROSSAR",    "O FOGO",      "" }
};

EstadoPanqueca estadoPanqueca = PAN_ZERO;
EstadoBrigadeiro estadoBrigadeiro = BRI_ZERO;
EstadoSopa estadoSopa = SOP_ZERO;
EstadoBolo estadoBolo = BOL_PERGUNTAS;
EstadoOmelete estadoOmelete = OME_PREPARAR;
EstadoMingau estadoMingau = MIN_DEBUG;

int mexidasBrigadeiro = 0;
bool brigadeiroPodeMexer = false;

int cenarioSopa = 0;
const bool sopaFervendo[4] = { false, true, false, true };
bool avisoSopaTocado = false;

int perguntaBolo = 0;
const char* boloLinha1[4] = {
  "A=3  B=4",
  "X=8",
  "Q=2 +3",
  "N=9 -6"
};
const char* boloLinha2[4] = {
  "A+B = ?",
  "METADE = ?",
  "NOVO Q = ?",
  "NOVO N = ?"
};
const int boloResposta[4] = { 7, 4, 5, 3 };

int etapaCor = 0;
bool canalR = false;
bool canalG = false;
bool canalB = false;
const char* alvoCorNome[3] = {
  "AMARELO",
  "MAGENTA",
  "CIANO"
};

int passoDebug = 0;
const int TOTAL_DEBUG = 6;
const char* debugComando[TOTAL_DEBUG] = {
  "PEGAR PANELA",
  "ADD ACUCAR",
  "FOGO MAXIMO",
  "MEXER",
  "IR EMBORA",
  "DESLIGAR"
};
const bool debugEhBug[TOTAL_DEBUG] = {
  false,
  false,
  true,
  false,
  true,
  false
};
int mexidasMingau = 0;
bool mingauPodeMexer = false;

void buzzerOff() {
  digitalWrite(BUZZER, HIGH);
}

void buzzerOn() {
  digitalWrite(BUZZER, LOW);
}

void bip(int tempo) {
  buzzerOn();
  delay(tempo);
  buzzerOff();
}

void bipDuplo() {
  bip(65);
  delay(60);
  bip(65);
}

void bipTriplo() {
  bip(55);
  delay(50);
  bip(55);
  delay(50);
  bip(55);
}

void somVitoria() {
  bip(70);
  delay(60);
  bip(70);
  delay(60);
  bip(130);
}

void corRGB(int r, int g, int b) {
  analogWrite(LED_R, constrain(r, 0, 255));
  analogWrite(LED_G, constrain(g, 0, 255));
  analogWrite(LED_B, constrain(b, 0, 255));
}

void apagarFogo() {
  corRGB(0, 0, 0);
}

DadosFogo calcularFogo() {
  DadosFogo fogo;

  fogo.pot = analogRead(POT);
  fogo.potencia = 0;
  fogo.r = 0;
  fogo.g = 0;
  fogo.b = 0;
  fogo.zona = FOGO_OFF;

  if (fogo.pot < 250) {
    return fogo;
  }

  fogo.potencia = map(fogo.pot, 250, 4095, 0, 255);
  fogo.potencia = constrain(fogo.potencia, 0, 255);

  if (fogo.potencia < 85) {
    fogo.r = map(fogo.potencia, 0, 84, 90, 180);
    fogo.g = map(fogo.potencia, 0, 84, 7, 18);
    fogo.b = 0;
    fogo.zona = FOGO_BAIXO;
  }
  else if (fogo.potencia < 170) {
    fogo.r = map(fogo.potencia, 85, 169, 180, 255);
    fogo.g = map(fogo.potencia, 85, 169, 18, 25);
    fogo.b = 0;
    fogo.zona = FOGO_MEDIO;
  }
  else if (fogo.potencia < 220) {
    fogo.r = 255;
    fogo.g = map(fogo.potencia, 170, 219, 30, 12);
    fogo.b = 0;
    fogo.zona = FOGO_ALTO;
  }
  else if (fogo.potencia < 245) {
    fogo.r = 255;
    fogo.g = map(fogo.potencia, 220, 244, 12, 0);
    fogo.b = 0;
    fogo.zona = FOGO_MUITO_ALTO;
  }
  else {
    fogo.r = 255;
    fogo.g = 0;
    fogo.b = map(fogo.potencia, 245, 255, 1, 7);
    fogo.zona = FOGO_PERIGO;
  }

  return fogo;
}

void aplicarFogoAnimado(DadosFogo fogo) {
  if (fogo.zona == FOGO_OFF) {
    apagarFogo();
    return;
  }

  if (millis() - ultimaVariacaoChama >= 55) {
    ultimaVariacaoChama = millis();
    brilhoChama = random(84, 101);

    if (random(0, 100) < 10) {
      brilhoChama = random(72, 84);
    }
  }

  int r = fogo.r * brilhoChama / 100;
  int g = fogo.g * brilhoChama / 100;
  int b = fogo.b * brilhoChama / 100;

  corRGB(r, g, b);
}

void atualizarAlarme(ZonaFogo zona) {
  if (zona == FOGO_PERIGO) {
    if (millis() - ultimoAlarme >= 120) {
      ultimoAlarme = millis();
      alarmeLigado = !alarmeLigado;
      if (alarmeLigado) buzzerOn();
      else buzzerOff();
    }
    return;
  }

  buzzerOff();
  alarmeLigado = false;

  if (zona == FOGO_ALTO || zona == FOGO_MUITO_ALTO) {
    if (millis() - ultimoAviso >= 900) {
      ultimoAviso = millis();
      bip(50);
    }
  }
}

bool clicou(int pino) {
  if (digitalRead(pino) == LOW) {
    delay(25);
    if (digitalRead(pino) == LOW) {
      while (digitalRead(pino) == LOW) {
        delay(5);
      }
      return true;
    }
  }
  return false;
}

void desenharBitmapVertical(int x0, int y0, const unsigned char* bitmap, int largura, int altura) {
  int paginas = (altura + 7) / 8;

  for (int pagina = 0; pagina < paginas; pagina++) {
    for (int x = 0; x < largura; x++) {
      byte valor = pgm_read_byte(&bitmap[pagina * largura + x]);

      for (int bit = 0; bit < 8; bit++) {
        int y = pagina * 8 + bit;
        if (y >= altura) continue;

        if (valor & (1 << bit)) {
          display.drawPixel(x0 + x, y0 + y, SSD1306_WHITE);
        }
      }
    }
  }
}

void desenharGota(int x, int y) {
  display.drawPixel(x + 2, y, SSD1306_WHITE);
  display.drawPixel(x + 1, y + 1, SSD1306_WHITE);
  display.drawPixel(x + 2, y + 1, SSD1306_WHITE);
  display.drawPixel(x + 3, y + 1, SSD1306_WHITE);
  display.drawPixel(x, y + 2, SSD1306_WHITE);
  display.drawPixel(x + 1, y + 2, SSD1306_WHITE);
  display.drawPixel(x + 2, y + 2, SSD1306_WHITE);
  display.drawPixel(x + 3, y + 2, SSD1306_WHITE);
  display.drawPixel(x + 4, y + 2, SSD1306_WHITE);
  display.drawPixel(x + 1, y + 3, SSD1306_WHITE);
  display.drawPixel(x + 2, y + 3, SSD1306_WHITE);
  display.drawPixel(x + 3, y + 3, SSD1306_WHITE);
}

void desenharGato(TipoGato tipo, bool gota) {
  if (tipo == GATO_DORMINDO) {
    desenharBitmapVertical(89, 34, gatoDormindo, gatoDormindo_width, gatoDormindo_height);
  }
  else if (tipo == GATO_ATENTO) {
    desenharBitmapVertical(97, 27, gatoAtento, gatoAtento_width, gatoAtento_height);
    if (gota) desenharGota(91, 26);
  }
  else if (tipo == GATO_ASSUSTADO) {
    desenharBitmapVertical(88, 33, gatoAssustado, gatoAssustado_width, gatoAssustado_height);
    if (gota) desenharGota(84, 28);
  }
  else {
    desenharBitmapVertical(93, 35, gatoMega, gatoMega_width, gatoMega_height);
    if (gota) desenharGota(87, 29);
  }
}

void printLimitado(const char* texto, int maxChars) {
  if (texto == nullptr) return;
  for (int i = 0; texto[i] != '\0' && i < maxChars; i++) {
    display.write(texto[i]);
  }
}

void cabecalho(const char* texto) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  printLimitado(texto, 20);
}

void telaBase(const char* titulo, const char* l1, const char* l2, const char* l3, TipoGato gato, bool gota) {
  display.clearDisplay();
  cabecalho(titulo);

  display.setCursor(3, 18);
  printLimitado(l1, 13);

  display.setCursor(3, 30);
  printLimitado(l2, 13);

  display.setCursor(3, 42);
  printLimitado(l3, 13);

  desenharGato(gato, gota);
  display.display();
}

void telaComBarra(const char* titulo, const char* l1, const char* l2, const char* l3,
                  TipoGato gato, bool gota, unsigned long inicio, unsigned long total) {
  display.clearDisplay();
  cabecalho(titulo);

  display.setCursor(3, 18);
  printLimitado(l1, 13);

  display.setCursor(3, 30);
  printLimitado(l2, 13);

  display.setCursor(3, 42);
  printLimitado(l3, 13);

  desenharGato(gato, gota);

  display.drawRect(3, 54, 76, 8, SSD1306_WHITE);

  if (inicio > 0 && total > 0) {
    unsigned long tempo = millis() - inicio;
    if (tempo > total) tempo = total;

    int largura = map((long)tempo, 0, (long)total, 0, 74);
    largura = constrain(largura, 0, 74);
    display.fillRect(4, 55, largura, 6, SSD1306_WHITE);
  }

  display.display();
}

void mostrarMensagem(const char* titulo, const char* l1, const char* l2, int tempo, TipoGato gato) {
  telaBase(titulo, l1, l2, "", gato, false);
  delay(tempo);
}

void telaAbertura() {
  telaBase("GATINHO ENG.", "OLA!", "CASA ONLINE", "VAMOS BRINCAR", GATO_ATENTO, false);
  bipDuplo();
  delay(1400);
}

bool fogoNaFaixa(DadosFogo fogo, ZonaFogo alvo) {
  if (alvo == FOGO_MEDIO) {
    return fogo.potencia >= 105 && fogo.potencia <= 155;
  }

  if (alvo == FOGO_BAIXO) {
    return fogo.potencia >= 25 && fogo.potencia <= 80;
  }

  return false;
}

void feedbackFogo(DadosFogo fogo, ZonaFogo alvo, const char*& texto, TipoGato& gato, bool& gota) {
  gota = false;

  if (fogo.zona == FOGO_OFF) {
    texto = "FOGO APAGOU";
    gato = GATO_DORMINDO;
    return;
  }

  if (fogo.zona == FOGO_PERIGO) {
    texto = "PERIGO!";
    gato = GATO_MEGA;
    gota = true;
    return;
  }

  if (fogo.zona == FOGO_MUITO_ALTO) {
    texto = "MUITO ALTO!";
    gato = GATO_MEGA;
    gota = true;
    return;
  }

  int minimo = (alvo == FOGO_MEDIO) ? 105 : 25;
  int maximo = (alvo == FOGO_MEDIO) ? 155 : 80;

  if (fogo.potencia < minimo) {
    texto = "AUMENTE FOGO";
    gato = GATO_ATENTO;
    gota = true;
  }
  else if (fogo.potencia > maximo) {
    texto = "DIMINUA FOGO";
    gato = GATO_ASSUSTADO;
    gota = true;
  }
  else {
    texto = "PERFEITO!";
    gato = GATO_ATENTO;
  }
}

bool manterFaixa(DadosFogo fogo, ZonaFogo alvo, unsigned long tempoNecessario) {
  if (fogoNaFaixa(fogo, alvo)) {
    if (inicioFaixa == 0) inicioFaixa = millis();

    if (millis() - inicioFaixa >= tempoNecessario) {
      inicioFaixa = 0;
      return true;
    }
  }
  else {
    inicioFaixa = 0;
  }

  return false;
}

void carregarProgresso() {
  for (int i = 0; i < TOTAL_RECEITAS; i++) {
    char chaveU[5];
    char chaveD[5];
    snprintf(chaveU, sizeof(chaveU), "u%d", i);
    snprintf(chaveD, sizeof(chaveD), "d%d", i);

    receitaDesbloqueada[i] = prefs.getBool(chaveU, false);
    receitaConcluida[i] = prefs.getBool(chaveD, false);

    if (receitaConcluida[i]) receitaDesbloqueada[i] = true;
  }
}

void salvarUnlock(int receita) {
  receitaDesbloqueada[receita] = true;
  char chave[5];
  snprintf(chave, sizeof(chave), "u%d", receita);
  prefs.putBool(chave, true);
}

void salvarConclusao(int receita) {
  receitaDesbloqueada[receita] = true;
  receitaConcluida[receita] = true;

  char chaveU[5];
  char chaveD[5];
  snprintf(chaveU, sizeof(chaveU), "u%d", receita);
  snprintf(chaveD, sizeof(chaveD), "d%d", receita);

  prefs.putBool(chaveU, true);
  prefs.putBool(chaveD, true);
}

void resetarProgresso() {
  prefs.clear();
  for (int i = 0; i < TOTAL_RECEITAS; i++) {
    receitaDesbloqueada[i] = false;
    receitaConcluida[i] = false;
  }
}

bool todasReceitasConcluidas() {
  for (int i = 0; i < TOTAL_RECEITAS; i++) {
    if (!receitaConcluida[i]) return false;
  }
  return true;
}

void atualizarMenuPrincipal() {
  apagarFogo();
  buzzerOff();

  display.clearDisplay();
  cabecalho("GATINHO ENG.");

  display.setCursor(3, 21);
  display.print(opcaoMenuPrincipal == 0 ? "> RECEITAS" : "  RECEITAS");

  display.setCursor(3, 35);
  display.print(opcaoMenuPrincipal == 1 ? "> MODO LIVRE" : "  MODO LIVRE");

  display.setCursor(3, 51);
  display.print(todasReceitasConcluidas() ? "LIVRE: OK" : "LIVRE: BLOQ.");

  desenharGato(GATO_ATENTO, false);
  display.display();

  if (clicou(BOTAO_1)) {
    opcaoMenuPrincipal--;
    if (opcaoMenuPrincipal < 0) opcaoMenuPrincipal = 1;
    bip(35);
  }

  if (clicou(BOTAO_3)) {
    opcaoMenuPrincipal++;
    if (opcaoMenuPrincipal > 1) opcaoMenuPrincipal = 0;
    bip(35);
  }

  if (clicou(BOTAO_2)) {
    bip(45);

    if (opcaoMenuPrincipal == 0) {
      receitaSelecionada = 0;
      estadoAtual = MENU_RECEITAS;
    }
    else {
      estadoAtual = MODO_LIVRE;
    }
  }
}

void atualizarMenuReceitas() {
  apagarFogo();
  buzzerOff();

  display.clearDisplay();
  cabecalho("RECEITAS");

  if (receitaSelecionada == TOTAL_RECEITAS) {
    display.setCursor(3, 24);
    display.print("> VOLTAR");
    display.setCursor(3, 40);
    display.print("OK = MENU");
    desenharGato(GATO_ATENTO, false);
  }
  else {
    display.setCursor(3, 18);
    display.print(receitaSelecionada + 1);
    display.print("/6 ");
    printLimitado(NOMES_RECEITAS[receitaSelecionada], 10);

    display.setCursor(3, 34);
    if (receitaConcluida[receitaSelecionada]) display.print("CONCLUIDA!");
    else if (receitaDesbloqueada[receitaSelecionada]) display.print("LIBERADA");
    else display.print("BLOQUEADA");

    display.setCursor(3, 51);
    display.print("<  OK  >");
    desenharGato(GATO_ATENTO, false);
  }

  display.display();

  if (clicou(BOTAO_1)) {
    receitaSelecionada--;
    if (receitaSelecionada < 0) receitaSelecionada = TOTAL_RECEITAS;
    bip(35);
  }

  if (clicou(BOTAO_3)) {
    receitaSelecionada++;
    if (receitaSelecionada > TOTAL_RECEITAS) receitaSelecionada = 0;
    bip(35);
  }

  if (clicou(BOTAO_2)) {
    bip(45);

    if (receitaSelecionada == TOTAL_RECEITAS) {
      estadoAtual = MENU_PRINCIPAL;
      return;
    }

    if (receitaDesbloqueada[receitaSelecionada]) {
      iniciarReceita(receitaSelecionada);
    }
    else {
      codigoDigitado = "";
      estadoAtual = DIGITAR_CODIGO;
    }
  }
}

void atualizarTelaCodigo() {
  apagarFogo();
  buzzerOff();

  int digito = map(analogRead(POT), 0, 4095, 0, 9);
  digito = constrain(digito, 0, 9);

  int tamanhoCodigo = strlen(CODIGOS_RECEITAS[receitaSelecionada]);

  display.clearDisplay();
  cabecalho(NOMES_RECEITAS[receitaSelecionada]);

  display.setCursor(3, 18);
  display.print("CODIGO:");

  display.setCursor(3, 30);
  display.print(codigoDigitado);
  for (int i = codigoDigitado.length(); i < tamanhoCodigo; i++) {
    display.print("_");
  }

  display.setCursor(3, 45);
  display.print("NUM: ");
  display.setTextSize(2);
  display.print(digito);
  display.setTextSize(1);

  desenharGato(GATO_ATENTO, false);
  display.display();

  if (clicou(BOTAO_1)) {
    if (codigoDigitado.length() > 0) {
      codigoDigitado.remove(codigoDigitado.length() - 1);
      bip(35);
    }
    else {
      estadoAtual = MENU_RECEITAS;
    }
  }

  if (clicou(BOTAO_2)) {
    if ((int)codigoDigitado.length() < tamanhoCodigo) {
      codigoDigitado += char('0' + digito);
      bip(35);
    }
  }

  if (clicou(BOTAO_3)) {
    if ((int)codigoDigitado.length() != tamanhoCodigo) {
      bipTriplo();
      mostrarMensagem("CODIGO", "INCOMPLETO!", "TENTE DE NOVO", 850, GATO_ASSUSTADO);
      return;
    }

    if (codigoDigitado == CODIGOS_RECEITAS[receitaSelecionada]) {
      salvarUnlock(receitaSelecionada);
      somVitoria();
      mostrarMensagem(NOMES_RECEITAS[receitaSelecionada], "CODIGO CERTO!", "RECUPERADA!", 1200, GATO_ATENTO);
      iniciarReceita(receitaSelecionada);
    }
    else {
      bipTriplo();
      mostrarMensagem(NOMES_RECEITAS[receitaSelecionada], "CODIGO ERRADO", "VEJA O LIVRO", 1100, GATO_ASSUSTADO);
      codigoDigitado = "";
    }
  }
}

void iniciarReceita(int receita) {
  receitaEmJogo = receita;
  faseReceita = RECEITA_RECAP;
  passoRecap = 0;

  inicioFaixa = 0;
  inicioDesligado = 0;
  inicioAcerto = 0;

  apagarFogo();
  buzzerOff();

  estadoPanqueca = PAN_ZERO;
  estadoBrigadeiro = BRI_ZERO;
  estadoSopa = SOP_ZERO;
  estadoBolo = BOL_PERGUNTAS;
  estadoOmelete = OME_PREPARAR;
  estadoMingau = MIN_DEBUG;

  mexidasBrigadeiro = 0;
  brigadeiroPodeMexer = false;

  cenarioSopa = 0;
  avisoSopaTocado = false;

  perguntaBolo = 0;

  etapaCor = 0;
  canalR = false;
  canalG = false;
  canalB = false;

  passoDebug = 0;
  mexidasMingau = 0;
  mingauPodeMexer = false;

  estadoAtual = JOGAR_RECEITA;
}

void entrarJogoReceita() {
  faseReceita = RECEITA_JOGO;
  inicioFaixa = 0;
  inicioDesligado = 0;
  inicioAcerto = 0;
  apagarFogo();
  buzzerOff();
  bipDuplo();
}

void atualizarRecapReceita() {
  apagarFogo();
  buzzerOff();

  int total = RECAP_TOTAL[receitaEmJogo];

  if (passoRecap < total) {
    display.clearDisplay();
    cabecalho(NOMES_RECEITAS[receitaEmJogo]);

    char etapa[14];
    if (receitaEmJogo == 3) {
      snprintf(etapa, sizeof(etapa), "VALOR %d/%d", passoRecap + 1, total);
    }
    else if (receitaEmJogo == 4) {
      snprintf(etapa, sizeof(etapa), "COR %d/%d", passoRecap + 1, total);
    }
    else {
      snprintf(etapa, sizeof(etapa), "PASSO %d/%d", passoRecap + 1, total);
    }

    const char* statusRecap = "SEQUENCIA OK";
    if (receitaEmJogo == 3) statusRecap = "VALORES OK";
    if (receitaEmJogo == 4) statusRecap = "CORES OK";
    if (receitaEmJogo == 5) statusRecap = "BUG REMOVIDO";

    display.setCursor(3, 16);
    printLimitado(statusRecap, 13);

    display.setCursor(3, 27);
    printLimitado(etapa, 13);

    display.setCursor(3, 39);
    printLimitado(RECAP_L1[receitaEmJogo][passoRecap], 13);

    display.setCursor(3, 50);
    printLimitado(RECAP_L2[receitaEmJogo][passoRecap], 13);

    desenharGato(GATO_ATENTO, false);
    display.display();

    if (clicou(BOTAO_1)) {
      if (passoRecap > 0) {
        passoRecap--;
        bip(35);
      }
      else {
        estadoAtual = MENU_RECEITAS;
      }
    }

    if (clicou(BOTAO_3)) {
      passoRecap++;
      bip(35);
    }

    return;
  }

  telaBase(
    NOMES_RECEITAS[receitaEmJogo],
    "RECEITA",
    "RECUPERADA!",
    "OK = FAZER",
    GATO_ATENTO,
    false
  );

  if (clicou(BOTAO_1)) {
    passoRecap = total - 1;
    bip(35);
  }

  if (clicou(BOTAO_2)) {
    entrarJogoReceita();
  }
}

void concluirReceita(int receita) {
  apagarFogo();
  buzzerOff();
  salvarConclusao(receita);
  somVitoria();

  mostrarMensagem(NOMES_RECEITAS[receita], "PRONTINHA!", "RECEITA SALVA", 1400, GATO_ATENTO);

  if (todasReceitasConcluidas()) {
    mostrarMensagem("MODO LIVRE", "DESBLOQUEADO!", "BOA!", 1300, GATO_ATENTO);
  }

  estadoAtual = MENU_RECEITAS;
}

void jogoPanqueca() {
  const char* fb = "";
  TipoGato gato = GATO_ATENTO;
  bool gota = false;

  if (estadoPanqueca == PAN_PREPARAR_MASSA) {
    apagarFogo();
    buzzerOff();
    telaBase("PANQUECA 2/10", "OVO + LEITE", "ADD FARINHA", "OK = MISTUREI", GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      mostrarMensagem("PANQUECA", "MASSA PRONTA", "VAMOS AO FOGO", 650, GATO_ATENTO);
      estadoPanqueca = PAN_BAIXO_INICIAL;
      bipDuplo();
    }
  }

  else {
    DadosFogo fogo = calcularFogo();
    aplicarFogoAnimado(fogo);
    atualizarAlarme(fogo.zona);

    if (estadoPanqueca == PAN_ZERO) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("PANQUECA 1/10", "VAMOS FAZER!", "COMECE EM 0", "TUDO PRONTO", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 600) {
          inicioDesligado = 0;
          estadoPanqueca = PAN_PREPARAR_MASSA;
          bipDuplo();
        }
      }
      else {
        inicioDesligado = 0;
        telaBase("PANQUECA 1/10", "VAMOS FAZER!", "COMECE EM 0", "GIRE ATE 0", GATO_ATENTO, true);
      }
    }

    else if (estadoPanqueca == PAN_BAIXO_INICIAL) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_BAIXO, 1200);
      telaComBarra("PANQUECA 3/10", "LIGUE FOGO", "ALVO: BAIXO", fb, gato, gota, inicioFaixa, 1200);

      if (pronto) {
        estadoPanqueca = PAN_AQUECER;
        bipDuplo();
      }
    }

    else if (estadoPanqueca == PAN_AQUECER) {
      feedbackFogo(fogo, FOGO_MEDIO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_MEDIO, 3000);
      telaComBarra("PANQUECA 4/10", "AQUECA FRIG.", "ALVO: MEDIO", fb, gato, gota, inicioFaixa, 3000);

      if (pronto) {
        mostrarMensagem("PANQUECA", "FRIGIDEIRA", "QUENTINHA!", 650, GATO_ATENTO);
        estadoPanqueca = PAN_MASSA;
        bipDuplo();
      }
    }

    else if (estadoPanqueca == PAN_MASSA) {
      feedbackFogo(fogo, FOGO_MEDIO, fb, gato, gota);

      if (fogoNaFaixa(fogo, FOGO_MEDIO)) {
        telaBase("PANQUECA 5/10", "COLOQUE MASSA", "NA FRIGIDEIRA", "OK = COLOQ.", GATO_ATENTO, false);
      }
      else {
        telaBase("PANQUECA 5/10", "COLOQUE MASSA", "ALVO: MEDIO", fb, gato, gota);
      }

      if (clicou(BOTAO_2)) {
        if (fogoNaFaixa(fogo, FOGO_MEDIO)) {
          estadoPanqueca = PAN_COZINHAR_1;
          inicioFaixa = 0;
          bipDuplo();
        }
        else {
          bipTriplo();
        }
      }
    }

    else if (estadoPanqueca == PAN_COZINHAR_1) {
      feedbackFogo(fogo, FOGO_MEDIO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_MEDIO, 4000);
      telaComBarra("PANQUECA 6/10", "ESPERE BOLHAS", "FOGO MEDIO", fb, gato, gota, inicioFaixa, 4000);

      if (pronto) {
        mostrarMensagem("PANQUECA", "TEM BOLHINHAS", "HORA DE VIRAR", 600, GATO_ATENTO);
        estadoPanqueca = PAN_VIRAR;
        bipDuplo();
      }
    }

    else if (estadoPanqueca == PAN_VIRAR) {
      feedbackFogo(fogo, FOGO_MEDIO, fb, gato, gota);

      if (fogoNaFaixa(fogo, FOGO_MEDIO)) {
        telaBase("PANQUECA 7/10", "VIRE A MASSA", "COM CUIDADO", "OK = VIREI", GATO_ATENTO, false);
      }
      else {
        telaBase("PANQUECA 7/10", "VIRE A MASSA", "ALVO: MEDIO", fb, gato, gota);
      }

      if (clicou(BOTAO_2)) {
        if (fogoNaFaixa(fogo, FOGO_MEDIO)) {
          estadoPanqueca = PAN_COZINHAR_2;
          inicioFaixa = 0;
          bipDuplo();
        }
        else {
          bipTriplo();
        }
      }
    }

    else if (estadoPanqueca == PAN_COZINHAR_2) {
      feedbackFogo(fogo, FOGO_MEDIO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_MEDIO, 2200);
      telaComBarra("PANQUECA 8/10", "DOURE O LADO", "FOGO MEDIO", fb, gato, gota, inicioFaixa, 2200);

      if (pronto) {
        estadoPanqueca = PAN_REDUZIR;
        bipDuplo();
      }
    }

    else if (estadoPanqueca == PAN_REDUZIR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_BAIXO, 2500);
      telaComBarra("PANQUECA 9/10", "QUASE PRONTA", "ABAIXE FOGO", fb, gato, gota, inicioFaixa, 2500);

      if (pronto) {
        estadoPanqueca = PAN_RETIRAR;
        bipDuplo();
      }
    }

    else if (estadoPanqueca == PAN_RETIRAR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);

      if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
        telaBase("PANQUECA 10/10", "RETIRE PANQ.", "DA FRIGIDEIRA", "OK = RETIREI", GATO_ATENTO, false);
      }
      else {
        telaBase("PANQUECA 10/10", "RETIRE PANQ.", "FOGO BAIXO", fb, gato, gota);
      }

      if (clicou(BOTAO_2)) {
        if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
          estadoPanqueca = PAN_DESLIGAR;
          inicioDesligado = 0;
          bipDuplo();
        }
        else {
          bipTriplo();
        }
      }
    }

    else if (estadoPanqueca == PAN_DESLIGAR) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("PANQUECA", "PRONTINHA!", "FOGO DESLIG.", "ISSO!", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 700) {
          concluirReceita(0);
          return;
        }
      }
      else {
        inicioDesligado = 0;
        TipoGato g = (fogo.zona >= FOGO_MUITO_ALTO) ? GATO_MEGA :
                     (fogo.zona >= FOGO_ALTO) ? GATO_ASSUSTADO : GATO_ATENTO;
        telaBase("PANQUECA", "PRONTINHA!", "DESLIGUE FOGO", "GIRE ATE 0", g, fogo.zona >= FOGO_ALTO);
      }
    }
  }

  if (clicou(BOTAO_1)) {
    apagarFogo();
    buzzerOff();
    estadoAtual = MENU_RECEITAS;
  }
}

void jogoBrigadeiro() {
  const char* fb = "";
  TipoGato gato = GATO_ATENTO;
  bool gota = false;

  if (estadoBrigadeiro == BRI_PANELA) {
    apagarFogo();
    buzzerOff();
    telaBase("BRIGADEIRO 2/8", "PEGUE PANELA", "DEIXE PRONTA", "OK = PEGUEI", GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      estadoBrigadeiro = BRI_INGREDIENTES;
      bipDuplo();
    }
  }

  else if (estadoBrigadeiro == BRI_INGREDIENTES) {
    apagarFogo();
    buzzerOff();
    telaBase("BRIGADEIRO 3/8", "LEITE COND.", "CACAU+MANT.", "OK = COLOQ.", GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      mostrarMensagem("BRIGADEIRO", "INGREDIENTES", "NA PANELA!", 550, GATO_ATENTO);
      estadoBrigadeiro = BRI_BAIXO;
      bipDuplo();
    }
  }

  else {
    DadosFogo fogo = calcularFogo();
    aplicarFogoAnimado(fogo);
    atualizarAlarme(fogo.zona);

    if (estadoBrigadeiro == BRI_ZERO) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("BRIGADEIRO 1/8", "VAMOS FAZER!", "COMECE EM 0", "TUDO PRONTO", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 600) {
          inicioDesligado = 0;
          estadoBrigadeiro = BRI_PANELA;
          bipDuplo();
        }
      }
      else {
        inicioDesligado = 0;
        telaBase("BRIGADEIRO 1/8", "VAMOS FAZER!", "COMECE EM 0", "GIRE ATE 0", GATO_ATENTO, true);
      }
    }

    else if (estadoBrigadeiro == BRI_BAIXO) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_BAIXO, 1800);
      telaComBarra("BRIGADEIRO 4/8", "LIGUE FOGO", "ALVO: BAIXO", fb, gato, gota, inicioFaixa, 1800);

      if (pronto) {
        estadoBrigadeiro = BRI_MEXER;
        mexidasBrigadeiro = 0;
        brigadeiroPodeMexer = false;
        inicioFaixa = 0;
        bipDuplo();
      }
    }

    else if (estadoBrigadeiro == BRI_MEXER) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);

      if (!fogoNaFaixa(fogo, FOGO_BAIXO)) {
        brigadeiroPodeMexer = false;
        inicioFaixa = 0;
      }

      if (!brigadeiroPodeMexer) {
        bool estabilizou = manterFaixa(fogo, FOGO_BAIXO, 1100);

        char etapa[14];
        snprintf(etapa, sizeof(etapa), "MEXER %d/3", mexidasBrigadeiro + 1);

        telaComBarra("BRIGADEIRO 5/8", etapa, "FOGO BAIXO", fb, gato, gota, inicioFaixa, 1100);

        if (estabilizou) {
          brigadeiroPodeMexer = true;
          bip(45);
        }
      }
      else {
        char etapa[14];
        snprintf(etapa, sizeof(etapa), "MEXER %d/3", mexidasBrigadeiro + 1);

        telaBase("BRIGADEIRO 5/8", etapa, "SEM PARAR!", "OK = MEXER", GATO_ATENTO, false);

        if (clicou(BOTAO_2)) {
          mexidasBrigadeiro++;
          brigadeiroPodeMexer = false;
          inicioFaixa = 0;
          bipDuplo();

          if (mexidasBrigadeiro >= 3) {
            estadoBrigadeiro = BRI_ENGROSSAR;
          }
        }
      }
    }

    else if (estadoBrigadeiro == BRI_ENGROSSAR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_BAIXO, 3200);
      telaComBarra("BRIGADEIRO 6/8", "MEXA DEVAGAR", "ATE ENGROSSAR", fb, gato, gota, inicioFaixa, 3200);

      if (pronto) {
        estadoBrigadeiro = BRI_PONTO;
        bipDuplo();
      }
    }

    else if (estadoBrigadeiro == BRI_PONTO) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);

      if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
        telaBase("BRIGADEIRO 7/8", "SOLTOU FUNDO?", "PONTO CERTO?", "OK = SIM", GATO_ATENTO, false);
      }
      else {
        telaBase("BRIGADEIRO 7/8", "VEJA O PONTO", "FOGO BAIXO", fb, gato, gota);
      }

      if (clicou(BOTAO_2)) {
        if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
          estadoBrigadeiro = BRI_DESLIGAR;
          inicioDesligado = 0;
          bipDuplo();
        }
        else {
          bipTriplo();
        }
      }
    }

    else if (estadoBrigadeiro == BRI_DESLIGAR) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("BRIGADEIRO 8/8", "PONTO CERTO!", "FOGO DESLIG.", "ISSO!", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 700) {
          mostrarMensagem("BRIGADEIRO", "DEIXE ESFRIAR", "PRONTINHO!", 650, GATO_ATENTO);
          concluirReceita(1);
          return;
        }
      }
      else {
        inicioDesligado = 0;
        TipoGato g = (fogo.zona >= FOGO_MUITO_ALTO) ? GATO_MEGA :
                     (fogo.zona >= FOGO_ALTO) ? GATO_ASSUSTADO : GATO_ATENTO;
        telaBase("BRIGADEIRO 8/8", "PONTO CERTO!", "DESLIGUE FOGO", "GIRE ATE 0", g, fogo.zona >= FOGO_ALTO);
      }
    }
  }

  if (clicou(BOTAO_1)) {
    apagarFogo();
    buzzerOff();
    estadoAtual = MENU_RECEITAS;
  }
}

void jogoSopa() {
  const char* fb = "";
  TipoGato gato = GATO_ATENTO;
  bool gota = false;

  if (estadoSopa == SOP_PANELA) {
    apagarFogo();
    buzzerOff();
    telaBase("SOPA 2/8", "PEGUE PANELA", "DEIXE PRONTA", "OK = PEGUEI", GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      estadoSopa = SOP_INGREDIENTES;
      bipDuplo();
    }
  }

  else if (estadoSopa == SOP_INGREDIENTES) {
    apagarFogo();
    buzzerOff();
    telaBase("SOPA 3/8", "ADD LEGUMES", "E AGUA", "OK = PRONTO", GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      mostrarMensagem("SOPA", "PANELA PRONTA", "VAMOS AQUECER", 550, GATO_ATENTO);
      estadoSopa = SOP_AQUECER;
      bipDuplo();
    }
  }

  else {
    DadosFogo fogo = calcularFogo();
    aplicarFogoAnimado(fogo);
    atualizarAlarme(fogo.zona);

    if (estadoSopa == SOP_ZERO) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("SOPA 1/8", "VAMOS FAZER!", "COMECE EM 0", "TUDO PRONTO", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 600) {
          inicioDesligado = 0;
          estadoSopa = SOP_PANELA;
          bipDuplo();
        }
      }
      else {
        inicioDesligado = 0;
        telaBase("SOPA 1/8", "VAMOS FAZER!", "COMECE EM 0", "GIRE ATE 0", GATO_ATENTO, true);
      }
    }

    else if (estadoSopa == SOP_AQUECER) {
      feedbackFogo(fogo, FOGO_MEDIO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_MEDIO, 2400);
      telaComBarra("SOPA 4/8", "AQUECA SOPA", "ALVO: MEDIO", fb, gato, gota, inicioFaixa, 2400);

      if (pronto) {
        estadoSopa = SOP_MEXER;
        bipDuplo();
      }
    }

    else if (estadoSopa == SOP_MEXER) {
      feedbackFogo(fogo, FOGO_MEDIO, fb, gato, gota);

      if (fogoNaFaixa(fogo, FOGO_MEDIO)) {
        telaBase("SOPA 5/8", "MEXA A SOPA", "DEVAGAR", "OK = MEXI", GATO_ATENTO, false);
      }
      else {
        telaBase("SOPA 5/8", "MEXA A SOPA", "ALVO: MEDIO", fb, gato, gota);
      }

      if (clicou(BOTAO_2)) {
        if (fogoNaFaixa(fogo, FOGO_MEDIO)) {
          estadoSopa = SOP_CENARIOS;
          cenarioSopa = 0;
          avisoSopaTocado = false;
          inicioFaixa = 0;
          bipDuplo();
        }
        else {
          bipTriplo();
        }
      }
    }

    else if (estadoSopa == SOP_CENARIOS) {
      bool fervendo = sopaFervendo[cenarioSopa];
      ZonaFogo alvo = fervendo ? FOGO_BAIXO : FOGO_MEDIO;

      if (fervendo && !avisoSopaTocado) {
        bipTriplo();
        avisoSopaTocado = true;
      }

      feedbackFogo(fogo, alvo, fb, gato, gota);
      bool pronto = manterFaixa(fogo, alvo, 1800);

      char etapa[14];
      snprintf(etapa, sizeof(etapa), "CASO %d/4", cenarioSopa + 1);

      if (fervendo) {
        telaComBarra("SOPA 6/8", etapa, "FERVEU DEMAIS", fb, gato, gota, inicioFaixa, 1800);
      }
      else {
        telaComBarra("SOPA 6/8", etapa, "CONTINUE MED.", fb, gato, gota, inicioFaixa, 1800);
      }

      if (pronto) {
        cenarioSopa++;
        inicioFaixa = 0;
        avisoSopaTocado = false;
        bipDuplo();

        if (cenarioSopa >= 4) {
          estadoSopa = SOP_FINALIZAR;
        }
      }
    }

    else if (estadoSopa == SOP_FINALIZAR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);

      if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
        telaBase("SOPA 7/8", "TEMPERE SOPA", "PROVE O CALDO", "OK = PRONTO", GATO_ATENTO, false);
      }
      else {
        telaBase("SOPA 7/8", "FINALIZANDO", "ABAIXE FOGO", fb, gato, gota);
      }

      if (clicou(BOTAO_2)) {
        if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
          estadoSopa = SOP_DESLIGAR;
          inicioDesligado = 0;
          bipDuplo();
        }
        else {
          bipTriplo();
        }
      }
    }

    else if (estadoSopa == SOP_DESLIGAR) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("SOPA 8/8", "SOPA PRONTA!", "FOGO DESLIG.", "ISSO!", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 700) {
          concluirReceita(2);
          return;
        }
      }
      else {
        inicioDesligado = 0;
        TipoGato g = (fogo.zona >= FOGO_MUITO_ALTO) ? GATO_MEGA :
                     (fogo.zona >= FOGO_ALTO) ? GATO_ASSUSTADO : GATO_ATENTO;
        telaBase("SOPA 8/8", "SOPA PRONTA!", "DESLIGUE FOGO", "GIRE ATE 0", g, fogo.zona >= FOGO_ALTO);
      }
    }
  }

  if (clicou(BOTAO_1)) {
    apagarFogo();
    buzzerOff();
    estadoAtual = MENU_RECEITAS;
  }
}

void jogoBolo() {
  const char* fb = "";
  TipoGato gato = GATO_ATENTO;
  bool gota = false;

  if (estadoBolo == BOL_PERGUNTAS) {
    apagarFogo();
    buzzerOff();

    int valor = map(analogRead(POT), 0, 4095, 0, 9);
    valor = constrain(valor, 0, 9);

    char linha3[14];
    snprintf(linha3, sizeof(linha3), "VALOR: %d OK", valor);

    telaBase("BOLO 1/10", boloLinha1[perguntaBolo], boloLinha2[perguntaBolo], linha3, GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      if (valor == boloResposta[perguntaBolo]) {
        bipDuplo();
        mostrarMensagem("BOLO NUMEROS", "MEDIDA CERTA", "VALOR SALVO", 500, GATO_ATENTO);
        perguntaBolo++;

        if (perguntaBolo >= 4) {
          estadoBolo = BOL_JUNTAR;
        }
      }
      else {
        bipTriplo();
        mostrarMensagem("BOLO NUMEROS", "VALOR ERRADO", "TENTE DE NOVO", 600, GATO_ASSUSTADO);
      }
    }
  }

  else if (estadoBolo == BOL_JUNTAR) {
    apagarFogo();
    buzzerOff();
    telaBase("BOLO 2/10", "JUNTE MEDIDAS", "NA TIGELA", "OK = JUNTEI", GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      estadoBolo = BOL_MISTURAR;
      bipDuplo();
    }
  }

  else if (estadoBolo == BOL_MISTURAR) {
    apagarFogo();
    buzzerOff();
    telaBase("BOLO 3/10", "MISTURE MASSA", "ATE FICAR LISA", "OK = PRONTO", GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      estadoBolo = BOL_FORMA;
      bipDuplo();
    }
  }

  else if (estadoBolo == BOL_FORMA) {
    apagarFogo();
    buzzerOff();
    telaBase("BOLO 4/10", "UNTE FRIGID.", "DEIXE PRONTA", "OK = PRONTO", GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      estadoBolo = BOL_ZERO;
      inicioDesligado = 0;
      bipDuplo();
    }
  }

  else {
    DadosFogo fogo = calcularFogo();
    aplicarFogoAnimado(fogo);
    atualizarAlarme(fogo.zona);

    if (estadoBolo == BOL_ZERO) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("BOLO 5/10", "HORA DO FOGO", "COMECE EM 0", "TUDO PRONTO", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 600) {
          inicioDesligado = 0;
          estadoBolo = BOL_PREAQUECER;
          bipDuplo();
        }
      }
      else {
        inicioDesligado = 0;
        telaBase("BOLO 5/10", "HORA DO FOGO", "COMECE EM 0", "GIRE ATE 0", GATO_ATENTO, true);
      }
    }

    else if (estadoBolo == BOL_PREAQUECER) {
      feedbackFogo(fogo, FOGO_MEDIO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_MEDIO, 1800);
      telaComBarra("BOLO 6/10", "PRE-AQUECA", "ALVO: MEDIO", fb, gato, gota, inicioFaixa, 1800);

      if (pronto) {
        estadoBolo = BOL_REDUZIR;
        bipDuplo();
      }
    }

    else if (estadoBolo == BOL_REDUZIR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_BAIXO, 1300);
      telaComBarra("BOLO 7/10", "AGORA ABAIXE", "ALVO: BAIXO", fb, gato, gota, inicioFaixa, 1300);

      if (pronto) {
        estadoBolo = BOL_COLOCAR;
        bipDuplo();
      }
    }

    else if (estadoBolo == BOL_COLOCAR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);

      if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
        telaBase("BOLO 8/10", "COLOQUE MASSA", "E TAMPE", "OK = PRONTO", GATO_ATENTO, false);
      }
      else {
        telaBase("BOLO 8/10", "COLOQUE MASSA", "FOGO BAIXO", fb, gato, gota);
      }

      if (clicou(BOTAO_2)) {
        if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
          estadoBolo = BOL_ASSAR;
          inicioFaixa = 0;
          bipDuplo();
        }
        else {
          bipTriplo();
        }
      }
    }

    else if (estadoBolo == BOL_ASSAR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_BAIXO, 4500);
      telaComBarra("BOLO 9/10", "DEIXE ASSAR", "FOGO BAIXO", fb, gato, gota, inicioFaixa, 4500);

      if (pronto) {
        estadoBolo = BOL_VERIFICAR;
        bipDuplo();
      }
    }

    else if (estadoBolo == BOL_VERIFICAR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);

      if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
        telaBase("BOLO 10/10", "CONFIRA BOLO", "ESTA FIRME?", "OK = SIM", GATO_ATENTO, false);
      }
      else {
        telaBase("BOLO 10/10", "CONFIRA BOLO", "FOGO BAIXO", fb, gato, gota);
      }

      if (clicou(BOTAO_2)) {
        if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
          estadoBolo = BOL_DESLIGAR;
          inicioDesligado = 0;
          bipDuplo();
        }
        else {
          bipTriplo();
        }
      }
    }

    else if (estadoBolo == BOL_DESLIGAR) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("BOLO NUMEROS", "BOLO PRONTO!", "FOGO DESLIG.", "ISSO!", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 700) {
          concluirReceita(3);
          return;
        }
      }
      else {
        inicioDesligado = 0;
        TipoGato g = (fogo.zona >= FOGO_MUITO_ALTO) ? GATO_MEGA :
                     (fogo.zona >= FOGO_ALTO) ? GATO_ASSUSTADO : GATO_ATENTO;
        telaBase("BOLO NUMEROS", "BOLO PRONTO!", "DESLIGUE FOGO", "GIRE ATE 0", g, fogo.zona >= FOGO_ALTO);
      }
    }
  }

  if (clicou(BOTAO_1)) {
    apagarFogo();
    buzzerOff();
    estadoAtual = MENU_RECEITAS;
  }
}

void jogoOmelete() {
  if (estadoOmelete == OME_PREPARAR) {
    buzzerOff();
    apagarFogo();
    telaBase("OMELETE 1/11", "QUEBRE OVOS", "MISTURE BEM", "OK = PRONTO", GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      estadoOmelete = OME_TEMPERO;
      bipDuplo();
    }
  }

  else if (estadoOmelete == OME_TEMPERO) {
    apagarFogo();
    buzzerOff();
    telaBase("OMELETE 2/11", "TEMPERE OVOS", "MISTURE MAIS", "OK = PRONTO", GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      mostrarMensagem("OMELETE RGB", "ANTES DO FOGO", "VAMOS AS CORES", 600, GATO_ATENTO);
      estadoOmelete = OME_CORES;
      bipDuplo();
    }
  }

  else if (estadoOmelete == OME_CORES) {
    buzzerOff();
    int canalSelecionado = map(analogRead(POT), 0, 4095, 0, 2);
    canalSelecionado = constrain(canalSelecionado, 0, 2);

    int r = canalR ? 255 : 0;
    int g = canalG ? 30 : 0;
    int b = canalB ? 35 : 0;

    if (!canalR && canalG && canalB) {
      g = 110;
      b = 55;
    }

    corRGB(r, g, b);

    char estado[14];
    snprintf(estado, sizeof(estado), "R%d G%d B%d", canalR ? 1 : 0, canalG ? 1 : 0, canalB ? 1 : 0);

    const char* seletor = (canalSelecionado == 0) ? "SEL:R OK=TOG" :
                          (canalSelecionado == 1) ? "SEL:G OK=TOG" : "SEL:B OK=TOG";

    telaBase("OMELETE 3/11", alvoCorNome[etapaCor], estado, seletor, GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      if (canalSelecionado == 0) canalR = !canalR;
      else if (canalSelecionado == 1) canalG = !canalG;
      else canalB = !canalB;

      bip(35);
      inicioAcerto = 0;
    }

    bool correto = false;
    if (etapaCor == 0) correto = canalR && canalG && !canalB;
    if (etapaCor == 1) correto = canalR && !canalG && canalB;
    if (etapaCor == 2) correto = !canalR && canalG && canalB;

    if (correto) {
      if (inicioAcerto == 0) inicioAcerto = millis();

      if (millis() - inicioAcerto >= 900) {
        bipDuplo();
        mostrarMensagem("OMELETE RGB", "COR CERTA!", alvoCorNome[etapaCor], 500, GATO_ATENTO);
        etapaCor++;
        inicioAcerto = 0;

        if (etapaCor >= 3) {
          estadoOmelete = OME_APAGAR;
        }
        else {
          canalR = false;
          canalG = false;
          canalB = false;
          apagarFogo();
        }
      }
    }
    else {
      inicioAcerto = 0;
    }
  }

  else if (estadoOmelete == OME_APAGAR) {
    buzzerOff();
    int canalSelecionado = map(analogRead(POT), 0, 4095, 0, 2);
    canalSelecionado = constrain(canalSelecionado, 0, 2);

    int r = canalR ? 255 : 0;
    int g = canalG ? 30 : 0;
    int b = canalB ? 35 : 0;
    corRGB(r, g, b);

    const char* seletor = (canalSelecionado == 0) ? "SEL:R OK=TOG" :
                          (canalSelecionado == 1) ? "SEL:G OK=TOG" : "SEL:B OK=TOG";

    telaBase("OMELETE 4/11", "CORES CERTAS", "APAGUE TUDO", seletor, GATO_ATENTO, false);

    if (clicou(BOTAO_2)) {
      if (canalSelecionado == 0) canalR = !canalR;
      else if (canalSelecionado == 1) canalG = !canalG;
      else canalB = !canalB;
      bip(35);
      inicioAcerto = 0;
    }

    bool tudoOff = !canalR && !canalG && !canalB;

    if (tudoOff) {
      if (inicioAcerto == 0) inicioAcerto = millis();

      if (millis() - inicioAcerto >= 700) {
        apagarFogo();
        inicioAcerto = 0;
        inicioDesligado = 0;
        estadoOmelete = OME_ZERO_FOGO;
        mostrarMensagem("OMELETE RGB", "CORES OK!", "HORA DE COZIN.", 650, GATO_ATENTO);
        bipDuplo();
      }
    }
    else {
      inicioAcerto = 0;
    }
  }

  else {
    DadosFogo fogo = calcularFogo();
    aplicarFogoAnimado(fogo);
    atualizarAlarme(fogo.zona);

    const char* fb = "";
    TipoGato gato = GATO_ATENTO;
    bool gota = false;

    if (estadoOmelete == OME_ZERO_FOGO) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("OMELETE 5/11", "HORA DO FOGO", "COMECE EM 0", "TUDO PRONTO", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 600) {
          inicioDesligado = 0;
          estadoOmelete = OME_BAIXO;
          bipDuplo();
        }
      }
      else {
        inicioDesligado = 0;
        telaBase("OMELETE 5/11", "HORA DO FOGO", "COMECE EM 0", "GIRE ATE 0", GATO_ATENTO, true);
      }
    }

    else if (estadoOmelete == OME_BAIXO) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_BAIXO, 1400);
      telaComBarra("OMELETE 6/11", "AQUECA FRIG.", "ALVO: BAIXO", fb, gato, gota, inicioFaixa, 1400);

      if (pronto) {
        estadoOmelete = OME_COLOCAR;
        bipDuplo();
      }
    }

    else if (estadoOmelete == OME_COLOCAR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);

      if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
        telaBase("OMELETE 7/11", "COLOQUE OVOS", "NA FRIGIDEIRA", "OK = COLOQ.", GATO_ATENTO, false);
      }
      else {
        telaBase("OMELETE 7/11", "COLOQUE OVOS", "FOGO BAIXO", fb, gato, gota);
      }

      if (clicou(BOTAO_2)) {
        if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
          estadoOmelete = OME_SUBIR_MEDIO;
          inicioFaixa = 0;
          bipDuplo();
        }
        else {
          bipTriplo();
        }
      }
    }

    else if (estadoOmelete == OME_SUBIR_MEDIO) {
      feedbackFogo(fogo, FOGO_MEDIO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_MEDIO, 1300);
      telaComBarra("OMELETE 8/11", "AGORA AUMENTE", "ALVO: MEDIO", fb, gato, gota, inicioFaixa, 1300);

      if (pronto) {
        estadoOmelete = OME_COZINHAR;
        bipDuplo();
      }
    }

    else if (estadoOmelete == OME_COZINHAR) {
      feedbackFogo(fogo, FOGO_MEDIO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_MEDIO, 2800);
      telaComBarra("OMELETE 9/11", "COZINHE OVOS", "FOGO MEDIO", fb, gato, gota, inicioFaixa, 2800);

      if (pronto) {
        estadoOmelete = OME_REDUZIR;
        bipDuplo();
      }
    }

    else if (estadoOmelete == OME_REDUZIR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_BAIXO, 1200);
      telaComBarra("OMELETE 10/11", "QUASE PRONTO", "ABAIXE FOGO", fb, gato, gota, inicioFaixa, 1200);

      if (pronto) {
        estadoOmelete = OME_DOBRAR;
        bipDuplo();
      }
    }

    else if (estadoOmelete == OME_DOBRAR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);

      if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
        telaBase("OMELETE 11/11", "DOBRE OMELETE", "COM CUIDADO", "OK = DOBREI", GATO_ATENTO, false);
      }
      else {
        telaBase("OMELETE 11/11", "DOBRE OMELETE", "FOGO BAIXO", fb, gato, gota);
      }

      if (clicou(BOTAO_2)) {
        if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
          estadoOmelete = OME_DESLIGAR;
          inicioDesligado = 0;
          bipDuplo();
        }
        else {
          bipTriplo();
        }
      }
    }

    else if (estadoOmelete == OME_DESLIGAR) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("OMELETE RGB", "OMELETE OK!", "FOGO DESLIG.", "ISSO!", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 700) {
          concluirReceita(4);
          return;
        }
      }
      else {
        inicioDesligado = 0;
        TipoGato g = (fogo.zona >= FOGO_MUITO_ALTO) ? GATO_MEGA :
                     (fogo.zona >= FOGO_ALTO) ? GATO_ASSUSTADO : GATO_ATENTO;
        telaBase("OMELETE RGB", "OMELETE OK!", "DESLIGUE FOGO", "GIRE ATE 0", g, fogo.zona >= FOGO_ALTO);
      }
    }
  }

  if (clicou(BOTAO_1)) {
    apagarFogo();
    buzzerOff();
    estadoAtual = MENU_RECEITAS;
  }
}

void jogoMingau() {
  if (estadoMingau == MIN_DEBUG) {
    apagarFogo();
    buzzerOff();

    char etapa[14];
    snprintf(etapa, sizeof(etapa), "TESTE %d/6", passoDebug + 1);

    telaBase("MINGAU BUG", etapa, debugComando[passoDebug], "OK USAR >BUG", GATO_ATENTO, false);

    bool respondeu = false;
    bool marcouBug = false;

    if (clicou(BOTAO_2)) {
      respondeu = true;
      marcouBug = false;
    }

    if (clicou(BOTAO_3)) {
      respondeu = true;
      marcouBug = true;
    }

    if (respondeu) {
      bool correto = (marcouBug == debugEhBug[passoDebug]);

      if (correto) {
        bipDuplo();
        mostrarMensagem("MINGAU BUG", marcouBug ? "BUG ACHADO!" : "COMANDO OK", "CONTINUE", 500, GATO_ATENTO);
        passoDebug++;

        if (passoDebug >= TOTAL_DEBUG) {
          estadoMingau = MIN_ZERO;
          inicioDesligado = 0;
          mostrarMensagem("MINGAU BUG", "ALGORITMO OK", "VAMOS COZINHAR", 700, GATO_ATENTO);
        }
      }
      else {
        bipTriplo();
        mostrarMensagem("MINGAU BUG", "OPA...", "REVISA ISSO", 600, GATO_ASSUSTADO);
      }
    }
  }

  else {
    DadosFogo fogo = calcularFogo();
    aplicarFogoAnimado(fogo);
    atualizarAlarme(fogo.zona);

    const char* fb = "";
    TipoGato gato = GATO_ATENTO;
    bool gota = false;

    if (estadoMingau == MIN_ZERO) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("MINGAU 1/7", "RECEITA CERTA", "COMECE EM 0", "TUDO PRONTO", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 600) {
          inicioDesligado = 0;
          estadoMingau = MIN_INGREDIENTES;
          bipDuplo();
        }
      }
      else {
        inicioDesligado = 0;
        telaBase("MINGAU 1/7", "RECEITA CERTA", "COMECE EM 0", "GIRE ATE 0", GATO_ATENTO, true);
      }
    }

    else if (estadoMingau == MIN_INGREDIENTES) {
      apagarFogo();
      buzzerOff();
      telaBase("MINGAU 2/7", "LEITE+AVEIA", "ADD ACUCAR", "OK = PRONTO", GATO_ATENTO, false);

      if (clicou(BOTAO_2)) {
        estadoMingau = MIN_BAIXO;
        bipDuplo();
      }
    }

    else if (estadoMingau == MIN_BAIXO) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_BAIXO, 1800);
      telaComBarra("MINGAU 3/7", "LIGUE FOGO", "ALVO: BAIXO", fb, gato, gota, inicioFaixa, 1800);

      if (pronto) {
        estadoMingau = MIN_MEXER;
        mexidasMingau = 0;
        mingauPodeMexer = false;
        inicioFaixa = 0;
        bipDuplo();
      }
    }

    else if (estadoMingau == MIN_MEXER) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);

      if (!fogoNaFaixa(fogo, FOGO_BAIXO)) {
        mingauPodeMexer = false;
        inicioFaixa = 0;
      }

      if (!mingauPodeMexer) {
        bool estabilizou = manterFaixa(fogo, FOGO_BAIXO, 900);

        char etapa[14];
        snprintf(etapa, sizeof(etapa), "MEXER %d/3", mexidasMingau + 1);

        telaComBarra("MINGAU 4/7", etapa, "FOGO BAIXO", fb, gato, gota, inicioFaixa, 900);

        if (estabilizou) {
          mingauPodeMexer = true;
          bip(45);
        }
      }
      else {
        char etapa[14];
        snprintf(etapa, sizeof(etapa), "MEXER %d/3", mexidasMingau + 1);

        telaBase("MINGAU 4/7", etapa, "SEM PARAR!", "OK = MEXER", GATO_ATENTO, false);

        if (clicou(BOTAO_2)) {
          mexidasMingau++;
          mingauPodeMexer = false;
          inicioFaixa = 0;
          bipDuplo();

          if (mexidasMingau >= 3) {
            estadoMingau = MIN_ENGROSSAR;
          }
        }
      }
    }

    else if (estadoMingau == MIN_ENGROSSAR) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);
      bool pronto = manterFaixa(fogo, FOGO_BAIXO, 3000);
      telaComBarra("MINGAU 5/7", "ATE ENGROSSAR", "FOGO BAIXO", fb, gato, gota, inicioFaixa, 3000);

      if (pronto) {
        estadoMingau = MIN_PONTO;
        bipDuplo();
      }
    }

    else if (estadoMingau == MIN_PONTO) {
      feedbackFogo(fogo, FOGO_BAIXO, fb, gato, gota);

      if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
        telaBase("MINGAU 6/7", "CONFIRA PONTO", "CREMOSO?", "OK = SIM", GATO_ATENTO, false);
      }
      else {
        telaBase("MINGAU 6/7", "CONFIRA PONTO", "FOGO BAIXO", fb, gato, gota);
      }

      if (clicou(BOTAO_2)) {
        if (fogoNaFaixa(fogo, FOGO_BAIXO)) {
          estadoMingau = MIN_DESLIGAR;
          inicioDesligado = 0;
          bipDuplo();
        }
        else {
          bipTriplo();
        }
      }
    }

    else if (estadoMingau == MIN_DESLIGAR) {
      if (fogo.zona == FOGO_OFF) {
        if (inicioDesligado == 0) inicioDesligado = millis();
        telaBase("MINGAU 7/7", "MINGAU PRONTO", "FOGO DESLIG.", "ISSO!", GATO_ATENTO, false);

        if (millis() - inicioDesligado >= 700) {
          estadoMingau = MIN_ESFRIAR;
          inicioDesligado = 0;
          bipDuplo();
        }
      }
      else {
        inicioDesligado = 0;
        TipoGato g = (fogo.zona >= FOGO_MUITO_ALTO) ? GATO_MEGA :
                     (fogo.zona >= FOGO_ALTO) ? GATO_ASSUSTADO : GATO_ATENTO;
        telaBase("MINGAU 7/7", "MINGAU PRONTO", "DESLIGUE FOGO", "GIRE ATE 0", g, fogo.zona >= FOGO_ALTO);
      }
    }

    else if (estadoMingau == MIN_ESFRIAR) {
      apagarFogo();
      buzzerOff();
      telaBase("MINGAU BUG", "DEIXE ESFRIAR", "ANTES SERVIR", "OK = PRONTO", GATO_ATENTO, false);

      if (clicou(BOTAO_2)) {
        concluirReceita(5);
        return;
      }
    }
  }

  if (clicou(BOTAO_1)) {
    apagarFogo();
    buzzerOff();
    estadoAtual = MENU_RECEITAS;
  }
}

void atualizarReceitaAtual() {
  if (faseReceita == RECEITA_RECAP) {
    atualizarRecapReceita();
    return;
  }

  switch (receitaEmJogo) {
    case 0: jogoPanqueca(); break;
    case 1: jogoBrigadeiro(); break;
    case 2: jogoSopa(); break;
    case 3: jogoBolo(); break;
    case 4: jogoOmelete(); break;
    case 5: jogoMingau(); break;
  }
}

void atualizarModoLivre() {
  if (!todasReceitasConcluidas()) {
    apagarFogo();
    buzzerOff();
    telaBase("MODO LIVRE", "BLOQUEADO", "COMPLETE AS 6", "RECEITAS", GATO_ATENTO, false);

    if (clicou(BOTAO_1) || clicou(BOTAO_2)) {
      estadoAtual = MENU_PRINCIPAL;
    }
    return;
  }

  DadosFogo fogo = calcularFogo();
  aplicarFogoAnimado(fogo);
  atualizarAlarme(fogo.zona);

  const char* nivel = "DESLIGADO";
  TipoGato gato = GATO_ATENTO;
  bool gota = false;

  if (fogo.zona == FOGO_OFF) {
    nivel = "DESLIGADO";
    gato = GATO_DORMINDO;
  }
  else if (fogo.zona == FOGO_BAIXO) {
    nivel = "FOGO BAIXO";
  }
  else if (fogo.zona == FOGO_MEDIO) {
    nivel = "FOGO MEDIO";
  }
  else if (fogo.zona == FOGO_ALTO) {
    nivel = "FOGO ALTO";
    gato = GATO_ASSUSTADO;
    gota = true;
  }
  else if (fogo.zona == FOGO_MUITO_ALTO) {
    nivel = "MUITO ALTO";
    gato = GATO_MEGA;
    gota = true;
  }
  else {
    nivel = "PERIGO!";
    gato = GATO_MEGA;
    gota = true;
  }

  telaBase("MODO LIVRE", "CONTROLE FOGO", nivel, "< = SAIR", gato, gota);

  if (clicou(BOTAO_1)) {
    apagarFogo();
    buzzerOff();
    estadoAtual = MENU_PRINCIPAL;
  }
}

void verificarResetProgresso() {
  if (digitalRead(BOTAO_1) == LOW && digitalRead(BOTAO_3) == LOW) {
    unsigned long inicio = millis();

    while (digitalRead(BOTAO_1) == LOW && digitalRead(BOTAO_3) == LOW) {
      if (millis() - inicio >= 2000) {
        resetarProgresso();
        apagarFogo();
        buzzerOff();
        bipTriplo();
        mostrarMensagem("PROGRESSO", "RESETADO!", "TUDO BLOQ.", 1200, GATO_ATENTO);

        while (digitalRead(BOTAO_1) == LOW || digitalRead(BOTAO_3) == LOW) {
          delay(10);
        }
        return;
      }
      delay(10);
    }
  }
}

void setup() {
  Serial.begin(57600);

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(BOTAO_1, INPUT_PULLUP);
  pinMode(BOTAO_2, INPUT_PULLUP);
  pinMode(BOTAO_3, INPUT_PULLUP);

  pinMode(BUZZER, OUTPUT);

  apagarFogo();
  buzzerOff();

  randomSeed(micros());

  prefs.begin("fogao", false);
  carregarProgresso();

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("ERRO: OLED NAO ENCONTRADO");
    while (true) delay(100);
  }

  display.setTextWrap(false);
  display.clearDisplay();
  display.display();

  verificarResetProgresso();
  telaAbertura();

  estadoAtual = MENU_PRINCIPAL;
}

void loop() {
  switch (estadoAtual) {
    case MENU_PRINCIPAL:
      atualizarMenuPrincipal();
      break;

    case MENU_RECEITAS:
      atualizarMenuReceitas();
      break;

    case DIGITAR_CODIGO:
      atualizarTelaCodigo();
      break;

    case JOGAR_RECEITA:
      atualizarReceitaAtual();
      break;

    case MODO_LIVRE:
      atualizarModoLivre();
      break;
  }

  delay(20);
}
