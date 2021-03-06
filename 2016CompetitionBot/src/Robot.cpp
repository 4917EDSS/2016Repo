#include "WPILib.h"
#include <unistd.h>
#include <iostream>
#include "Commands/AutoGenericDuoGrp.h"
#include "COmmands/AutoDefaultGrp.h"
#include "Commands/AutoLowBarGrp.h"
#include "Commands/AutoPortcullisGrp.h"
#include "Commands/AutoChevalGrp.h"
#include "Commands/AutoRampartsGrp.h"
#include "Commands/AutoMoatGrp.h"
#include "Commands/AutoDrawbridgeGrp.h"
#include "Commands/AutoSallyPortGrp.h"
#include "Commands/AutoRockWallGrp.h"
#include "Commands/AutoRoughTerrainGrp.h"
#include "Commands/AutoPosition1ShootGrp.h"
#include "Commands/AutoPosition2ShootGrp.h"
#include "Commands/AutoPosition2ShootLeftGrp.h"
#include "Commands/AutoPosition3ShootGrp.h"
#include "Commands/AutoPosition3ShootRightGrp.h"
#include "Commands/AutoPosition4ShootGrp.h"
#include "Commands/AutoPosition5ShootGrp.h"

#include "Commands/IntakeUntilLimitHitCmd.h"
#include "Commands/BallToIntakeCmd.h"
#include "Commands/CameraUpdateCmd.h"
#include "Commands/ControlIntakeWithJoystickCmd.h"
#include "Commands/ControlTurretWithJoystickCmd.h"
#include "Commands/DriveStraightCmd.h"
#include "Commands/DriveTurnCmd.h"
#include "Commands/DriveWithJoystickCmd.h"
#include "Commands/ResetDrivetrainEncCmd.h"
#include "Commands/TurretRotateZeroEncCmd.h"
#include "Commands/IntakeUntilLimitHitCmd.h"
#include "Commands/SetIntakeHeightCmd.h"
#include "Commands/SpinupCmd.h"
#include "Commands/ToggleDriveLiftCmd.h"
#include "Commands/SetTurretRotateCmd.h"
#include "Commands/ZeroRotateEncoderCmd.h"
#include "CommandBase.h"
#include "RobotMap.h"
#include "AHRS.h"


class Robot: public IterativeRobot
{
private:
	void SetSmartDashboardAutoOptions();
	void SendCmdAndSubInfoToSmartDashboard();
	void SetSmartDashboardDriverContent();
	void UpdateSmartDashboard();

	Command* autonomousCommand;
	Command* autonomousDefenceCommand;
	Command* autonomousLocationCommand;
	SendableChooser* autoDefenceOptions;
	SendableChooser* autoLocationOptions;
	AHRS *ahrs;

	// Constants to run the GRIP process
	const char *JAVA = "/home/lvuser/frc/JRE/bin/java";
	char *GRIP_ARGS[5] = { "java", "-jar", "/home/lvuser/grip.jar",
				"/home/lvuser/project.grip", NULL };


	void RobotInit()
	{
		// Running GRIP in a new process
		// TODO:
		// THIS PROGRAM HAS YET TO WORK AS IS. Commenting out the rest of this function
		// from CommandBase::init() down to the bottom makes it work.
		if (fork() == 0) {
			system("/home/lvuser/grip &");/*
			if (execv(JAVA, GRIP_ARGS) == -1) {
				perror("Error running GRIP");
			}*/
		}

		CommandBase::init();

		//the camera name (ex "cam0") can be found through the roborio web interface
//		CameraServer::GetInstance()->SetQuality(50);
//		CameraServer::GetInstance()->StartAutomaticCapture(CAMERA_NAME);

		// Send to the Smart Dashboard a list of auto commands to choose from
		SetSmartDashboardAutoOptions();
		SetSmartDashboardDriverContent();
		SendCmdAndSubInfoToSmartDashboard();	// Enable for debugging
	}

	/**
     * This function is called once each time the robot enters Disabled mode.
     * You can use it to reset any subsystem information you want to clear when
	 * the robot is disabled.
     */
	void DisabledInit()
	{
	}

	void DisabledPeriodic()
	{
		Scheduler::GetInstance()->Run();
	}

	/**
	 * This autonomous (along with the chooser code above) shows how to select between different autonomous modes
	 * using the dashboard. The sendable chooser code works with the Java SmartDashboard. If you prefer the LabVIEW
	 * Dashboard, remove all of the chooser code and uncomment the GetString code to get the auto name from the text box
	 * below the Gyro
	 *
	 * You can add additional auto modes by adding additional commands to the chooser code above (like the commented example)
	 * or additional comparisons to the if-else structure below with additional strings & commands.
	 */
	void AutonomousInit()
	{
		/* std::string autoSelected = SmartDashboard::GetString("Auto Selector", "Default");
		if(autoSelected == "My Auto") {
			autonomousCommand = new MyAutoCommand();
		} else {
			autonomousCommand = new ExampleCommand();
		} */

		CommandBase::rDrivetrainSub->ResetDrive();
		CommandBase::rShooterSub->ResetRotate();

		autonomousDefenceCommand = (Command *)autoDefenceOptions->GetSelected();
		autonomousLocationCommand = (Command *)autoLocationOptions->GetSelected();
		autonomousCommand = new AutoGenericDuoGrp(autonomousDefenceCommand, autonomousLocationCommand);
		autonomousCommand->Start();
	}

	void AutonomousPeriodic()
	{
		Scheduler::GetInstance()->Run();
		SmartDashboard::PutNumber("Left Wheel dis Encoder", CommandBase::rDrivetrainSub->GetLeftEnc());
		SmartDashboard::PutNumber("Right Wheel dis Encoder", CommandBase::rDrivetrainSub->GetRightEnc());

		//UpdateSmartDashboard();//TODO:Get rid of this
	}

	void TeleopInit()
	{
		// This makes sure that the autonomous stops running when
		// teleop starts running. If you want the autonomous to
		// continue until interrupted by another command, remove
		// this line or comment it out.
		if (autonomousCommand != NULL)
			autonomousCommand->Cancel();
	}

	void TeleopPeriodic()
	{
		Scheduler::GetInstance()->Run();
		UpdateSmartDashboard();
	}

	void TestPeriodic()
	{

		LiveWindow::GetInstance()->Run();
	}
};

void Robot::SendCmdAndSubInfoToSmartDashboard()
{
	// See what command is running
#if 0
	SmartDashboard::PutData(Scheduler::GetInstance());

	// See what command requires a particular subsystem
	if(CommandBase::rDrivetrainSub)
		SmartDashboard::PutData(CommandBase::rDrivetrainSub);
	if(CommandBase::rIntakeSub)
		SmartDashboard::PutData(CommandBase::rIntakeSub);
	if(CommandBase::rShooterSub)
		SmartDashboard::PutData(CommandBase::rShooterSub);
	//if(CommandBase::rCameraSub)
		//SmartDashboard::PutData(CommandBase::rCameraSub);
	if(CommandBase::rHopperSub)
		SmartDashboard::PutData(CommandBase::rHopperSub);

	// See status of each command and be able to run it manually
	SmartDashboard::PutData("Ball To Intake", new BallToIntakeCmd());
	SmartDashboard::PutData("Ball To Shooter", new BallToShooterCmd());
	//SmartDashboard::PutData("Camera Update", new CameraUpdateCmd());
	SmartDashboard::PutData("Joystick Intake Ctrl", new ControlIntakeWithJoystickCmd());
	SmartDashboard::PutData("Joystick Turret Ctrl", new ControlTurretWithJoystickCmd());
	SmartDashboard::PutData("Drive Straight", new DriveStraightCmd((Preferences::GetInstance())->GetInt("DriveStraightMM", 0.0), (Preferences::GetInstance())->GetFloat("DriveStraightSpeed", 0.0)));
	SmartDashboard::PutData("Drive Turn", new DriveTurnCmd((Preferences::GetInstance())->GetInt("DriveTurnDegrees")));
	std::cout << Preferences::GetInstance()->GetInt("DriveTurnDegrees") << std::endl;
	SmartDashboard::PutData("Joystick Drive", new DriveWithJoystickCmd());
	SmartDashboard::PutData("Intake Until Limit Hit", new IntakeUntilLimitHitCmd());
//	SmartDashboard::PutData("Set Intake Height", new SetIntakeHeightCmd());	// Needs parameters
	SmartDashboard::PutData("Spinup", new SpinupCmd());
	SmartDashboard::PutData("Toggle Drive Lift", new ToggleDriveLiftCmd());
	SmartDashboard::PutData("ResetDrive Encoders", new ResetDrivetrainEncCmd());
	SmartDashboard::PutData("Rotate Turret to Value", new SetTurretRotateCmd(Preferences::GetInstance()->GetInt("TurretRotateEncoder")));
#endif
	SmartDashboard::PutData("Zero Turret encoder", new TurretRotateZeroEncCmd());
}

void Robot::SetSmartDashboardAutoOptions()
{
	autoDefenceOptions = new SendableChooser();
	autoDefenceOptions->AddDefault("Do Nothing", new AutoDefaultGrp());
	autoDefenceOptions->AddObject("Low Bar Defence", new AutoLowBarGrp());
	autoDefenceOptions->AddObject("Ramparts Defence", new AutoRampartsGrp());
	autoDefenceOptions->AddObject("Moat Defence", new AutoMoatGrp());
	autoDefenceOptions->AddObject("Rock Wall Defence", new AutoRockWallGrp());
	autoDefenceOptions->AddObject("Rough Terrain Defence", new AutoRoughTerrainGrp());

	autoLocationOptions = new SendableChooser();
	autoLocationOptions->AddDefault("Do Nothing", new AutoDefaultGrp());
	autoLocationOptions->AddObject("Position 1 (Low Bar)", new AutoPosition1ShootGrp());
	autoLocationOptions->AddObject("Position 2", new AutoPosition2ShootGrp());
	autoLocationOptions->AddObject("Position 2 SHOOT LEFT", new AutoPosition2ShootLeftGrp());
	autoLocationOptions->AddObject("Position 3", new AutoPosition3ShootGrp());
	autoLocationOptions->AddObject("Position 3 SHOOT RIGHT", new AutoPosition3ShootRightGrp());
	autoLocationOptions->AddObject("Position 4", new AutoPosition4ShootGrp());
	autoLocationOptions->AddObject("Position 5", new AutoPosition5ShootGrp());

	//chooser->AddObject("My Auto", new MyAutoCommand());
	SmartDashboard::PutData("Auto Modes", autoDefenceOptions);
	SmartDashboard::PutData("Auto Modes 2", autoLocationOptions);
}

void Robot::SetSmartDashboardDriverContent()
{
	SmartDashboard::PutData("Zero Turret Rotate", new ZeroRotateEncoderCmd());
}

// This is run during Teleop periodic to update the Smart Dashboard values
void Robot::UpdateSmartDashboard()
{
	SmartDashboard::PutNumber("Tilt raw encoder", CommandBase::rHoodSub->GetRawTiltEnc());
	SmartDashboard::PutNumber("IntakeHeight raw encoder", CommandBase::rIntakeSub->GetRawHeight());
	SmartDashboard::PutNumber("LeftDrive raw encoder", CommandBase::rDrivetrainSub->GetRawLeftEnc());
	SmartDashboard::PutNumber("RightDrive raw encoder", CommandBase::rDrivetrainSub->GetRawRightEnc());
	SmartDashboard::PutNumber("Turret rotate raw encoder", CommandBase::rShooterSub->GetRawRotateEnc());
	SmartDashboard::PutBoolean("Intake limit switch", CommandBase::rHopperSub->IsBallIn());

	// For the navX-mxp IMU (accelerometer, gyro, compass, aka Attitude Heading Reference System)
	AHRS* ahrs = CommandBase::rDrivetrainSub->GetAHRS();
	if(ahrs)
	{
		double zeroYaw = 0.0;

		SmartDashboard::PutBoolean("IMU Connected", ahrs->IsConnected());
		SmartDashboard::PutNumber("IMU Yaw", ahrs->GetYaw());
		SmartDashboard::PutNumber("IMU Pitch", ahrs->GetPitch());
		SmartDashboard::PutNumber("IMU Roll", ahrs->GetRoll());
		SmartDashboard::GetNumber("IMU Zero Yaw", zeroYaw);
		// TODO:  Add "ahrs->ZeroYaw()" if useful
	}
}

START_ROBOT_CLASS(Robot)

