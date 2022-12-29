const int rGPIO = 13;
const int lGPIO = 14;

void setup() {
	Serial.begin(115200);

	pinMode(rGPIO, INPUT_PULLUP);
	pinMode(lGPIO, INPUT_PULLUP);

	initMotor();
}

long speedFwd = 128; // speed for 0~768
long speedCurve = 128; // speed for 0~768

void loop() {
	int right = digitalRead(rGPIO);
	int left = digitalRead(lGPIO);
	printf("%d\n", right<<1|left);

	if (right==0 && left==0) {
		goForward(speedFwd);
	} else if (right==0&& left==1 ) {
		turnRight(speedCurve);
	} else if (right==1 && left==0) {
		turnLeft(speedCurve);
	} else if (right==1 && left==1) {
		stopMotor();
	}  
}
