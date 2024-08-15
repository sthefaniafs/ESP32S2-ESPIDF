#ifndef RELAY_H
#define RELAY_H

typedef struct
{
    int pin;    // pino do relé
    int state;  // estado do relé (on/off)
}  Relay;

// inicia componente
void relay_init(Relay *relay, int pin);
// liga relé
void relay_turn_on (Relay *relay);
// desliga relé
void relay_turn_off (Relay *relay);
// obter status do relé
int relay_get_status(Relay *relay);


#endif