#include "OI.h"
#include "RobotMap.h"
#include "Commands/DriveTurnCmd.h"
#include "Commands/SpinupCmd.h"
#include "Commands/BallToIntakeCmd.h"
#include "Commands/BallToShooterGrp.h"
#include "Commands/ToggleDriveLiftCmd.h"
#include "Commands/PrepareShotCmd.h"
#include "Commands/LowBarTransformGrp.h"
#include "Commands/IntakeUntilLimitHitCmd.h"
#include "Commands/EmergencyStopCmd.h"
#include "Commands/DriveStraightCmd.h"
#include "Commands/IntakeUntilLimitHitGrp.h"
#include "Commands/ShootFromBatterGrp.h"
#include "Commands/ShootFromTowerGrp.h"
#include "Commands/ExtendClimberCmd.h"
#include "Commands/RetractClimberCmd.h"
#include "Commands/ZeroRotateEncoderCmd.h"

OI::OI()
{
	dController = new Joystick(DControllerDriverPort);
	oController = new Joystick(OControllerOperatorPort);

	oFlywheelSpinBtn = new JoystickButton(oController, OFlywheelSpinBtn);
	oFlywheelSpinBtn->WhenPressed(new SpinupCmd);

	oHighGoalBtn = new JoystickButton(oController, OHighGoalBtn);
	oHighGoalBtn->WhenPressed(new BallToShooterGrp);

	dLiftShifterBtn = new JoystickButton(dController, DLiftShifterBtn);
	dLiftShifterBtn->WhenPressed(new ToggleDriveLiftCmd);

	dStraightForwardBtn = new JoystickButton(dController, DStraightForwardBtn);
	dStraightForwardBtn->WhileHeld(new DriveStraightCmd(10000, 0.7));

	dStraightBackwardBtn = new JoystickButton(dController, DStraightBackwardBtn);
	dStraightBackwardBtn->WhileHeld(new DriveStraightCmd(-10000, 0.7));

	oPrepareShotBtn = new JoystickButton (oController, OPrepareShotBtn);
	oPrepareShotBtn->WhenPressed(new PrepareShotCmd);

	oLowBarTransformBtn = new JoystickButton(oController, OLowBarTransformBtn);
	oLowBarTransformBtn->WhenPressed(new LowBarTransformGrp);

	oIntakeUntilLimitBtn = new JoystickButton(oController, OIntakeUntilLimitBtn);
	oIntakeUntilLimitBtn->WhenPressed(new IntakeUntilLimitHitGrp);

	oShootFromLeftBatterBtn = new JoystickButton(oController, OShootFromLeftBatterBtn);
	oShootFromLeftBatterBtn->WhenPressed(new ShootFromBatterGrp(LEFT_SIDE));

	//oShootFromRightBatterBtn = new JoystickButton(oController, OShootFromRightBatterBtn);
	//oShootFromRightBatterBtn->WhenPressed(new ShootFromBatterGrp(RIGHT_SIDE));

	oShootFromTowerBtn = new JoystickButton(oController, OShootFromTowerBtn); // Todo: remove left or right side batter shot button
	oShootFromTowerBtn->WhenPressed(new ShootFromTowerGrp());

	oExtendClimberBtn = new JoystickButton(oController, OExtendClimberBtn);
	oExtendClimberBtn->WhileHeld(new ExtendClimberCmd);

	oZeroRotateEncoderBtn = new JoystickButton(oController, OZeroRotateEncoderBtn);
	oZeroRotateEncoderBtn->WhenPressed(new ZeroRotateEncoderCmd);

	//Emergency Stop commands.
	oEmergencyStopBtn1 = new JoystickButton(oController, OEmergencyStopBtn1);
	oEmergencyStopBtn1->WhenPressed(new EmergencyStopCmd);

	dEmergencyStopBtn1 = new JoystickButton(dController, DEmergencyStopBtn1);
	dEmergencyStopBtn1->WhenPressed(new EmergencyStopCmd);

	oEmergencyStopBtn2 = new JoystickButton(oController, OEmergencyStopBtn2);
	oEmergencyStopBtn2->WhenPressed(new EmergencyStopCmd);

	dEmergencyStopBtn2 = new JoystickButton(dController, DEmergencyStopBtn2);
	dEmergencyStopBtn2->WhenPressed(new EmergencyStopCmd);
	//oIntakeBtn = new JoystickButton(oController, OIntakeBtn);
	//oIntakeBtn->WhenPressed(new IntakeUntilLimitHitCmd());


	// Process operator interface input here.
}

float OI::getStick(Joystick* controller, int axis, float sensitivity){
	float rawInput = controller->GetRawAxis(axis);
	float adjustedInput = pow(fabs(rawInput), sensitivity);
	if (rawInput > CONTROLLER_DEADZONE_VALUE)
	{
		return adjustedInput;
	}
	else if(rawInput < -CONTROLLER_DEADZONE_VALUE)
	{
		return -adjustedInput;
	}
	else
	{
		return 0.0;
	}
}

float OI::DGetRightVer(float sensitivity){
	return -getStick(dController, DRightVerticalAxis, sensitivity);
}

float OI::DGetLeftVer(float sensitivity){
	return -getStick(dController, DLeftVerticalAxis, sensitivity);
}

float OI::DGetLeftHor(float sensitivity){
	return getStick(dController, DLeftHorizontalAxis, sensitivity);
}

float OI::DGetRightHor(float sensitivity){
	return getStick(dController, DRightHorizontalAxis, sensitivity);
}

float OI::OGetRightVer(float sensitivity){
	return -getStick(oController, ORightVerticalAxis, sensitivity);
}

float OI::OGetLeftVer(float sensitivity){
	return -getStick(oController, OLeftVerticalAxis, sensitivity);
}

float OI::OGetLeftHor(float sensitivity){
	return getStick(oController, OLeftHorizontalAxis, sensitivity);
}

float OI::OGetRightHor(float sensitivity){
	return getStick(oController, ORightHorizontalAxis, sensitivity);
}

int OI::OGetPov() {
	return oController->GetPOV();
}
