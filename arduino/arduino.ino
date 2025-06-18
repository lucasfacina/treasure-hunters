// --- Pinos para o Joystick 1 (Player 1) ---
#define P1_X_PIN A0
#define P1_Y_PIN A1

// --- Pinos para o Joystick 2 (Player 2) ---
#define P2_X_PIN A2
#define P2_Y_PIN A3

// --- "Zona Morta" do Joystick ---
// Se o valor lido estiver entre esses dois, consideramos o joystick parado.
#define LIMIAR_INFERIOR 300
#define LIMIAR_SUPERIOR 700

/**
 * Função que converte os valores X e Y de um joystick em um caractere de ação.
 * @param xValue - Valor lido do eixo X (0-1023)
 * @param yValue - Valor lido do eixo Y (0-1023)
 * @param upChar - Caractere para "Cima"
 * @param leftChar - Caractere para "Esquerda"
 * @param downChar - Caractere para "Baixo"
 * @param rightChar - Caractere para "Direita"
 * @param neutralChar - Caractere para "Neutro/Parado"
 * @return O caractere da ação correspondente.
 */
char getPlayerAction(int xValue, int yValue, char upChar, char leftChar, char downChar, char rightChar,
                     char neutralChar) {
  // NOTA: Em alguns joysticks, o eixo Y é invertido.
  // Se "para cima" der um valor baixo, inverta as condições para upChar e downChar.

  // Verifica o eixo Y primeiro (Cima/Baixo)
  if (yValue > LIMIAR_SUPERIOR) {
    return upChar;
  }
  if (yValue < LIMIAR_INFERIOR) {
    return downChar;
  }

  // Se não for Cima/Baixo, verifica o eixo X (Esquerda/Direita)
  if (xValue < LIMIAR_INFERIOR) {
    return rightChar;
  }
  if (xValue > LIMIAR_SUPERIOR) {
    return leftChar;
  }

  // Se não se moveu em nenhum eixo, está neutro
  return neutralChar;
}

void setup() {
  // Inicia a comunicação serial com uma taxa de 9600 bits por segundo.
  // Essa taxa deve ser a mesma no seu código C++.
  Serial.begin(9600);
}

void loop() {
  // Lê os valores analógicos dos dois joysticks
  int p1_x_val = analogRead(P1_X_PIN);
  int p1_y_val = analogRead(P1_Y_PIN);
  int p2_x_val = analogRead(P2_X_PIN);
  int p2_y_val = analogRead(P2_Y_PIN);

  // Processa a direção para cada jogador
  char p1_action = getPlayerAction(p1_x_val, p1_y_val, 'W', 'A', 'S', 'D', 'N');
  char p2_action = getPlayerAction(p2_x_val, p2_y_val, 'T', 'L', 'B', 'R', 'M');

  // Envia os caracteres pela porta serial
  Serial.print(p1_action);
  Serial.print(p2_action);
  Serial.println(); // Envia um caractere de nova linha para marcar o fim do comando

  // Pequeno delay para não sobrecarregar a porta serial
  delay(50);
}
