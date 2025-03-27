#define PIN_BOTONA 6
#define PIN_BOTONB 7

// Declaramos los pins de los led
bool botonesSoltados=false;
bool devuelto=false;
const int ledPins[] = {2, 3, 4, 5}; // Pines digitales de los LEDs indicadores de precio
const int LedEntrega = 8; // Led que indica que la entrega ha sido realizada
const int LedFallo = 9; //Led indica fallo al comprobar precio

unsigned long tiempoInicioPulsacionA = 0; 
unsigned long tiempoPulsacionA = 0;      
unsigned long int inicioPulsacionA, botonPulsadoA;
unsigned long int inicioPulsacionB, botonPulsadoB;
unsigned long int valor;
unsigned long int cantidadAIntroducir;

int Misaldo;//Saldo que tiene en la maquina actualmente
int precio=3;
int finCarrera;
int cantidadAAnadir = 0;
int compartimentoSeleccionado = 0; // Compartimento inicial
int compartimentos[] = {5,6,7}; //Stock inicial de cada compartimento

// Enumeracion estados maquina
enum estadoMaquina{ELECCION,ENTREGA,RECOGIDA,TECNICO,PRECIOS,RELLENARMAQUINA}maquina=ELECCION,estadoAnterior=7;
// Enumeracion estados boton
enum Estadoboton { PULSADO = 1,
                   SUELTO = 2,
                   TR_PULS = 3,
                   TR_SOLT = 4 } botonA = SUELTO, botonB= SUELTO; /*boton es una variable estadoboton*/
//Variables temporales de inicio de pulsacion

// Definicion funcion gestion del boton
Estadoboton gestorBoton(Estadoboton estadoAnteriorBoton, bool lectura) {
  switch (estadoAnteriorBoton) {
    case PULSADO:
      if (lectura == 1) {
        estadoAnteriorBoton = TR_SOLT;
      }
      break;
    case SUELTO:
      if (lectura == 0) {
        estadoAnteriorBoton = TR_PULS;
      }
      break;
    case TR_PULS:
      estadoAnteriorBoton = PULSADO;
      break;
    case TR_SOLT:
      estadoAnteriorBoton = SUELTO;  
      break;
  }
  return estadoAnteriorBoton;
  }
//Funcion que muestra en los led el saldo
unsigned long int MuestraSaldo(int valor){  
  for (int i = 0; i < 4; i++) {
    int bit = (cantidadAIntroducir >> i) & 1; // Extraer bit
    digitalWrite(ledPins[i], bit); // Encender/apagar LED
    }
  }
// Funcion que sirve para retirar el dinero que hay acumulado
void retirarDinero(){
  Serial.print("Saldo actual: ");
  Misaldo = 0;
  devuelto = true;
  Serial.println(Misaldo);
}
int seleccionarCompartimento(){
    int maxStock = 0;
    int compartimentoSeleccionado = 0;
    for (int i = 1; i < 4; i++) {
        if (compartimentos[i] > maxStock) {
            maxStock = compartimentos[i];
            compartimentoSeleccionado = i;
        }
    }
    return compartimentoSeleccionado;
}

void setup() {
    pinMode(PIN_BOTONA, INPUT_PULLUP);
    pinMode(PIN_BOTONB, INPUT_PULLUP);
    for (int i = 0; i < 4; i++) {
      pinMode(ledPins[i], OUTPUT); // Configurar pines como salidas
    }
    pinMode(LedFallo,OUTPUT);
    pinMode(LedEntrega,OUTPUT);
    pinMode(finCarrera,INPUT);
    Serial.begin(9600);
    Serial.println("Aqui empezamos:");
}


void loop() {  
  cantidadAIntroducir = (analogRead(A0) >> 6); // Lectura de la señal analogica y desplazamiento para obtener una señal entre 0 y 15
  MuestraSaldo(cantidadAIntroducir); 
  bool lecturaA = digitalRead(PIN_BOTONA);
  bool lecturaB = digitalRead(PIN_BOTONB);
  finCarrera=digitalRead(11);
  // Actualizar el estado del botón A
  botonA = gestorBoton(botonA, lecturaA);
  //calculo el tiempo de pulsacion del botonA
  if(botonA == TR_PULS){
        inicioPulsacionA = millis();
    }
  if(botonA == PULSADO){
        botonPulsadoA = millis()-inicioPulsacionA;
    }
  // Actualizar el estado del botón B
  botonB = gestorBoton(botonB, lecturaB);
  //Calculo el tiempo de pulsacion del botonB
  if(botonB == TR_PULS){
        inicioPulsacionB = millis();
    }
  if(botonB == PULSADO){
    botonPulsadoB = millis()-inicioPulsacionB;
    }
  // Gestionar la máquina 
  switch (maquina){
    case ELECCION:
      if (estadoAnterior != ELECCION){
      Serial.println("En enleccion, A para introducir, B para elegir producto");
      estadoAnterior=maquina;
      botonesSoltados=false;
      devuelto=false;
      }
      if (!botonesSoltados && botonA == SUELTO && botonB == SUELTO) {
                botonesSoltados = true;
      }
      if(botonA == TR_SOLT && botonesSoltados==true && devuelto==false){
        Serial.print("Tu Saldo:");
        Misaldo= cantidadAIntroducir+Misaldo;
        Serial.println(Misaldo);
      }
      if(botonB == TR_SOLT && botonesSoltados==true){
        maquina = ENTREGA;
      }
      // Verificar si el botón A ha estado pulsado durante más de 2 segundos
      if(botonA == PULSADO && botonPulsadoA>3000 && botonesSoltados==true){
        retirarDinero();
      }
      if(botonA == PULSADO && botonB==PULSADO && botonPulsadoA>2000 && botonPulsadoB>2000 && botonesSoltados==true){
          maquina=TECNICO;
      }
      break;

    case ENTREGA:
      if (estadoAnterior != ENTREGA) {
        Serial.println("Estoy en entrega");
        estadoAnterior=maquina;
      }
      digitalWrite(LedEntrega,LOW);
      if (botonB == TR_PULS && ((Misaldo - precio) >= 0)) {
        int compartimento = seleccionarCompartimento();
        compartimentos[compartimento]--;
        Serial.print("El compartimento ");
        Serial.print(compartimento);
        Serial.print(" le quedan ");
        Serial.println(compartimentos[compartimento]);
        digitalWrite(LedEntrega,HIGH);
        Misaldo=Misaldo - precio;
        Serial.print("Tu nuevo saldo es:");
        Serial.println(Misaldo);
        maquina = RECOGIDA;
      }
      else if(botonB==TR_PULS && ((Misaldo-precio)<0)){
          Serial.println("Su saldo no es suficiente");
          Serial.print("Su saldo: ");
          Serial.println(Misaldo);
          digitalWrite(LedFallo,HIGH);
          maquina = RECOGIDA;
      } 
      break;
    case RECOGIDA:
      if (estadoAnterior != RECOGIDA) {
        estadoAnterior=maquina;
        Serial.println("Estoy en recogida");
      }
      if(finCarrera==0){
      maquina = ELECCION;
      digitalWrite(LedFallo,LOW);
      digitalWrite(LedEntrega,LOW);
      Serial.println("Vuelvo a Eleccion");
      }
      break;
    case TECNICO:
      if (estadoAnterior != TECNICO){
          Serial.println("Estoy en el modo Técnico, pulse A para cambiar el precio y B para añadir existencias");
          estadoAnterior=maquina;
          botonesSoltados=false;
        }
      if(botonA == TR_SOLT && botonesSoltados == true){
        maquina=PRECIOS;
        }
      if(botonB == TR_SOLT && botonesSoltados == true ){


        maquina=RELLENARMAQUINA;
        }
      // Esta aqui el boton pulsado
      if (!botonesSoltados && botonA == SUELTO && botonB == SUELTO) {
                botonesSoltados = true;
            }
      if(botonA == PULSADO && botonB==PULSADO && botonPulsadoA>2000 && botonPulsadoB>2000 && botonesSoltados==true ){
        maquina = ELECCION;
        devuelto = true;
      }
      break;
    case RELLENARMAQUINA:
    if (estadoAnterior != RELLENARMAQUINA) {
        Serial.println("Modo Rellenar Maquina:");
        Serial.print("Compartimento: ");
        Serial.println(compartimentoSeleccionado + 1); // Mostrar 1, 2, 3 en lugar de 0, 1, 2
        Serial.println("Use A para cambiar, B para añadir.");
        estadoAnterior = maquina;
    }
    if (botonA == TR_SOLT) {
        compartimentoSeleccionado = (compartimentoSeleccionado + 1) % 3; // Cambiar compartimento
        Serial.print("Compartimento: ");
        Serial.println(compartimentoSeleccionado + 1);
    }

    if (botonB == TR_SOLT) {
        cantidadAAnadir = cantidadAIntroducir; // Usar el valor del potenciómetro
        compartimentos[compartimentoSeleccionado] += cantidadAAnadir; // Añadir stock
        Serial.print("Añadido ");
        Serial.print(cantidadAAnadir);
        Serial.print(" al compartimento ");
        Serial.println(compartimentoSeleccionado + 1);
        Serial.print("El compartimento seleccionado tiene:");
        Serial.println(compartimentos[compartimentoSeleccionado]);
        maquina = TECNICO; // Volver al modo técnico
    }

      break;

    case PRECIOS:
      if(estadoAnterior !=PRECIOS){
        Serial.println("Estoy en Precios. Pulse A para establecer un nuevo precio y B para volver a Tecnico.");
        estadoAnterior=maquina;
      }
      if(botonA == TR_PULS) {
        Serial.print("El nuevo precio es:");
        precio= cantidadAIntroducir;
        Serial.println(precio);       
      }
      if(botonB== TR_PULS){
      maquina = TECNICO;
      }
      break;
    }  
  //Aqui ya estamos fuera del switch
}
