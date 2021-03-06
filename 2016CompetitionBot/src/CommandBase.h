#ifndef COMMAND_BASE_H
#define COMMAND_BASE_H

#include <string>
#include "Commands/Command.h"
#include "OI.h"
#include "WPILib.h"
#include "Subsystems/DrivetrainSub.h"
#include "Subsystems/IntakeSub.h"
#include "Subsystems/HoodSub.h"
#include "Subsystems/ShooterSub.h"
#include "Subsystems/CameraSub.h"
#include "Subsystems/HopperSub.h"
#include "Subsystems/ClimberSub.h"
#include "Commands/Scheduler.h"

/**
 * The base for all commands. All atomic commands should subclass CommandBase.
 * CommandBase stores creates and stores each control system. To access a
 * subsystem elsewhere in your code in your code use CommandBase.examplesubsystem
 */
class CommandBase: public Command
{
public:
	CommandBase(const std::string &name);
	CommandBase();
	static void init();
	static float GetGripValue(std::string gripValue);
	static float GetTargetDistance();
	// Create a single static instance of all of your subsystems
	static DrivetrainSub* rDrivetrainSub;
	static IntakeSub* rIntakeSub;
	static HoodSub* rHoodSub;
	static ShooterSub* rShooterSub;
	static CameraSub* rCameraSub;
	static HopperSub* rHopperSub;
	static ClimberSub* rClimberSub;
	// OI MUST BE AT BOTTOM
	static OI* oi;
};

#endif
