//===============================================================================================//
//=====================================//INCLUDES E DEFINES//====================================//
//===============================================================================================//
#pragma region INCLUDES

#ifndef MOVE_H
#define MOVE_H

#include <defines.hpp>

#pragma endregion

//===============================================================================================//
//========================================//MOVIMENTACAO//=======================================//
//===============================================================================================//

#pragma region PWM

void moverMotores(int velEsq, int velDir)
{
    // Variáveis estáticas lembram o último comando enviado mesmo após a função terminar
    static int ultimaVelEsq = 0;
    static int ultimaVelDir = 0;

    // Checa se algum dos motores inverteu a rotação:
    bool inverteuDir = (velDir > 0 && ultimaVelDir < 0) || (velDir < 0 && ultimaVelDir > 0);
    bool inverteuEsq = (velEsq > 0 && ultimaVelEsq < 0) || (velEsq < 0 && ultimaVelEsq > 0);

    if (inverteuDir || inverteuEsq) // Se algum deles inverteu a rotação, limpamos o PWM:
    {
        analogWrite(Motor_Direito_Positivo, LOW);
        analogWrite(Motor_Direito_Negativo, LOW);
        analogWrite(Motor_Esquerdo_Positivo, LOW);
        analogWrite(Motor_Esquerdo_Negativo, LOW);
        delayMicroseconds(60);
    }

    // --- ACIONAMENTO MOTOR DIREITO ---
    if (velDir > 0)
    {
        analogWrite(Motor_Direito_Positivo, abs(velDir));
        analogWrite(Motor_Direito_Negativo, LOW);
    }
    else if (velDir < 0)
    {
        analogWrite(Motor_Direito_Negativo, abs(velDir));
        analogWrite(Motor_Direito_Positivo, LOW);
    }
    else if (velDir == 0)
    {
        analogWrite(Motor_Direito_Positivo, 255); // mudar
        analogWrite(Motor_Direito_Negativo, 255); // mudar
    }

    // --- ACIONAMENTO MOTOR ESQUERDO ---
    if (velEsq > 0)
    {
        analogWrite(Motor_Esquerdo_Positivo, abs(velEsq));
        analogWrite(Motor_Esquerdo_Negativo, LOW);
    }
    else if (velEsq < 0)
    {
        analogWrite(Motor_Esquerdo_Negativo, abs(velEsq));
        analogWrite(Motor_Esquerdo_Positivo, LOW);
    }

    else if (velEsq == 0)
    {
        analogWrite(Motor_Esquerdo_Positivo, 255);
        analogWrite(Motor_Esquerdo_Negativo, 255);
    }
    // Atualiza o histórico para o próximo ciclo
    ultimaVelEsq = velEsq;
    ultimaVelDir = velDir;
}
#pragma endregion

#pragma region BUSCAS

void buscaTcholas()
{
    static unsigned long tempoManobraLinha = 0;
    static int direcaoGiroLinha = 1; // 1 para direita, -1 para esquerda

    // ==============================================================================
    // 1. PRIORIDADE MÁXIMA: Sensores de Linha
    // ==============================================================================

    if (estadoAtual != FUGA_LINHA_RE && estadoAtual != FUGA_LINHA_GIRO && estadoAtual != FUGA_LINHA_ABRIR)
    {
        if ((valorLinhaE < 500 && valorLinhaE > 0) && (valorLinhaD < 500 && valorLinhaD > 0))
        {
            servoLaminas.write(90);
            tempoManobraLinha = millis();
            direcaoGiroLinha = 1;
            estadoAtual = FUGA_LINHA_RE;
        }
        else if (valorLinhaE < 500 && valorLinhaE > 0)
        {
            servoLaminas.write(90);
            tempoManobraLinha = millis();
            direcaoGiroLinha = 1; // Viu na esquerda, gira pra direita pro centro
            estadoAtual = FUGA_LINHA_RE;
        }
        else if (valorLinhaD < 500 && valorLinhaD > 0)
        {
            servoLaminas.write(90);
            tempoManobraLinha = millis();
            direcaoGiroLinha = -1; // Viu na direita, gira pra esquerda pro centro
            estadoAtual = FUGA_LINHA_RE;
        }
    }

    // ==============================================================================
    // 2. PRIORIDADE SECUNDÁRIA: Detecção de Oponente (LDRs)
    // ==============================================================================

    if (estadoAtual != FUGA_LINHA_RE && estadoAtual != FUGA_LINHA_GIRO && estadoAtual != FUGA_LINHA_ABRIR)
    {
        if (valorLDRE < 100 || valorLDRF < 100 || valorLDRD < 100)
        {
            estadoAtual = ATAQUE_RAPIDO;
        }
        else
        {
            estadoAtual = AVANCAR_LENTO;
        }
    }

    // ==============================================================================
    // 3. MÁQUINA DE ESTADOS (Ações Isoladas)
    // ==============================================================================
    switch (estadoAtual)
    {
    case FUGA_LINHA_RE:
        moverMotores(-255, -255);
        // Dá ré por 200ms
        if (millis() - tempoManobraLinha >= 200)
        {
            tempoManobraLinha = millis(); // Reseta relógio
            estadoAtual = FUGA_LINHA_GIRO;
        }
        break;

    case FUGA_LINHA_GIRO:
        if (direcaoGiroLinha == 1)
        {
            moverMotores(255, -255); // Gira direita
        }
        else
        {
            moverMotores(-255, 255); // Gira esquerda
        }

        // Tempo do giro (ajuste para ele mirar pro centro do dohyo)
        if (millis() - tempoManobraLinha >= 150)
        {
            tempoManobraLinha = millis();

            servoLaminas.write(180);

            estadoAtual = FUGA_LINHA_ABRIR;
        }
        break;

    case FUGA_LINHA_ABRIR:
        // Dá uma micro-pausa de 150ms com os motores parados só para
        // garantir que a lâmina raspou no chão antes de voltar a avançar
        moverMotores(0, 0);

        if (millis() - tempoManobraLinha >= 150)
        {
            estadoAtual = AVANCAR_LENTO;
        }
        break;

    case ATAQUE_RAPIDO:

        moverMotores(255, 255);
        break;

    case AVANCAR_LENTO:

        moverMotores(50, 50);
        break;
    }
}

void buscaOfensiva()
{
    moverMotores(255, 255);
}

#endif