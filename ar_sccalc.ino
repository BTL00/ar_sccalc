#include <LiquidCrystal_I2C.h>

/*
 *  Arduino Scientific Calculator
 *  by Piotr Bomba
 *  bombapiotr@mail.com
 *  FIRST GITHUB UPLOAD VERSION
 */

char clicked;
byte equation[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte sndline[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


double stack[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char commands[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};



byte display_0[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
byte display_1[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


//level 64
char lexpr[12][64];
char rexpr[12][64];

float m0;
float m1;



//precedence table

char precedence[5][2] =   {{'+',0},
                            {'-',0},
                            {'*',1},
                            {'/',1},
                            {'^',2}};
                            //{'10^',0},
                            //{'0b',0}};
//oczywisćie poprawić





char cursorPosition = 0;
char screenOffset = 0;
byte hexChar[8] = {
  0b11111,
  0b10001,
  0b11111,
  0b00000,
  0b10100,
  0b01000,
  0b10100,
  0b00000
};


byte binChar[8] = {
  0b11111,
  0b10001,
  0b11111,
  0b00000,
  0b11111,
  0b10100,
  0b11100,
  0b00000
};

byte arrowLeft[8] = {
  0b00000,
  0b00100,
  0b01000,
  0b11111,
  0b01000,
  0b00100,
  0b00000,
  0b00000
};

byte arrowRight[8] = {
  0b00000,
  0b00100,
  0b00010,
  0b11111,
  0b00010,
  0b00100,
  0b00000,
  0b00000
};


byte memory0[8] = {
  0b11011,
  0b10101,
  0b10001,
  0b10001,
  0b00000,
  0b01110,
  0b01010,
  0b01110
};

byte memory1[8] = {
  0b11011,
  0b10101,
  0b10001,
  0b10001,
  0b00000,
  0b00100,
  0b00100,
  0b00100
};

byte expon[8] = {
  0b10111,
  0b10101,
  0b10101,
  0b10111,
  0b00000,
  0b00100,
  0b01010,
  0b00000
};

boolean refresh = true;
LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
  Serial.begin(38400);
    lcd.init();                     
    lcd.backlight();
    lcd.cursor();
  lcd.createChar(0, hexChar);
  lcd.createChar(1, binChar);
  lcd.createChar(2, arrowLeft);
  lcd.createChar(3, arrowRight);
    lcd.createChar(4, memory0);
  lcd.createChar(5, memory1);
    lcd.createChar(6, expon);

  lcd.home();




}

void loop() {

  // LCD first line display equation and cursor
  // LCD second line display

    lcd.setCursor(cursorPosition, 0);



    if(doINeedToDoMath(cursorPosition)) {
      doMath();
          sndline[1] = 0xFF;
    }else{
          sndline[2] = 0;
    }


    if(m0 != 0) {
          sndline[2] = 0xFC;
    }else{
          sndline[2] = 0;

    }
    if(m1 != 0) {
          sndline[3] = 0xFD;
    }else{
          sndline[3] = 0;

    }

        if(screenOffset < 0) {
      screenOffset = 0;
    }
    if(screenOffset > 0) {
    sndline[0] = 0xFF;
    }else{
          sndline[0] = 0;

    }
      if(equation[16] !=0 || screenOffset > 0) {
    sndline[15] = 0xFE;
    }else{
          sndline[15] = 0;

    }
    if(refresh) {
      lcd.setCursor(0,0);
    for(int i = screenOffset ; i <= screenOffset + 15; i++) {   
    
      if(equation[i] != 0 && equation[i] <= 0xEF) {
        lcd.write(equation[i]);  
      }else{

        switch(equation[i]) {
          case 0:
            lcd.write(' ');
          break;
          case 0xFF:
            lcd.write((uint8_t) 0);
          break;
          case 0xFE:
            lcd.write((uint8_t) 1);
          break;     
                   
          case 0xFD:
            lcd.write((uint8_t) 5);
          break;
          case 0xFC:
            lcd.write((uint8_t) 4);
          break;   
                    case 0xFB:
            lcd.write((uint8_t) 6);
          break;       
        }
        
      }
    }
      lcd.setCursor(0,1);
    for(int i = 0; i <=  15; i++) {
      if(sndline[i] != 0 && sndline[i] <= 0xEF) {
        lcd.write(sndline[i]);  
      }else{

        switch(sndline[i]) {
          case 0:
            lcd.write(' ');
          break;
          case 0xFF:
            lcd.write((uint8_t) 2);
          break;
          case 0xFE:
            lcd.write((uint8_t) 3);
          break;      

                        case 0xFD:
            lcd.write((uint8_t) 5);
          break;
          case 0xFC:
            lcd.write((uint8_t) 4);
          break;  
        
        }
         
      }
    }
    }


  
  // buttons get clicked -- serial conversion
  if(Serial.available()) {
    clicked = Serial.read(); 

    Serial.print(clicked);
  }
  // do actions to keys
  if(clicked != 0) {
    refresh = true;
  }else{
    refresh = false;
  }
  if(clicked >= 0x30 && clicked <= 0x39) {
    equation[cursorPosition] = clicked; // (to ascii)
    cursorPosition++;
  }
  switch(clicked) {
      case '+':
    equation[cursorPosition] = '+';
        cursorPosition++;
    break;
     case 0x2D:
    equation[cursorPosition] = '-';
        cursorPosition++;
    break;   
    case 0x2A:
    equation[cursorPosition] = '*';
        cursorPosition++;
    break;
     case 0x2F:
    equation[cursorPosition] = '/';
        cursorPosition++;
    break;   
      case 0x3D:
    equation[cursorPosition] = '=';
        cursorPosition++;
      //do math!
    break;
    case 0x65: //"DEL" e
        for(int i = cursorPosition; i <=62; i++) {
          equation[i] = equation[i + 1];      
        }
        cursorPosition--;
      //do math!
    break;     
    case 0x64: //"-->" d
        cursorPosition++;
      //do math!
    break;
    case 0x61: //"<--" a
        cursorPosition--;
      //do math!
    break;
    case 0x74: //"<--" 0b
        equation[cursorPosition] = 0xFE;
        cursorPosition++;
      //do math!
    break;
    case 0x79: //"<--" 0x
        equation[cursorPosition] = 0xFF;
        cursorPosition++;
      //do math!
    break;
        case 0x28: //"(" 0x
        equation[cursorPosition] = 0x28;
        cursorPosition++;
      //do math!
    break;
        case 0x29: //")" 0x
        equation[cursorPosition] = 0x29;
        cursorPosition++;
      //do math!
    break;
        case 0x5E: //"^" 0x
        equation[cursorPosition] = 0x5E;
        cursorPosition++;
      //do math!
    break;
    case 0x67: //"sqr" 0x
        equation[cursorPosition] = 's';
        cursorPosition++;
                equation[cursorPosition] = 'q';
        cursorPosition++;
                equation[cursorPosition] = 'r';
        cursorPosition++;
                        equation[cursorPosition] = '(';
        cursorPosition++;
      //do math!
    break;
    case 0x68: //"cqr" 0x
        equation[cursorPosition] = 'c';
        cursorPosition++;
                equation[cursorPosition] = 'q';
        cursorPosition++;
                equation[cursorPosition] = 'r';
        cursorPosition++;
                        equation[cursorPosition] = '(';
        cursorPosition++;
      //do math!
    break;
    case 0x2E: //"." 0x
        equation[cursorPosition] = 0x2E;
        cursorPosition++;
      //do math!
    break;
        break;
    case 0x63: //"c" 0x
        screenOffset++;
      //do math!
    break;
        break;
    case 0x7A: //"z" 0x
        screenOffset--;
      //do math!
    break;

        case 'n': //"n" 0x
        equation[cursorPosition] = 0xFC;
        cursorPosition++;      //do math!
    break;

        case 'm': //"m" 0x
        equation[cursorPosition] = 0xFD;
        cursorPosition++;      //do math!
    break;
            case 'r': //"m" 0x
        equation[cursorPosition] = 0xFB;
        cursorPosition++;      //do math!
    break;


    
                case 'A': //"m" 0x
        equation[cursorPosition] = 'A';
        cursorPosition++;      //do math!
    break;
                case 'B': //"m" 0x
        equation[cursorPosition] = 'B';
        cursorPosition++;      //do math!
    break;
                case 'C': //"m" 0x
        equation[cursorPosition] = 'C';
        cursorPosition++;      //do math!
    break;

                case 'D': //"m" 0x
        equation[cursorPosition] = 'D';
        cursorPosition++;      //do math!
    break;

                case 'E': //"m" 0x
        equation[cursorPosition] = 'E';
        cursorPosition++;      //do math!
    break;

                case 'F': //"m" 0x
        equation[cursorPosition] = 'F';
        cursorPosition++;      //do math!
    break;




                case 'T': //"m" 0x
        equation[cursorPosition] = '!';
        cursorPosition++;      //do math!
    break;
                case 'Y': //"m" 0x
        equation[cursorPosition] = '&';
        cursorPosition++;      //do math!
    break;
                case 'U': //"m" 0x
        equation[cursorPosition] = '|';
        cursorPosition++;      //do math!
    break;     
                    case 'I': //"m" 0x
        equation[cursorPosition] = '>';
        cursorPosition++;      //do math!
                equation[cursorPosition] = '>';
        cursorPosition++;      //do math!
    break;
                    case 'O': //"m" 0x
        equation[cursorPosition] = '<';
        cursorPosition++;      //do math!
                equation[cursorPosition] = '<';
        cursorPosition++;      //do math!
    break;
      

  }
  clicked = 0;
}




boolean doINeedToDoMath(int postion) {
  if(equation[postion-2] == '=' && equation[postion-1] == '=')
      return true;
  return false;
}

void doMath() {
    char resultFormat;
      for(int i = 0; i < 64; i++) {
        if(equation[i-1] == 0xFE && equation[i] == '=' && equation[i+1] == '=') {
              printArray(equation, 64);
                equation[i-1] = 0;
                equation[i] = 0;
                equation[i+1] = 0;

        parseExp(equation, 0);

        resultFormat = 2;
       
        }else if(equation[i-1] == 0xFF && equation[i] =='=' && equation[i+1] == '=') {
                printArray(equation, 64);
                equation[i-1] = 0;
                equation[i] = 0;
                equation[i+1] = 0;
        parseExp(equation, 0);

        resultFormat = 16;
          
        }else if(equation[i] == '=' && equation[i+1] == '=') {
                printArray(equation, 64);

                equation[i] = 0;
                equation[i+1] = 0;
      //  lcd.setCursor(1, 4);              
          Serial.println(parseExp(equation, 0)); // zapisz w 2ndline

        resultFormat = 10;
          
        }
}
  
}


void printArray(char temp[], char st) {
Serial.print("{");
  for(int i = 0; i < st; i++ ) {
   Serial.write(temp[i]);
   Serial.print(", ");
   
  }
Serial.print("}");
Serial.print("\n");

}


float parseExp(char expr[], char level) {
  int iflag = 0;
  char op = 0;
  float number;
  int nflag;
  int sizof = sizeof(expr) / sizeof(expr[0]);
      Serial.print("SIZOF ");
      Serial.println(sizof, DEC);

  for(int i = 0; i <= sizof; i++) {
    Serial.print("expr[i] "); Serial.println(expr[i]);
    
    if(expr[i] == '(') {
      Serial.println("iflag");
      iflag =  i;
    }
    if(iflag > 0 && expr[i] == '0') {
            Serial.println("iflag > 0 && expr[i] == '0'");

       char tempCharArray[i - iflag];
       for(int j = i; i <= iflag; i++) {
        tempCharArray[iflag - i] = expr[j];
        expr[j] = 0;
       }
       return parseExp(tempCharArray, level + 1);
    }



    if(expr[i] == '+') {
      Serial.println("expr[i] == '+'");
    for(char n = 0; n < i; n++) {
      lexpr[level][n] = 0;
    }
        Serial.print("lexpr["); Serial.print(level, DEC); Serial.print("] (all 0s) ");
        printArray(lexpr[level], i);

    for(char o = 0; o < i; o++) {
      lexpr[level][o] = expr[o];
    }
    
            Serial.print("lexpr["); Serial.print(level, DEC); Serial.print("] (created) ");
    printArray(lexpr[level], i);

    
    for(char n = 64-i; n < 64; n++) {
      rexpr[level][n] = 0;
    }
        Serial.print("rexpr["); Serial.print(level, DEC); Serial.print("] (all 0s) ");

       printArray(rexpr[level], 64-i);

    for(char o = i; o < 64; o++) {

      rexpr[level][o - i - 1] = expr[o];
    }
                Serial.print("rexpr["); Serial.print(level, DEC); Serial.print("] (created) ");
           printArray(rexpr[level], 64-i);
    return parseExp(lexpr[level], level + 1) + parseExp(rexpr[level], level+1);
    }

    
   // if(expr[i] == '+') return parseExp(lexpr) + parseExp(rexpr);
   // if(expr[i] == '-') return parseExp(lexpr) - parseExp(rexpr); // jeśli minus to chyba też plus, tylko liczba po prawo ma mieć inverted znak, ale nie jestem pewien wiec tego nie robie
   // if(expr[i] == '*') return parseExp(lexpr) * parseExp(rexpr);
   // if(expr[i] == '/') return parseExp(lexpr) / parseExp(rexpr);
   // if(expr[i] == '^') return pow(parseExp(lexpr), parseExp(rexpr));

    

      Serial.print("parseExp: i: ");
        Serial.println(i, DEC);

  }


    //if this doesnt contain operator return number
   return readNumber(expr);
  
}



char toNumeric(char a) {

  const char table[10][2] = {{'0', 0},
                             {'1', 1},
                             {'2', 2},
                             {'3', 3},
                             {'4', 4},
                             {'5', 5},
                             {'6', 6},
                             {'7', 7},
                             {'8', 8},
                             {'9', 9}};

  for(int i = 0; i < 10; i++) {
    if(table[i][0] == a)   return table[i][1];

  }
  return 255;
  
}


float readNumber(char nar[]) {
  
  int flag = 0;
  double ret = 0;
  int power = 0;
  long multiplier = 1;
  for(int i = (sizeof(nar) / sizeof(nar[0])) ; i >= 0; i--) {
  Serial.print("sizof(nar)/[0]: "); Serial.println(sizeof(nar) / sizeof(nar[0]), DEC);
  Serial.print("readNumber: i:");
  Serial.print(i, DEC);
  Serial.print(" toNumeric: ");
  Serial.println(toNumeric(nar[i]), DEC);

    multiplier = 1;
      
    if(!(toNumeric(nar[i]) == -1)) {
    for(byte h = 0; h < power; h++) {
        multiplier = multiplier * 10;
    }
    Serial.print(" mul:");
    Serial.println(multiplier, DEC);
    ret += toNumeric(nar[i]) * multiplier;
    power++;
       
    }else{
      Serial.println(" -1!");
    }
  }
    
  
  Serial.print("readNumber(");
  Serial.print(sizeof(nar) / sizeof(nar[0]), DEC);
  Serial.print("): return: ");
  Serial.println(ret, DEC);
 return ret;
 
 }
  





