//===============================================================================================//
//======================================// MODO AUTO //==========================================//
//===============================================================================================//

#ifndef MODO_AUTO_HPP
#define MODO_AUTO_HPP

#pragma region INCLUDES
#include "defines.hpp"
#include "move.hpp"

#pragma endregion

void LedFicaVerde(int numeroDaEtapa)
{
    // Altera apenas a cor do LED da etapa correspondente
    if (numeroDaEtapa >= 0 && numeroDaEtapa < 5)
    {
        leds[numeroDaEtapa] = CRGB::Green;
        FastLED.show(); // Envia o comando atualizado
    }
}

void TodosLedsVermelhos()
{
    fill_solid(leds, 5, CRGB::Red);
    FastLED.show();
}

void TodosLedsBrancos()
{
    fill_solid(leds, 5, CRGB::White);
    FastLED.show();
}

#pragma region Menu Guia
void mostrarMenu()
{
    SerialBT.println("\n========== MENU MAROLA ==========");

    SerialBT.println("\n--- POSIÇAO ---");
    SerialBT.println(" P : Perto");
    SerialBT.println(" M : Medio");
    SerialBT.println(" L : Longe");

    SerialBT.println("--- ATAQUE ---");
    SerialBT.println(" f : Frentao Progressivo");
    SerialBT.println(" F : Frentao BOLADO (Anti-Pescador)");
    SerialBT.println(" b : Costas");
    SerialBT.println(" c : Curva Leve");
    SerialBT.println(" C : Curva Pesada");
    SerialBT.println(" e : Espera");
    SerialBT.println(" v : Movimentacao em V");
    SerialBT.println(" d : Diagonal");
    SerialBT.println(" g : Giro 90°");
    SerialBT.println(" j : Desempate");

    SerialBT.println("\n--- Buscas ---");
    SerialBT.println(" O : Ofensiva");
    SerialBT.println(" T : Tcholas");

    SerialBT.println("\n--- CONFIGURACAO ---");
    SerialBT.println(" < / > : Define Lado (Esq/Dir)");
    SerialBT.println(" =     : Define Lado Frente");
    SerialBT.println(" S     : Debug Sensores");
    SerialBT.println(" R     : Debug Motores");
    SerialBT.println(" h     : Mostrar Menu");
    SerialBT.println(" Y / y : Testar Código");
    SerialBT.println("======================================\n");
}
#pragma endregion

#pragma region Function Check

void checarStatus()
{
    SerialBT.println("\n--- STATUS ATUAL ---");
    SerialBT.print("Estrategia: ");
    SerialBT.println(estrategia);

    SerialBT.print("Lado: ");
    SerialBT.println(lado == direita ? "Direita" : (lado == esquerda ? "Esquerda" : "Frente"));

    SerialBT.print("Posicionamento: ");
    SerialBT.println(Posicao == Perto ? "Perto" : (Posicao == Medio ? "Medio" : "Longe"));

    SerialBT.print("Busca: ");
    SerialBT.println(busca == Tchola ? "Busca Tchola" : "Ofensiva");

    SerialBT.println("--------------------\n");
    LedFicaVerde(4);
    num_led = 5;
}
#pragma endregion

#pragma region Inicio Setup

void setupAuto()
{
    num_led = 2;
    LedFicaVerde(0);
    for (int i = 0; i < 2; i++)
    {
        digitalWrite(led, HIGH);
        delay(250);
        digitalWrite(led, LOW);
        delay(250);
    }

    SerialBT.begin("Marola_RCX");
    LedFicaVerde(1);
    bool primeiraVez = true;
    bool testarMovimento = false;

    while (true)
    {
        uint8_t brilhoPulsante = beatsin8(25, 50, 250);
        fill_solid(leds, num_led, CHSV(96, 255, brilhoPulsante));
        FastLED.show();

        if (SerialBT.connected() && primeiraVez)
        {
            SerialBT.println("CONECTADO");
            mostrarMenu();
            digitalWrite(led, HIGH);
            primeiraVez = false;
            LedFicaVerde(2);
            LedFicaVerde(3);
            num_led = 4;
        }

        // ============================|| ESCOLHA DE CONFIGURAÇÕES ||================================

        if (SerialBT.available())
        {
            BT = SerialBT.read();

            if (BT != 13 && BT != 10)
            {
                SerialBT.println(BT);

                if (BT == '<')
                {
                    lado = esquerda;
                }
                else if (BT == '>')
                {
                    lado = direita;
                }
                else if (BT == '=')
                {
                    lado = frente;
                }

                else if (BT == 'P')
                {
                    Posicao = Perto;
                }
                else if (BT == 'M')
                {
                    Posicao = Medio;
                }
                else if (BT == 'L')
                {
                    Posicao = Longe;
                }
                else if (BT == 'O')
                {
                    busca = Ofensiva;
                }
                else if (BT == 'h')
                {
                    mostrarMenu();
                }
                else if (BT == 'Y' || BT == 'y')
                {
                    SerialBT.println("\n[DEBUG] Executando APENAS o movimento inicial...");
                    testarMovimento = true;
                }

#pragma region Debugs

                else if (BT == 'S')
                {
                    running_Sensors = true;
                    char modoDebug = 'c';

                    SerialBT.println("\n--- MODO DEBUG ATIVO ---");
                    SerialBT.println("Envie 'l' para Linha | 'r' para LDR | 'X' para Sair");

                    for (;;)
                    {

                        if (SerialBT.available())
                        {
                            char comandoDbg = SerialBT.read();

                            if (comandoDbg == 'X' || comandoDbg == 'x')
                            {
                                running_Sensors = false;
                                break;
                            }
                            else if (comandoDbg == 'l' || comandoDbg == 'r')
                            {
                                modoDebug = comandoDbg;
                            }
                        }

                        if (modoDebug == 'l')
                        {
                            SerialBT.printf("LINHA_E: %d | LINHA_D: %d\n", valorLinhaE, valorLinhaD);
                        }
                        else if (modoDebug == 'r')
                        {
                            SerialBT.printf("LDR_E: %d | LDR_F: %d | LDR_D: %d\n", valorLDRE, valorLDRF, valorLDRD);
                        }

                        vTaskDelay(pdMS_TO_TICKS(100));
                    }
                }
                else if (BT == 'R')
                {
                    SerialBT.println("\n--- DEBUG DE MOTORES ATIVO ---");
                    SerialBT.println("Envie 'X' para Sair");

                    unsigned long tempoUltimoComando = millis();
                    int estadoMotor = 0; // 0 = Frente, 1 = Ré, 2 = Direita, 3 = Esquerda

                    SerialBT.println("Motor: FRENTE");
                    moverMotores(50, 50);

                    for (;;)
                    {

                        if (SerialBT.available())
                        {
                            char comandoDbg = SerialBT.read();
                            if (comandoDbg == 'X' || comandoDbg == 'x')
                            {
                                moverMotores(0, 0);
                                break;
                            }
                        }

                        if (millis() - tempoUltimoComando >= 3000)
                        {
                            tempoUltimoComando = millis();
                            estadoMotor++;

                            if (estadoMotor > 3)
                                estadoMotor = 0; // Volta pro começo do ciclo

                            switch (estadoMotor)
                            {
                            case 0:
                                SerialBT.println("Motor: FRENTE");
                                moverMotores(50, 50);
                                break;
                            case 1:
                                SerialBT.println("Motor: RE");
                                moverMotores(-50, -50);
                                break;
                            case 2:
                                SerialBT.println("Motor: DIREITA");
                                moverMotores(50, -50);
                                break;
                            case 3:
                                SerialBT.println("Motor: ESQUERDA");
                                moverMotores(-50, 50);
                                break;
                            }
                        }

                        vTaskDelay(pdMS_TO_TICKS(50)); // Pequena pausa para manter o ESP32 estável
                    }
                }
#pragma endregion
                else
                {
                    if (BT > 48 && BT < 58)
                    { /* Ignora números soltos se houver */
                    }
                    else
                    {
                        estrategia = BT;
                    }
                }
                checarStatus();
            }
        }
#pragma endregion

#pragma region Comandos Controle

        if (IrReceiver.decode()) // No final do Loop checamos o receptor
        {
            IrReceiver.resume();
            if (IrReceiver.decodedIRData.command == botao1) // Se recebermos 1 do controle Remoto:
            {

                for (int i = 0; i < 2; i++) // Entramos no loop
                {

                    digitalWrite(led, LOW);
                    vTaskDelay(pdMS_TO_TICKS(80));

                    if (IrReceiver.decode()) // Se o juiz apertar o 1 e logo depois o 2, identificamos rapidamente aqui
                    {
                        IrReceiver.resume();
                        if (IrReceiver.decodedIRData.command == botao2) // se Recebermos 2
                        {
                            digitalWrite(led, HIGH);
                            break;
                        }
                    }

                    digitalWrite(led, HIGH);
                    vTaskDelay(pdMS_TO_TICKS(80));
                }
            }

            if (IrReceiver.decodedIRData.command == botao2) // se Recebermos 2
            {
                servoLaminas.attach(SERVOMOTOR_PIN, SERVO_MIN_PULSE_US, SERVO_MAX_PULSE_US);
                digitalWrite(led, LOW);
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
#pragma endregion
#pragma region Estrategias
    //=============================|| COMEÇO DE LUTA ||================================
    TodosLedsVermelhos();

    servoLaminas.write(0);

    float multDist = 1.0;

    if (Posicao == Perto)
    {
        multDist = 0.7; // Corta o tempo de avanço pela metade (bate rápido)
    }
    else if (Posicao == Medio)
    {
        multDist = 1.0; // Mantém os tempos originais do seu código
    }
    else if (Posicao == Longe)
    {
        multDist = 1.6; // Aumenta o tempo em 50% para cruzar o dohyo inteiro
    }

    switch (estrategia)
    {
    case 'f': // frentão Progressivo
        SerialBT.print("FRENTAO PROGRESSIVO");
        moverMotores(48, 48);
        delay(4);
        moverMotores(60, 60);
        delay(4);
        moverMotores(72, 72);
        delay(4);
        moverMotores(84, 84);
        delay(4);
        moverMotores(96, 96);
        delay(4);
        moverMotores(128, 128);
        delay(8);
        moverMotores(160, 160);
        delay(8);
        moverMotores(196, 196);
        delay(8);
        moverMotores(228, 228);
        delay(12);
        moverMotores(255, 255);
        delay(int(200 * multDist)); // Aceleração mantida, só muda a distância final
        break;

    case 'F': // Frentão Anti Pescador

        SerialBT.print("FRENTAO BOLADO");
        moverMotores(255, 255);
        delay(int(250 * multDist));
        break;

    case 'b': // Costas

        SerialBT.print("COSTAS");
        if (lado == frente)
        {
            moverMotores(-255, -255);
            delay(int(150 * multDist));
            moverMotores(0, 0);
            delay(10);
            break;
        }
        else if (lado == esquerda)
        {
            moverMotores(-255, -255);
            delay(int(120 * multDist));
            moverMotores(-255, 255);
            delay(30); // Giro NÃO muda com a distância
            moverMotores(255, 255);
            delay(int(160 * multDist));
            moverMotores(0, 0);
            delay(10);
            lado = direita;
            break;
        }
        else if (lado == direita)
        {
            moverMotores(-255, -255);
            delay(int(120 * multDist));
            moverMotores(255, -255);
            delay(30); // Giro NÃO muda com a distância
            moverMotores(255, 255);
            delay(int(160 * multDist));
            moverMotores(0, 0);
            delay(10);
            lado = esquerda;
            break;
        }

    case 'c': // Curva Leve

        SerialBT.print("CURVA LEVE");

        if (lado == esquerda)
        {
            moverMotores(-255, 255);
            delay(20);
            moverMotores(255, 255);
            delay(int(120 * multDist)); // Arrancada ajustada
            moverMotores(0, 0);
            delay(10);
            moverMotores(255, -255);
            delay(70);
            moverMotores(0, 0);
            delay(10);
            lado = direita;
            break;
        }
        else if (lado == direita)
        {
            moverMotores(255, -255);
            delay(20);
            moverMotores(255, 255);
            delay(int(120 * multDist)); // Arrancada ajustada
            moverMotores(0, 0);
            delay(10);
            moverMotores(-255, 255);
            delay(70);
            moverMotores(0, 0);
            delay(10);
            lado = esquerda;
            break;
        }

    case 'C': // Curva Pesada

        SerialBT.print("CURVA PESADA");

        if (lado == esquerda)
        {
            moverMotores(-255, 255);
            delay(40);
            moverMotores(255, 255);
            delay(int(170 * multDist)); // Arrancada ajustada
            moverMotores(0, 0);
            delay(10);
            moverMotores(255, -255);
            delay(100);
            moverMotores(255, 255);
            delay(int(50 * multDist)); // Bote final ajustado
            moverMotores(0, 0);
            delay(10);
            lado = direita;
            break;
        }
        else if (lado == direita)
        {
            moverMotores(255, -255);
            delay(40);
            moverMotores(255, 255);
            delay(int(170 * multDist)); // Arrancada ajustada
            moverMotores(0, 0);
            delay(10);
            moverMotores(-255, 255);
            delay(100);
            moverMotores(255, 255);
            delay(int(50 * multDist)); // Bote final ajustado
            moverMotores(0, 0);
            delay(10);
            lado = esquerda;
            break;
        }

    case 'd': // diagonal

        if (lado == esquerda)
        {
            moverMotores(-255, 255);
            delay(70);
            moverMotores(255, 255);
            delay(int(150 * multDist));
            moverMotores(0, 0);
            delay(10);
            moverMotores(255, -255);
            delay(70);
            lado = direita;
            break;
        }
        else if (lado == direita)
        {
            moverMotores(255, -255);
            delay(70);
            moverMotores(255, 255);
            delay(int(150 * multDist));
            moverMotores(0, 0);
            delay(10);
            moverMotores(-255, 255);
            delay(70);
            lado = esquerda;
            break;
        }

    case 'z': // Basico
        SerialBT.print("Basico");
        break;

    case 'e': // Espera

        delay(400);
        break;

    case 'v': // movimento em V
        if (lado == esquerda)
        {
            moverMotores(-255, 255);
            delay(40);
            moverMotores(255, 255);
            delay(int(140 * multDist));
            moverMotores(0, 0);
            delay(10);
            moverMotores(255, -255);
            delay(190);
            moverMotores(0, 0);
            delay(10);
            moverMotores(255, 255);
            delay(int(180 * multDist));
            lado = direita;
            break;
        }
        else if (lado == direita)
        {
            moverMotores(255, -255);
            delay(40);
            moverMotores(255, 255);
            delay(int(140 * multDist));
            moverMotores(0, 0);
            delay(10);
            moverMotores(-255, 255);
            delay(190);
            moverMotores(0, 0);
            delay(10);
            moverMotores(255, 255);
            delay(int(180 * multDist));
            lado = esquerda;
            break;
        }

    case 'a': // Aquele lá

        if (lado == esquerda)
        {
            moverMotores(-255, 255);
            delay(60);
            moverMotores(255, 255);
            delay(int(110 * multDist));
            moverMotores(0, 0);
            delay(20);
            moverMotores(255, -255);
            delay(130);
            moverMotores(0, 0);
            delay(10);
            break;
        }
        else if (lado == direita)
        {
            moverMotores(255, -255);
            delay(60);
            moverMotores(255, 255);
            delay(int(110 * multDist));
            moverMotores(0, 0);
            delay(20);
            moverMotores(-255, 255);
            delay(130);
            moverMotores(0, 0);
            delay(10);
            break;
        }

    case 'g': // giro 45° (Puro giro, distância não afeta)
        if (lado == esquerda)
        {
            moverMotores(-255, 255);
            delay(50);
            moverMotores(0, 0);
            delay(500);
            lado = direita;
            break;
        }
        else if (lado == direita)
        {
            moverMotores(255, -255);
            delay(50);
            moverMotores(0, 0);
            delay(500);
            lado = esquerda;
            break;
        }

    case 'j': // Desempate

        if (lado == esquerda)
        {
            moverMotores(-255, 255);
            delay(150);
            moverMotores(0, 0);
            delay(20);
            break;
        }
        else if (lado == direita)
        {
            moverMotores(255, -255);
            delay(150);
            moverMotores(0, 0);
            delay(20);
            break;
        }
    }

    running_Sensors = true;

    if (testarMovimento)
    {
        moverMotores(0, 0); // Garante que os motores desligaram
        SerialBT.println("[DEBUG] Movimento finalizado. Reinicie o robo no botao para um novo teste.");

        while (true)
        {
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}

#pragma endregion

#pragma region Loop AUTO
void loopAuto()
{

    while (true)
    {
        if (busca == Tchola)
        {
            buscaTcholas();
        }
        else if (busca == Ofensiva)
        {
            buscaOfensiva();
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

#pragma endregion

#endif
