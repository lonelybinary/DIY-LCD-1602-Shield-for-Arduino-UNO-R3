//Pacman
//Written By: Jean Malha
//http://forum.snootlab.com/viewtopic.php?f=34&t=207
// Modified by Lonely Binary to fit the LCD 1602 Shield

#include <LiquidCrystal.h>


#define VITESSE_PAC 150
#define VITESSE_FANT 2000
#define MAXX 15
#define MAXY 1

#define btnRight     0
#define btnUp        1
#define btnDown      2
#define btnLeft      3
#define btnSelect    4
#define btnNone      5

void(* resetFunc) (void) = 0;
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

#define lbUP 13
#define lbDOWN 12
#define lbLEFT 10
#define lbRIGHT 11
#define lbA 2
#define lbB 3
#define lbBUZZER A0


// Charactère spécifique pacman
byte pacman[8] = {
  B00000,
  B00000,
  B01110,
  B11011,
  B11100,
  B01110,
  B00000,
  B00000
};

// Charactère spécifique fantome
byte fantome[8] = {
  B00000,
  B00000,
  B01110,
  B10101,
  B11111,
  B11111,
  B10101,
  B00000
};

byte point[8] = {
  B00000,
  B00000,
  B00000,
  B01110,
  B01110,
  B00000,
  B00000,
  B00000
};

// Tableau des points à manger
byte points[MAXX+1][MAXY+1];

int xpac=2; // Position de pacman en X (colone)
int ypac=1; //position de pacmanen y (ligne)
int xfant=15;// Position du fantome en X (colone)
int yfant=0;// Position du fantome en Y (ligne)
byte light=true; //Eclairage
long keystruck=0; //dernier appui sur un bouton
long poursuite=0; //dernier movement du fantome
byte partieEnCours=true; // pour eviter de boucler sur la fin
byte vide=false; // pour tester si tout est manger

byte level=0; // niveau
int score=0; // niveau

void bouge(int x,int y) // fonction pour bouger pacman
{
  int oldx=xpac;
  int oldy=ypac;
  if (((xpac+x)>=0)&((xpac+x)<=MAXX)) xpac=xpac+x; //Si pas sorti d'ecran, on change x
  if (((ypac+y)>=0)&((ypac+y)<=MAXY)) ypac=ypac+y;//Si pas sorti d'ecran, on change y
  lcd.setCursor(xpac,ypac); // On se place en nouvelle position
  lcd.write(byte(0)); // et on pose le caractere 0 (Pacman)
  lcd.setCursor(oldx,oldy); // On se place en ancienne position
  if ((xpac!=oldx)||(ypac!=oldy)) lcd.print(" "); // et on efface Pacman (s'il a bougé)
  if(points[xpac][ypac]){
    points[xpac][ypac]=false; // mange le truc
    score++;
  }
  vide=true;
  for (int i=0; i<=MAXX; i=i+1)
    for (int j=0; j<=MAXY; j=j+1)
      if (points[i][j])  vide=false;
  if ((vide)&&(partieEnCours)) gagne();
}

void perdu(){
  lcd.setCursor(0, 0); // on se place au point 0,0 (1ere ligne, 1er caractere)
  lcd.print("***Game Over****"); // on écrit le début du texte de début
  lcd.setCursor(0, 1); // on se place au point 0,1 (2eme ligne, 1er caractere)
  lcd.print("***");
  lcd.print(score);
  lcd.print("***"); 
  delay(2000);
  resetFunc();
}

void gagne()
{
  level++;
  lcd.setCursor(0, 0); // on se place au point 0,0 (1ere ligne, 1er caractere)
  lcd.print("*** Next level ***"); // on écrit le début du texte de début
  lcd.setCursor(0, 1); // on se place au point 0,0 (1ere ligne, 1er caractere)
  lcd.print("*** ");
  lcd.print(level,DEC);
  lcd.print(" ***"); // on écrit le début du texte de début
  delay(2000); // 2 secondes de pause
  initLevel(); //reinitialisation du tableau
}

void poursuis() // fonction pour bouger fantome
{
  int oldx=xfant;
  int oldy=yfant;
  if (yfant<ypac) yfant=yfant+1;
  else if (yfant>ypac) yfant=yfant-1;
  else if (xfant<xpac) xfant=xfant+1;
  else if (xfant>xpac) xfant=xfant-1;
  lcd.setCursor(xfant,yfant); // On se place en nouvelle position
  lcd.write(1); // et on pose le caractere 0 (Fantome)
  lcd.setCursor(oldx,oldy); // On se place en ancienne position
  if ((oldx!=xfant)||(oldy!=yfant)) // et on efface Fantome (s'il a bougé)
  {
    if (points[oldx][oldy]) lcd.write(2); // remplacé par un point si pas mangé
    else lcd.print(" "); // remplacé par un espace si déja magé
  }
}

//initialisation du tableau 
void initLevel(){
  for (int i=0; i<=MAXX; i=i+1)
    for (int j=0; j<=MAXY; j=j+1){
      points[i][j]=true; //initialisation du tableau des trucs à manger
      lcd.setCursor(i-1, j-1); // on se place au point j,i 
      lcd.write(2); // on écrit les points
    }
  lcd.setCursor(xpac,ypac); // On se place en position de départ de pacman
  lcd.write(byte(0)); // et on pose le caractere 0 (Pacman)
  lcd.setCursor(xfant,yfant); // On se place en position de départ du fantome
  lcd.write(1); // et on pose le caractere 1 (fantome)
  poursuite=millis(); // On initialise le timer de poursuite (pour eviter un mouvement immédiat)
  vide=false;
}

void setup() {
  Serial.begin(9600);
  pinMode(lbUP,INPUT_PULLUP);
  pinMode(lbDOWN,INPUT_PULLUP);
  pinMode(lbLEFT,INPUT_PULLUP);
  pinMode(lbRIGHT,INPUT_PULLUP);
  pinMode(lbA,INPUT_PULLUP);
  pinMode(lbB,INPUT_PULLUP);
  pinMode(lbBUZZER,OUTPUT);

  lcd.begin(16, 2);
  lcd.createChar(0, pacman); // creation du caractere pacman et affectation au numéro 0
  lcd.createChar(1, fantome); // creation du caractere de fantome et affectation au numéro 1
  lcd.createChar(2, point); // creation du caractere de point et affectation au numéro 2
  lcd.setCursor(0, 0); // on se place au point 0,0 (1ere ligne, 1er caractere)
  lcd.print("Pacman!"); // on écrit le début du texte de début
  delay (5000); // Splash screen
  initLevel(); // initialisation du tableau
}

void loop() {
  int thisChar = Serial.read();
  switch (thisChar)
  {
  case 'r':
    lcd.scrollDisplayRight();
    break;
  case 'l':
    lcd.scrollDisplayLeft();
    break;
  }
  if ((thisChar>'a')&(thisChar<'z'))
  {
    lcd.setCursor(1,1);
    lcd.write(thisChar);
  }
  if (millis()-keystruck>VITESSE_PAC) // Si plus de 200ms depuis le dernier mouvement de joystick
  {
    int joy=getKey();
    switch (joy)
    {
    case btnNone:
      break;
    case btnLeft:
      Serial.print("Pacman bouge à gauche.\n"); // envoi de controle sur liaison série
      Serial.print(keystruck);
      bouge(-1,0);// déplacement
      keystruck=millis(); // remise à zero du timer de mouvement
      break;
    case btnRight:
      Serial.print("Pacman bouge à droite\n");// envoi de controle sur liaison série
      bouge(1,0);// déplacement
      keystruck=millis(); // remise à zero du timer de mouvement
      break;
    case btnUp:
      Serial.print("Pacman bouge en haut\n");// envoi de controle sur liaison série
      bouge(0,-1);// déplacement
      keystruck=millis(); // remise à zero du timer de mouvement
      break;
    case btnDown:
      Serial.print("Pacman bouge en bas\n");
      bouge(0,1);// déplacement
      keystruck=millis(); // remise à zero du timer de mouvement
      break;
      /*case 4:
       Serial.print("centre\n");
       light=!light; //On inverse le statut d'allumage
       lcd.backLight(light); // on applique
       keystruck=millis(); // remise à zero du timer de mouvement
       break;*/
    default:
      Serial.print(joy); //au cas ou...
      keystruck=millis(); // remise à zero du timer de mouvement
    }; 
  };
  if (millis()-poursuite>VITESSE_FANT/(level+1)+10)
  {
    poursuis();
    poursuite=millis();
  }
  if ((xpac==xfant)&&(ypac==yfant)&&(partieEnCours)) 
  {
    perdu();
  }
}


int getKey() {
  if (digitalRead(lbUP) == 0) return  btnUp;
  if (digitalRead(lbDOWN) == 0) return  btnDown;
  if (digitalRead(lbLEFT) == 0) return  btnLeft;
  if (digitalRead(lbRIGHT) == 0) return  btnRight;
  if (digitalRead(lbA) == 0) return  btnSelect;
  return btnNone;
}
