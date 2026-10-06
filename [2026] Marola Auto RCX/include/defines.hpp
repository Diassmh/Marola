//===============================================================================================//
//=====================================//INCLUDES E DEFINES//====================================//
//===============================================================================================//

#pragma region INCLUDES

#ifndef DEFINES_H
#define DEFINES_H

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <IRremote.hpp>
#include <ESP32Servo.h>
#include <FastLED.h>

#pragma endregion

//=====================================//Objetos e Handles//=====================================//

#pragma region OBJETOS E HANDLES

BluetoothSerial SerialBT;
Servo servoLaminas;

TaskHandle_t SensorTask; // Avisando o ESP que teremos uma task

//=========================================//Bluetooth//=========================================//

// Verifica se o Bluetooth esta devidamente habilitado no menuconfig

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif // Finaliza o condicional do erro

#pragma endregion

//===============================================================================================//
//=====================================//PINOS E VARIAVEIS//=====================================//
//===============================================================================================//

//==========================================//Motores//==========================================//

#pragma region PINOS MOTORES

#define Motor_Direito_Positivo 16  // 17    fuego 16
#define Motor_Direito_Negativo 17  // 16    fugego 17
#define Motor_Esquerdo_Positivo 19 // 18    fuegob 18
#define Motor_Esquerdo_Negativo 18 // 19   fuego 19

#pragma endregion

//========================================//Servomotor//========================================//

#pragma region PINOS SERVO

#define SERVOMOTOR_PIN 26 // 26
#define SERVO_ANGULO_INICIAL 96
#define SERVO_FREQ_HZ 50
#define SERVO_RESOLUTION 16
#define SERVO_MIN_PULSE_US 544  //  Largura de pulso para 0 graus (µs)
#define SERVO_MAX_PULSE_US 2400 //  Largura de pulso para 180 graus (µs)

#pragma endregion

//========================================//Comunicacao//========================================//

#pragma region COMUNICACAO

#define leds_end 33
#define led 2
#define IR 13

#define botao1 0x0
#define botao2 0x1
#define botao3 0x2 // Botões do controle remoto
#define botao8 0x7
#define botao9 0x8

#pragma endregion

//=======================================//Sensoreamento//=======================================//

#pragma region SENSOREAMENTO

#define linhaE 39   //
#define linhaD 34   //
#define pinoLDRE 36 //
#define pinoLDRF 27 //
#define pinoLDRD 14 //

#pragma endregion

//======================================//Enums//======================================//

#pragma region ENUMS

enum Direction
{
  esquerda,
  direita,
  frente
};

enum ServoMotor
{
  Fechado,
  Aberto
};

enum SelecionaBusca
{
  Tchola,
  Ofensiva
};

enum SelecionaModo
{
  NaoSelecionado,
  ModoAuto,
  ModoRc
};

enum Posicionamento
{
  Perto,
  Medio,
  Longe
};

enum EstadoMarola
{
  AVANCAR_LENTO,
  ATAQUE_RAPIDO,
  FUGA_LINHA_RE,
  FUGA_LINHA_GIRO,
  FUGA_LINHA_ABRIR
};

#pragma endregion

//===================================//Variaveis utilizadas//====================================//

#pragma region VARIAVEIS

Direction lado = esquerda;
ServoMotor servo = Fechado;
SelecionaBusca busca = Ofensiva;
SelecionaModo modoSelecionado = ModoAuto;
Posicionamento Posicao = Medio;
EstadoMarola estadoAtual = AVANCAR_LENTO;

int valorLinhaE, valorLinhaD;
int valorLDRE, valorLDRF, valorLDRD;

CRGB leds[5];

char BT = 'A';         // char que é recebida pelo bluetooth
char estrategia = 'A'; // estratégia de início da luta (qualquer caractere)
bool running_Sensors = false;
int num_led = 1;

#pragma endregion

#endif
