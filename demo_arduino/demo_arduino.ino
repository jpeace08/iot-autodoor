#include <Wire.h>
#include <Stepper.h>
#define OPEN 1;
#define CLOSE 0;

const int stepsPerRevolution = 640;// set so buoc cua dong co buoc 28byj48
Stepper myStepper(stepsPerRevolution, 2,4,3,5); // tao doi tuong dieu khien moto truyen vao so buoc va cac chan dieu khien 

char prevVal = 'h';// tao  bien nhan gia tri tra ve tu server
int sensorPin = 8;// chan cua cam bien
int sensorVal = 0;// bien luu gia tri truyen ve cua cam bien
int stateDoor = CLOSE;// khoi tao trang thai cua cong
bool isEnable = false;// bien luu trang thai ngat

void setup() {
	Serial.begin(9600);
	myStepper.setSpeed(50);//set toc do moto vong/phut
	pinMode(8,INPUT);
	pinMode(12,OUTPUT);//set che do cua cac chan
	Wire.begin(8); 

	//handler door at 'ban dem' :)
	Wire.onReceive(receiveEvent);
}

void loop() {
	//Handler door at 'ban ngay' :)
	if(isEnable)
	{
		handleSensor();// neu trang thai ngat la true thi su dung cam bien de dong mo cong
	}
	if(prevVal!='g')
	{


	if(prevVal == '0') {// dong cong khi co yeu cau
		myStepper.step(-stepsPerRevolution);
		stateDoor = CLOSE;

	}
	else if(prevVal == '1') {// mo cong khi nhan duoc yeu cau
		myStepper.step(stepsPerRevolution);
		stateDoor = OPEN;
	}
	else if(prevVal == '3') {//tat ngat khi co yeu cau tu server
		isEnable = true; 
	}
	else if(prevVal == '4') {// bat ngat khi co yeu cau
		isEnable = false; 
		myStepper.step(stepsPerRevolution);
		stateDoor = CLOSE;
	}

	prevVal = 'g';
	//  delay(200);
	}
}

void handleSensor(){
	sensorVal = digitalRead(sensorPin);
	if(sensorVal == 0 && stateDoor != 1){// neu cam bien co tin hieu va  cong dang dong thi mo cua
		openDoor();// mo cua
		stateDoor = OPEN;// chuyen trang thai sang mo
	}
	if(sensorVal == 1 && stateDoor != 0)// neu cam bien ko con tin hieu va cong dang mo thi dong cua  
	{
		closeDoor();// dong cua
		stateDoor = CLOSE;// chuyen trang thai sang dong
	}  
}

void receiveEvent(int howMany) {// ham nhan du lieu tu esp

	char c;
	while (0 <Wire.available()) {
		c = Wire.read();

	}

	prevVal = c;

}


// function that executes whenever data is requested from master
void requestDoor() {

}

void openDoor() {//ham mo cua
	myStepper.step(stepsPerRevolution);
}

void closeDoor() {//ham dong cua
	myStepper.step(-stepsPerRevolution);
}
