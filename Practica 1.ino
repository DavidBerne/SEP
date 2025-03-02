#define PIN_BOTONA 6
#define PIN_BOTONB 7
// Declaramos los pins de los led
const int ledPins[] = {2, 3, 4, 5}; // Pines digitales de los LEDs
unsigned long int valor;
unsigned long int cantidadAIntroducir;
int saldo;
enum estadoMaquina{ELECCION,ENTREGA,RECOGIDA,TECNICO,PRECIOS,RELLENARMAQUINA}maquina=ELECCION;
enum Estadoboton { PULSADO = 1,
                   SUELTO = 2,
                   TR_PULS = 3,
                   TR_SOLT = 4 } botonA = SUELTO, botonB= SUELTO; /*boton es una variable estadoboton*/


Estadoboton gestorBoton(Estadoboton estadoAnteriorBoton, bool lectura) {

  switch (estadoAnteriorBoton) {
      //Caso pulsado, si el boton sigue pulsado, no cambia, sale del caso y devuelve el valor, si el boton ha cambiado paso estar en transicion de suelto
    case PULSADO:
      if (lectura == 1) {
        estadoAnteriorBoton = TR_SOLT;
      }
      break;
      //Caso suelto, si el boton sigue suelto, no cambia, sale del caso y devuelve el valor, si el boton ha cambiado paso estar en transicion de pulsado
    case SUELTO:
      if (lectura == 0) {
        estadoAnteriorBoton = TR_PULS;
      }
      break;
      //Si esta en trnasicion a pulsado el unico estado posible siguiente es el de pulsado
    case TR_PULS:
      estadoAnteriorBoton = PULSADO;
      break;
      //Si esta en transicion a suelto el unico caso posible es que pase a suelto
    case TR_SOLT:
      estadoAnteriorBoton = SUELTO;  
      break;
  }
  Serial.print("Este es el estado del boton: ");
  Serial.println(estadoAnteriorBoton);
  return estadoAnteriorBoton;/* boton = estadoAnteriorBoton*/
}

unsigned long int Saldo(int valor){
  Serial.print("El valor es:");
  Serial.println(valor);
    for (int i = 0; i < 4; i++) {
    int bit = (cantidadAIntroducir >> i) & 1; // Extraer bit
    digitalWrite(ledPins[i], bit); // Encender/apagar LED
  }
}

estadoMaquina gestorMaquina(estadoMaquina maquina,bool lectura1,bool lectura2) {  
  switch (maquina){
    case ELECCION:
      //if(botonA == PULSADO && botonB==PULSADO){}zz
       // maquina=TECNICO;
      if(botonA==PULSADO){
        Serial.println("Estoy en eleccion");
        saldo=saldo+valor;
        maquina=ENTREGA;
      }
      /*if(mantineA){
        maquina=RETIRAR;
      }*/
    break;
    case ENTREGA:
      Serial.println("Estoy en entrega");
      Serial.println(saldo);
      maquina=RECOGIDA;
      //digital.write(ledEntrega);
      
      /*if(mantineA){
        maquina=RETIRAR;
      }
      if(millis()>tiempoentrega){
        maquina=RECOGIDA;
      }*/
    break;
    case RECOGIDA:
      Serial.println("Estoy en recogida");
      //if(finalCarrera){
      maquina=ELECCION;
      //}
    break;
    /*case TECNICO:
      if(presionaA){
        maquina=PRECIOS;
      }
      if(presionaB){
        maquina=RELLENARMAQUINA;
      }
      if(presionaAyB){
        maquina=ELECCION;
      }
    break;
    case PRECIOS:
      if(pulsarAyB){
        maquina=ELECCION;
      }
    break;
    case RELLENARMAQUINA:
      if(pulsarAyB){
        maquina=ELECCION;
      } 
    break; 
    */}
}

void setup() {
  pinMode(PIN_BOTONA,INPUT_PULLUP);
  pinMode(PIN_BOTONB,INPUT_PULLUP);
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT); // Configurar pines como salidas
  }
  Serial.begin(9600);
}

void loop() {  
  cantidadAIntroducir = (analogRead(A0)>>6);
  Saldo(cantidadAIntroducir);
  bool lecturaA = digitalRead(PIN_BOTONA);
  bool lecturaB = digitalRead(PIN_BOTONB);
  botonA = gestorBoton(botonA,lecturaA);
  botonB = gestorBoton(botonB,lecturaB);
  maquina = gestorMaquina(maquina,lecturaA,lecturaB);
  delay(1000);
}
