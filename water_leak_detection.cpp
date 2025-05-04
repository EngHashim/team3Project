#include <SoftwareSerial.h>               // الوسيلة بين النصية والاردوينو

#define flowPin1 2  // حساس المدخل
#define flowPin2 3  // حساس المخرج
#define ledPin 7
#define buzzerPin 8

SoftwareSerial sim800(10, 11); // RX, TX لـ SIM800L

volatile int flowCount1 = 0;
volatile int flowCount2 = 0;

bool alertSent = false;       // ما يرسل على طول           

void setup() {
  pinMode(flowPin1, INPUT_PULLUP);
  pinMode(flowPin2, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  attachInterrupt(digitalPinToInterrupt(flowPin1), countFlow1, RISING);   // يخلي الحساسات تقيس الزيادة
  attachInterrupt(digitalPinToInterrupt(flowPin2), countFlow2, RISING);

  Serial.begin(9600);  // تعريف النصية
  sim800.begin(9600);

  delay(3000); // انتظار تجهيز SIM800L
  sim800.println("AT");          // يخلي الرسالة في مود النص
  delay(1000);
}

void loop() {
  flowCount1 = 0;        // قيمة ابتدائية بصفر
  flowCount2 = 0;

  // قياس التدفق خلال ثانية واحدة
  delay(1000);

  int f1 = flowCount1; // قيمة المدخل
  int f2 = flowCount2; // قيمة المخرج

  

  int diff = abs(f1 - f2);              

  if (diff > 5 && !alertSent) {  // إذا الفرق أكبر من 5 نبضات = تسرب
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    sendSMS("!Alert!!!! There is Water Leakage");
    alertSent = true;               // يصير يرسل
  }
else{
    digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

    alertSent = false;               // يوقف ارسال
}
  delay(1000); // راحة ثانية
  
}

void countFlow1() {                // يقيس الزيادة
  flowCount1++;
}

void countFlow2() {
  flowCount2++;
}

void sendSMS(String message) {
  sim800.println("AT+CMGF=1");                // تعريف التواصل مع الاردوينو
  delay(1000);
  sim800.println("AT+CMGS=\"‪+966502197600‬\""); // ضع رقمك هنا
  delay(1000);
  sim800.print(message);
  delay(500);
  sim800.write(26); // رمز CTRL+Z لإنهاء الرسالة
  delay(5000);
}
