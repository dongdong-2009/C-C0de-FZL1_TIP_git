#pragma once

const UINT UM_PARAM_REFRESH	=		WM_USER + 100; // Refresh parameter view. WPARAM not used, LPARAM is the message id
const UINT UM_REFRESH_GRID_MSG =	WM_USER + 101; // Refresh a message on the grid. WPARAM not used, LPARAM is the message ID
const UINT UM_NEW_GRID_COLUMN =		WM_USER + 102; // Add new column has been added to the grid. WPARAM & LPARAM not used
const UINT UM_REFRESH_GRID =		WM_USER + 103; // Refresh the entire grid. WPARAM & LPARAM not used.
const UINT UM_EMPTY_MSG_GRID =		WM_USER + 104; // Delete all items in the grid. WPARAM & LPARAM not used.
const UINT UM_VALIDATE_MSG_GRID =	WM_USER + 105; // Perform validation. WPARAM is scroll to error. LPARAM is not used.
const UINT UM_EDIT_CELL =			WM_USER + 106; // Edit a cell. WPARAM is the row. LPARAM is the column.
const UINT UM_REFRESH_ERROR_DESC =	WM_USER + 107; // Display the error description. WPARAM is the row index, LPARAM is the message id
const UINT UM_GOTO_HELP_TOPIC =		WM_USER + 108; // Display the help topic. WPARAM is the topic id, LPARAM is any additional info