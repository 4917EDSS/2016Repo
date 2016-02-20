#include "ShooterSub.h"
#include "../Commands/ControlTurretWithJoystickCmd.h"
#include "../RobotMap.h"
#include "../Components/Encoder4917.h"

ShooterSub::ShooterSub(int shooterMotorCanC, int shooterEncoder1C, int shooterEncoder2C, int tiltEncoder1C, int tiltEncoder2C, int rotateEncoder1C, int rotateEncoder2C, int turretRotateC, int turretTiltC, int tiltDownC, int turretCenteredC) :
		Subsystem("ExampleSubsystem")
{
	spinnerMotor = new CANTalon(shooterMotorCanC);
	spinnerMotor->SetControlMode(CANSpeedController::ControlMode::kVoltage);
	spinnerMotor->SetPID(FLYWHEEL_P, FLYWHEEL_I, FLYWHEEL_D);
	spinnerMotor->EnableControl();
	rotateTurretMotor = new Talon(turretRotateC);
	tiltTurretMotor = new Talon(turretTiltC);

	shooterEncoder = new Encoder4917(shooterEncoder1C, shooterEncoder2C);
	tiltEncoder = new Encoder(tiltEncoder1C, tiltEncoder2C);
	rotateEncoder = new Encoder(rotateEncoder1C, rotateEncoder2C);

	turretCentered = new DigitalInput(turretCenteredC);
	tiltDown = new DigitalInput(tiltDownC);

	LiveWindow::GetInstance()->AddActuator("Shooter", "spinnerMotor", spinnerMotor);
//	LiveWindow::GetInstance()->AddSensor("Shooter", "shooterEncoder", shooterEncoder); // TODO: Add this type of encoder
	LiveWindow::GetInstance()->AddActuator("Shooter", "tiltMotor", tiltTurretMotor);
	LiveWindow::GetInstance()->AddSensor("Shooter", "tiltEncoder", tiltEncoder);
	LiveWindow::GetInstance()->AddActuator("Shooter", "rotateMotor", rotateTurretMotor);
	LiveWindow::GetInstance()->AddSensor("Shooter", "rotateEncoder", rotateEncoder);
	LiveWindow::GetInstance()->AddSensor("Shooter", "turretCentered", turretCentered);
	LiveWindow::GetInstance()->AddSensor("Shooter", "tiltDown", tiltDown);



	target = 0;

}


void ShooterSub::Spin(float spinSpeed)
{
	spinnerMotor->Set(spinSpeed*FLYWHEEL_VOLTAGE);
}


void ShooterSub::RotateTurretClockwise(float speed) {
	SetTurretRotate(speed);
}

void ShooterSub::RotateTurretCounterClockwise(float speed) {
	SetTurretRotate(-speed);
}

//The reset function only works if SetTurretRotate is constantly called.
void ShooterSub::SetTurretRotate(float speed)
{
	speed *= ROTATE_SLOW;
	if (!GetTurretCentered())
	{
		std::cout << "CENTERED" << std::endl;
		rotateEncoder->Reset();
	}

	if (GetRotateEnc() > MAX_TURRET_ROTATE_EV && speed > 0) {
		// Hard stop on the RIGHT
		rotateTurretMotor->Set(0.0);
	}
	else if (GetRotateEnc() < -MAX_TURRET_ROTATE_EV && speed < 0) {
		// Hard stop on the LEFT
		rotateTurretMotor->Set(0.0);
	}
	else {
		rotateTurretMotor->Set(speed);
	}
}

//The reset function only works if SetTurretTilt is constantly called.
void ShooterSub::SetTurretTilt(float speed)
{
	if (GetTiltDown())
	{
		tiltEncoder->Reset();
		if (speed < 0) {
			tiltTurretMotor->Set(0.0);

			return;
		}
	}
	if (GetRawTiltEnc() >= MAX_HOOD_HEIGHT_EV && speed > 0) {
		//Stop tilting if at max height
		tiltTurretMotor->Set(0.0);
	}
	else {
		tiltTurretMotor->Set(speed);
	}
}

float ShooterSub::GetTargetOffsetFromCenter()
{
	int TargetIndex = 0, temp = 0;
	float  TargetWidth = 0;

	std::shared_ptr<NetworkTable> gripTable = NetworkTable::GetTable("GRIP/myContoursReport");

	std::vector<double> WidthArray = gripTable->GetNumberArray("width", llvm::ArrayRef<double>());

	if (WidthArray.size() < 1)
	{
		return 0.0;
	}

	for (unsigned int i = 0; i < WidthArray.size(); i++) {
		if (WidthArray[i] > temp)
		{
			temp = i;
		}
	}

	TargetIndex = temp;

	std::vector<double> XArray = gripTable->GetNumberArray("centerX", llvm::ArrayRef<double>());

	TargetWidth = XArray[TargetIndex] - MIDDLE_CAMERA_X;
	return TargetWidth;
}

float ShooterSub::GetTiltEnc() {
	return tiltEncoder->GetDistance();
}

float ShooterSub::GetRotateEnc(){
	return rotateEncoder->GetDistance();
}

float ShooterSub::GetRawTiltEnc() {
	return tiltEncoder->GetRaw();
}

float ShooterSub::GetRawRotateEnc(){
	return rotateEncoder->GetRaw();
}

void ShooterSub::ResetTiltEnc(){
	tiltEncoder->Reset();
}

bool ShooterSub::GetTurretCentered(){
	return !turretCentered->Get();
}

bool ShooterSub::GetTiltDown(){
	return tiltDown->Get();
}

void ShooterSub::SetTarget(int newTarget){
	target = newTarget;
}

void ShooterSub::Update(bool visionActive){
	if(visionActive)
	{
		if (GetTargetOffsetFromCenter() > TARGET_RANGE)
		{
			SetTurretRotate(-0.25);
		}
		else if (GetTargetOffsetFromCenter() < -TARGET_RANGE)
		{
			SetTurretRotate(0.25);
		}
		else
		{
			SetTurretRotate(0.0);
		}
	}
	else
	{
		if(target > GetRotateEnc()+ROTATE_MARGIN)
		{
			SetTurretRotate(1.0);
		}
		else if(target < GetRotateEnc()-ROTATE_MARGIN)
		{
			SetTurretRotate(-1.0);
		}
		else {
			SetTurretRotate(0.0);
		}
	}

}

void ShooterSub::InitDefaultCommand()
{
	SetDefaultCommand(new ControlTurretWithJoystickCmd());
	//SetDefaultCommand(new AimBotCmd());
	//SetDefaultCommand(new SpinupCmd());

	// Set the default command for a subsystem here.
	//SetDefaultCommand(new MySpecialCommand());
}

// Put methods for controlling this subsystem
// here. Call these from Commands.
