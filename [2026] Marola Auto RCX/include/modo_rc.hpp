//===============================================================================================//
//======================================// MODO RC //============================================//
//===============================================================================================//

#ifndef MODO_RC_HPP
#define MODO_RC_HPP

#pragma region INCLUDES

#include "defines.hpp"
#include "move.hpp"
#include <PS4Controller.h>

#pragma endregion

int zonaMorta = 20;                // Zona desconsiderada do controle de PS4
volatile int r2 = 0;               // Valor do gatilho direito do PS4 (R2)
volatile int l2 = 0;               // Valor do gatilho esquerdo do PS4 (L2)
volatile int direcao = 0;          // Valor do direcional esquerdo do PS4 em X (LStickX)
volatile bool velLimitada = false; // Switch definido pra limitar a velocidade do motor
int limiteVelocidade = 255;        // Limite de velocidade do motor

bool ativarMira = false;
bool primeiroComando = true;    // Flag para abrir o servo no primeiro movimento
bool estadoServoFechado = true; // Controla se o robô está fechado (95º) ou aberto (0º)

float coefAtenuacao = 1.8; // Atenuacao da velocidade em curvas (deve ser FLOAT)
float coefReverse = 0.9;   // Coeficiente para balancear a re (deve ser FLOAT)
float limiteCurva = 200.0; // Velocidade limite em curvas puras (deve ser FLOAT)  120

const char *MAROLA = "30:76:f5:93:b0:30";
const char *TESTE = "c8:85:41:2c:24:3c";

void iniciarTodosAzuis()
{
    fill_solid(leds, 5, CRGB::Blue);
    FastLED.show();
}

#pragma region Setup RC

void setupRC()
{
    iniciarTodosAzuis();

    if (PS4.begin(MAROLA))
    {
        while (!PS4.isConnected())
        {
            uint8_t brilhoPulsante = beatsin8(20, 40, 240);
            fill_solid(leds, 5, CHSV(160, 255, brilhoPulsante));
            FastLED.show();
            digitalWrite(led, LOW);
            vTaskDelay(pdMS_TO_TICKS(5));
        }
    }
}

#pragma endregion

#pragma region Loop RC

void loopRC()
{

    while (true)
    {
        if (PS4.isConnected())
        {
            TodosLedsVermelhos();
            digitalWrite(led, HIGH);
            int velocidadeEsquerda = 0; // Velocidade do motor esquerdo
            int velocidadeDireita = 0;  // Velocidade do motor direito

            // ======================= || FLAG PRIMEIRO COMANDO || =======================
            if (primeiroComando)
            {
                // Se detetar QUALQUER botão de macro, gatilho, curva ou frentão
                if (PS4.Cross() || PS4.L1() || PS4.R1() || PS4.Left() > 0 || PS4.Right() > 0 || PS4.Up() > 0 || r2 > zonaMorta || l2 > zonaMorta || abs(direcao) > zonaMorta)
                {
                    servoLaminas.write(0);
                    estadoServoFechado = false;
                    primeiroComando = false;
                }
            }

            // ======================= || COMANDOS COM BOTÕES (MACROS) || ===================

            if (PS4.Cross())
            { // X = Frentão
                moverMotores(255, 255);
                vTaskDelay(pdMS_TO_TICKS(50));
                continue;
            }

            else if (PS4.L1())
            {

                vTaskDelay(pdMS_TO_TICKS(10));
                moverMotores(-255, 255);
                vTaskDelay(pdMS_TO_TICKS(100));
                moverMotores(255, 255);
                vTaskDelay(pdMS_TO_TICKS(200));
                moverMotores(0, 0);
                vTaskDelay(pdMS_TO_TICKS(10));
                moverMotores(255, -255);
                vTaskDelay(pdMS_TO_TICKS(110));
                moverMotores(255, 255);
                vTaskDelay(pdMS_TO_TICKS(100));
            }

            else if (PS4.R1())
            {

                vTaskDelay(pdMS_TO_TICKS(10));
                moverMotores(255, -255);
                vTaskDelay(pdMS_TO_TICKS(100));
                moverMotores(255, 255);
                vTaskDelay(pdMS_TO_TICKS(200));
                moverMotores(0, 0);
                vTaskDelay(pdMS_TO_TICKS(10));
                moverMotores(-255, 255);
                vTaskDelay(pdMS_TO_TICKS(110));
                moverMotores(255, 255);
                vTaskDelay(pdMS_TO_TICKS(100));
            }

            else if (PS4.Left() > 0)
            {
                moverMotores(-255, 255);
                vTaskDelay(pdMS_TO_TICKS(40));
                moverMotores(255, 255);
                vTaskDelay(pdMS_TO_TICKS(155));
                moverMotores(255, -255);
                vTaskDelay(pdMS_TO_TICKS(190));
                moverMotores(255, 255);
                vTaskDelay(pdMS_TO_TICKS(170));
                moverMotores(0, 0);
                vTaskDelay(pdMS_TO_TICKS(15));
            }
            else if (PS4.Right() > 0)
            {
                moverMotores(255, -255);
                vTaskDelay(pdMS_TO_TICKS(40));
                moverMotores(255, 255);
                vTaskDelay(pdMS_TO_TICKS(155));
                moverMotores(-255, 255);
                vTaskDelay(pdMS_TO_TICKS(160));
                moverMotores(255, 255);
                vTaskDelay(pdMS_TO_TICKS(170));
                moverMotores(0, 0);
                vTaskDelay(pdMS_TO_TICKS(15));
            }

            // ======================= || CONTROLO DO SERVO MANUAL || =======================
            else if (PS4.Down() > 0)
            {
                if (estadoServoFechado)
                {
                    servoLaminas.write(0); // Se está fechado, abre para 0º
                    estadoServoFechado = false;
                }
                else
                {
                    servoLaminas.write(95); // Se está aberto, fecha para 95º
                    estadoServoFechado = true;
                }
                moverMotores(0, 0);             // Opcional: manter o robô parado ao alternar
                vTaskDelay(pdMS_TO_TICKS(300)); // Debounce essencial para não "piscar" o servo
                continue;
            }

            else if (PS4.Up() > 0)
            {
                moverMotores(0, 0);
                vTaskDelay(pdMS_TO_TICKS(25));
                moverMotores(255, -255);
                vTaskDelay(pdMS_TO_TICKS(140));
                moverMotores(0, 0);
                vTaskDelay(pdMS_TO_TICKS(10));
                continue;
            }
            else if (PS4.Square() && !ativarMira)
            {
                ativarMira = true;
                running_Sensors = true;
                vTaskDelay(pdMS_TO_TICKS(200));
            }
            else if (PS4.Square())
            {
                ativarMira = false;
                running_Sensors = false;
                vTaskDelay(pdMS_TO_TICKS(200));
            }

            // ======================= || LIMITADOR DE VELOCIDADE || =======================

            // Limita a velocidade a 127
            else if (PS4.Triangle() && !velLimitada)
            {
                velLimitada = true;
                limiteVelocidade = 160;         // Define o limite reduzido
                vTaskDelay(pdMS_TO_TICKS(200)); // Evita multiplas leituras
            }

            // Permite a velocidade de 255
            else if (PS4.Triangle())
            {
                velLimitada = false;
                limiteVelocidade = 255;         // Restaura o limite maximo
                vTaskDelay(pdMS_TO_TICKS(200)); // Evita multiplas leituras
            }

            // ======================= || MOVIMENTAÇÃO LIVRE || ==========================

            else
            {

                r2 = PS4.R2Value();      // Armazena o valor de R2
                l2 = PS4.L2Value();      // Armazena o valor de L2
                direcao = PS4.LStickX(); // Armazena o valor em X do Direcional Esquerdo

                // Se nenhum gatilho estiver pressionado (considerando a zona morta)

                //===================== || CURVAS PURAS || =============================

                if (r2 <= zonaMorta && l2 <= zonaMorta)
                {
                    velocidadeEsquerda = 0;
                    velocidadeDireita = 0;

                    if (direcao < -zonaMorta)
                    { // Curva pra esquerda parado
                        int velCurva = map(abs(direcao), zonaMorta, 128, 0, limiteCurva);
                        velocidadeEsquerda = -velCurva;
                        velocidadeDireita = velCurva;
                    }
                    else if (direcao > zonaMorta)
                    { // Curva pra direita parado
                        int velCurva = map(direcao, zonaMorta, 127, 0, limiteCurva);
                        velocidadeEsquerda = velCurva;
                        velocidadeDireita = -velCurva;
                    }
                }

                //===================== || GATILHOS PUROS || =============================

                else
                { // Pelo menos um gatilho pressionado (R2 ou L2)

                    if (r2 > zonaMorta) // Gatilho direito (frente)
                    {

                        int velocidade = map(r2, zonaMorta, 255, 0, limiteVelocidade);
                        velocidadeEsquerda = velocidade;
                        velocidadeDireita = velocidade;
                    }
                    else
                    { // Gatilho esquerdo (re)
                        int velocidade = map(l2, zonaMorta, 255, 0, limiteVelocidade);
                        velocidadeEsquerda = -velocidade;
                        velocidadeDireita = -int(coefReverse * velocidade);
                    }

                    //===================== || GATILHOS + CURVA || =============================

                    // Aplica a curva sobre a velocidade existente

                    if (direcao < -zonaMorta)
                    { // Curva pra esquerda em movimento
                        // Reduz a velocidade da roda interna (esquerda para frente, direita para re)
                        if (r2 > zonaMorta)
                            velocidadeEsquerda *= (1.0 - (abs(direcao) * coefAtenuacao) / 255.0);
                        else
                            velocidadeDireita *= (1.0 - (abs(direcao) * coefAtenuacao) / 255.0);
                    }
                    else if (direcao > zonaMorta)
                    { // Curva pra direita em movimento
                        // Reduz a velocidade da roda interna (direita para frente, esquerda para re)
                        if (r2 > zonaMorta)
                            velocidadeDireita *= (1.0 - (abs(direcao) * coefAtenuacao) / 255.0);
                        else
                            velocidadeEsquerda *= (1.0 - (abs(direcao) * coefAtenuacao) / 255.0);
                    }
                }
            }

            moverMotores(velocidadeEsquerda, velocidadeDireita);
        }

        else
        {
            moverMotores(0, 0); // Para os motores caso a conexão seja perdida
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

#pragma endregion

#endif