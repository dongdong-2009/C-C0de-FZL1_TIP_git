// Validate command line arguments
var argc = WScript.Arguments.Count();
if (argc < 1
	|| (WScript.Arguments(0).toLowerCase() != "debug"
	&& WScript.Arguments(0).toLowerCase() != "release"
	&& WScript.Arguments(0).toLowerCase() != "all")
	|| argc > 2
	|| (argc == 2 && WScript.Arguments(1).toLowerCase() != "clean"))
{
	WScript.Echo( "Usage: Build_Everything Debug | Release | All [Clean]\n" );
	WScript.Quit(1);
}

// Determine which scripting engine is being used...
// We want to use the command line one (cscript) because the windows one (wscript)
// echoes to message boxes instead of the command prompt.
if (WScript.FullName.toLowerCase().search(/wscript/) > -1)
{
	// wscript is being used so shell out to cscript instead
	var WshShell = WScript.CreateObject("WScript.Shell");
	// build the command, appending all the original arguments
	var cmd	= "cscript.exe " + WScript.ScriptFullName;
	for (var i = 0; i < WScript.Arguments.Count(); i++)
	{
		cmd += " ";
		cmd += WScript.Arguments(i);
	}
	WshShell.Run( cmd );
	WScript.Quit(0);
}
// else cscript is already being used so simply carry on

var bCleanBuild = false;
if (argc == 2 /* && WScript.Arguments(1).toLowerCase() == "clean" */)
{
	bCleanBuild = true;
}

// Debug build
if (WScript.Arguments(0).toLowerCase() == "debug" || WScript.Arguments(0).toLowerCase() == "all")
{
	var outPath = "..\\..\\..\\build\\win32_nd\\"; // narrow debug
	invokeMSDEV("tools.build.Build_Base_Cots.dsw", "tools.build.cots.Build_Base_Cots - Win32 Debug", outPath, bCleanBuild);
	invokeMSDEV("tools.build.Build_Base_Cots.dsw", "ACE DLL - Win32 MFC Debug", outPath, bCleanBuild);
	invokeMSDEV("tools.build.Build_Base_Core.dsw", "tools.build.core_library.TA_Base_Core - Win32 Debug", outPath, bCleanBuild);
	invokeMSDEV("tools.build.Build_Base_Bus.dsw", "tools.build.bus_library.TA_Base_Bus - Win32 Debug", outPath, bCleanBuild);
	invokeMSDEV("tools.build.Build_Base_App.dsw", "tools.build.apps.Build_Base_Apps - Win32 Debug", outPath, bCleanBuild);
}

// Release build
if (WScript.Arguments(0).toLowerCase() == "release" || WScript.Arguments(0).toLowerCase() == "all")
{
	var outPath = "..\\..\\..\\build\\win32_nr\\"; // narrow release
	invokeMSDEV("tools.build.Build_Base_Cots.dsw", "tools.build.cots.Build_Base_Cots - Win32 Release", outPath, bCleanBuild);
	invokeMSDEV("tools.build.Build_Base_Cots.dsw", "ACE DLL - Win32 MFC Release", outPath, bCleanBuild);
	invokeMSDEV("tools.build.Build_Base_Core.dsw", "tools.build.core_library.TA_Base_Core - Win32 Release", outPath, bCleanBuild);
	invokeMSDEV("tools.build.Build_Base_Bus.dsw", "tools.build.bus_library.TA_Base_Bus - Win32 Release", outPath, bCleanBuild);
	invokeMSDEV("tools.build.Build_Base_App.dsw", "tools.build.apps.Build_Base_Apps - Win32 Release", outPath, bCleanBuild);
}

function invokeMSDEV(workspace, project, outdir, bClean)
{
	
	var WshShell = WScript.CreateObject("WScript.Shell");
	var msdevCmd = "msdev " + workspace + " /MAKE \"" + project + "\" /OUT \"" + outdir + project
				+ ".log\"";
    var sClean = "No";
    if (bClean)
    {
        msdevCmd += " /REBUILD";
        sClean = "Yes";
    }

    WScript.Echo( "Building...\n Workspace: " + workspace + "\n Project: " + project + "\n Output Log: \"" + 
        outdir + project + ".log\""+ "\n Clean? " + sClean);

	WshShell.Run(msdevCmd, 0 /* hide window */, true /* wait for completion */);
	WScript.Echo( " Done.\n");
}