#include<ISADefinitions.h>
#include<ISALiquidCrystal.h>
#include <ISAOLED.h>

#include "tablica.h"

ISALiquidCrystal lcd;
ISAOLED oled;

struct punkt {
  int x;
  int y;
};

struct snake {
  int length = 1;
  punkt tab[512];
};

snake kasia;

void ruch() {
  int a = kasia.length - 2;
  for(int i = a; i >= 0; i--){
    kasia.tab[i + 1] = kasia.tab[i]; 
  }
}

void wygas(int x, int y, int w, int h) {
  int l = 0;
  int k = 0;
  for(l = 0; l < w; ++l) {
    for(k = 0; k < h; ++k) {
      oled.setPixel(x + l, y + k, false);
    }
  }
}

bool czyjest(punkt losowy) { 
  int a = 0;
  a = kasia.length - 1;
  for(int i = a; i >= 0; i--){
    if(kasia.tab[i].x == losowy.x && kasia.tab[i].y == losowy.y){
      return true;
    }   
  }
  return false;  
}  


void when_you_die() {
  Serial.println("Zgon!");
  for(int y = 0; y < 64; ++y) {
    for(int x = 0; x < 128; ++x) {
      for(int bit = 0; bit < 8; ++bit) {
        int maska = 0x80 >> bit;
        int b = mapa[x + (y * 128)] & maska;
        if (b > 0) b = 1;
        oled.setPixel(x, y, b);
      }
    }
  }
  Serial.println("Koniec zgonu!");
  oled.renderAll();
}

punkt point;
punkt losowy;
int score;
int direct = 1;

void setup() {
  Serial.begin(9600);
  oled.begin();
  lcd.begin();

  long seed = 0;
  
  /*pinMode(KEY_UP, INPUT);
  pinMode(KEY_DOWN, INPUT);
  pinMode(KEY_RIGHT, INPUT);
  pinMode(KEY_LEFT, INPUT);*/
  
  point.x = 0;
  point.y = 0;
  
  for (int i = 0; i < 12; i++) {
    seed += analogRead(i);
  }
  
  randomSeed(seed);
  do {
    losowy.x = random(32) * 4;
    losowy.y = random(16) * 4;
  } while(losowy.x == 0 && losowy.y == 0);  
  
  oled.writeRect(losowy.x, losowy.y, 4, 4, true);
  kasia.tab[0] = point;

  oled.renderAll();

  Serial.println("Rozpoczęcie!");
}


void loop() { 
  bool up = false;
  bool down = false;
  bool right = false;
  bool left = false;
  int i = 0;
  for(i = 0; i < 10; ++i) {
    int joy2x = analogRead(JOY2X);
    int joy2y = analogRead(JOY2Y);
    if(joy2y > 700) {
      up = true;
      down = false;
      right = false;
      left = false;
    }
    if(joy2y < 300) {
      up = false;
      down = true;
      right = false;
      left = false;
    }
    if(joy2x > 700) {
      up = false;
      down = false;
      right = true;
      left = false;
    }
    if(joy2x < 300) {
      up = false;
      down = false;
      right = false;
      left = true;
    }
    delay(20);
  }
  
  if(up) {
    direct = 0;
  }
  else if(down) {
    direct = 1;
  }
  else if(right) {
    direct = 2;
  }
  else if(left) {
    direct = 3;
  }

  if(direct == 0) {
    point.y -= 4;
  }
  else if(direct == 1) {
    point.y += 4;
  }
  else if(direct == 2) {
    point.x += 4;
  }
  else if(direct == 3) {
    point.x -= 4;
  }

  if (point.x < 0 || point.x > 124 || point.y < 0 || point.y > 60) {
    when_you_die();
    while(1);
  }

  int a = 0;
  a = kasia.length - 1;
  
  for(int i = a; i >= 0; --i) {
    if(point.x == kasia.tab[i].x && point.y == kasia.tab[i].y && a != 0) {
      when_you_die();
      while(1);
    } 
  }    
    
  if(kasia.tab[0].x == losowy.x && kasia.tab[0].y == losowy.y) {
    kasia.length++;
    //score++;
       
    do {
      losowy.x = random(32) * 4;
      losowy.y = random(16) * 4;
    } while (czyjest(losowy));
    
    /*lcd.print("Wynik: ");
    lcd.print(score);
    lcd.setCursor(0, 0);*/

  }

  wygas(kasia.tab[kasia.length - 1].x, kasia.tab[kasia.length - 1].y, 4, 4);
  ruch(); 
  kasia.tab[0].x = point.x;
  kasia.tab[0].y = point.y;
  
  oled.writeRect(kasia.tab[0].x, kasia.tab[0].y, 4, 4, true);
  oled.writeRect(losowy.x, losowy.y, 4, 4, true);
  
  oled.renderAll();
}
