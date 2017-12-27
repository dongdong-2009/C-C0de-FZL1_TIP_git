''Option Explicit


''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'''    pop up a dialogue windows to ask the dabase_connection_string  ''''
Dim WSHShell               ' declare the object variable
Dim Title                  'the title to be displayed on the dialogue window.
dim welcomstirng           'the string that welcomes the user.
Dim Message
welcomestring = "Please input the database connection string here."   
title = "Database connection string modifier"
Message = "Please specify the database connection string, e.g." & vblfcr  _
          & vblfcr & vblfcr   &     "           TRANSACT,IT62KS,IT62KS      "
Set WSHShell = WScript.CreateObject("WScript.Shell")

'the transact,userid,password pattern that will be repaced.
textTobeRelaced = "TRANSACT,IT62KS,IT62KS"


'textTobeRelaced = "a,b,c"
'get the string inputed by the user.
textTobeRelacedBy = InputBox(Message,Title,welcomestring)   



'if the inputed string is empty, the simply quit
if (textTobeRelacedBy  = "" ) then
	WScript.Quit()
end if
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

'calling the ReplaceAll() Sub here.
ReplaceAll



Sub ReplaceAll()

Const MyFolder = "."    'set the working directory to be current directory
Dim objFS, file,folder
Dim Folders 
Folders = array ("C:\transActive\config\database","C:\transActive","C:\transActive\bin","C:\WINNT\system32" )

'create FileSystemObject
Set objFS = CreateObject("Scripting.FileSystemObject")

'for each .csv file in the currect directoty, performing a text replacement
'This is accomplished by a for loop.

'for each file the folder.

For Each folder in Folders
	For Each file In objFS.GetFolder(folder).Files

		'if it ends with .csv, then perfoming the replacement.
		If LCase(objFS.GetExtensionName(file.Path)) = "csv" Then

		'calling the function doing the replacement work.
		reading(file.Path)

		End If
	Next
next
End sub
''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''


function reading(byval name)

	dim tmpname

	'the name of the temp file
	tmpname= name + ".tmp"

	
	Set objFSO = CreateObject("Scripting.FileSystemObject")

	'open the .csv file
	Set objFile = objFSO.OpenTextFile(name, 1)
	'open a temp file
	set tempfile = objFSO.CreateTextFile(tmpname)


	'each iteration of the loop, read in a line, performing the replacement,
	'and put the result into the temp file.
	Do Until objFile.AtEndOfStream

		'read in a line
	    StringToSearch = objFile.Readline()

		'use regular expression to search the patttern which intended to be replaced.
		Set RegularExpressionObject = New RegExp
		With RegularExpressionObject
			.Pattern = textTobeRelaced
			.IgnoreCase = True
			.Global = True
		End With

		
'		expressionmatch = RegularExpressionObject.Test(StringToSearch)

'		If expressionmatch Then
			tosearch = RegularExpressionObject.Replace(StringToSearch,textTobeRelacedBy)
			tempfile.writeline(tosearch)

'		End If
	Loop

	objFile.close                            'close the current .csv file
	tempfile.close                           'close the temp file
	objFSO.DeleteFile(name)                  'delete the current .csv file
	objFSO.moveFile tmpname, name            'rename the temp file to be the .csv file

end function

WScript.Quit()
