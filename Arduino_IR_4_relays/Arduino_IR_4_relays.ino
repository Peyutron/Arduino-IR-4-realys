/*///////////Control de reles con infrarrojos///////////////////////
   Programa para controlar hasta 4 reles con un control remoto
   infrarrojo configurable.
   Test de lectura de control remoto infrarrojo con monitor serial.
   Graba las teclas seleccionando el boton del control remoto
   para el rele 1, 2, 3, 4. desde el monitor serial.

   El modulo de reles funciona a la inversa, es decir, cuando el puerto
   esta en LOW el rele esta ON y viceversa

   El teminal serial solo se usa la primera vez para configurar las
   teclas del control remoto, una vez guardadas en la memoria EEPROM
   el modulo no necesita estar conectado a ningun PC para funcionar

   Autor: Carlos Muñoz
   http://www.infotronikblog.com
*/

//Añadimos la libreria necesaria:

#include <IRremote.h>
#include <EEPROM.h>

const int irPin = 2;
int8_t currIn = -1;
int8_t currSt = -1;
int8_t normalFn = -1;
String serialStr;
byte resultado = 0;
int but[5];
int direccion = 0;
byte rele1 = 3;
byte rele2 = 4;
byte rele3 = 5;
byte rele4 = 6;
boolean startService = false;
boolean onOff[4] = {false, false, false, false};

IRrecv irrecv(irPin);
decode_results results;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn();
  currSt = 0;
  currIn = 0;
  normalFn = 0;
  for (int x = 3; x < 7; x++) { //Iniciamos los reles
    pinMode(x, OUTPUT);
    digitalWrite(x, HIGH);
  }
  delay (500);
  but[0] = EEPROM.read (direccion);
  but[1] = EEPROM.read (direccion + 1);
  but[2] = EEPROM.read (direccion + 2);
  but[3] = EEPROM.read (direccion + 3);
  delay(1000);
  //showValues(); //Descomenta esta linea para ver los datos cargados en la EEPROM
}
/////////////////Programa principal/////////////////
void loop()
{
  if (Serial.available()) {
    serialStr = (Serial.readStringUntil('\n'));
    serialStr.trim();

    Serial.println(serialStr);
    currIn = serialStr.toInt();  //Cualquier tecla no numerica resultara "0"
  }

  if (currIn >= 0) {  // entra en el estado solo si es una entrada valida
    switch (currSt) { // Menu de seleccion
      case 0: {       // Este es el menu principal
          switch (currIn) { //selecciona una entrada del menu
            case 0:
              startService = true;
              delay(500);
              Serial.println(F("---------------Menu---------------"));
              Serial.println(F("Selecciona una opcion de la lista"));
              Serial.println(F(" 1- Comprobar mando:"));
              Serial.println(F(" 2- Guardar teclas:"));
              Serial.println(F(" 3- Ayuda."));
              currIn = -1;
              break;
            case 1:
              currSt  = 1;
              currIn  = 0;
              break;
            case 2:
              currSt  = 2;
              currIn  = 0;
              break;
            case 3:
              currSt  = 3;
              currIn  = 0;
              break;
            default:
              Serial.println (F("No es una entrada valida, por favor elige una de la lista"));
              currSt = 0;
              currIn = 0;
              break;
          }
          break;
        }
      case 1:
        switch (currIn) {
          case 0:
            startService = false;
            Serial.println(F("----------------------------------------------------------"));
            Serial.println(F("Coprobacion de mando a distancia: "));
            Serial.println(F("Pulsa 1 para empezar y 2 para salir "));
            currIn  = -1;
            break;
          case 1:
            readIR();
            currIn  = 1;
            break;
          case 2:
            currSt = 0;
            currIn = 0;
            break;
        }
        break;
      case 2:   /////////////currSt = 2 ////////////////////
        switch (currIn) {
          case  0 :
            startService = false;
            resultado = 0;
            Serial.println(F("----------------------------------------------------------"));
            Serial.println(F("Pulsa una tecla en el mando para continuar: "));
            Serial.print(F("Boton nº1: "));
            currIn  = 1;
            break;
          case  1:
            readIR();
            if ((currIn == 1) && (resultado > 0)) {
              EEPROM.write (direccion, resultado);
              Serial.println("Dato guardado en direccion 0");
              delay (10);
              but[0] = EEPROM.read (direccion);
              Serial.println("Dato cargago en rele 1");
              Serial.print(F("Boton nº2: "));
              currIn = 2;
              resultado = 0;
              break;
            }
            currIn  = 1;
            break;
          case  2:
            readIR();
            if ((currIn == 2) && (resultado > 0)) {
              EEPROM.write (direccion + 1, resultado);
              Serial.println("Dato guardado en direccion 1");
              delay (10);
              but[1] = EEPROM.read (direccion + 1);
              Serial.println("Dato cargago en rele 2");
              Serial.print(F("Boton nº3: "));
              resultado = 0;
              currIn = 3;
              break;
            }
            currIn  = 2;
            break;
          case  3:
            readIR();
            if ((currIn == 3) && (resultado > 0)) {
              EEPROM.write (direccion + 2, resultado);
              Serial.println("Dato guardado en direccion 2");
              delay (10);
              but[2] = EEPROM.read (direccion + 2);
              Serial.println("Dato cargago en rele 3");
              Serial.print(F("Boton nº4: "));
              resultado = 0;
              currIn  = 4;
              break;
            }
            currIn  = 3;
            break;
          case  4:
            readIR();
            if ((currIn == 4) && (resultado > 0)) {
              EEPROM.write (direccion + 3, resultado);
              Serial.println("Dato guardado en direccion 3");
              delay (10);
              but[3] = EEPROM.read (direccion + 3);
              Serial.println("Dato cargago en rele 4");
              resultado = 0;
              currSt  = 0;
              currIn  = 0;
              showValues();
              break;
            }
            currIn  = 4;
            break;
        }
        break;
      case 3:
        Serial.println(F("----------------------------------------------------------"));
        Serial.println(F("Sobre este programa:"));
        Serial.println(F("Control de 4 reles control remoto IR"));
        Serial.println(F("mediante una simple interfaz de menus."));
        Serial.println();
        Serial.println(F("Una vez guardados los valores, no necesita "));
        Serial.println(F("conexion al PC para funcionar"));
        Serial.println();
        Serial.println(F("Para un correcto funcionamiento recuerda poner el "));
        Serial.println(F("monitor serial en 'Ambos NL & CR'."));
        Serial.println();
        Serial.println(F("Pagina web: www.infotronikblog.com"));
        Serial.println(F("Email: peyutron@gmail.com"));

        currSt = 0;
        currIn = 0;
        break;
    }

  }
  if (startService == true) {

    readIR();
    if (resultado == but[0]) {
      resultado = 0;
      if (onOff[0] == true) {
        digitalWrite (rele1, onOff[0]);
        Serial.println(onOff[0]);
        onOff[0] = false;
      } else {
        digitalWrite (rele1, onOff[0]);
        Serial.println(onOff[0]);
        onOff[0] = true;
      }
    }
    if (resultado == but[1]) {
      resultado = 0;
      if (onOff[1] == true) {
        digitalWrite (rele2, onOff[1]);
        Serial.println(onOff[1]);
        onOff[1] = false;
      } else {
        digitalWrite (rele2, onOff[1]);
        Serial.println(onOff[1]);
        onOff[1] = true;
      }
    }  if (resultado == but[2]) {
      resultado = 0;
      if (onOff[2] == true) {
        digitalWrite (rele3, onOff[2]);
        Serial.println(onOff[2]);
        onOff[2] = false;
      } else {
        digitalWrite (rele3, onOff[2]);
        Serial.println(onOff[2]);
        onOff[2] = true;
      }
    }
    if (resultado == but[3]) {
      resultado = 0;
      if (onOff[3] == true) {
        digitalWrite (rele4, onOff[3]);
        Serial.println(onOff[3]);
        onOff[3] = false;
      } else {
        digitalWrite (rele4, onOff[3]);
        Serial.println(onOff[3]);
        onOff[3] = true;
      }
    }
  }
  serialStr = ("");
  resultado = 0;
  ////////////////
}
//////////////Subrutina para recibir datos del control remoto///////////////////
void readIR() {
  delay(500);
  if (irrecv.decode(&results))
  {
    resultado = (results.value);
    resultado = (abs (resultado));
    if (resultado == 255) {
      Serial.println ("Error: pulse de nuevo");
      irrecv.resume();
    } else {
      Serial.print(resultado);
      irrecv.resume();
      Serial.println(" Dato recibido con exito!!!");
    }
  }
  return;
  ///////
}
//////////Subrutina para ver los valor del Array de botones///////////////////////
void showValues() {
  Serial.println(but[0]);
  Serial.println(but[1]);
  Serial.println(but[2]);
  Serial.println(but[3]);
}

