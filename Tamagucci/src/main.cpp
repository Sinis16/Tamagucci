#include <Arduino.h>
#include <ESP32Time.h>
#include "driver/gpio.h"
#include "stdio.h"

//PIN DEFINITION

//pin definition
#define BUTTON_PIN_FLAG 0
#define BUTTON_PINMid GPIO_NUM_35
#define BUTTON_PINLeft GPIO_NUM_34
#define BUTTON_PINRight GPIO_NUM_39
//VARIABLES

//Buttons
bool left = false;
bool mid = false;
bool right = false;

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

//jugarVariable
int jugarState = 0;
bool jugarFlag = false;

//comerVariable
int comerState = 0;
bool comerFlag = false;

//asearVariable
int asearState = 0;
bool asearFlag = false;

//comprarVariable
int comprarState = 0;
bool comprarFlag = false;

//TASK Handlers

//Botones
TaskHandle_t botonLeft = NULL;
TaskHandle_t botonMid = NULL;
TaskHandle_t botonRight = NULL;

//Pasivos: Stats, Rana
SemaphoreHandle_t pasivos = NULL;
TaskHandle_t idleH = NULL;
TaskHandle_t statsH = NULL;

//Activos: Menu y demas
SemaphoreHandle_t salir = NULL;
SemaphoreHandle_t jugar = NULL;
SemaphoreHandle_t comer = NULL;
SemaphoreHandle_t asear = NULL;
SemaphoreHandle_t comprar = NULL;
TaskHandle_t jugarH = NULL;
TaskHandle_t comerH = NULL;
TaskHandle_t asearH = NULL;
TaskHandle_t comprarH = NULL;



//FUNCTIONS

//Buttons
void buttonTaskLeft(void *p) 
{
  while(1)
  {
    left = true;
    vTaskSuspend(NULL);
    Serial.print("AmogosLeft \r\n");
    vTaskDelay(10 * configTICK_RATE_HZ / 1000);
    
  }
}

void buttonTaskMid(void *p) 
{
  while(1)
  {
    mid = true;
    vTaskSuspend(NULL);
    Serial.print("AmogosMid \r\n");
    vTaskDelay(10 * configTICK_RATE_HZ / 1000);
  }
}

void buttonTaskRight(void *p) 
{
  while(1)
  {
    right = true;
    vTaskSuspend(NULL);
    Serial.print("AmogosRight \r\n");
    vTaskDelay(10 * configTICK_RATE_HZ / 1000);
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

//Complementary


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

//Idle
void idle(void *p)
{
  
  while(1) {
    Serial.print("Phrog");
    vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
    if (left) 
    {
      state = 1;
      left = false;
    }
    else if (mid) {
      state = 2;
      mid = false;
    }
    else if (right) {
      state = 3;
      right = false;
    }

    while(state>0)
    {
      Serial.print(state);
      if (state == 1) {
        state = 0;
        xSemaphoreGive(pasivos);
        vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
      }
      if (state == 2) {
        xSemaphoreGive(salir);
        vTaskDelay(1000 * configTICK_RATE_HZ / 1000);
      }
      if (state == 3) {
        state = 0;
      }
    }


  }
}

void stats(void *p) {
  while(1) {
    if (xSemaphoreTake(pasivos, portMAX_DELAY)) {
      printStats();
    }
  }
}


//Menu

void menu(void *p) {
  while(1) {

    //xSemaphoreTake(salir, portMAX_DELAY)

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

    SemaphoreHandle_t semaforos[5] = {salir, jugar, comer, asear, comprar};

    String semaforosPrint[5] = {"salir", "jugar", "comer", "asear", "comprar"};
    Serial.print(semaforosPrint[menuState]);

    if (enterFlag == true) {
      enterFlag = false;
      xSemaphoreGive(semaforos[menuState]);
    }

  }
}

void jugarTask(void *p) {

}

void comerTask(void *p) {
  while(1) {
    //xSemaphoreTake(salir, portMAX_DELAY)

    if (left) 
    {
      menuState -= 1;
      left = false;
      xSemaphoreGive(comer);
    }
    else if (mid) {
      comerFlag = true;
      mid = false;
    }

    else if (right) {
      comerState += 1;
      right = false;
    }
  }


}

void asearTask(void *p) {

}

void comprarTask(void *p) {

}

void setup() {
  Serial.begin(9600);

  buttonInit();

  //Botones

  // Create two tasks
  pasivos = xSemaphoreCreateBinary();
  salir = xSemaphoreCreateMutex();
  jugar = xSemaphoreCreateMutex();
  comer = xSemaphoreCreateMutex();
  asear = xSemaphoreCreateMutex();
  comprar = xSemaphoreCreateMutex();
  xTaskCreate(buttonTaskLeft, "buttonPressLeft", 4096, NULL, 10, &botonLeft);
  xTaskCreate(buttonTaskMid, "buttonPressMid", 4096, NULL, 10, &botonMid);
  xTaskCreate(buttonTaskRight, "buttonPressRight", 4096, NULL, 10, &botonRight);
  xTaskCreate(idle, "trialtask", 4096, NULL, 200, &idleH);
  xTaskCreate(stats, "menuTask", 4096, NULL, 10, &statsH);


  // Start the FreeRTOS scheduler
  vTaskStartScheduler();

  while(1) {

  }
}


void loop() {
  // This loop will not execute since FreeRTOS is running the tasks
}
