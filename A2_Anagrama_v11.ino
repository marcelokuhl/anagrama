// Criado por Marcelo Guilherme Kühl
// Anagrama ver 11
// 23/02/2025
// Componentes: Módulo Display 8x32
// botão tipo Arcade
// Placa Arduíno UNO
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <OneButton.h>
#include <stdlib.h>
#include <string.h>

// Definições do display
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4  // 8x32 = 4 módulos 8x8
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

// Definições do botão
#define BUTTON_PIN 2
OneButton button(BUTTON_PIN, true);

// Inicialização do display
MD_Parola matriz = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Lista de palavras do MannaBrax
const char* palavras[] = {"Paz", "Amor", "Manna", "LED", "JACK", "PIN", "Game"};
const int totalPalavras = sizeof(palavras) / sizeof(palavras[0]);

char anagrama[15];
char palavraFormada[15];
bool palavraConfirmada = false;
int indiceCursor = 0;
int palavraAtual;
int deslocamento = 0;  // Variável para controlar o deslocamento da palavra formada
int pontuacao = 0;  // Pontuação do jogador
bool palavrasUsadas[totalPalavras] = {false};  // Marca as palavras que já foram usadas

void embaralhar(char *str) {
    int len = strlen(str);
    for (int i = len - 1; i > 0; i--) {
        int j = random(i + 1);
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

void selecionarPalavra() {
    // Seleciona uma palavra aleatória que não foi usada
    do {
        palavraAtual = random(totalPalavras);
    } while (palavrasUsadas[palavraAtual]);

    strcpy(anagrama, palavras[palavraAtual]);
    embaralhar(anagrama);
    memset(palavraFormada, 0, sizeof(palavraFormada));
    indiceCursor = 0;
    deslocamento = 0;  // Reseta o deslocamento
    palavraConfirmada = false;

    // Marca a palavra como usada
    palavrasUsadas[palavraAtual] = true;

    matriz.displayClear();
    matriz.displayText(anagrama, PA_CENTER, 100, 2000, PA_PRINT, PA_NO_EFFECT);
    matriz.displayAnimate();
}

void atualizarDisplay() {
    // Ajusta a palavra formada para considerar o deslocamento
    char temp[30];
    snprintf(temp, sizeof(temp), "%s [%c]", palavraFormada, anagrama[indiceCursor]);

    // Mover a palavra formada para a esquerda se necessário
    if (strlen(palavraFormada) >= 8) {
        deslocamento = (strlen(palavraFormada) - 7);  // Ajuste o deslocamento
    } else {
        deslocamento = 0;
    }

    // Exibe a palavra formada com o deslocamento
    matriz.displayClear();
    matriz.displayText(temp + deslocamento, PA_CENTER, 100, 2000, PA_PRINT, PA_NO_EFFECT);
    matriz.displayAnimate();
}

void avancarCursor() {
    indiceCursor = (indiceCursor + 1) % strlen(anagrama);
    atualizarDisplay();
}

void selecionarLetra() {
    int len = strlen(palavraFormada);
    if (len < sizeof(palavraFormada) - 1) {
        palavraFormada[len] = anagrama[indiceCursor];
        palavraFormada[len + 1] = '\0';
    }
    atualizarDisplay();

    if (strcmp(palavraFormada, palavras[palavraAtual]) == 0) {
        // Acertou a palavra
        pontuacao++;
        matriz.displayClear();
        matriz.displayText("Acertou! ", PA_CENTER, 100, 1000, PA_PRINT, PA_NO_EFFECT);
        matriz.displayText(palavraFormada, PA_CENTER, 100, 2000, PA_PRINT, PA_NO_EFFECT);
        matriz.displayAnimate();
        delay(2000);  // Delay para mostrar a palavra acertada
        if (pontuacao == totalPalavras) {
            matriz.displayClear();
            matriz.displayText("WINNER", PA_CENTER, 100, 2000, PA_PRINT, PA_NO_EFFECT);
            matriz.displayAnimate();
        } else {
            selecionarPalavra();
        }
    } else if (strlen(palavraFormada) >= strlen(palavras[palavraAtual])) {
        // Errou a palavra
        matriz.displayClear();
        matriz.displayText("Errou!", PA_CENTER, 100, 1000, PA_PRINT, PA_NO_EFFECT);
        matriz.displayText(palavras[palavraAtual], PA_CENTER, 100, 2000, PA_PRINT, PA_NO_EFFECT);
        matriz.displayAnimate();
        delay(2000);  // Delay para mostrar a palavra errada
        selecionarPalavra();
    }
}

void mostrarPontuacao() {
    char pontuacaoTexto[20];
    snprintf(pontuacaoTexto, sizeof(pontuacaoTexto), "Pontos: %d", pontuacao);
    matriz.displayClear();
    matriz.displayText(pontuacaoTexto, PA_CENTER, 100, 2000, PA_PRINT, PA_NO_EFFECT);
    matriz.displayAnimate();
}

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    matriz.begin();
    matriz.setIntensity(3);
    matriz.displayClear();
    delay(100);
    randomSeed(analogRead(0));
    selecionarPalavra();
    button.attachClick(avancarCursor);
    button.attachDoubleClick(selecionarLetra); // Alterado para duplo clique
}

void loop() {
    button.tick();  // Verifica o estado do botão
    if (pontuacao < totalPalavras) {
        mostrarPontuacao();
    } else {
        // Se o jogador acertou todas as palavras, exibe o WINNER
        matriz.displayClear();
        matriz.displayText("WINNER!", PA_CENTER, 100, 2000, PA_PRINT, PA_NO_EFFECT);
        matriz.displayAnimate();
    }
    delay(100);  // Delay para evitar looping rápido e permitir tempo para ver a pontuação
}
