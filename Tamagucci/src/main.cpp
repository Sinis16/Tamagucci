#include <Arduino.h>
#include <ESP32Time.h>
#include "driver/gpio.h"
#include "stdio.h"


//PIN DEFINITION
#define BUTTON_PIN_FLAG 0
#define BUTTON_PINMid GPIO_NUM_35
#define BUTTON_PINLeft GPIO_NUM_34
#define BUTTON_PINRight GPIO_NUM_39


//VARIABLES

//Buttons
bool left = false;
bool mid = false;
bool right = false;

//Fixes
int start = 0;

//Stats Variables
int life = 100;
int happiness = 100;
int saturation = 100;
int hygiene = 100;
int money = 0;
int age = 1;

//StateMachine
int state = 0;
//state = 0 / Pantalla inicial
//state = 1 / Pantalla estadistica
//state = 2 / Pantalla menu
//state = 3 / Pantalla tienda

//menuVarible
int menuState = 0;
bool enterFlag = false;
char txbuff[50];
String actividades[5] = {"Salir", "Jugar", "Comer", "Asear", "Comprar"};

//jugarVariable
int jugarState = 0;
bool jugarFlag = false;
char txbuffJugar[50];
String juegos[5] = {"Salir", "Moriohtraques", "Lozanoaventuras", "Where's Freddy", "Amogus"};

//comerVariable
int comerState = 0;
bool comerFlag = false;
char txbuffComer[50];
String productosComida[3] = {"Salir", "Empanada", "Arepa"};
int cantidadComida[2] = {0,0};

//asearVariable
int asearState = 0;
bool asearFlag = false;
char txbuffAsear[50];
String productosAseo[3] = {"Salir", "Jabón Rey", "Tío Nacho"};
int cantidadAseo[2] = {0,0};

//comprarVariable
int comprarState = 0;
bool comprarFlag = false;
char txbuffComprar[50];
String productos[5] = {"Salir", "Jabón Rey", "Tío Nacho", "Empanada", "Arepa"};

//moriohTraquesVariable
int moriohTraquesState = 0;
bool moriohTraquesFlag = false;
int moriohTraquesCounter = 0;
char txbuffMoriohTraques[50];

//TASK Handlers

//Botones
TaskHandle_t botonLeft = NULL;
TaskHandle_t botonMid = NULL;
TaskHandle_t botonRight = NULL;

//Pasivos: Stats, Rana
SemaphoreHandle_t pasivos = NULL;
SemaphoreHandle_t inventarioSemaphore = NULL;
TaskHandle_t idleH = NULL;
TaskHandle_t statsH = NULL;
TaskHandle_t menuH = NULL;
TaskHandle_t tiempoH = NULL;
TaskHandle_t inventoryH = NULL;

//Activos: Menu y demas
SemaphoreHandle_t salir = NULL;
SemaphoreHandle_t jugarSemaphore = NULL;
SemaphoreHandle_t comerSemaphore = NULL;
SemaphoreHandle_t asearSemaphore = NULL;
SemaphoreHandle_t comprarSemaphore = NULL;
SemaphoreHandle_t statsManagerSemaphore = NULL;
SemaphoreHandle_t videojuegos = NULL;
TaskHandle_t jugarH = NULL;
TaskHandle_t comerH = NULL;
TaskHandle_t asearH = NULL;
TaskHandle_t comprarH = NULL;
QueueHandle_t actividadQ;
QueueHandle_t asearQ;
QueueHandle_t comerQ;
QueueHandle_t comprarQ;
QueueHandle_t jugarQ;
QueueHandle_t moriohTraquesQ;

//FUNCTIONS

//Complementary Stats
void printStats() {
  Serial.print("Life: ");
  Serial.println(life);
  Serial.print("Happiness: ");
  Serial.println(happiness);
  Serial.print("Saturation: ");
  Serial.println(saturation);
  Serial.print("Hygiene: ");
  Serial.println(hygiene);
  Serial.print("Age: ");
  Serial.println(age);
  Serial.print("Phroglins: ");
  Serial.println(money);

    delay(2000);
}


//Stats
void stats(void *p) {
  while(1) {
    if (xSemaphoreTake(pasivos, portMAX_DELAY)) {
      printStats();
    }
  }
}

//Complementary inventory
void printInventory() {
  Serial.print("Jabón Rey: ");
  Serial.println(cantidadAseo[0]);
  Serial.print("Tío Nacho: ");
  Serial.println(cantidadAseo[1]);
  Serial.print("Empanada: ");
  Serial.println(cantidadComida[0]);
  Serial.print("Arepa: ");
  Serial.println(cantidadComida[1]);
  delay(2000);
}

//Inventory
void inventory(void *p) {
  while(1) {
    if (xSemaphoreTake(inventarioSemaphore, portMAX_DELAY)) {
      printInventory();
    }
  }
}

//Menu
void menu() {
  xSemaphoreTake(salir, portMAX_DELAY);
  while(1 && !enterFlag) {
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left && menuState>0) 
    {
      menuState -= 1;
      left = false;
    }
    else if (mid) {
      enterFlag = true;
      mid = false;
    }
    else if (right && menuState<4) {
      menuState += 1;
      right = false;
    }
    Serial.print(actividades[menuState]);
    Serial.print(enterFlag);
  }
  sprintf(txbuff, actividades[menuState].c_str());
  xQueueSend(actividadQ, (void*)txbuff, (TickType_t)0);
  enterFlag = false;
  xSemaphoreGive(salir);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
}


//Asear
void asear() {
  xSemaphoreTake(asearSemaphore, portMAX_DELAY);
  while(!asearFlag) {
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left && asearState>0) 
    {
      asearState -= 1;
      left = false;
    }
    else if (mid) {
      asearFlag = true;
      mid = false;
    }
    else if (right && asearState<2) {
      asearState += 1;
      right = false;
    }
    Serial.print(productosAseo[asearState]);
    Serial.print(asearFlag); 
  }
  sprintf(txbuffAsear, productosAseo[asearState].c_str());
  xQueueSend(asearQ, (void*)txbuffAsear, (TickType_t)0);
  asearFlag = false;
  xSemaphoreGive(asearSemaphore);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
}


//Comer
void comer() {
  xSemaphoreTake(comerSemaphore, portMAX_DELAY);
  while(!comerFlag) {
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left && comerState>0) 
    {
      comerState -= 1;
      left = false;
    }
    else if (mid) {
      comerFlag = true;
      mid = false;
    }
    else if (right && comerState<2) {
      comerState += 1;
      right = false;
    }
    Serial.print(productosComida[comerState]);
    Serial.print(comerFlag); 
  }
  sprintf(txbuffComer, productosComida[comerState].c_str());
  xQueueSend(comerQ, (void*)txbuffComer, (TickType_t)0);
  comerFlag = false;
  xSemaphoreGive(comerSemaphore);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
}


//Comprar
void comprar() {
  xSemaphoreTake(comprarSemaphore, portMAX_DELAY);
  while(!comprarFlag) {
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left && comprarState>0) 
    {
      comprarState -= 1;
      left = false;
    }
    else if (mid) {
      comprarFlag = true;
      mid = false;
    }
    else if (right && comprarState<4) {
      comprarState += 1;
      right = false;
    }
    Serial.print(productos[comprarState]);
    Serial.print(comprarFlag); 
  }
  sprintf(txbuffComprar, productos[comprarState].c_str());
  xQueueSend(comprarQ, (void*)txbuffComprar, (TickType_t)0);
  comprarFlag = false;
  xSemaphoreGive(comprarSemaphore);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
}

//Jugar
void jugar() {
  xSemaphoreTake(jugarSemaphore, portMAX_DELAY);
  while(!jugarFlag) {
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left && jugarState>0) 
    {
      jugarState -= 1;
      left = false;
    }
    else if (mid) {
      jugarFlag = true;
      mid = false;
    }
    else if (right && jugarState<4) {
      jugarState += 1;
      right = false;
    }
    Serial.print(juegos[jugarState]);
    Serial.print(jugarFlag); 
  }
  sprintf(txbuffJugar, juegos[jugarState].c_str());
  xQueueSend(jugarQ, (void*)txbuffJugar, (TickType_t)0);
  jugarFlag = false;
  xSemaphoreGive(jugarSemaphore);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
}

//Buttons
void buttonTaskLeft(void *p) 
{
  while(1)
  {
    left = true;
    vTaskSuspend(NULL);
    Serial.print("AmogosLeft \r\n");
    vTaskDelay(100 * configTICK_RATE_HZ / 1000);
    
  }
}

void buttonTaskMid(void *p) 
{
  while(1)
  {
    mid = true;
    vTaskSuspend(NULL);
    Serial.print("AmogosMid \r\n");
    vTaskDelay(100 * configTICK_RATE_HZ / 1000);
  }
}

void buttonTaskRight(void *p) 
{
  while(1)
  {
    right = true;
    vTaskSuspend(NULL);
    Serial.print("AmogosRight \r\n");
    vTaskDelay(100 * configTICK_RATE_HZ / 1000);
  }
}

void IRAM_ATTR buttonLeft_isr_handler(void *arg) {
  xTaskResumeFromISR(botonLeft);
}

void IRAM_ATTR buttonMid_isr_handler(void *arg) {
  xTaskResumeFromISR(botonMid);
}

void IRAM_ATTR buttonRight_isr_handler(void *arg) {
  xTaskResumeFromISR(botonRight);
}

void buttonInit() {
  gpio_pad_select_gpio(BUTTON_PINMid);
  gpio_pad_select_gpio(BUTTON_PINLeft);
  gpio_pad_select_gpio(BUTTON_PINRight);

  gpio_set_direction(BUTTON_PINMid, GPIO_MODE_INPUT);
  gpio_set_direction(BUTTON_PINLeft, GPIO_MODE_INPUT);
  gpio_set_direction(BUTTON_PINRight, GPIO_MODE_INPUT);

  gpio_set_intr_type(BUTTON_PINMid, GPIO_INTR_NEGEDGE);
  gpio_set_intr_type(BUTTON_PINLeft, GPIO_INTR_NEGEDGE);
  gpio_set_intr_type(BUTTON_PINRight, GPIO_INTR_NEGEDGE);

  gpio_install_isr_service(BUTTON_PIN_FLAG);

  gpio_isr_handler_add(BUTTON_PINMid, buttonMid_isr_handler, NULL);
  gpio_isr_handler_add(BUTTON_PINLeft, buttonLeft_isr_handler, NULL);
  gpio_isr_handler_add(BUTTON_PINRight, buttonRight_isr_handler, NULL);

  }

//depresion complementaria

void sePaso() {
  xSemaphoreTake(statsManagerSemaphore, portMAX_DELAY);
  happiness -= 0.02;
  saturation -= 0.02;
  hygiene -= 0.02;
  age += 0.01;
  xSemaphoreGive(statsManagerSemaphore);
}

void disminuirVida() {
  xSemaphoreTake(statsManagerSemaphore, portMAX_DELAY);
  int perdida = 0;
  if (happiness < 20) {
    perdida += (21-happiness)*0.01;
  }
  if (saturation < 20) {
    perdida += (21-saturation)*0.01;
  }
  if (hygiene < 20) {
    perdida += (21-hygiene)*0.01;
  }
  life -= perdida;
  xSemaphoreGive(statsManagerSemaphore);
}

void muerte() {
  Serial.print("kgaste");
}

//depresion
void inevitablePasoTiempo(void *p) {
  const TickType_t unSec = 1000 / portTICK_PERIOD_MS;
  while(1)
  {
    //vTaskDelay(unSec*60);
    vTaskDelay(unSec*60);
    sePaso();
    if (life < 1) {
      muerte();
    }

  }
}

void moriohTraques() {
  xSemaphoreTake(videojuegos, portMAX_DELAY);
  while(!moriohTraquesFlag) {
    //vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    int dir = 0;
    Serial.print(dir);
    
    if (left) 
    {
      moriohTraquesState = 1;
      left = false;
    }
    else if (mid) {
      moriohTraquesState = 2;
      mid = false;
    }
    else if (right) {
      moriohTraquesState = 3;
      right = false;
    }
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (dir != moriohTraquesState)
    {
      moriohTraquesFlag = true;
    }
    Serial.print(moriohTraquesFlag); 
    moriohTraquesCounter += 1;
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
  }

  sprintf(txbuffMoriohTraques, moriohTraquesCounter);
  //xQueueSend(moriohTraquesQ, (void*)txbuffMoriohTraques, (TickType_t)0);
  moriohTraquesFlag = false;
  moriohTraquesCounter = 0;
  xSemaphoreGive(videojuegos);
  vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
  
  
}


//Idle
void idle(void *p) {
  while(1) {
    Serial.print("Phrog");
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left) 
    {
      state = 1;
      left = false;
      start += 1;
    }
    else if (mid) {
      state = 2;
      mid = false;
      start += 1;
    }
    else if (right) {
      state = 3;
      right = false;
      start += 1;
    }

    while(state>0 && start > 6)
    {
      Serial.print("()");
      Serial.print(state);
      if (state == 1) {
        state = 0;
        xSemaphoreGive(pasivos);
        vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
      }
      if (state == 2) {
        state = 0;
        menu();
        vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
      }
      if (state == 3) {
        state = 0;
        xSemaphoreGive(inventarioSemaphore);
        vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
      }
    }

    char rxbuff[50];
    if(xQueueReceive(actividadQ, &(rxbuff), (TickType_t)5)){
      String comparar = rxbuff;
      if(comparar == "Asear") {
        Serial.print("SONOS SAPOO");
        asear();
      }
      if(comparar == "Comer") {
        Serial.print("SONOS COME MOCOS");
        comer();
      }
      if(comparar == "Comprar") {
        Serial.print("SONOS INFLA LOS STOCKS");
        comprar();
      }
      if(comparar == "Jugar") {
        Serial.print("MORIOH JOGA BONITO");
        jugar();
      }
      vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    }

    char rxbuffAsear[50];
        if(xQueueReceive(asearQ, &(rxbuffAsear), (TickType_t)5)){
          String compararAseo = rxbuffAsear;
          if(compararAseo == "Jabón Rey") {
            Serial.print("JAAAAAAAABOOOOOOOOON");
          }
          else if(compararAseo == "Tío Nacho") {
            Serial.print("TTTTTTTTTIOOOOOO");
          }
          vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
        }

      char rxbuffComer[50];
        if(xQueueReceive(comerQ, &(rxbuffComer), (TickType_t)5)){
          String compararComida = rxbuffComer;
          if(compararComida == "Empanada") {
            Serial.print("EMPANAAAAADA");
          }
          else if(compararComida == "Arepa") {
            Serial.print("AREPA CON QUESSSOOOOOOOOOO");
          }
          vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
        }

      char rxbuffComprar[50];
        if(xQueueReceive(comprarQ, &(rxbuffComprar), (TickType_t)5)){
          String compararCompra = rxbuffComprar;
          if(compararCompra == "Jabón Rey") {
            Serial.print("JAAAAAAAABOOOOOOOOON COMPRA");
          }
          else if(compararCompra == "Tío Nacho") {
            Serial.print("TTTTTTTTTIOOOOOO COMPRA");
          }
          else if(compararCompra == "Empanada") {
            Serial.print("EMPANAAAAADA COMPRA");
          }
          else if(compararCompra == "Arepa") {
            Serial.print("AREPA CON QUESSSOOOOOOOOOO COMPRA");
          }
          vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
        }
String juegos[5] = {"Salir", "Moriohtraques", "Lozanoaventuras", "Where's Freddy", "Amogus"};

      char rxbuffJugar[50];
        if(xQueueReceive(jugarQ, &(rxbuffJugar), (TickType_t)5)){
          String compararJuego = rxbuffJugar;
          if(compararJuego == "Amogus") {
            Serial.print("AMOGOS");
          }
          else if(compararJuego == "Moriohtraques") {
            Serial.print("AIUA ME VA A MATAR");
            moriohTraques();
          }
          else if(compararJuego == "Lozanoaventuras") {
            Serial.print("OTRA TAREA...");
          }
          else if(compararJuego == "Where's Freddy") {
            Serial.print("NO SE U-U");
          }
          
          vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
        }
  }
}


//Setup
void setup() {
  Serial.begin(9600);

  buttonInit();

  //Botones

  // Create two tasks
  pasivos = xSemaphoreCreateBinary();
  inventarioSemaphore = xSemaphoreCreateBinary();
  salir = xSemaphoreCreateMutex();
  asearSemaphore = xSemaphoreCreateMutex();
  comerSemaphore = xSemaphoreCreateMutex();
  comprarSemaphore = xSemaphoreCreateMutex();
  statsManagerSemaphore = xSemaphoreCreateMutex();
  jugarSemaphore = xSemaphoreCreateMutex();
  videojuegos = xSemaphoreCreateMutex();
  
  actividadQ = xQueueCreate(1,sizeof(txbuff));
  asearQ = xQueueCreate(1,sizeof(txbuffAsear));
  comerQ = xQueueCreate(1,sizeof(txbuffComer));
  comprarQ = xQueueCreate(1,sizeof(txbuffComprar));
  jugarQ = xQueueCreate(1,sizeof(txbuffJugar));
  moriohTraquesQ = xQueueCreate(1,sizeof(txbuffMoriohTraques));

  xTaskCreate(inevitablePasoTiempo, "tiempoTask", 4096, NULL, 10, &tiempoH);
  xTaskCreate(buttonTaskLeft, "buttonPressLeft", 4096, NULL, 10, &botonLeft);
  xTaskCreate(buttonTaskMid, "buttonPressMid", 4096, NULL, 10, &botonMid);
  xTaskCreate(buttonTaskRight, "buttonPressRight", 4096, NULL, 10, &botonRight);
  xTaskCreate(idle, "idletask", 4096, NULL, 200, &idleH);
  xTaskCreate(stats, "statsTask", 4096, NULL, 10, &statsH);
  xTaskCreate(inventory, "statsTask", 4096, NULL, 10, &inventoryH);

  // Start the FreeRTOS scheduler
  vTaskStartScheduler();

  xSemaphoreGive(salir);

  while(1) {
  }
}


void loop() {
  // This loop will not execute since FreeRTOS is running the tasks
}